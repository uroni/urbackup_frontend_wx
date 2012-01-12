ACLOCAL_AMFLAGS = -I m4
bin_PROGRAMS = urbackup_client_gui
urbackup_client_gui_SOURCES = gui/GUI.cpp main.cpp ConfigPath.cpp Connector.cpp escape.cpp FileSettingsReader.cpp Info.cpp Logs.cpp Settings.cpp stringtools.cpp TaskBarBaloon.cpp tcpstack.cpp TrayIcon.cpp
AM_CXXFLAGS = -DLINUX
noinst_HEADERS= ConfigPath.h Connector.h escape.h FileSettingsReader.h Info.h Logs.h main.h resource.h Settings.h stringtools.h TaskBarBaloon.h tcpstack.h TrayIcon.h utf8/utf8.h utf8/utf8/checked.h utf8/utf8/core.h utf8/utf8/unchecked.h gui/GUI.h capa_bits.h
EXTRA_DIST=data/info.txt data/backup-bad.xpm data/backup-ok.xpm data/backup-ok-big.xpm data/backup-progress.xpm data/backup-progress-pause.xpm data/en/trans.mo data/fr/trans.mo data/logo1.png
install-data-local: data/info.txt data/backup-bad.xpm data/backup-ok.xpm data/backup-ok-big.xpm data/backup-progress.xpm data/backup-progress-pause.xpm data/logo1.png data/en/trans.mo data/fr/trans.mo
	mkdir -p "$(DESTDIR)$(datadir)/urbackup"
	$(INSTALL_DATA) $(srcdir)/data/*.png "$(DESTDIR)$(datadir)/urbackup/"
	$(INSTALL_DATA) $(srcdir)/data/*.xpm "$(DESTDIR)$(datadir)/urbackup/"
	$(INSTALL_DATA) $(srcdir)/data/info.txt "$(DESTDIR)$(datadir)/urbackup/"
	$(INSTALL_DATA) -D $(srcdir)/data/en/trans.mo "$(DESTDIR)$(datadir)/urbackup/en/trans.mo"
	$(INSTALL_DATA) -D $(srcdir)/data/fr/trans.mo "$(DESTDIR)$(datadir)/urbackup/fr/trans.mo"