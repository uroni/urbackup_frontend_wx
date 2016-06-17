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

#include "Info.h"
#include "stringtools.h"
#include "TrayIcon.h"

extern std::string g_lang;
extern std::string g_res_path;

#ifdef wxUSE_WCHAR_T
std::wstring ConvertToUnicode(const std::string &str);
#else
#define ConvertToUnicode(x)
#endif

Info* Info::instance;


Info::Info(wxWindow* parent) : GUIInfo(parent)
{
	std::string inf=getFile(g_res_path+"info_"+g_lang+".txt");
	if(inf.empty())
	{
		inf=getFile(g_res_path+"info.txt");
	}
	m_textCtrl14->SetValue(ConvertToUnicode(inf));
	Show(true);

	std::string n_version = getFile("version.txt");
	std::string c_version = getFile("curr_version.txt");
	if (n_version.empty())n_version = "0";
	if (c_version.empty())c_version = "0";

	if (atoi(n_version.c_str()) > atoi(c_version.c_str()))
	{
		wxButton* updateButton = new wxButton(this, wxID_ANY, _("Update available. Update now."));
		m_versionSizer->Add(updateButton);

		updateButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Info::OnUpdateClick), NULL, this);

		Layout();
	}

	if (FileExists("urbctctl.exe"))
	{
		wxButton* cbtStatusButton = new wxButton(this, wxID_ANY, _("Show CBT status"));
		m_versionSizer->Add(cbtStatusButton);

		cbtStatusButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Info::OnShowCBTStatusClick), NULL, this);

		Layout();
	}

	instance=this;
}

void Info::OnOKClick( wxCommandEvent& event )
{
	Close();
}

void update_urbackup();

void Info::OnUpdateClick(wxCommandEvent & event)
{
	update_urbackup();
}

void Info::OnShowCBTStatusClick(wxCommandEvent& event)
{
	runCommand("cbt-status");
}

Info* Info::getInstance()
{
	return instance;
}

void Info::OnClose()
{
	instance=NULL;
	Destroy();
}
