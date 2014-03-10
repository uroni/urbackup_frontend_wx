set version=1.4-1

candle -ext WixUtilExtension -ext WixFirewallExtension urbackup.wxs
light urbackup.wixobj -ext WixUtilExtension -ext WixFirewallExtension -ext WixUIExtension -out "UrBackup Client %version%(x64).msi"

cp urbackup.wxs urbackup_notray.wxs
patch urbackup_notray.wxs urbackup.wxs.notray.patch

candle -ext WixUtilExtension -ext WixFirewallExtension urbackup_notray.wxs
light urbackup_notray.wixobj -ext WixUtilExtension -ext WixFirewallExtension -ext WixUIExtension -out "UrBackup Client (No tray) %version%(x64).msi"


pause