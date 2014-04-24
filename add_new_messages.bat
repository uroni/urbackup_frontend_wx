xgettext gui\GUI.h gui\GUI.cpp *.cpp *.h --keyword="_" --keyword="wxPLURAL:1,2" --from-code=utf-8
tx pull -l en
msgmerge -N translations\urbackup.frontend\en.po messages.po > translations\urbackup.frontend\en_new.po
