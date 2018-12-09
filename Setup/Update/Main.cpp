// Update.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Update.h"
#include "GlobalData.h"
#include "WndUpdate.h"

bool InitLog();
bool CheckSingleMutex();
void CreateUpdateWnd(bool bCheck);
bool InitDuilibRes(HINSTANCE hInstance);
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	CoInitialize(NULL);
	PublicLib::EnableDebugPrivilege();
	InitLog();
	CGlobalData::Instance()->m_strCmdLine.assign(lpCmdLine);
	bool bIsSelfUpdate = CGlobalData::Instance()->IsSelfUpdate();
	if (!bIsSelfUpdate)//不是进行自更新
	{
		if (!CheckSingleMutex())
			return 0;
	}
	CGlobalData::Instance()->Init();
// 	wstring strError;
// 	CheckUpdate(strError);
	if (!CGlobalData::Instance()->HasInstall())
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"没有找到安装信息");
		MessageBox(NULL, L"没有找到安装信息", L"出错了：", MB_OK | MB_ICONERROR);
		return 0;
	}
	if (!InitDuilibRes(hInstance))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"初始化界面库资源失败");
		MessageBox(NULL, L"初始化界面库资源失败", L"出错了：", MB_OK | MB_ICONERROR);
		return 0;
	}
	if (wcslen(lpCmdLine) == 0 || wcscmp(lpCmdLine, L"/update") == 0)//手动升级
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"进入手动升级模式");
		CreateUpdateWnd(true);
	}
	else if (wcscmp(lpCmdLine, L"/autoupdate") == 0)//自动升级
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"进入自动升级模式");
		wstring strError;
		UpdateStatus status = CheckUpdate(strError);
		OUTPUT_XYLOG(LEVEL_INFO, L"检测更新返回值：%d", (int)status);
		if (status == UpdateNew)
		{
			CreateUpdateWnd(false);
		}
	}
	else//升级程序替换
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"进入升级程序替换模式");
		Json::Value vOther;
		Json::Reader rd;
		try
		{
			Json::Value vRoot;
			string strJson = PublicLib::UToA(lpCmdLine);
			if (rd.parse(strJson, vRoot))
			{
				vOther = vRoot["other"];
				const Json::Value& vPid = vOther["pid"];
				const Json::Value& vCmd = vOther["cmd"];
				const Json::Value& vVersion = vOther["updateversion"];
				if (vPid.isString() && vCmd.isString() && vVersion.isString())
				{
					return RunNewUpdate(vPid, vCmd, vVersion);
				}
			}
		}
		catch (...)
		{

		}
	}
	CoUninitialize();
	return 0;
}


bool CheckSingleMutex()
{
	HANDLE hSingleMutex = CreateMutex(NULL, FALSE, L"PapaGameHallUpdate_Mutex");
	if (hSingleMutex)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			HWND hWnd = FindWindow(GUI_CLASS_NAME, UPDATE_WND_NAME);
			if (hWnd)
			{
				::PostMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, NULL);
				SetForegroundWindow(hWnd);
			}
			OUTPUT_XYLOG(LEVEL_INFO, L"已经有更新程序在运行");
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
	strLogPath.append(L"\\Update.log");
	SET_XYLOG_PATH(strLogPath.c_str());
	SET_XYLOG_LEVEL(LEVEL_DEBUG);
	SET_PROCESS_NAME(L"Update_Main");
	return true;
}

bool InitDuilibRes(HINSTANCE hInstance)
{
	CPaintManagerUI::SetInstance(hInstance);
	CWndShadow::Initialize(hInstance);
#ifdef _DEBUG
	wstring wstrResoucePath = CPaintManagerUI::GetInstancePath() + L"Skin\\Update";
	CPaintManagerUI::SetResourcePath(wstrResoucePath.c_str());
#else
	BYTE* pSkinBuffer = NULL;
	HRSRC hRes = ::FindResource(NULL, MAKEINTRESOURCE(IDR_DAT1), L"DAT");
	HGLOBAL hGlobal = ::LoadResource(NULL, hRes);
	DWORD dwSkinBufferSize = ::SizeofResource(NULL, hRes);
	BYTE* pResource = (BYTE*)::LockResource(hGlobal);
	CPaintManagerUI::SetResourceZip(pResource, dwSkinBufferSize);
#endif
	return true;
}

void CreateUpdateWnd(bool bCheck)
{
	CWndUpdate* pWnd = new CWndUpdate(bCheck);
	pWnd->Create(NULL);
	pWnd->CenterWindow();
	pWnd->ShowWindow();
	CPaintManagerUI::MessageLoop();
}