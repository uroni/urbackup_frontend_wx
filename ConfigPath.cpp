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

#include "ConfigPath.h"
#include "stringtools.h"
#include "FileSettingsReader.h"
#include "Settings.h"

#define CP_ID_OK 100

std::string ConvertToUTF8(const std::wstring &input);
#ifndef wxUSE_WCHART_T
std::wstring ConvertToUnicode(const std::string &input);
#else
#define ConvertToUnicode(x)
#endif

extern wxString res_path;
extern std::string g_res_path;
extern wxString ico_ext;
extern wxBitmapType ico_type;

wxTextValidator getPathValidator(void);

#undef _
#define _(s) wxGetTranslation(wxT(s))

ConfigPath::ConfigPath(wxWindow* parent)
: GUIConfigPath(parent)

{
	mod=false;
	SetIcon(wxIcon(res_path+wxT("backup-ok.")+ico_ext, ico_type));

#ifdef _DEBUG
	CFileSettingsReader settings("urbackup/data/settings.cfg");
#elif _WIN32
	CFileSettingsReader settings(g_res_path + "urbackup/data/settings.cfg");
#else
	CFileSettingsReader settings(VARDIR "/urbackup/data/settings.cfg");
#endif

	default_dirs_use = 0;
	std::string default_dirs_use_str;
	if (settings.getValue("default_dirs.use", &default_dirs_use_str))
		default_dirs_use = atoi(default_dirs_use_str.c_str());

	default_dirs_use_orig = default_dirs_use;

	default_dirs_use_lm_orig = 0;
	std::string default_dirs_use_lm_str;
	if (settings.getValue("default_dirs.use_lm", &default_dirs_use_lm_str))
		wxString(default_dirs_use_lm_str).ToLongLong(&default_dirs_use_lm_orig);

	std::vector<std::string> default_dirs_toks;

	size_t num_group_off = 0;
	{
		std::string val;
		if (settings.getValue("default_dirs.group", &val))
		{
			TokenizeMail(val, default_dirs_toks, ";");
			num_group_off = default_dirs_toks.size();
		}
	}
	size_t num_home_off = num_group_off;
	{
		std::string val;
		std::vector<std::string> toks;
		if (settings.getValue("default_dirs.home", &val))
		{
			TokenizeMail(val, toks, ";");
			default_dirs_toks.insert(default_dirs_toks.end(), toks.begin(), toks.end());
			num_home_off = default_dirs_toks.size();
		}
	}
	{
		std::string val;
		std::vector<std::string> toks;
		if (settings.getValue("default_dirs.client", &val))
		{
			TokenizeMail(val, toks, ";");
			default_dirs_toks.insert(default_dirs_toks.end(), toks.begin(), toks.end());
		}
	}

	std::vector<SBackupDir> c_dirs = Connector::getSharedPaths();

	std::map<wxString, size_t > path_n;

	for (size_t i = 0; i<default_dirs_toks.size(); ++i)
	{
		SBackupDir dir;
		dir.path = UnescapeParamString(trim(default_dirs_toks[i]));
		dir.group = 0;
		++path_n[dir.path];
		if (dir.path.find("|") != std::string::npos)
		{
			std::vector<std::string> toks;
			std::string s_dir(dir.path.ToUTF8());
			TokenizeMail(s_dir, toks, "|");
			dir.path = toks[0];
			dir.name = toks[1];
			if (toks.size()>2)
			{
				dir.group = (std::max)(0, atoi(toks[2].c_str()));
			}
		}

		size_t match_n = 0;
		for (size_t j = 0; j < c_dirs.size(); ++j)
		{
			if (c_dirs[j].path == dir.path)
			{
				++match_n;
				if (match_n == path_n[dir.path])
				{
					std::string flags = getafter("/", std::string(dir.name.ToUTF8()));
					dir.name = c_dirs[j].name;

					if (!flags.empty())
						dir.name += "/" + flags;
				}
			}
		}

		if (dir.name.IsEmpty())
		{
			dir.name = getDefaultDirname(dir.path.wc_str());
		}

		dir.server_default = 1;

		if (i < num_group_off)
		{
			dirs_group.push_back(dir);
		}
		else if (i < num_home_off)
		{
			dirs_home.push_back(dir);
		}
		else
		{
			dir.server_default = 0;
			dirs_client.push_back(dir);
		}
	}

	renderListBoxContent();
	switchBitmapLabel();

	m_textCtrl18->SetValidator(getPathValidator());

	Centre();
	Show(true);
}

void ConfigPath::OnClickOk(wxCommandEvent &evt)
{
	if(mod)
	{
		if (default_dirs_use != default_dirs_use_orig)
		{
			std::string s_data = "default_dirs.use=" + nconvert(default_dirs_use) + "\n";
			int64 ctime = wxGetUTCTimeMillis().GetValue() / 1000;

			if (default_dirs_use_lm_orig > ctime)
				ctime = default_dirs_use_lm_orig + 1;

			s_data += "default_dirs.use_lm=" + nconvert(ctime) + "\n";

			Connector::updateSettings(s_data);
		}
		if (!Connector::saveSharedPaths(dirs_client))
		{
			wxMessageBox(_("Saving the changed paths to backup failed"), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);
			return;
		}
	}
	Close();
}

void ConfigPath::OnClickAbort(wxCommandEvent &evt)
{
	Close();
}

void ConfigPath::OnClickNew(wxCommandEvent &evt)
{
	wxDirDialog ed(this, _("Please select the directory that will be backed up."), wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST );
	int rc=ed.ShowModal();
	if(rc==wxID_OK)
	{
		SBackupDir ad;
		ad.path=ed.GetPath();
		ad.name=getDefaultDirname(ad.path.wc_str());
		ad.group=0;
		ad.id=0;
		ad.server_default = 0;
		dirs_client.push_back(ad);

		if (dirs_group.empty() && dirs_home.empty())
			default_dirs_use = c_use_value_client;
		else
			default_dirs_use |= c_use_value_client;

		switchBitmapLabel();
		renderListBoxContent();

		mod=true;
	}
}

void ConfigPath::OnClickDel(wxCommandEvent &evt)
{
	int sel=listbox->GetSelection();
	if(sel>=0)
	{
		size_t offs_home = (default_dirs_use & c_use_group) ? dirs_group.size() : 0;
		bool switch_client = false;
		if (default_dirs_use & c_use_group
			&& sel < dirs_group.size())
		{
			switch_client = true;
		}
		else if (default_dirs_use & c_use_value
			&& sel < offs_home + dirs_home.size())
		{
			switch_client = true;
		}

		offs_home+= (default_dirs_use & c_use_value) ? dirs_home.size() : 0;

		if (switch_client)
		{
			if (default_dirs_use & c_use_group)
				dirs_client.insert(dirs_client.begin(), dirs_group.begin(), dirs_group.end());
			if (default_dirs_use & c_use_value)
				dirs_client.insert(dirs_client.begin(), dirs_home.begin(), dirs_home.end());

			default_dirs_use = c_use_value_client;

			dirs_client.erase(dirs_client.begin() + sel);
		}
		else
		{
			dirs_client.erase(dirs_client.begin() + sel - offs_home);
			if (dirs_client.empty())
			{
				default_dirs_use &= ~c_use_value_client;
			}
		}

		renderListBoxContent();
		switchBitmapLabel();

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
		for(size_t j=0;j<sizeof(illegalchars)/sizeof(illegalchars[0]);++j)
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
	for(size_t i=0;i<dirs_client.size();++i)
	{
		if(dirs_client[i].name==pn )
		{
			return true;
		}
	}
	for (size_t i = 0; i<dirs_group.size(); ++i)
	{
		if (dirs_group[i].name == pn)
		{
			return true;
		}
	}
	for (size_t i = 0; i<dirs_home.size(); ++i)
	{
		if (dirs_home[i].name == pn)
		{
			return true;
		}
	}
	return false;
}

void ConfigPath::switchBitmapLabel()
{
	if (default_dirs_use == c_use_group)
	{
		m_bitmapButton1->SetBitmapLabel(fa_lock_img_scaled);
	}
	else if (default_dirs_use == c_use_value)
	{
		m_bitmapButton1->SetBitmapLabel(fa_home_img_scaled);
	}
	else if (default_dirs_use == c_use_value_client)
	{
		m_bitmapButton1->SetBitmapLabel(fa_client_img_scaled);
	}
	else
	{
		m_bitmapButton1->SetBitmapLabel(fa_copy_img_scaled);
	}
}

void ConfigPath::renderListBoxContent()
{
	listbox->Clear();
	if (default_dirs_use & c_use_group)
	{
		for (size_t i = 0; i < dirs_group.size(); ++i)
		{
			listbox->Append(dirs_group[i].path);
		}
	}

	if (default_dirs_use & c_use_value)
	{
		for (size_t i = 0; i < dirs_home.size(); ++i)
		{
			listbox->Append(dirs_home[i].path);
		}
	}

	if (default_dirs_use & c_use_value_client)
	{
		for (size_t i = 0; i < dirs_client.size(); ++i)
		{
			listbox->Append(dirs_client[i].path);
		}
	}
}

SBackupDir & ConfigPath::getSel(int sel)
{
	size_t offs_home = (default_dirs_use & c_use_group) ? dirs_group.size() : 0;
	bool switch_client = false;
	if (default_dirs_use & c_use_group
		&& sel < dirs_group.size())
	{
		return dirs_group[sel];
	}
	else if (default_dirs_use & c_use_value
		&& sel < offs_home + dirs_home.size())
	{
		return dirs_home[sel - offs_home];
	}

	offs_home += (default_dirs_use & c_use_value) ? dirs_home.size() : 0;

	return dirs_client[sel - offs_home];
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
		m_textCtrl18->SetValue(getSel(sel).name);
		/*m_group->Enable();
		m_group->Select(dirs[sel].group);*/
	}
}

void ConfigPath::OnNameTextChange(wxCommandEvent &evt)
{
	int sel=listbox->GetSelection();
	if(sel>=0)
	{
		getSel(sel).name=m_textCtrl18->GetValue();
		mod=true;
	}
}

void ConfigPath::OnGroupChange( wxCommandEvent& evt )
{
	/*int sel=listbox->GetSelection();
	if(sel>=0)
	{
		int group = m_group->GetSelection();
		if(group>=0)
		{
			dirs[sel].group=group;
			mod=true;
		}		
	}*/
}

void ConfigPath::OnClickSourceSwitch(wxCommandEvent & event)
{
	if (default_dirs_use == c_use_group)
	{
		default_dirs_use = c_use_value;
	}
	else if (default_dirs_use == c_use_value)
	{
		default_dirs_use = c_use_value_client;
	}
	else if (default_dirs_use == c_use_value_client)
	{
		default_dirs_use = c_use_group | c_use_value | c_use_value_client;
	}
	else
	{
		default_dirs_use = c_use_group;
	}

	renderListBoxContent();
	switchBitmapLabel();
	mod = true;
}
