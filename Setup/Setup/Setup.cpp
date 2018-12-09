// Setup.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Setup.h"
#include "Regedit.h"
#include "SetupModule.h"
#include "WndSetup.h"
#include <DataCenter\DataCenterApi.h>

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
	CoInitialize(NULL);
	PublicLib::EnableDebugPrivilege();
	RegWritePca();
	InitLog();
	if (!CheckSingleMutex())
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"检测到已经有一个安装程序正在运行");
		return 0;
	}
	if (!InitDuilibRes(hInstance))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"初始化界面库资源失败");
		MessageBox(NULL, L"初始化界面库资源失败", L"卸载：", MB_OK | MB_ICONERROR);
		return 0;
	}
	InitDataCenter(NULL);
	CSetupModule::Instance()->Init(hInstance);
	CWndSetup* pWnd = new CWndSetup;
	pWnd->Create(NULL);
	pWnd->CenterWindow();
	pWnd->ShowWindow();
	CPaintManagerUI::MessageLoop();
	ExitDataCenter();
	CoUninitialize();
	return 0;
}


bool CheckSingleMutex()
{
	HANDLE hSingleMutex = CreateMutex(NULL, FALSE, L"PapaGameHallSetup_Mutex");
	if (hSingleMutex)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			HWND hWnd = FindWindow(GUI_CLASS_NAME, SETUP_WND_NAME);
			if (hWnd)
			{
				::PostMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, NULL);
				SetForegroundWindow(hWnd);
			}
			OUTPUT_XYLOG(LEVEL_INFO, L"已经有安装程序在运行");
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
	strLogPath.append(L"\\Setup.log");
	SET_XYLOG_PATH(strLogPath.c_str());
	SET_XYLOG_LEVEL(LEVEL_DEBUG);
	SET_PROCESS_NAME(L"Setup_Main");
	return true;
}

bool InitDuilibRes(HINSTANCE hInstance)
{
	CPaintManagerUI::SetInstance(hInstance);
	CWndShadow::Initialize(hInstance);
#ifdef _DEBUG
	wstring wstrResoucePath = CPaintManagerUI::GetInstancePath() + L"Debug\\Skin\\Setup";
	CPaintManagerUI::SetResourcePath(wstrResoucePath.c_str());
#else
	BYTE* pSkinBuffer = NULL;
	HRSRC hRes = ::FindResource(NULL, MAKEINTRESOURCE(IDR_DAT2), L"DAT");
	HGLOBAL hGlobal = ::LoadResource(NULL, hRes);
	DWORD dwSkinBufferSize = ::SizeofResource(NULL, hRes);
	BYTE* pResource = (BYTE*)::LockResource(hGlobal);
	CPaintManagerUI::SetResourceZip(pResource, dwSkinBufferSize);
	//CPaintManagerUI::SetResourceZipBaseDepth(g_wstrSkinFolder.c_str());
#endif
	return true;
}