!define MUI_BRANDINGTEXT "UrBackup 0.22"
!include "${NSISDIR}\Contrib\Modern UI\System.nsh"
!include WinVer.nsh
!include "x64.nsh"
!define MUI_ICON "backup-ok.ico"

SetCompressor /FINAL /SOLID lzma


CRCCheck On
Name "UrBackup 0.22"
OutFile "UrBackup Client 0.22-1-x64.exe"
InstallDir "$PROGRAMFILES\UrBackup"
RequestExecutionLevel highest

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH



!insertmacro MUI_LANGUAGE "German"

Section "install"

	${IfNot} ${RunningX64}
		MessageBox MB_OK "Dies ist die Installationsdatei fuer das 64Bit Betriebssystem. Das momentane Betriebssystem ist nicht 64-bittig. Benutzen sie die andere Installationsdatei."
		Abort "Falsche Installationsdatei"
	${EndIf}

	!insertmacro DisableX64FSRedirection
	SetRegView 64


	StrCpy $0 "UrBackupClient.exe"
	KillProc::KillProcesses
	SimpleSC::StopService "UrBackupServer"
	Pop $0
	SimpleSC::RemoveService "UrBackupServer"
	Pop $0
	
	SetOutPath "$INSTDIR"
	
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	
	CreateDirectory "$SMPROGRAMS\UrBackup"
	CreateShortCut "$SMPROGRAMS\UrBackup\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
	CreateShortCut "$SMPROGRAMS\UrBackup\UrBackup.lnk" "$INSTDIR\UrBackupClient.exe" "" "$INSTDIR\UrBackupClient.exe" 0
	
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" "DisplayName" "UrBackup (remove only)"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" "UninstallString" "$INSTDIR\Uninstall.exe"
	
	File "data\args.txt"
	File "data\backup-bad.ico"
	File "data\backup-ok.ico"
	File "data\backup-ok-big.ico"
	File "data\backup-progress.ico"
	File "data_x64\fileservplugin.dll"
	File "data_x64\fsimageplugin.dll"
	File "data\new.txt"
	File "data_x64\urbackup.dll"
	File "data_x64\UrBackupClient.exe"
	File "data_x64\urbackupsrv.exe"
	File "data\logo1.png"
	File "data\backup-progress-pause.ico"
	
	SetOutPath "$INSTDIR\urbackup"
	
	File "data\urbackup\backup_client_new.db"
	
	CreateDirectory "$INSTDIR\urbackup\data"	
	
	WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "UrBackupClient" "$INSTDIR\UrBackupClient.exe"
	
	;${If} ${IsWinXP}
	;	StrCpy $0 "$INSTDIR\args_xp.txt" ;Path of copy file from
	;	StrCpy $1 "$INSTDIR\args.txt"   ;Path of copy file to
	;	StrCpy $2 0 ; only 0 or 1, set 0 to overwrite file if it already exists
	;	System::Call 'kernel32::CopyFile(t r0, t r1, b r2) l'
	;	Pop $0
	;${EndIf}
	;${If} ${IsWin2003}
	;	StrCpy $0 "$INSTDIR\args_server03.txt" ;Path of copy file from
	;	StrCpy $1 "$INSTDIR\args.txt"   ;Path of copy file to
	;	StrCpy $2 0 ; only 0 or 1, set 0 to overwrite file if it already exists
	;	System::Call 'kernel32::CopyFile(t r0, t r1, b r2) l'
	;	Pop $0
	;${EndIf}
	
	IfFileExists "$INSTDIR\urbackup\backup_client.db" next_s do_copy
do_copy:
	StrCpy $0 "$INSTDIR\urbackup\backup_client_new.db" ;Path of copy file from
	StrCpy $1 "$INSTDIR\urbackup\backup_client.db"   ;Path of copy file to
	StrCpy $2 0 ; only 0 or 1, set 0 to overwrite file if it already exists
	System::Call 'kernel32::CopyFile(t r0, t r1, b r2) l'
	Pop $0
next_s:	
	Delete "$INSTDIR\urbackup\backup_client_new.db"
	nsisFirewall::AddAuthorizedApplication "$INSTDIR\urbackupsrv.exe" "UrBackup Server"
	Pop $0
	
	SimpleSC::InstallService "UrBackupServer" "UrBackup Server for doing backups" "16" "2" "$INSTDIR\urbackupsrv.exe" "" "" ""
	Pop $0
	SimpleSC::StartService "UrBackupServer" ""
	Pop $0

SectionEnd

Section "Uninstall"
	!insertmacro DisableX64FSRedirection
	SetRegView 64

	
	StrCpy $0 "UrBackupClient.exe"
	KillProc::KillProcesses

	SimpleSC::StopService "UrBackupServer"
	Pop $0
	SimpleSC::RemoveService "UrBackupServer"
	Pop $0

	RMDir /r "$INSTDIR\*.*"
	RMDir "$INSTDIR"
	Delete "$SMPROGRAMS\UrBackup\*.*"
	RMDir "$SMPROGRAMS\UrBackup"
	
	DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" 
	DeleteRegValue HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "UrBackupClient"
SectionEnd

Function .onInstSuccess
  Exec "$INSTDIR\UrBackupClient.exe"
FunctionEnd
 
 
Function un.onUninstSuccess
  MessageBox MB_OK "UrBackup wurde erfolgreich deinstalliert."
FunctionEnd

