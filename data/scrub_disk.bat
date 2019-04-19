@echo off

set scrubDisks="%1"
if [%1]==[] set scrubDisks="SelectViaGUI"

"%~dp0\UrBackupClientBackend.exe" --cmdline --no-server --plugin fsimageplugin.dll --disk_scrub "%scrubDisks%"
pause