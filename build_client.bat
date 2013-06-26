call update_deps.bat

call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"

msbuild UrBackupClientGUI.sln /p:Configuration=Release /p:Platform="Win32"
if %errorlevel% neq 0 exit /b %errorlevel% 

msbuild UrBackupClientGUI.sln /p:Configuration=Release /p:Platform="x64"
if %errorlevel% neq 0 exit /b %errorlevel%

call update_data.bat

"C:\Program Files (x86)\NSIS\Unicode\makensis.exe" "%~dp0/urbackup.nsi"
if %errorlevel% neq 0 exit /b %errorlevel%

"C:\Program Files (x86)\NSIS\Unicode\makensis.exe" "%~dp0/urbackup_notray.nsi"
if %errorlevel% neq 0 exit /b %errorlevel%

call build_msi.bat