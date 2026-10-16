<#
.SYNOPSIS
Builds a Windows PE based UrBackup restore ISO.

.DESCRIPTION
Needs the Windows ADK with the Windows PE add-on (free download from Microsoft) and must run
as administrator (DISM mounts the boot image). The ADK license does not allow distributing the
result, so every user builds the ISO themselves with this script.

The restore medium starts the UrBackup restore web interface in Firefox ESR, like the Linux
restore CD.

.PARAMETER ClientDir
Folder with the restore binaries: UrBackupClientBackend.exe, urbackup.dll, fileservplugin.dll,
fsimageplugin.dll, cryptoplugin.dll, httpserver.dll and the restorewww folder.

.PARAMETER Output
Path of the ISO to write.

.PARAMETER FirefoxSetup
Firefox ESR 64-bit full installer (.exe). Downloaded from mozilla.org if not given.

.PARAMETER DriverDir
Optional folder with extra drivers (.inf), e.g. for storage or network controllers. Added recursively.

.PARAMETER ConnectionSettings
Optional connection_settings.json (same format as for the Linux restore CD). It is put onto the
root of the medium, where it can also be added or changed later.

.PARAMETER WorkDir
Temporary folder. Deleted and recreated.
#>
param(
	[Parameter(Mandatory=$true)][string]$ClientDir,
	[Parameter(Mandatory=$true)][string]$Output,
	[string]$FirefoxSetup,
	[string]$DriverDir,
	[string]$ConnectionSettings,
	[string]$WorkDir = (Join-Path $env:TEMP "urbackup_restore_pe")
)

$ErrorActionPreference = "Stop"
$here = Split-Path -Parent $MyInvocation.MyCommand.Path

function Run([string]$exe, [string[]]$params)
{
	& $exe @params
	if ($LASTEXITCODE -ne 0) { throw "$exe $params failed with exit code $LASTEXITCODE" }
}

if (-not ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator))
{
	throw "Run this script as administrator."
}

if ($WorkDir.Contains(" ")) { throw "WorkDir must not contain spaces: $WorkDir" }

$client_files ="UrBackupClientBackend.exe", "urbackup.dll", "fileservplugin.dll", "fsimageplugin.dll", "cryptoplugin.dll", "httpserver.dll", "restorewww\index.html"
foreach ($f in $client_files)
{
	if (-not (Test-Path (Join-Path $ClientDir $f))) { throw "$f not found in $ClientDir" }
}

$kits = (Get-ItemProperty "HKLM:\SOFTWARE\WOW6432Node\Microsoft\Windows Kits\Installed Roots" -Name KitsRoot10).KitsRoot10
$adk = Join-Path $kits "Assessment and Deployment Kit"
$winpe = Join-Path $adk "Windows Preinstallation Environment"
if (-not (Test-Path (Join-Path $winpe "copype.cmd"))) { throw "Windows PE add-on of the Windows ADK not found in $adk" }
$denv = Join-Path $adk "Deployment Tools\DandISetEnv.bat"

if (Test-Path "$WorkDir\mount\Windows") { & dism.exe /Unmount-Image /MountDir:"$WorkDir\mount" /Discard | Out-Null }
if (Test-Path $WorkDir) { Remove-Item -Recurse -Force $WorkDir }

Write-Host "Copying Windows PE..."
Run cmd.exe @("/c", "call `"$denv`" >nul && copype amd64 `"$WorkDir`" >nul")
$media = Join-Path $WorkDir "media"
$mount = Join-Path $WorkDir "mount"

Write-Host "Unpacking Firefox ESR..."
if (-not $FirefoxSetup)
{
	$FirefoxSetup = Join-Path $WorkDir "firefox_setup.exe"
	Invoke-WebRequest "https://download.mozilla.org/?product=firefox-esr-latest-ssl&os=win64&lang=en-US" -OutFile $FirefoxSetup -UseBasicParsing
}
$ffx = Join-Path $WorkDir "firefox_extract"
# The Firefox installer does not accept a quoted path here
Start-Process -FilePath $FirefoxSetup -ArgumentList "/ExtractDir=$ffx" -Wait
$ff_core = Join-Path $ffx "core"
if (-not (Test-Path (Join-Path $ff_core "firefox.exe"))) { throw "Unpacking Firefox from $FirefoxSetup failed" }
Copy-Item -Recurse $ff_core (Join-Path $media "firefox")
New-Item -ItemType Directory (Join-Path $media "firefox\distribution") | Out-Null
Copy-Item (Join-Path $here "policies.json") (Join-Path $media "firefox\distribution\policies.json")

Write-Host "Adding UrBackup to the boot image..."
Run dism.exe @("/Mount-Image", "/ImageFile:$media\sources\boot.wim", "/Index:1", "/MountDir:$mount")
try
{
	$ocs = Join-Path $winpe "amd64\WinPE_OCs"
	foreach ($p in "WinPE-WMI", "WinPE-Scripting")
	{
		Run dism.exe @("/Image:$mount", "/Add-Package", "/PackagePath:$ocs\$p.cab")
		Run dism.exe @("/Image:$mount", "/Add-Package", "/PackagePath:$ocs\en-us\${p}_en-us.cab")
	}
	if ($DriverDir)
	{
		Run dism.exe @("/Image:$mount", "/Add-Driver", "/Driver:$DriverDir", "/Recurse")
	}
	Run dism.exe @("/Image:$mount", "/Set-ScratchSpace:512")

	# The restore client needs a writable working directory, so it runs from the RAM disk (X:)
	$app = Join-Path $mount "urbackup_restore"
	Copy-Item -Recurse $ClientDir $app
	New-Item -ItemType Directory -Force (Join-Path $app "urbackup") | Out-Null
	foreach ($dll in "vcruntime140.dll", "vcruntime140_1.dll", "msvcp140.dll")
	{
		if (-not (Test-Path (Join-Path $app $dll))) { Copy-Item (Join-Path $env:SystemRoot "System32\$dll") $app }
	}
	Copy-Item (Join-Path $here "start_restore.cmd") $app
	Copy-Item (Join-Path $here "restore_client_args.txt") $app
	Copy-Item -Force (Join-Path $here "startnet.cmd") (Join-Path $mount "Windows\System32\startnet.cmd")

	Run dism.exe @("/Unmount-Image", "/MountDir:$mount", "/Commit")
}
catch
{
	& dism.exe /Unmount-Image /MountDir:"$mount" /Discard | Out-Null
	throw
}

Set-Content -Path (Join-Path $media "urbackup_restore_medium.txt") -Value "UrBackup restore medium"
if ($ConnectionSettings) { Copy-Item $ConnectionSettings (Join-Path $media "connection_settings.json") }

Write-Host "Writing $Output..."
Run cmd.exe @("/c", "call `"$denv`" >nul && MakeWinPEMedia /ISO /F `"$WorkDir`" `"$Output`"")
Remove-Item -Recurse -Force $WorkDir
Write-Host "Done: $Output"
