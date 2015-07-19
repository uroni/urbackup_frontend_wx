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
mkdir data\lang\uk
mkdir data\lang\da
mkdir data\lang\nl
mkdir data\lang\fa
mkdir data\lang\cs
mkdir data\lang\tr
mkdir data\lang\et
tx pull -a
set LANG_DIR=translations\urbackup.frontend\
msgfmt "%LANG_DIR%\de.po" -o "data\lang\de\urbackup.mo"
msgfmt "%LANG_DIR%\fr.po" -o "data\lang\fr\urbackup.mo"
msgfmt "%LANG_DIR%\ru.po" -o "data\lang\ru\urbackup.mo"
msgfmt "%LANG_DIR%\es.po" -o "data\lang\es\urbackup.mo"
msgfmt "%LANG_DIR%\zh_TW.Big5.po" -o "data\lang\zh_TW\urbackup.mo"
msgfmt "%LANG_DIR%\zh_CN.GB2312.po" -o "data\lang\zh_CN\urbackup.mo"
msgfmt "%LANG_DIR%\pt_BR.po" -o "data\lang\pt_BR\urbackup.mo"
msgfmt "%LANG_DIR%\it_IT.po" -o "data\lang\it\urbackup.mo"
msgfmt "%LANG_DIR%\pl.po" -o "data\lang\pl\urbackup.mo"
msgfmt "%LANG_DIR%\sk.po" -o "data\lang\sk\urbackup.mo"
msgfmt "%LANG_DIR%\uk.po" -o "data\lang\uk\urbackup.mo"
msgfmt "%LANG_DIR%\da_DK.po" -o "data\lang\da\urbackup.mo"
msgfmt "%LANG_DIR%\nl.po" -o "data\lang\nl\urbackup.mo"
msgfmt "%LANG_DIR%\fa_IR.po" -o "data\lang\fa\urbackup.mo"
msgfmt "%LANG_DIR%\cs_CZ.po" -o "data\lang\cs\urbackup.mo"
msgfmt "%LANG_DIR%\tr.po" -o "data\lang\tr\urbackup.mo"
msgfmt "%LANG_DIR%\et.po" -o "data\lang\et\urbackup.mo"

git rev-parse HEAD > "data\build_revision.txt"
echo. >> "data\build_revision.txt"
cd ..
git rev-parse HEAD >> "client\data\build_revision.txt"
cd client