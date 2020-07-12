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
#include <wx/listctrl.h>

#include "Connector.h"
#include "gui/GUI.h"


class ConfigPath : public GUIConfigPath
{
public:
	ConfigPath(wxWindow* parent);

	void OnClickOk(wxCommandEvent &evt);
	void OnClickAbort(wxCommandEvent &evt);
	void OnClickNew(wxCommandEvent &evt);
	void OnClickDel(wxCommandEvent &evt);
	void OnPathSelected(wxCommandEvent &evt);
	void OnNameTextChange(wxCommandEvent &evt);
	void OnGroupChange( wxCommandEvent& evt );
	void OnClickSourceSwitch(wxCommandEvent& event);
	

private:
	std::vector<SBackupDir> dirs_group;
	std::vector<SBackupDir> dirs_home;
	std::vector<SBackupDir> dirs_client;

	void switchBitmapLabel();
	void renderListBoxContent();

	SBackupDir& getSel(int sel);

	std::wstring getDefaultDirname(const std::wstring &path);
	bool findPathName(const std::wstring &pn);

	int default_dirs_use;
	int default_dirs_use_orig;
	int64 default_dirs_use_lm_orig;
	bool mod;
};