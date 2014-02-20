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
#include <wx/taskbar.h>
#include <wx/menu.h>

class TrayIcon : public wxTaskBarIcon
{
public:
	TrayIcon(void);
	wxMenu* CreatePopupMenu(void);
	void OnPopupClick(wxCommandEvent &evt);
	void OnClick(wxCommandEvent &evt);
	void OnBalloonClick(wxCommandEvent &evt);
	void BalloonActionUpgrade(void);
	void BalloonActionNewServer(const std::string &ident);


private:
	int balloon_action;
	std::string new_ident;
};

#define ICON_WIDTH -1
#define ICON_HEIGHT -1