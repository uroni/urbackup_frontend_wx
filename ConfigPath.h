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
	

private:
	std::vector<SBackupDir> dirs;

	std::string getDefaultDirname(const std::string &path);
	bool findPathName(const std::string &pn);
};