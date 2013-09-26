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
#include "ConfigPath.h"
#include "Settings.h"
#include "Logs.h"
#include "main.h"
#include "Connector.h"
#include "Info.h"
#include "capa_bits.h"

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
}

void TrayIcon::OnPopupClick(wxCommandEvent &evt)
{
	if(evt.GetId()==ID_TI_EXIT)
	{
		wxExit();	
		return;
	}
	if(evt.GetId()==ID_TI_ADD_PATH)
	{
		ConfigPath *cp=new ConfigPath(NULL);
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
		Settings *s=new Settings(NULL);
	}
	else if(evt.GetId()==ID_TI_LOGS)
	{
		Logs *l=new Logs(NULL);
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
}

wxMenu* TrayIcon::CreatePopupMenu(void)
{
	wxMenu *mnu=new wxMenu();
	bool any_prev=false;
	if(!timer->hasCapability(DONT_ALLOW_STARTING_FILE_BACKUPS))
	{
		mnu->Append(ID_TI_BACKUP_FULL, _("Do full file backup"), wxT("Jetzt ein volles Backup ausführen"));
		mnu->Append(ID_TI_BACKUP_INCR, _("Do incremental file backup"), wxT("Jetzt ein inkrementelles Backup ausführen"));
		any_prev=true;
	}
#ifdef _WIN32
	if(!timer->hasCapability(DONT_ALLOW_STARTING_IMAGE_BACKUPS) && !timer->hasCapability(DONT_DO_IMAGE_BACKUPS) )
	{
		mnu->Append(ID_TI_BACKUP_IMAGE_FULL, _("Do full image backup"), wxT("Jetzt ein inkrementelles Image-Backup ausführen"));
		mnu->Append(ID_TI_BACKUP_IMAGE_INCR, _("Do incremental image backup"), wxT("Jetzt ein inkrementelles Image-Backup ausführen"));
		any_prev=true;
	}
#endif
	if(any_prev)
	{
		mnu->AppendSeparator();
	}
	if(!timer->hasCapability(DONT_SHOW_SETTINGS))
	{
		mnu->Append(ID_TI_SETTINGS, _("Settings") );
	}
	if(!timer->hasCapability(DONT_ALLOW_CONFIG_PATHS))
	{
		mnu->Append(ID_TI_ADD_PATH, _("Add/Remove backup paths"));
	}
	if(!timer->hasCapability(DONT_SHOW_LOGS))
	{
		mnu->Append(ID_TI_LOGS, _("Logs") );
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
		Connector::addNewServer(new_ident);
	}
}