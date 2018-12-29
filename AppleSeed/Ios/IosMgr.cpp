#include "StdAfx.h"
#include "IosMgr.h"
#include "cef_bind.h"
#include "EngineInterface.h"
#include "GlobalData.h"
#include "WndIos.h"
#include <winnt.rh>
#include "MsgDefine.h"
#include "wrapper/cef_closure_task.h"
#include "emulator_state_info.h"
#include "Download/Task.h"
#include "IosThread.h"
#include "KeyMap/KeyWnd.h"
#include "Download/DownloadMgr.h"
#include "CommonWnd.h"
#include "CallBack/callback_mgr.h"
#include "TaskCenter/TaskCenter.h"
#include "UserData.h"

static int engine_callbacks(int status, uintptr_t param1, uintptr_t param2) {
    return CIosMgr::Instance()->EngineCallback(status, param1, param2);
}

CIosMgr::CIosMgr()
 : ios_wnd_(nullptr)
 , key_wnd_(nullptr)
 , hor_screen_mode_(false)
 , wnd_size_(CSize(0, 0))
 , ios_scale_(750.0 / 1334.0) {
}


CIosMgr::~CIosMgr() {
}

CIosMgr* CIosMgr::Instance() {
    static CIosMgr obj;
    return &obj;
}

bool CIosMgr::Init() {
    emulator_state_info_ = new emulator::EmulatorStateInfo();
    if (!emulator_state_info_) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"malloc emulator_state_info_ failed, nullptr");
        return false;
    }

    RegisterEngineStatusCallback(engine_callbacks);
    return true;
}

void CIosMgr::Exit() {
    list_install_task_.clear();
    list_install_file_.clear();

    if (install_app_thread_) install_app_thread_.reset(nullptr);
    if (engine_off_thread_) engine_off_thread_.reset(nullptr);

    //if (emulator_state_info_ && !emulator_state_info_->engine_off()) EngineOff();

    emulator_state_info_ = nullptr;
}

base::WeakPtr<emulator::iEmulatorStateInfo> CIosMgr::emulator_state_info() {
    if (emulator_state_info_) return emulator_state_info_->AsWeakPtr();
    return base::WeakPtr<emulator::iEmulatorStateInfo>();
}

bool CIosMgr::IosEngineOn(const string& iso_file) {
    if (!ios_wnd_) return false;

    auto result = EngineOn(ios_wnd_->GetHWND(), iso_file.c_str(), nullptr);

    return (result == 0);
}

bool CIosMgr::IosCheckLicense(const char *serial) {
    if (!serial) return false;
    return (SetLicense(serial) == 0);
}

void CIosMgr::IosHome() {
    if (emulator_state_info_ && !emulator_state_info_->engine_off()) Home();
}

void CIosMgr::InstallApp(const UINT_PTR& task_ptr) {
    if (!emulator_state_info_) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"error, emulator_state_info_ is nullptr!");
        return;
    }

    list_install_task_.push_back(task_ptr);

    if (emulator_state_info_->engine_on()) StartInstallApp();
}

void CIosMgr::InstallApp(const std::wstring& file_path) {
    if (!emulator_state_info_) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"error, emulator_state_info_ is nullptr!");
        return;
    }

    ITask* pTask = new ITask;
    if (!pTask) return;

    auto pos = file_path.rfind(L"\\");
    if (pos != wstring::npos) pTask->strName = file_path.substr(pos + 1, file_path.length() - pos - 1);

    pTask->strSavePath = file_path;
    list_install_file_.push_back((UINT_PTR)pTask);

    if (emulator_state_info_->engine_on()) StartInstallApp();
}

void CIosMgr::StartInstallApp() {
    if (!emulator_state_info_) return;

    if (!list_install_file_.empty()) {
        auto it = list_install_file_.front();

        list_install_file_.pop_front();

        ITask* task = (ITask*)it;
        if (!task) return;

        CreateInstallFileTask(it);

        emulator_state_info_->set_state(emulator::STATE_INSTALLING);

        return;
    }

    if (!list_install_task_.empty()) {
        auto it = list_install_task_.front();

        list_install_task_.pop_front();

        ITask* task = (ITask*)it;
        if (!task) return;

        CreateInstallAppTask(it);

        emulator_state_info_->set_state(emulator::STATE_INSTALLING);
        emulator_state_info_->set_app_of_installing(it);
    }
}

void CIosMgr::CreateInstallAppTask(const UINT_PTR& task_ptr) {
    if (!install_app_thread_) install_app_thread_.reset(new CIosThread());
    if (install_app_thread_) {
        install_app_thread_->Create("Install App", (void *)task_ptr, std::bind(&CIosMgr::InstallAppThread, this, (void *)task_ptr));
    }
}

int CIosMgr::InstallAppThread(void * argument) {
    ITask* task = (ITask*)argument;
    if (!task) return 0;

    auto state = InstallIPA(PublicLib::UToUtf8(task->strSavePath).c_str());

    CefPostTask(TID_UI, base::Bind(&CIosMgr::OnInstallApp, AsWeakPtr(), (UINT_PTR)task, state));

    return state;
}

void CIosMgr::OnInstallApp(const UINT_PTR& task_ptr, int state) {
    if (!emulator_state_info_) return;

    if (state != 0) {
        emulator_state_info_->set_state(emulator::STATE_ENGINE_ON);
        emulator_state_info_->set_app_of_installing(0);
        ::PostMessage(CDownloadMgr::Instance()->GetMsgWnd(), WM_DLWND_MSG_IPA_INSTALL, (WPARAM)task_ptr, state);
        StartInstallApp();
    }
}

void CIosMgr::CreateInstallFileTask(const UINT_PTR& task_ptr) {
    if (!install_app_thread_) install_app_thread_.reset(new CIosThread());
    if (install_app_thread_) {
        install_app_thread_->Create("Install App", (void *)task_ptr, std::bind(&CIosMgr::InstallFileThread, this, (void *)task_ptr));
    }
}

int CIosMgr::InstallFileThread(void * argument) {
    ITask* task = (ITask*)argument;
    if (!task) return 0;

    auto state = InstallIPA(PublicLib::UToUtf8(task->strSavePath).c_str());

    CefPostTask(TID_UI, base::Bind(&CIosMgr::OnInstallFile, AsWeakPtr(), (UINT_PTR)task, state));

    return state;
}

void CIosMgr::OnInstallFile(const UINT_PTR& task_ptr, int state) {
    if (!emulator_state_info_) return;

    emulator_state_info_->set_state(emulator::STATE_ENGINE_ON);

    ITask* task = (ITask*)task_ptr;
    if (task && state != 0) {
        wstring prefix = L"安装";
        prefix.append(task->strName);
        prefix.append(L"文件失败!");
        ShowMsg(CGlobalData::Instance()->GetMainWnd(), L"提示", prefix.c_str(), MB_OK);
    }

    StartInstallApp();
}

void CIosMgr::OnEngineOn() {
    OUTPUT_XYLOG(LEVEL_ERROR, L"CIosMgr::OnEngineOn : ENGINE_STATUS_SWITCHED_ON");

    if (!emulator_state_info_) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"error, emulator_state_info_ is nullptr!");
        return;
    }

    PostMessage(CGlobalData::Instance()->GetMainWnd(), WM_MAINWND_MSG_EMULATOR_ALREADY, 0, 0);

    CheckEngineUpdate();
}

void CIosMgr::OnScreenSizeChanged(uintptr_t param1, uintptr_t param2) {
    if (param2 <= 0 || param2 <= 0) return;

    wnd_size_.cx = param1;
    wnd_size_.cy = param2;

    if (wnd_size_.cx >= wnd_size_.cy) {
        if (!hor_screen_mode_) {
            hor_screen_mode_ = true;
            ios_scale_ = (double)param2 / (double)param1; 
            UpdateIosWnd(nullptr);
        }
    }
    else {
        if (hor_screen_mode_) {
            hor_screen_mode_ = false;
            ::PostMessage(CGlobalData::Instance()->GetMainWnd(), WM_MAINWND_MSG_UPDATE_IOSWND_POS, 0, 0);
        }

        CloseKeyWnd();
    }
}

void CIosMgr::OnForegroundAppChanged(uintptr_t param1, uintptr_t param2) {
    if (!emulator_state_info_) return;

    const char *context = (char *)param1;
    emulator_state_info_->set_running_app_id(context ? context : "");

    if (!context || !ios_wnd_) return;

    if (hor_screen_mode_) {
        if (HasKeyMapFile()) CreateKeyWnd(CGlobalData::Instance()->GetMainWnd());
        else {
            MSG msg = { 0 };
            msg.hwnd = CGlobalData::Instance()->GetMainWnd();
            msg.message = WM_MAINWND_MSG_GET_KEYBOARD;

            auto keymap_dir = GetKeyMapDir();
            auto download_path = GetDocumentPath() + L"\\keymap\\"+ PublicLib::AToU(emulator_state_info_->running_app_id()) + L".zip";
            if (!::PathFileExists(keymap_dir.c_str())) SHCreateDirectory(nullptr, keymap_dir.c_str());

            auto strAppId = emulator_state_info_->running_app_id();
            TaskCenter::CTaskCenter::Instance()->CreateGetKeyBoardConfigTask(msg, strAppId, download_path, keymap_dir);
        }
    }
    else CloseKeyWnd();

    return;
}

void CIosMgr::OnApplicationAdded(uintptr_t param1, uintptr_t param2) {
    if (!emulator_state_info_) return;

    ITask* task = (ITask*)emulator_state_info_->app_of_installing();

    emulator_state_info_->set_state(emulator::STATE_ENGINE_ON);
    emulator_state_info_->set_app_of_installing(0);

    if (task) {
        const char *context = (char *)param1;
        if (context) task->strPkgName = context;
        ::PostMessage(CDownloadMgr::Instance()->GetMsgWnd(), WM_DLWND_MSG_IPA_INSTALL, (WPARAM)task, 0);
    }

    StartInstallApp();
}

void CIosMgr::OnApplicationModified(uintptr_t param1, uintptr_t param2) {
    if (!emulator_state_info_) return;

    ITask* task = (ITask*)emulator_state_info_->app_of_installing();

    emulator_state_info_->set_state(emulator::STATE_ENGINE_ON);
    emulator_state_info_->set_app_of_installing(0);

    if (task) {
        const char *context = (char *)param1;
        if (context) task->strPkgName = context;
        ::PostMessage(CDownloadMgr::Instance()->GetMsgWnd(), WM_DLWND_MSG_IPA_INSTALL, (WPARAM)task, 0);
    }

    StartInstallApp();
}

void CIosMgr::OnApplicationRemoved(uintptr_t param1, uintptr_t param2) {
    const char *context = (char *)param1;
    if (!context) return;

    string package_name = context;
    ITask* pTask = CDatabaseMgr::Instance()->GetGameInfo(package_name);
    if (!pTask) return;

    auto nGameID = pTask->nGameID;

    CDatabaseMgr::Instance()->DeleteGameInfo(nGameID);

    SetWebGameStatus(nGameID, GameUnload);

    auto observers = CCallBackMgr::Instance()->Observers();
    if (observers) observers->Go(&AppleSeedCallback::ApplicationRemoved, nGameID);
}

int CIosMgr::EngineCallback(int status, uintptr_t param1, uintptr_t param2) {
    switch (status) {
    case ENGINE_STATUS_SWITCHED_ON: 
        CefPostTask(TID_UI, base::Bind(&CIosMgr::OnEngineOn, AsWeakPtr()));
        break;
    case ENGINE_STATUS_SWITCHED_OFF: {
        break;
    }
    case ENGINE_STATUS_ON_PROGRESS:
        break;
    case ENGINE_STATUS_SCREEN_SIZE_CHANGE: 
        CefPostTask(TID_UI, base::Bind(&CIosMgr::OnScreenSizeChanged, AsWeakPtr(), param1, param2));
        break;
    case ENGINE_STATUS_FOREGROUND_APP_CHANGE: 
        CefPostTask(TID_UI, base::Bind(&CIosMgr::OnForegroundAppChanged, AsWeakPtr(), param1, param2));
        break;
    case ENGINE_STATUS_APPLICATION_ADDED:
        CefPostTask(TID_UI, base::Bind(&CIosMgr::OnApplicationAdded, AsWeakPtr(), param1, param2));
        break;
    case ENGINE_STATUS_APPLICATION_MODIFIED:
        CefPostTask(TID_UI, base::Bind(&CIosMgr::OnApplicationModified, AsWeakPtr(), param1, param2));
        break;
    case ENGINE_STATUS_APPLICATION_REMOVED:
        CefPostTask(TID_UI, base::Bind(&CIosMgr::OnApplicationRemoved, AsWeakPtr(), param1, param2));
        break;
    case ENGINE_STATUS_GET_UID_AND_TOKEN:
        CefPostTask(TID_UI, base::Bind(&CIosMgr::OnGetUidAndToken, AsWeakPtr(), param1, param2));
        break;
    case ENGINE_STATUS_EMULATION_QUIT:
        CefPostTask(TID_UI, base::Bind(&CIosMgr::OnEmulationQuit, AsWeakPtr(), param1, param2));
        break;
    default:
        break;
    }

    return 0;
}

void CIosMgr::CreateWndIos(HWND hParentWnd) {
    ios_wnd_ = new CWndIos();
    if (!ios_wnd_) return;

    ios_wnd_->Create(hParentWnd, true/*, 80 + 22, 90 + 75*/);
    ::SetWindowPos(*ios_wnd_, HWND_TOP, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE);
}

void CIosMgr::UpdateIosWnd(const QRect* lprc /*= nullptr*/) {
    if (!ios_wnd_) return;

    if (!hor_screen_mode_) {
        if (!lprc) return;

        QRect rc;
        rc = *lprc;

        ::SetWindowPos(*ios_wnd_, nullptr, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), SWP_NOZORDER);
    }
    else {
        QRect rc;
        if (!lprc) GetClientRect(CGlobalData::Instance()->GetMainWnd(), &rc);
        else rc = *lprc;

        rc.top += 60;
        rc.bottom -= 60;

        ::SetWindowPos(*ios_wnd_, nullptr, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), SWP_NOZORDER);
    }
}

bool CIosMgr::IosSnap(const std::string& save_path) {
    if (!emulator_state_info_) return false;
    if (emulator_state_info_->engine_off()) return false;

    if (save_path.empty()) return false;
    return (0 == CapturePictureToFile(save_path.c_str()));
}

void CIosMgr::CreateKeyWnd(HWND hParentWnd) {
    CloseKeyWnd();

    key_wnd_ = new CKeyWnd;
    if (!key_wnd_ || !ios_wnd_) return;

    QRect rc;
    GetWindowRect(*ios_wnd_, &rc);

    key_wnd_->Create(hParentWnd, false, rc.left, rc.top, rc.GetWidth(), rc.GetHeight());
    key_wnd_->ShowWindow(true);
}

void CIosMgr::UpdateKeyWnd(const QRect* lprc/* = nullptr*/) {
    if (!ios_wnd_ || !key_wnd_) return;

    QRect rc;
    if (lprc) rc = *lprc;
    else GetWindowRect(*ios_wnd_, &rc);

    MoveWindow(*key_wnd_, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);

    key_wnd_->UpdateBrowserWnd(&rc);
}

void CIosMgr::UpdateBrowserMode(bool browser_mode) {
    if (key_wnd_) key_wnd_->SetBrowserMode(browser_mode);
}

void CIosMgr::CheckEngineUpdate() {
    int version[3] = {0, 0, 0};

    GetEngineVersion(&version[0], &version[1], &version[2]);
    
    wchar_t szVersion[MAX_PATH + 1] = { 0 };
    wsprintf(szVersion, L"%d.%d.%d", version[0], version[1], version[2]);

    std::wstring strVersion = szVersion;

    OUTPUT_XYLOG(LEVEL_INFO, L"镜像版本号:%s", szVersion);

    MSG msg = { 0 };
    msg.hwnd = CGlobalData::Instance()->GetMainWnd();
    msg.message = WM_MAINWND_MSG_IOSENGINE_UPDATING;
    TaskCenter::CTaskCenter::Instance()->CreateIosEngineUpdateTask(msg, WM_MAINWND_MSG_IOSENGINE_UPDATE, strVersion);

    emulator_state_info_->set_state(emulator::STATE_UPDATING);
}

int CIosMgr::UpdatePackage(const std::wstring& file_path) {
    return UpdatePack(PublicLib::UToUtf8(file_path).c_str());
}

void CIosMgr::OnPackUpdating(int percent) {
}

void CIosMgr::OnPackUpdate(int status){
    if (status == 0 || status == -1) {
        if (ios_wnd_) ios_wnd_->ShowWindow(true);
        emulator_state_info_->set_state(emulator::STATE_ENGINE_ON);
        ::PostMessage(CGlobalData::Instance()->GetMainWnd(), WM_MAINWND_MSG_IOSENGINE_APPLIACTION, 0, 0);
        StartInstallApp();
    }
    else {
        CreateEngineOffTask();
    }
}

void CIosMgr::CreateEngineOffTask() {
    if (!engine_off_thread_) engine_off_thread_.reset(new CIosThread());
    if (engine_off_thread_) {
        engine_off_thread_->Create("Engine Off", nullptr, std::bind(&CIosMgr::EngineOffThread, this, nullptr));
    }
}

int CIosMgr::EngineOffThread(void * argument){
    auto state = EngineOff();

    CefPostTask(TID_UI, base::Bind(&CIosMgr::OnEngineOff, AsWeakPtr(), state));

    return state;
}

void CIosMgr::OnEngineOff(int state) {
    if (state == 0) {
        ShowMsg(CGlobalData::Instance()->GetMainWnd(), L"提示", L"更新成功，请重启", MB_OK);
        CGlobalData::Instance()->SetNeedReboot(true);
        PostMessage(CGlobalData::Instance()->GetMainWnd(), WM_MAINWND_MSG_EXIT, 0, 0);
    }
    else {
        if (ios_wnd_) ios_wnd_->ShowWindow(true);
        emulator_state_info_->set_state(emulator::STATE_ENGINE_ON);
        ::PostMessage(CGlobalData::Instance()->GetMainWnd(), WM_MAINWND_MSG_IOSENGINE_APPLIACTION, 0, 0);
        StartInstallApp();
    }
}

int CIosMgr::UpdateKeyMap(const std::wstring& file_path) {
    return LoadKeyMap(PublicLib::UToUtf8(file_path).c_str());
}

void CIosMgr::CloseKeyWnd() {
    if (key_wnd_){
        key_wnd_->CloseBrowserWnd();
        key_wnd_->Close(IDCLOSE);
        key_wnd_ = nullptr;
    }
}

bool CIosMgr::HasKeyMapFile() {
    auto key_dir = GetDocumentPath() + L"\\keymap";
    if (!::PathFileExists(key_dir.c_str())) return false;

    auto keymap_dir = key_dir + L"\\" + PublicLib::AToU(emulator_state_info_->running_app_id());
    if (!::PathFileExists(keymap_dir.c_str())) return false;

    auto config_name = keymap_dir + L"\\conf.ini";
    if (!::PathFileExists(config_name.c_str())) return false;

    wchar_t szValue[128] = { 0 };
    DWORD dwLen = GetPrivateProfileString(L"setting", L"record", L"", szValue, 128, config_name.c_str());
    if (dwLen <= 0) return false;

    return true;
}

wstring CIosMgr::GetKeyMapDir() {
    if (!emulator_state_info_) return L"";
    wstring keymap_dir = GetDocumentPath() + L"\\keymap\\" + PublicLib::AToU(emulator_state_info_->running_app_id());
    return keymap_dir;
}

void CIosMgr::OnGetUidAndToken(uintptr_t param1, uintptr_t param2) {
    if (CUserData::Instance()->GetUserState() == UsNone)  SetUidAndToken(nullptr, nullptr);
    else if (CUserData::Instance()->GetUserState() == UsLogin) {
        char szUid[MAX_PATH] = { 0 };
        sprintf(szUid, "%d", CUserData::Instance()->GetFileUserID());
        SetUidAndToken(szUid, PublicLib::AToUtf(CUserData::Instance()->GetFileUserToken()).c_str());
    }
}

void CIosMgr::OnEmulationQuit(uintptr_t param1, uintptr_t param2) {
    ShowMsg(CGlobalData::Instance()->GetMainWnd(), L"提示", L"没有权限打开该游戏", MB_OK);
}

void CIosMgr::OnGetKeyboard(WPARAM wParam, LPARAM lParam) {
    if (!emulator_state_info_) return;

    UINT_PTR nTask = (UINT_PTR)lParam;
    string strJson;
    if (!TaskCenter::CTaskCenter::Instance()->GetTaskResult(nTask, strJson)) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"错误的任务ID");
        return;
    }

    bool bSuccess = strJson == "1";

    string strAppId;
    TaskCenter::CTaskCenter::Instance()->GetKeyBoardConfigAppId(nTask, strAppId);

    if (strAppId != emulator_state_info_->running_app_id()) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"该游戏已经不再运行");
        return;
    }

    if (!bSuccess) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"键盘映射文件下载失败");
        WriteBaseKeyBoard(strAppId);
    }

    CIosMgr::Instance()->CreateKeyWnd(CGlobalData::Instance()->GetMainWnd());
}

bool CIosMgr::GetEngineApplications(std::vector<string>& engine_apps) {
    auto app_count = GetInstalledAppCount();
    if (app_count <= 0) return false;

    PIOSAPPINFO app_info = new IOSAPPINFO[app_count];
    if (!app_info) return false;

    if (GetInstalledAppInfo(app_info, app_count) == 0) {
        delete []app_info;
        return false;
    }

    PIOSAPPINFO info = nullptr;
    for (int i = 0; i < app_count; i++) {
        info = (app_info + i);
        if (!info) continue;
        engine_apps.push_back(info->bundle_name);
    }

    delete []app_info;

    return true;
}

void CIosMgr::WriteBaseKeyBoard(const string& app_id) {
    if (!emulator_state_info_) return;

    auto config = GetKeyMapDir() + L"\\conf.ini";

    if (!WritePrivateProfileString(L"setting", L"record", L"records_1", config.c_str())) return;

    WritePrivateProfileString(L"records_1", L"name", L"默认配置", config.c_str());
    WritePrivateProfileString(L"records_1", L"file", PublicLib::AToU(app_id).c_str(), config.c_str());
}

