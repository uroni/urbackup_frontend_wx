/*************************************************************************
*    UrBackup - Client/Server backup system
*    Copyright (C) 2011  Martin Raiber
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <wx/wx.h>
#include <string>
#include <vector>
#include <wx/socket.h>
#include <wx/sharedptr.h>
#include "tcpstack.h"

typedef wxLongLong_t int64;

struct SBackupDir
{
	wxString path;
	wxString name;
	int id;
	int group;
	wxString flags;
	int server_default;
};

struct SStatus
{
	SStatus()
		: pause(false), capa(0), has_server(false),
		needs_restore_restart(0), ask_restore_ok(0),
		error(false), init(false), restore_file(false),
		client(NULL)
	{
	}

	bool isAvailable();
	bool hasError();

	wxString lastbackupdate;
	wxString status;
	wxString pcdone;
	bool pause;
	int capa;
	std::string new_server;
	bool has_server;
	int ask_restore_ok;
	bool restore_file;
	wxString restore_path;
	int needs_restore_restart;
	bool init;

	wxLongLong starttime;
	size_t timeoutms;
	wxSocketClient* client;
	bool error;
	CTCPStack tcpstack;
};

struct SLogEntry
{
	int logid;
	wxString logtime;
};

struct SLogLine
{
	int loglevel;
	wxString msg;
};

struct SRunningProcess
{
	wxString action;
	int percent_done;
	wxLongLong_t eta_ms;
	wxString details;
	int detail_pc;
	wxLongLong_t process_id;

	bool operator==(const SRunningProcess& other) const
	{
		return action == other.action
			&& percent_done == other.percent_done
			&& eta_ms / 1000 / 60 == other.eta_ms / 1000 / 60
			&& details == other.details
			&& detail_pc == other.detail_pc
			&& process_id == other.process_id;
	}
};

struct SUrBackupServer
{
	bool internet_connection;
	wxString name;

	bool operator==(const SUrBackupServer& other) const
	{
		return internet_connection == other.internet_connection
			&& name == other.name;
	}
};

struct SFinishedProcess
{
	SFinishedProcess()
		:id(0), success(false)
	{
	}

	bool operator==(const SFinishedProcess& other) const
	{
		return id == other.id
			&& success == other.success;
	}

	int64 id;
	bool success;
};

struct SStatusDetails
{
	bool ok;
	wxLongLong_t last_backup_time;
	
	std::vector<SRunningProcess> running_processes;
	std::vector<SFinishedProcess> finished_processes;
	std::vector<SUrBackupServer> servers;
	int64 time_since_last_lan_connection;
	bool internet_connected;
	wxString internet_status;
	
	int capability_bits;

	bool operator==(const SStatusDetails& other) const
	{
		return ok == other.ok
			&& last_backup_time == other.last_backup_time
			&& running_processes == other.running_processes
			&& servers == other.servers
			&& time_since_last_lan_connection/1000/60 == other.time_since_last_lan_connection/1000/60
			&& internet_connected == other.internet_connected
			&& internet_status == other.internet_status
			&& capability_bits == other.capability_bits;
	}
};

struct SConnection
{
	SConnection()
		: client(NULL) 
	{}

	wxSocketClient* client;
};

struct SPathMap
{
	std::string source;
	std::string target;
};

struct SBackupFile
{
	int64 access;
	int64 creat;
	int64 mod;
	int backupid;
	int64 backuptime;
	bool isdir;
	std::string name;
	std::string shahash;
	int64 size;
};

struct SStartRestore
{
	SStartRestore()
		: ok(false), restore_id(0), status_id(0),
		log_id(0), process_id(0) {}

	bool ok;
	int64 restore_id;
	int64 status_id;
	int64 log_id;
	int64 process_id;
};

class Connector
{
public:
	static std::vector<SBackupDir> getSharedPaths(void);
	static bool saveSharedPaths(const std::vector<SBackupDir> &res);
	static int startBackup(bool full);
	static int startImage(bool full);
	static bool updateSettings(const std::string &sdata, size_t timeoutms = 5000);
	static std::vector<SLogEntry> getLogEntries(void);
	static std::vector<SLogLine> getLogdata(int logid, int loglevel);
	static bool setPause(bool b_pause);
	static bool addNewServer(const std::string &ident);
	static SStatusDetails getStatusDetails(SConnection* connection = NULL);
	static int getCapabilities();
	static bool restoreOk(bool ok, wxLongLong_t& process_id);
	static SStatus initStatus(wxSocketClient* last_client, bool fast, size_t timeoutms=5000);

	enum EAccessError
	{
		EAccessError_Ok,
		EAccessError_NoServer,
		EAccessError_NoTokens
	};

	static const int64 restore_flag_no_overwrite;
	static const int64 restore_flag_no_reboot_overwrite;
	static const int64 restore_flag_ignore_overwrite_failures;
	static const int64 restore_flag_mapping_is_alternative;
	static const int64 restore_flag_open_all_files_first;
	static const int64 restore_flag_reboot_overwrite_all;


	static std::string getFileBackupsList(EAccessError& access_error);
	static std::string getFileList(const std::string& path, int* backupid, EAccessError& access_error);
	static SStartRestore startRestore(const std::string& path, int backupid,
		const std::vector<SPathMap>& map_paths, EAccessError& access_error, bool clean_other,
		bool ignore_other_fs, bool follow_symlinks, int64 restore_flags);

	static std::vector<SBackupFile> getFileList(const std::string& path, EAccessError& access_error);

	static bool hasError(void);
	static bool isBusy(void);

	static std::string getPasswordData(bool change_command, bool set_busy);

	static std::string getAccessParameters(const std::string& tokens);

private:
	static std::string getResponse(const std::string &cmd, const std::string &args, bool change_command, SConnection* connection=NULL, size_t timeoutms=5000, bool set_busy=true);
	static bool readTokens();
	static std::string pw;
	static std::string pw_change;
	static bool error;
	static bool busy;
	static std::string tokens;
};

#endif //CONNECTOR_H