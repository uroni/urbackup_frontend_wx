@echo off

net session >nul 2>&1
if NOT %errorLevel% == 0 (
	echo Failure: Current permissions inadequate. Please run as administrator.
	pause
	exit /b 1
)

if exist "%~dp0\args_nodebug.txt"  (
	echo Disabling debug logging...
	copy /Y "%~dp0\args_nodebug.txt" "%~dp0\args.txt"
) else (
	echo Enabling debug logging...
	copy /Y "%~dp0\args.txt" "%~dp0\args_nodebug.txt"
	copy /Y "%~dp0\args_debug.txt" "%~dp0\args.txt"
)

echo "Restarting the UrBackupBackend service..."
net stop UrBackupClientBackend
net start UrBackupClientBackend

pause