#pragma once
#include <string>
#include "AppHelper.h"
using std::wstring;



//��ȡͼ�������ļ�·��
inline wstring GetGameIconFile(const wstring& strName, const wstring& strVersion) {
	wstring strIconName = strName + L"_" + strVersion + L".png";
	return GetGamesIcoPath() + L"\\" + strIconName;
}

//��ȡ130*130ѹ������ͼ���ļ�·��
inline wstring GetGameIcon130File(const wstring& strName, const wstring& strVersion)
{
    wstring strIconName = strName + L"_" + strVersion + L".png";
    return GetGamesIco130Path() + L"\\" + strIconName;
}


//��ȡ��Ϸlogo�����ļ�·��
inline wstring GetGameLogoFile(const wstring& strName, const wstring& strVersion) {
    wstring strIconName = strName + L"_" + strVersion + L".png";
    return GetGamesLogoPath() + L"\\" + strIconName;
}