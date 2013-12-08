ACLOCAL_AMFLAGS = -I m4
bin_PROGRAMS = urbackup_client_gui
urbackup_client_gui_SOURCES = gui/GUI.cpp main.cpp ConfigPath.cpp Connector.cpp escape.cpp FileSettingsReader.cpp Info.cpp Logs.cpp Settings.cpp stringtools.cpp TaskBarBaloon.cpp tcpstack.cpp TrayIcon.cpp
AM_CXXFLAGS = -DLINUX
noinst_HEADERS= ConfigPath.h Connector.h escape.h FileSettingsReader.h Info.h Logs.h main.h resource.h Settings.h stringtools.h TaskBarBaloon.h tcpstack.h TrayIcon.h utf8/utf8.h utf8/utf8/checked.h utf8/utf8/core.h utf8/utf8/unchecked.h gui/GUI.h capa_bits.h
EXTRA_DIST=data/info.txt data/backup-bad.xpm data/backup-ok.xpm data/backup-ok-big.xpm data/backup-progress.xpm data/backup-progress-pause.xpm data/logo1.png lang/it/urbackup.mo lang/pl/urbackup.mo lang/pt_BR/urbackup.mo lang/sk/urbackup.mo lang/en/urbackup.mo lang/zh_TW/urbackup.mo lang/zh_CN/urbackup.mo lang/de/urbackup.mo lang/es/urbackup.mo lang/fr/urbackup.mo lang/ru/urbackup.mo lang/uk/urbackup.mo
install-data-local: data/info.txt data/backup-bad.xpm data/backup-ok.xpm data/backup-ok-big.xpm data/backup-progress.xpm data/backup-progress-pause.xpm data/logo1.png lang/*/*.mo
	mkdir -p "$(DESTDIR)$(datadir)/urbackup"
	$(INSTALL_DATA) $(srcdir)/data/*.png "$(DESTDIR)$(datadir)/urbackup/"
	$(INSTALL_DATA) $(srcdir)/data/*.xpm "$(DESTDIR)$(datadir)/urbackup/"
	$(INSTALL_DATA) $(srcdir)/data/info.txt "$(DESTDIR)$(datadir)/urbackup/"
	$(INSTALL_DATA) -D $(srcdir)/lang/it/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/it/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/lang/pl/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/pl/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/lang/pt_BR/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/pt_BR/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/lang/sk/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/sk/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/lang/en/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/en/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/lang/zh_TW/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/zh_TW/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/lang/zh_CN/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/zh_CN/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/lang/de/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/de/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/lang/es/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/es/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/lang/fr/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/fr/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/lang/ru/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/ru/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/lang/uk/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/uk/urbackup.mo"
	
