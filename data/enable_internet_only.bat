@echo off

net session >nul 2>&1
if NOT %errorLevel% == 0 (
	echo Failure: Current permissions inadequate. Please run as administrator.
	pause
	exit /b 1
)

echo --internet_only_mode> %~dp0\extra_args_0.txt
echo true>> %~dp0\extra_args_0.txt

echo Restarting the UrBackupBackend service...
net stop UrBackupClientBackend
net start UrBackupClientBackend

pause