#pragma once

#include "gui/GUIWindowsComponents.h"
#include "FileSettingsReader.h"
#include <wx/thread.h>
#include <wx/timer.h>
#include <Windows.h>
#include <Vss.h>
#include <VsWriter.h>
#include <VsBackup.h>
#include <vector>
#include <map>

struct SComponent
{
	SComponent()
		: is_root(false) { }

	bool is_root;
	VSS_ID writerId;
	std::string name;
	std::string logicalPathComponent;
	std::string logicalPath;
	std::string tooltip;
	bool selectable;
	bool writer;
	std::vector<char> icon;
	std::vector<SComponent*> children;
	std::vector<SComponent*> dependencies;
};

class WindowsComponentReader : public wxThread
{
public:
	WindowsComponentReader();
	~WindowsComponentReader();

	SComponent* getRoot();

	static bool readComponents(const std::string& restoreXml, const std::vector<std::string>& componentXmls, 
		const std::vector<SComponent>& filter_except, SComponent& root, std::vector<SComponent*>& components, std::string& errmsg);

protected:
	virtual ExitCode Entry();

private:
	static std::string GetErrorHResErrStr(HRESULT res);

	static bool wait_for(IVssAsync *vsasync, const std::string& error_prefix, std::string& errmsg);

	std::string errmsg;
	SComponent root;

	static SComponent* getComponent(SComponent* node, VSS_ID writerId, std::string logical_path);

	std::vector<SComponent*> components;
};

class SelectWindowsComponents : public GUIWindowsComponents, public wxTimer
{
public:
	SelectWindowsComponents(wxWindow* parent);
	~SelectWindowsComponents();

	static void addComponents(wxTreeCtrl* tree, wxImageList* iconList,
		wxTreeItemId treeId, SComponent* node, std::map<wxTreeItemId, SComponent*>& tree_components,
		std::map<SComponent*, wxTreeItemId>& tree_items);

	static void selectTreeItems(wxTreeCtrl* tree, std::map<SComponent*, wxTreeItemId>& tree_items,
		SComponent* root, SComponent* node, bool select, bool removeSelect = false);
	static bool hasSelectedChild(wxTreeCtrl* tree, std::map<SComponent*, wxTreeItemId>& tree_items,
		SComponent* node);

protected:
	virtual void Notify(void);
	virtual void evtOnTreeItemGetTooltip(wxTreeEvent& event);
	virtual void evtOnTreeStateImageClick(wxTreeEvent& event);
	virtual void onOkClick(wxCommandEvent& event);
	virtual void onCancel(wxCommandEvent& event);

private:

	void selectTreeItems(SComponent* node, bool select, bool removeSelect = false);
	
	void collectComponents(SComponent* node, size_t& idx, std::string& res);

	WindowsComponentReader componentReader;
	wxImageList* iconList;
	wxImageList* selectList;

	CFileSettingsReader *settings;

	std::map<wxTreeItemId, SComponent*> tree_components;
	std::map<SComponent*, wxTreeItemId> tree_items;
};