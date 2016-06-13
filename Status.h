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

#include "Connector.h"
#include "gui/GUI.h"

class Status : public GUIStatus, wxTimer
{
public:
	Status(wxWindow* parent, wxLongLong_t follow_only_process_id);

	static Status* getInstance();

	virtual void Notify(void);

protected:
	void OnClose();

private:

	static Status* instance;

	bool updateStatus(int errcnt);

	SStatusDetails last_status_details;

	SConnection connection;

	int error_count;
};