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

#include "Settings.h"
#include "stringtools.h"
#include "main.h"
#include "capa_bits.h"
#include "TranslationHelper.h"

#include <wx/valtext.h>

#ifndef wxFILTER_DIGITS
#define wxFILTER_DIGITS wxFILTER_NUMERIC
#endif

#undef _
#define _(s) wxGetTranslation(wxT(s))

extern MyTimer *timer;
extern wxString res_path;
extern wxString ico_ext;
extern wxBitmapType ico_type;
extern std::string g_res_path;

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

wxTextValidator getDigitSlashValidator(void)
{	
	wxTextValidator val=wxTextValidator(wxFILTER_INCLUDE_LIST);
	wxArrayString il;
	il.Add(wxT("0"));
	il.Add(wxT("1"));
	il.Add(wxT("2"));
	il.Add(wxT("3"));
	il.Add(wxT("4"));
	il.Add(wxT("5"));
	il.Add(wxT("6"));
	il.Add(wxT("7"));
	il.Add(wxT("8"));
	il.Add(wxT("9"));
	il.Add(wxT("-"));
	val.SetIncludes(il);
	return val;
}

Settings::Settings(wxWindow* parent) : GUISettings(parent)
{
	SetIcon(wxIcon(res_path+wxT("backup-ok.")+ico_ext, ico_type));
#ifdef _DEBUG
	settings=new CFileSettingsReader("urbackup/data/settings.cfg");
#else
	settings=new CFileSettingsReader(g_res_path+"urbackup/data/settings.cfg");
#endif

	std::wstring t;
	if(getSettingsValue(L"update_freq_incr", &t, settings))
	{
		m_textCtrl1->SetValue(wxString(convert(watoi(t)/60.f/60.f).c_str()));
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
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
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
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
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
	if(getSettingsValue(L"backup_window_incr_file", &t, settings))
	{
		m_textCtrl17->SetValue(t);
	}
	else
	{
		if(getSettingsValue(L"backup_window", &t, settings))
		{
			m_textCtrl17->SetValue(t);
		}
		else
		{
			m_textCtrl17->SetValue(wxT("1-7/0-24"));
		}
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
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
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
	if(getSettingsValue(L"internet_mode_enabled", &t, settings) && t==L"true")
	{
		m_checkBoxInternetEnabled->SetValue(true);
	}
	else
	{
		m_checkBoxInternetEnabled->SetValue(false);
	}
	if(getSettingsValue(L"internet_full_file_backups", &t, settings) && t==L"true")
	{
		m_checkBoxInternetFullFile->SetValue(true);
	}
	else
	{
		m_checkBoxInternetFullFile->SetValue(false);
	}
#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		if(getSettingsValue(L"internet_image_backups", &t, settings) && t==L"true")
		{
			m_checkBoxInternetImage->SetValue(true);
		}
		else
		{
			m_checkBoxInternetImage->SetValue(false);
		}
	}
#endif
	if(getSettingsValue(L"internet_server", &t, settings))
	{
		m_textCtrlInternetServer->SetValue(t);
	}
	else
	{
		m_textCtrlInternetServer->SetValue(wxT(""));
	}
	if(getSettingsValue(L"internet_server_port", &t, settings))
	{
		m_textCtrlInternetServerPort->SetValue(t);
	}
	else
	{
		m_textCtrlInternetServerPort->SetValue(wxT("55415"));
	}
	if(getSettingsValue(L"internet_authkey", &t, settings))
	{
		m_textCtrlInternetServerAuthkey->SetValue(t);
	}
	else
	{
		m_textCtrlInternetServerAuthkey->SetValue(wxT(""));
	}
	if(getSettingsValue(L"local_speed", &t, settings))
	{
		if(watoi(t)>0) 
			m_textCtrlLocalSpeed->SetValue(convert(watoi(t)/((1024*1024)/8)) );
		else
			m_textCtrlLocalSpeed->SetValue(wxT("-"));
	}
	else
	{
		m_textCtrlLocalSpeed->SetValue(wxT("-"));
	}
	if(getSettingsValue(L"internet_speed", &t, settings))
	{
		if(watoi(t)>0) 
			m_textCtrlInternetSpeed->SetValue(convert(watoi(t)/((1024)/8)) );
		else
			m_textCtrlInternetSpeed->SetValue(wxT("-"));
	}
	else
	{
		m_textCtrlInternetSpeed->SetValue(wxT("-"));
	}
	if(getSettingsValue(L"internet_compress", &t, settings) && t==L"false")
	{
		m_checkBoxInternetCompress->SetValue(false);
	}
	else
	{
		m_checkBoxInternetCompress->SetValue(true);
	}
	if(getSettingsValue(L"internet_encrypt", &t, settings) && t==L"false")
	{
		m_checkBoxInternetEncrypt->SetValue(false);
	}
	else
	{
		m_checkBoxInternetEncrypt->SetValue(true);
	}

	m_textCtrlInternetSpeed->SetValidator(getDigitSlashValidator());
	m_textCtrlLocalSpeed->SetValidator(getDigitSlashValidator());	
	m_textCtrl1->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	m_textCtrl2->SetValidator(wxTextValidator(wxFILTER_DIGITS));
	m_textCtrl131->SetValidator(wxTextValidator(wxFILTER_DIGITS));
	m_textCtrl13->SetValidator(wxTextValidator(wxFILTER_DIGITS));
	m_textCtrl133->SetValidator(wxTextValidator(wxFILTER_DIGITS));
	m_textCtrl132->SetValidator(wxTextValidator(wxFILTER_DIGITS));
#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		m_textCtrl22->SetValidator(wxTextValidator(wxFILTER_DIGITS));
		m_textCtrl21->SetValidator(wxTextValidator(wxFILTER_DIGITS));
		m_textCtrl134->SetValidator(wxTextValidator(wxFILTER_DIGITS));
		m_textCtrl135->SetValidator(wxTextValidator(wxFILTER_DIGITS));
		m_textCtrl136->SetValidator(wxTextValidator(wxFILTER_DIGITS));
		m_textCtrl137->SetValidator(wxTextValidator(wxFILTER_DIGITS));
	}
#endif
	m_textCtrl19->SetValidator(wxTextValidator(wxFILTER_DIGITS));
	m_textCtrlInternetServerPort->SetValidator(wxTextValidator(wxFILTER_DIGITS));
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
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
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
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
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
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		image_letters=m_textCtrl23->GetValue();
	}
#endif
	bool internet_mode_enabled=m_checkBoxInternetEnabled->GetValue();
	wxString internet_server=m_textCtrlInternetServer->GetValue();
	wxString internet_server_port=m_textCtrlInternetServerPort->GetValue();
#ifdef _WIN32
	bool internet_image_backups=false;
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		internet_image_backups=m_checkBoxInternetImage->GetValue();
	}
#endif
	bool internet_full_file_backups=m_checkBoxInternetFullFile->GetValue();
	wxString internet_authkey=m_textCtrlInternetServerAuthkey->GetValue();
	wxString internet_speed=m_textCtrlInternetSpeed->GetValue();
	wxString local_speed=m_textCtrlLocalSpeed->GetValue();
	bool internet_encrypt=m_checkBoxInternetEncrypt->GetValue();
	bool internet_compress=m_checkBoxInternetCompress->GetValue();

	double l_update_freq_incr;
	long l_update_freq_full;
	long l_update_freq_image_full, l_update_freq_image_full_orig, l_update_freq_image_incr;
	long l_max_file_incr, l_min_file_incr;
	long l_max_file_full, l_min_file_full;
	long l_min_image_incr, l_max_image_incr;
	long l_min_image_full, l_max_image_full;
	long l_startup_backup_delay;
	long l_internet_server_port;
	long l_internet_speed, l_local_speed;

	if(update_freq_incr.ToCDouble(&l_update_freq_incr)==false )
	{
		wxMessageBox( _("The incremental backup interval is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl1->SetFocus();
		return;
	}
	if(update_freq_full.ToLong(&l_update_freq_full)==false )
	{
		wxMessageBox( _("The full backup interval  is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl2->SetFocus();
		return;
	}
#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		if(update_freq_image_full.ToLong(&l_update_freq_image_full)==false && m_checkBox1->GetValue() )
		{
			wxMessageBox( _("The full image backup interval  is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl22->SetFocus();
			return;
		}
		if(update_freq_image_incr.ToLong(&l_update_freq_image_incr)==false && m_checkBox1->GetValue()  )
		{
			wxMessageBox( _("The incremental image backup time is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl21->SetFocus();
			return;
		}
	}
#endif
	if(max_file_incr.ToLong(&l_max_file_incr)==false )
	{
		wxMessageBox( _("The maximal number of incremental file backups  is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl131->SetFocus();
		return;
	}
	if(min_file_incr.ToLong(&l_min_file_incr)==false )
	{
		wxMessageBox( _("The minimal number of incremental file backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl13->SetFocus();
		return;
	}
	if(max_file_full.ToLong(&l_max_file_full)==false )
	{
		wxMessageBox( _("The maximal number of full file backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl133->SetFocus();
		return;
	}
	if(min_file_full.ToLong(&l_min_file_full)==false )
	{
		wxMessageBox( _("Minimal number of full file backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl132->SetFocus();
		return;
	}
#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		if(min_image_incr.ToLong(&l_min_image_incr)==false )
		{
			wxMessageBox( _("Minimal number of incremental image backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl134->SetFocus();
			return;
		}
		if(max_image_incr.ToLong(&l_max_image_incr)==false )
		{
			wxMessageBox( _("Maximal number of incremental image backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl135->SetFocus();
			return;
		}
		if(min_image_full.ToLong(&l_min_image_full)==false )
		{
			wxMessageBox( _("Minimal number of full image backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl136->SetFocus();
			return;
		}
		if(max_image_full.ToLong(&l_max_image_full)==false )
		{
			wxMessageBox( _("Maximal number of full image backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl137->SetFocus();
			return;
		}
	}
#endif
	if(startup_backup_delay.ToLong(&l_startup_backup_delay)==false)
	{
		wxMessageBox( _("Backup delay after system start is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl19->SetFocus();
		return;
	}
#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		std::string s_image_letters=image_letters.ToUTF8();
		if(strlower(s_image_letters)!="all" && strlower(s_image_letters)!="all_nonusb")
		{
			std::vector<std::string> img_paths;
			Tokenize(s_image_letters, img_paths, ";,");

			for(size_t i=0;i<img_paths.size();++i)
			{
				HANDLE hVolume = CreateFileA(("\\\\.\\"+img_paths[i]+":").c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL, OPEN_EXISTING, 0, NULL);

				if(hVolume==INVALID_HANDLE_VALUE && GetLastError()!=ERROR_ACCESS_DENIED)
				{
					wxMessageBox( trans_1(_("_1_ is not a volume"), ConvertToUnicode(img_paths[i])), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
					m_textCtrl23->SetFocus();
					return;
				}
				else if(hVolume!=INVALID_HANDLE_VALUE)
				{
					CloseHandle(hVolume);
				}
			}
		}		
	}
#endif
	if(internet_server_port.ToLong(&l_internet_server_port)==false)
	{
		wxMessageBox( _("The server port is not valid"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrlInternetServerPort->SetFocus();
		return;
	}

	internet_speed.ToLong(&l_internet_speed);
	l_internet_speed*=1024/8;
	local_speed.ToLong(&l_local_speed);
	l_local_speed*=(1024*1024)/8;

	l_update_freq_image_full_orig=l_update_freq_image_full;
#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		if(m_checkBox1->GetValue()==false)
		{
			l_update_freq_image_full=-1;
		}
	}
#endif

	std::map<std::string, std::string> n_vals;
	n_vals["update_freq_incr"]=nconvert(static_cast<float>(l_update_freq_incr*60.f*60.f));
	n_vals["update_freq_full"]=nconvert(l_update_freq_full*24*60*60);
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
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
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		n_vals["min_image_incr"]=nconvert(l_min_image_incr);
		n_vals["max_image_incr"]=nconvert(l_max_image_incr);
		n_vals["min_image_full"]=nconvert(l_min_image_full);
		n_vals["max_image_full"]=nconvert(l_max_image_full);
	}
#endif
	n_vals["computername"]=computername.ToUTF8();
	n_vals["backup_window"]=backup_window.ToUTF8();
	n_vals["backup_window_incr_file"]=backup_window.ToUTF8();
	n_vals["backup_window_full_file"]=backup_window.ToUTF8();
	n_vals["backup_window_incr_image"]=backup_window.ToUTF8();
	n_vals["backup_window_full_image"]=backup_window.ToUTF8();
	n_vals["exclude_files"]=exclude_files.ToUTF8();
	n_vals["include_files"]=include_files.ToUTF8();
	n_vals["startup_backup_delay"]=nconvert(l_startup_backup_delay*60);
#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		n_vals["image_letters"]=image_letters.ToUTF8();
		n_vals["internet_image_backups"]=nconvert(internet_image_backups);
	}
#endif
	n_vals["internet_mode_enabled"]=nconvert(internet_mode_enabled);
	n_vals["internet_full_file_backups"]=nconvert(internet_full_file_backups);
	n_vals["internet_server"]=internet_server.ToUTF8();
	n_vals["internet_server_port"]=nconvert(l_internet_server_port);
	n_vals["internet_authkey"]=internet_authkey.ToUTF8();

	if(internet_speed!=wxT("-") && !internet_speed.empty())
		n_vals["internet_speed"]=nconvert(l_internet_speed);
	else
		n_vals["internet_speed"]="-1";

	if(local_speed!=wxT("-") && !local_speed.empty())
		n_vals["local_speed"]=nconvert(l_local_speed);
	else
		n_vals["local_speed"]="-1";
	n_vals["internet_encrypt"]=nconvert(internet_encrypt);
	n_vals["internet_compress"]=nconvert(internet_compress);
	

	std::string ndata;

	std::vector<std::wstring> keys=settings->getKeys();

	for(std::map<std::string, std::string>::iterator it=n_vals.begin();it!=n_vals.end();++it)
	{
		const std::string &nkey=it->first;
		std::string def_value;
		bool found_key=false;

		std::wstring key_w=ConvertToUnicode(nkey);
		for(size_t i=0;i<keys.size();++i)
		{
			if(keys[i]==key_w)
			{
				found_key=true;
				break;
			}
		}

		if( found_key || !settings->getValue(it->first+"_def", &def_value) || def_value!=it->second )
		{
			ndata+=nkey+"="+it->second+"\n";
		}
	}

	for(size_t i=0;i<keys.size();++i)
	{
		std::string key=ConvertToUTF8(keys[i]);
		std::map<std::string, std::string>::iterator iter=n_vals.find(key);
		if(iter==n_vals.end())
		{
			std::wstring val;
			if(settings->getValue(keys[i], &val) )
			{
				ndata+=key+"="+ConvertToUTF8(val)+"\n";
			}
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
