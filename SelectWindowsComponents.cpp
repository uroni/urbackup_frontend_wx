#include "SelectWindowsComponents.h"
#include "Connector.h"
#include "Settings.h"
#include <wx/thread.h>
#include <wx/icon.h>
#include <fstream>
#include "stringtools.h"
#include "checked.xpm"
#include "unchecked.xpm"
#ifndef _WIN32
#include "config.h"
#endif

extern wxString res_path;
extern wxString ico_ext;
extern wxBitmapType ico_type;

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
		BSTR bstr;
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

SelectWindowsComponents::SelectWindowsComponents(wxWindow * parent)
	: GUIWindowsComponents(parent)
{
	SetIcon(wxIcon(res_path + wxT("backup-ok.") + ico_ext, ico_type));

	Start(100);

	wxIcon icons[2];
	icons[0] = wxIcon(unchecked_xpm);
	icons[1] = wxIcon(checked_xpm);

	int width = icons[0].GetWidth(),
		height = icons[0].GetHeight();

	selectList = new wxImageList(width, height, true);
	iconList = new wxImageList(width, height, true);

	for (size_t i = 0; i < WXSIZEOF(icons); i++)
		selectList->Add(icons[i]);

	m_treeCtrl1->AssignStateImageList(selectList);
	m_treeCtrl1->AddRoot(_("Loading..."));

#ifdef _DEBUG
	settings = new CFileSettingsReader("urbackup/data/settings.cfg");
#elif _WIN32
	settings = new CFileSettingsReader(g_res_path + "urbackup/data/settings.cfg");
#else
	settings = new CFileSettingsReader(VARDIR "/urbackup/data/settings.cfg");
#endif

	componentReader.Run();
}

SelectWindowsComponents::~SelectWindowsComponents()
{
	delete settings;
}

void SelectWindowsComponents::Notify(void)
{
	if (!componentReader.IsAlive())
	{
		Stop();
		componentReader.Wait();

		m_treeCtrl1->Delete(m_treeCtrl1->GetRootItem());
		wxTreeItemId rootId = m_treeCtrl1->AddRoot(_("Windows components"));

		m_treeCtrl1->SetItemState(rootId, 0);

		SComponent* root = componentReader.getRoot();

		tree_components[rootId] = root;
		tree_items[root] = rootId;

		addComponents(rootId, root);

		m_treeCtrl1->Toggle(rootId);

		m_treeCtrl1->SetImageList(iconList);

		std::string componentsStr;
		if (settings->getValue("vss_select_components", &componentsStr)
			|| settings->getValue("vss_select_components_def", &componentsStr))
		{
			std::map<std::string, std::string> comps;
			ParseParamStrHttp(componentsStr, &comps);

			if (!comps["all"].empty()
				&& comps["all"] != "0")
			{
				for (std::map<wxTreeItemId, SComponent*>::iterator it = tree_components.begin();
					it != tree_components.end(); ++it)
				{
					if (m_treeCtrl1->GetItemState(it->first) == 0)
					{
						selectTreeItems(it->second, true);
					}
				}
			}
			else
			{
				for (std::map<std::string, std::string>::iterator it = comps.begin();
					it != comps.end(); ++it)
				{
					if (next(it->first, 0, "writer_"))
					{
						std::string idx = getafter("writer_", it->first);

						std::string componentName = comps["name_" + idx];
						std::string logicalPath = comps["path_" + idx];
						VSS_ID writerId;
						HRESULT hr = IIDFromString(ConvertToUnicode(comps["writer_" + idx]).c_str(), &writerId);

						if (hr == S_OK)
						{
							for (std::map<wxTreeItemId, SComponent*>::iterator it = tree_components.begin();
								it != tree_components.end(); ++it)
							{
								if (it->second->writerId == writerId
									&& (it->second->name == componentName || (componentName.empty() && it->second->writer) )
									&& it->second->logicalPath == logicalPath
									&& m_treeCtrl1->GetItemState(it->first)==0)
								{
									if (!componentName.empty())
									{
										selectTreeItems(it->second, true);
									}
									else
									{
										wxTreeItemId nodeId = tree_items[it->second];
										m_treeCtrl1->SetItemState(nodeId, 1);
										m_treeCtrl1->Toggle(nodeId);
									}

									break;
								}
							}							
						}
					}
				}
			}
		}
	}
}

void SelectWindowsComponents::evtOnTreeItemGetTooltip(wxTreeEvent & event)
{
	SComponent* component = tree_components[event.GetItem()];

	if (component == NULL)
	{
		return;
	}

	std::string tooltip =component->tooltip;
	if (!tooltip.empty())
	{
		event.SetToolTip(tooltip);
	}
}

void SelectWindowsComponents::evtOnTreeStateImageClick(wxTreeEvent & event)
{
	SComponent* component = tree_components[event.GetItem()];

	if (component == NULL)
	{
		return;
	}

	bool select = m_treeCtrl1->GetItemState(event.GetItem()) == 0;

	selectTreeItems(component, select);
}

void SelectWindowsComponents::onOkClick(wxCommandEvent & event)
{
	std::map<std::string, std::string> n_vals;

	std::string res;
	size_t idx = 0;
	collectComponents(componentReader.getRoot(), idx, res);
	n_vals["vss_select_components"] = res;

	Connector::updateSettings(Settings::mergeNewSettings(settings, n_vals));

	Close();
}

void SelectWindowsComponents::onCancel(wxCommandEvent & event)
{
	Close();
}

void SelectWindowsComponents::addComponents(wxTreeItemId treeId, SComponent * node)
{
	for (size_t i = 0; i < node->children.size(); ++i)
	{
		SComponent* child = node->children[i];

		int image = -1;

		if (!child->icon.empty())
		{
			wxFileName tempFile;
			tempFile.AssignTempFileName("winCompIco");
			if (tempFile.IsOk())
			{
				std::fstream out;
				out.open(tempFile.GetFullPath().ToStdWstring(), std::ios::out | std::ios::binary);

				if (out.is_open())
				{
					out.write(child->icon.data(), child->icon.size());
					out.close();

					wxIcon icon(tempFile.GetFullPath(), wxBITMAP_TYPE_ICO);
					image = iconList->GetImageCount();
					iconList->Add(icon);
				}

				DeleteFile(tempFile.GetFullPath().wc_str());
			}
		}

		int checked = -1;

		if (child->writer)
		{
			checked = 0;
		}
		else if (child->selectable)
		{
			checked = 0;
		}

		wxTreeItemId childId = m_treeCtrl1->AppendItem(treeId, child->name, image);

		if (checked >= 0)
		{
			m_treeCtrl1->SetItemState(childId, checked);
		}

		tree_components[childId] = child;
		tree_items[child] = childId;

		addComponents(childId, child);
	}
}

void SelectWindowsComponents::selectTreeItems(SComponent * node, bool select, bool removeSelect)
{
	wxTreeItemId nodeId = tree_items[node];

	if (m_treeCtrl1->GetItemState(nodeId) != wxTREE_ITEMSTATE_NONE)
	{
		if (select && removeSelect)
		{
			m_treeCtrl1->SetItemState(nodeId, wxTREE_ITEMSTATE_NONE);
		}
		else
		{
			m_treeCtrl1->SetItemState(nodeId, select ? 1 : 0);
		}
	}
	else if (!select && !node->writer && node->selectable)
	{
		m_treeCtrl1->SetItemState(nodeId, 0);
	}

	if (select && !node->writer && node->selectable)
	{
		removeSelect = true;
	}

	if (!node->writer)
	{
		SComponent* root = componentReader.getRoot();
		for (size_t i = 0; i < root->children.size(); ++i)
		{
			if (root->children[i]->writerId == node->writerId)
			{
				if (select)
				{
					m_treeCtrl1->SetItemState(tree_items[root->children[i]], 1);
				}
				else
				{
					if (!hasSelectedChild(root->children[i]))
					{
						m_treeCtrl1->SetItemState(tree_items[root->children[i]], 0);
					}
				}

				break;
			}
		}
	}

	for(size_t i=0;i<node->dependencies.size();++i)
	{
		selectTreeItems(node->dependencies[i], select);
	}

	for (size_t i = 0; i < node->children.size(); ++i)
	{
		selectTreeItems(node->children[i], select, removeSelect);
	}
}

bool SelectWindowsComponents::hasSelectedChild(SComponent * node)
{
	for (size_t i = 0; i < node->children.size(); ++i)
	{
		SComponent* child = node->children[i];

		wxTreeItemId nodeId = tree_items[child];

		if (m_treeCtrl1->GetItemState(nodeId) == 1)
		{
			return true;
		}

		if (hasSelectedChild(child))
		{
			return true;
		}
	}

	return false;
}

void SelectWindowsComponents::collectComponents(SComponent * node, size_t & idx, std::string& res)
{
	wxTreeItemId nodeId = tree_items[node];

	int state = m_treeCtrl1->GetItemState(nodeId);

	if (node != componentReader.getRoot())
	{
		if ( state==1 )
		{
			if (!res.empty())
			{
				res += "&";
			}

			res += "writer_" + convert(idx) + "=" + convert(node->writerId);
			if (node->writer)
			{
				res += "&name_" + convert(idx) + "=";
			}
			else
			{
				res += "&name_" + convert(idx) + "=" + EscapeParamString(node->name);
			}
			res += "&path_" + convert(idx) + "=" + EscapeParamString(node->logicalPath);

			++idx;
		}
	}

	if(node== componentReader.getRoot()
		|| state == 1 )
	{
		for (size_t i = 0; i < node->children.size(); ++i)
		{
			collectComponents(node->children[i], idx, res);
		}
	}
}

WindowsComponentReader::WindowsComponentReader()
	: wxThread(wxTHREAD_JOINABLE)
{
}

WindowsComponentReader::~WindowsComponentReader()
{
	for (size_t i = 0; i < components.size(); ++i)
	{
		delete components[i];
	}
}

SComponent * WindowsComponentReader::getRoot()
{
	return &root;
}

wxThread::ExitCode WindowsComponentReader::Entry()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if (hr != S_OK)
	{
		errmsg = "Error initializing COM " + GetErrorHResErrStr(hr);
		return NULL;
	}

	hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
		RPC_C_IMP_LEVEL_IDENTIFY, NULL, EOAC_NONE, NULL);

	if (hr != S_OK)
	{
		errmsg = "Error initializing COM security " + GetErrorHResErrStr(hr);
		return NULL;
	}

	SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssBackupComponents, backupcom);

	hr = CreateVssBackupComponents(&backupcom);

	if (hr != S_OK || backupcom == NULL)
	{
		errmsg = "Error creating backup components " + GetErrorHResErrStr(hr);
		return NULL;
	}

	hr = backupcom->InitializeForBackup();

	if (hr != S_OK)
	{
		errmsg = "Error initializing components for backup " + GetErrorHResErrStr(hr);
		return NULL;
	}

	SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssAsync, pb_result);
	hr = backupcom->GatherWriterMetadata(&pb_result);

	if (hr != S_OK)
	{
		errmsg="Error gathering writer metadata " + GetErrorHResErrStr(hr);
		return NULL;
	}

	if (!wait_for(pb_result, "Gathering writer metadata failed"))
	{
		return NULL;
	}

	UINT nwriters;
	hr = backupcom->GetWriterMetadataCount(&nwriters);

	if (hr != S_OK)
	{
		errmsg = "Error getting writer metadata count";
		return NULL;
	}

	size_t maxDepth = 0;
	
	for (UINT i = 0; i < nwriters; ++i)
	{
		VSS_ID writerInstance;
		SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssExamineWriterMetadata, writerMetadata);
		HRESULT hr = backupcom->GetWriterMetadata(i, &writerInstance, &writerMetadata);

		if (hr != S_OK)
		{
			errmsg = "Getting metadata of writer " + convert(i) + " failed. VSS error code " + GetErrorHResErrStr(hr);
			continue;
		}

		VSS_ID instanceId;
		VSS_ID writerId;
		SCOPED_DECLARE_FREE_BSTR(writerName);
		VSS_USAGE_TYPE usageType;
		VSS_SOURCE_TYPE sourceType;
		hr = writerMetadata->GetIdentity(&instanceId, &writerId, &writerName, &usageType, &sourceType);

		if (hr != S_OK)
		{
			errmsg = "Getting identity of writer " + convert(i) + " failed. VSS error code " + GetErrorHResErrStr(hr);
			continue;
		}

		//Always skip System writer and ASR writer
		if (convert(writerId) == "{E8132975-6F93-4464-A53E-1050253AE220}"
			|| convert(writerId) == "{BE000CBE-11FE-4426-9C58-531AA6355FC4}")
		{
			continue;
		}

		std::string writerNameStr = ConvertFromWchar(writerName);

		UINT nIncludeFiles, nExcludeFiles, nComponents;
		hr = writerMetadata->GetFileCounts(&nIncludeFiles, &nExcludeFiles, &nComponents);
		if (hr != S_OK)
		{
			errmsg = "GetFileCounts of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr);
			continue;
		}

		if (nComponents > 0)
		{
			SComponent* component = new SComponent;

			component->writerId = writerId;
			component->name = writerNameStr;
			component->selectable = true;
			component->writer = true;

			components.push_back(component);

			if (maxDepth == 0) maxDepth = 1;
		}

		for (UINT j = 0; j < nComponents; ++j)
		{
			SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssWMComponent, wmComponent);
			hr = writerMetadata->GetComponent(j, &wmComponent);
			if (hr != S_OK)
			{
				errmsg = "Error getting component " + convert(j) + " of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr);
				continue;
			}

			SCOPED_DECLARE_FREE_COMPONENTINFO(wmComponent, componentInfo);
			hr = wmComponent->GetComponentInfo(&componentInfo);
			if (hr != S_OK)
			{
				errmsg = "Error getting component info " + convert(j) + " of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr);
				continue;
			}

			std::string componentNameStr = ConvertFromWchar(componentInfo->bstrComponentName);

			std::string logicalPathStr;
			if (componentInfo->bstrLogicalPath != NULL)
			{
				logicalPathStr = ConvertFromWchar(componentInfo->bstrLogicalPath);
			}

			SComponent* component = new SComponent;

			component->logicalPath = logicalPathStr;
			component->name = componentNameStr;
			component->writerId = writerId;
			component->writer = false;
			component->selectable = componentInfo->bSelectable == TRUE;

			if (componentInfo->bstrCaption != NULL)
			{
				component->tooltip = ConvertFromWchar(componentInfo->bstrCaption);
			}

			if (componentInfo->cbIcon > 0)
			{
				int abc = 5;
			}

			component->icon.resize(componentInfo->cbIcon);
			memcpy(component->icon.data(), componentInfo->pbIcon, componentInfo->cbIcon);

			components.push_back(component);

			std::vector<std::string> toks;
			TokenizeMail(logicalPathStr, toks, "\\");

			if (toks.size() + 2 > maxDepth)
			{
				maxDepth = toks.size() + 2;
			}
		}

	}

	for (size_t depth = 0; depth < maxDepth; ++depth)
	{
		for (size_t i = 0; i < components.size(); ++i)
		{
			std::vector<std::string> toks;
			TokenizeMail(components[i]->logicalPath, toks, "\\");

			SComponent* parent = NULL;
			if (components[i]->writer
				&& depth==0)
			{
				parent = &root;
			}
			else if (!components[i]->writer
				&& toks.size()+1 == depth)
			{
				std::string parentPath;
				if (!toks.empty()
					&& components[i]->logicalPath.size()>toks[toks.size() - 1].size())
				{
					parentPath = components[i]->logicalPath.substr(0, components[i]->logicalPath.size() - toks[toks.size() - 1].size() - 1);
				}
				parent = getComponent(&root, components[i]->writerId, parentPath);
			}

			if (parent != NULL)
			{
				if (!toks.empty())
				{
					size_t off = 0;
					if (!parent->logicalPath.empty())
					{
						off = parent->logicalPath.size() + 1;
					}
					components[i]->logicalPathComponent = components[i]->logicalPath.substr(off);
				}
				parent->children.push_back(components[i]);
			}
		}
	}


	for (UINT i = 0; i < nwriters; ++i)
	{
		VSS_ID writerInstance;
		SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssExamineWriterMetadata, writerMetadata);
		HRESULT hr = backupcom->GetWriterMetadata(i, &writerInstance, &writerMetadata);

		if (hr != S_OK)
		{
			errmsg = "Getting metadata of writer " + convert(i) + " failed. VSS error code " + GetErrorHResErrStr(hr);
			continue;
		}

		VSS_ID instanceId;
		VSS_ID writerId;
		SCOPED_DECLARE_FREE_BSTR(writerName);
		VSS_USAGE_TYPE usageType;
		VSS_SOURCE_TYPE sourceType;
		hr = writerMetadata->GetIdentity(&instanceId, &writerId, &writerName, &usageType, &sourceType);

		if (hr != S_OK)
		{
			errmsg = "Getting identity of writer " + convert(i) + " failed. VSS error code " + GetErrorHResErrStr(hr);
			continue;
		}

		//Always skip System writer and ASR writer
		if (convert(writerId) == "{E8132975-6F93-4464-A53E-1050253AE220}"
			|| convert(writerId) == "{BE000CBE-11FE-4426-9C58-531AA6355FC4}")
		{
			continue;
		}

		std::string writerNameStr = ConvertFromWchar(writerName);

		UINT nIncludeFiles, nExcludeFiles, nComponents;
		hr = writerMetadata->GetFileCounts(&nIncludeFiles, &nExcludeFiles, &nComponents);
		if (hr != S_OK)
		{
			errmsg = "GetFileCounts of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr);
			continue;
		}

		for (UINT j = 0; j < nComponents; ++j)
		{
			SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssWMComponent, wmComponent);
			hr = writerMetadata->GetComponent(j, &wmComponent);
			if (hr != S_OK)
			{
				errmsg = "Error getting component " + convert(j) + " of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr);
				continue;
			}

			SCOPED_DECLARE_FREE_COMPONENTINFO(wmComponent, componentInfo);
			hr = wmComponent->GetComponentInfo(&componentInfo);
			if (hr != S_OK)
			{
				errmsg = "Error getting component info " + convert(j) + " of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr);
				continue;
			}

			std::string componentNameStr = ConvertFromWchar(componentInfo->bstrComponentName);

			std::string logicalPathStr;
			if (componentInfo->bstrLogicalPath != NULL)
			{
				logicalPathStr = ConvertFromWchar(componentInfo->bstrLogicalPath);
			}

			SComponent* component = getComponent(&root, writerId, logicalPathStr);

			if (component == NULL)
			{
				errmsg = "Error getting component node of component \"" +
					componentNameStr + "\" of writer \"" + writerNameStr + "\"";
				continue;
			}

			for (UINT k = 0; k < componentInfo->cDependencies; ++k)
			{
				SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssWMDependency, wmDependency);
				hr = wmComponent->GetDependency(k, &wmDependency);

				if (hr != S_OK)
				{
					errmsg = "Error getting component dependency " + convert(k) + " of component \"" +
						componentNameStr + "\" of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr);
					continue;
				}

				SCOPED_DECLARE_FREE_BSTR(componentName);
				hr = wmDependency->GetComponentName(&componentName);

				if (hr != S_OK)
				{
					errmsg = "Error getting component name of dependency " + convert(k) + " of component \"" +
						componentNameStr + "\" of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr);
					continue;
				}

				SCOPED_DECLARE_FREE_BSTR(logicalPath);
				hr = wmDependency->GetLogicalPath(&logicalPath);

				if (hr != S_OK)
				{
					errmsg = "Error getting logical path of dependency " + convert(k) + " of component \"" +
						componentNameStr + "\" of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr);
					continue;
				}

				std::string dependencylogicalPathStr;
				if (logicalPath != NULL)
				{
					dependencylogicalPathStr = ConvertFromWchar(logicalPath);
				}

				VSS_ID dependencyWriterId;
				hr = wmDependency->GetWriterId(&dependencyWriterId);

				if (hr != S_OK)
				{
					errmsg = "Error getting writer id of dependency " + convert(k) + " of component \"" +
						componentNameStr + "\" of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr);
					continue;
				}

				SComponent* dependency = getComponent(&root, dependencyWriterId, dependencylogicalPathStr);

				if (dependency == NULL)
				{
					errmsg = "Could not find dependency " + convert(k) + " of component \"" +
						componentNameStr + "\" of writer \"" + writerNameStr + "\"";
				}
			}
		}
	}

	return ExitCode();
}

std::string WindowsComponentReader::GetErrorHResErrStr(HRESULT res)
{
#define CASE_VSS_ERROR(x) case x: return #x
	switch (res)
	{
	case E_INVALIDARG:
		return "E_INVALIDARG";
	case E_OUTOFMEMORY:
		return "E_OUTOFMEMORY";
	case E_UNEXPECTED:
		return "E_UNEXPECTED";
	case E_ACCESSDENIED:
		return "E_ACCESSDENIED";
	case VSS_E_OBJECT_NOT_FOUND:
		return "VSS_E_OBJECT_NOT_FOUND";
	case VSS_E_PROVIDER_VETO:
		return "VSS_E_PROVIDER_VETO";
	case VSS_E_UNEXPECTED_PROVIDER_ERROR:
		return "VSS_E_UNEXPECTED_PROVIDER_ERROR";
	case VSS_E_BAD_STATE:
		return "VSS_E_BAD_STATE";
	case VSS_E_SNAPSHOT_SET_IN_PROGRESS:
		return "VSS_E_SNAPSHOT_SET_IN_PROGRESS";
	case VSS_E_MAXIMUM_NUMBER_OF_VOLUMES_REACHED:
		return "VSS_E_MAXIMUM_NUMBER_OF_VOLUMES_REACHED";
	case VSS_E_MAXIMUM_NUMBER_OF_SNAPSHOTS_REACHED:
		return "VSS_E_MAXIMUM_NUMBER_OF_SNAPSHOTS_REACHED";
	case VSS_E_PROVIDER_NOT_REGISTERED:
		return "VSS_E_PROVIDER_NOT_REGISTERED";
	case VSS_E_VOLUME_NOT_SUPPORTED:
		return "VSS_E_VOLUME_NOT_SUPPORTED";
	case VSS_E_VOLUME_NOT_SUPPORTED_BY_PROVIDER:
		return "VSS_E_VOLUME_NOT_SUPPORTED_BY_PROVIDER";
		CASE_VSS_ERROR(VSS_E_INSUFFICIENT_STORAGE);
		CASE_VSS_ERROR(VSS_E_FLUSH_WRITES_TIMEOUT);
		CASE_VSS_ERROR(VSS_E_HOLD_WRITES_TIMEOUT);
		CASE_VSS_ERROR(VSS_E_NESTED_VOLUME_LIMIT);
		CASE_VSS_ERROR(VSS_E_REBOOT_REQUIRED);
		CASE_VSS_ERROR(VSS_E_TRANSACTION_FREEZE_TIMEOUT);
		CASE_VSS_ERROR(VSS_E_TRANSACTION_THAW_TIMEOUT);
	};
#undef CASE_VSS_ERROR
	return "UNDEF(" + nconvert((long long int)res) + ")";
}

bool WindowsComponentReader::wait_for(IVssAsync * vsasync, const std::string & error_prefix)
{
	if (vsasync == NULL)
	{
		errmsg = "vsasync is NULL";
		return false;
	}

	HRESULT hr = vsasync->Wait();
	if (hr != S_OK)
	{
		errmsg = "Waiting for async result failed " + GetErrorHResErrStr(hr);
		return false;
	}

	HRESULT res;
	hr = vsasync->QueryStatus(&res, NULL);
	if (hr != S_OK)
	{
		errmsg = "Querying async result status failed " + GetErrorHResErrStr(hr);
		return false;
	}

	while (res == VSS_S_ASYNC_PENDING)
	{
		hr = vsasync->Wait();

		if (hr != S_OK)
		{
			errmsg = "Waiting for async result failed (2) " + GetErrorHResErrStr(hr);
			return false;
		}

		hr = vsasync->QueryStatus(&res, NULL);

		if (hr != S_OK)
		{
			errmsg = "Querying async result status failed (2) " + GetErrorHResErrStr(hr);
			return false;
		}
	}

	if (res != VSS_S_ASYNC_FINISHED)
	{
		errmsg= error_prefix + ". Error code " + GetErrorHResErrStr(res);
		return false;
	}
	return true;
}

SComponent * WindowsComponentReader::getComponent(SComponent* node, VSS_ID writerId, std::string logical_path)
{
	if (node == &root)
	{
		for (size_t i = 0; i < node->children.size(); ++i)
		{
			if (node->children[i]->writerId == writerId)
			{
				if (logical_path.empty())
				{
					return node->children[i];
				}

				SComponent* ret = getComponent(node->children[i], writerId, logical_path);

				if (ret == NULL)
				{
					return node->children[i];
				}
				else
				{
					return ret;
				}
			}
		}

		return NULL;
	}

	std::string remainingPath = logical_path;
	std::string nextPath;

	do
	{
		if (!nextPath.empty())
		{
			nextPath += "\\";
		}
		nextPath += getuntil("\\", remainingPath);
		std::string prevRemainingPath = remainingPath;
		remainingPath = getafter("\\", remainingPath);

		if (nextPath.empty())
		{
			nextPath += prevRemainingPath;
			remainingPath.clear();
		}

		for (size_t i = 0; i < node->children.size(); ++i)
		{
			if (node->children[i]->writerId == writerId
				&& node->children[i]->logicalPathComponent == nextPath)
			{
				if (remainingPath.empty())
				{
					return node;
				}
				else
				{
					return getComponent(node->children[i], writerId, remainingPath);
				}
			}
		}
	} while (!remainingPath.empty());

	return NULL;
}
