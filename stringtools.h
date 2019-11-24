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

#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H

#include <string>
#include <vector>
#include <map>
#include "types.h"

std::string getafter(const std::string &str,const std::string &data);
std::string getafterinc(const std::string &str,const std::string &data);
std::wstring getafter(const std::wstring &str,const std::wstring &data);
std::wstring getafterinc(const std::wstring &str,const std::wstring &data);
std::string getbetween(std::string s1,std::string s2,std::string data);
std::string strdelete(std::string str,std::string data);
void writestring(std::string str,std::string file);
void writestring(char *str, unsigned int len,std::string file);
std::string getuntil(std::string str,std::string data);
std::wstring getuntil(std::wstring str,std::wstring data);
std::string getuntilinc(std::string str,std::string data);
std::string getline(int line, const std::string &str);
int linecount(const std::string &str);
std::string getFile(std::string filename);
#ifdef _WIN32
std::string getFile(std::wstring filename);
#endif
std::wstring getFileUTF8(std::string filename);
std::string ExtractFileName(std::string fulln);
std::wstring ExtractFileName(std::wstring fulln);
std::string ExtractFilePath(std::string fulln);
std::wstring ExtractFilePath(std::wstring fulln);
std::wstring convert(bool pBool);
std::wstring convert(int i);
std::wstring convert(float f);
std::wstring convert(long long int i);
std::wstring convert(size_t i);
std::wstring convert(unsigned long long int i);
std::wstring convert(unsigned int i);
std::string nconvert(bool pBool);
std::string nconvert(int i);
std::string nconvert(long int i);
std::string nconvert(long long int i);
std::string nconvert(size_t i);
std::string nconvert(unsigned long long int i);
std::string nconvert(unsigned int i);
std::string nconvert(float f);
std::string findextension(const std::string& pString);
std::string wnarrow(const std::wstring& pStr);
std::wstring widen(std::string tw);
std::string replaceonce(std::string tor, std::string tin, std::string data);
std::wstring replaceonce(std::wstring tor, std::wstring tin, std::wstring data);
void Tokenize(std::string& str, std::vector<std::string> &tokens, std::string seps);
void Tokenize(std::wstring& str, std::vector<std::wstring> &tokens, std::wstring seps);
void TokenizeMail(std::string& str, std::vector<std::string> &tokens, std::string seps);
bool isnumber(char ch);
bool isletter(char ch);
bool isnumber(wchar_t ch);
bool isletter(wchar_t ch);
void strupper(std::string *pStr);
void strupper(std::wstring *pStr);
std::string greplace(std::string tor, std::string tin, std::string data);
std::wstring greplace(std::wstring tor, std::wstring tin, std::wstring data);
int getNextNumber(const std::string &pStr, int *read=NULL);
std::string strlower(const std::string &str);
bool next(const std::string &pData, const size_t & doff, const std::string &pStr);
bool next(const std::wstring &pData, const size_t & doff, const std::wstring &pStr);
char getRandomChar(void);
std::string getRandomNumber(void);
void transformHTML(std::string &str);
void EscapeSQLString(std::string &pStr);
void EscapeSQLString(std::wstring &pStr);
void EscapeCh(std::string &pStr, char ch='\\');
void EscapeCh(std::wstring &pStr, wchar_t ch);
std::string UnescapeSQLString(std::string pStr);
std::wstring UnescapeSQLString(std::wstring pStr);
void ParseParamStrHttp(const std::string &pStr, std::map<std::string, std::string> *pMap);
std::string FormatTime(int timeins);
bool IsHex(const std::string &str);
std::string byteToHex(unsigned char ch);
std::string bytesToHex(const unsigned char *b, size_t bsize);
std::string bytesToHex(const std::string& data);
unsigned long hexToULong(const std::string &data);
std::string htmldecode(std::string str, bool html=true, char xc='%');
bool checkhtml(const std::string &str);
std::string nl2br(std::string str);
bool FileExists(std::string pFile);
bool checkStringHTML(const std::string &str);
std::string ReplaceChar(std::string str, char tr, char ch);
std::wstring ReplaceChar(std::wstring str, wchar_t tr, wchar_t ch);
std::string striptags(std::string html);
std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_encode_dash(const std::string& data);
std::string base64_decode(std::string const& s);
bool CheckForIllegalChars(const std::string &str);
int watoi(std::wstring str);
std::wstring strlower(const std::wstring &str);
std::string trim(const std::string &str);
void replaceNonAlphaNumeric(std::string &str, char rch);
std::string conv_filename(std::string fn);
std::string EscapeParamString(const std::string &pStr);
std::string EscapePathParamString(const std::string &pStr);
std::string UnescapeParamString(const std::string & str);

namespace
{

	bool is_big_endian(void)
	{
		union {
			unsigned int i;
			char c[4];
		} bint = { 0x01020304 };

		return bint.c[0] == 1;
	}

	unsigned int endian_swap(unsigned int x)
	{
		return (x >> 24) |
			((x << 8) & 0x00FF0000) |
			((x >> 8) & 0x0000FF00) |
			(x << 24);
	}

	unsigned short endian_swap(unsigned short x)
	{
		return x = (x >> 8) |
			(x << 8);
	}

	std::string endian_swap_utf16(std::string str)
	{
		for (size_t i = 0; i<str.size(); i += 2)
		{
			unsigned short *t = (unsigned short*)&str[i];
			*t = endian_swap(*t);
		}
		return str;
	}

	unsigned long long int endian_swap(unsigned long long int x)
	{
#ifdef _WIN32
		return (x >> 56) |
			((x << 40) & 0x00FF000000000000) |
			((x << 24) & 0x0000FF0000000000) |
			((x << 8) & 0x000000FF00000000) |
			((x >> 8) & 0x00000000FF000000) |
			((x >> 24) & 0x0000000000FF0000) |
			((x >> 40) & 0x000000000000FF00) |
			(x << 56);
#else
		return (x >> 56) |
			((x << 40) & 0x00FF000000000000LLU) |
			((x << 24) & 0x0000FF0000000000LLU) |
			((x << 8) & 0x000000FF00000000LLU) |
			((x >> 8) & 0x00000000FF000000LLU) |
			((x >> 24) & 0x0000000000FF0000LLU) |
			((x >> 40) & 0x000000000000FF00LLU) |
			(x << 56);
#endif
	}

	unsigned int little_endian(unsigned int x)
	{
		if (is_big_endian())
		{
			return endian_swap(x);
		}
		else
		{
			return x;
		}
	}

	unsigned short little_endian(unsigned short x)
	{
		if (is_big_endian())
		{
			return endian_swap(x);
		}
		else
		{
			return x;
		}
	}

	int little_endian(int x)
	{
		if (is_big_endian())
		{
			return static_cast<int>(endian_swap(static_cast<unsigned int>(x)));
		}
		else
		{
			return x;
		}
	}

	uint64 little_endian(uint64 x)
	{
		if (is_big_endian())
		{
			return endian_swap(x);
		}
		else
		{
			return x;
		}
	}

	int64 little_endian(int64 x)
	{
		if (is_big_endian())
		{
			return static_cast<int64>(endian_swap(static_cast<uint64>(x)));
		}
		else
		{
			return x;
		}
	}

	float little_endian(float x)
	{
		if (is_big_endian())
		{
			unsigned int* ptr = reinterpret_cast<unsigned int*>(&x);
			unsigned int ret = endian_swap(*ptr);
			return *reinterpret_cast<float*>(&ret);
		}
		else
		{
			return x;
		}
	}

	unsigned int big_endian(unsigned int x)
	{
		if (!is_big_endian())
		{
			return endian_swap(x);
		}
		else
		{
			return x;
		}
	}

	unsigned short big_endian(unsigned short x)
	{
		if (!is_big_endian())
		{
			return endian_swap(x);
		}
		else
		{
			return x;
		}
	}

	int big_endian(int x)
	{
		if (!is_big_endian())
		{
			return static_cast<int>(endian_swap(static_cast<unsigned int>(x)));
		}
		else
		{
			return x;
		}
	}

	uint64 big_endian(uint64 x)
	{
		if (!is_big_endian())
		{
			return endian_swap(x);
		}
		else
		{
			return x;
		}
	}

	int64 big_endian(int64 x)
	{
		if (!is_big_endian())
		{
			return static_cast<int64>(endian_swap(static_cast<uint64>(x)));
		}
		else
		{
			return x;
		}
	}

	float big_endian(float x)
	{
		if (!is_big_endian())
		{
			unsigned int* ptr = reinterpret_cast<unsigned int*>(&x);
			unsigned int ret = endian_swap(*ptr);
			return *reinterpret_cast<float*>(&ret);
		}
		else
		{
			return x;
		}
	}

	std::string big_endian_utf16(std::string str)
	{
		if (!is_big_endian())
		{
			return endian_swap_utf16(str);
		}
		else
		{
			return str;
		}
	}

}

#endif
