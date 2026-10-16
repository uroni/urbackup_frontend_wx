@echo off
rem Starts the UrBackup restore web interface (the restore client is started by it, see restore_client_args.txt)

rem The UrBackup server connects to the restore client
wpeutil disablefirewall >nul

set MEDIUM=
for %%d in (C D E F G H I J K L M N O P Q R S T U V W Y Z) do if exist %%d:\urbackup_restore_medium.txt set MEDIUM=%%d:

cd /d X:\urbackup_restore
if defined MEDIUM if exist %MEDIUM%\connection_settings.json copy /y %MEDIUM%\connection_settings.json . >nul

start "UrBackup restore" /min UrBackupClientBackend.exe --cmdline --plugin httpserver.dll --plugin fileservplugin.dll --plugin fsimageplugin.dll --plugin cryptoplugin.dll --plugin urbackup.dll --no-server --restore_http true --http_server true --http_root restorewww --http_localhost_only 1 --logfile X:\urbackup_restore\restore_http.log --loglevel debug

if not defined MEDIUM (
	echo Restore medium not found. Open http://127.0.0.1/ in a browser.
	goto :eof
)
mkdir X:\firefox_profile 2>nul
start "" %MEDIUM%\firefox\firefox.exe -no-remote -profile X:\firefox_profile http://127.0.0.1/
echo The restore runs in Firefox. Logs: X:\urbackup_restore\restore_http.log and restore_client.log
