// AppleSeed.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "SoftDefine.h"
#include "CefBase/util.h"
#include "CefBase/client_app_browser.h"
#include "CefBase/main_message_loop_std.h"
#include "GlobalData.h"
#include "UserData.h"
#include "ExceptionHandler.h"
#include "DataPost.h"
#include "CxImg/xImageApi.h"
#include "WndMain.h"
#include "Ios/IosMgr.h"
#include "TaskCenter/TaskCenter.h"
#include "Database/DatabaseMgr.h"
#include "Download/DownloadMgr.h"
#include "CallBack/callback_mgr.h"

bool InitLog();//��ʼ����־ϵͳ
bool CheckInstance(HANDLE& hMetux);//��ʵ�����
bool InitDuilibRes(HINSTANCE hInstance);//��ʼ���������Դ
bool InitModules(HINSTANCE hInstance);//��ʼ������ģ��
void ExitModules();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    HANDLE hMutex = NULL;
    if (CheckInstance(hMutex)) {
        OutputDebugString(L"�Ѿ���һ��ʵ���������У��˳�\n");
        return 0;
    }

    CExceptionHandler::Instance()->Init(lpCmdLine);//�쳣����

    CefMainArgs main_args(hInstance);
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->InitFromString(::GetCommandLineW());
    CefRefPtr<CefApp> app(new ClientAppBrowser());
    void* sandbox_info = NULL;
    CefSettings settings;
    std::string locale("zh-CN");
    cef_string_utf8_to_utf16(locale.c_str(), locale.size(), &settings.locale);
    settings.no_sandbox = 1;
    //settings.background_color = 0xFFFFFF;
    wstring strAppDataPath = GetAppDataPath();
    wstring strCachePath = strAppDataPath + L"CefCache";
    PublicLib::RemoveDir(strCachePath.c_str());
    SHCreateDirectory(NULL, strCachePath.c_str());
    cef_string_set(strCachePath.c_str(), strCachePath.size(), &settings.cache_path, 1);
#ifdef _DEBUG
    wstring strLogPath = strAppDataPath + L"CefLog.log";
    if (PathFileExists(strLogPath.c_str()))
    {
        DeleteFile(strLogPath.c_str());
    }
    cef_string_set(strLogPath.c_str(), strLogPath.size(), &settings.log_file, 1);
#else
    settings.log_severity = LOGSEVERITY_DISABLE;
#endif
    settings.multi_threaded_message_loop = 0;
    settings.single_process = 0;
    wstring strRender = GetRunPathW() + EXE_RENDER;
    cef_string_set(strRender.c_str(), strRender.size(), &settings.browser_subprocess_path, 1);
    settings.persist_session_cookies = 1;
    settings.windowless_rendering_enabled = 0;
    //settings.persist_user_preferences = 1;
    scoped_ptr<MainMessageLoop> message_loop(new MainMessageLoopStd);
    InitLog();
    CoInitialize(NULL);
    if (!InitDuilibRes(hInstance))
    {
        MessageBox(NULL, L"��ʼ���������Դʧ�ܣ�", L"�����ˣ�", MB_OK | MB_ICONERROR);
        CloseHandle(hMutex);
        return 0;
    }
    CefInitialize(main_args, settings, app, sandbox_info);
    g_shareData.MapSharedMem(sizeof(CEF_SHARE_DATA), g_pShareGuid);
    InitModules(hInstance);

    ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);
    ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);

    CWndMain* pWnd = new CWndMain;
    ((LPCEF_SHARE_DATA)g_shareData)->hMainWnd = pWnd->Create(NULL);
    pWnd->CenterWindow();
    ::ShowWindow(pWnd->GetHWND(), SW_SHOWNORMAL);

    bool bShow = wcscmp(lpCmdLine, L"/bootrun") != 0;
    PostStartPC(!bShow);

    message_loop->Run();

    bool need_reboot = CGlobalData::Instance()->NeedReboot();
    wstring strPath = CGlobalData::Instance()->GetRunPath() + EXE_MAIN;

    ExitModules();
    CefShutdown();
    message_loop.reset();
    CPaintManagerUI::Term();
    CoUninitialize();
    CloseHandle(hMutex);

    // ����ģ������Ҫ����
    if (need_reboot) ShellExecute(nullptr, L"open", strPath.c_str(), nullptr, nullptr, SW_SHOW);

    return 0;
}

bool InitDuilibRes(HINSTANCE hInstance)
{
    CPaintManagerUI::SetInstance(hInstance);
    CWndShadow::Initialize(hInstance);
#ifdef _DEBUG
    wstring strDir = L"Skin\\Client";
    wstring wstrResoucePath = CPaintManagerUI::GetInstancePath();
    wstrResoucePath += strDir;
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

bool InitLog()
{
    //��ʼ��XYClient��־
    wstring strLogPath = GetDocumentPath() + L"\\log\\AppleSeed";
    SHCreateDirectory(NULL, strLogPath.c_str());
    strLogPath.append(L"\\AppleSeed.log");
    SET_XYLOG_PATH(strLogPath.c_str());
    SET_XYLOG_LEVEL(LEVEL_DEBUG);
    SET_PROCESS_NAME(L"AppleSeed_Main");
    return true;
}

bool InitModules(HINSTANCE hInstance) {
    // ��ʼ��ȫ�ֱ���
    OUTPUT_XYLOG(LEVEL_INFO, L"��ʼ��ȫ������ģ��");
    CGlobalData::Instance()->Init();

    CDatabaseMgr::Instance()->Init();
    OUTPUT_XYLOG(LEVEL_INFO, L"��ʼ����������ģ��");

    // ���ݿ���´��� �����°汾
    if (CDatabaseMgr::Instance()->NeedUpdate()) CDatabaseMgr::Instance()->UpdateNewDbData();
    OUTPUT_XYLOG(LEVEL_INFO, L"�ж��Ƿ�������ݿ⣬�����°汾");

    AppSetting setting;
    CDatabaseMgr::Instance()->GetSetting(setting);
    OUTPUT_XYLOG(LEVEL_INFO, L"��ʼ����������ģ��");

    TaskCenter::CTaskCenter::Instance()->Init();

    CUserData::Instance()->Init();

    CIosMgr::Instance()->Init();

    CCallBackMgr::Instance()->Init();

    InitDataCenter(nullptr);

    // ��ʼ����������
    return CDownloadMgr::Instance()->Init(hInstance, 5, setting.nMaxLoadSpeed);
}

void ExitModules() {
    CCallBackMgr::Instance()->Exit();
    TaskCenter::CTaskCenter::Instance()->Exit();
    CGlobalData::Instance()->Exit();
    CDatabaseMgr::Instance()->Exit();
    CDownloadMgr::Instance()->Exit();
    CUserData::Instance()->Exit();
    CIosMgr::Instance()->Exit();

    ExitDataCenter();

    OUTPUT_XYLOG(LEVEL_INFO, L"����ʼ��ģ�����");
}

bool CheckInstance(HANDLE& hMutex)
{
    hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);
    if (hMutex)
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            HWND hWnd = FindWindow(GUI_CLASS_NAME, MAIN_WND_NAME);
            if (hWnd == NULL || !IsWindow(hWnd))
            {//�����Ѿ���ס��ɱ����
                vector<wstring> vecProcess;
                vecProcess.push_back(EXE_MAIN);
                PublicLib::KillProcessFromList(vecProcess);
                return false;
            }
            ActiveWnd(hWnd);
            return true;
        }
        return false;
    }
    return false;
}