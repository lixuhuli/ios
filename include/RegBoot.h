#pragma once



inline bool AddBootRun(const wstring& strRegKey, const wstring& strCmd)
{
	wstring strKeyPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	HKEY hKey = NULL;//HKEY_LOCAL_MACHINE  HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, strKeyPath.c_str(), 0, KEY_ALL_ACCESS, &hKey))
	{
		return false;
	}
	bool bRet = (ERROR_SUCCESS == RegSetValueEx(hKey, strRegKey.c_str(), 0, REG_SZ, (byte*)strCmd.c_str(), strCmd.size() * 2));
	RegCloseKey(hKey);
	return bRet;
}

inline bool DelBootRun(const wstring& strRegKey)
{
	wstring strKeyPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	HKEY hKey = NULL;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, strKeyPath.c_str(), 0, KEY_ALL_ACCESS, &hKey))
		return false;
	return ERROR_SUCCESS == RegDeleteValue(hKey, strRegKey.c_str());
}

inline bool IsBootRun(const wstring& strRegKey)
{
	wstring strKeyPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	HKEY hKey = NULL;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, strKeyPath.c_str(), 0, KEY_READ, &hKey))
		return false;
	byte bBuff[MAX_PATH] = { 0 };
	DWORD dwSize = MAX_PATH;
	DWORD dwType = 0;
	if (ERROR_SUCCESS != RegQueryValueEx(hKey, strRegKey.c_str(), NULL, &dwType, bBuff, &dwSize))
	{
		RegCloseKey(hKey);
		return false;
	}
	return true;
}