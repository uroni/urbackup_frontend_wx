/*************************************************************************
*    UrBackup - Client/Server backup system
*    Copyright (C) 2011-2015 Martin Raiber
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Affero General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include "SetupWizard.h"
#include <vector>
#include <iostream>
#include "stringtools.h"

#ifdef _WIN32
#include <Windows.h>
#include <LM.h>
#include <Sddl.h>
#include <UserEnv.h>
#include <Shlobj.h>

#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "Userenv.lib")
#endif

#include "Connector.h"
#include "ConfigPath.h"
#include "FileSettingsReader.h"


extern std::string ConvertToUTF8(const std::wstring &input);
extern wxString res_path;
extern wxString ico_ext;
extern wxBitmapType ico_type;


SVolumesCache* SetupWizard::cache = NULL;

namespace
{
#ifdef _WIN32
	std::vector<std::wstring> get_users()
	{
		LPUSER_INFO_0 buf;
		DWORD prefmaxlen = MAX_PREFERRED_LENGTH;
		DWORD entriesread = 0;
		DWORD totalentries = 0;
		DWORD resume_handle = 0;
		NET_API_STATUS status;
		std::vector<std::wstring> ret;
		do 
		{
			status = NetUserEnum(NULL, 0, FILTER_NORMAL_ACCOUNT,
				(LPBYTE*)&buf, prefmaxlen, &entriesread,
				&totalentries, &resume_handle);

			if (status == NERR_Success || status == ERROR_MORE_DATA)
			{
				for(DWORD i=0;i<entriesread;++i)
				{
					LPUSER_INFO_0 user_info = (buf+i);
					ret.push_back(user_info->usri0_name);
				}
			}
			else
			{
				std::cerr << "Error while enumerating users: "+ nconvert((int)status) << std::endl;
			}

			if(buf!=NULL)
			{
				NetApiBufferFree(buf);
			}
		}
		while (status == ERROR_MORE_DATA);

		return ret;
	}

	std::vector<std::wstring> getPathsNotToBackup()
	{
		HKEY FilesNotToBackup;
		if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\BackupRestore\\FilesNotToBackup",
			0, KEY_ENUMERATE_SUB_KEYS|KEY_READ, &FilesNotToBackup)!=ERROR_SUCCESS)
		{
			std::cerr << "Error opening registry key SYSTEM\\CurrentControlSet\\Control\\BackupRestore\\FilesNotToBackup" << std::endl;
			return std::vector<std::wstring>();
		}

		DWORD maxClassLen;
		DWORD numValues;
		DWORD maxValueKeyLen;
		DWORD maxValueLen;
		if(RegQueryInfoKey(FilesNotToBackup, NULL, NULL, NULL, NULL, NULL, &maxClassLen, &numValues,
			&maxValueKeyLen, &maxValueLen, NULL, NULL)!=ERROR_SUCCESS)
		{
			std::cerr << "Error getting RegQueryInfoKey" << std::endl;
			return std::vector<std::wstring>();
		}

		maxValueKeyLen+=sizeof(wchar_t);

		std::wstring keyName;
		keyName.resize(maxValueKeyLen);

		std::vector<std::wstring> ret;
		for(DWORD idx=0;idx<numValues;++idx)
		{
			std::fill(keyName.begin(), keyName.end(), 0);

			DWORD keyNameLen = maxValueKeyLen;
			DWORD classNameLen = maxClassLen;
			DWORD type;
			DWORD dataLen;
			std::wstring data;
			LSTATUS rc = RegEnumValueW(FilesNotToBackup, idx, &keyName[0], &keyNameLen,
				NULL, &type, NULL, &dataLen);

			if(rc==ERROR_MORE_DATA || rc==ERROR_SUCCESS)
			{
				keyNameLen=maxValueKeyLen;
				data.resize(dataLen/sizeof(wchar_t) + (dataLen%sizeof(wchar_t)!=0 ? 1 : 0) );
				rc = RegEnumValueW(FilesNotToBackup, idx, &keyName[0], &keyNameLen,
					NULL, &type, reinterpret_cast<byte*>(&data[0]), &dataLen);
			}

			if(rc!=ERROR_SUCCESS)
			{
				std::cerr << "RegEnumValueW has error" << std::endl;
				return ret;
			}

			if(type==REG_EXPAND_SZ || type==REG_SZ)
			{
				ret.push_back(data);
			}
			else if(type==REG_MULTI_SZ)
			{
				Tokenize(data, ret, L"\0");
			}
		}

		return ret;
	}

	std::vector<std::wstring> resolvePaths(const std::vector<std::wstring>& paths)
	{
		std::vector<std::wstring> ret;
		std::wstring out;
		out.resize(32767);
		for(size_t i=0;i<paths.size();++i)
		{
			std::fill(out.begin(), out.end(), 0);
			//Found no method to get the logon token of the other users without having their password
			//zwCreateToken does not work for example
			if(ExpandEnvironmentStrings(paths[i].c_str(), &out[0], out.size()))
			{
				ret.push_back(out.c_str());
			}
			else
			{
				std::cerr << "Error expanding " << paths[i] << std::endl;
			}
		}

		return ret;
	}

	BOOL DirectoryExists(LPCTSTR szPath)
	{
		DWORD dwAttrib = GetFileAttributes(szPath);

		return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
			(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	}

	std::vector<std::wstring> fixupPaths(std::vector<std::wstring> paths)
	{
		std::wstring userprofile_dir;
		HANDLE hUserToken;
		if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hUserToken))
		{
			DWORD dwSize = 0;
			GetUserProfileDirectoryW(hUserToken, NULL, &dwSize);

			if(dwSize>0)
			{
				userprofile_dir.resize(dwSize);

				if(GetUserProfileDirectoryW(hUserToken, &userprofile_dir[0], &dwSize) && dwSize>0)
				{
					userprofile_dir.resize(dwSize-1);
				}
			}

			CloseHandle(hUserToken);
		}

		std::vector<std::wstring> users = get_users();

		for(size_t i=0, size=paths.size();i<size;++i)
		{
			if(!paths.empty() && paths[i][0]==L'\\')
			{
				paths[i] = L":" + paths[i];
			}

			size_t fpos = paths[i].find(L" /s");
			if(fpos==paths[i].size()-3)
			{
				paths[i] = paths[i].substr(0, paths[i].size()-3);
			}
			else
			{
				paths[i] = greplace(L"*", L":", paths[i]);
			}

			if(!userprofile_dir.empty() && paths[i].find(userprofile_dir)==0)
			{
				std::wstring profile_base_dir = ExtractFilePath(userprofile_dir);
				for(size_t j=0;j<users.size();++j)
				{
					std::wstring prof_dir = profile_base_dir + "\\" + users[j];
					if(DirectoryExists(prof_dir.c_str()))
					{
						std::wstring new_prof_dir = paths[i];
						new_prof_dir.replace(new_prof_dir.begin(), new_prof_dir.begin() + userprofile_dir.size(),
							prof_dir.begin(), prof_dir.end());

						if(new_prof_dir!=paths[i])
						{
							paths.push_back(new_prof_dir);
						}
					}
				}
			}
		}
		return paths;
	}

	

	std::wstring get_volume_path(PWCHAR VolumeName)
	{		
		PWCHAR names = NULL;		
		DWORD CharCount = MAX_PATH + 1;
		std::wstring ret;

		BOOL rc;
		while(true)
		{
			names = (PWCHAR) new BYTE [CharCount * sizeof(WCHAR)];

			if ( !names )
			{
				return L"";
			}

			rc = GetVolumePathNamesForVolumeNameW(
				VolumeName, names, CharCount, &CharCount);

			if ( rc ) 
			{
				break;
			}

			if ( GetLastError() != ERROR_MORE_DATA ) 
			{
				break;
			}

			delete [] names;
			names = NULL;
		}

		if ( rc )
		{
			PWCHAR name = NULL;
			for ( name = names; name[0] != 0; name += wcslen(name) + 1 ) 
			{
				std::wstring cname = name;
				if(ret.empty() || cname.size()<ret.size())
				{
					ret = cname;
				}
			}
		}

		if ( names != NULL )
		{
			delete [] names;
			names = NULL;
		}

		return ret;
	}

	bool is_usb_disk(std::wstring path, SVolumesCache* cache)
	{
		std::map<std::wstring, bool>::iterator it = cache->is_usb_info.find(path);

		if(it!=cache->is_usb_info.end())
		{
			return it->second;
		}

		HANDLE hVolume = CreateFileW(path.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, NULL);

		if ( hVolume != INVALID_HANDLE_VALUE )
		{
			DWORD dwOutBytes = 0; 
			STORAGE_PROPERTY_QUERY query;  

			query.PropertyId = StorageDeviceProperty;
			query.QueryType = PropertyStandardQuery;

			char buffer[1024] = {}; 
			PSTORAGE_DEVICE_DESCRIPTOR dev_desc = (PSTORAGE_DEVICE_DESCRIPTOR)buffer;
			dev_desc->Size = sizeof(buffer);

			BOOL b = DeviceIoControl(hVolume,
				IOCTL_STORAGE_QUERY_PROPERTY,
				&query, sizeof(STORAGE_PROPERTY_QUERY),
				dev_desc, dev_desc->Size,
				&dwOutBytes,
				(LPOVERLAPPED)NULL);

			CloseHandle(hVolume);

			if ( b )
			{
				bool is_usb = dev_desc->BusType == BusTypeUsb;

				cache->is_usb_info[path] = is_usb;

				return is_usb;
			}
		}

		return false;
	}

	std::string get_all_volumes_list(bool filter_usb, SVolumesCache*& cache)
	{
		WCHAR vol_name[MAX_PATH] = {};
		HANDLE hFind = FindFirstVolumeW(vol_name, ARRAYSIZE(vol_name));

		if (hFind == INVALID_HANDLE_VALUE)
		{
			return "";
		}

		if(cache==NULL)
		{
			cache=new SVolumesCache;
		}

		std::string ret;

		while(true)
		{
			size_t idx = wcslen(vol_name) - 1;

			if (vol_name[0] != L'\\' || vol_name[1] != L'\\' ||
				vol_name[2] != L'?'  ||	vol_name[3] != L'\\' ||
				vol_name[idx] != L'\\' )
			{
				std::cerr << std::wstring(L"get_all_volumes_list: bad path: ") + vol_name << std::endl;
				return "";
			}

			vol_name[idx] = L'\0';

			WCHAR DeviceName[MAX_PATH] = {};
			DWORD CharCount = QueryDosDeviceW(&vol_name[4], DeviceName, ARRAYSIZE(DeviceName));

			vol_name[idx] = L'\\';

			if ( CharCount == 0 )
			{
				std::cerr << std::wstring(L"QueryDosDeviceW failed with ec = ") + convert(static_cast<int>(GetLastError())) << std::endl;
				break;
			}

			std::wstring new_volume = get_volume_path(vol_name);

			if(new_volume.size()==3 && new_volume[1]==':' && new_volume[2]=='\\')
			{
				std::map<std::wstring, bool>::iterator it_ntfs_info = cache->is_ntfs_info.find(new_volume);

				std::wstring dev_fn = std::wstring(L"\\\\.\\")+std::wstring(1, new_volume[0])+L":";

				if(it_ntfs_info!=cache->is_ntfs_info.end())
				{
					if(it_ntfs_info->second)
					{
						if(!filter_usb || !is_usb_disk(dev_fn, cache))
						{
							if(!ret.empty())
							{
								ret+=";";
							}
							ret+=new_volume[0];
						}
						else if(filter_usb)
						{
							std::cerr << "Device "+new_volume+" is connected via USB" << std::endl;
						}
					}
					else
					{
						std::cerr << "Device "+new_volume+" isn't NTFS formatted" << std::endl;
					}
				}
				else
				{				
					bool is_ntfs = false;
					wchar_t fs_name[MAX_PATH+1];
					if(GetVolumeInformation(new_volume.c_str(), NULL, 0, NULL, NULL, NULL,fs_name,MAX_PATH+1))
					{
						is_ntfs = strlower(std::wstring(fs_name))=="ntfs";
					}

					if(is_ntfs)
					{
						if(!filter_usb || !is_usb_disk(dev_fn, cache))
						{
							if(!ret.empty())
							{
								ret+=";";
							}
							ret+=new_volume[0];
						}
						else if(!filter_usb)
						{
							std::cerr << "Device "+new_volume+" is connected via USB" << std::endl;
						}
					}

					cache->is_ntfs_info[new_volume] = is_ntfs;
				}

			}

			BOOL rc = FindNextVolumeW(hFind, vol_name, ARRAYSIZE(vol_name));
			if ( !rc )
			{
				if (GetLastError() != ERROR_NO_MORE_FILES)
				{
					std::cerr << std::wstring(L"FindNextVolumeW failed with ec = ") + convert(static_cast<int>(GetLastError())) << std::endl;
					break;
				}
				break;
			}
		}

		FindVolumeClose(hFind);	

		return ret;
	}

	std::vector<std::wstring> volumesToPaths(std::string vols)
	{
		std::vector<std::string> toks;
		TokenizeMail(vols, toks, ";");

		std::vector<std::wstring> ret;
		for(size_t i=0;i<toks.size();++i)
		{
			ret.push_back(widen(toks[i])+L":\\");
		}

		return ret;
	}


	void rewrite_settings(CFileSettingsReader &setupSettings, std::map<std::wstring, std::wstring> &new_setup_settings, std::string output_fn )
	{
		std::wstring data;
		std::vector<std::wstring> keys = setupSettings.getKeys();
		for(size_t i=0;i<keys.size();++i)
		{
			if(!data.empty()) data+=L"\n";
			std::wstring val;
			std::map<std::wstring, std::wstring>::iterator it = new_setup_settings.find(keys[i]);
			if(it==new_setup_settings.end())
			{
				if(setupSettings.getValue(keys[i], &val))
				{
					data+=keys[i] + L"=" + val;
				}
			}
			else
			{
				data+=keys[i] + L"=" + it->second;
				new_setup_settings.erase(it);
			}			
		}		

		for(std::map<std::wstring, std::wstring>::iterator it=new_setup_settings.begin();
			it!=new_setup_settings.end();++it)
		{
			if(!data.empty()) data+=L"\n";
			data+=it->first+ L"=" + it->second;
		}

		writestring(ConvertToUTF8(data), output_fn);
	}
#endif //_WIN32
}



SetupWizard::SetupWizard( wxWindow* parent )
	: GUISetupWizard(parent)
{
	SetIcon(wxIcon(res_path+wxT("backup-ok.")+ico_ext, ico_type));

	EFileBackupChoice fileBackupChoice;
	EImageBackupChoice imageBackupChoice;
	std::wstring volume_choice;
	bool no_setupwizard;
	readConfig(fileBackupChoice, imageBackupChoice, volume_choice, no_setupwizard);

	if(fileBackupChoice == EFileBackupChoice_UserFiles)
	{
		m_radioBtn2->SetValue(true);
	}
	else if(fileBackupChoice == EFileBackupChoice_Manual)
	{
		m_radioBtn3->SetValue(true);
	}

	if(imageBackupChoice == EImageBackupChoice_AllNonUsb)
	{
		m_radioBtn5->SetValue(true);
	}
	else if(imageBackupChoice==EImageBackupChoice_Manual)
	{
		m_radioBtn6->SetValue(true);
		m_textCtrl1->Enable();
	}

	m_textCtrl1->SetValue(volume_choice);

	GetPageAreaSizer()->Add(*m_pages.begin());

	if(no_setupwizard)
	{
		Close();
	}
}

void SetupWizard::wizardCancel( wxWizardEvent& event )
{
	writestring("no_setupwizard=true", "setup_wizard.cfg");
}

void SetupWizard::wizardFinished( wxWizardEvent& event )
{
	EFileBackupChoice fileBackupChoice;

	if(m_radioBtn1->GetValue())
	{
		fileBackupChoice = EFileBackupChoice_AllWithoutSystem;
	}
	else if(m_radioBtn2->GetValue())
	{
		fileBackupChoice = EFileBackupChoice_UserFiles;
	}
	else
	{
		fileBackupChoice = EFileBackupChoice_Manual;
	}

	EImageBackupChoice imageBackupChoice;
	if(m_radioBtn4->GetValue())
	{
		imageBackupChoice = EImageBackupChoice_System;
	}
	else if(m_radioBtn5->GetValue())
	{
		imageBackupChoice = EImageBackupChoice_AllNonUsb;
	}
	else
	{
		imageBackupChoice = EImageBackupChoice_Manual;
	}

	finishSetup(fileBackupChoice, imageBackupChoice, m_textCtrl1->GetValue().ToStdWstring());


	if(fileBackupChoice==EFileBackupChoice_Manual)
	{
		Close();

		ConfigPath* cp = new ConfigPath(NULL);
		cp->ShowModal();
		cp->Destroy();
		wxExit();
	}
}

void SetupWizard::wizardNext( wxWizardEvent& event )
{
}


void SetupWizard::finishSetup( EFileBackupChoice fileBackupChoice, EImageBackupChoice imageBackupChoice, const std::wstring& volume_choice )
{
#ifdef _WIN32
	std::vector<std::wstring> pathsNotToBackup = fixupPaths(resolvePaths(getPathsNotToBackup()));

	pathsNotToBackup.push_back(L"C:\\Users\\:\\AppData\\Local\\Temp");
	pathsNotToBackup.push_back(L"C:\\Users\\:\\AppData\\Local\\Microsoft\\Windows\\Temporary Internet Files");
	pathsNotToBackup.push_back(L"C:\\Users\\:\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Cache");
	pathsNotToBackup.push_back(L"C:\\Users\\:\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Media Cache");
	pathsNotToBackup.push_back(L"C:\\Users\\:\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Code Cache");
	pathsNotToBackup.push_back(L"C:\\Users\\:\\AppData\\Local\\Microsoft\\Windows\\Explorer\\thumbcache*");
	pathsNotToBackup.push_back(L"C:\\Users\\:\\AppData\\Local\\Mozilla\\Firefox\\Profiles\\:\\cache2");
	pathsNotToBackup.push_back(L"C:\\Users\\:\\AppData\\Local\\Mozilla\\Firefox\\Profiles\\:\\cache");
	pathsNotToBackup.push_back(L"C:\\Windows\\Temp");
	pathsNotToBackup.push_back(L":\\$Recycle.Bin");
	pathsNotToBackup.push_back(L":\\System Volume Information");
	pathsNotToBackup.push_back(L"C:\\Windows.old");
	pathsNotToBackup.push_back(L"C:\\$Windows.~BT");
	pathsNotToBackup.push_back(L"C:\\ProgramData\\Microsoft\\Windows Defender\\Scans\\mpcache-*");

	std::vector<std::wstring> backupPaths;

	std::vector<std::wstring> includeDirs;

	if(fileBackupChoice==EFileBackupChoice_AllWithoutSystem)
	{
		pathsNotToBackup.push_back(L"C:\\Windows");
		pathsNotToBackup.push_back(L"C:\\$GetCurrent");

		TCHAR pf[MAX_PATH];
		if(SHGetFolderPathW(
			0,
			CSIDL_PROGRAM_FILES, 
			NULL, 
			SHGFP_TYPE_CURRENT,
			pf)==S_OK)
		{
			pathsNotToBackup.push_back(std::wstring(pf));
		}

		memset(pf, 0, MAX_PATH);

		if(SHGetFolderPathW(
			0,
			CSIDL_PROGRAM_FILESX86, 
			NULL, 
			SHGFP_TYPE_CURRENT,
			pf)==S_OK)
		{
			pathsNotToBackup.push_back(std::wstring(pf));
		}

		memset(pf, 0, MAX_PATH);

		if (SHGetFolderPathW(
			0,
			CSIDL_COMMON_APPDATA,
			NULL,
			SHGFP_TYPE_CURRENT,
			pf) == S_OK)
		{
			pathsNotToBackup.push_back(std::wstring(pf));
		}		
		
		backupPaths = volumesToPaths(get_all_volumes_list(false, cache));
	}
	else if(fileBackupChoice==EFileBackupChoice_UserFiles)
	{
		backupPaths.push_back(L"C:\\Users");

		includeDirs.push_back(L"C:\\Users\\:\\Documents\\*");
		includeDirs.push_back(L"C:\\Users\\:\\Pictures\\*");
		includeDirs.push_back(L"C:\\Users\\:\\Music\\*");
		includeDirs.push_back(L"C:\\Users\\:\\Videos\\*");
		includeDirs.push_back(L"C:\\Users\\:\\Desktop\\*");
	}


	std::wstring backup_volumes;
	if(imageBackupChoice==EImageBackupChoice_System)
	{
		backup_volumes=L"C";
	}
	else if(imageBackupChoice==EImageBackupChoice_AllNonUsb)
	{
		backup_volumes = widen(get_all_volumes_list(true, cache));
	}
	else
	{
		backup_volumes = volume_choice;
	}

	wxMilliClock_t starttime = wxGetLocalTimeMillis();
	while(!FileExists("urbackup/data/settings.cfg")
		&& FileExists("initial_settings.cfg") 
		&& wxGetLocalTimeMillis()-starttime<2*60*1000)
	{
		wxMilliSleep(100);
	}

	CFileSettingsReader setupSettings("setup_wizard.cfg");
	CFileSettingsReader backupSettings("urbackup/data/settings.cfg");
	std::map<std::wstring, std::wstring> new_settings;
	std::map<std::wstring, std::wstring> new_setup_settings;

	if(!backup_volumes.empty())
	{
		std::wstring curr_images;
		std::wstring image_key = L"image_letters";
		bool has_image_letters = true;
		if(!backupSettings.getValue(image_key, &curr_images))
		{
			has_image_letters = backupSettings.getValue(image_key+L"_def", &curr_images);
			if(has_image_letters) image_key+=L"_def";
		}

		std::wstring setup_image;
		if( (setupSettings.getValue(L"image_letters", &setup_image) &&
			setup_image==curr_images ) || !has_image_letters )
		{
			new_settings[image_key] = backup_volumes;
			new_setup_settings[L"image_letters"] = backup_volumes;
		}
	}

	if(!backupPaths.empty())
	{
		std::wstring new_backupdirs;

		for(size_t i=0;i<backupPaths.size();++i)
		{
			if(!new_backupdirs.empty())
				new_backupdirs+=L";";

			new_backupdirs+=backupPaths[i];
		}

		std::wstring curr_backups;
		std::wstring backup_key = L"default_dirs";
		bool has_default_dirs = true;
		if(!backupSettings.getValue(backup_key, &curr_backups))
		{
			has_default_dirs = backupSettings.getValue(backup_key+L"_def", &curr_backups);
			if(has_default_dirs) backup_key+=L"_def";
		}


		std::wstring setup_backup;
		if( (setupSettings.getValue(L"default_dirs", &setup_backup) &&
			setup_backup==curr_backups) || !has_default_dirs)
		{
			new_settings[backup_key] = new_backupdirs;
			new_setup_settings[L"default_dirs"] = new_backupdirs;
		}
	}	

	if(!pathsNotToBackup.empty())
	{
		std::wstring new_exclude_files;
		for(size_t i=0;i<pathsNotToBackup.size();++i)
		{
			if(!new_exclude_files.empty())
				new_exclude_files+=L";";
			new_exclude_files+=pathsNotToBackup[i];
		}

		std::wstring curr_excludes;
		std::wstring exclude_key=L"exclude_files";
		bool has_exclude_files = true;
		if(!backupSettings.getValue(exclude_key, &curr_excludes))
		{
			has_exclude_files = backupSettings.getValue(exclude_key+L"_def", &curr_excludes);
			if(has_exclude_files) exclude_key+=L"_def";
		}	

		std::wstring setup_exclude;
		if( (setupSettings.getValue(L"exclude_files", &setup_exclude) &&
			setup_exclude==curr_excludes) || !has_exclude_files )
		{
			new_settings[exclude_key] = new_exclude_files;
			new_setup_settings[L"exclude_files"] = new_exclude_files;
		}
	}

	if(!includeDirs.empty())
	{
		std::wstring new_include_files;
		for(size_t i=0;i<includeDirs.size();++i)
		{
			if(!new_include_files.empty())
				new_include_files+=L";";
			new_include_files+=includeDirs[i];
		}

		std::wstring curr_includes;
		std::wstring include_key = L"include_files";
		bool has_include_files = true;
		if(!backupSettings.getValue(include_key, &curr_includes))
		{
			has_include_files = backupSettings.getValue(include_key+L"_def", &curr_includes);
			if(has_include_files) include_key+=L"_def";
		}

		std::wstring setup_include;
		if( (setupSettings.getValue(L"include_files", &setup_include) &&
			setup_include==curr_includes ) || !has_include_files)
		{
			new_settings[include_key] = new_include_files;
			new_setup_settings[L"include_files"] = new_include_files;
		}
	}

	if(!new_settings.empty())
	{
		std::wstring data;
		std::vector<std::wstring> keys = backupSettings.getKeys();
		for(size_t i=0;i<keys.size();++i)
		{
			if(!data.empty()) data+=L"\n";
			std::wstring val;
			std::map<std::wstring, std::wstring>::iterator it = new_settings.find(keys[i]);
			if(it==new_settings.end())
			{
				if(backupSettings.getValue(keys[i], &val))
				{
					data+=keys[i] + L"=" + val;
				}
			}
			else
			{
				data+=keys[i] + L"=" + it->second;
				new_settings.erase(it);
			}			
		}		

		for(std::map<std::wstring, std::wstring>::iterator it=new_settings.begin();
			it!=new_settings.end();++it)
		{
			if(!data.empty()) data+=L"\n";
			data+=it->first+ L"=" + it->second;
		}

		if(!Connector::updateSettings(ConvertToUTF8(data), 3*60*1000))
		{
			std::cerr << "Error saving settings" << std::endl;
		}		
	}

	switch(fileBackupChoice)
	{
	case EFileBackupChoice_AllWithoutSystem:
		new_setup_settings[L"FileBackupChoice"] = L"AllWithoutSystem"; break;
	case EFileBackupChoice_UserFiles:
		new_setup_settings[L"FileBackupChoice"] = L"UserFiles"; break;
	case EFileBackupChoice_Manual:
		new_setup_settings[L"FileBackupChoice"] = L"Manual"; break;
	}

	switch(imageBackupChoice)
	{
	case EImageBackupChoice_AllNonUsb:
		new_setup_settings[L"ImageBackupChoice"] = L"AllNonUsb"; break;
	case EImageBackupChoice_System:
		new_setup_settings[L"ImageBackupChoice"] = L"System"; break;
	case EImageBackupChoice_Manual:
		new_setup_settings[L"ImageBackupChoice"] = L"Manual";
		new_setup_settings[L"VolumeChoice"] = volume_choice;
		break;
	}

	if(!new_setup_settings.empty())
	{
		rewrite_settings(setupSettings, new_setup_settings, "setup_wizard.cfg");
	}
#endif
}

void SetupWizard::doDefConfig()
{
	EFileBackupChoice fileBackupChoice;
	EImageBackupChoice imageBackupChoice;
	std::wstring volume_choice;
	bool no_setupwizard;
	readConfig(fileBackupChoice, imageBackupChoice, volume_choice, no_setupwizard);
	if(!no_setupwizard)
	{
		finishSetup(fileBackupChoice, imageBackupChoice, volume_choice);
	}
}

void SetupWizard::manualVolumeConfig( wxCommandEvent& event )
{
	if(m_radioBtn6->GetValue())
	{
		m_textCtrl1->Enable();
	}
	else
	{
		m_textCtrl1->Disable();
	}
}

void SetupWizard::readConfig( EFileBackupChoice& fileBackupChoice, EImageBackupChoice& imageBackupChoice, std::wstring& volume_choice, bool& no_setupwizard )
{
	CFileSettingsReader setupSettings("setup_wizard.cfg");

	std::string str_no_setupwizard;
	setupSettings.getValue("no_setupwizard", &str_no_setupwizard);

	no_setupwizard = (str_no_setupwizard=="true");

	if(no_setupwizard)
	{
		return;
	}

	std::wstring lastImageBackupChoice;
	std::wstring lastFileBackupChoice;
	setupSettings.getValue(L"ImageBackupChoice", &lastImageBackupChoice);
	setupSettings.getValue(L"FileBackupChoice", &lastFileBackupChoice);

	fileBackupChoice = EFileBackupChoice_AllWithoutSystem; //Default

	if(lastFileBackupChoice==L"UserFiles")
	{
		fileBackupChoice = EFileBackupChoice_UserFiles;
	}
	else if(lastFileBackupChoice == L"Manual" )
	{
		fileBackupChoice = EFileBackupChoice_Manual;
	}

#ifdef _WIN32
	volume_choice = widen(get_all_volumes_list(false, cache));
#endif

	if(lastImageBackupChoice==L"AllNonUsb")
	{
		imageBackupChoice = EImageBackupChoice_AllNonUsb;
	}
	else if(lastImageBackupChoice==L"Manual")
	{
		imageBackupChoice = EImageBackupChoice_Manual;

		setupSettings.getValue(L"VolumeChoice", &volume_choice);
	}
}

bool SetupWizard::runSetupWizard()
{
	EFileBackupChoice fileBackupChoice;
	EImageBackupChoice imageBackupChoice;
	std::wstring volume_choice;
	bool no_setupwizard;
	readConfig(fileBackupChoice, imageBackupChoice, volume_choice, no_setupwizard);

	return !no_setupwizard;
}
