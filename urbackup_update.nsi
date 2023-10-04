!define MUI_BRANDINGTEXT "UrBackup Client $version_short$"
!include "${NSISDIR}\Contrib\Modern UI\System.nsh"
!include WinVer.nsh
!include "x64.nsh"
!define MUI_ICON "icon3.ico"

SetCompressor /FINAL lzma
CRCCheck off

Name "UrBackup Client $version_short$"
OutFile "UrBackupUpdate.exe"
InstallDir "$PROGRAMFILES\UrBackup"
RequestExecutionLevel highest
Unicode true
ManifestSupportedOS all

!include "servicelib.nsh"

!define MUI_PAGE_CUSTOMFUNCTION_PRE skipPre
!insertmacro MUI_PAGE_WELCOME
!define MUI_PAGE_CUSTOMFUNCTION_PRE skipPre
!insertmacro MUI_PAGE_LICENSE "License-Install.txt"
!define MUI_PAGE_CUSTOMFUNCTION_PRE skipPre
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_PAGE_CUSTOMFUNCTION_PRE skipPre
!insertmacro MUI_PAGE_FINISH

!define MUI_CUSTOMFUNCTION_GUIINIT myGuiInit

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH


!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "SimpChinese"
!insertmacro MUI_LANGUAGE "TradChinese"
!insertmacro MUI_LANGUAGE "PortugueseBR"
!insertmacro MUI_LANGUAGE "Italian"
!insertmacro MUI_LANGUAGE "Polish"
!insertmacro MUI_LANGUAGE "Slovak"
!insertmacro MUI_LANGUAGE "Ukrainian"
!insertmacro MUI_LANGUAGE "Danish"
!insertmacro MUI_LANGUAGE "Dutch"
!insertmacro MUI_LANGUAGE "Farsi"
!insertmacro MUI_LANGUAGE "Turkish"
!insertmacro MUI_LANGUAGE "Estonian"
!insertmacro MUI_LANGUAGE "Norwegian"
!insertmacro MUI_LANGUAGE "Portuguese"
 
 
 Var INSTALL_TRAYICON
 Var HAS_SERVICE
 Var SITE_LOCAL_RUNTIME

Section "install"
	${If} ${RunningX64}
		!insertmacro DisableX64FSRedirection
		SetRegView 64
	${EndIf}
	
	${If} ${AtMostWin8}
		MessageBox MB_OK "Sorry, installation on this version of Windows (Windows 8 or older version) is not supported."
		Quit
	${EndIf}
	
	StrCpy $SITE_LOCAL_RUNTIME "0"
	
	SetOutPath "$INSTDIR"
	File "data\version_info.txt"
	
	SetOutPath "$TEMP"
	${If} ${RunningX64}
		File "..\deps\redist\vc_redist_2022.x64.exe"
		ExecWait '"$TEMP\vc_redist_2022.x64.exe" /q /norestart' $0
		${If} $0 != '0'
		${If} $0 != '3010'
		${If} $0 != '1638'
		${If} $0 != '8192'
		${If} $0 != '1641'
		${If} $0 != '1046'
			ExecWait '"$TEMP\vc_redist_2022.x64.exe" /passive /norestart' $0
			${If} $0 != '0'
			${If} $0 != '3010'
			${If} $0 != '1638'
				DetailPrint "Unable to install Visual Studio 2022 runtime. Falling back to site local runtime installation."
				StrCpy $SITE_LOCAL_RUNTIME "1"
			${EndIf}
			${EndIf}
			${EndIf}
		${EndIf}
		${EndIf}
		${EndIf}
		${EndIf}
		${EndIf}
		${EndIf}
			
	${Else}
		File "..\deps\redist\vc_redist_2022.x86.exe"
		ExecWait '"$TEMP\vc_redist_2022.x86.exe" /q /norestart' $0
		${If} $0 != '0'
		${If} $0 != '3010'
		${If} $0 != '1638'
		${If} $0 != '8192'
		${If} $0 != '1641'
		${If} $0 != '1046'
			ExecWait '"$TEMP\vc_redist_2022.x86.exe"  /passive /norestart' $0
			${If} $0 != '0'
			${If} $0 != '3010'
			${If} $0 != '1638'
				MessageBox MB_OK "Unable to install Visual Studio 2022 runtime. UrBackup needs that runtime."
				Quit
			${EndIf}
			${EndIf}
			${EndIf}
		${EndIf}
		${EndIf}
		${EndIf}
		${EndIf}
		${EndIf}
		${EndIf}
	${EndIf}
	
	SetOutPath "$INSTDIR"
	
	
	StrCpy $INSTALL_TRAYICON "0"
	IfFileExists $INSTDIR\UrBackupClient.exe UrBackupClientExists PastUrBackupClientExists
	UrBackupClientExists:
	StrCpy $INSTALL_TRAYICON "1"
	PastUrBackupClientExists:
	IfFileExists $INSTDIR\UrBackupClientBackend.exe UrBackupClientBackendExists PastUrBackupClientBackendExists
	PastUrBackupClientBackendExists:
	StrCpy $INSTALL_TRAYICON "1"
	UrBackupClientBackendExists:
	
	
	${If} $EXEFILE != 'UrBackupUpdate.exe'
		StrCpy $INSTALL_TRAYICON "1"
	${EndIf}
	
	
	
	${If} $INSTALL_TRAYICON == "1"
	
	${If} ${RunningX64}
		File "data_x64\KillProc.exe"		
		nsExec::Exec '"$INSTDIR\KillProc.exe" UrBackupClient.exe'
	${Else}
		File "data\KillProc.exe"		
		nsExec::Exec '"$INSTDIR\KillProc.exe" UrBackupClient.exe'
	${EndIf}
	
	${EndIf}
	
	StrCpy $HAS_SERVICE "0"
	
	!insertmacro SERVICE running "UrBackupClient Backend" ""
	Pop $0
	${If} $0 == "true"
		!insertmacro SERVICE stop "UrBackupClient Backend" ""
		!insertmacro SERVICE waitfor "UrBackupClient Backend" "status=stopped"
		!insertmacro SERVICE delete "UrBackupClient Backend" ""
	${EndIf}
	
	!insertmacro SERVICE running "UrBackupClientBackend" ""
	Pop $0
	${If} $0 == "true"
		StrCpy $HAS_SERVICE "1"
		!insertmacro SERVICE stop "UrBackupClientBackend" ""
		!insertmacro SERVICE waitfor "UrBackupClientBackend" "status=stopped"
		nsExec::Exec '"$INSTDIR\KillProc.exe" UrBackupClientBackend.exe'
	${EndIf}
	
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	
	Sleep 500
	
	CreateDirectory "$SMPROGRAMS\UrBackup"
	CreateShortCut "$SMPROGRAMS\UrBackup\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
	
	${If} $INSTALL_TRAYICON == "1"
	CreateShortCut "$SMPROGRAMS\UrBackup\UrBackup.lnk" "$INSTDIR\UrBackupClient.exe" "" "$INSTDIR\UrBackupClient.exe" 0
	${EndIf}
	
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" "DisplayName" "UrBackup (remove only)"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" "UninstallString" "$INSTDIR\Uninstall.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" "Path" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" "DisplayVersion" "$version_short$"
	
	ClearErrors
	
	File "data\args.txt"
	File "data\prefilebackup_new.bat"
	File "data\build_revision.txt"
	
	SetCompress off
	File "data\server_idents_new.txt"
	File "data\initial_settings.cfg"
	SetCompress auto
	
	${IfNot} ${RunningX64} 
		File "data\fileservplugin.dll"
		File "data\fsimageplugin.dll"
		File "data\urbackup.dll"
		${If} $INSTALL_TRAYICON == "1"
		File "data\UrBackupClient.exe"
		${EndIf}
		File "data\UrBackupClientBackend.exe"
		File "data\cryptoplugin.dll"
		File "data\UrBackupClient_cmd.exe"
		File "data\sysvol_test.exe"
		File "data\libzstd.dll"
	${Else}
		File "data_x64\fileservplugin.dll"
		File "data_x64\fsimageplugin.dll"
		File "data_x64\urbackup.dll"
		${If} $INSTALL_TRAYICON == "1"
		File "data_x64\UrBackupClient.exe"
		${EndIf}
		File "data_x64\UrBackupClientBackend.exe"
		File "data_x64\cryptoplugin.dll"
		File "data_x64\UrBackupClient_cmd.exe"
		File "data_x64\sysvol_test.exe"
		File "data_x64\libzstd.dll"
		
		ExecWait '"$INSTDIR\UrBackupClient_cmd.exe" --version' $0
		${If} $0 != '1'
			StrCpy $SITE_LOCAL_RUNTIME "1"
			File "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\14.36.32532_copy\x64\Microsoft.VC143.CRT\*"
			File "C:\Program Files (x86)\Windows Kits\10\Redist\ucrt\DLLs\x64\*"
		${EndIf}
	${EndIf}
	
	${If} ${Errors}
		${If} $HAS_SERVICE == "1"
			Goto start_service
		${Else}
			Quit
		${EndIf}
	${EndIf}
	
	File "data\backup-bad.ico"
	File "data\backup-ok.ico"
	File "data\backup-progress.ico"
	File "data\backup-no-server.ico"
	File "data\backup-indexing.ico"
	File "data\backup-no-recent.ico"
	File "data\logo1.png"
	File "data\fa-copy.png"
	File "data\fa-home.png"
	File "data\fa-lock.png"
	File "data\fa-road.png"
	File "data\backup-progress-pause.ico"
	File "data\urbackup_ecdsa409k1.pub"
	File "data\updates_h.dat"
	File "data\license.txt"
	File "data\info.txt"
	File "data\args_debug.txt"
	File "data\enable_debug_logging.bat"
	File "data\enable_internet_only.bat"
	File "data\scrub_disk.bat"
	SetOutPath "$INSTDIR\lang\de"
	File "data\lang\de\urbackup.mo"
	SetOutPath "$INSTDIR\lang\fr"
	File "data\lang\fr\urbackup.mo"
	SetOutPath "$INSTDIR\lang\ru"
	File "data\lang\ru\urbackup.mo"
	SetOutPath "$INSTDIR\lang\es"
	File "data\lang\es\urbackup.mo"
	SetOutPath "$INSTDIR\lang\zh_TW"
	File "data\lang\zh_TW\urbackup.mo"
	SetOutPath "$INSTDIR\lang\zh_CN"
	File "data\lang\zh_CN\urbackup.mo"
	SetOutPath "$INSTDIR\lang\pt_BR"
	File "data\lang\pt_BR\urbackup.mo"
	SetOutPath "$INSTDIR\lang\it"
	File "data\lang\it\urbackup.mo"
	SetOutPath "$INSTDIR\lang\sk"
	File "data\lang\sk\urbackup.mo"
	SetOutPath "$INSTDIR\lang\pl"
	File "data\lang\pl\urbackup.mo"
	SetOutPath "$INSTDIR\lang\uk"
	File "data\lang\uk\urbackup.mo"
	SetOutPath "$INSTDIR\lang\da"
	File "data\lang\da\urbackup.mo"
	SetOutPath "$INSTDIR\lang\nl"
	File "data\lang\nl\urbackup.mo"
	SetOutPath "$INSTDIR\lang\fa"
	File "data\lang\fa\urbackup.mo"
	SetOutPath "$INSTDIR\lang\cs"
	File "data\lang\cs\urbackup.mo"
	SetOutPath "$INSTDIR\lang\tr"
	File "data\lang\tr\urbackup.mo"
	SetOutPath "$INSTDIR\lang\et"
	File "data\lang\et\urbackup.mo"
	SetOutPath "$INSTDIR\lang\no_NO"
	File "data\lang\no_NO\urbackup.mo"
	SetOutPath "$INSTDIR\lang\sv"
	File "data\lang\sv\urbackup.mo"
	SetOutPath "$INSTDIR\lang\pt"
	File "data\lang\pt\urbackup.mo"
	
	${If} ${Errors}
		${If} $HAS_SERVICE == "1"
			Goto start_service
		${Else}
			Quit
		${EndIf}
	${EndIf}
	
	SetOutPath "$INSTDIR"
	File "data\curr_version.txt"
	
	SetOutPath "$INSTDIR\urbackup"
	
	CreateDirectory "$INSTDIR\urbackup\data"	
	
	${If} $INSTALL_TRAYICON == "1"
	WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "UrBackupClient" "$INSTDIR\UrBackupClient.exe"
	${EndIf}
	
	liteFirewallW::RemoveRule "$INSTDIR\UrBackupClientBackend.exe" "UrBackupClientBackend"
	Pop $0
	liteFirewallW::AddRule "$INSTDIR\UrBackupClientBackend.exe" "UrBackupClientBackend"
	Pop $0
	
	IfFileExists "$INSTDIR\prefilebackup.bat" next_s_pfb do_copy_pfb
do_copy_pfb:
	StrCpy $0 "$INSTDIR\prefilebackup_new.bat" ;Path of copy file from
	StrCpy $1 "$INSTDIR\prefilebackup.bat"   ;Path of copy file to
	StrCpy $2 0 ; only 0 or 1, set 0 to overwrite file if it already exists
	System::Call 'kernel32::CopyFile(t r0, t r1, b r2) l'
	Pop $0
next_s_pfb:	
	Delete "$INSTDIR\prefilebackup_new.bat"
	
	IfFileExists "$INSTDIR\server_idents.txt" next_idents do_copy_idents
do_copy_idents:
	StrCpy $0 "$INSTDIR\server_idents_new.txt" ;Path of copy file from
	StrCpy $1 "$INSTDIR\server_idents.txt"   ;Path of copy file to
	StrCpy $2 0 ; only 0 or 1, set 0 to overwrite file if it already exists
	System::Call 'kernel32::CopyFile(t r0, t r1, b r2) l'
	Pop $0
next_idents:
	Delete "$INSTDIR\server_idents_new.txt"
	
start_service:
	!insertmacro SERVICE installed "UrBackupClientBackend" ""
	Pop $0
	${If} $0 != "true"
		!insertmacro SERVICE create "UrBackupClientBackend" 'path="$INSTDIR\UrBackupClientBackend.exe";autostart=1;interact=0;display=UrBackup Client Service for Backups;description=UrBackup Client Service for Backups;'
	${EndIf}	
	!insertmacro SERVICE start "UrBackupClientBackend" ""
	
	${If} ${RunningX64}
		!insertmacro EnableX64FSRedirection
		SetRegView 32
	${EndIf}
	
SectionEnd

Section "Uninstall"
	${If} ${RunningX64}
		!insertmacro DisableX64FSRedirection
		SetRegView 64
	${EndIf}
	
	KillProcDLL::KillProc "UrBackupClient.exe"
	
	ExecWait '"$INSTDIR\UrBackupClient.exe" deleteshellkeys'
	
	${If} ${RunningX64}
		File "data_x64\KillProc.exe"		
		ExecWait '"$INSTDIR\KillProc.exe" UrBackupClient.exe'
	${EndIf}

	!insertmacro SERVICE stop "UrBackupClientBackend" ""
	!insertmacro SERVICE waitfor "UrBackupClientBackend" "status=stopped"
	!insertmacro SERVICE delete "UrBackupClientBackend" ""
	nsExec::Exec '"$INSTDIR\KillProc.exe" UrBackupClientBackend.exe'
	
	Sleep 500

	RMDir /r "$INSTDIR\*.*"
	RMDir "$INSTDIR"
	Delete "$SMPROGRAMS\UrBackup\*.*"
	RMDir "$SMPROGRAMS\UrBackup"
	
	DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" 
	DeleteRegValue HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "UrBackupClient"
	
	${If} ${RunningX64}
		!insertmacro EnableX64FSRedirection
		SetRegView 32
	${EndIf}
SectionEnd

Function .onInstSuccess
	${If} ${RunningX64}
		!insertmacro DisableX64FSRedirection
		SetRegView 64
	${EndIf}
	WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\WindowsBackup" "DisableMonitoring" 0x1
	${If} $INSTALL_TRAYICON == "1"
		StrCpy $0 "$INSTDIR\UrBackupClient.exe"
		startplugin::start
		Pop $0
		${If} $0 == '0'
			Exec '"$INSTDIR\UrBackupClient.exe"'
		${EndIf}
		
		SetOutPath "$INSTDIR"
		IfSilent setup_defaults
		${If} $EXEFILE != 'UrBackupUpdate.exe'
			Exec '"$INSTDIR\UrBackupClient.exe" setupWizard'	
		${EndIf}
		
		${If} $EXEFILE == 'UrBackupUpdate.exe'
			Exec '"$INSTDIR\UrBackupClient.exe" setupDefault'
		${EndIf}
		Goto after_setup_wizard
setup_defaults:
		Exec '"$INSTDIR\UrBackupClient.exe" setupDefault'
after_setup_wizard:
	${EndIf}
	${If} ${RunningX64}
		!insertmacro EnableX64FSRedirection
		SetRegView 32
	${EndIf}
FunctionEnd
 
 
Function un.onUninstSuccess
  ;MessageBox MB_OK "UrBackup wurde erfolgreich deinstalliert."
FunctionEnd

Function myGuiInit
	;MessageBox MB_OK "blub"
	;${If} ${RunningX64}
	;	!insertmacro DisableX64FSRedirection
	;	SetRegView 64
	;${EndIf}
FunctionEnd

Function .onInit
	${If} ${RunningX64}
		strcpy $INSTDIR "$PROGRAMFILES64\UrBackup"
		SetRegView 64
	${EndIf}

	ClearErrors
	ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" 'Path'
	IfErrors skip_modify
	
	strcpy $INSTDIR $0
	
skip_modify:
	${If} ${RunningX64}
		SetRegView 32
	${EndIf}
FunctionEnd

Function skipPre
	${If} $EXEFILE == 'UrBackupUpdate.exe'
		Abort
	${EndIf}
FunctionEnd

