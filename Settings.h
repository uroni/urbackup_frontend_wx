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

#include <wx/wx.h>
#include <map>
#include "Connector.h"
#include "gui/GUI.h"
#include "FileSettingsReader.h"
#include "ServerListUI.h"

//Path of a file of a client facet of the backend (urbackup/data_<facet id>/<fn>). Each server
//has a facet with its settings. facet_id 0: the default facet (the one with the lowest id)
std::string getFacetFilePath(int facet_id, const std::string& fn);

namespace
{
	const int c_use_group = 1;
	const int c_use_value = 2;
	const char* c_use_value_str = "2";
	const int c_use_value_client = 4;
	const char* c_use_value_client_str = "4";
}


class Settings : public GUISettings
{
public:
	//server_ident: show/edit the backup settings of this server (empty: primary server)
	//per_server: opened from the client settings window for one server (no server list,
	//no computer name, settings grouped by incremental/full)
	Settings(wxWindow* parent, const std::string& server_ident = std::string(), bool access_pw_checked = false,
		bool per_server = false);
	virtual ~Settings(void);

	virtual void OnOkClick( wxCommandEvent& event );
	//Saves the changed settings. False (with a message) if a value is not valid
	bool save();
	//per_server: the pages (notebook) are shown in another window
	wxNotebook* takePages(wxWindow* new_parent);
	bool settingsNotReceived() const { return not_received; }
	virtual void OnAbortClick( wxCommandEvent& event );
	virtual void OnDisableImageBackups( wxCommandEvent& event );
	virtual void OnBitmapBtnClick(wxCommandEvent& event);
	virtual void OnCtlChange(wxCommandEvent& event);

	//The other configuration windows, opened from here so they share the
	//(elevated) settings process
	void OnOpenPaths(wxCommandEvent& event);
	void OnOpenLogs(wxCommandEvent& event);
	void OnOpenComponents(wxCommandEvent& event);
	void OnOpenRestoreComponents(wxCommandEvent& event);

	//Used by the client settings window too
	static void openPaths(wxWindow* parent);
	static void openLogs(wxWindow* parent);
	static void openComponents(wxWindow* parent);
	static void openRestoreComponents(wxWindow* parent);
	//Asks for the tray access text if one is configured. False if canceled
	static bool checkTrayAccessPw(wxWindow* parent);
	static wxString currentComputerName();

private:
	void addWindowButtons();
	void applyGroupedLayout();

	bool per_server;
	bool not_received;

	std::wstring transformValToUI(const std::wstring& key, const std::wstring& val);
	std::wstring transformValFromUI(const std::wstring& key, const std::wstring& val);

	void setSettingsSwitch(const std::wstring& key, wxBitmapButton* btn, wxWindow* ctrl);

	std::map<wxWindowID, std::wstring> button_ids;
	std::map<wxWindowID, std::wstring> ctrl_ids;

	struct SSetting
	{
		wxBitmapButton* btn;
		wxWindow* ctrl;
		int use;
		std::wstring value_group;
		std::wstring value_home;
		std::wstring value_client;
	};

	std::map<std::wstring, SSetting> settings_info;

	CFileSettingsReader *settings;

	bool init_complete;

	//NULL if the backend has no server list (older backend)
	ServersPanel* servers_panel;

	void OnServerChoice(wxCommandEvent& event);

public:
	//Set when the user selected another server (modal dialog ends with wxID_RETRY)
	std::string switch_to_server;

private:
	//Server whose backup settings are shown (empty: no server list)
	std::string selected_server;
	std::string primary_server;
	wxChoice* m_serverChoice;
	std::vector<std::string> server_choice_idents;
};