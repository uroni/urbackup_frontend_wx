# Windows PE based restore medium

Builds an ISO that boots Windows PE and starts the UrBackup image restore web interface in Firefox ESR, like the Linux restore CD. It boots with Secure Boot enabled and uses the Windows drivers that come with Windows PE (plus any you add).

The Windows ADK license does not allow distributing Windows PE, so there is no ready-made ISO. Build it yourself:

1. Install the [Windows ADK](https://learn.microsoft.com/windows-hardware/get-started/adk-install) ("Deployment Tools" is enough) and the Windows PE add-on.
2. Get the restore binaries: `UrBackupClientBackend.exe`, `urbackup.dll`, `fileservplugin.dll`, `fsimageplugin.dll`, `cryptoplugin.dll`, `httpserver.dll` and the `restorewww` folder, all in one folder.
3. In an administrator PowerShell:

```
.\build_restore_iso.ps1 -ClientDir C:\path\to\restore_binaries -Output C:\path\to\urbackup_restore.iso
```

Options:

- `-FirefoxSetup` a Firefox ESR 64-bit installer, otherwise the current one is downloaded from mozilla.org
- `-DriverDir` a folder with extra drivers (`.inf`), e.g. for RAID/NVMe or network controllers
- `-ConnectionSettings` a `connection_settings.json` like for the Linux restore CD, e.g. `{"serverUrl": "urbackup://backup.example.com", "serverAuthkey": "..."}`. It goes onto the root of the medium.

Write the ISO to a USB stick with e.g. Rufus, or boot it in a VM.

Windows PE has no Wi-Fi, so the restore needs a wired network (or a USB network adapter with a driver). Keyboard layout and time zone are not set from the web interface.

On the running medium the logs are in `X:\urbackup_restore` (`restore_http.log`, `restore_client.log`), and the command prompt window stays open.
