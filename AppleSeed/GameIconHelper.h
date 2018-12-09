#pragma once
#include <string>
#include "AppHelper.h"
using std::wstring;



//获取图标下载文件路径
inline wstring GetGameIconFile(const wstring& strName, const wstring& strVersion) {
	wstring strIconName = strName + L"_" + strVersion + L".png";
	return GetGamesIcoPath() + L"\\" + strIconName;
}

//获取130*130压缩比例图标文件路径
inline wstring GetGameIcon130File(const wstring& strName, const wstring& strVersion)
{
    wstring strIconName = strName + L"_" + strVersion + L".png";
    return GetGamesIco130Path() + L"\\" + strIconName;
}


//获取游戏logo下载文件路径
inline wstring GetGameLogoFile(const wstring& strName, const wstring& strVersion) {
    wstring strIconName = strName + L"_" + strVersion + L".png";
    return GetGamesLogoPath() + L"\\" + strIconName;
}