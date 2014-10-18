ACLOCAL_AMFLAGS = -I m4
bin_PROGRAMS = urbackup_client_gui
urbackup_client_gui_SOURCES = gui/GUI.cpp main.cpp ConfigPath.cpp Connector.cpp escape.cpp FileSettingsReader.cpp Info.cpp Logs.cpp Settings.cpp stringtools.cpp TaskBarBaloon.cpp tcpstack.cpp TrayIcon.cpp Status.cpp jsoncpp.cpp TranslationHelper.cpp
AM_CPPFLAGS = -g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -D_FORTIFY_SOURCE=2
AM_CXXFLAGS = -DLINUX
noinst_HEADERS= ConfigPath.h Connector.h escape.h FileSettingsReader.h Info.h Logs.h main.h resource.h Settings.h stringtools.h TaskBarBaloon.h tcpstack.h TrayIcon.h utf8/utf8.h utf8/utf8/checked.h utf8/utf8/core.h utf8/utf8/unchecked.h gui/GUI.h capa_bits.h Status.h json/json.h json/json-forwards.h TranslationHelper.h
EXTRA_DIST=data/info.txt data/backup-bad.xpm data/backup-ok.xpm data/backup-ok-big.xpm data/backup-progress.xpm data/backup-progress-pause.xpm data/backup-no-server.xpm data/backup-no-recent.xpm data/backup-indexing.xpm data/logo1.png data/lang/it/urbackup.mo data/lang/pl/urbackup.mo data/lang/pt_BR/urbackup.mo data/lang/sk/urbackup.mo data/lang/zh_TW/urbackup.mo data/lang/zh_CN/urbackup.mo data/lang/de/urbackup.mo data/lang/es/urbackup.mo data/lang/fr/urbackup.mo data/lang/ru/urbackup.mo data/lang/uk/urbackup.mo data/lang/da/urbackup.mo data/lang/nl/urbackup.mo data/lang/fa/urbackup.mo data/lang/cs/urbackup.mo
install-data-local: data/info.txt data/backup-bad.xpm data/backup-ok.xpm data/backup-ok-big.xpm data/backup-progress.xpm data/backup-progress-pause.xpm data/backup-no-server.xpm data/backup-no-recent.xpm data/backup-indexing.xpm data/logo1.png data/lang/*/*.mo
	mkdir -p "$(DESTDIR)$(datadir)/urbackup"
	$(INSTALL_DATA) $(srcdir)/data/*.png "$(DESTDIR)$(datadir)/urbackup/"
	$(INSTALL_DATA) $(srcdir)/data/*.xpm "$(DESTDIR)$(datadir)/urbackup/"
	$(INSTALL_DATA) $(srcdir)/data/info.txt "$(DESTDIR)$(datadir)/urbackup/"
	$(INSTALL_DATA) -D $(srcdir)/data/lang/it/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/it/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/data/lang/pl/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/pl/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/data/lang/pt_BR/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/pt_BR/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/data/lang/sk/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/sk/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/data/lang/zh_TW/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/zh_TW/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/data/lang/zh_CN/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/zh_CN/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/data/lang/de/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/de/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/data/lang/es/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/es/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/data/lang/fr/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/fr/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/data/lang/ru/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/ru/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/data/lang/uk/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/uk/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/data/lang/da/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/da/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/data/lang/nl/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/nl/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/data/lang/fa/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/fa/urbackup.mo"
	$(INSTALL_DATA) -D $(srcdir)/data/lang/cs/urbackup.mo "$(DESTDIR)$(datadir)/urbackup/lang/cs/urbackup.mo"
	
