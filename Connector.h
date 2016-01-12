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

struct SBackupDir
{
	wxString path;
	wxString name;
	int id;
	int group;
};

struct SStatus
{
	SStatus()
		: pause(false), capa(0), has_server(false),
		needs_restore_restart(0), ask_restore_ok(false),
		error(false), init(false), restore_file(false)
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
	bool ask_restore_ok;
	bool restore_file;
	wxString restore_path;
	int needs_restore_restart;
	bool init;

	wxLongLong starttime;
	size_t timeoutms;
	wxSharedPtr<wxSocketClient> client;
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

struct SStatusDetails
{
	bool ok;
	wxString last_backup_time;
	
	std::vector<SRunningProcess> running_processes;
	std::vector<SUrBackupServer> servers;
	unsigned int time_since_last_lan_connection;
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

class Connector
{
public:
	static std::vector<SBackupDir> getSharedPaths(void);
	static bool saveSharedPaths(const std::vector<SBackupDir> &res);
	static int startBackup(bool full);
	static int startImage(bool full);
	static bool updateSettings(const std::string &sdata);
	static std::vector<SLogEntry> getLogEntries(void);
	static std::vector<SLogLine> getLogdata(int logid, int loglevel);
	static bool setPause(bool b_pause);
	static bool addNewServer(const std::string &ident);
	static SStatusDetails getStatusDetails();
	static int getCapabilities();
	static bool restoreOk(bool ok, wxLongLong_t& process_id);
	static SStatus initStatus(size_t timeoutms=5000);

	static bool hasError(void);
	static bool isBusy(void);

	static std::string getPasswordData(bool change_command, bool set_busy);

	static std::string getAccessParameters(const std::string& tokens);

private:
	static std::string escapeParam(const std::string &name);
	static std::string getResponse(const std::string &cmd, const std::string &args, bool change_command, size_t timeoutms=5000, bool set_busy=true);
	static std::string pw;
	static std::string pw_change;
	static bool error;
	static bool busy;
};

#endif //CONNECTOR_H