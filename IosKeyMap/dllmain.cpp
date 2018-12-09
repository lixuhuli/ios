// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <Shlobj.h>
#include "GlobalData.h"

bool InitLog();//��ʼ����־ϵͳ
bool InitDuilibRes(HINSTANCE hInstance); // ��ʼ���������Դ
bool InitModules(HINSTANCE hInstance);   // ��ʼ������ģ��
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
            OUTPUT_XYLOG(LEVEL_INFO, L"��ʼ��key map�������Դʧ��!");
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

// ��ȡӦ�ó����ĵ�·��
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
    // ��ʼ��XYClient��־
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