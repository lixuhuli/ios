// UUClient.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "BugReport.h"
#include "MainFrame.h"
#include <shellapi.h>
#include <ShlObj.h>
#include "SoftDefine.h"

wstring g_datFilePath;
wstring g_dumpFilePath;
wstring g_zipFilePath;
//wstring g_logFilePath;
wstring g_AppPath;
wstring g_AppVersion;
wstring g_exeType;
DumpProcessType g_dumpProcess = DptMain;


int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	if (wcslen(lpCmdLine) == 0)
		return 0;
	CPaintManagerUI::SetInstance(hInstance);
#ifdef _DEBUG
	wstring wstrResoucePath = CPaintManagerUI::GetInstancePath() + L"Skin\\BugReport";
	CPaintManagerUI::SetResourcePath(wstrResoucePath.c_str());
#else
	BYTE* pSkinBuffer = NULL;
	HRSRC hRes = ::FindResource(NULL, MAKEINTRESOURCE(IDR_DAT1), L"DAT");
	HGLOBAL hGlobal = ::LoadResource(NULL, hRes);
	DWORD dwSkinBufferSize = ::SizeofResource(NULL, hRes);
	BYTE* pResource = (BYTE*)::LockResource(hGlobal);
	CPaintManagerUI::SetResourceZip(pResource, dwSkinBufferSize);
#endif
	
	HRESULT Hr = ::CoInitialize(NULL);
	LPTSTR* CmdArgs = NULL;
	int ArgNum;
	CmdArgs = CommandLineToArgvW(lpCmdLine, &ArgNum);
	if(ArgNum >= 5)
	{
		g_datFilePath = CmdArgs[0];
		g_dumpFilePath = CmdArgs[1];
		g_AppPath = CmdArgs[2];
		g_AppVersion = CmdArgs[3];
		g_exeType = CmdArgs[4];
	}
	else
	{
		wstring strRunPath = GetRunPathW();
		g_datFilePath = strRunPath + _T("errfile.dat");
		g_dumpFilePath = strRunPath + _T("errfile.dmp");
		g_AppPath = strRunPath + EXE_MAIN;
		
		HKEY hKey = NULL;
		wstring strKey(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
		strKey.append(REG_ROOT_NAME);
		if (ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, strKey.c_str(), &hKey))
		{
			byte bBuff[20] = { 0 };
			DWORD dwSize = 20;
			DWORD dwType = 0;
			if (ERROR_SUCCESS == RegQueryValueEx(hKey, L"DisplayVersion", NULL, &dwType, bBuff, &dwSize))
				g_AppVersion = (wchar_t*)bBuff;
		}
	}
	g_zipFilePath = GetRunPathW() + _T("errfile.zip");
	CWndShadow::Initialize(hInstance);
	CMainFrame* pFrame = new CMainFrame;
	pFrame->Create(NULL);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);
	CPaintManagerUI::MessageLoop();
	::CoUninitialize();
	return 0;
}


