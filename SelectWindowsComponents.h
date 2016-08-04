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
		: is_root(false), parent(NULL){ }

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
	SComponent* parent;
};

std::string GetErrorHResErrStr(HRESULT res);

class WindowsComponentReader : public wxThread
{
public:
	WindowsComponentReader();
	~WindowsComponentReader();

	SComponent* getRoot();

	static bool readComponents(const std::string& restoreXml, const std::vector<std::string>& componentXmls, 
		const std::vector<SComponent>& filter_except, SComponent& root, std::vector<SComponent*>& components, std::string& errmsg);

	static bool wait_for(IVssAsync *vsasync, const std::string& error_prefix, std::string& errmsg);

protected:
	virtual ExitCode Entry();

private:

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
	static bool allChildrenSelected(wxTreeCtrl* tree, std::map<SComponent*, wxTreeItemId>& tree_items,
		SComponent* node, bool& has_selectable_child);

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

namespace
{
	template<typename T>
	class ReleaseIUnknown
	{
	public:
		ReleaseIUnknown(T*& unknown)
			: unknown(unknown) {}

		~ReleaseIUnknown() {
			if (unknown != NULL) {
				unknown->Release();
			}
		}

	private:
		T*& unknown;
	};

#define TOKENPASTE2(x, y) x ## y
#define TOKENPASTE(x, y) TOKENPASTE2(x, y)

#define SCOPED_DECLARE_RELEASE_IUNKNOWN(t, x) t* x = NULL; ReleaseIUnknown<t> TOKENPASTE(ReleaseIUnknown_,__LINE__) (x)

	class FreeBStr
	{
	public:
		FreeBStr(BSTR& bstr)
			: bstr(bstr)
		{}

		~FreeBStr() {
			if (bstr != NULL) {
				SysFreeString(bstr);
			}
		}
	private:
		BSTR& bstr;
	};

#define SCOPED_DECLARE_FREE_BSTR(x) BSTR x = NULL; FreeBStr TOKENPASTE(FreeBStr_, __LINE__) (x)

	class FreeComponentInfo
	{
	public:
		FreeComponentInfo(IVssWMComponent* wmComponent, PVSSCOMPONENTINFO& componentInfo)
			: wmComponent(wmComponent), componentInfo(componentInfo)
		{}

		~FreeComponentInfo() {
			if (wmComponent != NULL && componentInfo != NULL) {
				wmComponent->FreeComponentInfo(componentInfo);
			}
		}
	private:
		IVssWMComponent* wmComponent;
		PVSSCOMPONENTINFO componentInfo;
	};

#define SCOPED_DECLARE_FREE_COMPONENTINFO(c, i) PVSSCOMPONENTINFO i = NULL; FreeComponentInfo TOKENPASTE(FreeComponentInfo_,__LINE__) (c, i);

	std::string convert(VSS_ID id)
	{
		WCHAR GuidStr[128] = {};
		int rc = StringFromGUID2(id, GuidStr, 128);
		if (rc > 0)
		{
			return wxString(std::wstring(GuidStr, rc - 1)).ToStdString();
		}
		return std::string();
	}

	std::string ConvertFromWchar(std::wstring str)
	{
		return ConvertToUTF8(str);
	}
}