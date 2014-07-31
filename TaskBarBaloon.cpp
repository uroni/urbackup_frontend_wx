#include "TaskBarBaloon.h"
#include <wx/utils.h>
#include <wx/stdpaths.h>
#include "stringtools.h"
#include "main.h"
#include "Connector.h"

const int TIMER_BALOON=34;
extern MyTimer *timer;

BEGIN_EVENT_TABLE(TaskBarBaloon, wxFrame)
    EVT_PAINT(TaskBarBaloon::OnPaint)
    EVT_LEFT_DOWN(TaskBarBaloon::OnClick)
	EVT_KEY_DOWN(TaskBarBaloon::OnEscape)
    EVT_TIMER(TIMER_BALOON,TaskBarBaloon::OnTimerTick)
END_EVENT_TABLE()
 
TaskBarBaloon::TaskBarBaloon(wxString sTitle, wxString sMessage, std::string new_ident)
    : wxFrame(NULL,-1,wxT("no title"),wxDefaultPosition,wxDefaultSize,wxNO_BORDER | wxSTAY_ON_TOP | wxFRAME_SHAPED | wxFRAME_NO_TASKBAR),
	  new_ident(new_ident)
{
    wxColour bgColour(255,255,231); // yellow BG
    this->SetBackgroundColour(bgColour);
    wxBoxSizer * mainSizer = new wxBoxSizer(wxVERTICAL);
 
    wxStaticText * title = new wxStaticText(this, -1, sTitle);
    wxFont titleFont = this->GetFont();
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);
    mainSizer->Add(title,0,wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5);
    title->Connect(wxEVT_LEFT_DOWN,
                   wxMouseEventHandler(TaskBarBaloon::OnClick), NULL, this );
    title->Connect(wxEVT_KEY_DOWN,
                   wxKeyEventHandler(TaskBarBaloon::OnEscape), NULL, this );
 
    wxStaticText * text = new wxStaticText(this, -1, sMessage);
    mainSizer->Add(text,1,wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);
    text->Connect(wxEVT_LEFT_DOWN,
                  wxMouseEventHandler(TaskBarBaloon::OnClick), NULL, this );
    text->Connect(wxEVT_KEY_DOWN,
                   wxKeyEventHandler(TaskBarBaloon::OnEscape), NULL, this );
 
    this->SetSizer(mainSizer);
    mainSizer->SetSizeHints( this );
 
    this->timer = new wxTimer(this,TIMER_BALOON);
 
    // here, we try to align the frame to the right bottom corner
    this->Center();
    int iX = 0, iY = 0;
    this->GetPosition( &iX, &iY );
    iX = (iX * 2) - 2;
    iY = (iY * 2) - 2;
    this->Move( iX, iY );
}
 
void TaskBarBaloon::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
 
    int iWidth = 0, iHeight = 0;
    this->GetClientSize( &iWidth, &iHeight );
 
    wxPen pen(this->GetForegroundColour());
    dc.SetPen(pen);
 
    wxBrush brush(this->GetBackgroundColour());
    dc.SetBrush(brush);
 
    dc.Clear();
    dc.DrawRectangle(0,0,iWidth,iHeight);
}
 
/** closing frame at end of timeout */
void TaskBarBaloon::OnTimerTick(wxTimerEvent & event)
{
    this->Destroy();
}
 
/** showing frame and running timer */
void TaskBarBaloon::showBaloon(unsigned int iTimeout)
{
    this->Show(false);
    this->Show(true);
    this->timer->Start(iTimeout,wxTIMER_ONE_SHOT);
}

#ifdef _WIN32

HANDLE ExecuteProcessO( const std::string & CommandLine, const std::string & WorkDir )
{
   STARTUPINFOA sStartInfo;
   ZeroMemory( &sStartInfo, sizeof(STARTUPINFO) );
   sStartInfo.cb = sizeof(STARTUPINFO);
   sStartInfo.wShowWindow = SW_SHOWDEFAULT;
   sStartInfo.dwFlags = STARTF_USESHOWWINDOW;

   PROCESS_INFORMATION sProcessInfo;
   ZeroMemory( &sProcessInfo, sizeof(PROCESS_INFORMATION) );

   BOOL ok = CreateProcessA( NULL, (char*)CommandLine.c_str(),
                           NULL, NULL, true,
                           NORMAL_PRIORITY_CLASS, NULL, WorkDir.c_str() , &sStartInfo, &sProcessInfo );

   if ( !ok ) {
      sProcessInfo.hProcess = 0;
      return sProcessInfo.hProcess;
   }

   return sProcessInfo.hProcess;
}

HANDLE ExecuteProcess( const std::string & exe, const std::string &args, const std::string & WorkDir )
{
	std::string cmd="\""+exe+"\"";
	if( args!="" )
		cmd+=" "+args;
	HANDLE h=ExecuteProcessO(cmd, WorkDir);
	if( h!=NULL )
		return h;
		
	SHELLEXECUTEINFOA TempInfo = {0};
	TempInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
	TempInfo.fMask = 0; //SEE_MASK_NOCLOSEPROCESS
	TempInfo.hwnd = NULL;
	TempInfo.lpVerb = "runas";
	TempInfo.lpFile = exe.c_str();
	TempInfo.lpParameters = args.c_str();
	TempInfo.lpDirectory = WorkDir.c_str();
	TempInfo.nShow = SW_NORMAL;

	BOOL b=ShellExecuteExA(&TempInfo);
	
	if( b==TRUE )
	{
		return (HANDLE)1;
	}
	else
		return NULL;
}

#endif

void update_urbackup(void)
{
#ifdef _WIN32
	wxStandardPaths sp;
	std::string e_pstr=ExtractFilePath(sp.GetExecutablePath().ToUTF8().data());
	ExecuteProcess(e_pstr+"\\UrBackupUpdate.exe","","");
	timer->resetDisplayedUpdateInfo();
#endif
}

void TaskBarBaloon::OnClick(wxMouseEvent & event)
{
	this->Show(false);
	if(new_ident.empty())
	{
		update_urbackup();
	}
	else
	{
		Connector::addNewServer(new_ident);
	}
}