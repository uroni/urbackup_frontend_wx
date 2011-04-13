!define MUI_BRANDINGTEXT "UrBackup Update 0.37"
!include "${NSISDIR}\Contrib\Modern UI\System.nsh"
!include WinVer.nsh
!include "x64.nsh"
!define MUI_ICON "backup-ok.ico"

SetCompressor /FINAL /SOLID lzma

CRCCheck On
Name "UrBackup Update"
OutFile "UrBackupUpdate.exe"
InstallDir "$PROGRAMFILES\UrBackup"
RequestExecutionLevel highest

!insertmacro MUI_PAGE_INSTFILES

!define MUI_CUSTOMFUNCTION_GUIINIT myGuiInit


!insertmacro MUI_LANGUAGE "English"
 

Section "install"
	${If} ${RunningX64}
		!insertmacro DisableX64FSRedirection
		SetRegView 64
	${EndIf}
	
	${If} ${RunningX64}
		Push $R0
   		ClearErrors
   		ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{DA5E371C-6333-3D8A-93A4-6FD5B20BCC6E}" "Version"
	   	IfErrors 0 VSRedistInstalled64
		inetc::get "http://www.urserver.de/vc10/vcredist_x64.exe" $TEMP\vcredist_x64.exe
		Pop $0
		ExecWait '"$TEMP\vcredist_x64.exe" /q'  
		Delete '$TEMP\vcredist_x64.exe'
VSRedistInstalled64:
	${Else}
		ReadRegStr $0 HKLM "SOFTWARE\Microsoft\VisualStudio\10.0\VC\Runtimes\x86" 'Installed'
		${If} $0 != '1'
			ReadRegStr $0 HKLM "SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x86" 'Installed'
			${If} $0 != '1'
				inetc::get "http://www.urserver.de/vc10/vcredist_x86.exe" $TEMP\vcredist_x86.exe
				Pop $0
				ExecWait '"$TEMP\vcredist_x86.exe" /q'   
				Delete '$TEMP\vcredist_x86.exe'
			${EndIf}
		${EndIf}
	${EndIf}
	
	StrCpy $0 "UrBackupClient.exe"
	KillProc::KillProcesses
	
	SetOutPath "$INSTDIR"
	
	${If} ${RunningX64}
		File "data_x64\KillProc.exe"		
		ExecWait '"$INSTDIR\KillProc.exe" UrBackupClient.exe'
	${EndIf}
	
	SimpleSC::ExistsService "UrBackupServer"
	Pop $0
	${If} $0 == '0'
		SimpleSC::StopService "UrBackupServer"
		Pop $0
	${EndIf}
	;SimpleSC::RemoveService "UrBackupServer"
	;Pop $0
	
	
	
	Sleep 500
	
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	
	CreateDirectory "$SMPROGRAMS\UrBackup"
	CreateShortCut "$SMPROGRAMS\UrBackup\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
	CreateShortCut "$SMPROGRAMS\UrBackup\UrBackup.lnk" "$INSTDIR\UrBackupClient.exe" "" "$INSTDIR\UrBackupClient.exe" 0
	
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" "DisplayName" "UrBackup (remove only)"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" "UninstallString" "$INSTDIR\Uninstall.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" "Path" "$INSTDIR"
	
	File "data\args.txt"
	File "data\prefilebackup.bat"
	${IfNot} ${RunningX64} 
		File "data\args_server03.txt"
		File "data\args_xp.txt"
		File "data\fileservplugin.dll"
		File "data\fsimageplugin.dll"
		File "data\urbackup.dll"
		File "data\urbackup_server03.dll"
		File "data\urbackup_xp.dll"
		File "data\UrBackupClient.exe"
		File "data\urbackupsrv.exe"
		File "data\cryptoplugin.dll"
	${Else}
		File "data\args_server03.txt"
		File "data_x64\urbackup_server03.dll"
		File "data_x64\fileservplugin.dll"
		File "data_x64\fsimageplugin.dll"
		File "data_x64\urbackup.dll"
		File "data_x64\UrBackupClient.exe"
		File "data_x64\urbackupsrv.exe"
		File "data_x64\cryptoplugin.dll"
	${EndIf}
	File "data\backup-bad.ico"
	File "data\backup-ok.ico"
	File "data\backup-ok-big.ico"
	File "data\backup-progress.ico"	
	File "data\new.txt"	
	File "data\logo1.png"
	File "data\backup-progress-pause.ico"
	File "data\urbackup_dsa.pub"
	File "data\curr_version.txt"
	File "data\updates_h.dat"
	File "data\license.txt"
	SetOutPath "$INSTDIR\en"
	File "data\en\trans.mo"
	
	SetOutPath "$INSTDIR\urbackup"
	
	File "data\urbackup\backup_client_new.db"
	
	CreateDirectory "$INSTDIR\urbackup\data"	
	
	WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "UrBackupClient" "$INSTDIR\UrBackupClient.exe"
	
	${IfNot} ${RunningX64}
		${If} ${IsWinXP}
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
	nsisFirewall::AddAuthorizedApplication "$INSTDIR\urbackupsrv.exe" "UrBackup Server"
	Pop $0
	
	SimpleSC::ExistsService "UrBackupServer"
	Pop $0
	${If} $0 != '0'
		SimpleSC::InstallService "UrBackupServer" "UrBackup Server for doing backups" "16" "2" "$INSTDIR\urbackupsrv.exe" "" "" ""
		Pop $0
	${EndIf}	
	SimpleSC::StartService "UrBackupServer" ""
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
	
	StrCpy $0 "UrBackupClient.exe"
	KillProc::KillProcesses
	
	${If} ${RunningX64}
		File "data_x64\KillProc.exe"		
		ExecWait '"$INSTDIR\KillProc.exe" UrBackupClient.exe'
	${EndIf}

	SimpleSC::StopService "UrBackupServer"
	Pop $0
	SimpleSC::RemoveService "UrBackupServer"
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
	Exec "$INSTDIR\UrBackupClient.exe"
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
	
	ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UrBackup" 'Path'
	strcpy $INSTDIR $0
	
	${If} ${RunningX64}
		SetRegView 32
	${EndIf}
FunctionEnd


