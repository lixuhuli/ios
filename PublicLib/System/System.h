#pragma once

namespace PublicLib{

	//获取操作系统版本信息
	bool GetOSVersion(OUT wstring& strOsName, OUT wstring& strVersion);

	//获取操作系统信息
	void SafeGetNativeSystemInfo(LPSYSTEM_INFO lpSysInfo);

	//是否是64位系统
	bool Is64bitSystem();

	bool CreateGuid(OUT wstring& strGuid);
	//获取内网IP地址
	bool GetMacAddress(std::wstring& strMac);

	bool IsOsOverVista();

	//任务栏操作
	bool PinToWin7TaskBar(const wchar_t* pDirPath, const wchar_t* pExeName);
	bool UnpinToWin7TaskBar(const wchar_t* pDirPath, const wchar_t* pExeName);

	//刷新系统图标
	void RefreshIcons();

	//已当前权限运行
	DWORD ShellExecuteRunas(const wchar_t* lpExe, const wchar_t* lpParam, const wchar_t* lpWorkDir);

	//终止线程
	void SafeKillThread(HANDLE* hThread, DWORD dwWaitTime);

	BOOL IsRunasAdmin(HANDLE hProcess);

	//设置开机启动
	bool SetBootRun(const wstring& strKey, const wstring& strValue);

	bool CopyToClipboard(const wstring& strData);

	BOOL EnableDebugPrivilege();
}