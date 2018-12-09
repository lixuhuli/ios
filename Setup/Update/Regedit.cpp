#include "StdAfx.h"
#include "Regedit.h"
#include <PublicLib/AutoRegkey.h>
#include <PublicLib/StrHelper.h>


bool RegWriteUninstInfo(const INSTALL_INFO& info)
{
	CAutoRegkey hKey;
	wstring strKeyPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
	strKeyPath += info.strKeyName;
	DWORD dwRet;
	if ( ERROR_SUCCESS != RegCreateKeyEx(HKEY_LOCAL_MACHINE, strKeyPath.c_str(), 0, 
		NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwRet))
	{
		OutputDebugString(L"RegWriteUninstInfo创建注册表项失败");
		return false;
	}
	LONG lRet = RegSetValueEx(hKey, L"DisplayIcon",		0, REG_SZ, (byte*)info.strExePath.c_str(),		info.strExePath.size()*2);
	RegSetValueEx(hKey, L"DisplayName",		0, REG_SZ, (byte*)info.strDisplayName.c_str(),	info.strDisplayName.size()*2);
	RegSetValueEx(hKey, L"DisplayVersion",	0, REG_SZ, (byte*)info.strVersion.c_str(),		info.strVersion.size()*2);
	RegSetValueEx(hKey, L"Publisher",		0, REG_SZ, (byte*)info.strPublisher.c_str(),	info.strPublisher.size()*2);
	RegSetValueEx(hKey, L"UninstallString", 0, REG_SZ, (byte*)info.strUninstPath.c_str(),	info.strUninstPath.size() * 2);
	RegSetValueEx(hKey, L"URLInfoAbout",	0, REG_SZ, (byte*)info.strUrl.c_str(),			info.strUrl.size()*2);
	RegSetValueEx(hKey, L"ChannelName",		0, REG_SZ, (byte*)info.strChannel.c_str(),		info.strChannel.size()*2);

	DWORD dwDisableAutorun = info.dwAutoRun ? 0 : 1;
	RegSetValueEx(hKey, L"DisableAutorun",		0, REG_DWORD, (byte*)&dwDisableAutorun,	sizeof(dwDisableAutorun));

	int nPos = info.strUninstPath.find_last_of('\\');
	if ( nPos != wstring::npos )
	{
		wstring strInstPath = info.strUninstPath.substr(0, nPos);
		RegSetValueEx(hKey, L"InstallLocation", 0, REG_SZ, (byte*)strInstPath.c_str(), strInstPath.size()*2);
	}
	if ( !info.strUpExeVersion.empty() )
		RegSetValueEx(hKey, L"UpdateExeVersion", 0, REG_SZ, (byte*)info.strUpExeVersion.c_str(), info.strUpExeVersion.size()*2);
	return true;
}


bool RegWriteUpdateexeVersion(const wstring& strKeyName, const wstring& strVersion)
{
	CAutoRegkey hKey;
	wstring strKeyPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
	strKeyPath += strKeyName;
	DWORD dwRet;
	if ( ERROR_SUCCESS != RegCreateKeyEx(HKEY_LOCAL_MACHINE, strKeyPath.c_str(), 0, 
		NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwRet))
	{
		OutputDebugString(L"RegWriteUpdateexeVersion创建注册表项失败");
		return false;
	}
	if ( !strVersion.empty() )
		RegSetValueEx(hKey, L"UpdateExeVersion", 0, REG_SZ, (byte*)strVersion.c_str(), strVersion.size()*2);
	return true;
}

bool RegGetInstallInfo(const wstring& strKeyName, wstring& strInstallPath, wstring& strVersion, wstring& strUpExeVersion, wstring& strChannelName)
{
	CAutoRegkey hKey;
	wstring strKeyPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
	strKeyPath.append(strKeyName);
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, strKeyPath.c_str(), 0, KEY_READ, &hKey))
	{
		OutputDebugString(L"RegGetInstallInfo打开注册表项失败");
		return false;
	}
	byte bBuff[1024] = {0};
	DWORD dwSize = 1024;
	DWORD dwType = 0;
	if ( ERROR_SUCCESS != RegQueryValueEx(hKey, L"InstallLocation", NULL, &dwType,  bBuff, &dwSize) )
		return false;
	strInstallPath = (wchar_t*)bBuff;
	dwType = 0;
	dwSize = 1024;
	if ( ERROR_SUCCESS != RegQueryValueEx(hKey, L"DisplayVersion", NULL, &dwType,  bBuff, &dwSize) )
		return false;
	strVersion = (wchar_t*)bBuff;
	dwType = 0;
	dwSize = 1024;
	if ( ERROR_SUCCESS == RegQueryValueEx(hKey, L"UpdateExeVersion", NULL, &dwType,  bBuff, &dwSize) )
		strUpExeVersion = (wchar_t*)bBuff;
	dwType = 0;
	dwSize = 1024;
	if (ERROR_SUCCESS == RegQueryValueEx(hKey, L"ChannelName", NULL, &dwType, bBuff, &dwSize))
		strChannelName = (wchar_t*)bBuff;
	return true;
}

bool RegWriteDefChannel(const wstring& strKeyName, const wstring& strChannelName)
{
	CAutoRegkey hKey;
	wstring strKeyPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
	strKeyPath += strKeyName;
	DWORD dwRet;
	if ( ERROR_SUCCESS != RegCreateKeyEx(HKEY_LOCAL_MACHINE, strKeyPath.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwRet))
	{
		OutputDebugString(L"RegWriteUninstInfo创建注册表项失败");
		return false;
	}
	return ERROR_SUCCESS == RegSetValueEx(hKey, L"ChannelName",	0, REG_SZ, (byte*)strChannelName.c_str(), strChannelName.size()*2);
}

bool RegDelUninstInfo(const wstring& strKeyName)
{
	CAutoRegkey hKey;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, KEY_ALL_ACCESS, &hKey))
		return false;
	return ERROR_SUCCESS == RegDeleteKey(hKey, strKeyName.c_str());
}

bool RegDelBootRun(const wstring& strKeyName)
{
	return true;
	//CAutoRegkey hKey;
	//wstring strKeyPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	//DWORD dwFlag = bIs64Key? (KEY_ALL_ACCESS|KEY_WOW64_64KEY):KEY_ALL_ACCESS;
	//if ( ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, strKeyPath.c_str(), 0, dwFlag, &hKey) )
	//	return false;
	//return ERROR_SUCCESS == RegDeleteKey(hKey, strKeyName.c_str());
}

bool RegGetUpdateExeVersion(const wstring& strKeyName, wstring& strVersion)
{
	CAutoRegkey hKey;
	wstring strKeyPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
	strKeyPath.append(strKeyName);
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, strKeyPath.c_str(), 0, KEY_READ, &hKey))
	{
		OutputDebugString(L"RegGetUpdateExeVersion打开注册表项失败");
		return false;
	}
	byte bBuff[MAX_PATH] = {0};
	DWORD dwSize = 1024;
	DWORD dwType = 0;
	if ( ERROR_SUCCESS != RegQueryValueEx(hKey, L"UpdateExeVersion", NULL, &dwType,  bBuff, &dwSize) )
		return false;
	strVersion = (wchar_t*)bBuff;
	return true;
}

bool RegSetUpdateExeVersion(const wstring& strKeyName, const wstring& strVersion)
{
	CAutoRegkey hKey;
	wstring strKeyPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
	strKeyPath.append(strKeyName);
	DWORD dwRet;
	if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_LOCAL_MACHINE, strKeyPath.c_str(), 0, 
		NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwRet))
	{
		OutputDebugString(L"RegSetUpdateExeVersion创建注册表项失败");
		return false;
	}
	return ERROR_SUCCESS == RegSetValueEx(hKey, L"UpdateExeVersion", 0, REG_SZ, (byte*)strVersion.c_str(), strVersion.size()*2);
}

bool RegWriteList(const wstring& strKeyName, const vector<wstring>& comList, RegListType type)
{
	if (comList.empty())
		return true;
	CAutoRegkey hKey;
	wstring strKeyPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
	strKeyPath += strKeyName;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, strKeyPath.c_str(), 0, KEY_ALL_ACCESS, &hKey))
	{
		OutputDebugString(L"RegWriteList打开注册表项失败");
		return false;
	}
	wstring strValue;
	for ( size_t i=0; i<comList.size(); ++i )
	{
		strValue.append(comList[i]);
		strValue.append(L"*");
	}
	wstring strSubKeyName;
	switch( type )
	{
	case RegComList:
		strSubKeyName = L"ComList";
		break;
	case RegConflictList:
		strSubKeyName = L"ConflictList";
		break;
	case RegUnsetupCmdList:
		strSubKeyName = L"UnsetupCmdList";
		break;
	}
	RegSetValueEx(hKey, strSubKeyName.c_str(),	0, REG_SZ, (byte*)strValue.c_str(), strValue.size()*2);
	return true;
}

bool RegReadList(const wstring& strKeyName, vector<wstring>& comList, RegListType type)
{
	CAutoRegkey hKey;
	wstring strKeyPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
	strKeyPath += strKeyName;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, strKeyPath.c_str(), 0, KEY_READ, &hKey))
	{
		OutputDebugString(L"RegReadList打开注册表项失败");
		return false;
	}
	wstring strSubKeyName;
	switch( type )
	{
	case RegComList:
		strSubKeyName = L"ComList";
		break;
	case RegConflictList:
		strSubKeyName = L"ConflictList";
		break;
	case RegUnsetupCmdList:
		strSubKeyName = L"UnsetupCmdList";
		break;
	}
	byte bBuff[2048] = {0};
	DWORD dwSize = 2048;
	DWORD dwType = 0;
	if ( ERROR_SUCCESS != RegQueryValueEx(hKey, strSubKeyName.c_str(), NULL, &dwType,  bBuff, &dwSize) )
		return false;
	wstring strValue = (wchar_t*)bBuff;
	PublicLib::SplitStringW(strValue, L"*", comList);
	return true;
}

bool RegWritePca()
{
	CAutoRegkey hKey;
	if ( ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, 
		L"Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility Assistant\\Persisted", 0, KEY_READ|KEY_WRITE, &hKey) )
	{
		OutputDebugString(L"RegWritePca打开注册表项失败");
		return false;
	}
	wchar_t szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	DWORD dwCode = 1;
	LONG lRet = RegSetValueEx(hKey, szPath,	0, REG_DWORD, (byte*)&dwCode, sizeof(DWORD));
	return lRet == ERROR_SUCCESS;
}



