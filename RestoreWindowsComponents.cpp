#include "RestoreWindowsComponents.h"
#include "checked.xpm"
#include "unchecked.xpm"
#include <wx/filename.h>
#include <wx/dir.h>
#include <random>
#include "stringtools.h"
#include "SelectWindowsComponents.h"
#include "json/json.h"

extern wxString res_path;
extern wxString ico_ext;
extern wxBitmapType ico_type;
extern std::string g_res_path;

RetrieveComponentBackups::RetrieveComponentBackups()
	: wxThread(wxTHREAD_JOINABLE), success(false)
{
}

RetrieveComponentBackups::~RetrieveComponentBackups()
{
}

wxThread::ExitCode RetrieveComponentBackups::Entry()
{
	filelist = Connector::getFileList("windows_components_config", access_error);

	if (Connector::hasError())
	{
		return NULL;
	}

	success = true;

	return NULL;
}

SelectRestoreComponents::SelectRestoreComponents(wxWindow * parent)
	: GUISelectRestoreComponents(parent), canSelectBackup(false), retrievingBackupConfig(false)
{
	retrieveComponentBackups.Run();

	Start(200);

	m_listBox1->Append(_("Loading..."));

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
}

void SelectRestoreComponents::Notify(void)
{
	if (!retrieveComponentBackups.IsAlive()
		&& !canSelectBackup)
	{
		retrieveComponentBackups.Wait();
		canSelectBackup = true;

		if (!retrieveComponentBackups.hasSuccess())
		{
			Stop();
			wxMessageBox(_("Error retrieving Windows component backups: ") + 
				accessErrorToString(retrieveComponentBackups.getAccessError()), wxT("UrBackup"), wxOK | wxICON_ERROR);
			Start(200);
			return;
		}

		m_listBox1->Clear();

		for (size_t i = 0; i < retrieveComponentBackups.getFileList().size(); ++i)
		{
			wxDateTime logtime_dt((wxLongLong)(retrieveComponentBackups.getFileList()[i].backuptime * 1000));

			m_listBox1->Append(logtime_dt.Format());
		}

		if (!retrieveComponentBackups.getFileList().empty())
		{
			m_listBox1->Select(0);
			wxCommandEvent evt;
			onListBoxSelected(evt);
		}
	}

	if (retrievingBackupConfig
		&& retrieveComponentConfig.get()
		&& !retrieveComponentConfig->IsAlive())
	{
		retrieveComponentConfig->Wait();
		retrievingBackupConfig = false;

		if (!retrieveComponentConfig->hasSuccess())
		{
			wxString errmsg = accessErrorToString(retrieveComponentConfig->getAccessError());
			if (!retrieveComponentConfig->getErrMsg().empty())
			{
				errmsg = retrieveComponentConfig->getErrMsg();
			}
			Stop();
			wxMessageBox(_("Error retrieving Windows component configuration: ") +
				errmsg, wxT("UrBackup"), wxOK | wxICON_ERROR);
			Start(200);
			retrieveComponentConfig.reset();
			return;
		}

		m_treeCtrl1->Delete(m_treeCtrl1->GetRootItem());
		wxTreeItemId rootId = m_treeCtrl1->AddRoot(_("Windows components"));

		m_treeCtrl1->SetItemState(rootId, 0);

		SComponent* root = retrieveComponentConfig->getRoot();

		tree_components[rootId] = root;
		tree_items[root] = rootId;

		SelectWindowsComponents::addComponents(m_treeCtrl1, iconList, rootId, root, tree_components,
			tree_items);

		m_treeCtrl1->SetImageList(iconList);

		SelectWindowsComponents::selectTreeItems(m_treeCtrl1, tree_items,
			retrieveComponentConfig->getRoot(), retrieveComponentConfig->getRoot(), true);

		for (std::map<SComponent*, wxTreeItemId>::iterator it = tree_items.begin();
			it != tree_items.end(); ++it)
		{
			m_treeCtrl1->Toggle(it->second);
		}
	}
}

void SelectRestoreComponents::onListBoxSelected(wxCommandEvent & event)
{
	if (!canSelectBackup)
	{
		return;
	}

	int sel = m_listBox1->GetSelection();
	if (sel >= 0)
	{
		while (retrievingBackupConfig
			&& retrieveComponentConfig.get()
			&& retrieveComponentConfig->IsAlive() )
		{
			Sleep(100);
			wxTheApp->SafeYield(NULL, false);
		}

		m_treeCtrl1->DeleteAllItems();
		tree_components.clear();
		tree_items.clear();
		m_treeCtrl1->SetImageList(NULL);
		iconList->RemoveAll();

		m_treeCtrl1->AddRoot(_("Loading..."));

		retrieveComponentConfig.reset(new RetrieveComponentConfig);
		retrieveComponentConfig->reset(retrieveComponentBackups.getFileList()[sel].backupid);

		retrieveComponentConfig->Run();
		retrievingBackupConfig = true;
	}
}

void SelectRestoreComponents::onStartRestore(wxCommandEvent & event)
{
}

void SelectRestoreComponents::evtOnTreeStateImageClick(wxTreeEvent & event)
{
	SComponent* component = tree_components[event.GetItem()];

	if (component == NULL
		|| retrieveComponentConfig.get()==NULL)
	{
		return;
	}

	bool select = m_treeCtrl1->GetItemState(event.GetItem()) == 0;

	SelectWindowsComponents::selectTreeItems(m_treeCtrl1, tree_items, 
		retrieveComponentConfig->getRoot(), component, select);
}

void SelectRestoreComponents::onCancel(wxCommandEvent & event)
{
	Close();
}

RetrieveComponentConfig::RetrieveComponentConfig()
	: wxThread(wxTHREAD_JOINABLE), success(false)
{
	root.is_root = true;
}

RetrieveComponentConfig::~RetrieveComponentConfig()
{
	for (size_t i = 0; i < components.size(); ++i)
	{
		delete components[i];
	}
}

namespace
{
	class ScopedDeleteDir
	{
	public:
		ScopedDeleteDir(wxString dir)
			: dir(dir) {}
		~ScopedDeleteDir() {
			wxFileName::Rmdir(dir, wxPATH_RMDIR_RECURSIVE);
		}
	private:
		wxString dir;
	};
}

wxThread::ExitCode RetrieveComponentConfig::Entry()
{
	wxMilliClock_t now = wxGetLocalTimeMillis();
	std::random_device rd;

	wxString randomDir = wxFileName::GetTempDir().Append(wxString::Format(wxT("\\urb_win_comps_%llu_%lu"), now, rd()));
	if (!wxFileName::Mkdir(randomDir))
	{
		return NULL;
	}

	ScopedDeleteDir deleteRandomDir(randomDir);

	SPathMap path_map;
	path_map.source = "C:\\windows_components_config";
	path_map.target = randomDir.ToStdString();

	std::vector<SPathMap> path_maps;
	path_maps.push_back(path_map);

	SStartRestore restore_info = Connector::startRestore("windows_components_config", backupid,
		path_maps, access_error, false, true, false);

	if (!restore_info.ok)
	{
		return NULL;
	}

	SConnection connection;
	bool restore_finished = false;
	while (!restore_finished)
	{
		Sleep(100);

		SStatusDetails status = Connector::getStatusDetails(&connection);

		if (status.ok)
		{
			for (size_t i = 0; i < status.finished_processes.size(); ++i)
			{
				if (status.finished_processes[i].id == restore_info.process_id)
				{
					restore_finished = true;
					break;
				}
			}
		}
		else
		{
			return NULL;
		}
	}

	std::string data = getFile((randomDir + wxT("\\backupcom.xml")).ToStdWstring());

	if (data.empty())
	{		
		return NULL;
	}

	std::string info = getFile((randomDir + wxT("\\info.json")).ToStdWstring());

	if (info.empty())
	{
		return NULL;
	}

	wxDir dir(randomDir);
	if (!dir.IsOpened())
	{
		return NULL;
	}
	
	std::vector<std::string> componentXmls;
	wxString filename;
	bool cont = dir.GetFirst(&filename);
	while (cont)
	{	
		if ( filename != "backupcom.xml"
			&& filename != "info.json" )
		{
			componentXmls.push_back(getFile((randomDir + wxT("\\") + filename).ToStdWstring()));
		}
		cont = dir.GetNext(&filename);
	}

	Json::Value info_root;
	Json::Reader reader;
	if (!reader.parse(info, info_root, false))
	{
		return NULL;
	};

	std::vector<SComponent> filter_except;
	try
	{
		Json::Value selected_components = info_root["selected_components"];

		for (Json::ArrayIndex i = 0; i < selected_components.size(); ++i)
		{
			Json::Value compInfo = selected_components[i];

			SComponent comp;
			HRESULT hr = IIDFromString(ConvertToUnicode(compInfo["writerId"].asString()).c_str(),
				&comp.writerId);

			comp.logicalPath = compInfo["logicalPath"].asString();
			comp.name = compInfo["componentName"].asString();

			filter_except.push_back(comp);
		}
	}
	catch (std::runtime_error&)
	{
		return NULL;
	}


	if (!WindowsComponentReader::readComponents(data, componentXmls, filter_except, root, components, errmsg))
	{
		return NULL;
	}

	success = true;

	return ExitCode();
}

wxString accessErrorToString(Connector::EAccessError access_error)
{
	switch (access_error)
	{
	case Connector::EAccessError_Ok:
		return _("Unknown access error");
	case Connector::EAccessError_NoServer:
		return _("Currently not connected to a backup server. No backup server found.");
	case Connector::EAccessError_NoTokens:
		return _("No file backup access tokens found. Did you run a file backup yet?");
	}
}
