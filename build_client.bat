cd %~dp0

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsamd64_x86.bat"

msbuild UrBackupClientGUI.sln /p:Configuration=Release /p:Platform="Win32" /p:vcpkgTriplet="x86-windows-static-md"
if %errorlevel% neq 0 exit /b %errorlevel% 

msbuild UrBackupClientGUI.sln /p:Configuration=Release /p:Platform="x64" /p:vcpkgTriplet="x64-windows-static-md"
if %errorlevel% neq 0 exit /b %errorlevel%

call update_data.bat

if NOT "%SIGN%" == "true" GOTO skip_signing1

signtool sign /fd sha256 /t http://time.certum.pl /i Certum data\*.exe data\*.dll data_x64\*.exe data_x64\*.dll

:skip_signing1

"C:\Program Files (x86)\NSIS\makensis.exe" "%~dp0/urbackup.nsi"
if %errorlevel% neq 0 exit /b %errorlevel%

"C:\Program Files (x86)\NSIS\makensis.exe" "%~dp0/urbackup_notray.nsi"
if %errorlevel% neq 0 exit /b %errorlevel%

"C:\Program Files (x86)\NSIS\makensis.exe" "%~dp0/urbackup_update.nsi"
if %errorlevel% neq 0 exit /b %errorlevel%

call build_msi.bat

if NOT "%SIGN%" == "true" GOTO skip_signing2

signtool sign /fd sha256 /t http://time.certum.pl /i Certum "UrBackup Client $version_short$.exe" "UrBackup Client NoTray $version_short$.exe" "UrBackup Client $version_short$(x64).msi" "UrBackup Client (No tray) $version_short$(x64).msi"

:skip_signing2