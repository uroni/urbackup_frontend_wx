ACLOCAL_AMFLAGS = -I m4
bin_PROGRAMS = urbackup_client_gui
urbackup_client_gui_SOURCES = gui/GUI.cpp main.cpp ConfigPath.cpp Connector.cpp escape.cpp FileSettingsReader.cpp Info.cpp Logs.cpp Settings.cpp stringtools.cpp TaskBarBaloon.cpp tcpstack.cpp TrayIcon.cpp
AM_CXXFLAGS = -DLINUX
noinst_HEADERS= ConfigPath.h Connector.h escape.h FileSettingsReader.h Info.h Logs.h main.h resource.h Settings.h stringtools.h TaskBarBaloon.h tcpstack.h TrayIcon.h utf8/utf8.h utf8/utf8/checked.h utf8/utf8/core.h utf8/utf8/unchecked.h gui/GUI.h
EXTRA_DIST=data/info.txt data/backup-bad.ico data/backup-ok.ico data/backup-ok-big.ico data/backup-progress.ico data/backup-progress-pause.ico data/en/trans.mo data/fr/trans.mo
install-data-local: "$(srcdir)/data/info.txt" "$(srcdir)/data/backup-bad.ico" "$(srcdir)/data/backup-ok.ico" "$(srcdir)/data/backup-ok-big.ico" "$(srcdir)/data/backup-progress.ico" "$(srcdir)/data/backup-progress-pause.ico" "$(srcdir)/data/en/trans.mo" "$(srcdir)/data/fr/trans.mo"
	$(INSTALL_DATA) -D "$(srcdir)/data/*" "$(DESTDIR)$(sharedstatedir)/urbackup/"
	$(INSTALL_DATA) -D "$(srcdir)/data/en/trans.mo" "$(DESTDIR)$(sharedstatedir)/urbackup/data/en/"
	$(INSTALL_DATA) -D "$(srcdir)/data/fr/trans.mo" "$(DESTDIR)$(sharedstatedir)/urbackup/data/fr/"
	