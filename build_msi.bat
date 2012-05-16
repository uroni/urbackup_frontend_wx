candle -ext WixUtilExtension -ext WixFirewallExtension urbackup.wxs
light urbackup.wixobj -ext WixUtilExtension -ext WixFirewallExtension -ext WixUIExtension -out "UrBackup Client 1.0-1(x64).msi"
pause