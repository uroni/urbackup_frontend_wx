#pragma once

#include "gui/GUIRestoreWindowsComponents.h"
#include <wx/timer.h>
#include <wx/thread.h>
#include "Connector.h"
#include "SelectWindowsComponents.h"

wxString accessErrorToString(Connector::EAccessError access_error);

class RetrieveComponentBackups : public wxThread
{
public:
	RetrieveComponentBackups();
	~RetrieveComponentBackups();

	bool hasSuccess() { return success;  }
	std::vector<SBackupFile>& getFileList() { return filelist; }
	Connector::EAccessError getAccessError() {
		return access_error;
	}

protected:
	virtual ExitCode Entry();

private:
	Connector::EAccessError access_error;
	bool success;
	std::vector<SBackupFile> filelist;
};

class RetrieveComponentConfig : public wxThread
{
public:
	RetrieveComponentConfig();
	~RetrieveComponentConfig();

	void reset(int pbackupid) { 
		backupid = pbackupid;
		success = false; }
	bool hasSuccess() { return success; }
	Connector::EAccessError getAccessError() {
		return access_error;
	}

	SComponent* getRoot() { return &root; }

	std::string getErrMsg() { return errmsg; }

	wxString getComponentConfigDir() {
		return componentConfigDir;
	}

	int getBackupid() { return backupid;  }

protected:
	virtual ExitCode Entry();

private:
	int backupid;
	Connector::EAccessError access_error;
	bool success;
	SComponent root;
	std::vector<SComponent*> components;
	std::string errmsg;
	wxString componentConfigDir;
};

class SelectRestoreComponents : public GUISelectRestoreComponents, public wxTimer
{
public:
	SelectRestoreComponents(wxWindow* parent);

protected:
	virtual void Notify(void);
	virtual void onListBoxSelected(wxCommandEvent& event);
	virtual void onStartRestore(wxCommandEvent& event);
	virtual void evtOnTreeStateImageClick(wxTreeEvent& event);
	virtual void onCancel(wxCommandEvent& event);

private:
	RetrieveComponentBackups retrieveComponentBackups;
	std::auto_ptr<RetrieveComponentConfig> retrieveComponentConfig;
	wxImageList* iconList;
	wxImageList* selectList;

	bool canSelectBackup;
	bool retrievingBackupConfig;

	std::map<wxTreeItemId, SComponent*> tree_components;
	std::map<SComponent*, wxTreeItemId> tree_items;

	wxString componentConfigDir;
	int backupid;
};