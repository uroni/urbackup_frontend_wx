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

#include "TrayIcon.h"
#include "main.h"
#include "Connector.h"
#include "Info.h"
#include "capa_bits.h"
#include "Status.h"
#include "stringtools.h"

#ifdef _WIN32
#include <Windows.h>
#endif
#include "FileSettingsReader.h"
#include <wx/dir.h>

#undef _
#define _(s) wxGetTranslation(wxT(s))

#define ID_TI_EXIT 100
#define ID_TI_ADD_PATH 101
#define ID_TI_BACKUP_FULL 102
#define ID_TI_BACKUP_INCR 103
#define ID_TI_SEP -1
#define ID_TI_SETTINGS 104
#define ID_TI_LOGS 105
#define ID_TI_BACKUP_IMAGE_FULL 106
#define ID_TI_BACKUP_IMAGE_INCR 107
#define ID_TI_PAUSE 108
#define ID_TI_CONTINUE 109
#define ID_TI_INFO 110
#define ID_TI_STATUS 112
#define ID_TI_ACCESS 113

extern MyTimer *timer;
extern bool backup_is_running;
bool b_is_pausing=false;
extern MyTimer *timer;

extern wxString res_path;
extern wxString ico_ext;
extern wxBitmapType ico_type;
extern std::string g_res_path;

TrayIcon::TrayIcon(void)
	: wxTaskBarIcon()
{
	balloon_action=0;
#ifdef wxUSE_TASKBARICON_BALLOONS
	Connect(wxEVT_TASKBAR_BALLOON_CLICK, (wxObjectEventFunction)&TrayIcon::OnBalloonClick, NULL, this);
#endif
#ifdef _WIN32
	Connect(wxEVT_TASKBAR_LEFT_UP, (wxObjectEventFunction)&TrayIcon::OnClick, NULL, this);
#endif
}

namespace
{
#ifdef _WIN32
	void runCommand(std::string cmd, std::string arg1=std::string())
	{
		wchar_t module_path[MAX_PATH];
		DWORD rc = GetModuleFileNameW(NULL, module_path, MAX_PATH);
		if(rc!=0)
		{
			if(!Connector::getPasswordData(true).empty())
			{
				STARTUPINFO sStartInfo;
				ZeroMemory( &sStartInfo, sizeof(STARTUPINFO) );
				sStartInfo.cb = sizeof(STARTUPINFO);
				sStartInfo.wShowWindow = SW_SHOWDEFAULT;
				sStartInfo.dwFlags = STARTF_USESHOWWINDOW;

				PROCESS_INFORMATION sProcessInfo;
				ZeroMemory( &sProcessInfo, sizeof(PROCESS_INFORMATION) );

				BOOL ok = CreateProcessW( module_path, const_cast<LPWSTR>(std::wstring(std::wstring(L"\"")+std::wstring(module_path)+"\" "+widen(cmd+(arg1.empty()?std::string():(" "+arg1)))).data()),
					NULL, NULL, true,
					NORMAL_PRIORITY_CLASS, NULL, NULL , &sStartInfo, &sProcessInfo );

				if ( !ok ) {
					ShellExecuteW(NULL, L"runas", module_path, widen((cmd + (arg1.empty()?std::string():(" "+arg1)))).c_str(), NULL, SW_SHOWNORMAL);
				}
				else
				{
					CloseHandle(sProcessInfo.hProcess);
					CloseHandle(sProcessInfo.hThread);
				}			
			}
			else
			{
				ShellExecuteW(NULL, L"runas", module_path, widen((cmd + (arg1.empty()?std::string():(" "+arg1)))).c_str(), NULL, SW_SHOWNORMAL);
			}	
		}
	}
#else

	std::string sudo_app;
	void find_sudo_app()
	{
		if(system("type gksudo")==0) sudo_app="gksudo";
		else if(system("type kdesudo")==0) sudo_app="kdesudo";
		else if(system("type gksu")==0) sudo_app="gksu";
		else if(system("type kdesu")==0) sudo_app="kdesu";
		else sudo_app="sudo";
	}

	void runCommand(std::string cmd, std::string arg1=std::string())
	{
		wxExecute("urbackup_client_gui "+cmd+(arg1.empty()?std::string():(" "+arg1)), wxEXEC_ASYNC, NULL, NULL);
	}
#endif

	void accessBackups()
	{
		std::string s_path = g_res_path+"/tokens/";
		wxString wx_path = res_path + wxT("/tokens");
#ifdef _DEBUG
		s_path = "tokens/";
		wx_path = wxT("tokens");
#endif

		wxArrayString token_files; 
		wxDir::GetAllFiles(wx_path, &token_files, wxEmptyString, wxDIR_FILES);

		std::string tokens;
		for(size_t i=0;i<token_files.size();++i)
		{
			std::string nt = getFile(token_files[i].ToStdString());
			if(!nt.empty())
			{
				if(!tokens.empty())
				{
					tokens+=";";
				}
				tokens+=nt;
			}
		}

		if(tokens.empty())
		{
			wxMessageBox( _("No rights to access any files."), wxT("UrBackup"), wxICON_ERROR);
		}

		std::string params = Connector::getAccessParameters(tokens);

		if(!params.empty())
		{
			wxLaunchDefaultBrowser(wxString(params));
		}
		else
		{
			wxMessageBox( _("Error getting server URL. Cannot access files."), wxT("UrBackup"), wxICON_ERROR);
		}
	}
}


void TrayIcon::OnPopupClick(wxCommandEvent &evt)
{
	switch(evt.GetId())
	{
	case ID_TI_ADD_PATH:
		{
			runCommand("paths");
		}break;
	case ID_TI_BACKUP_FULL:
	case ID_TI_BACKUP_INCR:
		{
			bool full= (evt.GetId()==ID_TI_BACKUP_FULL);
			int rc=Connector::startBackup(full);
			if(rc==1)
			{
				SetIcon(getAppIcon(wxT("backup-progress")), _("Waiting for server..."));
				if(timer!=NULL)
					timer->Start(1000);
			}
			else if(rc==2)
				wxMessageBox( _("A backup is already running. Could not start another one."), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_EXCLAMATION);
			else
				wxMessageBox( _("Could not start backup, because no backup server was found."), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);

		}break;
	case ID_TI_SETTINGS:
		{
			runCommand("settings");
		}break;
	case ID_TI_LOGS:
		{
			runCommand("logs");
		}
	case ID_TI_BACKUP_IMAGE_FULL:
	case ID_TI_BACKUP_IMAGE_INCR:
		{
			bool full= (evt.GetId()==ID_TI_BACKUP_IMAGE_FULL);
			int rc=Connector::startImage(full);
			if(rc==1)
			{
				SetIcon(getAppIcon(wxT("backup-progress")), _("Waiting for server..."));
				if(timer!=NULL)
					timer->Start(1000);
			}
			else if(rc==2)
				wxMessageBox( _("A backup is already running. Could not start another one."), wxT("UrBackup"), wxICON_EXCLAMATION);
			else
				wxMessageBox( _("Could not start backup, because no backup server was found."), wxT("UrBackup"), wxICON_ERROR);
		}break;
	case ID_TI_PAUSE:
		{
			if(Connector::setPause(true))
			{
				b_is_pausing=true;
				if(timer!=NULL)
					timer->Notify();
			}
			else
			{
				wxMessageBox( _("Pausing failed: No connection to backup server."), wxT("UrBackup"), wxICON_ERROR);
			}
		}break;
	case ID_TI_CONTINUE:
		{
			if(Connector::setPause(false))
			{
				b_is_pausing=false;
				if(timer!=NULL)
					timer->Notify();
			}
			else
			{
				wxMessageBox( _("Continuing failed: No connection to backup server."), wxT("UrBackup"), wxICON_ERROR);
			}
		}break;
	case ID_TI_INFO:
		{
			Info *i=new Info(NULL);
		}break;
	case ID_TI_STATUS:
		{
			Status *s = new Status(NULL);
		}break;
	case ID_TI_EXIT:
		{
			if(!b_is_pausing)
			{
				Connector::setPause(true);
			}
			wxExit();
		}
	case ID_TI_ACCESS:
		{
			accessBackups();
		}break;
	}
}

void TrayIcon::OnClick(wxCommandEvent &evt)
{
	Status *s = new Status(NULL);
}

wxMenu* TrayIcon::CreatePopupMenu(void)
{
	wxMenu *mnu=new wxMenu();

	if(timer->hasCapability(ALLOW_TOKEN_AUTHENTICATION))
	{
		mnu->Append(ID_TI_ACCESS, _("Access backups"), _("Access backups"));
		mnu->AppendSeparator();
	}
	bool any_prev=false;
	if(!timer->hasCapability(DONT_ALLOW_STARTING_FILE_BACKUPS))
	{
		if(!timer->hasCapability(DONT_ALLOW_STARTING_FULL_FILE_BACKUPS))
		{
			mnu->Append(ID_TI_BACKUP_FULL, _("Do full file backup"), _("Do a full file backup now"));
			any_prev=true;
		}
		if(!timer->hasCapability(DONT_ALLOW_STARTING_INCR_FILE_BACKUPS))
		{
			mnu->Append(ID_TI_BACKUP_INCR, _("Do incremental file backup"), _("Do a incremental file backup now"));
			any_prev=true;
		}		
	}
#ifdef _WIN32
	if(!timer->hasCapability(DONT_ALLOW_STARTING_IMAGE_BACKUPS) && !timer->hasCapability(DONT_DO_IMAGE_BACKUPS) )
	{
		if(!timer->hasCapability(DONT_ALLOW_STARTING_FULL_IMAGE_BACKUPS))
		{
			mnu->Append(ID_TI_BACKUP_IMAGE_FULL, _("Do full image backup"), _("Do a full image backup now"));
			any_prev=true;
		}
		if(!timer->hasCapability(DONT_ALLOW_STARTING_INCR_IMAGE_BACKUPS))
		{
			mnu->Append(ID_TI_BACKUP_IMAGE_INCR, _("Do incremental image backup"), _("Do a incremental image backup now"));
			any_prev=true;
		}
	}
#endif
	if(any_prev)
	{
		mnu->AppendSeparator();
		any_prev=false;
	}
	if(!timer->hasCapability(DONT_SHOW_SETTINGS))
	{
		mnu->Append(ID_TI_SETTINGS, _("Settings") );
		any_prev=true;
	}
	if(!timer->hasCapability(DONT_ALLOW_CONFIG_PATHS))
	{
		mnu->Append(ID_TI_ADD_PATH, _("Add/Remove backup paths"));
		any_prev=true;
	}
	if(!timer->hasCapability(DONT_SHOW_LOGS))
	{
		mnu->Append(ID_TI_LOGS, _("Logs") );
		any_prev=true;
	}
	if(any_prev)
	{
		mnu->AppendSeparator();
		any_prev=false;
	}
	mnu->Append(ID_TI_INFO, _("Infos") );
	if(backup_is_running)
	{
		if(b_is_pausing==false)
		{
			if(!timer->hasCapability(DONT_ALLOW_PAUSE))
			{
				mnu->Append(ID_TI_PAUSE, _("Pause"));
			}
		}
		else
		{
			mnu->Append(ID_TI_CONTINUE, _("Continue"));
		}
	}
	mnu->Append(ID_TI_STATUS, _("Status"));
	if(!timer->hasCapability(DONT_ALLOW_EXIT_TRAY_ICON))
	{
		mnu->Append(ID_TI_EXIT, _("Exit"));
	}
	mnu->Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&TrayIcon::OnPopupClick, NULL, this);
	return mnu;
}

void TrayIcon::BalloonActionNewServer(const std::string &ident)
{
	balloon_action=1;
	new_ident=ident;
}

void TrayIcon::BalloonActionUpgrade(void)
{
	balloon_action=0;
}

void update_urbackup(void);

void TrayIcon::OnBalloonClick(wxCommandEvent &evt)
{
	if(balloon_action==0)
	{
		update_urbackup();
	}
	else
	{
		runCommand("newserver", new_ident);
	}
}