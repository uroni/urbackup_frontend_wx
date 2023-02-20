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

#include "utf8/utf8.h"
#include "stringtools.h"
#include "FileSettingsReader.h"

namespace
{
	std::string removeTrailingCR(const std::string& str)
	{
		if (!str.empty() && str[str.size() - 1] == '\r')
			return str.substr(0, str.size() - 1);
		else
			return str;
	}
}

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

	int state = 0;
	std::string key;
	std::string value;

	for (size_t i = 0; i < pData.size(); ++i)
	{
		char ch = pData[i];
		switch (state)
		{
		case 0:
			if (ch == '#')
			{
				state = 1;
			}
			else if (ch != '\n')
			{
				key += ch;
				state = 2;
			}
			break;
		case 1:
			if (ch == '\n')
			{
				state = 0;
			}
			break;
		case 2:
			if (ch == '\n')
			{
				mSettingsMap[L""] = ConvertToUnicode(removeTrailingCR(key));
				key.clear();
			}
			else if (ch == '=')
			{
				state = 3;
			}
			else
			{
				key += ch;
			}
			break;
		case 3:
			if (ch == '\n')
			{
				mSettingsMap[ConvertToUnicode(key)] = ConvertToUnicode(removeTrailingCR(value));
				key.clear();
				value.clear();
				state = 0;
			}
			else
			{
				value += ch;
			}
			break;
		}
	}

	if (state == 3)
	{
		mSettingsMap[ConvertToUnicode(key)] = ConvertToUnicode(removeTrailingCR(value));
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
