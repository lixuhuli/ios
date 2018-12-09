#pragma once
#include <string>
#include <vector>
using std::string;
using std::wstring;
using std::vector;

namespace PublicLib{

	//����ת��
	string	UToA(const wstring& str);
	wstring AToU(const string& str);
	string	UToUtf8(const wstring& wstrUnicode);
	wstring Utf8ToU(const string &str);
	string AToUtf(const string& str);
	string UtfToA(const string& str);

	//�ַ���תСд
	wstring	StrLowerW(const wstring& str);
	string	StrLowerA(const string& str);
	//�ַ���ת��д
	wstring StrUpperW(const wstring& str);
	string  StrUpperA(const string& str);
	//����URL
	void	MyParseUrlW(const wstring& strUrl, wstring& strHostName, wstring& strPage, WORD& sPort);
	void	MyParseUrlA(const string& strUrl, string& strHostName, string& strPage, WORD& sPort);

	//URL�����
	string	UrlEncode(const string& strSrc);
	string	UrlDecode(const string& strSrc);

	//����·���е��ļ���
	string	GetNameByPathA(const string& strPath, bool bIsUrl);												
	wstring GetNameByPathW(const wstring& strPath, bool bIsUrl);
	
	//�ָ�
	bool SplitStringW(const wstring& strSource, const wstring& strFlag, vector<wstring>& paramList);
	bool SplitStringA(const string& strSource, const string& strFlag, vector<string>& paramList);

	string StrReplaceA(const string& strContent, const string& strTag, const string& strReplace);
	wstring StrReplaceW(const wstring& strContent, const wstring& strTag, const wstring& strReplace);

}