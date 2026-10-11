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

#include "ServerListUI.h"
#include "stringtools.h"
#include "Settings.h"
#include "main.h"
#include "capa_bits.h"
#include <wx/datetime.h>
#include <wx/graphics.h>
#include <wx/notebook.h>
#include <algorithm>

wxTextValidator getPathValidator(void);
extern std::string g_res_path;
#ifdef _WIN32
#include "SelectWindowsComponents.h"
HRESULT initCom();
#endif

namespace
{
	enum
	{
		COL_NAME = 0,
		COL_LOCAL,
		COL_INTERNET,
		COL_STATUS,
		COL_FINGERPRINT
	};

	wxString statusText(const SServerListEntry& entry)
	{
		wxString status;
		if (!entry.local && !entry.internet)
		{
			return _("Disabled");
		}
		if (entry.local
			&& entry.online
			&& !(entry.internet && entry.internet_status == "connected"))
		{
			status = _("Connected (LAN)");
		}
		if (entry.internet)
		{
			wxString internet_status;
			if (entry.internet_status == "connected")
				internet_status = _("Connected (internet)");
			else if (entry.internet_status == "connected_local")
				internet_status = _("Internet paused (LAN)");
			else if (entry.internet_status == "wait_local" || entry.internet_status == "initializing")
				internet_status = _("Waiting");
			else if (entry.internet_status == "connecting_failed")
				internet_status = _("Internet connection failed");
			else if (entry.internet_status.find("error:") == 0)
				internet_status = wxString::FromUTF8(entry.internet_status.substr(6).c_str());
			else if (!entry.internet_status.empty())
				internet_status = wxString::FromUTF8(entry.internet_status.c_str());

			if (!internet_status.empty())
			{
				if (!status.empty()) status += wxT(", ");
				status += internet_status;
			}
		}
		if (status.empty())
		{
			status = entry.ident.empty() ? _("Not connected yet") : _("Offline");
		}
		return status;
	}

	wxString fingerprintText(const SServerListEntry& entry)
	{
		if (!entry.fingerprint.empty())
		{
			//Short form, like an SSH key fingerprint
			std::string fp = entry.fingerprint;
			if (fp.size() > 23)
				fp = fp.substr(0, 23) + "...";
			return wxString::FromUTF8(fp.c_str());
		}
		if (!entry.ident.empty())
		{
			return wxString::FromUTF8(entry.ident.c_str());
		}
		return wxT("-");
	}
}

wxString ServersPanel::displayName(const SServerListEntry& entry)
{
	if (!entry.name.empty())
		return wxString::FromUTF8(entry.name.c_str());
	if (!entry.endpoint.empty())
		return wxString::FromUTF8(entry.endpoint.c_str());
	if (!entry.internet_server.empty())
		return wxString::FromUTF8(entry.internet_server.c_str());
	return _("(unnamed server)");
}

wxString ServersPanel::toUrl(const SServerListEntry& entry)
{
	wxString server = wxString::FromUTF8(entry.internet_server.c_str());
	if (server.empty()
		|| server.find(wxT("urbackup://")) == 0
		|| server.find(wxT("ws://")) == 0
		|| server.find(wxT("wss://")) == 0)
	{
		return server;
	}

	wxString url = wxT("urbackup://") + server;
	if (!entry.internet_server_port.empty()
		&& entry.internet_server_port != "55415")
	{
		url += wxT(":") + wxString::FromUTF8(entry.internet_server_port.c_str());
	}
	return url;
}

void ServersPanel::fromUrl(wxString url, SServerListEntry& entry)
{
	url.Trim().Trim(false);
	entry.internet_server_port = "55415";

	if (!url.empty()
		&& url.find(wxT("urbackup://")) != 0
		&& url.find(wxT("ws://")) != 0
		&& url.find(wxT("wss://")) != 0)
	{
		url = wxT("urbackup://") + url;
	}

	if (url.find(wxT("urbackup://")) == 0)
	{
		wxString hostname = url.substr(11);
		if (hostname.find(wxT(":")) != wxString::npos)
		{
			entry.internet_server_port = std::string(hostname.AfterFirst(':').ToUTF8());
			entry.internet_server = std::string(hostname.BeforeFirst(':').ToUTF8());
		}
		else
		{
			entry.internet_server = std::string(hostname.ToUTF8());
		}
	}
	else
	{
		entry.internet_server = std::string(url.ToUTF8());
	}
}

ServerEditDialog::ServerEditDialog(wxWindow* parent, const SServerListEntry& p_entry)
	: wxDialog(parent, wxID_ANY, p_entry.id < 0 ? _("Add server") : _("Edit server"),
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
	entry(p_entry)
{
	wxBoxSizer* top = new wxBoxSizer(wxVERTICAL);
	wxFlexGridSizer* grid = new wxFlexGridSizer(2, wxDLG_UNIT(this, wxSize(4, 3)));
	grid->AddGrowableCol(1);

	wxSize input_size = wxDLG_UNIT(this, wxSize(150, -1));

	grid->Add(new wxStaticText(this, wxID_ANY, _("Name:")), 0, wxALIGN_CENTER_VERTICAL);
	m_name = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8(entry.name.c_str()), wxDefaultPosition, input_size);
	m_name->SetHint(ServersPanel::displayName(entry));
	grid->Add(m_name, 1, wxEXPAND);

	if (!entry.ident.empty())
	{
		grid->Add(new wxStaticText(this, wxID_ANY, _("Server identity:")), 0, wxALIGN_CENTER_VERTICAL);
		wxString ident = wxString::FromUTF8(entry.fingerprint.empty() ? entry.ident.c_str() : entry.fingerprint.c_str());
		wxTextCtrl* ident_ctrl = new wxTextCtrl(this, wxID_ANY, ident, wxDefaultPosition, input_size, wxTE_READONLY);
		grid->Add(ident_ctrl, 1, wxEXPAND);
	}

	grid->Add(new wxStaticText(this, wxID_ANY, wxEmptyString));
	m_local = new wxCheckBox(this, wxID_ANY, _("Local (LAN): accept backups from this server in the local network"));
	m_local->SetValue(entry.local);
	grid->Add(m_local);

	grid->Add(new wxStaticText(this, wxID_ANY, wxEmptyString));
	m_internet = new wxCheckBox(this, wxID_ANY, _("Internet: connect to this server via internet"));
	m_internet->SetValue(entry.internet);
	grid->Add(m_internet);

	grid->Add(new wxStaticText(this, wxID_ANY, _("Server URL:")), 0, wxALIGN_CENTER_VERTICAL);
	m_url = new wxTextCtrl(this, wxID_ANY, ServersPanel::toUrl(entry), wxDefaultPosition, input_size);
	m_url->SetHint(wxT("urbackup://backup.example.com"));
	grid->Add(m_url, 1, wxEXPAND);

	grid->Add(new wxStaticText(this, wxID_ANY, _("HTTP(s) proxy:")), 0, wxALIGN_CENTER_VERTICAL);
	m_proxy = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8(entry.internet_server_proxy.c_str()), wxDefaultPosition, input_size);
	grid->Add(m_proxy, 1, wxEXPAND);

	grid->Add(new wxStaticText(this, wxID_ANY, _("Server password:")), 0, wxALIGN_CENTER_VERTICAL);
	m_authkey = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8(entry.internet_authkey.c_str()), wxDefaultPosition, input_size, wxTE_PASSWORD);
	grid->Add(m_authkey, 1, wxEXPAND);

	grid->Add(new wxStaticText(this, wxID_ANY, wxEmptyString));
	m_compress = new wxCheckBox(this, wxID_ANY, _("Compressed transfer"));
	m_compress->SetValue(entry.internet_compress);
	grid->Add(m_compress);

	grid->Add(new wxStaticText(this, wxID_ANY, wxEmptyString));
	m_encrypt = new wxCheckBox(this, wxID_ANY, _("Encrypted transfer"));
	m_encrypt->SetValue(entry.internet_encrypt);
	grid->Add(m_encrypt);

	top->Add(grid, 1, wxEXPAND | wxALL, 10);
	top->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 10);
	SetSizerAndFit(top);

	m_internet->Bind(wxEVT_CHECKBOX, &ServerEditDialog::OnInternetCheck, this);
	Bind(wxEVT_BUTTON, &ServerEditDialog::OnOk, this, wxID_OK);

	updateEnabled();
	CentreOnParent();
}

void ServerEditDialog::updateEnabled()
{
	bool internet = m_internet->GetValue();
	m_url->Enable(internet);
	m_proxy->Enable(internet);
	m_authkey->Enable(internet);
	m_compress->Enable(internet);
	m_encrypt->Enable(internet);
}

void ServerEditDialog::OnInternetCheck(wxCommandEvent& event)
{
	updateEnabled();
}

void ServerEditDialog::OnOk(wxCommandEvent& event)
{
	if (!m_local->GetValue() && !m_internet->GetValue())
	{
		wxMessageBox(_("Select at least one of \"Local (LAN)\" and \"Internet\". "
			"To stop using a server, remove it from the list."), _("Server"), wxOK | wxICON_WARNING, this);
		return;
	}

	if (m_internet->GetValue()
		&& (m_url->GetValue().Trim().empty() || m_authkey->GetValue().empty()))
	{
		wxMessageBox(_("Internet connections need the server URL and the server password."),
			_("Server"), wxOK | wxICON_WARNING, this);
		return;
	}

	entry.name = std::string(m_name->GetValue().Trim().Trim(false).ToUTF8());
	entry.local = m_local->GetValue();
	entry.internet = m_internet->GetValue();
	ServersPanel::fromUrl(m_url->GetValue(), entry);
	entry.internet_server_proxy = std::string(m_proxy->GetValue().Trim().Trim(false).ToUTF8());
	entry.internet_authkey = std::string(m_authkey->GetValue().ToUTF8());
	entry.internet_compress = m_compress->GetValue();
	entry.internet_encrypt = m_encrypt->GetValue();

	EndModal(wxID_OK);
}

ServersPanel::ServersPanel(wxWindow* parent, const SServerList& server_list)
	: wxPanel(parent, wxID_ANY), entries(server_list.entries), pending(server_list.pending), modified(false)
{
	wxBoxSizer* top = new wxBoxSizer(wxVERTICAL);

	top->Add(new wxStaticText(this, wxID_ANY,
		_("Servers this client is backed up by. Each server can be reached in the local network, via internet, or both.")),
		0, wxALL, 5);

	m_list = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(330, 110)),
		wxLC_REPORT | wxLC_SINGLE_SEL);
	m_list->InsertColumn(COL_NAME, _("Name"), wxLIST_FORMAT_LEFT, wxDLG_UNIT(this, wxSize(80, -1)).GetWidth());
	m_list->InsertColumn(COL_LOCAL, _("Local"), wxLIST_FORMAT_LEFT, wxDLG_UNIT(this, wxSize(28, -1)).GetWidth());
	m_list->InsertColumn(COL_INTERNET, _("Internet"), wxLIST_FORMAT_LEFT, wxDLG_UNIT(this, wxSize(35, -1)).GetWidth());
	m_list->InsertColumn(COL_STATUS, _("Status"), wxLIST_FORMAT_LEFT, wxDLG_UNIT(this, wxSize(90, -1)).GetWidth());
	m_list->InsertColumn(COL_FINGERPRINT, _("Identity"), wxLIST_FORMAT_LEFT, wxDLG_UNIT(this, wxSize(90, -1)).GetWidth());
	top->Add(m_list, 1, wxEXPAND | wxALL, 5);

	wxBoxSizer* buttons = new wxBoxSizer(wxHORIZONTAL);
	m_add = new wxButton(this, wxID_ANY, _("Add..."));
	m_edit = new wxButton(this, wxID_ANY, _("Edit..."));
	m_remove = new wxButton(this, wxID_ANY, _("Remove"));
	m_trust = new wxButton(this, wxID_ANY, _("Trust new server..."));
	buttons->Add(m_add, 0, wxRIGHT, 5);
	buttons->Add(m_edit, 0, wxRIGHT, 5);
	buttons->Add(m_remove, 0, wxRIGHT, 5);
	buttons->AddStretchSpacer();
	buttons->Add(m_trust, 0);
	top->Add(buttons, 0, wxEXPAND | wxALL, 5);

	SetSizerAndFit(top);

	m_add->Bind(wxEVT_BUTTON, &ServersPanel::OnAdd, this);
	m_edit->Bind(wxEVT_BUTTON, &ServersPanel::OnEdit, this);
	m_remove->Bind(wxEVT_BUTTON, &ServersPanel::OnRemove, this);
	m_trust->Bind(wxEVT_BUTTON, &ServersPanel::OnTrust, this);
	m_list->Bind(wxEVT_LIST_ITEM_SELECTED, &ServersPanel::OnSelect, this);
	m_list->Bind(wxEVT_LIST_ITEM_DESELECTED, &ServersPanel::OnSelect, this);
	m_list->Bind(wxEVT_LIST_ITEM_ACTIVATED, &ServersPanel::OnActivate, this);

	fillList();
}

void ServersPanel::fillList()
{
	m_list->DeleteAllItems();
	for (size_t i = 0; i < entries.size(); ++i)
	{
		const SServerListEntry& e = entries[i];
		long item = m_list->InsertItem(static_cast<long>(i), displayName(e));
		m_list->SetItem(item, COL_LOCAL, e.local ? _("Yes") : _("No"));
		m_list->SetItem(item, COL_INTERNET, e.internet ? _("Yes") : _("No"));
		m_list->SetItem(item, COL_STATUS, statusText(e));
		m_list->SetItem(item, COL_FINGERPRINT, fingerprintText(e));
	}
	updateButtons();
}

long ServersPanel::selectedItem()
{
	return m_list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
}

void ServersPanel::updateButtons()
{
	bool has_selection = selectedItem() >= 0;
	m_edit->Enable(has_selection);
	m_remove->Enable(has_selection);
	m_trust->Enable(!pending.empty());
}

void ServersPanel::OnSelect(wxListEvent& event)
{
	updateButtons();
}

void ServersPanel::OnActivate(wxListEvent& event)
{
	wxCommandEvent evt;
	OnEdit(evt);
}

void ServersPanel::OnAdd(wxCommandEvent& event)
{
	SServerListEntry entry;
	entry.id = -1;
	entry.local = false;
	entry.internet = true;
	ServerEditDialog dlg(this, entry);
	if (dlg.ShowModal() == wxID_OK)
	{
		entries.push_back(dlg.getEntry());
		modified = true;
		fillList();
	}
}

void ServersPanel::OnEdit(wxCommandEvent& event)
{
	long item = selectedItem();
	if (item < 0 || item >= static_cast<long>(entries.size()))
		return;

	ServerEditDialog dlg(this, entries[item]);
	if (dlg.ShowModal() == wxID_OK)
	{
		entries[item] = dlg.getEntry();
		modified = true;
		fillList();
	}
}

void ServersPanel::OnRemove(wxCommandEvent& event)
{
	long item = selectedItem();
	if (item < 0 || item >= static_cast<long>(entries.size()))
		return;

	wxString msg = wxString::Format(_("Remove server \"%s\"? The client will no longer trust it, "
		"so it cannot back up this computer anymore until it is added again."), displayName(entries[item]));
	if (wxMessageBox(msg, _("Remove server"), wxYES_NO | wxICON_QUESTION, this) != wxYES)
		return;

	entries.erase(entries.begin() + item);
	modified = true;
	fillList();
}

void ServersPanel::OnTrust(wxCommandEvent& event)
{
	if (pending.empty())
		return;

	wxArrayString choices;
	for (size_t i = 0; i < pending.size(); ++i)
	{
		choices.Add(wxString::FromUTF8(pending[i].c_str()));
	}

	int idx = wxGetSingleChoiceIndex(_("These servers tried to back up this computer, but are not trusted. "
		"Only trust a server you know."), _("Trust new server"), choices, this);
	if (idx < 0)
		return;

	if (!Connector::addNewServer(pending[idx]))
	{
		wxMessageBox(_("Trusting the server failed."), _("Trust new server"), wxOK | wxICON_ERROR, this);
		return;
	}

	SServerList server_list = Connector::getServerList();
	if (server_list.supported)
	{
		//Keep local edits, add the newly trusted server
		for (size_t i = 0; i < server_list.entries.size(); ++i)
		{
			bool found = false;
			for (size_t j = 0; j < entries.size(); ++j)
			{
				if (entries[j].id == server_list.entries[i].id)
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				entries.push_back(server_list.entries[i]);
			}
		}
		pending = server_list.pending;
	}
	fillList();
}

namespace
{
	wxString lastBackupText(const SServerListEntry& entry)
	{
		if (entry.last_backup <= 0)
			return _("Never");
		return wxDateTime(static_cast<time_t>(entry.last_backup)).Format(wxT("%Y-%m-%d %H:%M"));
	}

	wxStaticText* boldText(wxWindow* parent, const wxString& text)
	{
		wxStaticText* ret = new wxStaticText(parent, wxID_ANY, text);
		wxFont font = ret->GetFont();
		font.SetWeight(wxFONTWEIGHT_BOLD);
		ret->SetFont(font);
		return ret;
	}

	enum EIcon
	{
		IconEdit,
		IconRemove
	};

	//Small pencil/trash can icons, drawn so no image files are needed
	wxBitmap drawIcon(EIcon icon, int size, const wxColour& col)
	{
		wxImage img(size, size);
		img.InitAlpha();
		unsigned char* alpha = img.GetAlpha();
		for (int i = 0; i < size * size; ++i)
			alpha[i] = 0;

		wxGraphicsContext* gc = wxGraphicsContext::Create(img);
		if (gc == NULL)
			return wxBitmap(img);

		gc->Scale(size / 16.0, size / 16.0);
		gc->SetBrush(wxBrush(col));
		gc->SetPen(wxPen(col, 1));

		if (icon == IconEdit)
		{
			//Pencil from top right to bottom left
			wxGraphicsPath body = gc->CreatePath();
			body.MoveToPoint(11, 1.5);
			body.AddLineToPoint(14.5, 5);
			body.AddLineToPoint(6, 13.5);
			body.AddLineToPoint(2.5, 10);
			body.CloseSubpath();
			gc->FillPath(body);
			wxGraphicsPath tip = gc->CreatePath();
			tip.MoveToPoint(2.5, 10);
			tip.AddLineToPoint(6, 13.5);
			tip.AddLineToPoint(1.5, 14.5);
			tip.CloseSubpath();
			gc->FillPath(tip);
		}
		else
		{
			//Trash can: handle, lid, bin with ribs
			gc->SetBrush(*wxTRANSPARENT_BRUSH);
			gc->SetPen(wxPen(col, 1.5));
			gc->StrokeLine(6.5, 1.5, 9.5, 1.5);
			gc->SetBrush(wxBrush(col));
			gc->SetPen(wxPen(col, 1));
			gc->DrawRectangle(2.5, 3, 11, 1.5);
			wxGraphicsPath bin = gc->CreatePath();
			bin.MoveToPoint(4, 5.5);
			bin.AddLineToPoint(12, 5.5);
			bin.AddLineToPoint(11, 15);
			bin.AddLineToPoint(5, 15);
			bin.CloseSubpath();
			gc->SetBrush(*wxTRANSPARENT_BRUSH);
			gc->SetPen(wxPen(col, 1.5));
			gc->StrokePath(bin);
			gc->StrokeLine(6.5, 7.5, 6.5, 13);
			gc->StrokeLine(9.5, 7.5, 9.5, 13);
		}
		delete gc;
		return wxBitmap(img);
	}

	enum
	{
		COL_COUNT = 6
	};
}

bool SPathsModel::isBackedUpTo(const SBackupDir& dir, const std::string& ident) const
{
	//No servers: all servers
	return dir.servers.empty()
		|| std::find(dir.servers.begin(), dir.servers.end(), ident) != dir.servers.end();
}

namespace
{
	enum
	{
		PCOL_PATH = 0,
		PCOL_NAME,
		PCOL_ALSO,
		PCOL_SOURCE
	};
}

ServerPathsPage::ServerPathsPage(wxWindow* parent, SPathsModel* model, const std::string& ident, bool read_only)
	: wxPanel(parent, wxID_ANY), model(model), ident(ident), read_only(read_only), updating(false)
{
	wxBoxSizer* top = new wxBoxSizer(wxVERTICAL);

	m_list = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(330, 110)),
		wxLC_REPORT | wxLC_SINGLE_SEL);
	m_list->InsertColumn(PCOL_PATH, _("Path"), wxLIST_FORMAT_LEFT, wxDLG_UNIT(this, wxSize(130, -1)).GetWidth());
	m_list->InsertColumn(PCOL_NAME, _("Name"), wxLIST_FORMAT_LEFT, wxDLG_UNIT(this, wxSize(55, -1)).GetWidth());
	m_list->InsertColumn(PCOL_ALSO, _("Also backed up to"), wxLIST_FORMAT_LEFT, wxDLG_UNIT(this, wxSize(80, -1)).GetWidth());
	m_list->InsertColumn(PCOL_SOURCE, _("Added"), wxLIST_FORMAT_LEFT, wxDLG_UNIT(this, wxSize(60, -1)).GetWidth());
	top->Add(m_list, 1, wxEXPAND | wxALL, 5);

	wxBoxSizer* bottom = new wxBoxSizer(wxHORIZONTAL);
	m_add = new wxButton(this, wxID_ANY, _("+ Add path"));
	m_remove = new wxButton(this, wxID_ANY, _("Remove"));
	m_remove->SetToolTip(_("Stop backing up the path to this server. Other servers keep backing it up."));
	bottom->Add(m_add, 0, wxRIGHT, 5);
	bottom->Add(m_remove, 0, wxRIGHT, 15);
	bottom->Add(new wxStaticText(this, wxID_ANY, _("Name:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
	m_name = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(this, wxSize(100, -1)));
	m_name->SetValidator(getPathValidator());
	bottom->Add(m_name, 0, wxALIGN_CENTER_VERTICAL);
	top->Add(bottom, 0, wxALL, 5);

	SetSizer(top);

	m_add->Bind(wxEVT_BUTTON, &ServerPathsPage::OnAdd, this);
	m_remove->Bind(wxEVT_BUTTON, &ServerPathsPage::OnRemove, this);
	m_list->Bind(wxEVT_LIST_ITEM_SELECTED, &ServerPathsPage::OnSelect, this);
	m_list->Bind(wxEVT_LIST_ITEM_DESELECTED, &ServerPathsPage::OnSelect, this);
	m_name->Bind(wxEVT_TEXT, &ServerPathsPage::OnName, this);

	refresh();
}

void ServerPathsPage::refresh()
{
	updating = true;
	int sel = selectedDir();
	m_list->DeleteAllItems();
	shown.clear();
	for (size_t i = 0; i < model->dirs.size(); ++i)
	{
		const SBackupDir& dir = model->dirs[i];
		if (!model->isBackedUpTo(dir, ident))
			continue;

		wxString also;
		if (dir.servers.empty())
		{
			also = model->server_idents.size() > 1 ? _("all servers") : wxString(wxT("-"));
		}
		else
		{
			std::vector<std::string> others = dir.servers;
			others.insert(others.end(), dir.also_servers.begin(), dir.also_servers.end());
			for (size_t j = 0; j < others.size(); ++j)
			{
				if (others[j] == ident)
					continue;
				std::map<std::string, wxString>::const_iterator it = model->server_names.find(others[j]);
				if (!also.empty()) also += wxT(", ");
				also += it != model->server_names.end() ? it->second : wxString::FromUTF8(others[j].c_str());
			}
			if (also.empty())
				also = wxT("-");
		}

		long item = m_list->InsertItem(m_list->GetItemCount(), dir.path);
		m_list->SetItem(item, PCOL_NAME, dir.name);
		m_list->SetItem(item, PCOL_ALSO, also);
		m_list->SetItem(item, PCOL_SOURCE, dir.server_default ? _("by a server") : _("on this computer"));
		if (static_cast<int>(i) == sel)
			m_list->SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		shown.push_back(i);
	}
	updating = false;
	updateButtons();
}

int ServerPathsPage::selectedDir()
{
	long item = m_list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (item < 0 || item >= static_cast<long>(shown.size()))
		return -1;
	return static_cast<int>(shown[item]);
}

void ServerPathsPage::updateButtons()
{
	updating = true;
	int sel = selectedDir();
	//Default paths of a server are changed on the server
	bool editable = !read_only && sel >= 0 && model->dirs[sel].server_default == 0;
	m_remove->Enable(editable);
	m_name->Enable(editable);
	m_name->ChangeValue(sel >= 0 ? model->dirs[sel].name : wxString());
	m_add->Enable(!read_only);
	updating = false;
}

void ServerPathsPage::OnSelect(wxListEvent& event)
{
	if (!updating)
		updateButtons();
}

void ServerPathsPage::OnName(wxCommandEvent& event)
{
	int sel = selectedDir();
	if (updating || sel < 0 || model->dirs[sel].server_default != 0)
		return;
	model->dirs[sel].name = m_name->GetValue();
	model->modified = true;
	long item = m_list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (item >= 0)
		m_list->SetItem(item, PCOL_NAME, model->dirs[sel].name);
}

wxString ServerPathsPage::uniqueName(const wxString& path)
{
	wxString base = path;
	while (!base.empty() && (base.Last() == '/' || base.Last() == '\\'))
		base.RemoveLast();
	base = base.AfterLast('/').AfterLast('\\');
	wxString clean;
	for (size_t i = 0; i < base.size(); ++i)
	{
		wxChar ch = base[i];
		if (ch != '*' && ch != ':' && ch != '/' && ch != '\\' && ch != ' ' && ch != '?'
			&& ch != '"' && ch != '<' && ch != '>' && ch != '|')
			clean += ch;
	}
	if (clean.empty())
		clean = wxT("root");

	wxString name = clean;
	for (int k = 0; k < 100; ++k)
	{
		bool used = false;
		for (size_t i = 0; i < model->dirs.size(); ++i)
		{
			if (model->dirs[i].name.CmpNoCase(name) == 0)
				used = true;
		}
		if (!used)
			return name;
		name = clean + wxString::Format(wxT("_%d"), k);
	}
	return name;
}

void ServerPathsPage::OnAdd(wxCommandEvent& event)
{
	wxDirDialog dlg(this, _("Please select the directory that will be backed up."), wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if (dlg.ShowModal() != wxID_OK)
		return;

	wxString path = dlg.GetPath();
	for (size_t i = 0; i < model->dirs.size(); ++i)
	{
		SBackupDir& dir = model->dirs[i];
		if (dir.path != path)
			continue;
		if (model->isBackedUpTo(dir, ident))
		{
			wxMessageBox(_("This path is already backed up to this server."), wxT("UrBackup"), wxOK | wxICON_INFORMATION, this);
			return;
		}
		if (dir.server_default != 0)
		{
			wxMessageBox(_("This is a default path of another server. It can only be changed on that server."), wxT("UrBackup"), wxOK | wxICON_INFORMATION, this);
			return;
		}
		//Already backed up to other servers: back it up to this one too
		dir.servers.push_back(ident);
		dir.servers_from_client = true;
		model->modified = true;
		refresh();
		return;
	}

	SBackupDir dir;
	dir.path = path;
	dir.name = uniqueName(path);
	dir.id = 0;
	dir.group = 0;
	dir.server_default = 0;
	//Only to this server. Add it on the pages of other servers to back it up there too
	if (model->server_idents.size() > 1)
	{
		dir.servers.push_back(ident);
		dir.servers_from_client = true;
	}
	else
	{
		dir.servers_from_client = false;
	}
	model->dirs.push_back(dir);
	model->modified = true;
	refresh();
}

void ServerPathsPage::OnRemove(wxCommandEvent& event)
{
	int sel = selectedDir();
	if (sel < 0 || model->dirs[sel].server_default != 0)
		return;

	SBackupDir& dir = model->dirs[sel];
	if (dir.servers.empty())
	{
		//All servers: all others from now on
		for (size_t i = 0; i < model->server_idents.size(); ++i)
		{
			if (model->server_idents[i] != ident)
				dir.servers.push_back(model->server_idents[i]);
		}
	}
	else
	{
		dir.servers.erase(std::remove(dir.servers.begin(), dir.servers.end(), ident), dir.servers.end());
	}
	dir.servers_from_client = true;

	if (dir.servers.empty())
	{
		//No server backs it up anymore
		model->dirs.erase(model->dirs.begin() + sel);
	}
	model->modified = true;
	refresh();
}

#ifdef _WIN32
ServerComponentsPage::ServerComponentsPage(wxWindow* parent, const std::string& ident, int facet_id, bool allow_restore)
	: wxPanel(parent, wxID_ANY), ident(ident), facet_id(facet_id), components(NULL)
{
	wxBoxSizer* top = new wxBoxSizer(wxVERTICAL);
	m_loading = new wxStaticText(this, wxID_ANY, _("Loading Windows components..."));
	top->Add(m_loading, 0, wxALL, 5);
	if (allow_restore)
	{
		wxButton* restore = new wxButton(this, wxID_ANY, _("Restore components..."));
		restore->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { Settings::openRestoreComponents(this); });
		top->Add(restore, 0, wxALL, 5);
	}
	SetSizer(top);
}

ServerComponentsPage::~ServerComponentsPage()
{
	//The tree is still a child of this page here
	delete components;
}

void ServerComponentsPage::load()
{
	if (components != NULL)
		return;

	initCom();
	//The dialog stays hidden; its tree is shown on this page
	components = new SelectWindowsComponents(NULL, ident, facet_id);
	wxWindow* tree = components->takeTree(this);
	GetSizer()->Insert(1, tree, 1, wxEXPAND | wxALL, 5);
	m_loading->Hide();
	Layout();
}

void ServerComponentsPage::save()
{
	if (components != NULL)
		components->save();
}
#endif

ServerLogsPage::ServerLogsPage(wxWindow* parent, const std::string& ident)
	: wxPanel(parent, wxID_ANY), ident(ident), loaded(false)
{
	wxBoxSizer* top = new wxBoxSizer(wxHORIZONTAL);
	m_list = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(150, 150)));
	top->Add(m_list, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* right = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* filter = new wxBoxSizer(wxHORIZONTAL);
	filter->AddStretchSpacer();
	filter->Add(new wxStaticText(this, wxID_ANY, _("Filter:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
	wxString levels[] = { _("Infos"), _("Warnings"), _("Errors") };
	m_level = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, levels);
	m_level->SetSelection(0);
	filter->Add(m_level, 0);
	right->Add(filter, 0, wxEXPAND | wxALL, 5);
	m_text = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(this, wxSize(260, 150)),
		wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxTE_DONTWRAP);
	right->Add(m_text, 1, wxEXPAND | wxALL, 5);
	top->Add(right, 1, wxEXPAND);

	SetSizer(top);

	m_list->Bind(wxEVT_LISTBOX, [this](wxCommandEvent&) { showLog(); });
	m_level->Bind(wxEVT_CHOICE, [this](wxCommandEvent&) { showLog(); });
}

void ServerLogsPage::load()
{
	if (loaded)
		return;
	loaded = true;

	entries = Connector::getLogEntries(ident);
	for (size_t i = 0; i < entries.size(); ++i)
	{
		wxLongLong_t logtime;
		wxString label;
		if (!entries[i].logtime.empty() && entries[i].logtime.ToLongLong(&logtime))
		{
			label = wxDateTime((wxLongLong)(logtime * 1000)).Format(wxT("%Y-%m-%d %H:%M"));
		}
		if (!entries[i].server_known)
		{
			label += wxT(" ") + _("(server unknown)");
		}
		m_list->Append(label);
	}
	if (entries.empty())
	{
		m_text->SetValue(_("No logs of this server yet."));
	}
}

void ServerLogsPage::showLog()
{
	int sel = m_list->GetSelection();
	if (sel < 0 || sel >= static_cast<int>(entries.size()))
		return;
	std::vector<SLogLine> data = Connector::getLogdata(entries[sel].logid, m_level->GetSelection(), true);

	//Time of each line, warnings and errors marked and coloured
	m_text->Freeze();
	m_text->Clear();
	wxTextAttr normal(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	wxTextAttr time_attr(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
	wxTextAttr warning(wxColour(176, 112, 0));
	wxTextAttr error(wxColour(192, 0, 0));
	//Only the time, unless the log goes over more than one day (e.g. a backup past midnight)
	wxString time_format = wxT("%H:%M:%S  ");
	wxString first_date;
	for (size_t i = 0; i < data.size(); ++i)
	{
		if (data[i].ltime <= 0)
			continue;
		wxString date = wxDateTime(static_cast<time_t>(data[i].ltime)).Format(wxT("%Y-%m-%d"));
		if (first_date.empty())
			first_date = date;
		else if (date != first_date)
		{
			time_format = wxT("%Y-%m-%d %H:%M:%S  ");
			break;
		}
	}
	for (size_t i = 0; i < data.size(); ++i)
	{
		if (data[i].ltime > 0)
		{
			m_text->SetDefaultStyle(time_attr);
			m_text->AppendText(wxDateTime(static_cast<time_t>(data[i].ltime)).Format(time_format));
		}
		if (data[i].loglevel >= 2)
		{
			m_text->SetDefaultStyle(error);
			m_text->AppendText(_("ERROR:") + wxT(" "));
		}
		else if (data[i].loglevel == 1)
		{
			m_text->SetDefaultStyle(warning);
			m_text->AppendText(_("WARNING:") + wxT(" "));
		}
		else
		{
			m_text->SetDefaultStyle(normal);
		}
		m_text->AppendText(data[i].msg + wxT("\n"));
	}
	m_text->SetDefaultStyle(normal);
	m_text->SetInsertionPoint(0);
	m_text->Thaw();
}

ClientSettingsDialog::ClientSettingsDialog(wxWindow* parent, const SServerList& server_list, int capa)
	: wxDialog(parent, wxID_ANY, _("Settings"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
	entries(server_list.entries), pending(server_list.pending), primary(server_list.primary), modified(false), capa(capa),
	selected(-1), current_pages(NULL), paths_loaded(false)
{
	wxBoxSizer* top = new wxBoxSizer(wxVERTICAL);

	//The computer name is the same for all servers
	wxBoxSizer* name_row = new wxBoxSizer(wxHORIZONTAL);
	name_row->Add(new wxStaticText(this, wxID_ANY, _("Computer name:")), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	computername_orig = Settings::currentComputerName();
	m_computername = new wxTextCtrl(this, wxID_ANY, computername_orig, wxDefaultPosition, wxDLG_UNIT(this, wxSize(120, -1)));
	m_computername->SetValidator(getPathValidator());
	name_row->Add(m_computername, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	top->Add(name_row, 0, wxALL, 5);

	wxBoxSizer* servers_header = new wxBoxSizer(wxHORIZONTAL);
	servers_header->Add(boldText(this, _("Servers")), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	servers_header->AddStretchSpacer();
	m_trust = new wxButton(this, wxID_ANY, _("Trust new server..."));
	servers_header->Add(m_trust, 0, wxALL, 5);
	wxButton* add = new wxButton(this, wxID_ANY, _("+ Add server"));
	servers_header->Add(add, 0, wxALL, 5);
	top->Add(servers_header, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);

	//The servers. A click on a row shows its backup settings below
	m_rows = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME);
	m_rows->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	top->Add(m_rows, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

	m_settings_heading = boldText(this, wxEmptyString);
	top->Add(m_settings_heading, 0, wxLEFT | wxRIGHT | wxTOP, 10);
	m_notice = new wxStaticText(this, wxID_ANY, wxEmptyString);
	m_notice->SetForegroundColour(wxColour(192, 0, 0));
	top->Add(m_notice, 0, wxLEFT | wxRIGHT | wxTOP, 10);
	m_pages = new wxPanel(this, wxID_ANY);
	m_pages->SetSizer(new wxBoxSizer(wxVERTICAL));
	top->Add(m_pages, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);

	//The other configuration windows (opened in this process, so without another elevation)
	wxBoxSizer* bottom = new wxBoxSizer(wxHORIZONTAL);
	bottom->AddStretchSpacer();
	wxButton* ok = new wxButton(this, wxID_OK, _("Ok"));
	wxButton* cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
	bottom->Add(ok, 0, wxALL, 5);
	bottom->Add(cancel, 0, wxALL, 5);
	top->Add(bottom, 0, wxEXPAND | wxALL, 5);

	SetSizer(top);

	add->Bind(wxEVT_BUTTON, &ClientSettingsDialog::OnAdd, this);
	m_trust->Bind(wxEVT_BUTTON, &ClientSettingsDialog::OnTrust, this);
	ok->Bind(wxEVT_BUTTON, &ClientSettingsDialog::OnOk, this);
	cancel->Bind(wxEVT_BUTTON, &ClientSettingsDialog::OnCancel, this);
	Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent&) { wxCommandEvent evt; OnCancel(evt); });

	//Start with the primary server's settings
	int sel = entries.empty() ? -1 : 0;
	for (size_t i = 0; i < entries.size(); ++i)
	{
		if (!primary.empty() && entries[i].ident == primary)
			sel = static_cast<int>(i);
	}
	fillServers();
	selectServer(sel);
	Centre();
}

ClientSettingsDialog::~ClientSettingsDialog()
{
	//Their pages are in this window: delete them while the pages still exist
	for (std::map<std::string, Settings*>::iterator it = server_settings.begin(); it != server_settings.end(); ++it)
	{
		delete it->second;
	}
	server_settings.clear();
}

void ClientSettingsDialog::fillServers()
{
	m_rows->DestroyChildren();
	row_panels.clear();
	row_texts.clear();

	wxBoxSizer* rows = new wxBoxSizer(wxVERTICAL);
	wxColour icon_col = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);
	int icon_size = wxDLG_UNIT(m_rows, wxSize(0, 8)).GetHeight();
	wxBitmap edit_icon = drawIcon(IconEdit, icon_size, icon_col);
	wxBitmap remove_icon = drawIcon(IconRemove, icon_size, icon_col);

	//Header and rows; the columns are aligned below
	std::vector<std::vector<wxWindow*> > cells;
	std::vector<wxBoxSizer*> row_sizers;
	{
		wxPanel* header = new wxPanel(m_rows, wxID_ANY);
		wxBoxSizer* hs = new wxBoxSizer(wxHORIZONTAL);
		std::vector<wxWindow*> c;
		const wxString titles[COL_COUNT] = { _("Name"), _("Identity"), _("Local"), _("Internet"), _("Status"), _("Last backup") };
		for (int i = 0; i < COL_COUNT; ++i)
		{
			c.push_back(boldText(header, titles[i]));
			hs->Add(c.back(), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
		}
		header->SetSizer(hs);
		rows->Add(header, 0, wxEXPAND);
		cells.push_back(c);
		row_sizers.push_back(hs);
	}

	for (size_t i = 0; i < entries.size(); ++i)
	{
		const SServerListEntry& e = entries[i];
		wxPanel* row = new wxPanel(m_rows, wxID_ANY);
		wxBoxSizer* rs = new wxBoxSizer(wxHORIZONTAL);
		wxString name = ServersPanel::displayName(e);
		std::vector<wxWindow*> c;
		c.push_back(new wxStaticText(row, wxID_ANY, name));
		c.push_back(new wxStaticText(row, wxID_ANY, fingerprintText(e)));
		if (!e.fingerprint.empty())
			c.back()->SetToolTip(wxString::FromUTF8(e.fingerprint.c_str()));
		c.push_back(new wxStaticText(row, wxID_ANY, e.local ? wxString::FromUTF8("\xE2\x9C\x93") : wxString(), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL | wxST_NO_AUTORESIZE));
		c.push_back(new wxStaticText(row, wxID_ANY, e.internet ? wxString::FromUTF8("\xE2\x9C\x93") : wxString(), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL | wxST_NO_AUTORESIZE));
		c.push_back(new wxStaticText(row, wxID_ANY, statusText(e)));
		c.push_back(new wxStaticText(row, wxID_ANY, e.ident.empty() ? wxString(wxT("-")) : lastBackupText(e)));
		for (int j = 0; j < COL_COUNT; ++j)
		{
			rs->Add(c[j], 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
		}

		wxBitmapButton* edit = new wxBitmapButton(row, wxID_ANY, edit_icon);
		edit->SetToolTip(_("Edit server"));
		wxBitmapButton* remove = new wxBitmapButton(row, wxID_ANY, remove_icon);
		remove->SetToolTip(_("Remove server"));
		rs->Add(edit, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxTOP | wxBOTTOM, 3);
		rs->Add(remove, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
		row->SetSizer(rs);
		rows->Add(row, 0, wxEXPAND);

		int idx = static_cast<int>(i);
		edit->Bind(wxEVT_BUTTON, [this, idx](wxCommandEvent&) { editServer(idx); });
		remove->Bind(wxEVT_BUTTON, [this, idx](wxCommandEvent&) { removeServer(idx); });
		//Click anywhere in the row to select it
		row->Bind(wxEVT_LEFT_DOWN, [this, idx](wxMouseEvent&) { selectServer(idx); });
		for (int j = 0; j < COL_COUNT; ++j)
		{
			c[j]->Bind(wxEVT_LEFT_DOWN, [this, idx](wxMouseEvent&) { selectServer(idx); });
		}

		row_panels.push_back(row);
		row_texts.push_back(c);
		cells.push_back(c);
		row_sizers.push_back(rs);
	}

	if (entries.empty())
	{
		rows->Add(new wxStaticText(m_rows, wxID_ANY, _("No servers yet. Add one, or wait for a server in the local network to find this computer.")), 0, wxALL, 5);
	}

	//Same width for the cells of a column
	for (int j = 0; j < COL_COUNT; ++j)
	{
		int width = 0;
		for (size_t i = 0; i < cells.size(); ++i)
			width = (std::max)(width, cells[i][j]->GetBestSize().GetWidth());
		for (size_t i = 0; i < cells.size(); ++i)
			cells[i][j]->SetMinSize(wxSize(width, -1));
	}

	m_rows->SetSizer(rows, true);
	m_trust->Show(!pending.empty());

	if (selected >= static_cast<int>(entries.size()))
		selected = entries.empty() ? -1 : 0;
	highlightSelected();

	m_rows->Layout();
	GetSizer()->Layout();
	GetSizer()->Fit(this);
}

void ClientSettingsDialog::highlightSelected()
{
	for (size_t i = 0; i < row_panels.size(); ++i)
	{
		bool sel = static_cast<int>(i) == selected;
		wxColour bg = wxSystemSettings::GetColour(sel ? wxSYS_COLOUR_HIGHLIGHT : wxSYS_COLOUR_WINDOW);
		wxColour fg = wxSystemSettings::GetColour(sel ? wxSYS_COLOUR_HIGHLIGHTTEXT : wxSYS_COLOUR_WINDOWTEXT);
		row_panels[i]->SetBackgroundColour(bg);
		for (size_t j = 0; j < row_texts[i].size(); ++j)
		{
			row_texts[i][j]->SetBackgroundColour(bg);
			row_texts[i][j]->SetForegroundColour(fg);
		}
		row_panels[i]->Refresh();
	}
}

void ClientSettingsDialog::selectServer(int idx)
{
	selected = idx;
	highlightSelected();

	//Stay on the same page (e.g. "Paths") when switching to another server
	wxString page_text;
	if (current_pages != NULL)
	{
		int page = current_pages->GetSelection();
		if (page >= 0)
			page_text = current_pages->GetPageText(page);
		current_pages->Hide();
		current_pages = NULL;
	}

	m_notice->SetLabel(wxEmptyString);
	m_notice->Hide();

	if (idx < 0 || idx >= static_cast<int>(entries.size()))
	{
		m_settings_heading->SetLabel(wxEmptyString);
	}
	else if (entries[idx].ident.empty())
	{
		m_settings_heading->SetLabel(wxString::Format(_("Backup settings of %s"), ServersPanel::displayName(entries[idx])));
		m_notice->SetLabel(_("The backup settings are available once the server has connected to this client."));
		m_notice->Show();
	}
	else
	{
		const std::string& ident = entries[idx].ident;
		m_settings_heading->SetLabel(wxString::Format(_("Backup settings of %s"), ServersPanel::displayName(entries[idx])));

		std::map<std::string, Settings*>::iterator it = server_settings.find(ident);
		Settings* s;
		if (it == server_settings.end())
		{
			//The settings dialog of the server stays hidden, its pages are shown here
			int server_capa = entries[idx].capa >= 0 ? entries[idx].capa : capa;
			GUISettings::capa_override = server_capa;
			s = new Settings(NULL, ident, true, true);
			GUISettings::capa_override = -1;
			server_settings[ident] = s;
			wxNotebook* pages = s->takePages(m_pages);
			m_pages->GetSizer()->Add(pages, 1, wxEXPAND | wxALL, 5);
			server_pages[ident] = pages;

			//The paths backed up to this server, after the backup type pages
			if (!paths_loaded)
			{
				paths.dirs = Connector::getSharedPaths();
				paths_loaded = true;
			}
			paths.server_idents.clear();
			for (size_t i = 0; i < entries.size(); ++i)
			{
				if (entries[i].ident.empty())
					continue;
				paths.server_idents.push_back(entries[i].ident);
				paths.server_names[entries[i].ident] = ServersPanel::displayName(entries[i]);
			}
			ServerPathsPage* paths_page = new ServerPathsPage(pages, &paths, ident,
				MyTimer::hasCapability(DONT_ALLOW_CONFIG_PATHS, server_capa));
			size_t pos = 0;
			while (pos < pages->GetPageCount()
				&& (pages->GetPageText(pos) == _("File backups") || pages->GetPageText(pos) == _("Image backups")))
			{
				++pos;
			}
			pages->InsertPage(pos, paths_page, _("Paths"));
			paths_pages[ident] = paths_page;

#ifdef _WIN32
			if (!MyTimer::hasCapability(DONT_ALLOW_COMPONENT_CONFIG, server_capa))
			{
				ServerComponentsPage* components_page = new ServerComponentsPage(pages, ident, entries[idx].id,
					!MyTimer::hasCapability(DONT_ALLOW_COMPONENT_RESTORE, server_capa)
					&& !MyTimer::hasCapability(STATUS_NO_COMPONENTS, server_capa));
				pages->InsertPage(pos + 1, components_page, _("Components"));
				components_pages.push_back(components_page);
			}
#endif
			if (!MyTimer::hasCapability(DONT_SHOW_LOGS, server_capa))
			{
				pages->AddPage(new ServerLogsPage(pages, ident), _("Logs"));
			}

			//Pages that take a while to fill are filled when they are shown first
			pages->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, [pages](wxBookCtrlEvent& evt) {
				if (evt.GetSelection() >= 0)
				{
					ServerLogsPage* lp = dynamic_cast<ServerLogsPage*>(pages->GetPage(evt.GetSelection()));
					if (lp != NULL)
						lp->load();
				}
#ifdef _WIN32
				if (evt.GetSelection() >= 0)
				{
					ServerComponentsPage* cp = dynamic_cast<ServerComponentsPage*>(pages->GetPage(evt.GetSelection()));
					if (cp != NULL)
						cp->load();
				}
#endif
				evt.Skip();
			});

			//The server does not do file backups: its file backup settings and paths do not matter
			if (MyTimer::hasCapability(DONT_DO_FILE_BACKUPS, server_capa))
			{
				for (size_t i = pages->GetPageCount(); i-- > 0;)
				{
					if (pages->GetPageText(i) == _("File backups") || pages->GetPageText(i) == _("Paths"))
					{
						pages->GetPage(i)->Hide();
						pages->RemovePage(i);
					}
				}
			}
		}
		else
		{
			s = it->second;
		}

		wxString notice;
		if (s->settingsNotReceived())
		{
			notice = _("This server has not sent its settings to this client yet, so the values below may be "
				"those of another server. They arrive when the client's settings are saved on the server "
				"or the server restarts.");
		}
		//What the server is configured for (sent when it connects)
		int server_capa = entries[idx].capa;
		if (server_capa >= 0)
		{
			if (MyTimer::hasCapability(DONT_DO_FILE_BACKUPS, server_capa))
			{
				if (!notice.empty()) notice += wxT("\n");
				notice += _("This server does not do file backups (set on the server).");
			}
			if (MyTimer::hasCapability(DONT_DO_IMAGE_BACKUPS, server_capa))
			{
				if (!notice.empty()) notice += wxT("\n");
				notice += entries[idx].internet && !entries[idx].local
					? _("This server does not do image backups via internet (set on the server).")
					: _("This server does not do image backups (set on the server).");
			}
		}
		if (!notice.empty())
		{
			m_notice->SetLabel(notice);
			m_notice->Wrap(wxDLG_UNIT(this, wxSize(330, -1)).GetWidth());
			m_notice->Show();
		}

		current_pages = server_pages[ident];
		if (!page_text.empty())
		{
			for (size_t i = 0; i < current_pages->GetPageCount(); ++i)
			{
				if (current_pages->GetPageText(i) == page_text)
				{
					current_pages->SetSelection(i);
					break;
				}
			}
		}
		current_pages->Show();
		//The paths may have been changed on the page of another server
		paths_pages[ident]->refresh();
	}

	m_pages->Layout();
	GetSizer()->Layout();
	GetSizer()->Fit(this);
}

void ClientSettingsDialog::editServer(int idx)
{
	if (idx < 0 || idx >= static_cast<int>(entries.size()))
		return;
	ServerEditDialog dlg(this, entries[idx]);
	if (dlg.ShowModal() == wxID_OK)
	{
		entries[idx] = dlg.getEntry();
		modified = true;
		CallAfter([this, idx]() { fillServers(); selectServer(idx); });
	}
}

void ClientSettingsDialog::removeServer(int idx)
{
	if (idx < 0 || idx >= static_cast<int>(entries.size()))
		return;
	wxString msg = wxString::Format(_("Remove server \"%s\"? The client will no longer trust it, "
		"so it cannot back up this computer anymore until it is added again."), ServersPanel::displayName(entries[idx]));
	if (wxMessageBox(msg, _("Remove server"), wxYES_NO | wxICON_QUESTION, this) != wxYES)
		return;
	entries.erase(entries.begin() + idx);
	modified = true;
	//The row (and the clicked button) is destroyed by fillServers
	CallAfter([this]() { fillServers(); selectServer(entries.empty() ? -1 : 0); });
}

void ClientSettingsDialog::OnAdd(wxCommandEvent& event)
{
	SServerListEntry entry;
	entry.id = -1;
	entry.local = false;
	entry.internet = true;
	ServerEditDialog dlg(this, entry);
	if (dlg.ShowModal() == wxID_OK)
	{
		entries.push_back(dlg.getEntry());
		modified = true;
		fillServers();
		selectServer(static_cast<int>(entries.size()) - 1);
	}
}

void ClientSettingsDialog::OnTrust(wxCommandEvent& event)
{
	if (pending.empty())
		return;

	wxArrayString choices;
	for (size_t i = 0; i < pending.size(); ++i)
	{
		choices.Add(wxString::FromUTF8(pending[i].c_str()));
	}

	int idx = wxGetSingleChoiceIndex(_("These servers tried to back up this computer, but are not trusted. "
		"Only trust a server you know."), _("Trust new server"), choices, this);
	if (idx < 0)
		return;

	if (!Connector::addNewServer(pending[idx]))
	{
		wxMessageBox(_("Trusting the server failed."), _("Trust new server"), wxOK | wxICON_ERROR, this);
		return;
	}

	SServerList server_list = Connector::getServerList();
	if (server_list.supported)
	{
		//Keep local edits, add the newly trusted server
		for (size_t i = 0; i < server_list.entries.size(); ++i)
		{
			bool found = false;
			for (size_t j = 0; j < entries.size(); ++j)
			{
				if (entries[j].id == server_list.entries[i].id)
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				entries.push_back(server_list.entries[i]);
			}
		}
		pending = server_list.pending;
	}
	CallAfter([this]() { fillServers(); selectServer(selected); });
}

void ClientSettingsDialog::OnOk(wxCommandEvent& event)
{
	//Backup settings of the servers that were shown (and are still in the list)
	for (size_t i = 0; i < entries.size(); ++i)
	{
		std::map<std::string, Settings*>::iterator it = server_settings.find(entries[i].ident);
		if (entries[i].ident.empty() || it == server_settings.end())
			continue;
		if (!it->second->save())
		{
			//Show the server with the value that is not valid
			selectServer(static_cast<int>(i));
			return;
		}
	}

#ifdef _WIN32
	for (size_t i = 0; i < components_pages.size(); ++i)
	{
		components_pages[i]->save();
	}
#endif

	if (paths.modified
		&& !Connector::saveSharedPaths(paths.dirs))
	{
		wxMessageBox(_("Saving the changed paths to backup failed"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR, this);
		return;
	}

	if (!m_computername->GetValue().empty()
		&& m_computername->GetValue() != computername_orig)
	{
		//The settings of the default facet (the primary server's) have the computer name
		if (!Connector::updateSettings("computername=" + std::string(m_computername->GetValue().ToUTF8()) + "\n", 5000, primary))
		{
			wxMessageBox(_("Saving the computer name failed."), wxT("UrBackup"), wxOK | wxICON_ERROR, this);
			return;
		}
	}

	if (modified
		&& !Connector::setServerList(entries))
	{
		wxMessageBox(_("Saving the server list failed."), wxT("UrBackup"), wxOK | wxICON_ERROR, this);
		return;
	}

	EndModal(wxID_OK);
}

void ClientSettingsDialog::OnCancel(wxCommandEvent& event)
{
	if ((modified || paths.modified)
		&& wxMessageBox(_("Discard the changes to the servers and backup paths?"), wxT("UrBackup"), wxYES_NO | wxICON_QUESTION, this) != wxYES)
	{
		return;
	}
	EndModal(wxID_CANCEL);
}
