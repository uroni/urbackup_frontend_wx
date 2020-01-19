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

#ifndef _WIN32
#include "../config.h"
#endif

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

#ifdef wxUSE_WCHAR_T
#define ConvertToWX(x) ConvertToUnicode(x)
#else
#define ConvertToWX(x)
#endif

bool getSettingsValue(std::wstring key, std::wstring *ret, CFileSettingsReader *settings)
{
	std::wstring use_str;
	settings->getValue(key + L".use", &use_str);
	int use = watoi(use_str);

	if (use == c_use_value_client)
	{
		if (settings->getValue(key + L".client", ret))
		{
			return true;
		}
	}
	else if(use == c_use_value)
	{
		if (settings->getValue(key + L".home", ret))
		{
			return true;
		}
	}
	else if (use == c_use_group)
	{
		if (settings->getValue(key + L".group", ret))
		{
			return true;
		}
	}
	else
	{
		if (!settings->getValue(key, ret))
		{
			if (settings->getValue(key + L"_def", ret))
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}

	return false;
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

Settings::Settings(wxWindow* parent) : GUISettings(parent),
	init_complete(false)
{
	SetIcon(wxIcon(res_path+wxT("backup-ok.")+ico_ext, ico_type));
#ifdef _DEBUG
	settings=new CFileSettingsReader("urbackup/data/settings.cfg");
#elif _WIN32
	settings=new CFileSettingsReader(g_res_path+"urbackup/data/settings.cfg");
#else
	settings = new CFileSettingsReader(VARDIR "/urbackup/data/settings.cfg");
#endif

	std::wstring client_settings_tray_access_pw;
	if (getSettingsValue(L"client_settings_tray_access_pw", &client_settings_tray_access_pw, settings)
		&& !client_settings_tray_access_pw.empty())
	{
		do
		{
			wxPasswordEntryDialog* pwEntry = new wxPasswordEntryDialog(this, _("Please enter the tray access text"));
			if (pwEntry->ShowModal() != wxID_OK)
			{
				Close();
				return;
			}

			if (pwEntry->GetValue() == client_settings_tray_access_pw)
			{
				break;
			}
			else
			{
				wxDialog* pwWrong = new wxDialog(this, -1, _("Tray access text wrong. Please retry or cancel."));
				if (pwWrong->ShowModal() != wxID_OK)
				{
					Close();
					return;
				}

			}
		} while (true);
	}

	std::wstring t;
	if(getSettingsValue(L"update_freq_incr", &t, settings))
	{
		m_textCtrl1->SetValue(wxString(convert(watoi(t)/60.f/60.f).c_str()));
	}
	else
	{
		m_textCtrl1->SetValue(wxT("12"));
	}
	setSettingsSwitch(L"update_freq_incr", m_bitmapButton1, m_textCtrl1);
	if(getSettingsValue(L"update_freq_full", &t, settings))
	{
		m_textCtrl2->SetValue(wxString(convert(watoi(t)/24/60/60).c_str()));
	}
	else
	{
		m_textCtrl2->SetValue(wxT("30"));
	}
	setSettingsSwitch(L"update_freq_full", m_bitmapButton2, m_textCtrl2);
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
				m_textCtrl22->SetValue(wxString(convert(-1*watoi(t)/24/60/60).c_str()));
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
		setSettingsSwitch(L"update_freq_image_full", m_bitmapButton22, m_textCtrl22);
		if(getSettingsValue(L"update_freq_image_incr", &t, settings))
		{
			if(watoi(t)>0)
				m_textCtrl21->SetValue(wxString(convert(watoi(t)/24/60/60).c_str()));
			else
				m_textCtrl21->SetValue(wxString(convert(-1*watoi(t) / 24 / 60 / 60).c_str()));
		}
		else
		{
			m_textCtrl21->SetValue(wxT("7"));
		}
		setSettingsSwitch(L"update_freq_image_incr", m_bitmapButton21, m_textCtrl21);
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
	setSettingsSwitch(L"max_file_incr", m_bitmapButton131, m_textCtrl131);
	if(getSettingsValue(L"min_file_incr", &t, settings))
	{
		m_textCtrl13->SetValue(wxString(convert(watoi(t)).c_str()));
	}
	else
	{
		m_textCtrl13->SetValue(wxT("40"));
	}
	setSettingsSwitch(L"min_file_incr", m_bitmapButton13, m_textCtrl13);
	if(getSettingsValue(L"max_file_full", &t, settings))
	{
		m_textCtrl133->SetValue(wxString(convert(watoi(t)).c_str()));
	}
	else
	{
		m_textCtrl133->SetValue(wxT("10"));
	}
	setSettingsSwitch(L"max_file_full", m_bitmapButton133, m_textCtrl133);
	if(getSettingsValue(L"min_file_full", &t,settings))
	{
		m_textCtrl132->SetValue(wxString(convert(watoi(t)).c_str()));
	}
	else
	{
		m_textCtrl132->SetValue(wxT("2"));
	}
	setSettingsSwitch(L"min_file_full", m_bitmapButton132, m_textCtrl132);
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
		setSettingsSwitch(L"min_image_incr", m_bitmapButton134, m_textCtrl134);
		if(getSettingsValue(L"max_image_incr", &t, settings))
		{
			m_textCtrl135->SetValue(wxString(convert(watoi(t)).c_str()));
		}
		else
		{
			m_textCtrl135->SetValue(wxT("30"));
		}
		setSettingsSwitch(L"max_image_incr", m_bitmapButton135, m_textCtrl135);
		if(getSettingsValue(L"min_image_full", &t, settings))
		{
			m_textCtrl136->SetValue(wxString(convert(watoi(t)).c_str()));
		}
		else
		{
			m_textCtrl136->SetValue(wxT("2"));
		}
		setSettingsSwitch(L"min_image_full", m_bitmapButton136, m_textCtrl136);
		if(getSettingsValue(L"max_image_full", &t, settings))
		{
			m_textCtrl137->SetValue(wxString(convert(watoi(t)).c_str()));
		}
		else
		{
			m_textCtrl137->SetValue(wxT("5"));
		}
		setSettingsSwitch(L"max_image_full", m_bitmapButton137, m_textCtrl137);
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
	setSettingsSwitch(L"backup_window_incr_file", m_bitmapButton17, m_textCtrl17);
	if(getSettingsValue(L"exclude_files", &t, settings))
	{
		m_textCtrl16->SetValue(t);
	}
	else
	{
		m_textCtrl16->SetValue(wxT(""));
	}
	setSettingsSwitch(L"exclude_files", m_bitmapButton16, m_textCtrl16);
	if(getSettingsValue(L"include_files", &t, settings))
	{
		m_textCtrl161->SetValue(t);
	}
	else
	{
		m_textCtrl161->SetValue(wxT(""));
	}
	setSettingsSwitch(L"include_files", m_bitmapButton161, m_textCtrl161);
	if(getSettingsValue(L"startup_backup_delay", &t, settings))
	{
		m_textCtrl19->SetValue(convert(watoi(t)/60));
	}
	else
	{
		m_textCtrl19->SetValue(wxT("0"));
	}
	setSettingsSwitch(L"startup_backup_delay", m_bitmapButton19, m_textCtrl19);
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
		setSettingsSwitch(L"image_letters", m_bitmapButton23, m_textCtrl23);
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
	setSettingsSwitch(L"internet_full_file_backups", m_bitmapButtonInternetFullFile, m_checkBoxInternetFullFile);
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
		setSettingsSwitch(L"internet_image_backups", m_bitmapButtonInternetImage, m_checkBoxInternetImage);
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
	if (getSettingsValue(L"internet_server_proxy", &t, settings))
	{
		m_textCtrlInternetServerProxy->SetValue(t);
	}
	else
	{
		m_textCtrlInternetServerProxy->SetValue(wxT(""));
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
	setSettingsSwitch(L"local_speed", m_bitmapButtonLocalSpeed, m_textCtrlLocalSpeed);
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
	setSettingsSwitch(L"internet_speed", m_bitmapButtonInternetSpeed, m_textCtrlInternetSpeed);
	if(getSettingsValue(L"internet_compress", &t, settings) && t==L"false")
	{
		m_checkBoxInternetCompress->SetValue(false);
	}
	else
	{
		m_checkBoxInternetCompress->SetValue(true);
	}
	setSettingsSwitch(L"internet_compress", m_bitmapButtonInternetCompress, m_checkBoxInternetCompress);
	if(getSettingsValue(L"internet_encrypt", &t, settings) && t==L"false")
	{
		m_checkBoxInternetEncrypt->SetValue(false);
	}
	else
	{
		m_checkBoxInternetEncrypt->SetValue(true);
	}
	setSettingsSwitch(L"internet_encrypt", m_bitmapButtonInternetEncrypt, m_checkBoxInternetEncrypt);

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
	init_complete = true;
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
	wxString internet_server_proxy = m_textCtrlInternetServerProxy->GetValue();
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

	if(!update_freq_incr.ToCDouble(&l_update_freq_incr)
		&& settings_info[L"update_freq_incr"].use==c_use_value_client )
	{
		wxMessageBox( _("The incremental backup interval is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl1->SetFocus();
		return;
	}
	if(!update_freq_full.ToLong(&l_update_freq_full)
		&& settings_info[L"update_freq_full"].use==c_use_value_client)
	{
		wxMessageBox( _("The full backup interval  is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl2->SetFocus();
		return;
	}
#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		if(!update_freq_image_full.ToLong(&l_update_freq_image_full) 
			&& m_checkBox1->GetValue()
			&& settings_info[L"update_freq_image_full"].use == c_use_value_client)
		{
			wxMessageBox( _("The full image backup interval  is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl22->SetFocus();
			return;
		}
		if(!update_freq_image_incr.ToLong(&l_update_freq_image_incr) && m_checkBox1->GetValue()
			&& settings_info[L"update_freq_image_incr"].use == c_use_value_client)
		{
			wxMessageBox( _("The incremental image backup time is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl21->SetFocus();
			return;
		}
	}
#endif
	if(!max_file_incr.ToLong(&l_max_file_incr)
		&& settings_info[L"max_file_incr"].use == c_use_value_client)
	{
		wxMessageBox( _("The maximal number of incremental file backups  is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl131->SetFocus();
		return;
	}
	if(!min_file_incr.ToLong(&l_min_file_incr)
		&& settings_info[L"min_file_incr"].use == c_use_value_client)
	{
		wxMessageBox( _("The minimal number of incremental file backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl13->SetFocus();
		return;
	}
	if(!max_file_full.ToLong(&l_max_file_full)
		&& settings_info[L"max_file_full"].use == c_use_value_client)
	{
		wxMessageBox( _("The maximal number of full file backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl133->SetFocus();
		return;
	}
	if(!min_file_full.ToLong(&l_min_file_full)
		&& settings_info[L"min_file_full"].use == c_use_value_client)
	{
		wxMessageBox( _("Minimal number of full file backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl132->SetFocus();
		return;
	}
#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		if(!min_image_incr.ToLong(&l_min_image_incr)
			&& settings_info[L"min_image_incr"].use == c_use_value_client)
		{
			wxMessageBox( _("Minimal number of incremental image backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl134->SetFocus();
			return;
		}
		if(!max_image_incr.ToLong(&l_max_image_incr)
			&& settings_info[L"max_image_incr"].use == c_use_value_client)
		{
			wxMessageBox( _("Maximal number of incremental image backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl135->SetFocus();
			return;
		}
		if(!min_image_full.ToLong(&l_min_image_full)
			&& settings_info[L"min_image_full"].use == c_use_value_client)
		{
			wxMessageBox( _("Minimal number of full image backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl136->SetFocus();
			return;
		}
		if(!max_image_full.ToLong(&l_max_image_full)
			&& settings_info[L"max_image_full"].use == c_use_value_client)
		{
			wxMessageBox( _("Maximal number of full image backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl137->SetFocus();
			return;
		}
	}
#endif
	if(!startup_backup_delay.ToLong(&l_startup_backup_delay)
		&& settings_info[L"start_backup_delay"].use == c_use_value_client)
	{
		wxMessageBox( _("Backup delay after system start is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl19->SetFocus();
		return;
	}
#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		std::string s_image_letters=image_letters.ToUTF8();
		if(strlower(s_image_letters)!="all" && strlower(s_image_letters)!="all_nonusb"
			&& settings_info[L"image_letters"].use == c_use_value_client)
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
	if(!internet_server_port.ToLong(&l_internet_server_port))
	{
		wxMessageBox( _("The server port is not valid"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrlInternetServerPort->SetFocus();
		return;
	}

	internet_speed.ToLong(&l_internet_speed);
	l_internet_speed*=1024/8;
	local_speed.ToLong(&l_local_speed);
	l_local_speed*=(1024*1024)/8;

#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		l_update_freq_image_full_orig = l_update_freq_image_full;
		if(m_checkBox1->GetValue()==false)
		{
			if(l_update_freq_image_full>0)
				l_update_freq_image_full*=-1;
			if (l_update_freq_image_incr > 0)
				l_update_freq_image_incr *= -1;
		}
	}
#endif

	std::string s_data;

	s_data += std::string("internet_mode_enabled=")+nconvert(internet_mode_enabled)+"\n";
	s_data += std::string("internet_server=")+ internet_server.ToUTF8() + "\n";
	s_data += std::string("internet_server_port=") + nconvert(l_internet_server_port) + "\n";
	s_data += std::string("internet_server_proxy=") + internet_server_proxy.ToUTF8() + "\n";
	s_data += std::string("internet_authkey=") + internet_authkey.ToUTF8() + "\n";
	s_data += std::string("computername=") + computername.ToUTF8() + "\n";

	for (std::map<std::wstring, SSetting>::iterator it = settings_info.begin(); it != settings_info.end(); ++it)
	{
		std::wstring old_val;
		std::wstring old_use;
		if (it->second.use != c_use_group
			&& it->second.use!=c_use_value
			&& (!settings->getValue(it->first+L".client", &old_val)
				|| old_val!=it->second.value_client
				|| !settings->getValue(it->first + L".use", &old_use)
				|| old_use!=convert(it->second.use) ) )
		{
			std::string key(wxString(it->first).ToUTF8());
			s_data += key + ".client=" + wxString(it->second.value_client).ToUTF8()+"\n";
			s_data += key + ".use=" + nconvert(it->second.use) + "\n";
		}
	}

	Connector::updateSettings(s_data);

	/*std::map<std::string, std::string> n_vals;
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
	n_vals["internet_server_proxy"] = internet_server_proxy.ToUTF8();
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

	Connector::updateSettings(mergeNewSettings(settings, n_vals));*/

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

void Settings::OnBitmapBtnClick(wxCommandEvent & event)
{
	std::wstring& key = button_ids[event.GetId()];
	SSetting& setting = settings_info[key];

	int use = setting.use;

	if (use == c_use_group)
		use = c_use_value;
	else if (use == c_use_value)
		use = c_use_value_client;
	else
		use = c_use_group;

	std::wstring val;
	if (use == c_use_value_client)
	{
		setting.btn->SetBitmapLabel(fa_client_img_scaled);
		val = setting.value_client;
	}
	else if (use == c_use_group)
	{
		setting.btn->SetBitmapLabel(fa_lock_img_scaled);
		val = setting.value_group;
	}
	else
	{
		setting.btn->SetBitmapLabel(fa_home_img_scaled);
		val = setting.value_home;
	}


	setting.use = use;

	wxCheckBox* checkBox = dynamic_cast<wxCheckBox*>(setting.ctrl);
	if (checkBox != NULL)
	{
		checkBox->SetValue(val == L"1" || val == L"true");
	}

	wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(setting.ctrl);
	if (textCtrl != NULL)
	{
		textCtrl->ChangeValue(transformValToUI(key, val));
	}
}

void Settings::OnCtlChange(wxCommandEvent & event)
{
	if (!init_complete)
		return;

	std::wstring key = ctrl_ids[event.GetId()];
	
	SSetting& setting = settings_info[key];

	setting.use = c_use_value_client;

	wxCheckBox* checkBox = dynamic_cast<wxCheckBox*>(setting.ctrl);
	if (checkBox != NULL)
	{
		setting.value_client = convert(checkBox->GetValue());
	}

	wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(setting.ctrl);
	if (textCtrl != NULL)
	{
		setting.value_client = transformValFromUI(key, textCtrl->GetValue().ToStdWstring());
	}	

	setting.btn->SetBitmapLabel(fa_client_img_scaled);
}

std::string Settings::mergeNewSettings(CFileSettingsReader * settings, const std::map<std::string, std::string>& n_vals)
{
	std::string ndata;

	std::vector<std::wstring> keys = settings->getKeys();

	for (std::map<std::string, std::string>::const_iterator it = n_vals.begin(); it != n_vals.end(); ++it)
	{
		const std::string &nkey = it->first;
		std::string def_value;
		bool found_key = false;

		std::wstring key_w = ConvertToUnicode(nkey);
		for (size_t i = 0; i<keys.size(); ++i)
		{
			if (keys[i] == key_w)
			{
				found_key = true;
				break;
			}
		}

		if (found_key || !settings->getValue(it->first + "_def", &def_value) || def_value != it->second)
		{
			ndata += nkey + "=" + it->second + "\n";
		}
	}

	for (size_t i = 0; i<keys.size(); ++i)
	{
		std::string key = ConvertToUTF8(keys[i]);
		std::map<std::string, std::string>::const_iterator iter = n_vals.find(key);
		if (iter == n_vals.end())
		{
			std::wstring val;
			if (settings->getValue(keys[i], &val))
			{
				ndata += key + "=" + ConvertToUTF8(val) + "\n";
			}
		}
	}

	return ndata;
}

std::wstring Settings::transformValToUI(const std::wstring & key, const std::wstring & val)
{
	if (key == "update_freq_incr")
	{
		return convert(watoi(val) / 60.f / 60.f);
	}
	else if (key == "update_freq_full")
	{
		return convert(watoi(val) / 24 / 60 / 60);
	}
	else if (key == "internet_speed")
	{
		return convert(watoi(val) / (1024 / 8));
	}
	else if (key == "local_speed")
	{
		return convert(watoi(val) / ((1024*1024) / 8));
	}
	else if (key == "update_freq_image_full"
		|| key=="update_freq_image_incr")
	{
		int ret = watoi(val) * 24 * 60 * 60;
		if (!m_checkBox1->GetValue()
			&& ret>0)
		{
			ret *= -1;
		}
		return convert(ret);
	}
	else if (key == "startup_backup_delay")
	{
		return convert(watoi(val) / 60);
	}

	return val;
}

std::wstring Settings::transformValFromUI(const std::wstring & key, const std::wstring & val)
{
	if (key == "update_freq_incr")
	{
		return convert(watoi(val) * 60.f * 60.f);
	}
	else if (key == "update_freq_full")
	{
		return convert(watoi(val)* 24 * 60 * 60);
	}
	else if (key == "internet_speed")
	{
		return convert(watoi(val) * (1024 / 8) );
	}
	else if (key == "local_speed")
	{
		return convert(watoi(val) * ((1024*1024) / 8));
	}
	else if (key == "update_freq_image_full"
		|| key=="update_freq_image_incr")
	{
		int ret = watoi(val) / 24 / 60 / 60;
		m_checkBox1->SetValue(ret > 0);
		return convert(ret);
	}
	else if (key == "startup_backup_delay")
	{
		return convert(watoi(val) * 60);
	}

	return val;
}

void Settings::setSettingsSwitch(const std::wstring & key, wxBitmapButton* btn, wxWindow* ctrl)
{
	std::wstring use_str;
	settings->getValue(key + L".use", &use_str);

	int use = watoi(use_str);;

	if (use == c_use_value_client)
	{
		btn->SetBitmapLabel(fa_client_img_scaled);
	}
	else if (use == c_use_group)
	{
		btn->SetBitmapLabel(fa_lock_img_scaled);
	}
	else
	{
		btn->SetBitmapLabel(fa_home_img_scaled);
	}

	ctrl_ids[ctrl->GetId()] = key;
	button_ids[btn->GetId()] = key;
	SSetting& setting = settings_info[key];
	setting.btn = btn;
	setting.ctrl = ctrl;
	setting.use = use;
	if (!settings->getValue(key + L".client", &setting.value_client))
	{
		settings->getValue(key, &setting.value_client);
	}
	if (!settings->getValue(key + L".home", &setting.value_home))
	{
		if(!settings->getValue(key+L".def", &setting.value_client))
			settings->getValue(key, &setting.value_client);
	}
	if (!settings->getValue(key + L".group", &setting.value_group))
	{
		if (!settings->getValue(key + L"_def", &setting.value_client))
			settings->getValue(key, &setting.value_client);
	}
}
