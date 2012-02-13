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
#ifndef wxUSE_WCHART_T
std::wstring ConvertToUnicode(const std::string &input);
#else
#define ConvertToUnicode(x)
#endif

extern wxString res_path;
extern wxString ico_ext;
extern wxBitmapType ico_type;

wxTextValidator getPathValidator(void);

ConfigPath::ConfigPath(wxWindow* parent)
: GUIConfigPath(parent)

{
	mod=false;
	SetIcon(wxIcon(res_path+wxT("backup-ok-big.")+ico_ext, ico_type));

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
			dirs[i].name=getDefaultDirname(dirs[i].path.wc_str());
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
		ad.name=getDefaultDirname(ad.path.wc_str());
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

std::wstring removeChars(std::wstring in)
{
	wchar_t illegalchars[] = {'*', ':', '/' , '\\'};
	std::wstring ret;
	for(size_t i=0;i<in.size();++i)
	{
		bool found=false;
		for(size_t j=0;j<sizeof(illegalchars);++j)
		{
			if(illegalchars[j]==in[i])
			{
				found=true;
				break;
			}
		}
		if(!found)
		{
			ret+=in[i];
		}
	}
	return ret;
}

bool ConfigPath::findPathName(const std::wstring &pn)
{
	for(size_t i=0;i<dirs.size();++i)
	{
		if(dirs[i].name==pn )
		{
			return true;
		}
	}
	return false;
}

std::wstring ConfigPath::getDefaultDirname(const std::wstring &path)
{
	std::wstring dirname=removeChars(ExtractFileName(path));

	if(dirname.empty())
		dirname=L"root";

	if(findPathName(dirname) )
	{
		for(int k=0;k<100;++k)
		{
			if(!findPathName(dirname+L"_"+convert(k)) )
			{
				dirname=dirname+L"_"+convert(k);
				break;
			}
		}
	}

	return dirname;
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