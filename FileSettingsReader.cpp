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

#include "utf8/utf8.h"
#include "stringtools.h"
#include "FileSettingsReader.h"

std::wstring ConvertToUnicode(const std::string &input)
{
    std::wstring ret;
    try
    {
		if(sizeof(wchar_t)==2)
			utf8::utf8to16(input.begin(), input.end(), back_inserter(ret));
        else
			utf8::utf8to32(input.begin(), input.end(), back_inserter(ret));
    }
    catch(...){}
	
    return ret;
}

std::string ConvertToUTF8(const std::wstring &input)
{
    std::string ret;
    try
    {
	if(sizeof(wchar_t)==2 )
    	    utf8::utf16to8(input.begin(), input.end(), back_inserter(ret));
        else
            utf8::utf32to8(input.begin(), input.end(), back_inserter(ret));
    }
    catch(...){}
    return ret;
}

CFileSettingsReader::CFileSettingsReader(std::string pFile)
{
	std::string pData=getFile(pFile);

	int num_lines=linecount(pData);
	for(int i=0;i<num_lines;++i)
	{
		std::string line=getline(i,pData);

		if(line.size()<2 || line[0]=='#' )
			continue;

		std::string key=getuntil("=",line);
		std::string value;

		if(key=="")
			value=line;
		else
		{
			line.erase(0,key.size()+1);
			value=line;
		}		
		
		mSettingsMap.insert(std::pair<std::wstring,std::wstring>(ConvertToUnicode(key), ConvertToUnicode(value)) );
	}
}

bool CFileSettingsReader::getValue(std::string key, std::string *value)
{
	std::wstring s_value;
	bool b=getValue( widen(key), &s_value);

	if(b==true)
	{
		std::string nvalue=wnarrow(s_value);
		*value=nvalue;		
		return true;
	}

	return false;
}

bool CFileSettingsReader::getValue(std::wstring key, std::wstring *value)
{
	std::map<std::wstring,std::wstring>::iterator i=mSettingsMap.find(key);
	if( i!=mSettingsMap.end() )
	{
		*value=i->second;
		return true;
	}
	return false;
}

std::vector<std::wstring> CFileSettingsReader::getKeys(void)
{
	std::vector<std::wstring> ret;
	for(std::map<std::wstring,std::wstring>::iterator i=mSettingsMap.begin();i!=mSettingsMap.end();++i)
	{
		ret.push_back(i->first);
	}
	return ret;
}
