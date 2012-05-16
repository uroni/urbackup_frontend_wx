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
#include "main.h"
#include "stringtools.h"
#include "TaskBarBaloon.h"
#include <iostream>
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/filename.h>


#include <wx/apptrait.h>
/*#if wxUSE_STACKWALKER && defined( __WXDEBUG__ )
// silly workaround for the link error with debug configuration:
// \src\common\appbase.cpp
wxString wxAppTraitsBase::GetAssertStackTrace()
{
   return wxT("");
}
#endif*/

#ifdef _WIN32
#include <windows.h>
#endif

#undef _
#define _(s) wxGetTranslation(wxT(s))

TrayIcon *tray;
MyTimer *timer;
int icon_type=0;
wxString last_status;
unsigned int incr_update_intervall=2*60*60+10*60;
bool incr_update_done=false;
int working_status=0;

#ifndef DD_RELEASE
IMPLEMENT_APP_NO_MAIN(MyApp)
#endif

class TheFrame : public wxFrame {
public:
    TheFrame(void) : wxFrame(NULL, -1, wxT("UrBackupGUI")) { }
};

std::string g_lang="en";
wxString res_path;
std::string g_res_path;
#ifdef _WIN32
wxString ico_ext=wxT("ico");
wxBitmapType ico_type=wxBITMAP_TYPE_ICO;
#else
wxString ico_ext=wxT("xpm");
wxBitmapType ico_type=wxBITMAP_TYPE_XPM;
#endif


bool MyApp::OnInit()
{
#ifdef _WIN32
#ifndef _DEBUG
	wchar_t buf[MAX_PATH];
	GetModuleFileNameW(NULL, buf, MAX_PATH);
	SetCurrentDirectoryW(ExtractFilePath(buf).c_str() );
#endif
#else
	if(FileExists("/usr/share/urbackup/info.txt"))
	{
		res_path=wxT("/usr/share/urbackup/");
		g_res_path="/usr/share/urbackup/";
	}
	if(FileExists("/usr/local/share/urbackup/info.txt"))
	{
		res_path=wxT("/usr/local/share/urbackup/");
		g_res_path="/usr/local/share/urbackup/";
	}
#endif
	wxLanguage lang=wxLANGUAGE_ENGLISH;
	wxLanguage sysdef=(wxLanguage)wxLocale::GetSystemLanguage();
	switch(sysdef)
	{
	case wxLANGUAGE_GERMAN:
	case wxLANGUAGE_GERMAN_AUSTRIAN:
	case wxLANGUAGE_GERMAN_BELGIUM:
	case wxLANGUAGE_GERMAN_LIECHTENSTEIN:
	case wxLANGUAGE_GERMAN_LUXEMBOURG:
	case wxLANGUAGE_GERMAN_SWISS:
		lang=wxLANGUAGE_GERMAN;
		g_lang="de";
		break;
	case wxLANGUAGE_FRENCH:
	case wxLANGUAGE_FRENCH_BELGIAN:
	case wxLANGUAGE_FRENCH_CANADIAN:
	case wxLANGUAGE_FRENCH_LUXEMBOURG:
	case wxLANGUAGE_FRENCH_MONACO:
	case wxLANGUAGE_FRENCH_SWISS:
		lang=wxLANGUAGE_FRENCH;
		g_lang="fr";
		break;
	}

	//std::cout << "Lang: " << g_lang << std::endl;
#ifndef _WIN32
	m_locale.AddCatalogLookupPathPrefix(res_path);
#endif
	m_locale.Init(lang, 0);
	m_locale.AddCatalog(L"trans");
	
	if(!m_locale.IsOk() )
	{
		std::cout << "Selected language is wrong!" << std::endl;
	}



	this->SetTopWindow(new TheFrame);
	wxImage::AddHandler(new wxPNGHandler);
	//wxInitAllImageHandlers();

	tray=new TrayIcon;
	bool b=tray->SetIcon(wxIcon(res_path+wxT("backup-ok.")+ico_ext, ico_type), wxT("UrBackup Client"));
	if(!b)
	{
		std::cout << "Setting icon failed." << std::endl;
	}

	timer=new MyTimer;

	timer->Notify();
	timer->Start(60000);

    return true;
}

int MyApp::OnExit()
{
	exit(0);
	return 0;
}

MyTimer::MyTimer(void) : 
	wxTimer()
{
	capa=0;
	resetDisplayedUpdateInfo();
}

void MyTimer::Notify()
{
	static bool working=false;
	if(working==true)
	{
		return;
	}
	if(Connector::isBusy())
	{
		return;
	}
	working=true;

	wxStandardPaths sp;
	static wxString cfgDir=sp.GetUserDataDir();
	static long starttime=wxGetLocalTime();
	static long startuptime_passed=0;
	static long lastbackuptime=-5*60*1000;
	static long lastversioncheck=starttime;

	if(!wxDir::Exists(cfgDir) )
	{
		wxFileName::Mkdir(cfgDir);
	}

	if(startuptime_passed==0)
	{
		startuptime_passed=atoi(getFile((cfgDir+wxT("/passedtime.cfg") ).ToUTF8().data() ).c_str() );
		startuptime_passed+=atoi(getFile((cfgDir+wxT("/passedtime_new.cfg") ).ToUTF8().data() ).c_str() );
		writestring(nconvert((int)startuptime_passed), (cfgDir+wxT("/passedtime.cfg") ).ToUTF8().data() );
		lastbackuptime=atoi(getFile((cfgDir+wxT("/lastbackuptime.cfg") ).ToUTF8().data() ).c_str() );
		if(lastbackuptime==0)
			lastbackuptime=-5*60*1000;
		std::string update_intv=getFile((cfgDir+wxT("/incr_updateintervall.cfg") ).ToUTF8().data() );
		if(!update_intv.empty())
			incr_update_intervall=atoi(update_intv.c_str());
	}

	long ct=wxGetLocalTime();

	if(ct-lastversioncheck>300 && !displayed_update_info)
	{
		std::string n_version=getFile("version.txt");
		std::string c_version=getFile("curr_version.txt");
		if(n_version.empty())n_version="0";
		if(c_version.empty())c_version="0";

		if( atoi(n_version.c_str())>atoi(c_version.c_str()))
		{
#ifndef wxUSE_TASKBARICON_BALLOONS
			TaskBarBaloon *tbb=new TaskBarBaloon(_("UrBackup: Update verfügbar"), _("Eine neue Version von UrBackup ist verfügbar. Klicken Sie hier um diese zu installieren"));
			tbb->showBaloon(80000);
#else
			tray->BalloonActionUpgrade();
			tray->ShowBalloon(_("UrBackup: Update verfügbar"), _("Eine neue Version von UrBackup ist verfügbar. Klicken Sie hier um diese zu installieren"), 30000, wxICON_INFORMATION);
			displayed_update_info=true;
#endif
		}
		ct=wxGetLocalTime();
		lastversioncheck=ct;
	}

	long passed=( ct-starttime );

	writestring(nconvert((int)passed), (cfgDir+wxT("/passedtime_new.cfg") ).ToUTF8().data() );

	wxString status_text;
	SStatus status=Connector::getStatus();

	if(Connector::hasError() )
	{
		if(icon_type!=4)
		{
			last_status=_("Keine Verbindung zum Backupserver möglich");
			if(tray!=NULL)
				tray->SetIcon(wxIcon(res_path+wxT("backup-bad.")+ico_ext, ico_type), last_status);
			icon_type=4;
		}
		working=false;
		return;
	}

	capa=status.capa;

	int last_icon_type=icon_type;
	bool refresh=false;
	
	if(status.status==wxT("DONE") )
	{
		writestring(nconvert((int)startuptime_passed+(int)passed), (cfgDir+wxT("/lastbackuptime.cfg") ).ToUTF8().data() );
		lastbackuptime=startuptime_passed+passed;
		icon_type=0;
		working_status=0;
		refresh=true;
	}
	else if(status.status==wxT("INCR") )
	{
		status_text+=_("Inkrementelles Backup läuft. ");
		if(!status.pcdone.empty())
		{
			status_text+=status.pcdone;
			status_text+=_("% fertig. ");
		}
		icon_type=1;
		working_status=1;

	}
	else if(status.status==wxT("FULL") )
	{
		status_text+=_("Volles Backup läuft. ");
		if(!status.pcdone.empty())
		{
			status_text+=status.pcdone;
			status_text+=_("% fertig. ");
		}
		icon_type=1;
		working_status=2;
	}
	else if(status.status==wxT("INCRI") )
	{
		status_text+=_("Inkrementelles Image-Backup läuft. ");
		if(!status.pcdone.empty())
		{
			status_text+=status.pcdone;
			status_text+=_("% fertig. ");
		}
		icon_type=1;
		working_status=3;
	}
	else if(status.status==wxT("FULLI") )
	{
		status_text+=_("Volles Image-Backup läuft. ");
		if(!status.pcdone.empty())
		{
			status_text+=status.pcdone;
			status_text+=_("% fertig. ");
		}
		icon_type=1;
		working_status=4;
	}
	else if(startuptime_passed+passed-(long)incr_update_intervall>lastbackuptime)
	{	
		status_text+=_("Kein aktuelles Backup. ");
		icon_type=2;
		working_status=0;
	}
	else
	{
		icon_type=0;
		working_status=0;
	}

	if(!status.lastbackupdate.Trim().empty() )
		status_text+=_("Letztes Backup am ")+status.lastbackupdate;

	if( icon_type<3 && incr_update_done==false)
	{
		unsigned int n_incr_update_intervall=Connector::getIncrUpdateIntervall();
		if(!Connector::hasError() && n_incr_update_intervall!=0)
		{
			incr_update_done=true;
			incr_update_intervall=n_incr_update_intervall;
			writestring(nconvert(incr_update_intervall), (cfgDir+wxT("/incr_updateintervall.cfg") ).ToUTF8().data() );
		}
	}

	if(status.pause && icon_type==1)
	{
		icon_type=3;
	}

	if(icon_type!=last_icon_type || last_status!=status_text || refresh)
	{
		last_status=status_text;
		switch(icon_type)
		{
		case 0:
			if(tray!=NULL)
				tray->SetIcon(wxIcon(res_path+wxT("backup-ok.")+ico_ext, ico_type), status_text);
			if(timer!=NULL)
				timer->Start(60000);
			break;
		case 1:
			if(tray!=NULL)
				tray->SetIcon(wxIcon(res_path+wxT("backup-progress.")+ico_ext, ico_type), status_text);
			if(timer!=NULL)
				timer->Start(10000);
			break;
		case 2:
			if(tray!=NULL)
				tray->SetIcon(wxIcon(res_path+wxT("backup-bad.")+ico_ext, ico_type), status_text);
			if(timer!=NULL)
				timer->Start(60000);
			break;
		case 3:
			if(tray!=NULL)
				tray->SetIcon(wxIcon(res_path+wxT("backup-progress-pause.")+ico_ext, ico_type), status_text);

			if(timer!=NULL)
				timer->Start(60000);
		}
	}

	if(!status.new_server.empty())
	{
#ifndef wxUSE_TASKBARICON_BALLOONS
			TaskBarBaloon *tbb=new TaskBarBaloon(_("UrBackup: Neuer Server"), _("Ein neuer Backup Server wurde gefunden. Hier klicken um diesen zu benutzen"), status.new_server);
			tbb->showBaloon(80000);
#else
			tray->BalloonActionNewServer(status.new_server);
			tray->ShowBalloon(_("UrBackup: Neuer Server"), _("Ein neuer Backup Server wurde gefunden. Hier klicken um diesen zu benutzen"), 80000, wxICON_INFORMATION);
#endif
	}

	working=false;
}

bool MyTimer::hasCapability(int capa_bit)
{
	return (capa & capa_bit)>0;
}

void MyTimer::resetDisplayedUpdateInfo(void)
{
	displayed_update_info=false;
}

#ifndef DD_RELEASE
int main(int argc, char *argv[])
{
	wxEntry(argc, argv);
}
#else
IMPLEMENT_APP(MyApp)
#endif