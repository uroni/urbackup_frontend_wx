/*************************************************************************
*    UrBackup - Client/Server backup system
*    Copyright (C) 2011  Martin Raiber
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include "Settings.h"
#include "stringtools.h"

std::string getServerName(void)
{
#ifdef _WIN32
	char hostname[MAX_PATH];
    int rc=gethostname(hostname, MAX_PATH);

	if(rc!=SOCKET_ERROR)
		return hostname;
	else
		return "_error_";
#else
	char hostname[300];
	_i32 rc=gethostname(hostname,300);
	
	if( rc!=-1 )
		return hostname;
	else
		return "_error_";
#endif
}

std::string ConvertToUTF8(const std::wstring &input);

bool getSettingsValue(std::wstring key, std::wstring *ret, CFileSettingsReader *settings)
{
	if(!settings->getValue(key, ret) )
	{
		if(!settings->getValue(key+L"_def", ret) )
		{
			return false;
		}
	}

	return true;
}

wxTextValidator getPathValidator(void)
{	
	wxTextValidator val=wxTextValidator(wxFILTER_EXCLUDE_LIST);
	wxArrayString il;
	il.Add("/");
	il.Add(" ");
	il.Add("\\");
	il.Add(":");
	il.Add("?");
	il.Add("*");
	il.Add("\"");
	il.Add("<");
	il.Add(">");
	il.Add("|");
	val.SetExcludes(il);
	return val;
}

Settings::Settings(wxWindow* parent) : GUISettings(parent)
{
	SetIcon(wxIcon(wxT("backup-ok-big.ico"), wxBITMAP_TYPE_ICO));
	settings=new CFileSettingsReader("urbackup/data/settings.cfg");

	std::wstring t;
	if(getSettingsValue(L"update_freq_incr", &t, settings))
	{
		m_textCtrl1->SetValue(wxString(convert(watoi(t)/60/60).c_str()));
	}
	else
	{
		m_textCtrl1->SetValue(wxT("1"));
	}
	if(getSettingsValue(L"update_freq_full", &t, settings))
	{
		m_textCtrl2->SetValue(wxString(convert(watoi(t)/24/60/60).c_str()));
	}
	else
	{
		m_textCtrl1->SetValue(wxT("30"));
	}
	if(getSettingsValue(L"update_freq_image_full", &t, settings))
	{
		if(watoi(t)>0)
		{
			m_textCtrl22->SetValue(wxString(convert(watoi(t)/24/60/60).c_str()));
			m_checkBox1->SetValue(true);
		}
		else
		{
			if(settings->getValue(L"update_freq_image_full_orig", &t))
			{
				m_textCtrl22->SetValue(wxString(convert(watoi(t)/24/60/60).c_str()));
			}
			m_textCtrl21->Enable(false);
			m_textCtrl22->Enable(false);
			m_checkBox1->SetValue(false);
		}
	}
	else
	{
		m_textCtrl21->SetValue(wxT("60"));
		m_checkBox1->SetValue(true);
	}
	if(getSettingsValue(L"update_freq_image_incr", &t, settings))
	{
		m_textCtrl21->SetValue(wxString(convert(watoi(t)/24/60/60).c_str()));
	}
	else
	{
		m_textCtrl21->SetValue(wxT("7"));
	}
	if(getSettingsValue(L"max_file_incr", &t, settings))
	{
		m_textCtrl131->SetValue(wxString(convert(watoi(t)).c_str()));
	}
	else
	{
		m_textCtrl131->SetValue(wxT("100"));
	}
	if(getSettingsValue(L"min_file_incr", &t, settings))
	{
		m_textCtrl13->SetValue(wxString(convert(watoi(t)).c_str()));
	}
	else
	{
		m_textCtrl13->SetValue(wxT("40"));
	}
	if(getSettingsValue(L"max_file_full", &t, settings))
	{
		m_textCtrl133->SetValue(wxString(convert(watoi(t)).c_str()));
	}
	else
	{
		m_textCtrl133->SetValue(wxT("10"));
	}
	if(getSettingsValue(L"min_file_full", &t,settings))
	{
		m_textCtrl132->SetValue(wxString(convert(watoi(t)).c_str()));
	}
	else
	{
		m_textCtrl132->SetValue(wxT("2"));
	}
	if(getSettingsValue(L"min_image_incr", &t,settings))
	{
		m_textCtrl134->SetValue(wxString(convert(watoi(t)).c_str()));
	}
	else
	{
		m_textCtrl134->SetValue(wxT("4"));
	}
	if(getSettingsValue(L"max_image_incr", &t, settings))
	{
		m_textCtrl135->SetValue(wxString(convert(watoi(t)).c_str()));
	}
	else
	{
		m_textCtrl135->SetValue(wxT("30"));
	}
	if(getSettingsValue(L"min_image_full", &t, settings))
	{
		m_textCtrl136->SetValue(wxString(convert(watoi(t)).c_str()));
	}
	else
	{
		m_textCtrl136->SetValue(wxT("2"));
	}
	if(getSettingsValue(L"max_image_full", &t, settings))
	{
		m_textCtrl137->SetValue(wxString(convert(watoi(t)).c_str()));
	}
	else
	{
		m_textCtrl137->SetValue(wxT("5"));
	}
	if(settings->getValue(L"computername", &t) )
	{
		m_textCtrl15->SetValue(t);
	}
	else
	{
		m_textCtrl15->SetValue(getServerName());
	}
	if(getSettingsValue(L"backup_window", &t, settings))
	{
		m_textCtrl17->SetValue(t);
	}
	else
	{
		m_textCtrl17->SetValue("1-7/0-24");
	}
	if(getSettingsValue(L"exclude_files", &t, settings))
	{
		m_textCtrl16->SetValue(t);
	}
	else
	{
		m_textCtrl16->SetValue("");
	}
	if(getSettingsValue(L"startup_backup_delay", &t, settings))
	{
		m_textCtrl19->SetValue(convert(watoi(t)/60));
	}
	else
	{
		m_textCtrl19->SetValue("0");
	}

	m_textCtrl15->SetValidator(getPathValidator());
	Show(true);
}

Settings::~Settings(void)
{
	delete settings;
}

void Settings::OnOkClick( wxCommandEvent& event )
{
	wxString update_freq_incr=m_textCtrl1->GetValue();
	wxString update_freq_full=m_textCtrl2->GetValue();
	wxString update_freq_image_full=m_textCtrl22->GetValue();
	wxString update_freq_image_incr=m_textCtrl21->GetValue();
	wxString max_file_incr=m_textCtrl131->GetValue();
	wxString min_file_incr=m_textCtrl13->GetValue();
	wxString max_file_full=m_textCtrl133->GetValue();
	wxString min_file_full=m_textCtrl132->GetValue();
	wxString min_image_incr=m_textCtrl134->GetValue();
	wxString max_image_incr=m_textCtrl135->GetValue();
	wxString min_image_full=m_textCtrl136->GetValue();
	wxString max_image_full=m_textCtrl137->GetValue();
	wxString computername=m_textCtrl15->GetValue();
	wxString backup_window=m_textCtrl17->GetValue();
	wxString exclude_files=m_textCtrl16->GetValue();
	wxString startup_backup_delay=m_textCtrl19->GetValue();

	long l_update_freq_incr,l_update_freq_full;
	long l_update_freq_image_full, l_update_freq_image_full_orig, l_update_freq_image_incr;
	long l_max_file_incr, l_min_file_incr;
	long l_max_file_full, l_min_file_full;
	long l_min_image_incr, l_max_image_incr;
	long l_min_image_full, l_max_image_full;
	long l_startup_backup_delay;

	if(update_freq_incr.ToLong(&l_update_freq_incr)==false )
	{
		wxMessageBox( _("Die inkrementelle Backupzeit ist keine Zahl"), wxT("UrBackup"), wxICON_ERROR);
		m_textCtrl1->SetFocus();
		return;
	}
	if(update_freq_full.ToLong(&l_update_freq_full)==false )
	{
		wxMessageBox( _("Die volle Backupzeit ist keine Zahl"), wxT("UrBackup"), wxICON_ERROR);
		m_textCtrl2->SetFocus();
		return;
	}
	if(update_freq_image_full.ToLong(&l_update_freq_image_full)==false )
	{
		wxMessageBox( _("Die volle Imagebackupzeit ist keine Zahl"), wxT("UrBackup"), wxICON_ERROR);
		m_textCtrl22->SetFocus();
		return;
	}
	if(update_freq_image_incr.ToLong(&l_update_freq_image_incr)==false )
	{
		wxMessageBox( _("Die inkrementelle Imagebackupzeit ist keine Zahl"), wxT("UrBackup"), wxICON_ERROR);
		m_textCtrl21->SetFocus();
		return;
	}
	if(max_file_incr.ToLong(&l_max_file_incr)==false )
	{
		wxMessageBox( _("Die maximale Anzahl an inkrementellen Backups ist keine Zahl"), wxT("UrBackup"), wxICON_ERROR);
		m_textCtrl131->SetFocus();
		return;
	}
	if(min_file_incr.ToLong(&l_min_file_incr)==false )
	{
		wxMessageBox( _("Die minimale Anzahl an inkrementellen Backups ist keine Zahl"), wxT("UrBackup"), wxICON_ERROR);
		m_textCtrl13->SetFocus();
		return;
	}
	if(max_file_full.ToLong(&l_max_file_full)==false )
	{
		wxMessageBox( _("Die maximale Anzahl an vollen Backups ist keine Zahl"), wxT("UrBackup"), wxICON_ERROR);
		m_textCtrl133->SetFocus();
		return;
	}
	if(min_file_full.ToLong(&l_min_file_full)==false )
	{
		wxMessageBox( _("Die minimale Anzahl an vollen Backups ist keine Zahl"), wxT("UrBackup"), wxICON_ERROR);
		m_textCtrl132->SetFocus();
		return;
	}
	if(min_image_incr.ToLong(&l_min_image_incr)==false )
	{
		wxMessageBox( _("Die minimale Anzahl an inkrementellen Image-Backups ist keine Zahl"), wxT("UrBackup"), wxICON_ERROR);
		m_textCtrl134->SetFocus();
		return;
	}
	if(max_image_incr.ToLong(&l_max_image_incr)==false )
	{
		wxMessageBox( _("Die maximale Anzahl an inkrementellen Image-Backups ist keine Zahl"), wxT("UrBackup"), wxICON_ERROR);
		m_textCtrl135->SetFocus();
		return;
	}
	if(min_image_full.ToLong(&l_min_image_full)==false )
	{
		wxMessageBox( _("Die minimale Anzahl an vollen Image-Backups ist keine Zahl"), wxT("UrBackup"), wxICON_ERROR);
		m_textCtrl136->SetFocus();
		return;
	}
	if(max_image_full.ToLong(&l_max_image_full)==false )
	{
		wxMessageBox( _("Die maximale Anzahl an vollen Image-Backups ist keine Zahl"), wxT("UrBackup"), wxICON_ERROR);
		m_textCtrl137->SetFocus();
		return;
	}
	if(startup_backup_delay.ToLong(&l_startup_backup_delay)==false)
	{
		wxMessageBox( _("Die Verzögerung bei Systemstart ist keine Zahl"), wxT("UrBackup"), wxICON_ERROR);
		m_textCtrl19->SetFocus();
		return;
	}


	l_update_freq_image_full_orig=l_update_freq_image_full;
	if(m_checkBox1->GetValue()==false)
	{
		l_update_freq_image_full=-1;
	}

	std::map<std::string, std::string> n_vals;
	n_vals["update_freq_incr"]=nconvert(l_update_freq_incr*60*60);
	n_vals["update_freq_full"]=nconvert(l_update_freq_full*24*60*60);
	n_vals["update_freq_image_full"]=nconvert(l_update_freq_image_full*24*60*60);
	n_vals["update_freq_image_full_orig"]=nconvert(l_update_freq_image_full_orig*24*60*60);
	n_vals["update_freq_image_incr"]=nconvert(l_update_freq_image_incr*24*60*60);
	n_vals["max_file_incr"]=nconvert(l_max_file_incr);
	n_vals["min_file_incr"]=nconvert(l_min_file_incr);
	n_vals["max_file_full"]=nconvert(l_max_file_full);
	n_vals["min_file_full"]=nconvert(l_min_file_full);
	n_vals["min_image_incr"]=nconvert(l_min_image_incr);
	n_vals["max_image_incr"]=nconvert(l_max_image_incr);
	n_vals["min_image_full"]=nconvert(l_min_image_full);
	n_vals["max_image_full"]=nconvert(l_max_image_full);
	n_vals["computername"]=computername.ToUTF8();
	n_vals["backup_window"]=backup_window.ToUTF8();
	n_vals["exclude_files"]=exclude_files.ToUTF8();
	n_vals["startup_backup_delay"]=nconvert(l_startup_backup_delay*60);

	std::string ndata;
	std::vector<std::wstring> keys=settings->getKeys();
	for(size_t i=0;i<keys.size();++i)
	{
		std::string key=ConvertToUTF8(keys[i]);
		ndata+=key+"=";
		std::map<std::string, std::string>::iterator iter=n_vals.find(key);
		if(iter!=n_vals.end())
		{
			ndata+=iter->second;
		}
		else
		{
			std::wstring val;
			if(settings->getValue(keys[i], &val) )
				ndata+=ConvertToUTF8(val);
		}
		ndata+="\n";
	}

	for(std::map<std::string, std::string>::iterator it=n_vals.begin();it!=n_vals.end();++it)
	{
		bool found=false;
		const std::string &nkey=it->first;
		for(size_t i=0;i<keys.size();++i)
		{
			if(ConvertToUTF8(keys[i])==nkey)
			{
				found=true;
				break;
			}
		}

		if(!found)
		{
			ndata+=nkey+"="+it->second+"\n";
		}
	}

	Connector::updateSettings(ndata);

	Close();
}

void Settings::OnAbortClick( wxCommandEvent& event )
{
	Close();
}

void Settings::OnDisableImageBackups( wxCommandEvent& event )
{
	if(m_checkBox1->GetValue()==false)
	{
		m_textCtrl21->Enable(false);
		m_textCtrl22->Enable(false);
	}
	else
	{
		m_textCtrl21->Enable(true);
		m_textCtrl22->Enable(true);
	}		
}
