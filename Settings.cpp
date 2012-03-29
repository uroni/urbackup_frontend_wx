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
#include "main.h"
#include "capa_bits.h"

#undef _
#define _(s) wxGetTranslation(wxT(s))

extern MyTimer *timer;
extern wxString res_path;
extern wxString ico_ext;
extern wxBitmapType ico_type;

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
	int rc=gethostname(hostname,300);
	
	if( rc!=-1 )
		return hostname;
	else
		return "_error_";
#endif
}

std::string ConvertToUTF8(const std::wstring &input);
std::wstring ConvertToUnicode(const std::string &str);

#ifdef wxUSE_WCHAR_T
#define ConvertToWX(x) ConvertToUnicode(x)
#else
#define ConvertToWX(x)
#endif

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
	il.Add(wxT("/"));
	il.Add(wxT(" "));
	il.Add(wxT("\\"));
	il.Add(wxT(":"));
	il.Add(wxT("?"));
	il.Add(wxT("*"));
	il.Add(wxT("\""));
	il.Add(wxT("<"));
	il.Add(wxT(">"));
	il.Add(wxT("|"));
	val.SetExcludes(il);
	return val;
}

Settings::Settings(wxWindow* parent) : GUISettings(parent)
{
	SetIcon(wxIcon(res_path+wxT("backup-ok-big.")+ico_ext, ico_type));
	settings=new CFileSettingsReader("urbackup/data/settings.cfg");

	std::wstring t;
	if(getSettingsValue(L"update_freq_incr", &t, settings))
	{
		m_textCtrl1->SetValue(wxString(convert(watoi(t)/60/60).c_str()));
	}
	else
	{
		m_textCtrl1->SetValue(wxT("12"));
	}
	if(getSettingsValue(L"update_freq_full", &t, settings))
	{
		m_textCtrl2->SetValue(wxString(convert(watoi(t)/24/60/60).c_str()));
	}
	else
	{
		m_textCtrl2->SetValue(wxT("30"));
	}
#ifdef _WIN32
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
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
			m_textCtrl22->SetValue(wxT("60"));
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
	}
#endif
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
#ifdef _WIN32
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
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
	}
#endif
	if(settings->getValue(L"computername", &t) )
	{
		m_textCtrl15->SetValue(t);
	}
	else
	{
		m_textCtrl15->SetValue(ConvertToWX(getServerName()));
	}
	if(getSettingsValue(L"backup_window", &t, settings))
	{
		m_textCtrl17->SetValue(t);
	}
	else
	{
		m_textCtrl17->SetValue(wxT("1-7/0-24"));
	}
	if(getSettingsValue(L"exclude_files", &t, settings))
	{
		m_textCtrl16->SetValue(t);
	}
	else
	{
		m_textCtrl16->SetValue(wxT(""));
	}
	if(getSettingsValue(L"include_files", &t, settings))
	{
		m_textCtrl161->SetValue(t);
	}
	else
	{
		m_textCtrl161->SetValue(wxT(""));
	}
	if(getSettingsValue(L"startup_backup_delay", &t, settings))
	{
		m_textCtrl19->SetValue(convert(watoi(t)/60));
	}
	else
	{
		m_textCtrl19->SetValue(wxT("0"));
	}
#ifdef _WIN32
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		if(getSettingsValue(L"image_letters", &t, settings))
		{
			m_textCtrl23->SetValue(t);
		}
		else
		{
			m_textCtrl23->SetValue(wxT("C"));
		}
	}
#endif

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
#ifdef _WIN32
	wxString update_freq_image_full;
	wxString update_freq_image_incr;
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		update_freq_image_full=m_textCtrl22->GetValue();
		update_freq_image_incr=m_textCtrl21->GetValue();
	}
#endif
	wxString max_file_incr=m_textCtrl131->GetValue();
	wxString min_file_incr=m_textCtrl13->GetValue();
	wxString max_file_full=m_textCtrl133->GetValue();
	wxString min_file_full=m_textCtrl132->GetValue();
#ifdef _WIN32
	wxString min_image_incr;
	wxString max_image_incr;
	wxString min_image_full;
	wxString max_image_full;
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		min_image_incr=m_textCtrl134->GetValue();
		max_image_incr=m_textCtrl135->GetValue();
		min_image_full=m_textCtrl136->GetValue();
		max_image_full=m_textCtrl137->GetValue();
	}
#endif
	wxString computername=m_textCtrl15->GetValue();
	wxString backup_window=m_textCtrl17->GetValue();
	wxString exclude_files=m_textCtrl16->GetValue();
	wxString include_files=m_textCtrl161->GetValue();
	wxString startup_backup_delay=m_textCtrl19->GetValue();
#ifdef _WIN32
	wxString image_letters;
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		image_letters=m_textCtrl23->GetValue();
	}
#endif

	long l_update_freq_incr,l_update_freq_full;
	long l_update_freq_image_full, l_update_freq_image_full_orig, l_update_freq_image_incr;
	long l_max_file_incr, l_min_file_incr;
	long l_max_file_full, l_min_file_full;
	long l_min_image_incr, l_max_image_incr;
	long l_min_image_full, l_max_image_full;
	long l_startup_backup_delay;

	if(update_freq_incr.ToLong(&l_update_freq_incr)==false )
	{
		wxMessageBox( _("Die inkrementelle Backupzeit ist keine Zahl"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl1->SetFocus();
		return;
	}
	if(update_freq_full.ToLong(&l_update_freq_full)==false )
	{
		wxMessageBox( _("Die volle Backupzeit ist keine Zahl"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl2->SetFocus();
		return;
	}
#ifdef _WIN32
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		if(update_freq_image_full.ToLong(&l_update_freq_image_full)==false && m_checkBox1->GetValue() )
		{
			wxMessageBox( _("Die volle Imagebackupzeit ist keine Zahl"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl22->SetFocus();
			return;
		}
		if(update_freq_image_incr.ToLong(&l_update_freq_image_incr)==false && m_checkBox1->GetValue()  )
		{
			wxMessageBox( _("Die inkrementelle Imagebackupzeit ist keine Zahl"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl21->SetFocus();
			return;
		}
	}
#endif
	if(max_file_incr.ToLong(&l_max_file_incr)==false )
	{
		wxMessageBox( _("Die maximale Anzahl an inkrementellen Backups ist keine Zahl"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl131->SetFocus();
		return;
	}
	if(min_file_incr.ToLong(&l_min_file_incr)==false )
	{
		wxMessageBox( _("Die minimale Anzahl an inkrementellen Backups ist keine Zahl"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl13->SetFocus();
		return;
	}
	if(max_file_full.ToLong(&l_max_file_full)==false )
	{
		wxMessageBox( _("Die maximale Anzahl an vollen Backups ist keine Zahl"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl133->SetFocus();
		return;
	}
	if(min_file_full.ToLong(&l_min_file_full)==false )
	{
		wxMessageBox( _("Die minimale Anzahl an vollen Backups ist keine Zahl"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl132->SetFocus();
		return;
	}
#ifdef _WIN32
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		if(min_image_incr.ToLong(&l_min_image_incr)==false )
		{
			wxMessageBox( _("Die minimale Anzahl an inkrementellen Image-Backups ist keine Zahl"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl134->SetFocus();
			return;
		}
		if(max_image_incr.ToLong(&l_max_image_incr)==false )
		{
			wxMessageBox( _("Die maximale Anzahl an inkrementellen Image-Backups ist keine Zahl"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl135->SetFocus();
			return;
		}
		if(min_image_full.ToLong(&l_min_image_full)==false )
		{
			wxMessageBox( _("Die minimale Anzahl an vollen Image-Backups ist keine Zahl"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl136->SetFocus();
			return;
		}
		if(max_image_full.ToLong(&l_max_image_full)==false )
		{
			wxMessageBox( _("Die maximale Anzahl an vollen Image-Backups ist keine Zahl"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl137->SetFocus();
			return;
		}
	}
#endif
	if(startup_backup_delay.ToLong(&l_startup_backup_delay)==false)
	{
		wxMessageBox( _("Die Verzögerung bei Systemstart ist keine Zahl"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl19->SetFocus();
		return;
	}
#ifdef _WIN32
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		std::string s_image_letters=image_letters.ToUTF8();
		std::vector<std::string> img_paths;
		Tokenize(s_image_letters, img_paths, ";,");

		for(size_t i=0;i<img_paths.size();++i)
		{
			char outb[1000];
			BOOL b=GetVolumePathNameA((img_paths[i]+":\\").c_str(), outb, 1000);
			if(b==FALSE)
			{
				wxMessageBox( ConvertToUnicode(img_paths[i])+_(" ist kein Laufwerk"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
				m_textCtrl23->SetFocus();
				return;
			}
		}
	}
#endif

	l_update_freq_image_full_orig=l_update_freq_image_full;
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		if(m_checkBox1->GetValue()==false)
		{
			l_update_freq_image_full=-1;
		}
	}

	std::map<std::string, std::string> n_vals;
	n_vals["update_freq_incr"]=nconvert(l_update_freq_incr*60*60);
	n_vals["update_freq_full"]=nconvert(l_update_freq_full*24*60*60);
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		n_vals["update_freq_image_full"]=nconvert(l_update_freq_image_full*24*60*60);
		n_vals["update_freq_image_full_orig"]=nconvert(l_update_freq_image_full_orig*24*60*60);
		n_vals["update_freq_image_incr"]=nconvert(l_update_freq_image_incr*24*60*60);
	}
	n_vals["max_file_incr"]=nconvert(l_max_file_incr);
	n_vals["min_file_incr"]=nconvert(l_min_file_incr);
	n_vals["max_file_full"]=nconvert(l_max_file_full);
	n_vals["min_file_full"]=nconvert(l_min_file_full);
#ifdef _WIN32
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		n_vals["min_image_incr"]=nconvert(l_min_image_incr);
		n_vals["max_image_incr"]=nconvert(l_max_image_incr);
		n_vals["min_image_full"]=nconvert(l_min_image_full);
		n_vals["max_image_full"]=nconvert(l_max_image_full);
	}
#endif
	n_vals["computername"]=computername.ToUTF8();
	n_vals["backup_window"]=backup_window.ToUTF8();
	n_vals["exclude_files"]=exclude_files.ToUTF8();
	n_vals["include_files"]=include_files.ToUTF8();
	n_vals["startup_backup_delay"]=nconvert(l_startup_backup_delay*60);
#ifdef _WIN32
	if(!timer->hasCapability(DONT_DO_IMAGE_BACKUPS))
	{
		n_vals["image_letters"]=image_letters.ToUTF8();
	}
#endif

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
