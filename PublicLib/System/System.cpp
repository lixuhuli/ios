#include "stdafx.h"
#include "System.h"
#include <ShlObj.h>
#include <ObjBase.h>
#include <atlbase.h>
#include <IPHlpApi.h>
#include <shellapi.h>
#include "VersionHelper.h"
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Version.lib")


namespace PublicLib{

	wstring GetKernel32Version()
	{
		const wchar_t* pDllName = L"kernel32.dll";
		wstring strVersion;
		DWORD dwHandle;
		DWORD VerInfoSize = GetFileVersionInfoSize(pDllName, &dwHandle);
		if (VerInfoSize)
		{
			wchar_t* VerInfo = new wchar_t[VerInfoSize / sizeof(wchar_t)];
			if (GetFileVersionInfo(pDllName, NULL, VerInfoSize, VerInfo))
			{
				VS_FIXEDFILEINFO* pFileInfo;
				UINT FileInfoLength;
				if (VerQueryValue(VerInfo, L"\\", (LPVOID*)&pFileInfo, &FileInfoLength))
				{
					wchar_t szVersion[100] = { 0 };
					swprintf_s(szVersion, L"%d.%d.%d.%d", HIWORD(pFileInfo->dwProductVersionMS),
						LOWORD(pFileInfo->dwProductVersionMS),
						HIWORD(pFileInfo->dwProductVersionLS),
						LOWORD(pFileInfo->dwProductVersionLS));
					strVersion = szVersion;
				}
			}
			delete[] VerInfo;
		}
		return strVersion;
	}

	bool GetOSVersion(OUT wstring& strOsName, OUT wstring& strVersion)
	{
		strVersion = GetKernel32Version();
		SYSTEM_INFO SysInfo;
		GetSystemInfo(&SysInfo);
		OSVERSIONINFOEX osVersinInfo = { sizeof(OSVERSIONINFOEX) };
		if (!GetVersionEx((OSVERSIONINFO*)&osVersinInfo))
			return false;
		switch (osVersinInfo.dwMajorVersion)
		{
		case 4:
			switch (osVersinInfo.dwMinorVersion)
			{
			case 0:
				if (osVersinInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
					strOsName = L"Windows NT 4.0";
				else if (osVersinInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
					strOsName = L"Windows 95";
				break;
			case 10:
				strOsName = L"Windows 98";
				break;
			case 90:
				strOsName = L"Windows Me";
				break;
			}
			break;
		case 5:
			switch (osVersinInfo.dwMinorVersion)
			{
			case 0:
				strOsName = L"Windows 2000";
				break;
			case 1:
				strOsName = L"Windows XP";
				break;
			case 2:
				if (osVersinInfo.wProductType == VER_NT_WORKSTATION && SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					strOsName = L"Windows XP Professional x64 Edition";
				else if (GetSystemMetrics(SM_SERVERR2) == 0)
					strOsName = L"Windows Server 2003";
				else if (GetSystemMetrics(SM_SERVERR2) != 0)
					strOsName = L"Windows Server 2003 R2";
				break;
			}
			break;
		case 6:
			switch (osVersinInfo.dwMinorVersion)
			{
			case 0:
				if (osVersinInfo.wProductType == VER_NT_WORKSTATION)
					strOsName = L"Windows Vista";
				else
					strOsName = L"Windows Server 2008";
				break;
			case 1:
				if (osVersinInfo.wProductType == VER_NT_WORKSTATION)
					strOsName = L"Windows 7";
				else
					strOsName = L"Windows Server 2008 R2";
				break;
			case 2:
				{
					CVersionHelper ver(strVersion);
					switch (ver.m_nMajorNumber)
					{
					case 8: strOsName = L"Windows 8"; break;
					case 10: strOsName = L"Windows 10"; break;
					default: strOsName = L"Unknow system"; break;
					}
				}
			}
			break;
		default:
			strOsName = L"Unknow system";
		}
		if (wcslen(osVersinInfo.szCSDVersion) != 0)
		{
			strOsName.append(L" ");
			strOsName.append(osVersinInfo.szCSDVersion);
		}
		return true;;
	}

	void SafeGetNativeSystemInfo(LPSYSTEM_INFO lpSysInfo)
	{
		if (NULL == lpSysInfo)
			return;
		typedef VOID(WINAPI *pfnGetNativeSystemInfo)(LPSYSTEM_INFO);
		pfnGetNativeSystemInfo pFun = (pfnGetNativeSystemInfo)GetProcAddress(GetModuleHandle(L"Kernel32"), "GetNativeSystemInfo");
		if (pFun)
			pFun(lpSysInfo);
		else
			GetSystemInfo(lpSysInfo);
	}

	bool Is64bitSystem()
	{
		SYSTEM_INFO si;
		SafeGetNativeSystemInfo(&si);
		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64
			|| si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
			return true;
		return false;
	}

	bool CreateGuid(OUT wstring& strGuid)
	{
		GUID id;
		WCHAR buff[40] = { '\0' };
		CHAR buff2[40] = { '\0' };
		if (FAILED(CoCreateGuid(&id)))
			return false;
		StringFromGUID2(id, buff, 40);
		strGuid.assign(buff);
		strGuid = strGuid.substr(1, strGuid.size() - 2);
		return true;
	}

	bool GetMacAddress(std::wstring& strMac)
	{
		PIP_ADAPTER_INFO pAdapterInfo;
		PIP_ADAPTER_INFO pAdapter = NULL;
		ULONG dwOutBuffLen = sizeof(IP_ADAPTER_INFO);
		DWORD dwRet = 0;
		pAdapterInfo = (PIP_ADAPTER_INFO)malloc(dwOutBuffLen);
		if (GetAdaptersInfo(pAdapterInfo, &dwOutBuffLen) == ERROR_BUFFER_OVERFLOW)
		{
			free(pAdapterInfo);
			pAdapterInfo = (PIP_ADAPTER_INFO)malloc(dwOutBuffLen);
			if (NULL == pAdapterInfo)
				return false;
		}
		if (NO_ERROR != GetAdaptersInfo(pAdapterInfo, &dwOutBuffLen))
		{
			free(pAdapterInfo);
			return false;
		}
		//只要一个就行了
		pAdapter = pAdapterInfo;
		for (size_t i = 0; i < pAdapter->AddressLength; ++i)
		{
			wchar_t szTemp[10] = { 0 };
			swprintf_s(szTemp, L"%.2x", (int)pAdapter->Address[i]);
			strMac.append(szTemp);
		}
		free(pAdapterInfo);
		return true;
	}

	bool IsOsOverVista()
	{
		SYSTEM_INFO SysInfo;
		GetSystemInfo(&SysInfo);
		OSVERSIONINFOEX osVersinInfo = { sizeof(OSVERSIONINFOEX) };
		if (!GetVersionEx((OSVERSIONINFO*)&osVersinInfo))
			return false;
		return osVersinInfo.dwMajorVersion >= 6;
	}

	static const wchar_t* pPinKey1 = L"Pin to Taskbar";
	static const wchar_t* pPinKey2 = L"锁定到任务栏";

	bool PinToWin7TaskBar(const wchar_t* pDirPath, const wchar_t* pExeName)
	{
		CComPtr<IShellDispatch> pShellDisp;
		CComPtr<Folder> folder_ptr;
		CComPtr<FolderItem> folder_item_ptr;
		CComPtr<FolderItemVerbs> folder_item_verbs_ptr;
		HRESULT hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_SERVER, IID_IDispatch, (LPVOID *)&pShellDisp);
		if (FAILED(hr) || (NULL == pShellDisp)) return false;
		hr = pShellDisp->NameSpace(CComVariant(pDirPath), &folder_ptr);
		if (FAILED(hr) || (NULL == folder_ptr)) return false;
		hr = folder_ptr->ParseName(CComBSTR(pExeName), &folder_item_ptr);
		if (FAILED(hr) || (NULL == folder_item_ptr)) return false;
		hr = folder_item_ptr->Verbs(&folder_item_verbs_ptr);
		if (FAILED(hr) || (NULL == folder_item_verbs_ptr)) return false;
		long count = 0;
		folder_item_verbs_ptr->get_Count(&count);
		for (long i = 0; i < count; ++i)
		{
			CComPtr<FolderItemVerb> item_verb;
			CComVariant varIndex(i);
			hr = folder_item_verbs_ptr->Item(varIndex, &item_verb);
			if (FAILED(hr) || (NULL == item_verb))
				continue;
			CComBSTR name;
			item_verb->get_Name(&name);
			if (_wcsnicmp(name, pPinKey1, wcslen(pPinKey1)) == 0
				|| _wcsnicmp(name, pPinKey2, wcslen(pPinKey2)) == 0)
			{
				item_verb->DoIt();
				return true;
			}
		}
		return false;
	}

	static const wchar_t* pUnPinKey1 = L"Unpin to Taskbar";
	static const wchar_t* pUnPinKey2 = L"从任务栏脱离(&K)";
	bool UnpinToWin7TaskBar(const wchar_t* pDirPath, const wchar_t* pExeName)
	{
		CComPtr<IShellDispatch> pShellDisp;
		CComPtr<Folder> folder_ptr;
		CComPtr<FolderItem> folder_item_ptr;
		CComPtr<FolderItemVerbs> folder_item_verbs_ptr;
		HRESULT hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_SERVER, IID_IDispatch, (LPVOID *)&pShellDisp);
		if (FAILED(hr) || (NULL == pShellDisp)) return false;
		hr = pShellDisp->NameSpace(CComVariant(pDirPath), &folder_ptr);
		if (FAILED(hr) || (NULL == folder_ptr)) return false;
		hr = folder_ptr->ParseName(CComBSTR(pExeName), &folder_item_ptr);
		if (FAILED(hr) || (NULL == folder_item_ptr)) return false;
		hr = folder_item_ptr->Verbs(&folder_item_verbs_ptr);
		if (FAILED(hr) || (NULL == folder_item_verbs_ptr)) return false;
		long count = 0;
		folder_item_verbs_ptr->get_Count(&count);
		for (long i = 0; i < count; ++i)
		{
			CComPtr<FolderItemVerb> item_verb;
			hr = folder_item_verbs_ptr->Item(CComVariant(i), &item_verb);
			if (FAILED(hr))
				continue;
			CComBSTR name;
			item_verb->get_Name(&name);
			if (_wcsnicmp(name, pUnPinKey1, wcslen(pUnPinKey1)) == 0
				|| _wcsnicmp(name, pUnPinKey2, wcslen(pUnPinKey2)) == 0)
			{
				item_verb->DoIt();
				return true;
			}
		}
		return false;
	}

	void RefreshIcons()
	{
		// 	int nSize = GetSystemMetrics(SM_CXICON); 
		// 	HKEY   hKey; 
		// 	RegOpenKeyEx(HKEY_CURRENT_USER, L"Control Panel\\Desktop\\WindowMetrics", 0, KEY_ALL_ACCESS, &hKey); 
		// 	wchar_t  szBuff[20]; 
		// 	swprintf(szBuff, L"%d", nSize-1);
		// 	RegSetValueEx(hKey, L"Shell Icon Size", 0, REG_SZ, (unsigned char*)szBuff, wcslen(szBuff)*2); 
		// 	PostMessage(HWND_BROADCAST, WM_WININICHANGE, 0, 0); 
		// 	swprintf(szBuff, L"%d", nSize);
		// 	RegSetValueEx(hKey, L"Shell Icon Size", 0, REG_SZ,(unsigned char*)szBuff, wcslen(szBuff)*2); 
		// 	PostMessage(HWND_BROADCAST, WM_WININICHANGE, 0, 0);
		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	}

	DWORD ShellExecuteRunas(const wchar_t* lpExe, const wchar_t* lpParam, const wchar_t* lpWorkDir)
	{
		if (IsOsOverVista())
		{
			SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
			sei.lpVerb = TEXT("runas");
			sei.lpFile = lpExe;
			sei.lpParameters = lpParam;
			sei.lpDirectory = lpWorkDir;
			sei.nShow = SW_SHOWNORMAL;
			ShellExecuteEx(&sei);
		}
		else
		{
			HINSTANCE hInst = ShellExecute(NULL, L"open", lpExe, lpParam, NULL, SW_SHOWNORMAL);
			if ((UINT)hInst > 32)
				return 0;
		}
		return GetLastError();
	}

	void SafeKillThread(HANDLE* hThread, DWORD dwWaitTime)
	{
		if (NULL == *hThread)
			return;
		DWORD dwCode;
		if (GetExitCodeThread(*hThread, &dwCode) && (STILL_ACTIVE == dwCode))
		{
			if (dwWaitTime > 0)
				WaitForSingleObject(*hThread, dwWaitTime);
			TerminateThread(*hThread, 555);
		}
		CloseHandle(*hThread);
		*hThread = NULL;
	}

	BOOL IsRunasAdmin(HANDLE hProcess)
	{
		BOOL bElevated = FALSE;
		HANDLE hToken = NULL;

		if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
		{
			return FALSE;
		}
		TOKEN_ELEVATION tokenEle;
		DWORD dwRetLen = 0;
		// Retrieve token elevation information
		if (GetTokenInformation(hToken, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen))
		{
			if (dwRetLen == sizeof(tokenEle))
			{
				bElevated = tokenEle.TokenIsElevated;
			}
		}
		CloseHandle(hToken);
		return bElevated;
	}

	bool SetBootRun(const wstring& strKey, const wstring& strValue)
	{
		HKEY hKey = NULL;
		if (ERROR_SUCCESS != RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey))
			return false;
		bool bRet = (ERROR_SUCCESS == RegSetValueEx(hKey, strKey.c_str(), 0, REG_SZ, (byte*)strValue.c_str(), strValue.size() * 2));
		RegCloseKey(hKey);
		return bRet;
	}

	bool CopyToClipboard(const wstring& strData)
	{
		if (strData.empty())
			return false;
		if (!OpenClipboard(NULL))
		{
			DWORD dwError = GetLastError();
			return false;//打开剪切板失败
		}
		EmptyClipboard();
		SIZE_T nSize = strData.size() + 1;
		HGLOBAL hClip = GlobalAlloc(GMEM_DDESHARE, nSize * sizeof(wchar_t));
		PTSTR pszBuf = (PTSTR)GlobalLock(hClip);
		wcscpy_s(pszBuf, nSize, strData.c_str());
		GlobalUnlock(hClip);
		SetClipboardData(CF_UNICODETEXT, hClip);
		CloseClipboard();
		return true;
	}

	BOOL EnableDebugPrivilege()
	{
		BOOL fOK = FALSE;     //Assume function fails
		HANDLE hToken;
		//Try to open this process's acess token
		HANDLE hProcess = GetCurrentProcess();
		if (OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken))
		{
			//Attempt to modify the "Debug" privilege
			TOKEN_PRIVILEGES tp;
			tp.PrivilegeCount = 1;
			LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
			fOK = (GetLastError() == ERROR_SUCCESS);
			CloseHandle(hToken);
		}
		CloseHandle(hProcess);
		return fOK;
	}
}