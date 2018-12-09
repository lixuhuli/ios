#include "stdafx.h"
#include "StrHelper.h"
#include <algorithm>


namespace PublicLib{

	string UToA(const wstring& str)
	{
		string strDes;
		if (str.empty())
			goto __end;
		int nLen = ::WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), NULL, 0, NULL, NULL);
		if (0 == nLen)
			goto __end;
		char* pBuffer = new char[nLen + 1];
		memset(pBuffer, 0, nLen + 1);
		::WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), pBuffer, nLen, NULL, NULL);
		pBuffer[nLen] = '\0';
		strDes.append(pBuffer);
		delete[] pBuffer;
	__end:
		return strDes;
	}

	wstring AToU(const string& str)
	{
		wstring strDes;
		if (str.empty())
			goto __end;
		int nLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
		if (0 == nLen)
			goto __end;
		wchar_t* pBuffer = new wchar_t[nLen + 1];
		memset(pBuffer, 0, nLen + 1);
		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), pBuffer, nLen);
		pBuffer[nLen] = '\0';
		strDes.append(pBuffer);
		delete[] pBuffer;
	__end:
		return strDes;
	}

	string UToUtf8(const wstring& wstrUnicode)
	{
		string strRet;
		if (wstrUnicode.empty())
			return strRet;
		int nLen = WideCharToMultiByte(CP_UTF8, 0, wstrUnicode.c_str(), -1, NULL, 0, NULL, NULL);
		char* pBuffer = new char[nLen + 1];
		pBuffer[nLen] = '\0';
		nLen = WideCharToMultiByte(CP_UTF8, 0, wstrUnicode.c_str(), -1, pBuffer, nLen, NULL, NULL);
		strRet.append(pBuffer);
		delete[] pBuffer;
		return strRet;
	}

	wstring Utf8ToU(const string &str)
	{
		int u16Len = ::MultiByteToWideChar(CP_UTF8, NULL, str.c_str(), (int)str.size(), NULL, 0);
		wchar_t* wstrBuf = new wchar_t[u16Len + 1];
		::MultiByteToWideChar(CP_UTF8, NULL, str.c_str(), (int)str.size(), wstrBuf, u16Len);
		wstrBuf[u16Len] = L'\0';
		wstring wStr;
		wStr.assign(wstrBuf, u16Len);
		delete[] wstrBuf;
		return wStr;
	}

	string AToUtf(const string& str)
	{
		wstring strUnicode = AToU(str);
		return UToUtf8(strUnicode);
	}

	string UtfToA(const string& str)
	{
		wstring strUnicode = Utf8ToU(str);
		return UToA(strUnicode);
	}

	wstring StrLowerW(const wstring& str)
	{
		wstring strOut;
		for (size_t i = 0; i < str.size(); ++i)
		{
			strOut += (wchar_t)towlower(str[i]);
		}
		return strOut;
	}

	string StrLowerA(const string& str)
	{
		string strOut;
		for (size_t i = 0; i < str.size(); ++i)
		{
			strOut += (char)tolower(str[i]);
		}
		return strOut;
	}

	wstring StrUpperW(const wstring& str)
	{
		wstring strOut;
		for (size_t i = 0; i < str.size(); ++i)
		{
			strOut += (wchar_t)towupper(str[i]);
		}
		return strOut;
	}

	string StrUpperA(const string& str)
	{
		string strOut;
		for (size_t i = 0; i < str.size(); ++i)
		{
			strOut += (char)toupper(str[i]);
		}
		return strOut;
	}

	void MyParseUrlW(const wstring& strUrl, wstring& strHostName, wstring& strPage, WORD& sPort)
	{
		sPort = 80;
		wstring strTemp = strUrl;
		int nPos = strTemp.find(L"http://");
		if (wstring::npos != nPos)
			strTemp = strTemp.substr(nPos + 7, strTemp.size() - nPos - 7);
		nPos = strTemp.find('/');
		if (wstring::npos == nPos)//没有找到 /
			strHostName = strTemp;
		else
			strHostName = strTemp.substr(0, nPos);
		int nPos1 = strHostName.find(':');
		if (nPos1 != wstring::npos)
		{
			wstring strPort = strTemp.substr(nPos1 + 1, strHostName.size() - nPos1 - 1);
			strHostName = strHostName.substr(0, nPos1);
			sPort = (WORD)_wtoi(strPort.c_str());
		}
		if (wstring::npos == nPos)
			return;
		strPage = strTemp.substr(nPos, strTemp.size() - nPos);
	}

	void MyParseUrlA(const string& strUrl, string& strHostName, string& strPage, WORD& sPort)
	{
		sPort = 80;
		string strTemp = strUrl;
		int nPos = strTemp.find("http://");
		if (string::npos != nPos)
			strTemp = strTemp.substr(nPos + 7, strTemp.size() - nPos - 7);
		nPos = strTemp.find('/');
		if (string::npos == nPos)//没有找到 /
			strHostName = strTemp;
		else
			strHostName = strTemp.substr(0, nPos);
		int nPos1 = strHostName.find(':');
		if (nPos1 != string::npos)
		{
			string strPort = strTemp.substr(nPos1 + 1, strHostName.size() - nPos1 - 1);
			strHostName = strHostName.substr(0, nPos1);
			sPort = (WORD)atoi(strPort.c_str());
		}
		if (string::npos == nPos)
			return;
		strPage = strTemp.substr(nPos, strTemp.size() - nPos);
	}

	bool SplitStringW(const wstring& strSource, const wstring& strFlag, vector<wstring>& paramList)
	{
		if (strSource.empty() || strFlag.empty())
			return false;
		paramList.clear();
		size_t nBeg = 0;
		size_t nFind = strSource.find(strFlag, nBeg);
		if (nFind == std::wstring::npos)
			paramList.push_back(strSource);
		else
		{
			while (true)
			{
				if (nFind != nBeg)
					paramList.push_back(strSource.substr(nBeg, nFind - nBeg));
				nBeg = nFind + strFlag.size();
				if (nBeg == strSource.size())
					break;
				nFind = strSource.find(strFlag, nBeg);
				if (nFind == std::wstring::npos)
				{
					paramList.push_back(wstring(strSource.begin() + nBeg, strSource.end()));
					break;
				}
			}
		}
		return true;
	}

	bool SplitStringA(const string& strSource, const string& strFlag, vector<string>& paramList)
	{
		if (strSource.empty() || strFlag.empty())
			return false;
		paramList.clear();
		size_t nBeg = 0;
		size_t nFind = strSource.find(strFlag, nBeg);
		if (nFind == std::string::npos)
			paramList.push_back(strSource);
		else
		{
			while (true)
			{
				if (nFind != nBeg)
					paramList.push_back(strSource.substr(nBeg, nFind - nBeg));
				nBeg = nFind + strFlag.size();
				if (nBeg == strSource.size())
					break;
				nFind = strSource.find(strFlag, nBeg);
				if (nFind == std::string::npos)
				{
					paramList.push_back(string(strSource.begin() + nBeg, strSource.end()));
					break;
				}
			}
		}
		return true;
	}

	inline	BYTE ToHex(BYTE ch)
	{
		return ch > 9 ? ch + 55 : ch + 48;
	}

	inline BYTE FromHex(BYTE ch)
	{
		BYTE ret;
		if (ch > 'A' && ch<'Z')
			ret = ch - 'A' + 10;
		else if (ch>'a' && ch<'z')
			ret = ch - 'a' + 10;
		else if (ch>'0' && ch < '9')
			ret = ch - '0';
		else ret = ch;
		return ret;
	}

	string UrlEncode(const string& strSrc)
	{
		string strDes;
		for (size_t i = 0; i < strSrc.size(); ++i)
		{
			BYTE ch = (BYTE)strSrc[i];
			if (isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~')
				strDes += ch;
			else if (ch == ' ')
				strDes += '+';
			else
			{
				strDes += '%';
				strDes += ToHex((ch >> 4));
				strDes += ToHex(ch % 16);
			}
		}
		return strDes;
	}

	string UrlDecode(const string& strSrc)
	{
		string strDes;
		for (size_t i = 0; i < strSrc.size(); i++)
		{
			BYTE ch = strSrc[i];
			if (ch == '+')
				strDes += ' ';
			else if (ch == '%')
			{
				BYTE h = FromHex((unsigned char)strSrc[++i]);
				BYTE l = FromHex((unsigned char)strSrc[++i]);
				strDes += (h << 4) + l;
			}
			else strDes += ch;
		}
		return strDes;
	}

	string GetNameByPathA(const string& strPath, bool bIsUrl)
	{
		char ch = bIsUrl ? '/' : '\\';
		int nPos = strPath.find_last_of(ch);
		if (nPos == string::npos)
		{
			return "";
		}
		return string(strPath.begin() + nPos + 1, strPath.end());
	}

	wstring GetNameByPathW(const wstring& strPath, bool bIsUrl)
	{
		wchar_t ch = bIsUrl ? '/' : '\\';
		int nPos = strPath.find_last_of(ch);
		if (nPos == wstring::npos)
		{
			return L"";
		}
		return wstring(strPath.begin() + nPos + 1, strPath.end());
	}

	string StrReplaceA(const string& strContent, const string& strTag, const string& strReplace)
	{
		size_t nBegin = 0, nFind = 0;
		nFind = strContent.find(strTag, nBegin);
		if (nFind == string::npos)
			return strContent;
		size_t nTagLen = strTag.size();
		string strRet;
		while (true)
		{
			strRet.append(strContent.begin() + nBegin, strContent.begin() + nFind);
			strRet.append(strReplace);
			nBegin = nFind + nTagLen;
			nFind = strContent.find(strTag, nBegin);
			if (nFind == string::npos)
			{
				strRet.append(strContent.begin() + nBegin, strContent.end());
				break;
			}
		}
		return strRet;
	}

	wstring StrReplaceW(const wstring& strContent, const wstring& strTag, const wstring& strReplace)
	{
		size_t nBegin = 0, nFind = 0;
		nFind = strContent.find(strTag, nBegin);
		if (nFind == wstring::npos)
			return strContent;
		size_t nTagLen = strTag.size();
		wstring strRet;
		while (true)
		{
			strRet.append(strContent.begin() + nBegin, strContent.begin() + nFind);
			strRet.append(strReplace);
			nBegin = nFind + nTagLen;
			nFind = strContent.find(strTag, nBegin);
			if (nFind == wstring::npos)
			{
				strRet.append(strContent.begin() + nBegin, strContent.end());
				break;
			}
		}
		return strRet;
	}

}//namespace