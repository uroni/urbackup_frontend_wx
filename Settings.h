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
#include <map>
#include "Connector.h"
#include "gui/GUI.h"
#include "FileSettingsReader.h"

namespace
{
	const int c_use_group = 1;
	const int c_use_value = 2;
	const char* c_use_value_str = "2";
	const int c_use_value_client = 4;
	const char* c_use_value_client_str = "4";
}


class Settings : public GUISettings
{
public:
	Settings(wxWindow* parent);
	virtual ~Settings(void);

	virtual void OnOkClick( wxCommandEvent& event );
	virtual void OnAbortClick( wxCommandEvent& event );
	virtual void OnDisableImageBackups( wxCommandEvent& event );
	virtual void OnBitmapBtnClick(wxCommandEvent& event);
	virtual void OnCtlChange(wxCommandEvent& event);

	static std::string mergeNewSettings(CFileSettingsReader *settings, const std::map<std::string, std::string>& n_vals);

private:

	std::wstring transformValToUI(const std::wstring& key, const std::wstring& val);
	std::wstring transformValFromUI(const std::wstring& key, const std::wstring& val);

	void setSettingsSwitch(const std::wstring& key, wxBitmapButton* btn, wxWindow* ctrl);

	std::map<wxWindowID, std::wstring> button_ids;
	std::map<wxWindowID, std::wstring> ctrl_ids;

	struct SSetting
	{
		wxBitmapButton* btn;
		wxWindow* ctrl;
		int use;
		std::wstring value_group;
		std::wstring value_home;
		std::wstring value_client;
	};

	std::map<std::wstring, SSetting> settings_info;

	CFileSettingsReader *settings;

	bool init_complete;
};