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

struct SBackupDir
{
	wxString path;
	wxString name;
	int id;
};

struct SStatus
{
	wxString lastbackupdate;
	wxString status;
	wxString pcdone;
	bool pause;
	int capa;
	std::string new_server;
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

class Connector
{
public:
	static std::vector<SBackupDir> getSharedPaths(void);
	static bool saveSharedPaths(const std::vector<SBackupDir> &res);
	static SStatus getStatus(void);
	static unsigned int getIncrUpdateIntervall(void);
	static int startBackup(bool full);
	static int startImage(bool full);
	static bool updateSettings(const std::string &sdata);
	static std::vector<SLogEntry> getLogEntries(void);
	static std::vector<SLogLine> getLogdata(int logid, int loglevel);
	static bool setPause(bool b_pause);
	static bool addNewServer(const std::string &ident);


	static bool hasError(void);
	static bool isBusy(void);

private:
	static std::string getResponse(const std::string &cmd, const std::string &args);
	static std::string pw;
	static bool error;
	static bool busy;
};

#endif //CONNECTOR_H