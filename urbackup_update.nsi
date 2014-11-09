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

!define MUI_PAGE_CUSTOMFUNCTION_PRE skipPre
!insertmacro MUI_PAGE_WELCOME
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

!define Unicode2Ansi "!insertmacro Unicode2Ansi"

!macro Unicode2Ansi String outVar
  System::Call 'kernel32::WideCharToMultiByte(i 0, i 0, w "${String}", i -1, t .s, i ${NSIS_MAX_STRLEN}, i 0, i 0) i'
  Pop "${outVar}"
!macroend  
 
 
 Var INSTALL_TRAYICON

Section "install"
	${If} ${RunningX64}
		!insertmacro DisableX64FSRedirection
		SetRegView 64
	${EndIf}
	
	SetOutPath "$TEMP"
	${If} ${RunningX64}
		; Push $R0
   		; ClearErrors
   		; ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{DA5E371C-6333-3D8A-93A4-6FD5B20BCC6E}" "Version"
	   	; IfErrors 0 VSRedistInstalled64
		; inetc::get "http://www.urserver.de/vc10/vcredist_x64.exe" $TEMP\vcredist_x64.exe
		; Pop $0
		; ExecWait '"$TEMP\vcredist_x64.exe" /q'  
		; Delete '$TEMP\vcredist_x64.exe'
; VSRedistInstalled64:
		File "deps\win\vcredist\vcredist_2010sp1_x64.exe"
		ExecWait '"$TEMP\vcredist_2010sp1_x64.exe" /q /norestart' $0
		${If} $0 != '0'
		${If} $0 != '3010'
		${If} $0 != '8192'
		${If} $0 != '1641'
		${If} $0 != '1046'
			ExecWait '"$TEMP\vcredist_2010sp1_x64.exe" /passive /norestart' $0
			${If} $0 != '0'
			${If} $0 != '3010'
				MessageBox MB_OK "Unable to install Visual Studio 2010SP1 runtime. UrBackup needs that runtime."
				Quit
			${EndIf}
			${EndIf}
		${EndIf}
		${EndIf}
		${EndIf}
		${EndIf}
		${EndIf}
			
	${Else}
		; ReadRegStr $0 HKLM "SOFTWARE\Microsoft\VisualStudio\10.0\VC\Runtimes\x86" 'Installed'
		; ${If} $0 != '1'
			; ReadRegStr $0 HKLM "SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x86" 'Installed'
			; ${If} $0 != '1'
				; inetc::get "http://www.urserver.de/vc10/vcredist_x86.exe" $TEMP\vcredist_x86.exe
				; Pop $0
				; ExecWait '"$TEMP\vcredist_x86.exe" /q'   
				; Delete '$TEMP\vcredist_x86.exe'
			; ${EndIf}
		; ${EndIf}
		File "deps\win\vcredist\vcredist_2010sp1_x86.exe"
		ExecWait '"$TEMP\vcredist_2010sp1_x86.exe" /q /norestart' $0
		${If} $0 != '0'
		${If} $0 != '3010'
		${If} $0 != '8192'
		${If} $0 != '1641'
		${If} $0 != '1046'
			ExecWait '"$TEMP\vcredist_2010sp1_x86.exe"  /passive /norestart' $0
			${If} $0 != '0'
			${If} $0 != '3010'
				MessageBox MB_OK "Unable to install Visual Studio 2010SP1 runtime. UrBackup needs that runtime."
				Quit
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
	
	${Unicode2Ansi} "UrBackupClient Backend" $R0
	SimpleSC::ExistsService "$R0"
	Pop $0
	${If} $0 == '0'
		SimpleSC::StopService "$R0"
		Pop $0
	${EndIf}
	
	${Unicode2Ansi} "UrBackupClientBackend" $R0
	SimpleSC::ExistsService "$R0"
	Pop $0
	${If} $0 == '0'
		SimpleSC::StopService "$R0"
		Pop $0
	${EndIf}
	
	Sleep 500
	
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	
	CreateDirectory "$SMPROGRAMS\UrBackup"
	CreateShortCut "$SMPROGRAMS\UrBackup\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
	
	${If} $INSTALL_TRAYICON == "1"
	CreateShortCut "$SMPROGRAMS\UrBackup\UrBackup.lnk" "$INSTDIR\UrBackupClient.exe" "" "$INSTDIR\UrBackupClient.exe" 0
	${EndIf}
	
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" "DisplayName" "UrBackup (remove only)"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" "UninstallString" "$INSTDIR\Uninstall.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" "Path" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" "DisplayVersion" "$version_short$"
	
	File "data\args.txt"
	File "data\prefilebackup_new.bat"
	File "data\build_revision.txt"
	
	SetCompress off
	File "data\server_idents_new.txt"
	File "data\initial_settings.cfg"
	SetCompress auto
	
	${IfNot} ${RunningX64} 
		File "data\args_server03.txt"
		File "data\args_xp.txt"
		File "data\fileservplugin.dll"
		File "data\fsimageplugin.dll"
		File "data\urbackup.dll"
		File "data\urbackup_server03.dll"
		File "data\urbackup_xp.dll"
		${If} $INSTALL_TRAYICON == "1"
		File "data\UrBackupClient.exe"
		${EndIf}
		File "data\UrBackupClientBackend.exe"
		File "data\cryptoplugin.dll"
		File "data\UrBackupClient_cmd.exe"
		File "data\sysvol_test.exe"
	${Else}
		File "data\args_server03.txt"
		File "data_x64\urbackup_server03.dll"
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
	${EndIf}
	File "data\backup-bad.ico"
	File "data\backup-ok.ico"
	File "data\backup-progress.ico"
	File "data\backup-no-server.ico"
	File "data\backup-indexing.ico"
	File "data\backup-no-recent.ico"
	File "data\new.txt"	
	File "data\logo1.png"
	File "data\backup-progress-pause.ico"
	File "data\urbackup_dsa.pub"
	File "data\curr_version.txt"
	File "data\updates_h.dat"
	File "data\license.txt"
	File "data\info.txt"
	File "data\args_debug.txt"
	File "data\enable_debug_logging.bat"
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
	
	SetOutPath "$INSTDIR\urbackup"
	
	File "data\urbackup\backup_client_new.db"
	
	CreateDirectory "$INSTDIR\urbackup\data"	
	
	${If} $INSTALL_TRAYICON == "1"
	WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "UrBackupClient" "$INSTDIR\UrBackupClient.exe"
	${EndIf}
	
	${IfNot} ${RunningX64}
		${If} ${IsWinXP}
			StrCpy $0 "$INSTDIR\args_xp.txt" ;Path of copy file from
			StrCpy $1 "$INSTDIR\args.txt"   ;Path of copy file to
			StrCpy $2 0 ; only 0 or 1, set 0 to overwrite file if it already exists
			System::Call 'kernel32::CopyFile(t r0, t r1, b r2) l'
			Pop $0
			;SetRebootFlag true
		${EndIf}
		${If} ${IsWin2000}
			StrCpy $0 "$INSTDIR\args_xp.txt" ;Path of copy file from
			StrCpy $1 "$INSTDIR\args.txt"   ;Path of copy file to
			StrCpy $2 0 ; only 0 or 1, set 0 to overwrite file if it already exists
			System::Call 'kernel32::CopyFile(t r0, t r1, b r2) l'
			Pop $0
			;SetRebootFlag true
		${EndIf}
		${If} ${IsWin2003}
			StrCpy $0 "$INSTDIR\args_server03.txt" ;Path of copy file from
			StrCpy $1 "$INSTDIR\args.txt"   ;Path of copy file to
			StrCpy $2 0 ; only 0 or 1, set 0 to overwrite file if it already exists
			System::Call 'kernel32::CopyFile(t r0, t r1, b r2) l'
			Pop $0
			;SetRebootFlag true
		${EndIf}
	${Else}
		${If} ${IsWin2003}
		${OrIf} ${IsWinXP}
			StrCpy $0 "$INSTDIR\args_server03.txt" ;Path of copy file from
			StrCpy $1 "$INSTDIR\args.txt"   ;Path of copy file to
			StrCpy $2 0 ; only 0 or 1, set 0 to overwrite file if it already exists
			System::Call 'kernel32::CopyFile(t r0, t r1, b r2) l'
			Pop $0
			;SetRebootFlag true
		${EndIf}
	${EndIf}
	
	IfFileExists "$INSTDIR\urbackup\backup_client.db" next_s do_copy
do_copy:
	StrCpy $0 "$INSTDIR\urbackup\backup_client_new.db" ;Path of copy file from
	StrCpy $1 "$INSTDIR\urbackup\backup_client.db"   ;Path of copy file to
	StrCpy $2 0 ; only 0 or 1, set 0 to overwrite file if it already exists
	System::Call 'kernel32::CopyFile(t r0, t r1, b r2) l'
	Pop $0
next_s:	
	Delete "$INSTDIR\urbackup\backup_client_new.db"
	
	${If} ${IsWinXP}
		nsisFirewallW::RemoveAuthorizedApplication "$INSTDIR\UrBackupClientBackend.exe"
		Pop $0
		nsisFirewallW::AddAuthorizedApplication "$INSTDIR\UrBackupClientBackend.exe" "UrBackupClientBackend"
	${ElseIf} ${IsWin2003}
		nsisFirewallW::RemoveAuthorizedApplication "$INSTDIR\UrBackupClientBackend.exe"
		Pop $0
		nsisFirewallW::AddAuthorizedApplication "$INSTDIR\UrBackupClientBackend.exe" "UrBackupClientBackend"
	${Else}
		liteFirewallW::RemoveRule "$INSTDIR\UrBackupClientBackend.exe" "UrBackupClientBackend"
		Pop $0
		liteFirewallW::AddRule "$INSTDIR\UrBackupClientBackend.exe" "UrBackupClientBackend"
	${EndIf}
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
	
	${Unicode2Ansi} "UrBackupClientBackend" $R0
	${Unicode2Ansi} "UrBackup Client Service for Backups" $R1
	${Unicode2Ansi} "16" $R2
	${Unicode2Ansi} "2" $R3
	${Unicode2Ansi} "$INSTDIR\UrBackupClientBackend.exe" $R4
	SimpleSC::ExistsService "$R0"
	Pop $0
	${If} $0 != '0'
		SimpleSC::InstallService "$R0" "$R1" "$R2" "$R3" "$R4" "" "" ""
		Pop $0
	${EndIf}	
	SimpleSC::StartService "$R0" ""
	Pop $0
	
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
	
	${If} ${RunningX64}
		File "data_x64\KillProc.exe"		
		ExecWait '"$INSTDIR\KillProc.exe" UrBackupClient.exe'
	${EndIf}

	${Unicode2Ansi} "UrBackupClientBackend" $R0
	SimpleSC::StopService "$R0"
	Pop $0
	SimpleSC::RemoveService "$R0"
	Pop $0
	
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
	${If} $INSTALL_TRAYICON == "1"
		StrCpy $0 "$INSTDIR\UrBackupClient.exe"
		startplugin::start
		Pop $0
		${If} $0 == '0'
			Exec '"$INSTDIR\UrBackupClient.exe"'
		${EndIf}
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

