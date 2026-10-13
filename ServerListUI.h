/*************************************************************************
*    UrBackup - Client/Server backup system
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

#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "Connector.h"
#include <map>

//Edits one server of the server list. "Local (LAN)" and "Internet" gate what the
//client does with the server; the internet fields are only enabled with "Internet"
class ServerEditDialog : public wxDialog
{
public:
	ServerEditDialog(wxWindow* parent, const SServerListEntry& entry);

	SServerListEntry getEntry() const { return entry; }

private:
	void OnInternetCheck(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);
	void updateEnabled();

	SServerListEntry entry;

	wxTextCtrl* m_name;
	wxCheckBox* m_local;
	wxCheckBox* m_internet;
	wxTextCtrl* m_url;
	wxTextCtrl* m_proxy;
	wxTextCtrl* m_authkey;
	wxCheckBox* m_compress;
	wxCheckBox* m_encrypt;
};

//"Servers" page of the settings dialog
class ServersPanel : public wxPanel
{
public:
	ServersPanel(wxWindow* parent, const SServerList& server_list);

	bool isModified() const { return modified; }
	const std::vector<SServerListEntry>& getEntries() const { return entries; }

	static wxString displayName(const SServerListEntry& entry);
	//How the server is connected at the moment
	static wxString statusText(const SServerListEntry& entry);
	//"urbackup://host:port" from the server/port fields (and back)
	static wxString toUrl(const SServerListEntry& entry);
	static void fromUrl(wxString url, SServerListEntry& entry);

private:
	void fillList();
	long selectedItem();
	void OnAdd(wxCommandEvent& event);
	void OnEdit(wxCommandEvent& event);
	void OnRemove(wxCommandEvent& event);
	void OnTrust(wxCommandEvent& event);
	void OnSelect(wxListEvent& event);
	void OnActivate(wxListEvent& event);
	void updateButtons();

	std::vector<SServerListEntry> entries;
	std::vector<std::string> pending;
	bool modified;

	wxListCtrl* m_list;
	wxButton* m_add;
	wxButton* m_edit;
	wxButton* m_remove;
	wxButton* m_trust;
};

//The backup paths of the client, shared by the "Paths" pages of the servers
struct SPathsModel
{
	SPathsModel() : modified(false) {}

	std::vector<SBackupDir> dirs;
	//Identity -> display name of the servers
	std::map<std::string, wxString> server_names;
	std::vector<std::string> server_idents;
	bool modified;

	bool isBackedUpTo(const SBackupDir& dir, const std::string& ident) const;
};

//"Paths" page of a server: the paths backed up to this server
class ServerPathsPage : public wxPanel
{
public:
	ServerPathsPage(wxWindow* parent, SPathsModel* model, const std::string& ident, bool read_only);

	//The paths may have been changed on the page of another server
	void refresh();

private:
	void OnAdd(wxCommandEvent& event);
	void OnRemove(wxCommandEvent& event);
	void OnSelect(wxListEvent& event);
	void OnName(wxCommandEvent& event);
	void updateButtons();
	int selectedDir();
	wxString uniqueName(const wxString& path);

	SPathsModel* model;
	std::string ident;
	bool read_only;
	bool updating;
	//Index into model->dirs of each list item
	std::vector<size_t> shown;

	wxListCtrl* m_list;
	wxTextCtrl* m_name;
	wxButton* m_add;
	wxButton* m_remove;
};

#ifdef _WIN32
class SelectWindowsComponents;
//"Components" page of a server: the Windows components backed up to this server
class ServerComponentsPage : public wxPanel
{
public:
	ServerComponentsPage(wxWindow* parent, const std::string& ident, int facet_id, bool allow_restore);
	~ServerComponentsPage();

	//Reads the components (takes a while) the first time the page is shown
	void load();
	void save();

private:
	std::string ident;
	int facet_id;
	SelectWindowsComponents* components;
	wxStaticText* m_loading;
};
#endif

//"Logs" page of a server: the logs of its backups
class ServerLogsPage : public wxPanel
{
public:
	ServerLogsPage(wxWindow* parent, const std::string& ident);

	//Reads the logs the first time the page is shown
	void load();

private:
	void showLog();

	std::string ident;
	bool loaded;
	std::vector<SLogEntry> entries;
	wxListBox* m_list;
	wxChoice* m_level;
	wxTextCtrl* m_text;
};

//Settings window of the client: computer name, the servers (a click on one shows
//its backup settings below) and the other configuration windows
class Settings;
class wxNotebook;
class ClientSettingsDialog : public wxDialog
{
public:
	ClientSettingsDialog(wxWindow* parent, const SServerList& server_list, int capa);
	~ClientSettingsDialog();

private:
	void fillServers();
	void highlightSelected();
	void selectServer(int idx);
	void editServer(int idx);
	void removeServer(int idx);
	void OnAdd(wxCommandEvent& event);
	void OnTrust(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

	std::vector<SServerListEntry> entries;
	std::vector<std::string> pending;
	std::string primary;
	bool modified;
	int capa;
	wxString computername_orig;
	int selected;

	wxTextCtrl* m_computername;
	wxPanel* m_rows;
	wxButton* m_trust;
	wxStaticText* m_settings_heading;
	wxStaticText* m_notice;
	wxPanel* m_pages;

	std::vector<wxPanel*> row_panels;
	std::vector<std::vector<wxWindow*> > row_texts;

	//Backup settings of the servers that were selected (hidden dialogs, their pages are in m_pages)
	std::map<std::string, Settings*> server_settings;
	std::map<std::string, wxNotebook*> server_pages;
	std::map<std::string, ServerPathsPage*> paths_pages;
#ifdef _WIN32
	std::vector<ServerComponentsPage*> components_pages;
#endif
	wxNotebook* current_pages;

	SPathsModel paths;
	bool paths_loaded;
};
