#include "RestoreWindowsComponents.h"
#include "stringtools.h"
#include "Connector.h"
#include "SelectRestoreWindowsComponents.h"
#include "TranslationHelper.h"

namespace
{
	std::string sortHex(UINT i)
	{
		UINT bi = big_endian(i);
		std::string ret = bytesToHex(reinterpret_cast<unsigned char*>(&bi), sizeof(bi));
		strupper(&ret);
		return ret;
	}

	class ScopedServiceHandle
	{
	public:
		ScopedServiceHandle(SC_HANDLE& h)
			: h(h)
		{}

		~ScopedServiceHandle() {
			if (h != NULL) {
				CloseServiceHandle(h);
			}
		}
	private:
		SC_HANDLE& h;
	};

#define SCOPED_DECLARE_RELEASE_SERVICE_HANDLE(x) SC_HANDLE x = NULL; ScopedServiceHandle TOKENPASTE(ScopedServiceHandle_, __LINE__) (x)
}

RestoreWindowsComponentsThread::RestoreWindowsComponentsThread(int backupid, std::vector<SComponent> selected_components, wxString componentConfigDir)
	: wxThread(wxTHREAD_JOINABLE), backupid(backupid), selected_components(selected_components), componentConfigDir(componentConfigDir), progress_bar_pc(-1)
{
}

RestoreWindowsComponentsThread::~RestoreWindowsComponentsThread()
{
}

void RestoreWindowsComponentsThread::log(const std::string & message)
{
	std::string fullmsg = wxDateTime::Now().Format().ToStdString() + ": " + message;
	wxCriticalSectionLocker lock(log_section);
	log_messages.push_back(fullmsg);
}

void RestoreWindowsComponentsThread::log(const wxString & message)
{
	wxString fullmsg = wxDateTime::Now().Format() + ": " + message;
	wxCriticalSectionLocker lock(log_section);
	log_messages.push_back(fullmsg.ToStdString());
}

std::vector<std::string> RestoreWindowsComponentsThread::getNewLogMessages()
{
	std::vector<std::string> ret;
	wxCriticalSectionLocker lock(log_section);
	ret = log_messages;
	log_messages.clear();
	return ret;
}

wxString RestoreWindowsComponentsThread::getMessage1()
{
	wxCriticalSectionLocker lock(log_section);
	return message1;
}

void RestoreWindowsComponentsThread::setMessage1(wxString msg)
{
	wxCriticalSectionLocker lock(log_section);
	message1 = msg;
}

void RestoreWindowsComponentsThread::setMessage2(wxString msg)
{
	wxCriticalSectionLocker lock(log_section);
	message2 = msg;
}

int RestoreWindowsComponentsThread::getProgressBarPc()
{
	wxCriticalSectionLocker lock(log_section);
	return progress_bar_pc;
}

void RestoreWindowsComponentsThread::setProgressBarPc(int pc)
{
	wxCriticalSectionLocker lock(log_section);
	progress_bar_pc = pc;
}

wxString RestoreWindowsComponentsThread::getMessage2()
{
	wxCriticalSectionLocker lock(log_section);
	return message2;
}

wxThread::ExitCode RestoreWindowsComponentsThread::Entry()
{
	setMessage1(_("Preparing restore"));
	log(_("Starting restore operation..."));

	for (size_t i = 0; i < selected_components.size(); ++i)
	{
		selected_components[i].is_root = false;
	}

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if (hr != S_OK)
	{
		log("CoInitializeEx failed " + GetErrorHResErrStr(hr));
		return NULL;
	}

	SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssBackupComponents, backupcom);

	hr = CreateVssBackupComponents(&backupcom);

	if (hr != S_OK || backupcom == NULL)
	{
		log("Error creating backup components " + GetErrorHResErrStr(hr));
		return NULL;
	}

	std::string restoreXml = getFile((componentConfigDir + "\\backupcom.xml").ToStdWstring());

	hr = backupcom->InitializeForRestore(const_cast<BSTR>(ConvertToUnicode(restoreXml).c_str()));

	if (hr != S_OK)
	{
		log("Error initializing components for restore " + GetErrorHResErrStr(hr));
		return false;
	}


	SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssAsync, pb_result);
	hr = backupcom->GatherWriterMetadata(&pb_result);

	if (hr != S_OK)
	{
		log("Error gathering writer metadata " + GetErrorHResErrStr(hr));
		return false;
	}

	log("Collecting information about running writers...");

	std::string werrmsg;
	if (!WindowsComponentReader::wait_for(pb_result, "Gathering writer metadata failed", werrmsg))
	{
		log(werrmsg);
		return false;
	}

	UINT nwriters;
	hr = backupcom->GetWriterMetadataCount(&nwriters);

	if (hr != S_OK)
	{
		log(std::string("Error getting writer metadata count"));
		return false;
	}

	log("Selecting components to restore...");

	std::vector<std::string> post_restart_services;
	std::vector<std::string> stop_services;

	for (UINT i = 0; i < nwriters; ++i)
	{
		VSS_ID writerInstance;
		SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssExamineWriterMetadata, writerMetadata);

		hr = backupcom->GetWriterMetadata(i, &writerInstance, &writerMetadata);

		if (hr != S_OK)
		{
			log("Getting metadata of writer " + convert(i) + " failed. VSS error code " + GetErrorHResErrStr(hr));
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
			log("Getting identity of writer " + convert(i) + " failed. VSS error code " + GetErrorHResErrStr(hr));
			continue;
		}

		bool writer_selected = false;
		for (size_t j = 0; j < selected_components.size(); ++j)
		{
			if (selected_components[j].writerId == writerId)
			{
				writer_selected = true;
				break;
			}
		}

		if (!writer_selected)
			continue;

		//Always skip System writer and ASR writer
		if (convert(writerId) == "{E8132975-6F93-4464-A53E-1050253AE220}"
			|| convert(writerId) == "{BE000CBE-11FE-4426-9C58-531AA6355FC4}")
		{
			continue;
		}

		std::string writerData = getFile((componentConfigDir + "\\" + convert(writerId) + ".xml").ToStdWstring());

		if (writerData.empty())
		{
			log("Cannot read writer config xml " + convert(writerId) + ".xml");
			continue;
		}

		SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssExamineWriterMetadata, writerMetadataFromBackup);
		hr = CreateVssExamineWriterMetadata(const_cast<BSTR>(ConvertToUnicode(writerData).c_str()), &writerMetadataFromBackup);
		if (hr != S_OK)
		{
			log("Getting metadata of writer " + convert(i) + " from XML failed. VSS error code " + GetErrorHResErrStr(hr));
			continue;
		}


		std::string writerNameStr = ConvertFromWchar(writerName);

		UINT nIncludeFiles, nExcludeFiles, nComponents;
		hr = writerMetadataFromBackup->GetFileCounts(&nIncludeFiles, &nExcludeFiles, &nComponents);
		if (hr != S_OK)
		{
			log("GetFileCounts of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr));
			continue;
		}

		UINT nCurrIncludeFiles, nCurrExcludeFiles, nCurrComponents;
		hr = writerMetadata->GetFileCounts(&nCurrIncludeFiles, &nCurrExcludeFiles, &nCurrComponents);
		if (hr != S_OK)
		{
			log("GetFileCounts of writer \"" + writerNameStr + "\" failed -2. VSS error code " + GetErrorHResErrStr(hr));
			continue;
		}

		VSS_RESTOREMETHOD_ENUM method;
		SCOPED_DECLARE_FREE_BSTR(service);
		SCOPED_DECLARE_FREE_BSTR(userProcedure);
		VSS_WRITERRESTORE_ENUM writerRestore;
		bool rebootRequired = false;
		UINT nMappings;
		hr = writerMetadataFromBackup->GetRestoreMethod(&method, &service, &userProcedure, &writerRestore, &rebootRequired, &nMappings);
		if (hr != S_OK)
		{
			log("GetRestoreMethod of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr));
			continue;
		}

		UINT nWriterComponents;
		hr = backupcom->GetWriterComponentsCount(&nWriterComponents);

		if (hr != S_OK)
		{
			log("Error getting writer components failed. VSS error code " + GetErrorHResErrStr(hr));
			continue;
		}

		SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssWriterComponentsExt, writerComponents);
		for (UINT j = 0; j < nWriterComponents; ++j)
		{
			SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssWriterComponentsExt, writerComponentsLoc);
			hr = backupcom->GetWriterComponents(j, &writerComponentsLoc);
			if (hr != S_OK)
			{
				log("Error getting writer component " + convert(j) + " for writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr));
				continue;
			}

			VSS_ID writerCompInstance;
			VSS_ID writerCompId;
			hr = writerComponentsLoc->GetWriterInfo(&writerCompInstance, &writerCompId);

			if (hr != S_OK)
			{
				log("Error getting writer component info " + convert(j) + " for writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr));
				continue;
			}

			if (writerCompId == writerId)
			{
				writerComponents = writerComponentsLoc;
				writerComponentsLoc = NULL;
				break;
			}
		}

		std::vector<SFileSpec> redirected_locations;
		for (UINT j = 0; j < nMappings; ++j)
		{
			SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssWMFiledesc, wmFile);
			hr = writerMetadataFromBackup->GetAlternateLocationMapping(j, &wmFile);
			if (hr != S_OK)
			{
				log("Error getting alternate location mapping " + convert(j) + " of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr));
				continue;
			}

			SFileSpec spec;
			if (!getFilespec(wmFile, spec))
			{
				continue;
			}

			redirected_locations.push_back(spec);
		}

		for (UINT j = 0; j < nComponents; ++j)
		{
			SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssWMComponent, wmComponent);
			hr = writerMetadataFromBackup->GetComponent(j, &wmComponent);
			if (hr != S_OK)
			{
				log("Error getting component " + convert(j) + " of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr));
				continue;
			}

			SCOPED_DECLARE_FREE_COMPONENTINFO(wmComponent, componentInfo);
			hr = wmComponent->GetComponentInfo(&componentInfo);
			if (hr != S_OK)
			{
				log("Error getting component info " + convert(j) + " of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr));
				continue;
			}

			std::string componentNameStr = ConvertFromWchar(componentInfo->bstrComponentName);

			std::string logicalPathStr;
			if (componentInfo->bstrLogicalPath != NULL)
			{
				logicalPathStr = ConvertFromWchar(componentInfo->bstrLogicalPath);
			}

			bool restoreComponent = false;
			for (size_t k = 0; k < selected_components.size(); ++k)
			{
				if (selected_components[k].writerId == writerId
					&& selected_components[k].logicalPath == logicalPathStr
					&& selected_components[k].name == componentNameStr)
				{
					restoreComponent = true;
					selected_components[k].is_root = true;
					break;
				}
			}

			if (restoreComponent)
			{
				UINT writerComponentsCount;
				hr = writerComponents->GetComponentCount(&writerComponentsCount);
				if (hr != S_OK)
				{
					log("Error writer component count " + convert(j) + " of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr));
					continue;
				}

				bool hasCurrComponent = false;
				for (UINT k = 0; k < writerComponentsCount; ++k)
				{
					SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssComponent, vssComponent);
					hr = writerComponents->GetComponent(k, &vssComponent);
					if (hr != S_OK)
					{
						log("Error getting component " + convert(k) + " of writer \"" + writerNameStr + "\" failed -2. VSS error code " + GetErrorHResErrStr(hr));
						continue;
					}

					SCOPED_DECLARE_FREE_BSTR(bstrComponentName);
					hr = vssComponent->GetComponentName(&bstrComponentName);
					if (hr != S_OK)
					{
						log("Error getting component name " + convert(k) + " of writer \"" + writerNameStr + "\" failed -2. VSS error code " + GetErrorHResErrStr(hr));
						continue;
					}

					std::string currComponentNameStr = ConvertFromWchar(bstrComponentName);

					SCOPED_DECLARE_FREE_BSTR(bstrLogicalPath);
					vssComponent->GetLogicalPath(&bstrLogicalPath);

					std::string currLogicalPathStr;
					if (bstrLogicalPath != NULL)
					{
						currLogicalPathStr = ConvertFromWchar(bstrLogicalPath);
					}

					if (currComponentNameStr == componentNameStr
						&& currLogicalPathStr == logicalPathStr)
					{
						VSS_RESTORE_TARGET target;
						hr = vssComponent->GetRestoreTarget(&target);

						if (hr != S_OK)
						{
							log("Error getting restore target of component \"" + componentNameStr + "\" of writer \"" + writerNameStr + "\" failed -2. VSS error code " + GetErrorHResErrStr(hr));
							continue;
						}

						if (target != VSS_RT_ORIGINAL)
						{
							log("Alternate restore target is used. Not implemented.");
							return false;
						}

						hasCurrComponent = true;
						break;
					}
				}

				hr = backupcom->SetSelectedForRestore(writerId, componentInfo->type,
					componentInfo->bstrLogicalPath, componentInfo->bstrComponentName, true);

				if (hr != S_OK)
				{
					log("Error selecting component \"" + componentNameStr + "\" with logical path \"" + logicalPathStr + "\" of writer \"" + writerNameStr + "\" for restore");
					return false;
				}

				SRestoreComponent comp;
				comp.componentIdx = j;
				comp.componentName = componentNameStr;
				comp.redirected_locations = redirected_locations;
				comp.writerId = writerId;
				comp.writerName = writerNameStr;
				comp.logicalPath = logicalPathStr;
				comp.type = componentInfo->type;

				if (method == VSS_RME_RESTORE_IF_NOT_THERE)
				{
					comp.restoreFlags = Connector::restore_flag_open_all_files_first | Connector::restore_flag_no_overwrite | Connector::restore_flag_mapping_is_alternative;
				}
				else if (method == VSS_RME_RESTORE_IF_CAN_REPLACE)
				{
					comp.restoreFlags = Connector::restore_flag_open_all_files_first | Connector::restore_flag_no_reboot_overwrite;
				}
				else if (method == VSS_RME_STOP_RESTORE_START
					|| method == VSS_RME_RESTORE_TO_ALTERNATE_LOCATION
					|| method == VSS_RME_RESTORE_STOP_START )
				{
					if (method == VSS_RME_STOP_RESTORE_START
						&& service!=NULL)
					{
						stop_services.push_back(ConvertFromWchar(service));
					}
					else if (method == VSS_RME_RESTORE_STOP_START
						&& service!=NULL)
					{
						post_restart_services.push_back(ConvertFromWchar(service));
					}
					comp.restoreFlags = Connector::restore_flag_no_reboot_overwrite;
				}
				else if (method == VSS_RME_RESTORE_AT_REBOOT)
				{
					comp.restoreFlags = Connector::restore_flag_reboot_overwrite_all;
				}
				else if (method == VSS_RME_RESTORE_AT_REBOOT_IF_CANNOT_REPLACE)
				{
					comp.restoreFlags = 0;
				}
				else
				{
					log("Unhandled restore method "+convert(method)+" of component \"" + componentNameStr + "\" with logical path \"" + logicalPathStr + "\" of writer \"" + writerNameStr + "\".");
					return false;
				}

				for (UINT k = 0; k < componentInfo->cFileCount; ++k)
				{
					SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssWMFiledesc, wmFile);
					hr = wmComponent->GetFile(k, &wmFile);

					if (hr != S_OK)
					{
						log("Error getting component file desc " + convert(k) + " of component \"" +
							componentNameStr + "\" of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr));
						return false;
					}

					if (!getFilespec(wmFile, comp.currspec))
					{
						return false;
					}

					comp.filesIdx = k;
					comp.filesPrefix = "files";

					restore_components.push_back(comp);
				}

				for (UINT k = 0; k < componentInfo->cDatabases; ++k)
				{
					SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssWMFiledesc, wmFile);
					hr = wmComponent->GetDatabaseFile(k, &wmFile);

					if (hr != S_OK)
					{
						log("Error getting component database file desc " + convert(k) + " of component \"" +
							componentNameStr + "\" of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr));
						return false;
					}

					if (!getFilespec(wmFile, comp.currspec))
					{
						return false;
					}

					comp.filesIdx = k;
					comp.filesPrefix = "database";

					restore_components.push_back(comp);
				}

				for (UINT k = 0; k < componentInfo->cLogFiles; ++k)
				{
					SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssWMFiledesc, wmFile);
					hr = wmComponent->GetDatabaseLogFile(k, &wmFile);

					if (hr != S_OK)
					{
						log("Error getting component database log file desc " + convert(k) + " of component \"" +
							componentNameStr + "\" of writer \"" + writerNameStr + "\" failed. VSS error code " + GetErrorHResErrStr(hr));
						return false;
					}

					if (!getFilespec(wmFile, comp.currspec))
					{
						return false;
					}

					comp.filesIdx = k;
					comp.filesPrefix = "database_log";

					restore_components.push_back(comp);
				}
			}
		}
	}

	if (restore_components.empty())
	{
		log("No components selected. Did not restore anything.");
		return false;
	}

	setMessage1(_("Restore operation in progress"));
	log("Starting restore operation (pre restore)...");

	SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssAsync, prepare_wait);
	hr = backupcom->PreRestore(&prepare_wait);

	if (hr != S_OK)
	{
		log("Error during pre restore. VSS error code " + GetErrorHResErrStr(hr));
		return false;
	}

	if (!WindowsComponentReader::wait_for(prepare_wait, "Pre restore failed", werrmsg))
	{
		log(werrmsg);
		return false;
	}

	for (size_t i = 0; i < stop_services.size(); ++i)
	{
		log("Stopping service \"" + stop_services[i] + "\"...");
		if (!stopService(stop_services[i]))
		{
			log("Failed to stop service \"" + stop_services[i] + "\"");
			return false;
		}
	}

	bool has_restore_err = false;

	for (size_t i = 0; i < restore_components.size(); ++i)
	{
		SRestoreComponent& comp = restore_components[i];
		if (!restoreFiles(comp))
		{
			has_restore_err = true;
		}

		bool more_files = false;
		for (size_t j = i + 1; j < restore_components.size(); ++j)
		{
			if (restore_components[j].writerId == comp.writerId
				&& restore_components[j].componentName == comp.componentName
				&& restore_components[j].logicalPath == comp.logicalPath)
			{
				more_files = true;
				break;
			}
		}

		if (!more_files)
		{
			if (has_restore_err)
			{
				log("Restore of component \"" + comp.componentName +
					"\" of writer \"" + comp.writerName + "\" failed.");
			}
			else
			{
				log("Restore of component \"" + comp.componentName +
					"\" of writer \"" + comp.writerName + "\" finished.");
			}

			hr = backupcom->SetFileRestoreStatus(comp.writerId,
				comp.type, comp.logicalPath.empty() ? NULL : ConvertToUnicode(comp.logicalPath).c_str(),
				ConvertToUnicode(comp.componentName).c_str(),
				has_restore_err ? VSS_RS_FAILED : VSS_RS_ALL);
			has_restore_err = false;

			if (hr != S_OK)
			{
				log("Error setting restore status of component \"" + comp.componentName +
					"\" of writer \"" + comp.writerName + "\". VSS error code " + GetErrorHResErrStr(hr));
				return false;
			}
		}
	}

	for (size_t i = 0; i < stop_services.size(); ++i)
	{
		log("Starting service \"" + stop_services[i] + "\"...");
		if (!startService(stop_services[i]))
		{
			log("Failed to start service \"" + stop_services[i] + "\". Maybe start it manually?");
		}
	}

	log("Finalizing restore operation (post restore)...");

	SCOPED_DECLARE_RELEASE_IUNKNOWN(IVssAsync, post_wait);
	hr = backupcom->PostRestore(&post_wait);

	if (hr != S_OK)
	{
		log("Error during post restore. VSS error code " + GetErrorHResErrStr(hr));
		return false;
	}

	if (!WindowsComponentReader::wait_for(post_wait, "Post restore failed", werrmsg))
	{
		log(werrmsg);
		return false;
	}

	for (size_t i = 0; i < post_restart_services.size(); ++i)
	{
		log("Restarting service \"" + post_restart_services[i] + "\"...");

		if (stopService(post_restart_services[i]))
		{
			if (!startService(post_restart_services[i]))
			{
				log("Failed to start service \"" + post_restart_services[i] + "\" after stopping it. Maybe start it manually?");
			}
		}
		else
		{
			log("Failed to stop service \"" + post_restart_services[i] + "\"");
			return false;
		}
	}

	setMessage1(_("Restore finished"));
	log("Restore finished.");

	return ExitCode();
}

bool RestoreWindowsComponentsThread::restoreFiles(const SRestoreComponent& comp)
{
	std::string path = ".symlink_"+conv_filename(comp.writerName) + "_" +
		convert(comp.writerId) + "_" + sortHex(comp.componentIdx) + "_"+conv_filename(comp.componentName) + "_" + comp.filesPrefix + sortHex(comp.filesIdx);

	std::vector<SPathMap> map_paths;

	std::string target = comp.currspec.path;

	std::vector<SFileSpec>::const_iterator it_alt = std::find(comp.redirected_locations.begin(),
		comp.redirected_locations.end(), comp.currspec);
	if (it_alt != comp.redirected_locations.end())
	{
		SPathMap m;
		m.source = comp.currspec.path;
		m.target = it_alt->altPath;

		target = it_alt->altPath;

		map_paths.push_back(m);
	}

	int pcdone = -1;

	setMessage2(trans_2(_("Restoring component \"_1_\" of writer \"_2_\". Indexing..."), wxString(comp.componentName), wxString(comp.writerName)) );
	
	Connector::EAccessError access_error;
	SStartRestore restore = Connector::startRestore(path, backupid, map_paths, access_error, false, true, false, comp.restoreFlags);

	if (!restore.ok)
	{
		log("Restoring files " + convert(comp.filesIdx) + " of component \"" + comp.componentName + "\" of writer \"" + comp.writerName + "\" failed. "
			+ accessErrorToString(access_error));
		return false;
	}

	log("Restoring file set " + convert(comp.filesIdx) + " of of component \"" + comp.componentName + "\" of writer \"" + comp.writerName + "\" to \"" + target + "\"...");

	SConnection connection;
	bool restore_finished = false;
	std::string restore_details;
	int detail_pc = -1;
	bool status_error = false;
	while (!restore_finished)
	{
		SStatusDetails status = Connector::getStatusDetails(&connection);

		if (status.ok)
		{
			status_error = false;

			for (size_t i = 0; i < status.finished_processes.size(); ++i)
			{
				if (status.finished_processes[i].id == restore.process_id)
				{
					restore_finished = true;

					if (!status.finished_processes[i].success)
					{
						setMessage2("");
						return false;
					}
				}
			}

			for (size_t i = 0; i < status.running_processes.size(); ++i)
			{
				SRunningProcess& proc = status.running_processes[i];
				if (proc.process_id == restore.process_id)
				{
					if (!proc.details.empty()
						&& (restore_details != proc.details
							|| detail_pc != proc.detail_pc))
					{
						restore_details = proc.details;
						detail_pc = proc.detail_pc;

						log("Restoring \"" + restore_details + "\"... " + convert(detail_pc) + "%");
					}

					if (getProgressBarPc() != proc.percent_done)
					{
						setProgressBarPc(proc.percent_done);
						setMessage2(trans_3(_("Restoring component \"_1_\" of writer \"_2_\". _3_% finished."),
							wxString(comp.componentName), wxString(comp.writerName), convert(proc.percent_done)));
					}

					break;
				}
			}
		}
		else
		{
			if (!status_error)
			{
				log(std::string("Error getting restore status."));
			}
			status_error = true;
		}

		Sleep(1000);
	}

	setMessage2("");

	return true;
}

bool RestoreWindowsComponentsThread::getFilespec(IVssWMFiledesc * wmFile, SFileSpec & filespec)
{
	SCOPED_DECLARE_FREE_BSTR(path);
	HRESULT hr = wmFile->GetPath(&path);

	if (hr != S_OK)
	{
		log("Getting path of file spec failed. VSS error code " + GetErrorHResErrStr(hr));
		return false;
	}

	SCOPED_DECLARE_FREE_BSTR(spec);
	hr = wmFile->GetFilespec(&spec);

	if (hr != S_OK)
	{
		log("Getting spec of file spec failed. VSS error code " + GetErrorHResErrStr(hr));
		return false;
	}

	SCOPED_DECLARE_FREE_BSTR(altPath);
	wmFile->GetAlternateLocation(&altPath);

	hr = wmFile->GetRecursive(&filespec.recursive);

	if (hr != S_OK)
	{
		log("Getting recursive flag of file spec failed. VSS error code " + GetErrorHResErrStr(hr));
		return false;
	}

	filespec.path = ConvertFromWchar(path);
	filespec.spec = ConvertFromWchar(spec);
	if (altPath != NULL)
	{
		filespec.altPath = ConvertFromWchar(altPath);
	}

	return true;
}

bool RestoreWindowsComponentsThread::readAltLocations(std::vector<SFileSpec> alt_locations)
{
	return false;
}

RestoreWindowsComponents::RestoreWindowsComponents(wxWindow * parent, int backupid, std::vector<SComponent> selected_components, wxString componentConfigDir)
	: GUIRestoreComponents(parent), restoreComponentsThread(backupid, selected_components, componentConfigDir)
{
	restoreComponentsThread.Run();

	Start(200);
}

void RestoreWindowsComponents::Notify(void)
{
	wxString msg1 = restoreComponentsThread.getMessage1();
	if (m_staticText1->GetLabel() != msg1)
	{
		m_staticText1->SetLabel(msg1);
	}
	wxString msg2 = restoreComponentsThread.getMessage2();
	if (m_staticText2->GetLabel() != msg2)
	{
		m_staticText2->SetLabel(msg2);
	}

	std::vector<std::string> msgs = restoreComponentsThread.getNewLogMessages();

	for (size_t i = 0; i < msgs.size(); ++i)
	{
		m_textCtrl1->AppendText(wxString(msgs[i]) + "\n");
	}

	int pc = restoreComponentsThread.getProgressBarPc();

	if (pc == -1)
	{
		m_gauge1->Disable();
	}
	else
	{
		m_gauge1->Enable();
		if (m_gauge1->GetRange() != 100)
		{
			m_gauge1->SetRange(100);
		}
		if (m_gauge1->GetValue() != pc)
		{
			m_gauge1->SetValue(pc);
		}
	}

	if (!restoreComponentsThread.IsAlive())
	{
		Stop();

		m_button3->Enable();
	}
}

void RestoreWindowsComponents::onOkClick(wxCommandEvent & event)
{
	Close();
}

bool RestoreWindowsComponentsThread::stopService(const std::string& service_name)
{
	ULONGLONG startTime = GetTickCount64();

	SCOPED_DECLARE_RELEASE_SERVICE_HANDLE(hsc);
	hsc = OpenSCManagerW(
		NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (NULL == hsc)
	{
		log("Could not open service manager");
		return false;
	}
	SCOPED_DECLARE_RELEASE_SERVICE_HANDLE(service);
	service = OpenServiceW(
		hsc, ConvertToUnicode(service_name).c_str(), 
		SERVICE_STOP |	SERVICE_QUERY_STATUS);

	if (service == NULL)
	{
		log("Could not open handle to service.");
		return false;
	}

	SERVICE_STATUS_PROCESS ssp;
	DWORD bytesNeeded;
	if (!QueryServiceStatusEx(service,
		SC_STATUS_PROCESS_INFO,	(LPBYTE)&ssp,
		sizeof(SERVICE_STATUS_PROCESS),	&bytesNeeded))
	{
		log("QueryServiceStatusEx failed");
		return false;
	}

	if (ssp.dwCurrentState == SERVICE_STOPPED)
	{
		return true;
	}

	while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
	{
		DWORD dwWaitTime = ssp.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		if (!QueryServiceStatusEx(
			service,
			SC_STATUS_PROCESS_INFO,	(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),	&bytesNeeded))
		{
			log("QueryServiceStatusEx failed");
			return false;
		}

		if (ssp.dwCurrentState == SERVICE_STOPPED)
		{
			return true;
		}

		if (GetTickCount64() - startTime > 30000)
		{
			log("Timeout while waiting for service to stop. Stopping service failed.");
			return false;
		}
	}

	if (!ControlService(
		service, SERVICE_CONTROL_STOP,	(LPSERVICE_STATUS)&ssp))
	{
		log("ControlService failed");
		return false;
	}

	while (ssp.dwCurrentState != SERVICE_STOPPED)
	{
		DWORD dwWaitTime = ssp.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		if (!QueryServiceStatusEx(
			service, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&bytesNeeded))
		{
			log("QueryServiceStatusEx failed");
			return false;
		}

		if (ssp.dwCurrentState == SERVICE_STOPPED)
		{
			return true;
		}

		if (GetTickCount64() - startTime > 30000)
		{
			log("Timeout while waiting for service to stop. Stopping service failed.");
			return false;
		}
	}

	return true;
}

bool RestoreWindowsComponentsThread::startService(const std::string & service_name)
{
	ULONGLONG startTime = GetTickCount64();

	SCOPED_DECLARE_RELEASE_SERVICE_HANDLE(hsc);
	hsc = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (NULL == hsc)
	{
		log("Could not open service manager");
		return false;
	}
	SCOPED_DECLARE_RELEASE_SERVICE_HANDLE(service);
	service = OpenServiceW(	hsc, ConvertToUnicode(service_name).c_str(),
		SERVICE_START | SERVICE_QUERY_STATUS);

	if (service == NULL)
	{
		log("Could not open handle to service.");
		return false;
	}

	SERVICE_STATUS_PROCESS ssp;
	DWORD bytesNeeded;
	if (!QueryServiceStatusEx(service,
		SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp,
		sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded))
	{
		log("QueryServiceStatusEx failed");
		return false;
	}

	if (ssp.dwCurrentState == SERVICE_RUNNING
		|| ssp.dwCurrentState == SERVICE_START_PENDING)
	{
		return true;
	}

	if (!StartServiceW(
		service, 0, NULL))
	{
		log("StartService failed");
		return false;
	}

	return true;
}
