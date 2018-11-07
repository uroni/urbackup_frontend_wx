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
#include "ConfigPath.h"
#include "main.h"
#include "stringtools.h"
#include "TaskBarBaloon.h"
#include "Settings.h"
#include "Logs.h"
#include "TranslationHelper.h"
#include "Status.h"
#include "CbtStatus.h"
#include <iostream>
#include <limits>
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/socket.h>

#ifndef _WIN32
#include "../config.h"
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/file.h>
#include <errno.h>
#endif

#ifdef _WIN32
#include "SelectWindowsComponents.h"
#include "SelectRestoreWindowsComponents.h"
#include <Shlobj.h>
#endif

#include <wx/apptrait.h>
/*#if wxUSE_STACKWALKER && defined( __WXDEBUG__ )
// silly workaround for the link error with debug configuration:
// \src\common\appbase.cpp
wxString wxAppTraitsBase::GetAssertStackTrace()
{
   return wxT("");
}
#endif*/

wxDECLARE_APP(MyApp);

#ifdef _WIN32
#include <windows.h>
#endif
#include "SetupWizard.h"

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

#ifdef _WIN32

HRESULT initCom()
{
	HRESULT hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
		RPC_C_IMP_LEVEL_IDENTIFY, NULL, EOAC_NONE, NULL);

	if (hr != S_OK)
	{
		wxMessageBox(wxString::Format(_("CoInitializeSecurity failed: %08x"), hr), wxT("UrBackup"), wxOK | wxICON_ERROR);
	}

	return hr;
}

HRESULT ModifyPrivilege(
	IN LPCTSTR szPrivilege,
	IN BOOL fEnable)
{
	HRESULT hr = S_OK;
	TOKEN_PRIVILEGES NewState;
	LUID             luid;
	HANDLE hToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken))
	{
		return ERROR_FUNCTION_FAILED;
	}
	if (!LookupPrivilegeValue(NULL,
		szPrivilege,
		&luid))
	{
		CloseHandle(hToken);
		return ERROR_FUNCTION_FAILED;
	}
	NewState.PrivilegeCount = 1;
	NewState.Privileges[0].Luid = luid;
	NewState.Privileges[0].Attributes =
		(fEnable ? SE_PRIVILEGE_ENABLED : 0);
	if (!AdjustTokenPrivileges(hToken,
		FALSE,
		&NewState,
		0,
		NULL,
		NULL))
	{
		hr = ERROR_FUNCTION_FAILED;
	}
	CloseHandle(hToken);
	return hr;
}
#endif

#ifdef __APPLE__
extern "C" void bring_to_foreground();
extern "C" void register_login_item();
extern "C" void remove_login_item();
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
	int ask_restore_ok = 0;
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

void startOnlyOnce()
{
#ifndef _WIN32
	const char *homedir = NULL;

	if ((homedir = getenv("HOME")) == NULL)
	{
		struct passwd *pw = getpwuid(getuid());
		if (pw != NULL)
		{
			homedir = pw->pw_dir;
		}
	}

	if (homedir == NULL)
	{
		return;
	}

	int fd = open((std::string(homedir) + "/.urbackupclientgui_startonce").c_str(), O_CREAT | O_RDWR, S_IRWXU);

	if (fd == -1)
	{
		return;
	}

	int rc = flock(fd, LOCK_EX | LOCK_NB);

	if (rc != 0 && errno==EWOULDBLOCK)
	{
		std::cout << "Application is already started" << std::endl;
		close(fd);
		exit(30);
	}
#else
	PWSTR fpathb;
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &fpathb);
	if (hr != S_OK)
	{
		return;
	}

	std::wstring fpath = fpathb;

	CoTaskMemFree(fpathb);

	HANDLE hFile = CreateFileW((fpath + "\\.urbackupclientgui_startonce").c_str(),
		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}


	if (!LockFile(hFile, 0, 0, 100, 0))
	{
		std::cout << "Application is already started" << std::endl;
		CloseHandle(hFile);
		exit(30);
	}
#endif
}

bool MyApp::OnInit()
{
	wxLog::SetLogLevel(0);
	wxSocketBase::Initialize();
#ifdef _WIN32
	wchar_t buf[MAX_PATH];
	GetModuleFileNameW(NULL, buf, MAX_PATH);
	//SetCurrentDirectoryW(ExtractFilePath(buf).c_str() );
	g_res_path=ConvertToUTF8(ExtractFilePath(buf))+"\\";
	res_path=wxString(g_res_path.c_str(), wxMBConvUTF8());
#else
	res_path=wxT(DATADIR "/urbackup/");
	g_res_path= DATADIR "/urbackup/";
#endif
	wxLanguage lang=wxLANGUAGE_ENGLISH;
	wxLanguage sysdef=(wxLanguage)wxLocale::GetSystemLanguage();
	switch(sysdef)
	{
	case wxLANGUAGE_ENGLISH:
	case wxLANGUAGE_ENGLISH_UK:
	case wxLANGUAGE_ENGLISH_US:
	case wxLANGUAGE_ENGLISH_AUSTRALIA:
	case wxLANGUAGE_ENGLISH_BELIZE:
	case wxLANGUAGE_ENGLISH_BOTSWANA:
	case wxLANGUAGE_ENGLISH_CANADA:
	case wxLANGUAGE_ENGLISH_CARIBBEAN:
	case wxLANGUAGE_ENGLISH_DENMARK:
	case wxLANGUAGE_ENGLISH_EIRE:
	case wxLANGUAGE_ENGLISH_JAMAICA:
	case wxLANGUAGE_ENGLISH_NEW_ZEALAND:
	case wxLANGUAGE_ENGLISH_PHILIPPINES:
	case wxLANGUAGE_ENGLISH_SOUTH_AFRICA:
	case wxLANGUAGE_ENGLISH_TRINIDAD:
	case wxLANGUAGE_ENGLISH_ZIMBABWE:
		lang = sysdef;
		break;
	case wxLANGUAGE_GERMAN:
	case wxLANGUAGE_GERMAN_AUSTRIAN:
	case wxLANGUAGE_GERMAN_BELGIUM:
	case wxLANGUAGE_GERMAN_LIECHTENSTEIN:
	case wxLANGUAGE_GERMAN_LUXEMBOURG:
	case wxLANGUAGE_GERMAN_SWISS:
		lang= sysdef;
		g_lang="de";
		break;
	case wxLANGUAGE_FRENCH:
	case wxLANGUAGE_FRENCH_BELGIAN:
	case wxLANGUAGE_FRENCH_CANADIAN:
	case wxLANGUAGE_FRENCH_LUXEMBOURG:
	case wxLANGUAGE_FRENCH_MONACO:
	case wxLANGUAGE_FRENCH_SWISS:
		lang= sysdef;
		g_lang="fr";
		break;
	case wxLANGUAGE_RUSSIAN:
	case wxLANGUAGE_RUSSIAN_UKRAINE:
		lang= sysdef;
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
		lang= sysdef;
		g_lang="es";
		break;
	case wxLANGUAGE_CHINESE:
    case wxLANGUAGE_CHINESE_SIMPLIFIED:
    case wxLANGUAGE_CHINESE_HONGKONG:
    case wxLANGUAGE_CHINESE_MACAU:
    case wxLANGUAGE_CHINESE_SINGAPORE:
		lang= sysdef;
		g_lang="zh_CN";
		break;
	case wxLANGUAGE_CHINESE_TRADITIONAL:
	case wxLANGUAGE_CHINESE_TAIWAN:
		lang= sysdef;
		g_lang="zh_TW";
		break;
	case wxLANGUAGE_PORTUGUESE_BRAZILIAN:
		lang= sysdef;
		g_lang="pt_BR";
		break;
	case wxLANGUAGE_PORTUGUESE:
		lang= sysdef;
		g_lang="pt";
		break;
	case wxLANGUAGE_ITALIAN:
	case wxLANGUAGE_ITALIAN_SWISS:
		lang= sysdef;
		g_lang="it";
		break;
	case wxLANGUAGE_POLISH:
		lang= sysdef;
		g_lang="pl";
		break;
	case wxLANGUAGE_SLOVAK:
		lang= sysdef;
		g_lang="sk";
		break;
	case wxLANGUAGE_UKRAINIAN:
		lang= sysdef;
		g_lang="uk";
		break;
	case wxLANGUAGE_DANISH:
		lang= sysdef;
		g_lang="da";
		break;
	case wxLANGUAGE_DUTCH:
	case wxLANGUAGE_DUTCH_BELGIAN:
		lang= sysdef;
		g_lang="nl";
		break;
	case wxLANGUAGE_FARSI:
		lang= sysdef;
		g_lang="fa";
		break;
	case wxLANGUAGE_CZECH:
		lang= sysdef;
		g_lang="cz";
		break;
	case wxLANGUAGE_ESTONIAN:
		lang= sysdef;
		g_lang="et";
		break;
	case wxLANGUAGE_TURKISH:
		lang= sysdef;
		g_lang="tr";
		break;
	case wxLANGUAGE_NORWEGIAN_BOKMAL:
	case wxLANGUAGE_NORWEGIAN_NYNORSK:
		lang= sysdef;
		g_lang="no_NO";
		break;
	case wxLANGUAGE_SWEDISH:
	case wxLANGUAGE_SWEDISH_FINLAND:
		lang= sysdef;
		g_lang="sv";
		break;
	case wxLANGUAGE_FINNISH:
		lang = sysdef;
		g_lang = "fi";
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

	if(cmd=="daemon")
	{
		cmd.clear();
	}

#ifdef _WIN32
	if(cmd.empty())
	{
		writestring(_("&Access/restore backups").ToUTF8().data(), "access_backups_shell_mui.txt");
	}
#endif

	#ifdef __WXMAC__
	if(!cmd.empty())
	{
    	bring_to_foreground();
	}
	#endif 

	if(cmd.empty())
	{
		startOnlyOnce();
		SetTopWindow(new TheFrame);

		tray=new TrayIcon;
		bool b=tray->SetIcon(getAppIcon(wxT("backup-ok")), wxT("UrBackup Client"));
		if(!b)
		{
			std::cout << "Setting icon failed." << std::endl;
		}

		timer=new MyTimer;

		timer->Start(1000);
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
	else if(cmd==wxT("setupWizard"))
	{
		if(SetupWizard::runSetupWizard())
		{
			SetupWizard* sw = new SetupWizard(NULL);
			SetTopWindow(sw);
			sw->ShowPage(*(sw->m_pages.begin()));
			sw->SetFocus();
			sw->Raise();
			sw->ShowModal();
			sw->Destroy();
		}		
		wxExit();
	}
	else if(cmd==wxT("setupDefault"))
	{
		SetupWizard::doDefConfig();
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
		wxLongLong_t process_id;
		Connector::restoreOk(wxString(argv[2])=="true", process_id);
		wxExit();
	}
	else if (cmd == wxT("cbt-status"))
	{
		CbtStatus *s = new CbtStatus(NULL);
		SetTopWindow(s);
		s->ShowModal();
		s->Destroy();
		wxExit();
	}
#ifdef __APPLE__
	else if (cmd == wxT("uninstall"))
	{
		std::string uninstaller = SBINDIR "/urbackup_uninstall";
		wxExecute("/bin/sh \"" + uninstaller + "\"", wxEXEC_SYNC, NULL, NULL);
		wxExit();
	}
	else if(cmd==wxT("register_login_item"))
	{
		register_login_item();
		exit(0);
	}
	else if(cmd==wxT("remove_login_item"))
	{
		remove_login_item();
		exit(0);
	}
#endif
#ifdef _WIN32
	else if (cmd == wxT("selectWindowsComponents"))
	{
		initCom();

		SelectWindowsComponents *cp = new SelectWindowsComponents(NULL);
		SetTopWindow(cp);
		cp->ShowModal();
		cp->Destroy();
		wxExit();
	}
	else if (cmd == wxT("restoreWindowsComponents"))
	{
		initCom();

		SelectRestoreComponents *cp = new SelectRestoreComponents(NULL);
		SetTopWindow(cp);
		cp->ShowModal();
		cp->Destroy();
		wxExit();
	}
#endif
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
	wxTimer(), first_status(true)
{
	capa=INT_MAX;
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
	else if (status == wxT("RESTORE_FILES"))
	{
		return _("Restoring files.");
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
	static SStatus status;
	if(!status.init || status.hasError())
	{
		status = Connector::initStatus(status.client, first_status, 60000);
		first_status = false;
	}

	if(Connector::hasError() )
	{
		if(icon_type!=ETrayIcon_ERROR)
		{
			last_status=_("Cannot connect to backup server");
			icon_type=ETrayIcon_ERROR;
			if(tray!=NULL)
				tray->SetIcon(getAppIcon(getIconName(icon_type)), last_status);
		}
		status.init = false;
		return;
	}

	if(!status.init)
	{
		return;
	}

	if(!status.isAvailable())
	{
		return;
	}

	status.init=false;

	wxStandardPathsBase& sp=wxStandardPaths::Get();
	static long lastversioncheck=wxGetLocalTime();

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

	capa=status.capa;

	ETrayIcon last_icon_type=icon_type;
	bool refresh=false;
	
	if(status.status==wxT("INCR") || 
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
			wxDateTime lastbackup_dt((wxLongLong)(lastbackups*1000));

			status_text+=trans_1(_("Last backup on _1_"), lastbackup_dt.Format());
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
	else if( icon_type==ETrayIcon_NO_RECENT
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

	if(status.ask_restore_ok>ask_restore_ok)
	{
#ifdef __WXMAC__
		bring_to_foreground();
#endif 
		ask_restore_ok = status.ask_restore_ok;

		wxString ask_msg;

		if (status.restore_file)
		{
			ask_msg = trans_1(_("Are you sure you want to restore the file at _1_? The current file content will be overwritten by the backup file content. When in doubt please cancel and run a file backup before proceeding."), status.restore_path);
		}
		else
		{
			ask_msg = trans_1(_("Are you sure you want to restore the files in _1_? Existing files in this folder will be overwritten. Files created within the selected folder since the backup will be deleted. When in doubt please cancel and run a file backup before proceeding."), status.restore_path);
		}

		wxMessageDialog* dialog = new wxMessageDialog(NULL,
			ask_msg, _("UrBackup - Allow restore"), wxOK | wxCANCEL);
#if !defined(__WXMAC__) && !defined(__linux__)
		dialog->RequestUserAttention();
#endif
		int rc = dialog->ShowModal();
		dialog->Destroy();
		if(rc == wxID_OK)
		{
			wxLongLong_t process_id = 0;
			Connector::restoreOk(true, process_id);

			if (process_id != 0)
			{
				new Status(NULL, process_id);
			}
		}
		else
		{
			wxLongLong_t process_id;
			Connector::restoreOk(false, process_id);
		}
	}

#ifdef _WIN32
	if(status.needs_restore_restart>needs_restore_restart)
	{
#ifdef __WXMAC__
		bring_to_foreground();
#endif
		needs_restore_restart = status.needs_restore_restart;

		wxMessageDialog* dialog = new wxMessageDialog(NULL,
			_("Some files could not be deleted or overwritten during the restore process. In order to overwrite/delete the files the system needs to be restarted. Do you want to do this now?"), _("UrBackup - Restart Windows"), wxOK | wxCANCEL);
	#if !defined(__WXMAC__) && !defined(__linux__)
		dialog->RequestUserAttention();
	#endif
		if(dialog->ShowModal() == wxID_OK)
		{
			ModifyPrivilege(SE_SHUTDOWN_NAME, TRUE);
			ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_APPLICATION|SHTDN_REASON_MINOR_OTHER );
		}
		dialog->Destroy();
	}
#endif
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
	#ifdef __APPLE__
	if(argc>1 && wxString(argv[1])==wxT("daemon"))
	{
		std::cout << "Daemonizing..." << std::endl;
		//Daemonize. At least that works *tears*
		size_t pid1;
		if( (pid1=fork())==0 )
		{
			setsid();
			if(fork()==0)
			{
				for (int i=getdtablesize();i>=0;--i) close(i);
				int i=open("/dev/null",O_RDWR);
				dup(i);
				dup(i);
			}
			else
			{
				exit(0);
			}
		}
		else
		{
			int status;
			waitpid(pid1, &status, 0);
			exit(0);
		}
	}
	#endif

	wxEntry(argc, argv);
}
#else
IMPLEMENT_APP(MyApp)
#endif