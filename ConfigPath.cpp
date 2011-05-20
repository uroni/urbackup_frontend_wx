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
#include "stringtools.h"

#define CP_ID_OK 100

std::string ConvertToUTF8(const std::wstring &input);
wxTextValidator getPathValidator(void);

ConfigPath::ConfigPath(wxWindow* parent)
: GUIConfigPath(parent)

{
	mod=false;
	SetIcon(wxIcon(wxT("backup-ok-big.ico"), wxBITMAP_TYPE_ICO));

	dirs=Connector::getSharedPaths();

	if(Connector::hasError())
	{
		wxMessageBox(_("Ein Fehler ist aufgetreten. Backups werden momentan nicht durchgeführt."), wxT("UrBackup"), wxOK|wxICON_ERROR);
		Hide();
		Close();
		return;
	}


	for(size_t i=0;i<dirs.size();++i)
	{
		listbox->Append(dirs[i].path);
		if(dirs[i].name.IsEmpty())
		{
			dirs[i].name=getDefaultDirname(wnarrow(dirs[i].path.wc_str()));
		}
	}

	m_textCtrl18->SetValidator(getPathValidator());

	Centre();
	Show(true);
}

void ConfigPath::OnClickOk(wxCommandEvent &evt)
{
	if(mod)
	{
		Connector::saveSharedPaths(dirs);
	}
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
		ad.name=getDefaultDirname(wnarrow(ad.path.wc_str()));
		dirs.push_back(ad);
		mod=true;
	}
}

void ConfigPath::OnClickDel(wxCommandEvent &evt)
{
	int sel=listbox->GetSelection();
	if(sel>=0)
	{
		listbox->Delete(sel);
		dirs.erase(dirs.begin()+sel);
		mod=true;
	}
}

std::string replaceChars(std::string in)
{
	char legalchars[] = {'_', '-'};
	for(size_t i=0;i<in.size();++i)
	{
		bool found=false;
		for(size_t j=0;j<sizeof(legalchars);++j)
		{
			if(legalchars[j]==in[i])
			{
				found=true;
				break;
			}
		}
		if( !isletter(in[i]) && !isnumber(in[i]) && !found )
		{
			in[i]='_';
		}
	}
	return in;
}

bool ConfigPath::findPathName(const std::string &pn)
{
	for(size_t i=0;i<dirs.size();++i)
	{
		if(dirs[i].name==pn)
		{
			return true;
		}
	}
	return false;
}

std::string ConfigPath::getDefaultDirname(const std::string &path)
{
	std::string dirname=ExtractFileName(path);

	if(findPathName(dirname) )
	{
		for(int k=0;k<100;++k)
		{
			if(!findPathName(dirname+"_"+nconvert(k)) )
			{
				dirname=dirname+"_"+nconvert(k);
				break;
			}
		}
	}

	return replaceChars(dirname);
}

void ConfigPath::OnPathSelected(wxCommandEvent &evt)
{
	int sel=listbox->GetSelection();
	if(sel>=0)
	{
		m_textCtrl18->Enable();
		m_textCtrl18->SetValue(dirs[sel].name);
	}
}

void ConfigPath::OnNameTextChange(wxCommandEvent &evt)
{
	int sel=listbox->GetSelection();
	if(sel>=0)
	{
		dirs[sel].name=m_textCtrl18->GetValue();
		mod=true;
	}
}