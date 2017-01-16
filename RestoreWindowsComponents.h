#pragma once

#include "gui/GUIRestoreWindowsComponents.h"
#include "SelectWindowsComponents.h"
#include <wx/timer.h>
#include <wx/thread.h>

struct SFileSpec
{
	std::string path;
	std::string spec;
	bool recursive;
	std::string altPath;

	bool operator==(const SFileSpec& other) const
	{
		return path == other.path
			&& spec == other.spec
			&& recursive == other.recursive;
	}
};

struct SRestoreComponent
{
	SFileSpec currspec;
	std::vector<SFileSpec> redirected_locations;
	std::string writerName;
	VSS_ID writerId;
	UINT componentIdx;
	std::string componentName;
	std::string filesPrefix;
	UINT filesIdx;
	std::string logicalPath;
	VSS_COMPONENT_TYPE type;
	long long int restoreFlags;
	bool hasComponent;
};

class RestoreWindowsComponentsThread : public wxThread
{
public:
	RestoreWindowsComponentsThread(int backupid,
		std::vector<SComponent> selected_components, wxString componentConfigDir);
	~RestoreWindowsComponentsThread();

	void log(const std::string& message);
	void log(const wxString& message);
	void log(const char* message) { log(std::string(message)); }

	std::vector<std::string> getNewLogMessages();

	wxString getMessage1();
	wxString getMessage2();

	void setMessage1(wxString msg);
	void setMessage2(wxString msg);

	int getProgressBarPc();
	void setProgressBarPc(int pc);

protected:
	virtual ExitCode Entry();

private:
	bool restoreFiles(const SRestoreComponent& comp);

	bool getFilespec(IVssWMFiledesc* wmFile, SFileSpec& filespec);

	bool readAltLocations(std::vector<SFileSpec> alt_locations);

	bool stopService(const std::string& service_name);
	bool startService(const std::string& service_name);

	int backupid;
	std::vector<SComponent> selected_components;
	wxString componentConfigDir;
	wxCriticalSection log_section;
	std::vector<std::string> log_messages;

	std::vector<SRestoreComponent> restore_components;

	int progress_bar_pc;

	wxString message1;
	wxString message2;
};

class RestoreWindowsComponents : public GUIRestoreComponents, public wxTimer
{
public:
	RestoreWindowsComponents(wxWindow* parent, int backupid,
		std::vector<SComponent> selected_components, wxString componentConfigDir);

protected:
	virtual void Notify(void);
	virtual void onOkClick(wxCommandEvent& event);

private:
	RestoreWindowsComponentsThread restoreComponentsThread;
};