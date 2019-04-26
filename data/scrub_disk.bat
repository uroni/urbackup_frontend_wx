@echo off

net session >nul 2>&1
if NOT %errorLevel% == 0 (
	echo Failure: Current permissions inadequate. Please run as administrator.
	pause
	exit /b 1
)

set scrubDisks="%1"
if [%1]==[] set scrubDisks="SelectViaGUI"

"%~dp0\UrBackupClientBackend.exe" --cmdline --no-server --plugin fsimageplugin.dll --disk_scrub "%scrubDisks%"
pause