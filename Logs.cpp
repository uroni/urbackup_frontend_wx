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

#include "Logs.h"

Logs::Logs(wxWindow* parent) : GUILogfiles(parent)
{
	SetIcon(wxIcon(wxT("backup-ok-big.ico"), wxBITMAP_TYPE_ICO));
	logentries=Connector::getLogEntries();

	if(Connector::hasError())
	{
		wxMessageBox(_("Ein Fehler ist aufgetreten. Backups werden momentan nicht durchgeführt."), wxT("UrBackup"), wxOK|wxICON_ERROR);
		Hide();
		Close();
		return;
	}

	for(size_t i=0;i<logentries.size();++i)
	{
		m_listBox1->Append(logentries[i].logtime);
	}

	Show(true);
}

void Logs::OnLogEntrySelect( wxCommandEvent& event )
{
	int sel=m_listBox1->GetSelection();
	if(sel>=0)
	{
		std::vector<SLogLine> data=Connector::getLogdata(logentries[sel].logid, m_choice1->GetSelection());
		wxString msg;
		for(size_t i=0;i<data.size();++i)
		{
			msg+=data[i].msg;
			msg+=wxT("\r\n");
		}
		m_textCtrl3->SetValue(msg);
	}
}

void Logs::OnLoglevelChange( wxCommandEvent& event )
{
	int sel=m_listBox1->GetSelection();
	if(sel>=0)
	{
		std::vector<SLogLine> data=Connector::getLogdata(logentries[sel].logid, m_choice1->GetSelection());
		wxString msg;
		for(size_t i=0;i<data.size();++i)
		{
			msg+=data[i].msg;
			msg+=wxT("\r\n");
		}
		m_textCtrl3->SetValue(msg);
	}
}

void Logs::OnExitClick( wxCommandEvent& event )
{
	Close();
}
