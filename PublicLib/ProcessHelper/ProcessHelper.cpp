#include "StdAfx.h"
#include "ProcessHelper.h"
#include <WinSock2.h>
#include <Tlhelp32.h>
#include <Psapi.h>
#pragma comment(lib, "psapi")
#include <IPHlpApi.h>
#pragma comment(lib, "iphlpapi")
#pragma comment(lib, "ws2_32")
#include <tchar.h>

namespace PublicLib{

	typedef struct _TCP_RAW
	{
		DWORD dwState;
		DWORD dwLocalAddr;
		DWORD DwLocalPort;
		DWORD dwRemoteAddr;
		DWORD dwRemotePort;
		DWORD dwProcessId;
	}TCP_RAW, *LPTCP_RAW;

	typedef struct _TCP_RAW_TABLE
	{
		DWORD dwNumEntries;
		TCP_RAW table[ANY_SIZE];
	}TCP_RAW_TABLE, *LPTCP_RAW_TABLE;



	HANDLE GetProcessById(DWORD dwPid)
	{
		return ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	}

	HANDLE GetProcessByWnd(HWND hWnd)
	{
		if (!::IsWindow(hWnd))
			return NULL;
		DWORD dwPid;
		DWORD dwTid = ::GetWindowThreadProcessId(hWnd, &dwPid);
		return GetProcessById(dwPid);
	}

	DLL_EXPORT	DWORD GetPidByName(LPCTSTR lpName)
	{
		HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hSnapshot)
			return NULL;
		PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
		BOOL bFlag = ::Process32First(hSnapshot, &pe);
		while (bFlag)
		{
			if (_tcsicmp(lpName, pe.szExeFile) == 0)
			{
				::CloseHandle(hSnapshot);
				return pe.th32ProcessID;
			}
			bFlag = ::Process32Next(hSnapshot, &pe);
		}
		::CloseHandle(hSnapshot);
		return 0;
	}

	HANDLE GetProcessByName(LPCTSTR lpName)
	{
		DWORD dwPid = GetPidByName(lpName);
		if (dwPid == 0)
			return NULL;
		return GetProcessById(dwPid);
	}

	HANDLE GetProcessByPort(WORD wPort)
	{
		DWORD dwSize = sizeof(TCP_RAW_TABLE);
		LPTCP_RAW_TABLE lpData = (LPTCP_RAW_TABLE)malloc(dwSize);
		DWORD dwRet = ::GetExtendedTcpTable((PVOID)lpData, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
		if (ERROR_INVALID_PARAMETER == dwRet)
			return NULL;
		if (ERROR_INSUFFICIENT_BUFFER == dwRet)
		{
			free(lpData);
			lpData = (LPTCP_RAW_TABLE)malloc(dwSize);
			if (NO_ERROR != ::GetExtendedTcpTable((PVOID)lpData, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0))
				return NULL;
		}
		for (DWORD i = 0; i < lpData->dwNumEntries; ++i)
		{
			TCP_RAW& tcp = lpData->table[i];
			WORD port = htons((u_short)tcp.DwLocalPort);
			wstring strName = GetProcessRunPath(tcp.dwProcessId);
			if (wPort == port)
				return GetProcessById(tcp.dwProcessId);
		}
		return NULL;
	}

	BOOL KillProcessById(DWORD dwPid)
	{
		HANDLE hProcess = GetProcessById(dwPid);
		if (NULL != hProcess)
		{
			DWORD dwCode;
			::GetExitCodeProcess(hProcess, &dwCode);
			BOOL bRet = ::TerminateProcess(hProcess, dwCode);
			::WaitForSingleObject(hProcess, 3000);
			::CloseHandle(hProcess);
			return bRet;
		}
		return FALSE;
	}

	BOOL KillProcessByHandle(HANDLE hProcess)
	{
		if (NULL != hProcess)
		{
			DWORD dwCode;
			::GetExitCodeProcess(hProcess, &dwCode);
			::TerminateProcess(hProcess, dwCode);
			::WaitForSingleObject(hProcess, 3000);
			::CloseHandle(hProcess);
			return TRUE;
		}
		return FALSE;
	}

	wstring GetProcessRunPath(DWORD dwPid)
	{
		wstring strName;
		HANDLE hProcess = GetProcessById(dwPid);
		if (hProcess)
		{
			wchar_t szPath[MAX_PATH + 1] = { 0 };
			::GetModuleFileNameEx(hProcess, NULL, szPath, MAX_PATH);
			strName.append(szPath);
			::CloseHandle(hProcess);
		}
		return strName;
	}

	wstring GetProcessExeName(DWORD dwPid)
	{
		wstring strName;
		HANDLE hProcess = GetProcessById(dwPid);
		if (hProcess)
		{
			wchar_t szPath[MAX_PATH + 1] = { 0 };
			::GetModuleBaseName(hProcess, NULL, szPath, MAX_PATH);//获取进程名
			strName.append(szPath);
			::CloseHandle(hProcess);
		}
		return strName;
	}
	/*
	Windows 2000 = GetModuleFileNameEx()
	Windows XP x32 = GetProcessImageFileName()
	Windows XP x64 = GetProcessImageFileName()
	Windows Vista = QueryFullProcessImageName()
	Windows 7 = QueryFullProcessImageName()
	*/

	BOOL EnablePriv()
	{
		HANDLE hToken;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
		{
			TOKEN_PRIVILEGES tkp;
			LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);//修改进程权限
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL);//通知系统修改进程权限
			return((GetLastError() == ERROR_SUCCESS));
		}
		return TRUE;
	}

	HWND GetWndByProcess(HANDLE hProcess)
	{
		ENUM_PARAM param;
		param.dwProcessId = ::GetProcessId(hProcess);
		param.hWnd = NULL;
		::EnumWindows(EnumWindowsProc, (LPARAM)&param);
		//::EnumThreadWindows(0, EnumThreadWndProc, (LPARAM)&param);
		return param.hWnd;
	}

	BOOL IsSystemProcess(HANDLE hProcess)
	{
		HANDLE hToken = NULL;
		if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
			return FALSE;
		DWORD dwLength;
		GetTokenInformation(hToken, TokenUser, NULL, 0, &dwLength);
		if (dwLength < 1)
			return FALSE;
		PTOKEN_USER pTokenUser = (PTOKEN_USER)GlobalAlloc(GPTR, dwLength);
		GetTokenInformation(hToken, TokenUser, pTokenUser, dwLength, &dwLength);
		SID_NAME_USE name = SidTypeUnknown;
		DWORD dwAccName = 0, dwDomainName = 0;
		PSID psId = pTokenUser->User.Sid;
		LookupAccountSid(NULL, psId, NULL, &dwAccName, NULL, &dwDomainName, &name);
		wchar_t szAccName[MAX_PATH + 1] = { 0 }, szDomainName[MAX_PATH + 1] = { 0 };
		LookupAccountSid(NULL, psId, szAccName, &dwAccName, szDomainName, &dwDomainName, &name);
		GlobalFree(pTokenUser);
		CloseHandle(hToken);
		if (wcscmp(szAccName, L"SYSTEM") == 0
			|| wcscmp(szAccName, L"LOCAL SERVICE") == 0
			|| wcscmp(szAccName, L"NETWORK SERVICE") == 0)
			return TRUE;
		return FALSE;

	}

	BOOL GetProcessList(vector<wstring>& processList)
	{
		HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hSnapshot)
			return FALSE;
		processList.clear();
		processList.reserve(100);
		PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
		BOOL bFlag = ::Process32First(hSnapshot, &pe);
		while (bFlag)
		{
			HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
			if (hProcess)
			{
				if (!IsSystemProcess(hProcess))
					processList.push_back(pe.szExeFile);
				CloseHandle(hProcess);
			}
			bFlag = ::Process32Next(hSnapshot, &pe);
		}
		::CloseHandle(hSnapshot);
		return TRUE;
	}

	BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
	{
		DWORD dwProcessId;
		::GetWindowThreadProcessId(hwnd, &dwProcessId);
		ENUM_PARAM* pParam = (ENUM_PARAM*)lParam;
		if (pParam->dwProcessId == dwProcessId)
		{
			if (::GetParent(hwnd) == NULL)
			{
				pParam->hWnd = hwnd;
				return FALSE;//终止遍历
			}
		}
		return TRUE;//继续遍历
	}

	BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam)
	{
		DWORD dwProcessId;
		DWORD dwThreadId = ::GetWindowThreadProcessId(hwnd, &dwProcessId);
		ENUM_PARAM* pParam = (ENUM_PARAM*)lParam;
		if (pParam->dwProcessId == dwProcessId)
		{
			pParam->hWnd = hwnd;
			return FALSE;//终止遍历
		}
		return TRUE;//继续遍历
	}

	DLL_EXPORT	HANDLE GetTokenByPid(DWORD dwPid)
	{
		HANDLE hRetToken = NULL;
		HANDLE hProcess = NULL;
		HANDLE hToken = NULL;
		PSECURITY_DESCRIPTOR pSD = NULL;
		try
		{
			hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
			if (NULL == hProcess)
				throw L"";
			if (!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
				throw L"";
			pSD = (PSECURITY_DESCRIPTOR)GlobalAlloc(GPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
			if (pSD == NULL)
				throw L"";
			if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
				throw L"";
			if (!SetSecurityDescriptorDacl(pSD, TRUE, (PACL)NULL, FALSE))
				throw L"";
			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(sa);
			sa.lpSecurityDescriptor = pSD;
			sa.bInheritHandle = TRUE;
			if (!DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, &sa, \
				SecurityImpersonation, TokenPrimary, &hRetToken))
				throw L"";
		}
		catch (const TCHAR* pMsg)
		{

		}
		catch (...)
		{

		}
		if (hToken)
			CloseHandle(hToken);
		if (hProcess)
			CloseHandle(hProcess);
		if (pSD)
			GlobalFree(pSD);
		return hRetToken;
	}

	DLL_EXPORT	BOOL RunExeAsUser(LPCTSTR lpExePath, LPTSTR lpCmd)
	{
		DWORD dwPid = GetPidByName(_T("explorer.exe"));
		if (dwPid == 0)
			return FALSE;
		HANDLE hToken = GetTokenByPid(dwPid);
		if (hToken == NULL)
			return FALSE;
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION pi;
		if (CreateProcessAsUser(hToken, lpExePath, lpCmd, NULL, NULL, \
			FALSE, 0, NULL, NULL, &si, &pi))
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			CloseHandle(hToken);
			return TRUE;
		}
		CloseHandle(hToken);
		return FALSE;
	}

	DLL_EXPORT  BOOL KillProcessByName(LPCTSTR lpName)
	{
		HANDLE hProcess = GetProcessByName(lpName);
		if (hProcess)
			KillProcessByHandle(hProcess);
		return TRUE;
	}

	BOOL KillProcessFromList(const vector<wstring>& killList)
	{
		BOOL bRet = FALSE;
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hSnapshot)
			return bRet;
		PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
		BOOL bFind = Process32First(hSnapshot, &pe);
		DWORD dwSelfPid = GetCurrentProcessId();
		while (bFind)
		{
			for (size_t i = 0; i < killList.size(); ++i)
			{
				if (wcsicmp(pe.szExeFile, killList[i].c_str()) == 0)
				{
					if (dwSelfPid == pe.th32ProcessID)
						continue;//禁止自杀
					bRet = TRUE;
					HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
					if (NULL == hProcess)
					{
						OutputDebugString(L"KillProcessFromList获取进程句柄失败！");
						continue;
					}
					TerminateProcess(hProcess, 0);
					WaitForSingleObject(hProcess, 3000);
					CloseHandle(hProcess);
				}
			}
			bFind = Process32Next(hSnapshot, &pe);
		}
		CloseHandle(hSnapshot);
		return bRet;
	}

}