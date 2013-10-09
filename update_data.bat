mkdir data_x64
mkdir pdbs\x86
mkdir pdbs\x64

copy /Y "..\Release Service\Server.exe" "data\urbackupsrv.exe"
copy /Y "..\Release Service\Server.pdb" "pdbs\x86\urbackupsrv.pdb"
copy /Y "..\Release Service\Server.exe" "data\UrBackupClientBackend.exe"
copy /Y "..\Release Service\Server.pdb" "pdbs\x86\UrBackupClientBackend.pdb"
copy /Y "..\release\urbackupclient.dll" "data\urbackup.dll"
copy /Y "..\release\urbackupclient.pdb" "pdbs\x86\urbackup.pdb"
copy /Y "..\release\sysvol_test.exe" "data\sysvol_test.exe"
copy /Y "..\Release WinXP\urbackupclient_xp.dll" "data\urbackup_xp.dll"
copy /Y "..\Release WinXP\urbackupclient.pdb" "pdbs\x86\urbackup_xp.pdb"
copy /Y "..\Release Server 2003\urbackupclient_server03.dll" "data\urbackup_server03.dll"
copy /Y "..\Release Server 2003\urbackupclient.pdb" "pdbs\x86\urbackup_server03.pdb"
copy /Y "..\release\fileservplugin.dll" "data\fileservplugin.dll"
copy /Y "..\release\fileservplugin.pdb" "pdbs\x86\fileservplugin.pdb"
copy /Y "..\release\fsimageplugin.dll" "data\fsimageplugin.dll"
copy /Y "..\release\fsimageplugin.pdb" "pdbs\x86\fsimageplugin.pdb"
copy /Y "..\release\cryptoplugin.dll" "data\cryptoplugin.dll"
copy /Y "..\release\cryptoplugin.pdb" "pdbs\x86\cryptoplugin.pdb"
copy /Y "Release\UrBackupClientGUI.exe" "data\UrBackupClient.exe"
copy /Y "Release\UrBackupClientGUI.pdb" "pdbs\x86\UrBackupClient.pdb"
copy /Y "deps\win\bin86\UrBackupCMD.exe" "data\UrBackupClient_cmd.exe"
copy /Y "deps\win\bin86\KillProc.exe" "data\KillProc.exe"

copy /Y "..\x64\Release Service\Server.pdb" "pdbs\x64\urbackupsrv.pdb"
copy /Y "..\x64\Release Service\Server.exe" "data_x64\urbackupsrv.exe"
copy /Y "..\x64\Release Service\Server.exe" "data_x64\UrBackupClientBackend.exe"
copy /Y "..\x64\Release Service\Server.pdb" "pdbs\x64\UrBackupClientBackend.pdb"
copy /Y "..\x64\release\urbackupclient.dll" "data_x64\urbackup.dll"
copy /Y "..\x64\release\urbackupclient.pdb" "pdbs\x64\urbackup.pdb"
copy /Y "..\x64\release\cryptoplugin.dll" "data_x64\cryptoplugin.dll"
copy /Y "..\x64\release\cryptoplugin.pdb" "pdbs\x64\cryptoplugin.pdb"
copy /Y "..\x64\Release Server 2003\urbackupclient_server03.dll" "data_x64\urbackup_server03.dll"
copy /Y "..\x64\Release Server 2003\urbackupclient.pdb" "pdbs\x64\urbackup_server03.pdb"
copy /Y "..\x64\release\fileservplugin.dll" "data_x64\fileservplugin.dll"
copy /Y "..\x64\release\fileservplugin.pdb" "pdbs\x64\fileservplugin.pdb"
copy /Y "..\x64\release\fsimageplugin.dll" "data_x64\fsimageplugin.dll"
copy /Y "..\x64\release\fsimageplugin.pdb" "pdbs\x64\fsimageplugin.pdb"
copy /Y "x64\Release\UrBackupClientGUI.exe" "data_x64\UrBackupClient.exe"
copy /Y "x64\Release\UrBackupClientGUI.pdb" "pdbs\x64\UrBackupClient.pdb"
copy /Y "..\x64\release\sysvol_test.exe" "data_x64\sysvol_test.exe"
copy /Y "deps\win\bin64\UrBackupCMD.exe" "data_x64\UrBackupClient_cmd.exe"
copy /Y "deps\win\bin64\KillProc.exe" "data_x64\KillProc.exe"
copy /Y "version.txt" "data\curr_version.txt"

mkdir data\lang\de
mkdir data\lang\fr
mkdir data\lang\ru
mkdir data\lang\es
mkdir data\lang\zh_TW
mkdir data\lang\zh_CN
mkdir data\lang\pt_BR
mkdir data\lang\it
mkdir data\lang\pl
mkdir data\lang\sk
msgfmt "lang\de\urbackup.po" -o "data\lang\de\urbackup.mo"
msgfmt "lang\fr\urbackup.po" -o "data\lang\fr\urbackup.mo"
msgfmt "lang\ru\urbackup.po" -o "data\lang\ru\urbackup.mo"
msgfmt "lang\es\urbackup.po" -o "data\lang\es\urbackup.mo"
msgfmt "lang\zh_TW\urbackup.po" -o "data\lang\zh_TW\urbackup.mo"
msgfmt "lang\zh_CN\urbackup.po" -o "data\lang\zh_CN\urbackup.mo"
msgfmt "lang\pt_BR\urbackup.po" -o "data\lang\pt_BR\urbackup.mo"
msgfmt "lang\it\urbackup.po" -o "data\lang\it\urbackup.mo"
msgfmt "lang\pl\urbackup.po" -o "data\lang\pl\urbackup.mo"
msgfmt "lang\sk\urbackup.po" -o "data\lang\sk\urbackup.mo"

msgfmt "lang\de\urbackup.po" -o "lang\de\urbackup.mo"
msgfmt "lang\fr\urbackup.po" -o "lang\fr\urbackup.mo"
msgfmt "lang\ru\urbackup.po" -o "lang\ru\urbackup.mo"
msgfmt "lang\es\urbackup.po" -o "lang\es\urbackup.mo"
msgfmt "lang\zh_TW\urbackup.po" -o "lang\zh_TW\urbackup.mo"
msgfmt "lang\zh_CN\urbackup.po" -o "lang\zh_CN\urbackup.mo"
msgfmt "lang\pt_BR\urbackup.po" -o "lang\pt_BR\urbackup.mo"
msgfmt "lang\it\urbackup.po" -o "lang\it\urbackup.mo"
msgfmt "lang\sk\urbackup.po" -o "lang\sk\urbackup.mo"
msgfmt "lang\pl\urbackup.po" -o "lang\pl\urbackup.mo"

git rev-parse HEAD > "data\build_revision.txt"
cd ..
git rev-parse HEAD >> "client\data\build_revision.txt"
cd client