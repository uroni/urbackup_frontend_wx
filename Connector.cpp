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
#include "tcpstack.h"
#include "stringtools.h"
#include <wx/socket.h>
#include "escape.h"

std::string Connector::pw;
bool Connector::error=false;
const size_t conn_retries=4;
bool Connector::busy=false;


std::string Connector::getResponse(const std::string &cmd, const std::string &args)
{
	busy=true;
	error=false;
	if(pw.empty())
	{
		pw=getFile("pw.txt");
	}
	wxSocketClient client(wxSOCKET_BLOCK);
	wxIPV4address addr;
	addr.Hostname(wxT("127.0.0.1"));
	addr.Service(35623);
	if(!client.Connect(addr,false))
	{
		for(size_t k=0;k<conn_retries;++k)
		{
			if(client.WaitOnConnect(0,500))
			{
				break;
			}
			wxTheApp->SafeYield(NULL, false);
		}
	}
	if(!client.IsConnected())
	{
		wxSocketError err=client.LastError();
		error=true;
		busy=false;
		return "";
	}

	std::string t_args;

	if(!args.empty())
		t_args="&"+args;
	else
		t_args=args;

	CTCPStack tcpstack;
	tcpstack.Send(&client, cmd+"#pw="+pw+t_args);

	char *resp=NULL;
	char buffer[1024];
	size_t packetsize;
	while(resp==NULL)
	{
		bool conn=false;
		for(size_t k=0;k<conn_retries;++k)
		{
			if(client.WaitForRead(0,500))
			{
				conn=true;
				break;
			}
			wxTheApp->SafeYield(NULL, false);
			if(client.Error())
				break;
		}
		if(!conn)
		{
			error=true;
			busy=false;
			return "";
		}
		client.Read(buffer, 1024);
		
		if(client.Error())
		{
			error=true;
			busy=false;
			return "";
		}
		tcpstack.AddData(buffer, client.GetLastIOSize() );
		

		resp=tcpstack.getPacket(&packetsize);
		if(packetsize==0)
		{
			client.Close();
			busy=false;
			return "";
		}
	}

	std::string ret;
	ret.resize(packetsize);
	memcpy(&ret[0], resp, packetsize);
	delete resp;

	client.Close();

	busy=false;
	return ret;
}

bool Connector::hasError(void)
{
	return error;
}

std::vector<SBackupDir> Connector::getSharedPaths(void)
{
	std::vector<SBackupDir> ret;
	std::string d=getResponse("GET BACKUP DIRS","");
	int lc=linecount(d);
	for(int i=0;i<lc;i+=2)
	{
		SBackupDir bd;
		bd.id=atoi(getline(i, d).c_str() );
		bd.path=wxString::FromUTF8(getline(i+1, d).c_str() );
		ret.push_back( bd );
	}
	return ret;
}

bool Connector::saveSharedPaths(const std::vector<SBackupDir> &res)
{
	std::string args;
	for(size_t i=0;i<res.size();++i)
	{
		if(i!=0)
			args+="&";

		args+="dir_"+nconvert(i)+"="+(std::string)res[i].path.ToUTF8().data();
	}

	std::string d=getResponse("SAVE BACKUP DIRS", args);

	if(d!="OK")
		return false;
	else
		return true;
}

SStatus Connector::getStatus(void)
{
	std::string d=getResponse("STATUS","");

	std::vector<std::string> toks;
	Tokenize(d, toks, "#");

	SStatus ret;
	ret.pause=false;
	if(toks.size()>0)
		ret.lastbackupdate=wxString::FromUTF8(toks[0].c_str() );
	if(toks.size()>1)
		ret.status=wxString::FromUTF8(toks[1].c_str() );
	if(toks.size()>2)
		ret.pcdone=wxString::FromUTF8(toks[2].c_str() );
	if(toks.size()>3)
	{
		if(toks[3]=="P")
			ret.pause=true;
		else if(toks[3]=="NP")
			ret.pause=false;
	}


	return ret;
}

unsigned int Connector::getIncrUpdateIntervall(void)
{
	std::string d=getResponse("GET INCRINTERVALL","");

	return atoi(d.c_str());
}

int Connector::startBackup(bool full)
{
	std::string s;
	if(full)
		s="START BACKUP FULL";
	else
		s="START BACKUP INCR";

	std::string d=getResponse(s,"");

	if(d=="RUNNING")
		return 2;
	else if(d!="OK")
		return 0;
	else
		return 1;
}

int Connector::startImage(bool full)
{
	std::string s;
	if(full)
		s="START IMAGE FULL";
	else
		s="START IMAGE INCR";

	std::string d=getResponse(s,"");

	if(d=="RUNNING")
		return 2;
	else if(d!="OK")
		return 0;
	else
		return 1;
}

bool Connector::updateSettings(const std::string &ndata)
{
	std::string data=ndata;
	escapeClientMessage(data);
	std::string d=getResponse("UPDATE SETTINGS "+data,"");

	if(d!="OK")
		return false;
	else
		return true;
}

std::vector<SLogEntry> Connector::getLogEntries(void)
{
	std::string d=getResponse("GET LOGPOINTS","");
	int lc=linecount(d);
	std::vector<SLogEntry> ret;
	for(int i=0;i<lc;++i)
	{
		std::string l=getline(i, d);
		if(l.empty())continue;
		SLogEntry le;
		le.logid=atoi(getuntil("-", l).c_str() );
		std::string lt=getafter("-", l);
		le.logtime=wxString::FromUTF8(lt.c_str());
		ret.push_back(le);
	}
	return ret;
}

std::vector<SLogLine>  Connector::getLogdata(int logid, int loglevel)
{
	std::string d=getResponse("GET LOGDATA","logid="+nconvert(logid)+"&loglevel="+nconvert(loglevel));
	std::vector<std::string> lines;
	TokenizeMail(d, lines, "\n");
	std::vector<SLogLine> ret;
	for(size_t i=0;i<lines.size();++i)
	{
		std::string l=lines[i];
		if(l.empty())continue;
		SLogLine ll;
		ll.loglevel=atoi(getuntil("-", l).c_str());
		ll.msg=wxString::FromUTF8(getafter("-", l).c_str());
		ret.push_back(ll);
	}
	return ret;
}

bool Connector::setPause(bool b_pause)
{
	std::string data=b_pause?"true":"false";
	std::string d=getResponse("PAUSE "+data,"");

	if(d!="OK")
		return false;
	else
		return true;
}

bool Connector::isBusy(void)
{
	return busy;
}