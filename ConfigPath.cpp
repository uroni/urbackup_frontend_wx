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

#include "ConfigPath.h"

#define CP_ID_OK 100

ConfigPath::ConfigPath(void)
: wxFrame(NULL, wxID_ANY, _("Pfade hinzufügen und entfernen"), wxDefaultPosition, wxSize(500, 600))

{
	SetIcon(wxIcon(wxT("backup-ok-big.ico"), wxBITMAP_TYPE_ICO));
	wxPanel *panel = new wxPanel(this, wxID_ANY);
	listbox=new wxListBox(panel, wxID_ANY, wxPoint(17,5), wxSize(450,500));
	wxButton *btnOk=new wxButton(panel, wxID_ANY, _("Ok"), wxPoint(17,517), wxSize(60,30) );
	wxButton *btnAbort=new wxButton(panel, wxID_ANY, _("Abbrechen"), wxPoint(85,517), wxSize(70,30) );
	wxButton *btnNew=new wxButton(panel, wxID_ANY, _("Neuer Pfad"), wxPoint(286,517), wxSize(80,30) );
	wxButton *btnDel=new wxButton(panel, wxID_ANY, _("Pfad entfernen"), wxPoint(377,517), wxSize(90,30) );

	btnOk->Connect(wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ConfigPath::OnClickOk, NULL, this);
	btnAbort->Connect(wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ConfigPath::OnClickAbort, NULL, this);
	btnNew->Connect(wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ConfigPath::OnClickNew, NULL, this);
	btnDel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ConfigPath::OnClickDel, NULL, this);

	dirs=Connector::getSharedPaths();

	if(Connector::hasError())
	{
		wxMessageBox(_("Ein Fehler ist aufgetreten. Backups werden momentan nicht durchgeführt."), wxT("UrBackup"), wxOK|wxICON_ERROR);
		Hide();
		Close();
	}


	for(size_t i=0;i<dirs.size();++i)
	{
		listbox->Append(dirs[i].path);
	}

	Centre();
	Show(true);
}

void ConfigPath::OnClickOk(wxCommandEvent &evt)
{
	Connector::saveSharedPaths(dirs);
	Close();
}

void ConfigPath::OnClickAbort(wxCommandEvent &evt)
{
	Close();
}

void ConfigPath::OnClickNew(wxCommandEvent &evt)
{
	wxDirDialog ed(this, _("Bitte Verzeichnis das gesichert werden soll auswählen"), wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST );
	int rc=ed.ShowModal();
	if(rc==wxID_OK)
	{
		listbox->Append(ed.GetPath() );
		SBackupDir ad;
		ad.path=ed.GetPath();
		dirs.push_back(ad);
	}
}

void ConfigPath::OnClickDel(wxCommandEvent &evt)
{
	int sel=listbox->GetSelection();
	if(sel>=0)
	{
		listbox->Delete(sel);
		dirs.erase(dirs.begin()+sel);
	}
}