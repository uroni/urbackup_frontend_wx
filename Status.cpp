/*************************************************************************
*    UrBackup - Client/Server backup system
*    Copyright (C) 2011-2015 Martin Raiber
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Affero General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include "Status.h"
#include "main.h"
#include "TranslationHelper.h"
#include "stringtools.h"

extern wxString res_path;
extern wxString ico_ext;
extern wxBitmapType ico_type;

const int reset_error_count=60;

namespace
{
	wxString getInternetConnectionStatus(wxString str, unsigned int last_seen)
	{
		if(str==wxT("initializing"))
		{
			return _("Initializing.");
		}
		else if(str==wxT("wait_local"))
		{
			return _("Waiting for local UrBackup server.");
		}
		else if(str==wxT("connected_local"))
		{
			unsigned int min_last_seen = (last_seen/1000)/60;

			wxString last_seen;
			last_seen << min_last_seen;

			return _("Connected to local UrBackup server.")+wxT("\n")+_("Not trying to connect to Internet server.")+wxT("\n")+
					trans_1(_("Local server last seen _1_ minutes ago."), last_seen);
		}
		else if(str==wxT("no_server"))
		{
			return _("No Internet server configured.");
		}
		else if(str==wxT("connected"))
		{
			return _("Connected to Internet server");
		}
		else if(str==wxT("connecting_failed"))
		{
			return _("Could not connect to Internet server. Server unreachable.");
		}
		else if(str.find(wxT("error:"))==0)
		{
			return trans_1(_("Error: _1_"), str.substr(6));
		}

		return str;
	}

	std::string PrettyPrintTime(wxLongLong_t ms)
	{
		std::string ret;

		unsigned int c_s=1000;
		unsigned int c_m=c_s*60;
		unsigned int c_h=c_m*60;
		unsigned int c_d=c_h*24;

		if( ms>c_d)
		{
			wxLongLong_t t=ms/c_d;
			if(!ret.empty()) ret+=" ";
			ret+=nconvert(t)+wxT(" ")+wxPLURAL("day", "days", static_cast<size_t>(t));
			ms-=t*c_d;
		}

		if( ms>c_h)
		{
			wxLongLong_t t=ms/c_h;
			if(!ret.empty()) ret+=" ";
			ret+=nconvert(t)+wxT(" ")+wxPLURAL("hour", "hours", static_cast<size_t>(t));
			ms-=t*c_h;
		}

		if( ms>c_m)
		{
			wxLongLong_t t=ms/c_m;
			if(!ret.empty()) ret+=" ";
			ret+=nconvert(t)+wxT(" ")+wxPLURAL("minute", "minutes", static_cast<size_t>(t));
			ms-=t*c_m;
		}

		return ret;
	}
}

Status* Status::instance = NULL;

Status::Status(wxWindow* parent)
	: GUIStatus(parent)
{
	SetIcon(wxIcon(res_path+wxT("backup-ok.")+ico_ext, ico_type));

	if(updateStatus(0))
	{
		SetFocus();
		Raise();
		Show(true);
		RequestUserAttention();

		Start(1000);
	}

#ifndef __APPLE__
	SetDoubleBuffered(true);
#endif

	error_count=reset_error_count;

	instance = this;
}


bool Status::updateStatus(int errcnt)
{
	SStatusDetails status_details = Connector::getStatusDetails();

	if(!status_details.ok)
	{
		if(errcnt-1<=0)
		{
			Stop();
			wxMessageBox(_("There was an error. Currently nothing can be backed up."), wxT("UrBackup"), wxOK|wxICON_ERROR);
			Hide();
			Close();
		}
		return false;
	}

	if(last_status_details == status_details)
	{
		return true;
	}

	last_status_details = status_details;

	wxString status_text = getStatusText(status_details.currently_running);

	if(status_text.empty())
	{
		status_text=_("Idle.");
		m_staticText312->SetLabel(wxEmptyString);
		m_gauge1->SetValue(0);
		m_gauge1->Disable();
	}
	else
	{
		m_gauge1->Enable();

		if(status_details.percent_done>=0)
		{
			m_gauge1->SetValue(status_details.percent_done);
		}
		else
		{
			m_gauge1->Pulse();
		}

		if(status_details.eta_ms>0)
		{
			m_staticText312->SetLabel(trans_1(_("ETA: Approx. _1_"), PrettyPrintTime(status_details.eta_ms)));
		}
		else
		{
			m_staticText312->SetLabel(wxEmptyString);
		}

		wxString s;
		s << status_details.percent_done;
		status_text+=wxT(" ");
		status_text+=getPercentText(s);
	}

	m_staticText31->SetLabel(status_text);
	wxLongLong_t lastbackups;
	if(!status_details.last_backup_time.empty() && status_details.last_backup_time.ToLongLong(&lastbackups))
	{
		wxDateTime lastbackup_dt((wxLongLong)(lastbackups*1000));

		m_staticText37->SetLabel(trans_1(_("Last backup on _1_"), lastbackup_dt.Format()));
	}
	else
	{
		m_staticText37->SetLabel(wxEmptyString);
	}

	wxString servers_text;

	for(size_t i=0;i<status_details.servers.size();++i)
	{
		const SUrBackupServer& server = status_details.servers[i];

		if(!servers_text.empty())
			servers_text+=wxT("\n");

		wxString internet_s;
		if(server.internet_connection)
			internet_s=_("Yes");
		else
			internet_s=_("No");

		servers_text+=server.name+" ("+trans_1(_("Internet: _1_"), internet_s) +")";
	}

	if(status_details.servers.empty())
	{
		m_staticText32->SetLabel(wxEmptyString);
	}
	else
	{
		m_staticText32->SetLabel(_("Servers:"));
	}

	m_staticText33->SetLabel(servers_text);

	m_staticText35->SetLabel(getInternetConnectionStatus(status_details.internet_status, status_details.time_since_last_lan_connection));

	Layout();

	GetSizer()->Fit(this);

	return true;
}

void Status::Notify(void)
{
	if(!IsVisible())
	{
		Stop();
		return;
	}
	
	if(!updateStatus(error_count))
	{
		--error_count;
	}
	else
	{
		error_count=reset_error_count;
	}
}

Status* Status::getInstance()
{
	return instance;
}

void Status::OnClose()
{
	instance=NULL;
}
