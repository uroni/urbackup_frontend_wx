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

#ifdef _WIN32
#include <Windows.h>
#endif

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

extern MyTimer *timer;
extern int working_status;
bool b_is_pausing=false;
extern MyTimer *timer;

extern wxString res_path;
extern wxString ico_ext;
extern wxBitmapType ico_type;

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
		char module_path[MAX_PATH];
		DWORD rc = GetModuleFileNameA(NULL, module_path, MAX_PATH);
		if(rc!=0)
		{
			ShellExecuteA(NULL, "runas", module_path, (cmd + (arg1.empty()?std::string():(" "+arg1))).c_str(), NULL, SW_SHOWNORMAL);
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
}


void TrayIcon::OnPopupClick(wxCommandEvent &evt)
{
	if(evt.GetId()==ID_TI_ADD_PATH)
	{
		runCommand("paths");
	}
	else if(evt.GetId()==ID_TI_BACKUP_FULL || evt.GetId()==ID_TI_BACKUP_INCR)
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
	}
	else if(evt.GetId()==ID_TI_SETTINGS)
	{
		runCommand("settings");
	}
	else if(evt.GetId()==ID_TI_LOGS)
	{
		runCommand("logs");
	}
	else if(evt.GetId()==ID_TI_BACKUP_IMAGE_FULL || evt.GetId()==ID_TI_BACKUP_IMAGE_INCR)
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
	}
	else if(evt.GetId()==ID_TI_PAUSE)
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
	}
	else if(evt.GetId()==ID_TI_CONTINUE)
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
	}
	else if(evt.GetId()==ID_TI_INFO )
	{
		Info *i=new Info(NULL);
	}
	else if(evt.GetId()==ID_TI_STATUS)
	{
		Status *s = new Status(NULL);
	}
	else if(evt.GetId()==ID_TI_EXIT)
	{
		if(!b_is_pausing)
		{
			Connector::setPause(true);
		}
		wxExit();
	}
}

void TrayIcon::OnClick(wxCommandEvent &evt)
{
	Status *s = new Status(NULL);
}

wxMenu* TrayIcon::CreatePopupMenu(void)
{
	wxMenu *mnu=new wxMenu();
	bool any_prev=false;
	if(!timer->hasCapability(DONT_ALLOW_STARTING_FILE_BACKUPS))
	{
		if(!timer->hasCapability(DONT_ALLOW_STARTING_FULL_FILE_BACKUPS))
		{
			mnu->Append(ID_TI_BACKUP_FULL, _("Do full file backup"), wxT("Jetzt ein volles Backup ausführen"));
			any_prev=true;
		}
		if(!timer->hasCapability(DONT_ALLOW_STARTING_INCR_FILE_BACKUPS))
		{
			mnu->Append(ID_TI_BACKUP_INCR, _("Do incremental file backup"), wxT("Jetzt ein inkrementelles Backup ausführen"));
			any_prev=true;
		}		
	}
#ifdef _WIN32
	if(!timer->hasCapability(DONT_ALLOW_STARTING_IMAGE_BACKUPS) && !timer->hasCapability(DONT_DO_IMAGE_BACKUPS) )
	{
		if(!timer->hasCapability(DONT_ALLOW_STARTING_FULL_IMAGE_BACKUPS))
		{
			mnu->Append(ID_TI_BACKUP_IMAGE_FULL, _("Do full image backup"), wxT("Jetzt ein inkrementelles Image-Backup ausführen"));
			any_prev=true;
		}
		if(!timer->hasCapability(DONT_ALLOW_STARTING_INCR_IMAGE_BACKUPS))
		{
			mnu->Append(ID_TI_BACKUP_IMAGE_INCR, _("Do incremental image backup"), wxT("Jetzt ein inkrementelles Image-Backup ausführen"));
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
	if(working_status>0)
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
	mnu->Append(ID_TI_EXIT, _("Exit"));
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