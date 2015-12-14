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

#include "Info.h"
#include "stringtools.h"

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

	instance=this;
}

void Info::OnOKClick( wxCommandEvent& event )
{
	Close();
}

Info* Info::getInstance()
{
	return instance;
}

Info::~Info()
{
	instance=NULL;
}
