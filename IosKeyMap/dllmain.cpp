// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <Shlobj.h>
#include "GlobalData.h"

bool InitLog();//初始化日志系统
bool InitDuilibRes(HINSTANCE hInstance); // 初始化界面库资源
bool InitModules(HINSTANCE hInstance);   // 初始化其他模块
void ExitModules();

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call){
    case DLL_PROCESS_DETACH:
        CPaintManagerUI::Term();
        ExitModules();
        break;
    case DLL_PROCESS_ATTACH: {
        InitModules(hModule);
        InitLog();
        if (!InitDuilibRes(hModule))
            OUTPUT_XYLOG(LEVEL_INFO, L"初始化key map界面库资源失败!");
    }
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

bool InitDuilibRes(HINSTANCE hInstance) {
    CPaintManagerUI::SetInstance(hInstance);
    CWndShadow::Initialize(hInstance);
#ifdef _DEBUG
    wstring strDir = L"Skin/KeyMap";
    wstring wstrResoucePath = CPaintManagerUI::GetInstancePath();
    wstrResoucePath += strDir;
    CPaintManagerUI::SetResourcePath(wstrResoucePath.c_str());
#else
    BYTE* pSkinBuffer = NULL;
    HRSRC hRes = ::FindResource(hInstance, MAKEINTRESOURCE(IDR_DAT1), L"DAT");
    HGLOBAL hGlobal = ::LoadResource(hInstance, hRes);
    DWORD dwSkinBufferSize = ::SizeofResource(hInstance, hRes);
    BYTE* pResource = (BYTE*)::LockResource(hGlobal);
    CPaintManagerUI::SetResourceZip(pResource, dwSkinBufferSize);
#endif
    return true;
}

// 获取应用程序文档路径
wstring GetDocumentPath() {
    wchar_t szUserFolder[MAX_PATH] = { 0 };
    SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szUserFolder);
    std::wstring strFolder = szUserFolder;
    strFolder.append(L"\\");
    strFolder.append(L"IosKeyMap");
    SHCreateDirectory(NULL, strFolder.c_str());
    return strFolder;
}

bool InitLog() {
    // 初始化XYClient日志
    wstring strLogPath = GetDocumentPath() + L"\\log\\IosKeyMap";
    SHCreateDirectory(NULL, strLogPath.c_str());
    strLogPath.append(L"\\IosKeyMap.log");
    SET_XYLOG_PATH(strLogPath.c_str());
    SET_XYLOG_LEVEL(LEVEL_DEBUG);
    SET_PROCESS_NAME(L"IosKeyMap_Main");
    return true;
}

bool InitModules(HINSTANCE hInstance) {
    CGlobalData::Instance()->Init();
    return true;
}

void ExitModules() {
    CGlobalData::Instance()->Exit();
    return;
}