#pragma once
#include <string>
#include <vector>
using std::string;
using std::wstring;
using std::vector;

namespace PublicLib{

	//编码转换
	string	UToA(const wstring& str);
	wstring AToU(const string& str);
	string	UToUtf8(const wstring& wstrUnicode);
	wstring Utf8ToU(const string &str);
	string AToUtf(const string& str);
	string UtfToA(const string& str);

	//字符串转小写
	wstring	StrLowerW(const wstring& str);
	string	StrLowerA(const string& str);
	//字符串转大写
	wstring StrUpperW(const wstring& str);
	string  StrUpperA(const string& str);
	//解析URL
	void	MyParseUrlW(const wstring& strUrl, wstring& strHostName, wstring& strPage, WORD& sPort);
	void	MyParseUrlA(const string& strUrl, string& strHostName, string& strPage, WORD& sPort);

	//URL编解码
	string	UrlEncode(const string& strSrc);
	string	UrlDecode(const string& strSrc);

	//解析路径中的文件名
	string	GetNameByPathA(const string& strPath, bool bIsUrl);												
	wstring GetNameByPathW(const wstring& strPath, bool bIsUrl);
	
	//分割
	bool SplitStringW(const wstring& strSource, const wstring& strFlag, vector<wstring>& paramList);
	bool SplitStringA(const string& strSource, const string& strFlag, vector<string>& paramList);

	string StrReplaceA(const string& strContent, const string& strTag, const string& strReplace);
	wstring StrReplaceW(const wstring& strContent, const wstring& strTag, const wstring& strReplace);

}