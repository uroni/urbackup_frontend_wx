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
#include "Settings.h"
#include "Logs.h"
#include "TranslationHelper.h"
#include <iostream>
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/log.h>

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
std::string ConvertToUTF8(const std::wstring &input);

TrayIcon *tray = NULL;
MyTimer *timer = NULL;
wxString last_status;
unsigned int incr_update_intervall=2*60*60+10*60;
bool backup_is_running=false;
extern bool b_is_pausing;

#ifndef DD_RELEASE
IMPLEMENT_APP_NO_MAIN(MyApp)
#endif

#ifndef _WIN32
#undef wxUSE_TASKBARICON_BALLOONS
#endif

#ifdef __APPLE__
extern "C" void bring_to_foreground();
#endif

class TheFrame : public wxFrame {
public:
    TheFrame(void) : wxFrame(NULL, -1, wxT("UrBackupGUI")) { }
};

namespace
{
	enum ETrayIcon
	{
		ETrayIcon_OK,
		ETrayIcon_PROGRESS,
		ETrayIcon_ERROR,
		ETrayIcon_NO_SERVER,
		ETrayIcon_INDEXING,
		ETrayIcon_PAUSED,
		ETrayIcon_NO_RECENT
	};

	ETrayIcon icon_type=ETrayIcon_OK;

	int needs_restore_restart = 0;
}

std::string g_lang="en";
wxString res_path;
std::string g_res_path;
#if defined(_WIN32) || defined(__APPLE__)
wxString ico_ext=wxT("ico");
wxBitmapType ico_type=wxBITMAP_TYPE_ICO;
#else
wxString ico_ext=wxT("xpm");
wxBitmapType ico_type=wxBITMAP_TYPE_XPM;
#endif

wxIcon getAppIcon(wxString fn)
{
	int small_sys_x=wxSystemSettings::GetMetric(wxSYS_SMALLICON_X);
	int small_sys_y=wxSystemSettings::GetMetric(wxSYS_SMALLICON_X);
	/*wxIconBundle bundle(res_path+fn+wxT(".")+ico_ext);
	return bundle.GetIcon(wxSize(sys_x, sys_y), wxIconBundle::FALLBACK_NEAREST_LARGER);*/
	return wxIcon(res_path+fn+wxT(".")+ico_ext, ico_type, small_sys_x, small_sys_y);
}

namespace
{
	wxString getPercentDoneText(wxString pc)
	{
		wxString pcdone=wxString(_("_percent_ done."))+wxT(" ");
		pcdone.Replace(wxT("_percent_"), pc + wxT("%"));
		return pcdone;
	}
}

void deleteShellKeys()
{
#ifdef _WIN32
	bool deleted_key;
	size_t i=0;
	do 
	{
		deleted_key=false;
		if(RegDeleteKeyW(HKEY_CLASSES_ROOT, (L"AllFilesystemObjects\\shell\\urbackup.access."+convert(i)).c_str())==ERROR_SUCCESS)
		{
			deleted_key=true;
		}
		++i;
	} while (deleted_key);
#endif
}

bool MyApp::OnInit()
{
	wxLog::SetLogLevel(0);
#ifdef _WIN32
	wchar_t buf[MAX_PATH];
	GetModuleFileNameW(NULL, buf, MAX_PATH);
	//SetCurrentDirectoryW(ExtractFilePath(buf).c_str() );
	g_res_path=ConvertToUTF8(ExtractFilePath(buf))+"\\";
	res_path=wxString(g_res_path.c_str(), wxMBConvUTF8());
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
	case wxLANGUAGE_RUSSIAN:
	case wxLANGUAGE_RUSSIAN_UKRAINE:
		lang=wxLANGUAGE_RUSSIAN;
		g_lang="ru";
		break;
	case wxLANGUAGE_SPANISH:
	case wxLANGUAGE_SPANISH_ARGENTINA:
	case wxLANGUAGE_SPANISH_BOLIVIA:
	case wxLANGUAGE_SPANISH_CHILE:
	case wxLANGUAGE_SPANISH_COLOMBIA:
	case wxLANGUAGE_SPANISH_COSTA_RICA:
	case wxLANGUAGE_SPANISH_DOMINICAN_REPUBLIC:
	case wxLANGUAGE_SPANISH_ECUADOR:
	case wxLANGUAGE_SPANISH_EL_SALVADOR:
	case wxLANGUAGE_SPANISH_GUATEMALA:
	case wxLANGUAGE_SPANISH_HONDURAS:
	case wxLANGUAGE_SPANISH_MEXICAN:
	case wxLANGUAGE_SPANISH_MODERN:
	case wxLANGUAGE_SPANISH_NICARAGUA:
	case wxLANGUAGE_SPANISH_PANAMA:
	case wxLANGUAGE_SPANISH_PARAGUAY:
	case wxLANGUAGE_SPANISH_PERU:
	case wxLANGUAGE_SPANISH_PUERTO_RICO:
	case wxLANGUAGE_SPANISH_URUGUAY:
	case wxLANGUAGE_SPANISH_US:
	case wxLANGUAGE_SPANISH_VENEZUELA:
		lang=wxLANGUAGE_SPANISH;
		g_lang="es";
		break;
	case wxLANGUAGE_CHINESE:
    case wxLANGUAGE_CHINESE_SIMPLIFIED:
    case wxLANGUAGE_CHINESE_HONGKONG:
    case wxLANGUAGE_CHINESE_MACAU:
    case wxLANGUAGE_CHINESE_SINGAPORE:
		lang=wxLANGUAGE_CHINESE_SIMPLIFIED;
		g_lang="zh_CN";
		break;
	case wxLANGUAGE_CHINESE_TRADITIONAL:
	case wxLANGUAGE_CHINESE_TAIWAN:
		lang=wxLANGUAGE_CHINESE_TRADITIONAL;
		g_lang="zh_TW";
		break;
	case wxLANGUAGE_PORTUGUESE_BRAZILIAN:
		lang=wxLANGUAGE_PORTUGUESE_BRAZILIAN;
		g_lang="pt_BR";
		break;
	case wxLANGUAGE_ITALIAN:
	case wxLANGUAGE_ITALIAN_SWISS:
		lang=wxLANGUAGE_ITALIAN;
		g_lang="it";
		break;
	case wxLANGUAGE_POLISH:
		lang=wxLANGUAGE_POLISH;
		g_lang="pl";
		break;
	case wxLANGUAGE_SLOVAK:
		lang=wxLANGUAGE_SLOVAK;
		g_lang="sk";
		break;
	case wxLANGUAGE_UKRAINIAN:
		lang=wxLANGUAGE_UKRAINIAN;
		g_lang="uk";
		break;
	case wxLANGUAGE_DANISH:
		lang=wxLANGUAGE_DANISH;
		g_lang="da";
		break;
	case wxLANGUAGE_DUTCH:
	case wxLANGUAGE_DUTCH_BELGIAN:
		lang=wxLANGUAGE_DUTCH;
		g_lang="nl";
		break;
	case wxLANGUAGE_FARSI:
		lang=wxLANGUAGE_FARSI;
		g_lang="fa";
		break;
	case wxLANGUAGE_CZECH:
		lang=wxLANGUAGE_CZECH;
		g_lang="cz";
		break;
	}

	//std::cout << "Lang: " << g_lang << std::endl;
#ifndef _WIN32
	m_locale.AddCatalogLookupPathPrefix(res_path);
#else
	m_locale.AddCatalogLookupPathPrefix(res_path+wxT("lang"));
#endif
	m_locale.AddCatalog(L"urbackup");
	m_locale.Init(lang, 0);
	
	if(!m_locale.IsOk() )
	{
		std::cout << "Selected language is wrong!" << std::endl;
	}

	wxImage::AddHandler(new wxPNGHandler());
	wxImage::AddHandler(new wxICOHandler());
	//wxInitAllImageHandlers();

	timer=NULL;

	wxString cmd;

	if(argc>1)
	{
		cmd=argv[1];
	}
	else
	{
		writestring(_("&Access backups").ToUTF8().data(), "access_backups_shell_mui.txt");
	}

	#ifdef __WXMAC__
	if(!cmd.empty())
	{
    	bring_to_foreground();
	}
	#endif 

	if(cmd.empty())
	{
		SetTopWindow(new TheFrame);

		tray=new TrayIcon;
		bool b=tray->SetIcon(getAppIcon(wxT("backup-ok")), wxT("UrBackup Client"));
		if(!b)
		{
			std::cout << "Setting icon failed." << std::endl;
		}

		timer=new MyTimer;

		timer->Notify();
		timer->Start(10);
	}
	else if(cmd==wxT("settings"))
	{
		Settings *s=new Settings(NULL);
		SetTopWindow(s);
		s->ShowModal();
		s->Destroy();
		wxExit();
	}
	else if(cmd==wxT("paths"))
	{
		ConfigPath *cp=new ConfigPath(NULL);
		SetTopWindow(cp);
		cp->ShowModal();
		cp->Destroy();
		wxExit();
	}
	else if(cmd==wxT("logs"))
	{
		Logs *l=new Logs(NULL);
		SetTopWindow(l);
		l->ShowModal();
		l->Destroy();
		wxExit();
	}
	else if(cmd==wxT("newserver"))
	{
		if(argc<3)
		{
			return false;
		}
		Connector::addNewServer(wxString(argv[2]).ToStdString());
		wxExit();
	}
	else if(cmd==wxT("access"))
	{
		if(argc<3)
		{
			return false;
		}
		TrayIcon::accessBackups(wxString(argv[2]));
		wxExit();
	}
	else if(cmd==wxT("deleteshellkeys"))
	{
		deleteShellKeys();
		wxExit();
	}
	else if(cmd==wxT("restoreok"))
	{
		Connector::restoreOk(wxString(argv[2])=="true");
		wxExit();
	}
	else
	{
		return false;
	}

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

wxString getStatusText(wxString status)
{
	if(status==wxT("INCR") )
	{
		return _("Incremental file backup running.");
	}
	else if(status==wxT("FULL") )
	{
		return _("Full file backup running.");
	}
	else if(status==wxT("INCRI") )
	{
		return _("Incremental image backup running.");
	}
	else if(status==wxT("FULLI") )
	{
		return _("Full image backup running.");
	}
	else if(status==wxT("R_INCR"))
	{
		return _("Resumed incremental file backup running.");
	}
	else if(status==wxT("R_FULL"))
	{
		return _("Resumed full file backup running.");
	}
	return wxT("");
}

wxString getPercentText(wxString pcdone)
{
	wxString ret;
	if(!pcdone.empty())
	{
		if(pcdone!=wxT("-1"))
		{
			ret+=getPercentDoneText(pcdone);
		}
		else
		{
			ret+=wxString(_("Indexing."))+wxT(" ");
		}
	}
	return ret;
}

namespace
{
	wxString getIconName(ETrayIcon selicon)
	{
		switch(selicon)
		{
		case ETrayIcon_OK: return wxT("backup-ok");
		case ETrayIcon_PROGRESS: return wxT("backup-progress");
		case ETrayIcon_ERROR: return wxT("backup-bad");
		case ETrayIcon_NO_SERVER: return wxT("backup-no-server");
		case ETrayIcon_INDEXING: return wxT("backup-indexing");
		case ETrayIcon_PAUSED: return wxT("backup-progress-pause");
		case ETrayIcon_NO_RECENT: return wxT("backup-no-recent");
		default: wxEmptyString;
		}

		return wxEmptyString;
	}
}

void MyTimer::Notify()
{
	static bool working=false;
	if(working==true)
	{
		return;
	}

	working=true;

	wxStandardPathsBase& sp=wxStandardPaths::Get();
	static wxString cfgDir=sp.GetUserDataDir();
	static long lastversioncheck=wxGetLocalTime();

	if(!wxDir::Exists(cfgDir) )
	{
		wxFileName::Mkdir(cfgDir);
	}

	long ct=wxGetLocalTime();

	if(ct-lastversioncheck>600 && !displayed_update_info)
	{
		std::string n_version=getFile("version.txt");
		std::string c_version=getFile("curr_version.txt");
		if(n_version.empty())n_version="0";
		if(c_version.empty())c_version="0";

		if( atoi(n_version.c_str())>atoi(c_version.c_str()))
		{
#ifndef wxUSE_TASKBARICON_BALLOONS
			TaskBarBaloon *tbb=new TaskBarBaloon(_("UrBackup: Update available"), _("A new version of UrBackup is available. Please click here to install it now."));
			tbb->showBaloon(80000);
#else
			tray->BalloonActionUpgrade();
			tray->ShowBalloon(_("UrBackup: Update available"), _("A new version of UrBackup is available. Please click here to install it now."), 30000, wxICON_INFORMATION);
			displayed_update_info=true;
#endif
		}
		lastversioncheck=ct;
	}

	wxString status_text;
	SStatus status=Connector::getStatus(60000);

	if(Connector::hasError() )
	{
		if(icon_type!=ETrayIcon_ERROR)
		{
			last_status=_("Cannot connect to backup server");
			icon_type=ETrayIcon_ERROR;
			if(tray!=NULL)
				tray->SetIcon(getAppIcon(getIconName(icon_type)), last_status);
		}
		working=false;
		return;
	}

	capa=status.capa;

	ETrayIcon last_icon_type=icon_type;
	bool refresh=false;
	
	if(status.status==wxT("DONE") )
	{
		icon_type=ETrayIcon_OK;
		backup_is_running=false;
		refresh=true;
	}
	else if(status.status==wxT("INCR") || 
		status.status==wxT("FULL") ||
		status.status==wxT("INCRI") ||
		status.status==wxT("FULLI") ||
		status.status==wxT("R_INCR") ||
		status.status==wxT("R_FULL"))
	{
		status_text+=getStatusText(status.status)+wxT(" ");
		status_text+=getPercentText(status.pcdone);
		icon_type=ETrayIcon_PROGRESS;
		backup_is_running=true;

	}
	else if(status.status==wxT("NO_RECENT"))
	{	
		status_text+=wxString(_("No current backup.")) + wxT(" ");
		icon_type=ETrayIcon_NO_RECENT;
		backup_is_running=false;
	}
	else
	{
		icon_type=ETrayIcon_OK;
		backup_is_running=false;
	}

	if(!status.lastbackupdate.Trim().empty() )
	{
		wxLongLong_t lastbackups;
		if(status.lastbackupdate.ToLongLong(&lastbackups))
		{
			wxDateTime lastbackup(lastbackups);

			status_text+=trans_1(_("Last backup on _1_"), lastbackup.Format());
		}	
	}

	if(status.pause && icon_type==ETrayIcon_PROGRESS)
	{
		icon_type=ETrayIcon_PAUSED;
	}
	else if(icon_type==ETrayIcon_PROGRESS && status.pcdone==wxT("-1"))
	{
		icon_type=ETrayIcon_INDEXING;
	}
	else if( (icon_type==ETrayIcon_NO_RECENT || icon_type==ETrayIcon_OK)
		&& !status.has_server )
	{
		icon_type=ETrayIcon_NO_SERVER;
	}

	b_is_pausing=status.pause;
	

	if(icon_type!=last_icon_type || last_status!=status_text || refresh)
	{
		last_status=status_text;

		if(tray!=NULL)
		{
			tray->SetIcon(getAppIcon(getIconName(icon_type)), status_text);
		}
		if(timer!=NULL)
		{
			timer->Start(10);
		}
	}

	if(!status.new_server.empty() && tray)
	{
#ifndef wxUSE_TASKBARICON_BALLOONS
			TaskBarBaloon *tbb=new TaskBarBaloon(_("UrBackup: New server"), _("A new backup server has been found. Click here to use it"), status.new_server);
			tbb->showBaloon(180000);
#else
			tray->BalloonActionNewServer(status.new_server);
			tray->ShowBalloon(_("UrBackup: New server"), _("A new backup server has been found. Click here to use it"), 180000, wxICON_INFORMATION);
#endif
	}

	if(status.ask_restore_ok)
	{
		wxMessageDialog* dialog = new wxMessageDialog(NULL,
			_("Are you sure you want to restore the selected files? Existing files will be overwritten. Files created within the selected folder since the backup will be deleted. When in doubt please cancel and run a file backup before proceeding."), wxT("UrBackup - Allow restore"), wxOK | wxCANCEL);
		if(dialog->ShowModal() == wxOK)
		{
			Connector::restoreOk(true);
		}
		else
		{
			Connector::restoreOk(false);
		}
	}

#ifdef _WIN32
	if(status.needs_restore_restart>needs_restore_restart)
	{
		needs_restore_restart = status.needs_restore_restart;

		wxMessageDialog* dialog = new wxMessageDialog(NULL,
			_("Some files could not be deleted or overwritten during the restore process. In order to overwrite/delete the files the system needs to be restarted. Do you want to do this now?"), wxT("UrBackup - Restart Windows"), wxOK | wxCANCEL);
		if(dialog->ShowModal() == wxOK)
		{
			ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_APPLICATION|SHTDN_REASON_MINOR_OTHER );
		}
	}
#endif

	working=false;
}

bool MyTimer::hasCapability(int capa_bit)
{
	return MyTimer::hasCapability(capa_bit, capa);
}

bool MyTimer::hasCapability( int capa_bit, int capa )
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
#ifdef _WIN32
	 HANDLE gmutex = CreateMutex(NULL, TRUE, L"Local\\UrBackupClientGUI");
	 if( gmutex!=NULL && GetLastError()==ERROR_ALREADY_EXISTS )
	 {
		 return 68;
	 }
#endif

	wxEntry(argc, argv);
}
#else
IMPLEMENT_APP(MyApp)
#endif