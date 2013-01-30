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
#include <iostream>

std::string Connector::pw;
bool Connector::error=false;
const size_t conn_retries=4;
bool Connector::busy=false;

extern std::string g_res_path;

#ifdef wxUSE_WCHAR_T
std::string ConvertToUTF8(const std::wstring &str);
std::wstring ConvertToUnicode(const std::string &str);
#else
#define std_string std::string
#define ConvertToUTF8(x)
#define ConvertToUnicode(x)
#endif


std::string Connector::getResponse(const std::string &cmd, const std::string &args)
{
	busy=true;
	error=false;
	if(pw.empty())
	{
		if(FileExists(g_res_path+"pw.txt"))
			pw=getFile(g_res_path+"pw.txt");
		else if(FileExists("pw.txt"))
			pw=getFile("pw.txt");
		else if(FileExists("/usr/local/var/urbackup/pw.txt"))
			pw=getFile("/usr/local/var/urbackup/pw.txt");
		else if(FileExists("/var/urbackup/pw.txt"))
			pw=getFile("/var/urbackup/pw.txt");
		else if(FileExists("/var/lib/urbackup/pw.txt"))
			pw=getFile("/var/lib/urbackup/pw.txt");
		if(pw.empty())
		{
			std::cout << "Could not load password file!" << std::endl;
		}
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
			wxTheApp->Yield(false);
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
			wxTheApp->Yield(false);
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
	std::string d=getResponse("1GET BACKUP DIRS","");
	int lc=linecount(d);
	for(int i=0;i<lc;i+=2)
	{
		SBackupDir bd;
		bd.id=atoi(getline(i, d).c_str() );
		std::string path=getline(i+1, d);
		bd.path=wxString::FromUTF8(path.c_str() );
		if(path.find("|")!=std::string::npos)
		{
			bd.path=ConvertToUnicode(getafter("|", path));
			bd.name=ConvertToUnicode(getuntil("|", path));
		}
		ret.push_back( bd );
	}
	return ret;
}

std::string Connector::escapeParam(const std::string &name)
{
	std::string tmp=greplace("%", "%25", name);
	tmp=greplace("=", "%3D", tmp);
	tmp=greplace("&", "%26", tmp);
	return tmp;
}

bool Connector::saveSharedPaths(const std::vector<SBackupDir> &res)
{
	std::string args;
	for(size_t i=0;i<res.size();++i)
	{
		if(i!=0)
			args+="&";

		std::string path=escapeParam(res[i].path.ToUTF8().data());
		std::string name=escapeParam(res[i].name.ToUTF8().data());

		args+="dir_"+nconvert(i)+"="+path;
		args+="&dir_"+nconvert(i)+"_name="+name;
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
	ret.capa=0;
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
	if(toks.size()>4)
	{
		std::map<std::wstring,std::wstring> params;
		ParseParamStr(toks[4], &params);
		std::map<std::wstring,std::wstring>::iterator it_capa=params.find(L"capa");
		if(it_capa!=params.end())
		{
			ret.capa=watoi(it_capa->second);
		}
		std::map<std::wstring,std::wstring>::iterator it_new_server=params.find(L"new_ident");
		if(it_new_server!=params.end())
		{
			ret.new_server=wnarrow(it_new_server->second);
		}
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

bool Connector::addNewServer(const std::string &ident)
{
	std::string d=getResponse("NEW SERVER","ident="+ident);

	if(d!="OK")
		return false;
	else
		return true;
}