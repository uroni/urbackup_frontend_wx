#include "Status.h"
#include "main.h"
#include "TranslationHelper.h"

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
}


Status::Status(wxWindow* parent)
	: GUIStatus(parent)
{
	SetIcon(wxIcon(res_path+wxT("backup-ok.")+ico_ext, ico_type));

	if(updateStatus(0))
	{
		SetFocus();
		Raise();
		Show(true);

		Start(1000);
	}

	SetDoubleBuffered(true);

	error_count=reset_error_count;
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

		wxString s;
		s << status_details.percent_done;
		status_text+=getPercentText(s);
	}

	m_staticText31->SetLabel(status_text);

	if(!status_details.last_backup_time.empty())
	{
		m_staticText37->SetLabel(_("Last backup on ")+status_details.last_backup_time);
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

		servers_text+=server.name+" ("+_("Internet: ");
		if(server.internet_connection)
			servers_text+=_("Yes");
		else
			servers_text+=_("No");

		servers_text+=")";
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