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
			SetIcon(wxIcon(wxT("backup-progress.ico"), wxBITMAP_TYPE_ICO), wxT("Warte auf Server..."));
			if(timer!=NULL)
				timer->Start(1000);
		}
		else if(rc==2)
			wxMessageBox( _("Ein Backup läuft bereits. Konnte leider kein weiteres starten."), wxT("UrBackup"), wxICON_EXCLAMATION);
		else
			wxMessageBox( _("Konnte kein Backup starten, da leider kein Server gefunden wurde"), wxT("UrBackup"), wxICON_ERROR);
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
			SetIcon(wxIcon(wxT("backup-progress.ico"), wxBITMAP_TYPE_ICO), wxT("Warte auf Server..."));
			if(timer!=NULL)
				timer->Start(1000);
		}
		else if(rc==2)
			wxMessageBox( _("Ein Backup läuft bereits. Konnte leider kein weiteres starten."), wxT("UrBackup"), wxICON_EXCLAMATION);
		else
			wxMessageBox( _("Konnte kein Backup starten, da leider kein Server gefunden wurde"), wxT("UrBackup"), wxICON_ERROR);
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
			wxMessageBox( _("Verbindung mit Backupserver fehlgeschlagen. Konnte nicht pausieren."), wxT("UrBackup"), wxICON_ERROR);
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
			wxMessageBox( _("Verbindung mit Backupserver fehlgeschlagen. Konnte nicht fortfahren."), wxT("UrBackup"), wxICON_ERROR);
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
	mnu->Append(ID_TI_BACKUP_FULL, _("Volles Backup jetzt"), wxT("Jetzt ein volles Backup ausführen"));
	mnu->Append(ID_TI_BACKUP_INCR, _("Inkremetelles Backup jetzt"), wxT("Jetzt ein inkrementelles Backup ausführen"));
	mnu->Append(ID_TI_BACKUP_IMAGE_FULL, _("Volles Image-Backup jetzt"), wxT("Jetzt ein inkrementelles Image-Backup ausführen"));
	mnu->Append(ID_TI_BACKUP_IMAGE_INCR, _("Inkremetelles Image-Backup jetzt"), wxT("Jetzt ein inkrementelles Image-Backup ausführen"));
	mnu->AppendSeparator();
	mnu->Append(ID_TI_SETTINGS, _("Einstellungen") );
	mnu->Append(ID_TI_ADD_PATH, _("Pfade konfigurieren"));
	mnu->Append(ID_TI_LOGS, _("Logs") );
	mnu->Append(ID_TI_INFO, _("Info") );
	if(working_status>0)
	{
		if(b_is_pausing==false)
		{
			mnu->Append(ID_TI_PAUSE, _("Pausieren"));
		}
		else
		{
			mnu->Append(ID_TI_CONTINUE, _("Fortfahren"));
		}
	}
	mnu->Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&TrayIcon::OnPopupClick, NULL, this);
	return mnu;
}