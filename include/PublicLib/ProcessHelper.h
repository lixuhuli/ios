/******************************************
/*进程操作的功能函数
/*Jelin
/*2014年10月11日10:28:35
/******************************************/
#pragma once
#include <string>
#include <vector>
using std::string;
using std::wstring;
using std::vector;
#define DLL_EXPORT
#define DLL_EXPORT_CPP
namespace PublicLib
{

	DLL_EXPORT	HANDLE	GetProcessById(DWORD dwPid);

	DLL_EXPORT	HANDLE	GetProcessByWnd(HWND hWnd);

	DLL_EXPORT	DWORD	GetPidByName(LPCTSTR lpName);

	DLL_EXPORT	HANDLE	GetProcessByName(LPCTSTR lpName);

	DLL_EXPORT	HANDLE	GetProcessByPort(WORD wPort);

	DLL_EXPORT	BOOL	KillProcessById(DWORD dwPid);

	DLL_EXPORT	BOOL	KillProcessByHandle(HANDLE hProcess);

	DLL_EXPORT  BOOL	KillProcessByName(LPCTSTR lpName);

	DLL_EXPORT_CPP		wstring	GetProcessRunPath(DWORD dwPid);

	DLL_EXPORT_CPP		wstring	GetProcessExeName(DWORD dwPid);

	DLL_EXPORT	HWND	GetWndByProcess(HANDLE hProcess);

	DLL_EXPORT	BOOL	EnablePriv();

	DLL_EXPORT	BOOL	IsSystemProcess(HANDLE hProcess);

	DLL_EXPORT_CPP	BOOL	GetProcessList(vector<wstring>& processList);

	DLL_EXPORT	HANDLE	GetTokenByPid(DWORD dwPid);

	DLL_EXPORT	BOOL	RunExeAsUser(LPCTSTR lpExePath, LPTSTR lpCmd);

	struct ENUM_PARAM
	{
		DWORD	dwProcessId;
		HWND	hWnd;
	};

	BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

	BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam);

	BOOL KillProcessFromList(const vector<wstring>& killList);
}