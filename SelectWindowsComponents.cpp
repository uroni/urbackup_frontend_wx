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
extern std::string g_res_path;

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

		addComponents(m_treeCtrl1, iconList, rootId, root, tree_components,
			tree_items);

		m_treeCtrl1->Toggle(rootId);

		m_treeCtrl1->SetImageList(iconList);

		std::string componentsStr;
		if (settings->getValue("vss_select_components", &componentsStr)
			|| settings->getValue("vss_select_components_def", &componentsStr))
		{
			std::map<std::string, std::string> comps;
			ParseParamStrHttp(componentsStr, &comps);

			bool has_comps = true;
			if (!comps["all"].empty()
				&& comps["all"] != "0")
			{
				has_comps = false;

				for (std::map<wxTreeItemId, SComponent*>::iterator it = tree_components.begin();
					it != tree_components.end(); ++it)
				{
					if (m_treeCtrl1->GetItemState(it->first) == 0)
					{
						selectTreeItems(it->second, true);
					}
				}
			}
			else if (!comps["default"].empty()
				&& comps["default"] != "0")
			{
				comps["writer_0"] = "{a65faa63-5ea8-4ebc-9dbd-a0c4db26912a}"; //MS SQL Server 2014
				comps["writer_1"] = "{76fe1ac4-15f7-4bcd-987e-8e1acb462fb7}"; //MS Exchange 2010
			}

			if(has_comps)
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
								if (!it->second->is_root
									&& it->second->writerId == writerId
									&& (it->second->name == componentName || (componentName.empty() && it->second->writer) )
									&& it->second->logicalPath == logicalPath
									&& m_treeCtrl1->GetItemState(it->first)==0)
								{
									selectTreeItems(it->second, true);

									if (!it->second->children.empty())
									{
										wxTreeItemId nodeId = tree_items[it->second];
										m_treeCtrl1->ExpandAllChildren(nodeId);
									}

									SComponent* node = it->second->parent;
									while (node != NULL)
									{
										wxTreeItemId nodeId = tree_items[node];
										m_treeCtrl1->Expand(nodeId);
										node = node->parent;
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
	if (m_treeCtrl1->GetItemState(tree_items[componentReader.getRoot()]) == 1)
	{
		res = "all=1";
	}
	else
	{
		size_t idx = 0;
		collectComponents(componentReader.getRoot(), idx, res);
	}

	n_vals["vss_select_components"] = res;

	Connector::updateSettings(Settings::mergeNewSettings(settings, n_vals));

	Close();
}

void SelectWindowsComponents::onCancel(wxCommandEvent & event)
{
	Close();
}

void SelectWindowsComponents::addComponents(wxTreeCtrl* tree, wxImageList* iconList, wxTreeItemId treeId,
	SComponent * node, std::map<wxTreeItemId, SComponent*>& tree_components,
	std::map<SComponent*, wxTreeItemId>& tree_items)
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

		wxTreeItemId childId = tree->AppendItem(treeId, child->name, image);

		if (checked >= 0)
		{
			tree->SetItemState(childId, checked);
		}

		tree_components[childId] = child;
		tree_items[child] = childId;

		addComponents(tree, iconList, childId, child, tree_components, tree_items);
	}
}

void SelectWindowsComponents::selectTreeItems(wxTreeCtrl* tree, std::map<SComponent*, wxTreeItemId>& tree_items,
	SComponent* root, SComponent * node, bool select, bool removeSelect)
{
	wxTreeItemId nodeId = tree_items[node];

	int new_itemstate=-5;
	if (tree->GetItemState(nodeId) != wxTREE_ITEMSTATE_NONE)
	{
		if (select && removeSelect)
		{
			tree->SetItemState(nodeId, wxTREE_ITEMSTATE_NONE);
			new_itemstate = wxTREE_ITEMSTATE_NONE;
		}
		else
		{
			new_itemstate = select ? 1 : 0;
			tree->SetItemState(nodeId, new_itemstate);			
		}
	}
	else if (!select && !node->writer && node->selectable)
	{
		tree->SetItemState(nodeId, 0);
		new_itemstate = 0;
	}

	if (!node->writer)
	{
		for (size_t i = 0; i < root->children.size(); ++i)
		{
			if (root->children[i]->writerId == node->writerId)
			{
				bool has_selectable_child = false;
				if (!allChildrenSelected(tree, tree_items, root->children[i], has_selectable_child))
				{
					tree->SetItemState(tree_items[root->children[i]], 0);
				}

				if (!has_selectable_child)
				{
					tree->SetItemState(tree_items[root->children[i]], select ? 1 : 0);
				}
				break;
			}
		}
	}

	if (node != root)
	{
		bool has_selectable_child = false;
		if (!allChildrenSelected(tree, tree_items, root, has_selectable_child))
		{
			tree->SetItemState(tree_items[root], 0);
		}
	}

	for(size_t i=0;i<node->dependencies.size();++i)
	{
		selectTreeItems(tree, tree_items, root, node->dependencies[i], select);
	}

	for (size_t i = 0; i < node->children.size(); ++i)
	{
		bool curr_remove_select = select && !node->writer && node->selectable;
		selectTreeItems(tree, tree_items, root, node->children[i], select, curr_remove_select);
	}

	if (new_itemstate != -5)
	{
		tree->SetItemState(nodeId, new_itemstate);
	}
}

bool SelectWindowsComponents::hasSelectedChild(wxTreeCtrl* tree, std::map<SComponent*, wxTreeItemId>& tree_items,
	SComponent * node)
{
	for (size_t i = 0; i < node->children.size(); ++i)
	{
		SComponent* child = node->children[i];

		wxTreeItemId nodeId = tree_items[child];

		if (tree->GetItemState(nodeId) == 1)
		{
			return true;
		}

		if (hasSelectedChild(tree, tree_items, child))
		{
			return true;
		}
	}

	return false;
}

bool SelectWindowsComponents::allChildrenSelected(wxTreeCtrl * tree, std::map<SComponent*, wxTreeItemId>& tree_items,
	SComponent * node, bool& has_selectable_child)
{
	for (size_t i = 0; i < node->children.size(); ++i)
	{
		SComponent* child = node->children[i];

		wxTreeItemId nodeId = tree_items[child];

		if (tree->GetItemState(nodeId) != wxTREE_ITEMSTATE_NONE)
		{
			has_selectable_child = true;
		}

		if (tree->GetItemState(nodeId)!=1
			&& tree->GetItemState(nodeId)!= wxTREE_ITEMSTATE_NONE)
		{
			return false;
		}

		if (!allChildrenSelected(tree, tree_items, child, has_selectable_child))
		{
			return false;
		}
	}

	return true;
}

void SelectWindowsComponents::selectTreeItems(SComponent * node, bool select, bool removeSelect)
{
	selectTreeItems(m_treeCtrl1, tree_items, componentReader.getRoot(),
		node, select, removeSelect);
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
	root.is_root = true;
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

bool WindowsComponentReader::readComponents(const std::string& restoreXml, const std::vector<std::string>& componentXmls,
	const std::vector<SComponent>& filter_except, SComponent & root, std::vector<SComponent*>& components, std::string& errmsg)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if (hr != S_OK)
	{
		errmsg = "CoInitializeEx failed " + GetErrorHResErrStr(hr);
		return false;
	}

	SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssBackupComponents, backupcom);

	hr = CreateVssBackupComponents(&backupcom);

	if (hr != S_OK || backupcom == NULL)
	{
		errmsg = "Error creating backup components " + GetErrorHResErrStr(hr);
		return false;
	}

	if (!restoreXml.empty())
	{
		hr = backupcom->InitializeForRestore(const_cast<BSTR>(ConvertToUnicode(restoreXml).c_str()));

		if (hr != S_OK)
		{
			errmsg = "Error initializing components for restore " + GetErrorHResErrStr(hr);
			return false;
		}
	}
	else
	{
		hr = backupcom->InitializeForBackup();

		if (hr != S_OK)
		{
			errmsg = "Error initializing components for backup " + GetErrorHResErrStr(hr);
			return false;
		}
	}

	UINT nwriters;
	if (restoreXml.empty())
	{
		SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssAsync, pb_result);
		hr = backupcom->GatherWriterMetadata(&pb_result);

		if (hr != S_OK)
		{
			errmsg = "Error gathering writer metadata " + GetErrorHResErrStr(hr);
			return false;
		}

		if (!wait_for(pb_result, "Gathering writer metadata failed", errmsg))
		{
			return false;
		}

		
		hr = backupcom->GetWriterMetadataCount(&nwriters);

		if (hr != S_OK)
		{
			errmsg = "Error getting writer metadata count";
			return false;
		}
	}
	else
	{
		nwriters = static_cast<UINT>(componentXmls.size());
	}

	size_t maxDepth = 0;

	for (UINT i = 0; i < nwriters; ++i)
	{
		VSS_ID writerInstance;
		SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssExamineWriterMetadata, writerMetadata);

		HRESULT hr;
		if (restoreXml.empty())
		{
			hr = backupcom->GetWriterMetadata(i, &writerInstance, &writerMetadata);
		}
		else
		{
			hr = CreateVssExamineWriterMetadata(const_cast<BSTR>(ConvertToUnicode(componentXmls[i]).c_str()), &writerMetadata);
		}

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
			component->parent = &root;

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
			if (restoreXml.empty())
			{
				component->selectable = componentInfo->bSelectable == TRUE;
			}
			else
			{
				component->selectable = componentInfo->bSelectableForRestore == TRUE;
			}

			if (componentInfo->bstrCaption != NULL)
			{
				component->tooltip = ConvertFromWchar(componentInfo->bstrCaption);
			}

			if (componentInfo->cbIcon > 0)
			{
				int abc = 5;
			}

			bool filtered = true;
			if (filter_except.empty())
			{
				filtered = false;
			}
			else
			{
				for (size_t k = 0; k < filter_except.size(); ++k)
				{
					if (filter_except[k].name == component->name
						&& filter_except[k].writerId == component->writerId
						&& filter_except[k].logicalPath == component->logicalPath)
					{
						filtered = false;
						break;
					}
				}
			}

			if (!filtered)
			{
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

	}

	for (size_t depth = 0; depth < maxDepth; ++depth)
	{
		for (size_t i = 0; i < components.size(); ++i)
		{
			std::vector<std::string> toks;
			TokenizeMail(components[i]->logicalPath, toks, "\\");

			SComponent* parent = NULL;
			if (components[i]->writer
				&& depth == 0)
			{
				parent = &root;
			}
			else if (!components[i]->writer
				&& toks.size() + 1 == depth)
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
				components[i]->parent = parent;
				parent->children.push_back(components[i]);
			}
		}
	}


	for (UINT i = 0; i < nwriters; ++i)
	{
		VSS_ID writerInstance;
		SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssExamineWriterMetadata, writerMetadata);
		HRESULT hr;
		if (restoreXml.empty())
		{
			hr = backupcom->GetWriterMetadata(i, &writerInstance, &writerMetadata);
		}
		else
		{
			hr = CreateVssExamineWriterMetadata(const_cast<BSTR>(ConvertToUnicode(componentXmls[i]).c_str()), &writerMetadata);
		}

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

	return true;
}

wxThread::ExitCode WindowsComponentReader::Entry()
{
	readComponents(std::string(), std::vector<std::string>(), std::vector<SComponent>(),
		root, components, errmsg);

	return ExitCode();
}

std::string GetErrorHResErrStr(HRESULT res)
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

bool WindowsComponentReader::wait_for(IVssAsync * vsasync, const std::string & error_prefix, std::string& errmsg)
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
	if (node->is_root)
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
