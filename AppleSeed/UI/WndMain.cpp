#include "StdAfx.h"
#include "WndMain.h"
#include "UICefWebkit.h"
#include "UICenterHorizontalLayout.h"
#include "GlobalData.h"
#include "Ios/IosMgr.h"
#include "WebCtrlUI.h"
#include "SoftDefine.h"
#include "PageDownload.h"
#include "UrlDefine.h"
#include "download/download_info.h"
#include "TaskCenter/TaskCenter.h"
#include "CommonWnd.h"
#include "WndVolume.h"
#include "WndUser.h"
#include "UserData.h"
#include "DataPost.h"
#include "WndUserCenter.h"
#include "Database/DatabaseMgr.h"
#include "Download/DownloadMgr.h"
#include "WndPerOptimiz.h"

#define URL_PACKAGE_NAME_INTEL   "MotherDisc_Intel_1230.7z"
#define URL_PACKAGE_NAME_AMD     "MotherDisc_AMD_1230.7z"

#define URL_ISO_NAME_INIT_INTEL  "MotherDisc_Intel_1230.vmdk"
#define URL_ISO_NAME_INIT_AMD    "MotherDisc_AMD_1230.vmdk"

#define URL_ISO_NAME_INTEL       "MotherDisc_Intel_1230_Using.vmdk"
#define URL_ISO_NAME_AMD         "MotherDisc_AMD_1230_Using.vmdk"

#define DOWNLOAD_PROGRESS_TIME   (int)150

CWndMain::CWndMain()
 : web_focus_(nullptr)
 , btn_ios_restart_(nullptr)
 , btn_ios_home_(nullptr)
 , web_home_page_(nullptr) 
 , layout_main_(nullptr)
 , opt_home_market_(nullptr)
 , opt_home_bbs_(nullptr)
 , opt_home_download_(nullptr)
 , install_system_(nullptr) 
 , select_folder_(nullptr)
 , btn_install_sys_(nullptr) 
 , btn_install_cancel_(nullptr)
 , layout_install_(nullptr)
 , progress_install_(nullptr)
 , lbl_install_status_(nullptr)
 , lbl_install_speed_(nullptr)
 , install_loading_(nullptr)
 , loading_frame_(0)
 , btn_install_pause_(nullptr)
 , btn_install_restart_(nullptr) 
 , btn_ios_volume_(nullptr)
 , btn_user_icon_(nullptr)
 , user_mask_ico_(nullptr)
 , btn_key_(nullptr) 
 , btn_per_opitimiz_(nullptr)
 , client_layout_left_(nullptr) 
 , client_layout_right_(nullptr) 
 , need_hide_left_layout_(false)
 , layout_update_(nullptr)
 , progress_update_(nullptr)
 , lbl_update_status_(nullptr)
 , client_iphone_(nullptr)
 , btn_install_home_(nullptr)
 , label_load_count_(nullptr)
 , client_iphone_emulator_(nullptr) {
    m_dwStyle = UI_WNDSTYLE_FRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    m_bShowShadow = true;
}

CWndMain::~CWndMain() {
}

LPCWSTR CWndMain::GetXmlPath() const {
    return L"WndMain.xml";
}

void CWndMain::InitWindow() {
    __super::InitWindow();

    CGlobalData::Instance()->SetMainWnd(m_hWnd);
    CGlobalData::Instance()->SetMainWndPtr(this);

    // 暂时注销  后续扩展
    //AddNotifyIcon();

    BIND_SUB_PAGE(page_download_, CPageDownloadUI, L"page_download")

    if (web_home_page_) {
        web_focus_ = web_home_page_->GetCefWebkit();
    }

    InitTasks();

    CIosMgr::Instance()->CreateWndIos(m_hWnd);
    
    string iso_file = PublicLib::UToA(CGlobalData::Instance()->GetIosVmPath()) + "\\"+ GetUrlIsoName();
    if (PathFileExistsA(iso_file.c_str())) {
        if (!LoadIosEngine()) {
            ShowMsg(m_hWnd, L"提示", L"启动失败，请重启", MB_OK);
            CIosMgr::Instance()->CreateEngineOffTask();
            return;
        }

        if (layout_install_) layout_install_->SelectItem(2);
        loading_frame_ = 0;
        UpdateLoadingIcon();
        ::SetTimer(m_hWnd, TIMER_ID_DOWNLOAD_PROGRESS, DOWNLOAD_PROGRESS_TIME, nullptr);
    }
    else RemoveSpilthVmdk();

    if (client_iphone_emulator_) client_iphone_emulator_->OnSize += MakeDelegate(this, &CWndMain::OnIphoneEmulatorSize);

    if (btn_ios_volume_) btn_ios_volume_->SetEnabled(false);

    ::SetTimer(m_hWnd, TIMER_ID_CHECK_UPDATE, 1 * 1000, NULL);//1秒后检查更新

    RegisterHotKey(m_hWnd, ID_HOTKEY_BOS, MOD_ALT + MOD_CONTROL, 'N');

    if (CGlobalData::Instance()->IsShowPerOptimizWnd()) {
        ::PostMessage(m_hWnd, WM_MAINWND_MSG_SHOW_PEROPTIMIZATION, 0, 0);
        CGlobalData::Instance()->SetShowPerOptimizWnd(false);
    }
    else {
        bool support = false;
        auto IsVTOpened = CIosMgr::Instance()->IsCPUVTOpened(support);
        bool show_per = CGlobalData::Instance()->IsCheckShowPerOptimiz();
        if (!show_per && IsVTOpened) {
            btn_per_opitimiz_->SetNormalImage(L"");
            btn_per_opitimiz_->SetEnabled(false);
        }
    }
}

void CWndMain::InitTasks() {
    MSG msg = { 0 };
    msg.hwnd = m_hWnd;

    // 验证账号
    if (CUserData::Instance()->GetAutoLogin() && CUserData::Instance()->HasFileAccount()) {  
        msg.message = WM_MAINWND_MSG_USERINFO;
        TaskCenter::CTaskCenter::Instance()->CreateUserInfoTask(msg, CUserData::Instance()->GetFileUserID(), CUserData::Instance()->GetFileUserToken());
    }

    msg.message = WM_MAINWND_MSG_CHECK_UPDATE_GAME;
    std::map<string, string> gameMap;

    const list<ITask*>& gameList = CDownloadMgr::Instance()->GetFinishList();
    if (!gameList.empty())
    {
        for (auto itor = gameList.begin(); itor != gameList.end(); ++itor)
        {
            char szGameID[20] = { 0 };
            sprintf_s(szGameID, "%I64d", (*itor)->nGameID);
            gameMap[szGameID] = PublicLib::UToUtf8((*itor)->strVersion);
        }
        TaskCenter::CTaskCenter::Instance()->CreateCheckGameUpdateTask(msg, gameMap);
    }
}

LRESULT CWndMain::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;

    if (uMsg > WM_MAINWND_MSG_BEGIN && uMsg < WM_MAINWND_MSG_END) {
        switch (uMsg) {
        case WM_MAINWND_MSG_DOWNLOAD_MIRRORSYSTEM: lRes = OnDownloadMirrorSystem(wParam, lParam, bHandled); break;
        case WM_MAINWND_MSG_FILE_UNZIPING: OnMsgFileUnziping(wParam, lParam, bHandled); break;
        case WM_MAINWND_MSG_FILE_UNZIP: OnMsgFileUnzip(wParam, lParam, bHandled); break;
        case WM_MAINWND_MSG_EMULATOR_ALREADY: return OnMsgEmulatorAlready(wParam, lParam, bHandled);
        case WM_MAINWND_MSG_LOAD_IOS_ENGINE: return OnMsgLoadIosEngine(wParam, lParam, bHandled);
        case WM_MAINWND_MSG_EXIT: OnMsgExit(wParam, lParam, bHandled); break;
        case WM_MAINWND_MSG_USERINFO: OnMsgUserInfo(wParam, lParam, bHandled); break;
        case WM_MAINWND_MSG_COMMON: return OnMsgCommon(wParam, lParam, bHandled);
        case WM_MAINWND_MSG_USER_ICO: OnMsgUserIco(wParam, lParam, bHandled); break;
        case WM_MAINWND_MSG_IOSENGINE_UPDATING: OnMsgIosEngineUpdating(wParam, lParam, bHandled); break;
        case WM_MAINWND_MSG_IOSENGINE_UPDATE: OnMsgIosEngineUpdate(wParam, lParam, bHandled); break;
        case WM_MAINWND_MSG_UPDATE_IOSWND_POS: OnMsgUpdateIosWndPos(wParam, lParam, bHandled); break;
        case WM_MAINWND_MSG_GET_KEYBOARD: OnMsgGetKeyboardConfig(wParam, lParam, bHandled); break;
        case WM_MAINWND_MSG_IOSENGINE_APPLIACTION: OnMsgIosEngineApplication(wParam, lParam, bHandled); break;
        case WM_MAINWND_MSG_CHECK_UPDATE_GAME: OnMsgGameCheckUpdateGame(wParam, lParam); break;
        case WM_MAINWND_MSG_SHOW_PEROPTIMIZATION: ShowPerOptimizWnd(); break;
        case WM_MAINWND_MSG_SHOW_PEROPTIMIZATION_WARNING: OnMsgShowPerOptimizIcon(wParam, lParam, bHandled); break;
        default: bHandled = FALSE; break;
        }

        TaskCenter::CTaskCenter::Instance()->DetachTask((UINT_PTR)lParam);
        return 0;
    }

    switch (uMsg) {
    case WM_SIZE: lRes = OnSize(wParam, lParam, bHandled); break;
    case WM_SIZING: lRes = OnSizing(wParam, lParam, bHandled); break;
    case WM_MOVE: lRes = OnMoving(wParam, lParam, bHandled); break;
    case WM_COPYDATA: return OnMsgCopyData(wParam, lParam);
    case WM_TIMER: OnTimer(wParam, lParam, bHandled); bHandled = FALSE; break;
    case WM_KEYDOWN: {
        bHandled = FALSE;
        break;
    }
    case WM_HOTKEY: OnMsgHotkey(wParam, lParam, bHandled); break;
    case WM_SETFOCUS: {
        if (web_focus_ && IsWindow(web_focus_->GetHostWnd())) {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(m_hWnd, &pt);

            RECT rc = { 224, 0, 224 + 290, 39 };
            if (PtInRect(&rc, pt)) break;

            SetFocus(web_focus_->GetHostWnd());

            return 0;
        }
        break;
    }
    case WM_RBUTTONDOWN: page_download_ ? page_download_->ShowUninstallBtnStatus(false) : 0; bHandled = FALSE; break;
    case WM_NCRBUTTONDOWN : page_download_ ? page_download_->ShowUninstallBtnStatus(false) : 0; bHandled = FALSE; break;
    case WM_CLOSE: OnWndClose(wParam, lParam, bHandled); break;
    default: bHandled = FALSE; break;
    }

    if (bHandled) {
        return lRes;
    }
    return __super::HandleMessage(uMsg, wParam, lParam);
}

void CWndMain::OnFinalMessage(HWND hWnd) {
    // 暂时注销  后续扩展
    //RemoveN otifyIcon(); 
    if (download_info_) {
        auto http = download_info_->http();
        if (http) http->NeedStop();
        download_info_ = nullptr;
    }

    __super::OnFinalMessage(hWnd);
    CefQuitMessageLoop();
}

base::WeakPtr<download::iDownloadInfo> CWndMain::download_info() {
    if (download_info_) return download_info_->AsWeakPtr();
    return base::WeakPtr<download::iDownloadInfo>();
}

LRESULT CWndMain::OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    QRect rc;
    GetClientRect(m_hWnd, &rc);

    if (client_iphone_ && client_layout_left_ && btn_install_home_) {
        auto client_left_width = (int)((float)rc.GetWidth() * 506.0 / 1334.0 + 0.5);
        auto phone_width = (int)((float)rc.GetWidth() * 334.0 / 1334.0 + 0.5);
        auto phone_height = (int)((float)phone_width * 807.0 / 417.0 + 0.5);

        auto dis_height = (rc.GetHeight() - phone_height - 120) / 2;
        if (dis_height < 30) {
            dis_height = 30;
            phone_height = rc.GetHeight() - 120 - dis_height * 2;
            phone_width = (int)((float)phone_height * 417.0 / 807.0 + 0.5);
        }

        client_layout_left_->SetFixedWidth(client_left_width);
        client_iphone_->SetFixedWidth(phone_width);
        client_iphone_->SetFixedHeight(phone_height);
        client_iphone_->SetPadding(QRect(0, dis_height, 0, 0));

        QRect inset(int((float)phone_width * 21.0 / 417.0), int((float)phone_height * 70.0 / 807.0), int((float)phone_width * 21.0 / 417.0), int((float)phone_height * 70.0 / 807.0));
        client_iphone_->SetInset(inset);

        QRect rc_home(int((float)phone_width * 183.0 / 417.0), int((float)phone_height * 747.0 / 807.0), int((float)phone_width * 235.0 / 417.0), int((float)phone_height * 799.0 / 807.0));

        SIZE szXY = {rc_home.left >= 0 ? rc_home.left : rc_home.right, rc_home.top >= 0 ? rc_home.top : rc_home.bottom};
        btn_install_home_->SetFixedXY(szXY);
        btn_install_home_->SetFixedWidth(rc_home.GetWidth());
        btn_install_home_->SetFixedHeight(rc_home.GetHeight());
    }

    if (CIosMgr::Instance()->HorScreenMode()) {
        CIosMgr::Instance()->UpdateIosWnd(&rc);
        CIosMgr::Instance()->UpdateKeyWnd(nullptr);
    }

    bHandled = FALSE;

    return 0;
}

LRESULT CWndMain::OnSizing(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    QRect *lprc = (QRect*)lParam;
    if (!lprc) return 0;

    auto scale = CIosMgr::Instance()->IosWndScale();

    if (wParam == 2 || wParam == 1) {
        auto height = (int)(scale * (double)lprc->GetWidth()) + 120;
        lprc->bottom = lprc->top + height;
    }
    else if (wParam == 3 || wParam == 6) {
        auto width = (int)((double)(lprc->GetHeight() - 120) / scale);
        lprc->right = lprc->left + width;
    }
    else {
        auto height = (int)(scale * (double)lprc->GetWidth()) + 120;
        lprc->bottom = lprc->top + height;
    }

    return 0;
}

LRESULT CWndMain::OnMoving(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    CIosMgr::Instance()->UpdateKeyWnd();
    return 0;
}

bool CWndMain::OnIphoneEmulatorSize(void* param) {
    if (!client_iphone_emulator_) return true;
    if (CIosMgr::Instance()->HorScreenMode()) return true;
    auto rc = client_iphone_emulator_->GetPos();
    CIosMgr::Instance()->UpdateIosWnd(&rc);
    return true;
}

LRESULT CWndMain::OnMsgUpdateIosWndPos(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (!client_iphone_emulator_) return 0;
    auto rc = client_iphone_emulator_->GetPos();
    CIosMgr::Instance()->UpdateIosWnd(&rc);
    return 0;
}

LRESULT CWndMain::OnMsgGetKeyboardConfig(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    CIosMgr::Instance()->OnGetKeyboard(wParam, lParam);
    return 0;
}

CControlUI* CWndMain::CreateControl(LPCTSTR pstrClass) {
    if (!pstrClass) return nullptr;

    std::wstring control_name = pstrClass;
    if (control_name == L"CenterHorizontalLayout") return new CCenterHorizontalLayoutUI();
    else if (control_name == L"WebCtrl") return new CWebCtrlUI(&m_pm);

    return nullptr;
}

bool CWndMain::OnBtnClickClose(void* lpParam) {
    //AppSetting setting;
    //CDatabaseMgr::Instance()->GetSetting(setting);
    //if (setting.nExit == 0) {
    //    ShowWindow(false, false);
    //    return true;
    //}

    if (IDOK != ShowMsg(m_hWnd, L"退出", L"确定退出客户端？", MB_OKCANCEL)) return true;

    Exit();

    return true;
}

bool CWndMain::OnBtnClickMin(void* lpParam) {
    ::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
    return true;
}

bool CWndMain::OnBtnClickMax(void* lpParam) {
    ::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
    return true;
}

bool CWndMain::OnBtnClickRestore(void* lpParam) {
    ::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    return true;
}

bool CWndMain::OnBtnClickIosRestart(void* param) {
    return true;
}

bool CWndMain::OnBtnClickIosHome(void* param) {
    CIosMgr::Instance()->IosHome();
    return true;
}

bool CWndMain::OnBtnClickVolume(void* param) {
    ShowVolumeWnd();
    return true;
}

void CWndMain::ShowClientLayoutLeft(bool show) {
    if (client_layout_left_) client_layout_left_->SetVisible(show);

    if (!web_home_page_ || !web_home_page_->GetCefWebkit()) return;

    HWND web_wnd = web_home_page_->GetCefWebkit()->GetHostWnd();

    ::SetWindowPos(web_wnd, show ? HWND_BOTTOM : HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void CWndMain::ShowUserCenter() {
    if (!btn_user_icon_) return;

    CWndUserCenter* wnd_user_center = new CWndUserCenter();
    if (!wnd_user_center) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"malloc user center window failed, nullptr!");
        return;
    }

    wnd_user_center->Create(m_hWnd, false);

    QRect rc_user(0, 0, 0, 0);
    GetClientRect(*wnd_user_center, &rc_user);

    const RECT& rc = btn_user_icon_->GetPos();
    POINT pt = { (rc.left + rc.right - rc_user.GetWidth()) / 2, rc.bottom + 1 };
    ClientToScreen(m_hWnd, &pt);

    ::SetForegroundWindow(*wnd_user_center);
    ::MoveWindow(*wnd_user_center, pt.x, pt.y, rc_user.GetWidth(), rc_user.GetHeight(), FALSE);

    wnd_user_center->ShowWindow(true);
}

bool CWndMain::OnEnterBtnUser(void* param) {
    TNotifyUI* notify = (TNotifyUI*)param;
    if (!notify || !notify->pSender) return true;

    if (CUserData::Instance()->GetUserState() != UsLogin) return true;

    ShowUserCenter();

    return true;
}

bool CWndMain::OnLeaveBtnUser(void* param) {
    return true;
}

bool CWndMain::OnBtnClickUser(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (!pNotify || !pNotify->pSender) return true;

    if (CUserData::Instance()->GetUserState() == UsLogin) ShowUserCenter();
    else if (CUserData::Instance()->GetUserState() == UsNone)
        ::PostMessage(m_hWnd, WM_MAINWND_MSG_COMMON, WpCommonLogin, 0);    // 登录

    return true;
}

void CWndMain::Exit() {
    RELEASE_SUB_PAGE(page_download_);

    CIosMgr::Instance()->CloseKeyWnd();

    Close();
}

void CWndMain::OnWndClose(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (web_home_page_) web_home_page_->Close();

    web_focus_ = nullptr;

    bHandled = FALSE;
}

void CWndMain::AddNotifyIcon() {
    ZeroMemory(&m_nd, sizeof(m_nd));
    m_nd.cbSize = sizeof(NOTIFYICONDATA);
    m_nd.hWnd = m_hWnd;
    m_nd.uID = IDI_APPLESEED;
    m_nd.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;//|NIF_INFO; 
    m_nd.uCallbackMessage = WM_MAINWND_MSG_NOTIFYICON;
    m_nd.hIcon = LoadIcon(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(IDI_SMALL));
    m_nd.uTimeout = 500;
    wcscpy_s(m_nd.szTip, NOTIFY_ICON_NAME);
    m_nd.szTip[_tcslen(m_nd.szTip)] = '\0';
    Shell_NotifyIcon(NIM_ADD, &m_nd);
}

void CWndMain::RemoveNotifyIcon() {
    Shell_NotifyIcon(NIM_DELETE, &m_nd);
}

void CWndMain::ExitAccount() {
    CUserData::Instance()->ClearUserInfo();
    CUserData::Instance()->ClearAccount();

    UpdateUserUI();
}

bool CWndMain::OnClickHomePage(void* lpParam) {
    TNotifyUI* pNotify = (TNotifyUI*)lpParam;
    if (!pNotify || !pNotify->pSender || !layout_main_ || !web_home_page_) return true;

    COptionUI* pOptHome = dynamic_cast<COptionUI*>(pNotify->pSender);
    if (!pOptHome) return true;

    auto page = pOptHome->GetTag();

    if (page == layout_main_->GetTag()) {
        if (page == web_home_page_->GetTag()) {
            ShowClientLayoutLeft(true);
            need_hide_left_layout_ = false;
            web_home_page_->NavigateUrl();
            web_focus_ = web_home_page_->GetCefWebkit();
        }

        return true;
    }

    if (page == web_home_page_->GetTag()) {
        if (need_hide_left_layout_) ShowClientLayoutLeft(false);
    }

    SwitchPage(page);

    return true;
}

bool CWndMain::OnClickHomeBbs(void* lpParam) {
    TNotifyUI* pNotify = (TNotifyUI*)lpParam;
    if (!pNotify || !pNotify->pSender || !layout_main_ || !web_home_page_) return true;

    COptionUI* pOptHome = dynamic_cast<COptionUI*>(pNotify->pSender);
    if (!pOptHome) return true;

    auto page = pOptHome->GetTag();

    if (page == layout_main_->GetTag()) return true;

    layout_main_->SetTag(page);

    ShellExecute(NULL, L"open", URL_IOS_BBS, NULL, NULL, SW_SHOW);

    return true;
}

bool CWndMain::OnClickHomeDownload(void* lpParam) {
    TNotifyUI* pNotify = (TNotifyUI*)lpParam;
    if (!pNotify || !pNotify->pSender || !layout_main_) return true;

    CButtonUI* btn_down_load = dynamic_cast<CButtonUI*>(pNotify->pSender);
    if (!btn_down_load) return true;

    if (btn_down_load->GetTag() == layout_main_->GetTag()) return true;

    ShowClientLayoutLeft(true);

    SwitchPage(btn_down_load->GetTag());

    return true;
}

bool CWndMain::OnClickInstallSys(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (!pNotify || !pNotify->pSender) return true;

    if (!install_system_ || !select_folder_) return true;

    install_system_->SetVisible(false);
    select_folder_->SetVisible(true);

    return true;
}

bool CWndMain::OnClickInstallCancel(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (!pNotify || !pNotify->pSender) return true;

    if (!install_system_ || !select_folder_) return true;

    install_system_->SetVisible(true);
    select_folder_->SetVisible(false);

    return true;
}

bool CWndMain::OnClickGetIsoSys(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (!pNotify || !pNotify->pSender) return true;

    if (!install_system_ || !select_folder_ || !layout_install_) return true;

    install_system_->SetVisible(true);
    select_folder_->SetVisible(false);
    layout_install_->SelectItem(1);

    DownloadMirrorSystem();

    return true;
}

bool CWndMain::OnClickInstallPause(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (!pNotify || !pNotify->pSender || !lbl_install_status_ || !lbl_install_speed_) return true;

    COptionUI *opt_install_pause = (COptionUI*)pNotify->pSender;
    if (!opt_install_pause) return true;

    if (opt_install_pause->IsSelected() && download_info_) {
        auto http = download_info_->http();
        if (http) http->Stop();
        download_info_ = nullptr;
        lbl_install_status_->SetText(L"暂停中");
        lbl_install_speed_->SetText(L"");
    }
    else DownloadMirrorSystem();

    return true;
}

bool CWndMain::OnClickInstallClose(void* param) {
    if (download_info_) {
        auto http = download_info_->http();
        http->NeedStop();
        download_info_ = nullptr;
    }

    if (progress_install_) progress_install_->SetValue(0);
    if (lbl_install_status_) lbl_install_status_->SetText(L"");
    if (lbl_install_speed_) lbl_install_speed_->SetText(L"");
    if (btn_install_restart_) btn_install_restart_->SetVisible(false);
    if (btn_install_pause_) btn_install_pause_->SetVisible(true);
    if (btn_install_pause_) btn_install_pause_->Selected(true);

    install_system_->SetVisible(false);
    select_folder_->SetVisible(true);
    layout_install_->SelectItem(0);

    return true;
}

bool CWndMain::OnClickInstallRestart(void* param) {
    if (btn_install_restart_) btn_install_restart_->SetVisible(false);
    if (btn_install_pause_) btn_install_pause_->SetVisible(true);

    DownloadMirrorSystem();

    return true;
}

bool CWndMain::OnBtnClickSnap(void* param) {
    //bool success = CIosMgr::Instance()->IosSnap("D:\\PC\\trunk\\ios\\bin\\Debug\\Snap\\12.jpg");
    return true;
}

bool CWndMain::OnBtnClickRecord(void* param) {
    return true;
}

bool CWndMain::OnBtnClickKey(void* param) {
    CIosMgr::Instance()->UpdateBrowserMode(false);
    return true;
}

bool CWndMain::OnBtnShowPerOpitimiz(void* param) {
    ShowPerOptimizWnd();
    return true;
}

bool CWndMain::LoadIosEngine() {
    string iso_file = PublicLib::UToA(CGlobalData::Instance()->GetIosVmPath()) + "\\"+ GetUrlIsoName();
    PostStartApp(StatusBegin, CUserData::Instance()->GetFileUserID());
    auto success = CIosMgr::Instance()->IosEngineOn(iso_file);
    if (!success) PostStartApp(StatusFail, CUserData::Instance()->GetFileUserID());
    return success;
}

void CWndMain::UpdateLoadingIcon() {
    if (!install_loading_) return;

    CStdString image;
    image.Format(L"middle/loading_%d.png", loading_frame_);

    install_loading_->SetBkImage(image.GetData());
}

void CWndMain::UnzipMirrorSystem() {
    MSG msg = { 0 };
    msg.hwnd = m_hWnd;
    msg.message = WM_MAINWND_MSG_FILE_UNZIPING;

    std::wstring strOutDir = CGlobalData::Instance()->GetIosVmPath();
    std::wstring save_path = CGlobalData::Instance()->GetIosPath() +  L"\\" + PublicLib::Utf8ToU(GetUrlPackageName());

    TaskCenter::CTaskCenter::Instance()->CreateUnzipFileTask(msg, WM_MAINWND_MSG_FILE_UNZIP, save_path, strOutDir);
}

LRESULT CWndMain::OnDownloadMirrorSystem(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    switch (wParam) {
    case 0: {
        download_info_ = nullptr;
        if (lbl_install_status_) lbl_install_status_->SetText(L"下载失败请重试....");
        if (lbl_install_speed_) lbl_install_speed_->SetText(L"");
        if (btn_install_restart_) btn_install_restart_->SetVisible(true);
        if (btn_install_pause_) btn_install_pause_->SetVisible(false);
        if (progress_install_) progress_install_->SetValue(0);
    }
    break;
    case 1: {
        if (!download_info_) break;
        wstring strSpeed = PublicLib::SpeedToString(download_info_->speed());
        if (lbl_install_speed_) lbl_install_speed_->SetText(strSpeed.c_str());

        CDuiString strStateText;
        wstring strTotal = PublicLib::SizeToString(download_info_->total_size());
        wstring strLoad = PublicLib::SizeToString(download_info_->load_size());
        strStateText.Format(L"已下载 %s/%s", strLoad.c_str(), strTotal.c_str());
        lbl_install_status_->SetText(strStateText.GetData());

        if (progress_install_) progress_install_->SetValue(lParam);
    }
    break;
    case 2: {
        download_info_ = nullptr;
        if (lbl_install_status_) lbl_install_status_->SetText(L"下载完成");
        if (lbl_install_speed_) lbl_install_speed_->SetText(L"");
        if (progress_install_) progress_install_->SetValue(100);
        
        UnzipMirrorSystem();
        if (layout_install_) layout_install_->SelectItem(2);
        loading_frame_ = 0;
        UpdateLoadingIcon();
        ::SetTimer(m_hWnd, TIMER_ID_DOWNLOAD_PROGRESS, DOWNLOAD_PROGRESS_TIME, nullptr);
    }
    break;
    default:
        break;
    }

    return 0;
}

LRESULT CWndMain::OnMsgFileUnziping(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    return 0;
}

LRESULT CWndMain::OnMsgFileUnzip(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (wParam == 1) {
        KillTimer(m_hWnd, TIMER_ID_DOWNLOAD_PROGRESS);
        if (layout_install_) layout_install_->SelectItem(0);
        if (install_system_) install_system_->SetVisible(false);
        if (select_folder_) select_folder_->SetVisible(true);
        return 0;
    } 

    string iso_file = PublicLib::UToA(CGlobalData::Instance()->GetIosVmPath()) + "\\" + GetUrlIsoName();
    string iso_init_file = PublicLib::UToA(CGlobalData::Instance()->GetIosVmPath()) + "\\" + GetUrlInitIsoName();

    if (!MoveFileA(iso_init_file.c_str(), iso_file.c_str())) {
        KillTimer(m_hWnd, TIMER_ID_DOWNLOAD_PROGRESS);
        if (layout_install_) layout_install_->SelectItem(0);
        if (install_system_) install_system_->SetVisible(false);
        if (select_folder_) select_folder_->SetVisible(true);
        return 0;
    }

    std::wstring save_path = CGlobalData::Instance()->GetIosPath() + L"\\" + PublicLib::Utf8ToU(GetUrlPackageName());
    if (PathFileExists(save_path.c_str())) DeleteFile(save_path.c_str());

    if (!LoadIosEngine()) {
        ShowMsg(m_hWnd, L"提示", L"启动失败，请重启", MB_OK);
        CIosMgr::Instance()->CreateEngineOffTask();
        return 0;
    }

    return 0;
}

LRESULT CWndMain::OnMsgEmulatorAlready(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (layout_install_) layout_install_->SelectItem(3);
    KillTimer(m_hWnd, TIMER_ID_DOWNLOAD_PROGRESS);
    return 0;
}

LRESULT CWndMain::OnMsgLoadIosEngine(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (!LoadIosEngine()) {
        ShowMsg(m_hWnd, L"提示", L"启动失败，请重启", MB_OK);
        CIosMgr::Instance()->CreateEngineOffTask();
        return 0;
    }

    if (layout_install_) layout_install_->SelectItem(2);
    loading_frame_ = 0;
    UpdateLoadingIcon();
    ::SetTimer(m_hWnd, TIMER_ID_DOWNLOAD_PROGRESS, DOWNLOAD_PROGRESS_TIME, nullptr);
    return 0;
}

LRESULT CWndMain::OnMsgExit(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    Exit();
    return 0;
}

LRESULT CWndMain::OnTimer(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    switch (wParam) {
    case TIMER_ID_DOWNLOAD_PROGRESS: {
        loading_frame_++;
        if (loading_frame_ >= 3) loading_frame_ = 0;
        UpdateLoadingIcon();
        break;
    }
    case TIMER_ID_CHECK_UPDATE:
        {
            ::KillTimer(m_hWnd, TIMER_ID_CHECK_UPDATE);
            wstring strUpdatePath = CGlobalData::Instance()->GetRunPath() + EXE_UPDATE;
            if (!PathFileExists(strUpdatePath.c_str()))
            {
                OUTPUT_XYLOG(LEVEL_ERROR, L"没有找到升级程序！");
                break;
            }
            PublicLib::ShellExecuteRunas(strUpdatePath.c_str(), L"/autoupdate", NULL);
        }
        break;
    default:
        break;
    }
    return 0;
}

void CWndMain::DownloadMirrorSystem() {
    std::string url = "http://guorenres.5fun.com/mirror/" + GetUrlPackageName();
    std::wstring save_path = CGlobalData::Instance()->GetIosPath() + L"\\";
    if (!PathFileExists(save_path.c_str())) SHCreateDirectory(nullptr, save_path.c_str());

    save_path += PublicLib::Utf8ToU(GetUrlPackageName());

    if (PathFileExists(save_path.c_str())) {
        UnzipMirrorSystem();
        if (layout_install_) layout_install_->SelectItem(2);
        loading_frame_ = 0;
        UpdateLoadingIcon();
        ::SetTimer(m_hWnd, TIMER_ID_DOWNLOAD_PROGRESS, DOWNLOAD_PROGRESS_TIME, nullptr);
        return;
    }

    bool success = false;

    do {
        download_info_ = new download::DownloadInfo();
        if (!download_info_) break;

        auto http = download_info_->http();
        if (!http) break;

        if (!http->Initialize(url, save_path, TRUE, this, LoadMirrorSystemCallback)) {
            // 初始化下载失败
            OUTPUT_XYLOG(LEVEL_ERROR, L"初始化下载任务失败，下载地址：%s，保存路径：%s", PublicLib::Utf8ToU(url).c_str(), save_path.c_str());
            break;
        }

        if (!http->Start()) {
            //  开始下载失败
            OUTPUT_XYLOG(LEVEL_ERROR, L"开始下载任务失败，下载地址：%s，保存路径：%s", PublicLib::Utf8ToU(url).c_str(), save_path.c_str());
            break;
        }

        download_info_->set_save_path(save_path);
        download_info_->set_url(url);

        success = true;
    } while(false);

    if (!success) {
        if (lbl_install_status_) lbl_install_status_->SetText(L"下载失败请重试....");
        if (lbl_install_speed_) lbl_install_speed_->SetText(L"");
        if (btn_install_restart_) btn_install_restart_->SetVisible(true);
        if (btn_install_pause_) btn_install_pause_->SetVisible(false);
        download_info_ = nullptr;
    }
}

void CWndMain::LoadMirrorSystemCallback(PublicLib::EnumDownloadState state, double dltotal, double dlnow) {
    switch (state) {
    case PublicLib::STATE_DOWNLOADING: {
        if (!download_info_) break;
        if (download_info_->total_size() == 0)
            download_info_->set_total_size((__int64)dltotal);

        __int64 nAdd = int(dlnow - download_info_->load_size());
        download_info_->set_load_size((__int64)dlnow);
        __int64 nCurTime = (__int64)time(NULL);

        if (download_info_->last_time() == 0 || nCurTime == download_info_->last_time()) {
            download_info_->set_last_time(nCurTime);
            download_info_->set_size(download_info_->size() + nAdd);
            break;
        }

        // 计算下载速度
        download_info_->set_speed((int)((download_info_->size() / (nCurTime - download_info_->last_time())) / 1024));
        download_info_->set_size(0);
        download_info_->set_last_time(nCurTime);

        auto nLoadSize = download_info_->load_size();
        auto nTotalSize = download_info_->total_size();
        int nPercent = (nTotalSize == 0) ? 0 : (int)((100 * nLoadSize / nTotalSize));

        ::PostMessage(m_hWnd, WM_MAINWND_MSG_DOWNLOAD_MIRRORSYSTEM, 1, nPercent);
    }
    break;
    case STATE_DOWNLOAD_HAS_FINISHED: {
        if (!download_info_) break;
        download_info_->set_date(time(NULL));

        ::PostMessage(m_hWnd, WM_MAINWND_MSG_DOWNLOAD_MIRRORSYSTEM, 2, 100);
        break;
    }
    case STATE_DOWNLOAD_HAS_FAILED: {
        ::PostMessage(m_hWnd, WM_MAINWND_MSG_DOWNLOAD_MIRRORSYSTEM, 0, 0);
    }
    break;
    default: break;
    }
}

void CWndMain::LoadMirrorSystemCallback(PublicLib::EnumDownloadState state, double dltotal, double dlnow, void * Userdata) {
    auto main_ptr = (CWndMain*)Userdata;
    if (main_ptr) return main_ptr->LoadMirrorSystemCallback(state, dltotal, dlnow);
}

void CWndMain::SwitchPage(int page) {
    if (!layout_main_ || !web_home_page_) return;

    web_focus_ = nullptr;

    layout_main_->SelectItem(page - 1);
    layout_main_->SetTag(page);

    if (page == web_home_page_->GetTag()) web_focus_ = web_home_page_->GetCefWebkit();

    UpdateTopWebCtrls();
}

void CWndMain::UpdateTopWebCtrls() {
    if (nullptr == web_focus_) return;
    
    HWND hWnd = web_focus_->GetHostWnd();
    SetFocus(hWnd);
}

void CWndMain::ShowVolumeWnd() {
    if (!btn_ios_volume_) return;

    CWndVolume* wnd_volume = new CWndVolume();
    if (!wnd_volume) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"malloc volume window failed, nullptr!");
        return;
    }

    wnd_volume->Create(m_hWnd, false);

    QRect rc_volume(0, 0, 0, 0);
    GetClientRect(*wnd_volume, &rc_volume);

    const RECT& rc = btn_ios_volume_->GetPos();
    POINT pt = { (rc.left + rc.right - rc_volume.GetWidth()) / 2, rc.top - rc_volume.GetHeight() - 12 };
    ClientToScreen(m_hWnd, &pt);

    ::SetForegroundWindow(*wnd_volume);
    ::MoveWindow(*wnd_volume, pt.x, pt.y, rc_volume.GetWidth(), rc_volume.GetHeight(), FALSE);

    wnd_volume->ShowWindow(true);
}

void CWndMain::ShowUserWnd() {
    CWndUser* pWnd = new CWndUser();
    if (!pWnd) return;

    pWnd->Create(m_hWnd, false);
    pWnd->CenterWindow();
    pWnd->ShowModal();
}

void CWndMain::ShowPerOptimizWnd() {
    CWndPerOptimiz* wnd_ptr = new CWndPerOptimiz();
    if (!wnd_ptr) return;

    HWND hWnd = wnd_ptr->CreateModalWnd(m_hWnd);
    ShowModalWnd(hWnd);
}

void CWndMain::SelectedHomePage(COptionUI* opt_select) {
    if (opt_select && layout_main_ && layout_main_->GetTag() != opt_select->GetTag()) {
        opt_select->Selected(true);
        SwitchPage(opt_select->GetTag());
    }
}

void CWndMain::RemoveSpilthVmdk() {
    wstring iso_path = CGlobalData::Instance()->GetIosVmPath();
    PublicLib::RemoveDir(iso_path.c_str());
}

string CWndMain::GetUrlPackageName() {
    return (CGlobalData::Instance()->GetCpuType() == 0 ? URL_PACKAGE_NAME_INTEL : URL_PACKAGE_NAME_AMD);
}

string CWndMain::GetUrlInitIsoName() {
    return (CGlobalData::Instance()->GetCpuType() == 0 ? URL_ISO_NAME_INIT_INTEL : URL_ISO_NAME_INIT_AMD);
}

string CWndMain::GetUrlIsoName(){
    return (CGlobalData::Instance()->GetCpuType() == 0 ? URL_ISO_NAME_INTEL : URL_ISO_NAME_AMD);
}

LRESULT CWndMain::OnMsgUserInfo(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    UINT_PTR nTask = (UINT_PTR)lParam;
    string strJson;
    if (!TaskCenter::CTaskCenter::Instance()->GetTaskResult(nTask, strJson)) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"错误的任务ID");
        return 0;
    }

    Json::Value vRoot;
    Json::Reader rd;
    if (!rd.parse(strJson, vRoot)) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"json解析失败");
        return 0;
    }

    string strCode = vRoot["code"].asString();
    if (strCode.compare("600") != 0) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"获取用户信息失败，code = %s", PublicLib::Utf8ToU(strCode).c_str());
        wstring str_msg = PublicLib::Utf8ToU(vRoot["msg"].asString());
        ShowToast(m_hWnd, str_msg.empty() ? L"获取用户信息失败" : str_msg.c_str());
        return 0;
    }

    try {
        int nParamUid = 0;
        string strParamToken;
        if (!TaskCenter::CTaskCenter::Instance()->GetUserInfoTaskParam(nTask, nParamUid, strParamToken)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"任务中心获取任务信息失败");
            return 0;
        }

        bool bUpdateInfo = false;
        if (!TaskCenter::CTaskCenter::Instance()->IsUpdateUserInfoTask(nTask, bUpdateInfo)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"任务中心获取任务信息失败");
            return 0;
        }

        Json::Value& vUserInfo = vRoot["data_info"];
        int nUid = 0;
        if (vUserInfo["uid"].isInt()) {
            nUid = vUserInfo["uid"].asInt();
        }
        else if (vUserInfo["uid"].isString())  {
            string str = vUserInfo["uid"].asString();
            nUid = atoi(str.c_str());
        }

        string strPhone = vUserInfo["mobile"].asString();

        Json::Value& vUser = CUserData::Instance()->GetUserInfo();
        Json::Value::Members vecMems = vUserInfo.getMemberNames();
        for (size_t i = 0; i < vecMems.size(); ++i) {
            string& strName = vecMems[i];
            vUser[strName] = vUserInfo[strName];
        }

        vUser["token"] = strParamToken;
        vUser["uid"] = nUid;
        Json::FastWriter fw;
        string strJson = fw.write(vUser);
        strcpy_s(((LPCEF_SHARE_DATA)g_shareData)->szUserInfo, strJson.c_str());

#ifdef _DEBUG
        OutputDebugString(PublicLib::Utf8ToU(strJson).c_str());
        OutputDebugString(L"\r\n");
#endif

        if (bUpdateInfo) return 0;

        CUserData::Instance()->SetFileAccount(nUid, strParamToken);
        CUserData::Instance()->SetAccountPhone(strPhone);
        CUserData::Instance()->SetUserState(UsLogin);

        wchar_t szUid[20] = { 0 };
        swprintf_s(szUid, L"%d", nUid);
        wstring strUserPath = GetAppDataPath() + szUid;
        SHCreateDirectory(NULL, strUserPath.c_str());
        CUserData::Instance()->SetUserPath(strUserPath);

        PostUserLogin(nUid);

        UpdateUserUI();
    }
    catch (...) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"解析服务器配置失败");
    }

    return 0;
}

LRESULT CWndMain::OnMsgCommon(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    switch (wParam) {
    case WpCommonLogin: ShowUserWnd();break;
    case WpCommonUserInfo: {
        need_hide_left_layout_ = true;
        ShowClientLayoutLeft(false);
        SelectedHomePage(opt_home_market_);
        web_home_page_->NavigateUrl(L"http://testguoren.abcs8.com/static/v1/memberCenter.html");
        break;
    }
    case WpCommonSwitchAccount: {
        ExitAccount();
        ::PostMessage(m_hWnd, WM_MAINWND_MSG_COMMON, WpCommonLogin, 0);
        if (web_home_page_) web_home_page_->NavigateUrl();
        need_hide_left_layout_ = false;
        SelectedHomePage(opt_home_market_);
        ShowClientLayoutLeft(true);
        break;
    }
    case WpCommonModifyUserInfo: {
        uint32_t nUid = CUserData::Instance()->GetFileUserID();
        string strToken = CUserData::Instance()->GetFileUserToken();

        // 验证账号信息
        MSG msg = { 0 };
        msg.hwnd = m_hWnd;
        msg.message = WM_MAINWND_MSG_USERINFO;
        TaskCenter::CTaskCenter::Instance()->CreateUserInfoTask(msg, nUid, strToken);
        break;
    }
    case WpCommonUpdateLoadCount: {
        if (!label_load_count_) break;

        int nCount = (int)lParam;

        CDuiString str;
        if (nCount <= 0) str = L"";
        else if (nCount < 100) str.Format(L"%d", nCount);
        else str = L"...";

        bool show = (nCount > 0);

        label_load_count_->SetText(str.GetData());
        label_load_count_->SetVisible(show);

        break;
    }
    default:
        break;
    }

    return 0;
}

void CWndMain::UpdateUserUI(bool bClear/* = false*/) {
    if (!btn_user_icon_ || !user_mask_ico_) return;

    if (CUserData::Instance()->GetUserState() == UsLogin) {
        btn_user_icon_->SetToolTip(L"");
        UpdateUserIco();
    }
    else {
        btn_user_icon_->SetToolTip(L"点击登录");
        user_mask_ico_->SetVisible(false);
        btn_user_icon_->SetBkImage(btn_user_icon_->GetUserData().GetData());
    }
}

void CWndMain::UpdateUserIco() {
    if (!btn_user_icon_ || !user_mask_ico_) return;

    Json::Value& vUser = CUserData::Instance()->GetUserInfo();
    wchar_t szUid[10] = { 0 };
    try {
        swprintf_s(szUid, L"%u", vUser["uid"].asInt());
    }
    catch (...) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"用户数据中uid类型转换失败");
    }

    string strName = vUser["avatarUrl"].asString();
    if (strName.empty()) {
        user_mask_ico_->SetVisible(false);
        btn_user_icon_->SetBkImage(btn_user_icon_->GetUserData().GetData());
        return;
    }

    string strUrl;
    if (strName.find("http") != string::npos) strUrl = strName;
    else {
        user_mask_ico_->SetVisible(false);
        btn_user_icon_->SetBkImage(btn_user_icon_->GetUserData().GetData());
        return;
    }

    CGlobalData::Instance()->SetUserIcoPath(strUrl);
    PublicLib::MD5 md5(strUrl);
    string strMd5 = md5.md5();
    const wstring &strUserPath = CUserData::Instance()->GetUserPath();
    wstring strIcoPath = strUserPath + L"\\" + PublicLib::AToU(strMd5) + L".png";

    if (!PathFileExists(strIcoPath.c_str())) {
        MSG msg;
        msg.hwnd = m_hWnd;
        msg.message = WM_MAINWND_MSG_USER_ICO;
        TaskCenter::CTaskCenter::Instance()->CreateUserIcoTask(msg, PublicLib::Utf8ToU(strUrl), strIcoPath);
    }
    else {
        btn_user_icon_->SetBkImage(strIcoPath.c_str());
        user_mask_ico_->SetVisible(true);
    }
}

LRESULT CWndMain::OnMsgUserIco(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (!btn_user_icon_ || !user_mask_ico_) return 0;

    UINT_PTR nTask = (UINT_PTR)lParam;
    string strRet;
    if (!TaskCenter::CTaskCenter::Instance()->GetTaskResult(nTask, strRet)) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"错误的任务ID");
        return 0;
    }

    if (strRet.compare("1") == 0) {
        wstring strIcoPath;
        wstring strIcoUrl;
        TaskCenter::CTaskCenter::Instance()->GetUserIcoPath(nTask, strIcoPath, strIcoUrl);
        if (PathFileExists(strIcoPath.c_str())) {
            btn_user_icon_->SetBkImage(strIcoPath.c_str());
            user_mask_ico_->SetVisible(true);
            CUserData::Instance()->RecordLastHead(strIcoPath, strIcoUrl);
        }
    }
    else {
        OUTPUT_XYLOG(LEVEL_ERROR, L"用户头像下载失败, 使用上一次的头像");

        wstring strIcoPath;
        wstring strIcoUrl;
        CUserData::Instance()->GetLastHead(strIcoPath, strIcoUrl);
        if (PathFileExists(strIcoPath.c_str())) {
            btn_user_icon_->SetBkImage(strIcoPath.c_str());
            user_mask_ico_->SetVisible(true);

            Json::Value& vUser = CUserData::Instance()->GetUserInfo();
            Json::FastWriter fw;
            string user_json = fw.write(vUser);
            strcpy_s(((LPCEF_SHARE_DATA)g_shareData)->szUserInfo, user_json.c_str());
        }
    }

    return 0;
}

LRESULT CWndMain::OnMsgIosEngineUpdating(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (!layout_update_ || !progress_update_ || !lbl_update_status_) return 0;

    layout_update_->SetVisible(true);

    if (lParam < 0) {
        progress_update_->SetValue(0);

        CStdString str_progress;
        str_progress.Format(L"更新失败");
        lbl_update_status_->SetText(str_progress.GetData());
    }
    else {
        progress_update_->SetValue(lParam);

        CStdString str_progress;
        str_progress.Format(L"更新中 %d%s", lParam, "%");
        lbl_update_status_->SetText(str_progress.GetData());
    }

    return 0;
}

LRESULT CWndMain::OnMsgIosEngineUpdate(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (wParam == 0 || wParam == 1) {
        if (layout_update_) layout_update_->SetVisible(false);
        if (progress_update_) progress_update_->SetValue(0);
        if (lbl_update_status_) lbl_update_status_->SetText(L"");
    }

    CIosMgr::Instance()->OnPackUpdate(wParam, lParam == 1);
    return 0;
}

LRESULT CWndMain::OnMsgIosEngineApplication(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (page_download_) page_download_->UpdateLoadLayout();
    return 0;
}

LRESULT CWndMain::OnMsgHotkey(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (wParam == ID_HOTKEY_BOS) {
        auto fuModifiers = (UINT)LOWORD(lParam);     // 辅助按键 
        auto uVirtKey = (UINT)HIWORD(lParam);        // 键值 

        bool programme_mode = CGlobalData::Instance()->ProgrammeMode();
        CGlobalData::Instance()->SetProgrammeMode(!programme_mode);
        CIosMgr::Instance()->UpdateIosWndStatus();
        ShowToast(m_hWnd, programme_mode ? L"已关闭编程键" : L"已开启编程键");
    }

    return 0;
}

LRESULT CWndMain::OnMsgShowPerOptimizIcon(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (btn_per_opitimiz_) {
        bool show = (wParam == 1);
        btn_per_opitimiz_->SetNormalImage(show ? btn_per_opitimiz_->GetUserData().GetData() : L"");
        btn_per_opitimiz_->SetEnabled(show);
    }
    return 0;
}

LRESULT CWndMain::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (wParam == SC_CLOSE && QuitOnSysClose())
        return 0;

    BOOL bZoomed = ::IsZoomed(m_hWnd);
    LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    if (wParam == SC_CLOSE)
        return 0;

    if (::IsZoomed(m_hWnd) != bZoomed)  {
        CControlUI* pCtrl = m_pm.FindControl(L"btn_sys_max");
        if (pCtrl)
            pCtrl->SetVisible(TRUE == bZoomed);
        pCtrl = m_pm.FindControl(L"btn_sys_restore");
        if (pCtrl)
            pCtrl->SetVisible(FALSE == bZoomed);

        if (FALSE == bZoomed) CIosMgr::Instance()->UpdateBrowserMode(true);
    }

    return lRes;
}

LRESULT CWndMain::OnMsgGameCheckUpdateGame(WPARAM wParam, LPARAM lParam) {
    /*{"error":0,"data":[{"game_id":"105674098","is_low_version":false,"version_code":"1.1.2"},{"game_id":"122078927","is_low_version":false,"version_code":"1.1"}, */
    if (!page_download_) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"房间指针pLocalPage为空");
        return -1;
    }

    UINT_PTR nTaskID = (UINT_PTR)lParam;
    string strJson;
    if (!TaskCenter::CTaskCenter::Instance()->GetTaskResult(nTaskID, strJson))
    {
        OUTPUT_XYLOG(LEVEL_ERROR, L"错误的任务ID");
        return -1;
    }

    Json::Value vRoot;
    Json::Reader rd;
    if (!rd.parse(strJson, vRoot))
    {
        OUTPUT_XYLOG(LEVEL_ERROR, L"json解析失败");
        return -1;
    }
    try
    {
        string strCode = vRoot["code"].asString();
        if (strCode.compare("600") != 0) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"接口返回失败，code = %s", PublicLib::Utf8ToU(strCode).c_str());
            return 0;
        }

        Json::Value& vData = vRoot["data_info"];
        if (vData.size() < 1)
        {
            OUTPUT_XYLOG(LEVEL_INFO, L"没有查询到游戏更新信息");
            return 0;
        }

        for (size_t i = 0; i < vData.size(); ++i) {
            auto strGameID = vData[i]["game_id"].asString();
            auto version = vData[i]["game_ver"].asString();
            page_download_->UpdateGameStatus(PublicLib::Utf8ToU(strGameID), PublicLib::Utf8ToU(version));
        }
    }
    catch (...)
    {
        OUTPUT_XYLOG(LEVEL_ERROR, L"json解析失败");
    }

    page_download_->PostNeedUpdateGameCount();

    return 0;
}