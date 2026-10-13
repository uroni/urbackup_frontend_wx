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
#include <wx/dir.h>
#include "main.h"
#include "capa_bits.h"
#include "TranslationHelper.h"
#include "ConfigPath.h"
#include "Logs.h"
#include "BackupSummary.h"
#ifdef _WIN32
#include "SelectWindowsComponents.h"
#include "SelectRestoreWindowsComponents.h"
HRESULT initCom();
#endif

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

std::string getFacetFilePath(int facet_id, const std::string& fn)
{
#ifdef _DEBUG
	std::string root = "urbackup/";
#elif _WIN32
	std::string root = g_res_path + "urbackup/";
#else
	std::string root = VARDIR "/urbackup/";
#endif

	if (facet_id <= 0)
	{
		//The default facet is the one with the lowest id
		wxDir dir(wxString::FromUTF8(root.c_str()));
		wxString name;
		bool cont = dir.IsOpened() && dir.GetFirst(&name, wxT("data_*"), wxDIR_DIRS);
		while (cont)
		{
			long curr_id;
			if (name.Mid(5).ToLong(&curr_id)
				&& curr_id > 0
				&& (facet_id <= 0 || curr_id < facet_id))
			{
				facet_id = static_cast<int>(curr_id);
			}
			cont = dir.GetNext(&name);
		}

		if (facet_id <= 0)
		{
			//Backend without client facets
			return root + "data/" + fn;
		}
	}

	return root + "data_" + nconvert(facet_id) + "/" + fn;
}

namespace
{
	//File with the settings that are the same for all servers
	std::string settingsFilePath(const std::string& fn)
	{
		return getFacetFilePath(0, fn);
	}
}

bool Settings::checkTrayAccessPw(wxWindow* parent)
{
	CFileSettingsReader settings(settingsFilePath("settings.cfg"));
	std::wstring client_settings_tray_access_pw;
	if (getSettingsValue(L"client_settings_tray_access_pw", &client_settings_tray_access_pw, &settings)
		&& !client_settings_tray_access_pw.empty())
	{
		do
		{
			wxPasswordEntryDialog* pwEntry = new wxPasswordEntryDialog(parent, _("Please enter the tray access text"));
			if (pwEntry->ShowModal() != wxID_OK)
			{
				return false;
			}

			if (pwEntry->GetValue() == client_settings_tray_access_pw)
			{
				break;
			}
			else
			{
				wxDialog* pwWrong = new wxDialog(parent, -1, _("Tray access text wrong. Please retry or cancel."));
				if (pwWrong->ShowModal() != wxID_OK)
				{
					return false;
				}

			}
		} while (true);
	}
	return true;
}

wxString Settings::currentComputerName()
{
	CFileSettingsReader settings(settingsFilePath("settings.cfg"));
	std::wstring t;
	if (settings.getValue(L"computername", &t))
	{
		return t;
	}
	return ConvertToWX(getServerName());
}

Settings::Settings(wxWindow* parent, const std::string& server_ident, bool access_pw_checked, bool p_per_server) : GUISettings(parent),
	init_complete(false), servers_panel(NULL), m_serverChoice(NULL), per_server(p_per_server), not_received(false),
	m_fileSummary(NULL), m_imageSummary(NULL)
{
	SetIcon(wxIcon(res_path+wxT("backup-ok.")+ico_ext, ico_type));
	settings=new CFileSettingsReader(settingsFilePath("settings.cfg"));

	if (!access_pw_checked && !checkTrayAccessPw(this))
	{
		Close();
		return;
	}

	SServerList server_list = Connector::getServerList();
	//The selected server has not sent its settings to this client yet
	bool settings_not_received = false;
	if (server_list.supported)
	{
		primary_server = server_list.primary;
		selected_server = server_ident;
		if (selected_server.empty())
		{
			selected_server = primary_server;
		}
		if (selected_server.empty())
		{
			//No server sent settings yet. Show the first one
			for (size_t i = 0; i < server_list.entries.size() && selected_server.empty(); ++i)
				selected_server = server_list.entries[i].ident;
		}

		if (!selected_server.empty())
		{
			//The settings of a server are in the directory of its client facet
			int facet_id = 0;
			for (size_t i = 0; i < server_list.entries.size(); ++i)
			{
				if (server_list.entries[i].ident == selected_server)
					facet_id = server_list.entries[i].id;
			}
			std::string srv_fn = getFacetFilePath(facet_id, "settings.cfg");
			if (facet_id > 0
				&& wxFileExists(wxString::FromUTF8(srv_fn.c_str())))
			{
				delete settings;
				settings = new CFileSettingsReader(srv_fn);
			}
			else
			{
				settings_not_received = true;
			}
		}
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
	std::wstring internet_server_url;
	getSettingsValue(L"internet_server", &internet_server_url, settings);
	std::wstring internet_server_port;
	getSettingsValue(L"internet_server_port", &internet_server_port, settings);

	if (internet_server_url.find(L"urbackup://") != 0
		&& internet_server_url.find(L"wss://") != 0
		&& internet_server_url.find(L"ws://") != 0)
	{
		if (watoi(internet_server_port) == 55415 && !internet_server_url.empty())
		{
			internet_server_url = L"urbackup://" + internet_server_url;
		}
		else if(!internet_server_url.empty() && !internet_server_port.empty())
		{
			internet_server_url = L"urbackup://" + internet_server_url + L":" + internet_server_port;
		}
	}

	m_textCtrlInternetServer->SetValue(internet_server_url);

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
	m_textCtrl15->SetValidator(getPathValidator());

	if (server_list.supported && per_server)
	{
		//The server list and the computer name are in the client settings window
		const SServerListEntry* entry = NULL;
		for (size_t i = 0; i < server_list.entries.size(); ++i)
		{
			if (server_list.entries[i].ident == selected_server)
				entry = &server_list.entries[i];
		}
		if (entry != NULL)
		{
			SetTitle(wxString::Format(_("Backup settings of %s"), ServersPanel::displayName(*entry)));
		}

		//The client settings window shows the notice and the pages of this dialog
		not_received = settings_not_received;

		applyGroupedLayout();

		//Backup over internet only matters for a server reached via internet
		if (entry != NULL && !entry->internet)
		{
			int internet_page = m_notebook->FindPage(m_tab_internet);
			if (internet_page >= 0)
				m_notebook->RemovePage(internet_page);
			m_tab_internet->Hide();
		}

		Layout();
		Fit();
	}
	else if (server_list.supported)
	{
		//Each server has its own connection settings in the server list. The internet
		//tab keeps the backup settings that apply to internet backups
		servers_panel = new ServersPanel(m_notebook, server_list);
		int internet_page = m_notebook->FindPage(m_tab_internet);
		m_notebook->InsertPage(internet_page >= 0 ? internet_page : m_notebook->GetPageCount(), servers_panel, _("Servers"));

		wxWindow* connection_ctrls[] = { m_staticTextInternetEnabled, m_checkBoxInternetEnabled,
			m_staticInternetServer, m_textCtrlInternetServer, m_staticInternetServerProxy, m_textCtrlInternetServerProxy,
			m_staticInternetServerAuthkey, m_textCtrlInternetServerAuthkey,
			m_staticTextInternetCompress, m_checkBoxInternetCompress, m_bitmapButtonInternetCompress,
			m_staticTextInternetEncrypt, m_checkBoxInternetEncrypt, m_bitmapButtonInternetEncrypt };
		for (size_t i = 0; i < sizeof(connection_ctrls) / sizeof(connection_ctrls[0]); ++i)
		{
			if (connection_ctrls[i] != NULL)
				connection_ctrls[i]->Hide();
		}
		m_tab_internet->Layout();

		//Backup settings (intervals, retention, directories, ...) are per server
		std::vector<SServerListEntry> choice_entries;
		for (size_t i = 0; i < server_list.entries.size(); ++i)
		{
			if (!server_list.entries[i].ident.empty())
				choice_entries.push_back(server_list.entries[i]);
		}
		if (choice_entries.size() > 1)
		{
			wxBoxSizer* sel = new wxBoxSizer(wxHORIZONTAL);
			sel->Add(new wxStaticText(this, wxID_ANY, _("Backup settings of:")), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
			m_serverChoice = new wxChoice(this, wxID_ANY);
			for (size_t i = 0; i < choice_entries.size(); ++i)
			{
				wxString label = ServersPanel::displayName(choice_entries[i]);
				m_serverChoice->Append(label);
				server_choice_idents.push_back(choice_entries[i].ident);
				if (choice_entries[i].ident == selected_server)
					m_serverChoice->SetSelection(static_cast<int>(i));
			}
			sel->Add(m_serverChoice, 1, wxALL, 5);
			GetSizer()->Insert(0, sel, 0, wxEXPAND);
			m_serverChoice->Bind(wxEVT_CHOICE, &Settings::OnServerChoice, this);

			if (settings_not_received)
			{
				wxStaticText* notice = new wxStaticText(this, wxID_ANY,
					_("This server has not sent its settings to this client yet, so the values below may be "
					"those of another server. They arrive when the client's settings are saved on the server "
					"or the server restarts."));
				notice->SetForegroundColour(wxColour(192, 0, 0));
				notice->Wrap(wxDLG_UNIT(this, wxSize(330, -1)).GetWidth());
				GetSizer()->Insert(1, notice, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
			}

			if (selected_server != primary_server)
			{
				//The computer name applies to all servers
				m_textCtrl15->Disable();
			}
		}

		Layout();
		Fit();
	}

	if (!per_server)
	{
		addWindowButtons();
		Show(true);
	}

	init_complete = true;

	updateBackupSummary();
}

namespace
{
	//Grouped layout of a settings page: label, control (with its unit), lock button in aligned columns
	class SettingsGrid
	{
	public:
		SettingsGrid(wxWindow* parent)
			: parent(parent), grid(new wxFlexGridSizer(3, 0, 0)), rows(0)
		{}

		void heading(const wxString& text)
		{
			if (rows > 0)
			{
				//Space between the groups
				for (int i = 0; i < 3; ++i)
					grid->Add(0, 10);
			}
			wxStaticText* heading = new wxStaticText(parent, wxID_ANY, text);
			wxFont font = heading->GetFont();
			font.SetWeight(wxFONTWEIGHT_BOLD);
			heading->SetFont(font);
			grid->Add(heading, 0, wxLEFT | wxRIGHT | wxTOP, 5);
			for (int i = 0; i < 2; ++i)
				grid->Add(0, 0);
			++rows;
		}

		void row(wxWindow* label, wxWindow* ctrl, wxWindow* unit, wxWindow* btn)
		{
			if (label != NULL)
				grid->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
			else
				grid->Add(0, 0);

			wxBoxSizer* ctrl_unit = new wxBoxSizer(wxHORIZONTAL);
			if (ctrl != NULL)
				ctrl_unit->Add(ctrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
			if (unit != NULL)
				ctrl_unit->Add(unit, 0, wxALIGN_CENTER_VERTICAL | wxTOP | wxBOTTOM | wxRIGHT, 5);
			grid->Add(ctrl_unit, 0, wxALIGN_CENTER_VERTICAL);

			if (btn != NULL)
				grid->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
			else
				grid->Add(0, 0);
			++rows;
		}

		wxSizer* sizer() { return grid; }

	private:
		wxWindow* parent;
		wxFlexGridSizer* grid;
		int rows;
	};
}

void Settings::applyGroupedLayout()
{
	//File backups: interval and number of backups kept, grouped by incremental and full
	{
		SettingsGrid g(m_tab_filebackups);
		g.heading(_("Incremental file backups"));
		g.row(m_staticText1, m_textCtrl1, m_staticText2, sourceChoice(m_bitmapButton1));
		g.row(m_staticText9, m_textCtrl13, NULL, sourceChoice(m_bitmapButton13));
		g.row(m_staticText10, m_textCtrl131, NULL, sourceChoice(m_bitmapButton131));
		g.heading(_("Full file backups"));
		g.row(m_staticText3, m_textCtrl2, m_staticText4, sourceChoice(m_bitmapButton2));
		g.row(m_staticText11, m_textCtrl132, NULL, sourceChoice(m_bitmapButton132));
		g.row(m_staticText12, m_textCtrl133, NULL, sourceChoice(m_bitmapButton133));
		g.heading(_("Files"));
		g.row(m_staticText26, m_textCtrl16, NULL, sourceChoice(m_bitmapButton16));
		g.row(m_staticText261, m_textCtrl161, NULL, sourceChoice(m_bitmapButton161));
		m_staticText30->Hide();
		m_staticText29->Hide();
		m_fileSummary = addBackupSummary(m_tab_filebackups, g.sizer());
	}
#ifdef _WIN32
	if (!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		SettingsGrid g(m_tab_imagebackups);
		g.row(m_checkBox1, NULL, NULL, NULL);
		g.heading(_("Incremental image backups"));
		g.row(m_staticText6, m_textCtrl21, m_staticText41, sourceChoice(m_bitmapButton21));
		g.row(m_staticText14, m_textCtrl134, NULL, sourceChoice(m_bitmapButton134));
		g.row(m_staticText15, m_textCtrl135, NULL, sourceChoice(m_bitmapButton135));
		g.heading(_("Full image backups"));
		g.row(m_staticText7, m_textCtrl22, m_staticText42, sourceChoice(m_bitmapButton22));
		g.row(m_staticText16, m_textCtrl136, NULL, sourceChoice(m_bitmapButton136));
		g.row(m_staticText17, m_textCtrl137, NULL, sourceChoice(m_bitmapButton137));
		g.heading(_("Volumes"));
		g.row(m_staticText301, m_textCtrl23, NULL, sourceChoice(m_bitmapButton23));
		m_staticText28->Hide();
		m_imageSummary = addBackupSummary(m_tab_imagebackups, g.sizer());
	}
#endif
	//Client tab: only schedule and speed; the computer name is in the client settings window
	{
		SettingsGrid g(m_tab_client);
		g.heading(_("Schedule"));
		g.row(m_staticText27, m_textCtrl17, NULL, sourceChoice(m_bitmapButton17));
		g.row(m_staticText281, m_textCtrl19, m_staticText291, sourceChoice(m_bitmapButton19));
		g.heading(_("Limits"));
		g.row(m_staticTextLocalSpeed, m_textCtrlLocalSpeed, m_staticTextLocalSpeedUnit, sourceChoice(m_bitmapButtonLocalSpeed));
		m_staticText25->Hide();
		m_textCtrl15->Hide();
		m_tab_client->SetSizer(g.sizer(), true);
		int page = m_notebook->FindPage(m_tab_client);
		if (page >= 0)
			m_notebook->SetPageText(page, _("Schedule & limits"));
	}
	//Internet tab: only what is backed up over internet; the connection is in the server list
	{
		SettingsGrid g(m_tab_internet);
		g.heading(_("Backups over internet"));
#ifdef _WIN32
		if (!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
		{
			g.row(m_staticTextInternetImage, m_checkBoxInternetImage, NULL, sourceChoice(m_bitmapButtonInternetImage));
		}
#endif
		g.row(m_staticTextInternetFullFile, m_checkBoxInternetFullFile, NULL, sourceChoice(m_bitmapButtonInternetFullFile));
		g.row(m_staticTextInternetSpeed, m_textCtrlInternetSpeed, m_staticTextInternetSpeedUnit, sourceChoice(m_bitmapButtonInternetSpeed));
		wxWindow* connection_ctrls[] = { m_staticTextInternetEnabled, m_checkBoxInternetEnabled,
			m_staticInternetServer, m_textCtrlInternetServer, m_staticInternetServerProxy, m_textCtrlInternetServerProxy,
			m_staticInternetServerAuthkey, m_textCtrlInternetServerAuthkey,
			m_staticTextInternetCompress, m_checkBoxInternetCompress, m_bitmapButtonInternetCompress,
			m_staticTextInternetEncrypt, m_checkBoxInternetEncrypt, m_bitmapButtonInternetEncrypt };
		for (size_t i = 0; i < sizeof(connection_ctrls) / sizeof(connection_ctrls[0]); ++i)
		{
			if (connection_ctrls[i] != NULL)
				connection_ctrls[i]->Hide();
		}
		m_tab_internet->SetSizer(g.sizer(), true);
	}

	//Each page as large as its content
	m_tab_filebackups->Layout();
	m_tab_client->Layout();
	m_tab_internet->Layout();
}

wxTextCtrl* Settings::addBackupSummary(wxWindow* page, wxSizer* settings_sizer)
{
	wxTextCtrl* text = new wxTextCtrl(page, wxID_ANY, wxEmptyString, wxDefaultPosition,
		page->FromDIP(wxSize(-1, 120)), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
	text->SetBackgroundColour(page->GetBackgroundColour());

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(settings_sizer, 0);
	sizer->Add(text, 1, wxEXPAND | wxALL, 5);
	page->SetSizer(sizer, true);
	return text;
}

namespace
{
	std::string translateSummary(const std::string& text)
	{
		return std::string(wxGetTranslation(wxString::FromUTF8(text.c_str())).ToUTF8().data());
	}
}

void Settings::showBackupSummary(wxTextCtrl* text, bool image)
{
	if (text == NULL)
		return;

	backup_summary::SSettings s;
	s.image = image;

	wxTextCtrl* incr = image ? m_textCtrl21 : m_textCtrl1;
	wxTextCtrl* full = image ? m_textCtrl22 : m_textCtrl2;
	//The image backup intervals are shown without sign, the checkbox has it
	bool disabled = image && !m_checkBox1->GetValue();
	s.incr = backup_summary::parseInterval(incr->GetValue().ToStdString(), disabled, image ? 24 * 60 * 60 : 60 * 60);
	s.full = backup_summary::parseInterval(full->GetValue().ToStdString(), disabled, 24 * 60 * 60);
	s.max_full = backup_summary::parseCount((image ? m_textCtrl137 : m_textCtrl133)->GetValue().ToStdString());
	s.min_full = backup_summary::parseCount((image ? m_textCtrl136 : m_textCtrl132)->GetValue().ToStdString());
	s.max_incr = backup_summary::parseCount((image ? m_textCtrl135 : m_textCtrl131)->GetValue().ToStdString());
	s.min_incr = backup_summary::parseCount((image ? m_textCtrl134 : m_textCtrl13)->GetValue().ToStdString());

	//What incremental image backups are based on is set on the server
	std::wstring style;
	bool local_to_last = getSettingsValue(L"local_incr_image_style", &style, settings) && style == L"to-last";
	bool internet_to_last = !getSettingsValue(L"internet_incr_image_style", &style, settings) || style != L"to-full";
	s.to_last.push_back(local_to_last);
	if (internet_to_last != local_to_last)
		s.to_last.push_back(internet_to_last);

	backup_summary::SSummary summary = backup_summary::describe(s, translateSummary);

	wxTextAttr normal(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	wxTextAttr warning(wxColour(192, 0, 0));

	text->Freeze();
	text->Clear();
	text->SetDefaultStyle(normal);
	for (size_t i = 0; i < summary.info.size(); ++i)
	{
		text->AppendText(wxString::FromUTF8(summary.info[i].c_str()) + wxT("\n"));
	}
	text->SetDefaultStyle(warning);
	for (size_t i = 0; i < summary.warn.size(); ++i)
	{
		text->AppendText(wxT("\n") + _("Warning:") + wxT(" ") + wxString::FromUTF8(summary.warn[i].c_str()) + wxT("\n"));
	}
	text->ShowPosition(0);
	text->Thaw();
}

void Settings::updateBackupSummary()
{
	if (!init_complete)
		return;

	showBackupSummary(m_fileSummary, false);
	showBackupSummary(m_imageSummary, true);
}

void Settings::addWindowButtons()
{
	wxSizer* sizer = m_button1->GetContainingSizer();
	if (sizer == NULL)
		return;

	size_t pos = 0;
	if (!MyTimer::hasCapability(DONT_ALLOW_CONFIG_PATHS, capa))
	{
		wxButton* btn = new wxButton(this, wxID_ANY, _("Backup paths..."));
		btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Settings::OnOpenPaths), NULL, this);
		sizer->Insert(pos++, btn, 0, wxALIGN_BOTTOM | wxALL, 5);
	}
	if (!MyTimer::hasCapability(DONT_SHOW_LOGS, capa))
	{
		wxButton* btn = new wxButton(this, wxID_ANY, _("Logs..."));
		btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Settings::OnOpenLogs), NULL, this);
		sizer->Insert(pos++, btn, 0, wxALIGN_BOTTOM | wxALL, 5);
	}
#ifdef _WIN32
	if (!MyTimer::hasCapability(DONT_ALLOW_COMPONENT_CONFIG, capa))
	{
		wxButton* btn = new wxButton(this, wxID_ANY, _("Components..."));
		btn->SetToolTip(_("Configure components to backup"));
		btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Settings::OnOpenComponents), NULL, this);
		sizer->Insert(pos++, btn, 0, wxALIGN_BOTTOM | wxALL, 5);
	}
	if (!MyTimer::hasCapability(DONT_ALLOW_COMPONENT_RESTORE, capa)
		&& !MyTimer::hasCapability(STATUS_NO_COMPONENTS, capa))
	{
		wxButton* btn = new wxButton(this, wxID_ANY, _("Restore components..."));
		btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Settings::OnOpenRestoreComponents), NULL, this);
		sizer->Insert(pos++, btn, 0, wxALIGN_BOTTOM | wxALL, 5);
	}
#endif
	//Same height as Ok/Cancel
	for (size_t i = 0; i < pos; ++i)
	{
		sizer->GetItem(i)->GetWindow()->SetMinSize(wxDLG_UNIT(this, wxSize(-1, 13)));
	}
	if (pos > 0)
	{
		Layout();
		Fit();
	}
}

void Settings::openPaths(wxWindow* parent)
{
	ConfigPath* cp = new ConfigPath(parent);
	cp->ShowModal();
	cp->Destroy();
}

void Settings::openLogs(wxWindow* parent)
{
	Logs* l = new Logs(parent);
	l->ShowModal();
	l->Destroy();
}

void Settings::openComponents(wxWindow* parent)
{
#ifdef _WIN32
	initCom();
	SelectWindowsComponents* cp = new SelectWindowsComponents(parent);
	cp->ShowModal();
	cp->Destroy();
#endif
}

void Settings::openRestoreComponents(wxWindow* parent)
{
#ifdef _WIN32
	initCom();
	SelectRestoreComponents* cp = new SelectRestoreComponents(parent);
	cp->ShowModal();
	cp->Destroy();
#endif
}

void Settings::OnOpenPaths(wxCommandEvent& event)
{
	openPaths(this);
}

void Settings::OnOpenLogs(wxCommandEvent& event)
{
	openLogs(this);
}

void Settings::OnOpenComponents(wxCommandEvent& event)
{
	openComponents(this);
}

void Settings::OnOpenRestoreComponents(wxCommandEvent& event)
{
	openRestoreComponents(this);
}

Settings::~Settings(void)
{
	delete settings;
}

bool Settings::save()
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
	wxString internet_server_port = L"55415";

	if (!internet_server.empty() &&
		internet_server.find("urbackup://") != 0 &&
		internet_server.find("wss://") != 0 &&
		internet_server.find("ws://") != 0)
	{
		internet_server = "urbackup://" + internet_server;
	}

	if (internet_server.find("urbackup://") == 0)
	{
		wxString hostname = internet_server.substr(11);
		if (hostname.find(":") != std::string::npos)
		{
			internet_server_port = getafter(L":", hostname.ToStdWstring());
			internet_server = getuntil(L":", hostname.ToStdWstring());
		}
		else
		{
			internet_server_port = L"55415";
			internet_server = hostname;
		}
	}

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
		return false;
	}
	if(!update_freq_full.ToLong(&l_update_freq_full)
		&& settings_info[L"update_freq_full"].use==c_use_value_client)
	{
		wxMessageBox( _("The full backup interval  is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl2->SetFocus();
		return false;
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
			return false;
		}
		if(!update_freq_image_incr.ToLong(&l_update_freq_image_incr) && m_checkBox1->GetValue()
			&& settings_info[L"update_freq_image_incr"].use == c_use_value_client)
		{
			wxMessageBox( _("The incremental image backup time is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl21->SetFocus();
			return false;
		}
	}
#endif
	if(!max_file_incr.ToLong(&l_max_file_incr)
		&& settings_info[L"max_file_incr"].use == c_use_value_client)
	{
		wxMessageBox( _("The maximal number of incremental file backups  is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl131->SetFocus();
		return false;
	}
	if(!min_file_incr.ToLong(&l_min_file_incr)
		&& settings_info[L"min_file_incr"].use == c_use_value_client)
	{
		wxMessageBox( _("The minimal number of incremental file backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl13->SetFocus();
		return false;
	}
	if(!max_file_full.ToLong(&l_max_file_full)
		&& settings_info[L"max_file_full"].use == c_use_value_client)
	{
		wxMessageBox( _("The maximal number of full file backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl133->SetFocus();
		return false;
	}
	if(!min_file_full.ToLong(&l_min_file_full)
		&& settings_info[L"min_file_full"].use == c_use_value_client)
	{
		wxMessageBox( _("Minimal number of full file backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl132->SetFocus();
		return false;
	}
#ifdef _WIN32
	if(!MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, capa))
	{
		if(!min_image_incr.ToLong(&l_min_image_incr)
			&& settings_info[L"min_image_incr"].use == c_use_value_client)
		{
			wxMessageBox( _("Minimal number of incremental image backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl134->SetFocus();
			return false;
		}
		if(!max_image_incr.ToLong(&l_max_image_incr)
			&& settings_info[L"max_image_incr"].use == c_use_value_client)
		{
			wxMessageBox( _("Maximal number of incremental image backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl135->SetFocus();
			return false;
		}
		if(!min_image_full.ToLong(&l_min_image_full)
			&& settings_info[L"min_image_full"].use == c_use_value_client)
		{
			wxMessageBox( _("Minimal number of full image backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl136->SetFocus();
			return false;
		}
		if(!max_image_full.ToLong(&l_max_image_full)
			&& settings_info[L"max_image_full"].use == c_use_value_client)
		{
			wxMessageBox( _("Maximal number of full image backups is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			m_textCtrl137->SetFocus();
			return false;
		}
	}
#endif
	if(!startup_backup_delay.ToLong(&l_startup_backup_delay)
		&& settings_info[L"start_backup_delay"].use == c_use_value_client)
	{
		wxMessageBox( _("Backup delay after system start is not a number"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
		m_textCtrl19->SetFocus();
		return false;
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
					return false;
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
		m_textCtrlInternetServer->SetFocus();
		return false;
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

	if (servers_panel == NULL)
	{
		s_data += std::string("internet_mode_enabled=")+nconvert(internet_mode_enabled)+"\n";
		s_data += std::string("internet_server=")+ std::string(internet_server.ToUTF8()) + "\n";
		s_data += std::string("internet_server_port=") + nconvert(l_internet_server_port) + "\n";
		s_data += std::string("internet_server_proxy=") + std::string(internet_server_proxy.ToUTF8()) + "\n";
		s_data += std::string("internet_authkey=") + std::string(internet_authkey.ToUTF8()) + "\n";
	}
	if (!per_server
		&& (selected_server.empty() || selected_server == primary_server))
	{
		s_data += std::string("computername=") + std::string(computername.ToUTF8()) + "\n";
	}

	int64 ctime = wxGetUTCTimeMillis().GetValue() / 1000;

	for (std::map<std::wstring, SSetting>::iterator it = settings_info.begin(); it != settings_info.end(); ++it)
	{
		std::wstring old_val;
		std::wstring old_use;
		if ((it->second.use != c_use_group &&
			 it->second.use!=c_use_value &&
			  (!settings->getValue(it->first+L".client", &old_val) ||
				old_val!=it->second.value_client) ) ||
				( !settings->getValue(it->first + L".use", &old_use) ||
				old_use!=convert(it->second.use) ) )  
		{
			std::string key(wxString(it->first).ToUTF8());
			s_data += key + ".client=" + std::string(wxString(it->second.value_client).ToUTF8()) + "\n";
			s_data += key + ".use=" + nconvert(it->second.use) + "\n";
			int64 use_lm = 0;
			std::wstring old_use_lm_str;
			if (!settings->getValue(it->first + L".use_lm", &old_use_lm_str)
				|| old_use_lm_str.empty())
				use_lm = ctime;
			else
				wxString(old_use_lm_str).ToLongLong(&use_lm);


			if (convert(it->second.use) != old_use)
			{
				if (use_lm > ctime)
					++use_lm;
				else
					use_lm = ctime;
			}

			s_data += key + ".use_lm=" + nconvert(use_lm) + "\n";
		}
	}

	if (!s_data.empty())
	{
		Connector::updateSettings(s_data, 5000, selected_server);
	}

	if (servers_panel != NULL
		&& servers_panel->isModified())
	{
		if (!Connector::setServerList(servers_panel->getEntries()))
		{
			wxMessageBox(_("Saving the server list failed."), wxT("UrBackup"), wxOK | wxICON_ERROR);
		}
	}

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

	return true;
}

void Settings::OnOkClick( wxCommandEvent& event )
{
	if (save())
	{
		Close();
	}
}

void Settings::OnServerChoice(wxCommandEvent& event)
{
	int sel = m_serverChoice->GetSelection();
	if (sel < 0 || sel >= static_cast<int>(server_choice_idents.size())
		|| server_choice_idents[sel] == selected_server)
		return;

	//Reopen the dialog with the settings of the selected server (unsaved changes are discarded)
	switch_to_server = server_choice_idents[sel];
	if (IsModal())
	{
		EndModal(wxID_RETRY);
	}
	else
	{
		new Settings(GetParent(), switch_to_server, true);
		Destroy();
	}
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

	updateBackupSummary();
}

namespace
{
	//Explains where the value of a setting comes from and what a click changes
	void setSourceTooltip(wxBitmapButton* btn, int use)
	{
		if (use == c_use_value_client)
		{
			btn->SetToolTip(_("The value is set here on this computer and overrides the server. "
				"Click to use the server's default value instead."));
		}
		else if (use == c_use_group)
		{
			btn->SetToolTip(_("The value is the server's default (the same for all clients in the group). "
				"Click to use the value set on the server for this computer instead."));
		}
		else
		{
			btn->SetToolTip(_("The value is set on the server for this computer. "
				"Click to set the value here on this computer instead."));
		}
	}
}

void Settings::OnBitmapBtnClick(wxCommandEvent & event)
{
	std::wstring key = button_ids[event.GetId()];
	SSetting& setting = settings_info[key];

	int use = setting.use;

	//The server's value for this computer only if the server has one. Selecting it without
	//would save a setting without a value
	if (use == c_use_group)
		use = setting.has_home ? c_use_value : c_use_value_client;
	else if (use == c_use_value)
		use = c_use_value_client;
	else
		use = c_use_group;

	setSettingSource(key, use);
}

void Settings::OnSourceChoice(wxCommandEvent & event)
{
	std::wstring key = choice_ids[event.GetId()];
	SSetting& setting = settings_info[key];

	int sel = event.GetSelection();
	if (sel < 0 || sel >= static_cast<int>(setting.choice_uses.size())
		|| setting.choice_uses[sel] == setting.use)
		return;

	setSettingSource(key, setting.choice_uses[sel]);
}

wxWindow* Settings::sourceChoice(wxBitmapButton* btn)
{
	std::map<wxWindowID, std::wstring>::iterator it = button_ids.find(btn->GetId());
	if (it == button_ids.end())
		return btn;

	SSetting& setting = settings_info[it->second];

	wxArrayString labels;
	labels.Add(_("Server default"));
	setting.choice_uses.push_back(c_use_group);
	if (setting.has_home || setting.use == c_use_value)
	{
		labels.Add(_("Server, for this computer"));
		setting.choice_uses.push_back(c_use_value);
	}
	labels.Add(_("Set on this computer"));
	setting.choice_uses.push_back(c_use_value_client);

	setting.choice = new wxChoice(btn->GetParent(), wxID_ANY, wxDefaultPosition, wxDefaultSize, labels);
	setting.choice->SetToolTip(_("Where the value comes from: the server's default (the same for all clients in the group), "
		"a value set on the server for this computer, or a value set here on this computer."));
	for (size_t i = 0; i < setting.choice_uses.size(); ++i)
	{
		if (setting.choice_uses[i] == setting.use)
			setting.choice->SetSelection(static_cast<int>(i));
	}
	choice_ids[setting.choice->GetId()] = it->second;
	setting.choice->Bind(wxEVT_CHOICE, &Settings::OnSourceChoice, this);
	btn->Hide();
	return setting.choice;
}

void Settings::setSettingSource(const std::wstring & key, int use)
{
	SSetting& setting = settings_info[key];

	std::wstring val;
	if (use == c_use_value_client)
	{
		setting.btn->SetBitmapLabel(fa_client_img_scaled);
		setSourceTooltip(setting.btn, c_use_value_client);
		if (setting.value_client.empty())
		{
			//No value set on this computer yet: start with the value used so far
			setting.value_client = setting.use == c_use_value ? setting.value_home : setting.value_group;
		}
		if (setting.value_client.empty())
		{
			//Else with the value that is shown
			wxCheckBox* shown_checkBox = dynamic_cast<wxCheckBox*>(setting.ctrl);
			if (shown_checkBox != NULL)
			{
				setting.value_client = convert(shown_checkBox->GetValue());
			}
			wxTextCtrl* shown_textCtrl = dynamic_cast<wxTextCtrl*>(setting.ctrl);
			if (shown_textCtrl != NULL
				&& !shown_textCtrl->GetValue().Trim().empty())
			{
				setting.value_client = transformValFromUI(key, shown_textCtrl->GetValue().ToStdWstring());
			}
		}
		val = setting.value_client;
	}
	else if (use == c_use_group)
	{
		setting.btn->SetBitmapLabel(fa_lock_img_scaled);
		setSourceTooltip(setting.btn, c_use_group);
		val = setting.value_group;
	}
	else
	{
		setting.btn->SetBitmapLabel(fa_home_img_scaled);
		setSourceTooltip(setting.btn, c_use_value);
		val = setting.value_home;
	}

	setting.use = use;

	if (setting.choice != NULL)
	{
		for (size_t i = 0; i < setting.choice_uses.size(); ++i)
		{
			if (setting.choice_uses[i] == use)
				setting.choice->SetSelection(static_cast<int>(i));
		}
	}

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

	updateBackupSummary();
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
	setSourceTooltip(setting.btn, c_use_value_client);

	if (setting.choice != NULL)
	{
		for (size_t i = 0; i < setting.choice_uses.size(); ++i)
		{
			if (setting.choice_uses[i] == c_use_value_client)
				setting.choice->SetSelection(static_cast<int>(i));
		}
	}

	updateBackupSummary();
}

std::wstring Settings::transformValToUI(const std::wstring & key, const std::wstring & val)
{
	if (key == L"update_freq_incr")
	{
		return convert(watoi(val) / 60.f / 60.f);
	}
	else if (key == L"update_freq_full")
	{
		return convert(watoi(val) / 24 / 60 / 60);
	}
	else if (key == L"internet_speed")
	{
		return convert(watoi(val) / (1024 / 8));
	}
	else if (key == L"local_speed")
	{
		return convert(watoi(val) / ((1024*1024) / 8));
	}
	else if (key == L"update_freq_image_full"
		|| key==L"update_freq_image_incr")
	{
		//Seconds to days. A negative interval is a disabled one (shown by the check box)
		int ret = watoi(val) / (24 * 60 * 60);
		if (ret < 0)
		{
			ret *= -1;
		}
		return convert(ret);
	}
	else if (key == L"startup_backup_delay")
	{
		return convert(watoi(val) / 60);
	}

	return val;
}

std::wstring Settings::transformValFromUI(const std::wstring & key, const std::wstring & val)
{
	if (key == L"update_freq_incr")
	{
		return convert(watoi(val) * 60.f * 60.f);
	}
	else if (key == L"update_freq_full")
	{
		return convert(watoi(val) * 24 * 60 * 60);
	}
	else if (key == L"internet_speed")
	{
		return convert(watoi(val) * (1024 / 8) );
	}
	else if (key == L"local_speed")
	{
		return convert(watoi(val) * ((1024*1024) / 8));
	}
	else if (key == L"update_freq_image_full"
		|| key==L"update_freq_image_incr")
	{
		int ret = watoi(val) * 24 * 60 * 60;
		m_checkBox1->SetValue(ret > 0);
		return convert(ret);
	}
	else if (key == L"startup_backup_delay")
	{
		return convert(watoi(val) * 60);
	}

	return val;
}

void Settings::setSettingsSwitch(const std::wstring & key, wxBitmapButton* btn, wxWindow* ctrl)
{
	std::wstring use_str;
	int use;
	if (!settings->getValue(key + L".use", &use_str))
	{
		use = c_use_group;
	}
	else
	{
		use = watoi(use_str);;
	}

	if (use == c_use_value_client)
	{
		btn->SetBitmapLabel(fa_client_img_scaled);
		setSourceTooltip(btn, c_use_value_client);
	}
	else if (use == c_use_group)
	{
		btn->SetBitmapLabel(fa_lock_img_scaled);
		setSourceTooltip(btn, c_use_group);
	}
	else
	{
		btn->SetBitmapLabel(fa_home_img_scaled);
		setSourceTooltip(btn, c_use_value);
	}

	ctrl_ids[ctrl->GetId()] = key;
	button_ids[btn->GetId()] = key;
	SSetting& setting = settings_info[key];
	setting.btn = btn;
	setting.ctrl = ctrl;
	setting.choice = NULL;
	setting.use = use;
	{
		std::wstring value_home;
		setting.has_home = settings->getValue(key + L".home", &value_home) && !value_home.empty();
	}
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

wxNotebook* Settings::takePages(wxWindow* new_parent)
{
	m_notebook->Reparent(new_parent);
	return m_notebook;
}
