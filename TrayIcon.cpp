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

#include "TrayIcon.h"
#include "main.h"
#include "Connector.h"
#include "Info.h"
#include "capa_bits.h"
#include "Status.h"
#include "stringtools.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include "../config.h"
#endif
#include "FileSettingsReader.h"
#include <wx/dir.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/textfile.h>

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
#define ID_TI_UNINSTALL 114
#define ID_TI_CONFIG_COMPONENTS 115
#define ID_TI_RESTORE_COMPONENTS 116

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

#ifdef _WIN32
void runCommand(std::string cmd, std::string arg1)
{
	wchar_t module_path[MAX_PATH];
	DWORD rc = GetModuleFileNameW(NULL, module_path, MAX_PATH);
	if(rc!=0)
	{
		if(!Connector::getPasswordData(true, false).empty())
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
#ifndef __APPLE__
	if(system("type gksudo")==0) sudo_app="gksudo";
	else if(system("type kdesudo")==0) sudo_app="kdesudo";
	else if(system("type gksu")==0) sudo_app="gksu";
	else if(system("type kdesu")==0) sudo_app="kdesu";
	else sudo_app="sudo";
#else
	sudo_app="\"" BINDIR "/UrBackup Client Administration\"";
#endif
}

void runCommand(std::string cmd, std::string arg1)
{
	std::string sudo_prefix = "";

	if (Connector::getPasswordData(true, false).empty())
	{
		if (sudo_app.empty())
		{
			find_sudo_app();
		}
		sudo_prefix = sudo_app + " ";
	}
#ifdef __APPLE__
	wxString clientexecutable = wxStandardPaths::Get().GetExecutablePath();
	wxExecute(sudo_prefix +"\""+ clientexecutable +"\" "+cmd+(arg1.empty()?std::string():(" "+arg1)), wxEXEC_ASYNC, NULL, NULL);
#else
	wxExecute(sudo_prefix + BINDIR "/urbackupclientgui "+cmd+(arg1.empty()?std::string():(" "+arg1)), wxEXEC_ASYNC, NULL, NULL);
#endif
}
#endif //WIN32


#ifdef __APPLE__
extern "C" void bring_to_foreground();
#endif

void TrayIcon::OnPopupClick(wxCommandEvent &evt)
{
#ifdef __WXMAC__
    bring_to_foreground();
#endif 
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
			}
			else if(rc==2)
				wxMessageBox( _("A backup is already running. Could not start another one."), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_EXCLAMATION);
			else if(rc==3)
				wxMessageBox( _("Could not start backup, because no backup server was found."), wxT("UrBackup"),  wxOK | wxCENTRE | wxICON_ERROR);
			else
				wxMessageBox( _("Could not start backup."), wxT("UrBackup"), wxOK | wxCENTRE | wxICON_ERROR);

		}break;
	case ID_TI_SETTINGS:
		{
			runCommand("settings");
		}break;
	case ID_TI_LOGS:
		{
			runCommand("logs");
		}break;
	case ID_TI_BACKUP_IMAGE_FULL:
	case ID_TI_BACKUP_IMAGE_INCR:
		{
			bool full= (evt.GetId()==ID_TI_BACKUP_IMAGE_FULL);
			int rc=Connector::startImage(full);
			if(rc==1)
			{
				SetIcon(getAppIcon(wxT("backup-progress")), _("Waiting for server..."));
			}
			else if(rc==2)
				wxMessageBox( _("A backup is already running. Could not start another one."), wxT("UrBackup"), wxICON_EXCLAMATION);
			else if(rc==3)
				wxMessageBox( _("Could not start backup, because no backup server was found."), wxT("UrBackup"), wxICON_ERROR);
			else
				wxMessageBox( _("Could not start backup, because no backup server was found."), wxT("UrBackup"), wxICON_ERROR);
		}break;
	case ID_TI_PAUSE:
		{
			if(Connector::setPause(true))
			{
				b_is_pausing=true;
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
			}
			else
			{
				wxMessageBox( _("Continuing failed: No connection to backup server."), wxT("UrBackup"), wxICON_ERROR);
			}
		}break;
	case ID_TI_INFO:
		{
			if(Info::getInstance()!=NULL)
			{
				Info::getInstance()->SetFocus();
				Info::getInstance()->Raise();
#if !defined(__WXMAC__) && !defined(__linux__)
				Info::getInstance()->RequestUserAttention();
#endif
			}
			else
			{
				new Info(NULL);
			}
		}break;
	case ID_TI_STATUS:
		{
			if(Status::getInstance()!=NULL)
			{
				Status::getInstance()->SetFocus();
				Status::getInstance()->Raise();
#if !defined(__WXMAC__) && !defined(__linux__)
				Status::getInstance()->RequestUserAttention();
#endif
			}
			else
			{
				new Status(NULL, 0);
			}
		}break;
	case ID_TI_EXIT:
		{
			if( !b_is_pausing
				 && !timer->hasCapability(DONT_ALLOW_PAUSE) )
			{
				Connector::setPause(true);
			}
			wxExit();
		}break;
	case ID_TI_ACCESS:
		{
			accessBackups(std::wstring());
		}break;
#ifdef __APPLE__
	case ID_TI_UNINSTALL:
		{
			int answer = wxMessageBox(_("Do you really want to remove the UrBackup client from this system?"),
							 _("Confirm uninstall"),
            					wxYES_NO | wxCANCEL);

            if (answer == wxYES)
            {
				runCommand("uninstall");
			}
	} break;
#endif
	case ID_TI_CONFIG_COMPONENTS:
	{
		runCommand("selectWindowsComponents");
	} break;
	case ID_TI_RESTORE_COMPONENTS:
	{
		runCommand("restoreWindowsComponents");
	} break;

	}
}

void TrayIcon::OnClick(wxCommandEvent &evt)
{
	if(Status::getInstance()!=NULL)
	{
		Status::getInstance()->SetFocus();
		Status::getInstance()->Raise();
#if !defined(__WXMAC__) && !defined(__linux__)
		Status::getInstance()->RequestUserAttention();
#endif
	}
	else
	{
		new Status(NULL, 0);
	}
}

wxMenu* TrayIcon::CreatePopupMenu(void)
{
	wxMenu *mnu=new wxMenu();

	if(timer->hasCapability(ALLOW_TOKEN_AUTHENTICATION))
	{
		wxString msg = _("Access/restore backups");
		if (timer->hasCapability(DONT_ALLOW_FILE_RESTORE))
		{
			msg = _("Access backups");
		}
		mnu->Append(ID_TI_ACCESS, msg, msg);
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
#ifdef _WIN32
	if (!timer->hasCapability(DONT_ALLOW_COMPONENT_CONFIG))
	{
		mnu->Append(ID_TI_CONFIG_COMPONENTS, _("Configure components to backup"));
		any_prev = true;
	}
	if (!timer->hasCapability(DONT_ALLOW_COMPONENT_RESTORE)
		&& !timer->hasCapability(STATUS_NO_COMPONENTS))
	{
		mnu->Append(ID_TI_RESTORE_COMPONENTS, _("Restore components"));
		any_prev = true;
	}
#endif
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
#ifndef __APPLE__
	if(!timer->hasCapability(DONT_ALLOW_EXIT_TRAY_ICON))
	{
		mnu->Append(ID_TI_EXIT, _("Exit"));
	}
#else
	mnu->AppendSeparator();
	mnu->Append(ID_TI_UNINSTALL, _("Uninstall"));
#endif
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

void read_tokens(wxString token_path, std::string& tokens)
{
#ifdef _WIN32
	static bool adj_priv = false;
	static bool has_backup_priv = false;
	if (!adj_priv)
	{
		adj_priv = true;
		has_backup_priv = ModifyPrivilege(SE_BACKUP_NAME, TRUE) == S_OK;
	}
#endif

	wxArrayString token_files; 
	wxDir::GetAllFiles(token_path, &token_files, wxEmptyString, wxDIR_FILES);

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
#ifdef _WIN32
		else if (has_backup_priv)
		{
			HANDLE hFile = CreateFileW(token_files[i].ToStdWstring().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);

			std::string nt;
			if (hFile != INVALID_HANDLE_VALUE)
			{
				char buf[512];
				DWORD read;
				while (ReadFile(hFile, buf, 512, &read, NULL) != FALSE)
				{
					nt.insert(nt.end(), buf, buf + read);
					if (read == 0)
					{
						break;
					}
				}

				CloseHandle(hFile);
			}

			if (!nt.empty())
			{
				if (!tokens.empty())
				{
					tokens += ";";
				}
				tokens += nt;
			}
		}
#endif
	}
}

void TrayIcon::accessBackups( wxString path )
{
	wxString orig_path = path;
	if(!path.empty())
	{
		std::vector<SBackupDir> backupdirs = Connector::getSharedPaths();

		bool found=false;

		for(size_t i=0;i<backupdirs.size();++i)
		{
			if(path.find(backupdirs[i].path)==0)
			{
				path.erase(0, backupdirs[i].path.size());
				if(!path.empty() && path[0]!='\\' && path[0]!='/')
				{
					path=wxT("/")+path;
				}

				for(size_t j=0;j<path.size();++j)
				{
#ifdef _WIN32
					if(path[j]=='\\')
#else
					if(path[j]=='\\' 
						&& (j==0 || (path[j-1]!='\\'))
						&& (j+1>=path.size() || path[j+1]!='\\'))
#endif
					{
						path[j]='/';
					}
				}

				path="/"+backupdirs[i].name+path;

				found=true;
				break;
			}
		}

		if(!found)
		{
			return;
		}
	}

	wxString wx_path = res_path + wxT("/tokens");
#ifdef _DEBUG
	wx_path = wxT("tokens");
#endif

	std::string tokens;
	
	read_tokens(wx_path, tokens);
	
#if !defined(_WIN32)
	read_tokens(wxT(VARDIR "/urbackup/tokens"), tokens);
#endif

	if(tokens.empty())
	{
		wxMessageBox( _("No rights to access any files."), wxT("UrBackup"), wxICON_ERROR);
	}

	std::string params = Connector::getAccessParameters(tokens);

	if(!path.empty() && !params.empty())
	{
		params+="&path="+base64_encode_dash(path.ToUTF8().data());

		if(wxDirExists(orig_path))
		{
			params+="&is_file=false";
		}
		else
		{
			params+="&is_file=true";
		}
	}

	if(!params.empty())
	{
		if (params.size() > 2000)
		{
			wxFileName tmpfn(wxStandardPaths::Get().GetTempDir(), wxT("urbackupclient_fileaccess.htm"));
			wxTextFile file(tmpfn.GetFullPath());
			file.AddLine("<html><head>");
			file.AddLine("<meta http-equiv=\"refresh\" content=\"0;URL='" + params + "'\">");
			file.AddLine("<script type=\"text/javascript\">");
			file.AddLine("window.location.href='" + params + "';");
			file.AddLine("</script></head><body>");
			file.AddLine("You will be redirected to your UrBackup instance. <a href=\"" + params + "\">Click here</a> if not.");
			file.AddLine("</body></html>");
			file.Write();
			file.Close();

			std::string fileurl = tmpfn.GetFullPath().ToStdString();
#ifdef _WIN32
			fileurl = greplace("\\", "/", fileurl);
#endif
			fileurl = EscapeParamString(fileurl);
			wxLaunchDefaultBrowser("file:///" + fileurl);
		}
		else
		{
			wxLaunchDefaultBrowser(wxString::FromUTF8(params.data(), params.size()));
		}
	}
	else
	{
		wxMessageBox( _("Error getting server URL. Cannot access files."), wxT("UrBackup"), wxICON_ERROR);
	}
}
