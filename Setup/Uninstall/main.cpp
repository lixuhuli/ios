// Uninstall.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Uninstall.h"
#include "Regedit.h"
#include "UnSetupModule.h"
#include "WndUninstall.h"
#include <DataCenter\DataCenterApi.h>

#define CMD_TEMP_UNINSTALL	L"/tempuninstall"
#define CMD_UNINSTALL		L"/unsetup"

bool InitLog();
bool CheckSingleMutex();
bool InitDuilibRes(HINSTANCE hInstance);
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	PublicLib::EnableDebugPrivilege();
	RegWritePca();
	InitLog();
	if (!CheckSingleMutex())
	{
		return 0;
	}
	if (!CUnSetupModule::Instance()->GetInstallInfo())
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"没有获取到安装信息，卸载程序退出");
		MessageBox(NULL, L"没有获取到安装信息", L"卸载：", MB_OK | MB_ICONERROR);
		return 0;
	}
	if (!InitDuilibRes(hInstance))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"初始化界面库资源失败");
		MessageBox(NULL, L"初始化界面库资源失败", L"卸载：", MB_OK | MB_ICONERROR);
		return 0;
	}
	//解析命令行
	if (wcslen(lpCmdLine) == 0 || wcscmp(lpCmdLine, CMD_UNINSTALL) == 0)
	{
		wchar_t szTemp[60];
		wcscpy_s(szTemp, L"5funUninstallXXXXXX");
		wchar_t* pTempName = _wmktemp(szTemp);
		if (NULL == pTempName)
			pTempName = L"5funGameHallUninstallTemp";
		wchar_t szTempPath[MAX_PATH], szOldPath[MAX_PATH];
		GetTempPath(MAX_PATH, szTempPath);
		wstring strTemp(szTempPath);
		wcscat_s(szTempPath, pTempName);
		wcscat_s(szTempPath, L".exe");
		GetModuleFileName(NULL, szOldPath, MAX_PATH);
		if (!CopyFile(szOldPath, szTempPath, FALSE))
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"复制文件，从：%s到：%s失败", szOldPath, szTempPath);
			return 0;
		}
		PublicLib::ShellExecuteRunas(szTempPath, CMD_TEMP_UNINSTALL, strTemp.c_str());
		//ShellExecute(NULL, L"open", szTempPath, CMD_TEMP_UNINSTALL, strTemp.c_str(), SW_SHOWNORMAL);
		return 0;
	}
	if (wcscmp(lpCmdLine, CMD_TEMP_UNINSTALL) == 0)
	{
		InitDataCenter(NULL);
		CWndUninstall* pWnd = new CWndUninstall;
		pWnd->Create(NULL);
		pWnd->CenterWindow();
		pWnd->ShowWindow();
		CPaintManagerUI::MessageLoop();
		ExitDataCenter();
	}
	return 0;
}

bool CheckSingleMutex()
{
	HANDLE hSingleMutex = CreateMutex(NULL, FALSE, L"PapaGameHallUninst_Mutex");
	if (hSingleMutex)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			HWND hWnd = FindWindow(GUI_CLASS_NAME, UNINSTALL_WND_NAME);
			if (hWnd)
			{
				::PostMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, NULL);
				SetForegroundWindow(hWnd);
			}
			OUTPUT_XYLOG(LEVEL_INFO, L"已经有卸载程序在运行");
			return false;
		}
		return true;
	}
	return false;
}

bool InitLog()
{
	//初始化XYClient日志
	wstring strLogPath = GetDocumentPath() + L"\\log";
	SHCreateDirectory(NULL, strLogPath.c_str());
	strLogPath.append(L"\\Unsetup.log");
	SET_XYLOG_PATH(strLogPath.c_str());
	SET_XYLOG_LEVEL(LEVEL_DEBUG);
	SET_PROCESS_NAME(L"Unsetup_Main");
	return true;
}

bool InitDuilibRes(HINSTANCE hInstance)
{
	CPaintManagerUI::SetInstance(hInstance);
	CWndShadow::Initialize(hInstance);
#ifdef _DEBUG
	wstring wstrResoucePath = CPaintManagerUI::GetInstancePath() + L"Skin\\Unsetup";
	CPaintManagerUI::SetResourcePath(wstrResoucePath.c_str());
#else
	BYTE* pSkinBuffer = NULL;
	HRSRC hRes = ::FindResource(NULL, MAKEINTRESOURCE(IDR_DAT1), L"DAT");
	HGLOBAL hGlobal = ::LoadResource(NULL, hRes);
	DWORD dwSkinBufferSize = ::SizeofResource(NULL, hRes);
	BYTE* pResource = (BYTE*)::LockResource(hGlobal);
	CPaintManagerUI::SetResourceZip(pResource, dwSkinBufferSize);
	//CPaintManagerUI::SetResourceZipBaseDepth(g_wstrSkinFolder.c_str());
#endif
	return true;
}
