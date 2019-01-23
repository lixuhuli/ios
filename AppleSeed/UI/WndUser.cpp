#include "StdAfx.h"
#include "WndUser.h"
#include "WndPassword.h"
#include "TaskCenter/TaskCenter.h"
#include "MsgDefine.h"
#include "CommonWnd.h"
#include "UrlDefine.h"
#include "GlobalData.h"
#include "UserData.h"
#include "WndUserItem.h"

#define MOBILE_CODE_REG           L"reg"
#define MOBILE_CODE_LOGIN         L"login"
#define MOBILE_CODE_FINDPWD       L"findPwd"

CWndUser::CWndUser()
 : layout_user_(nullptr)
 , btn_back_(nullptr)
 , edit_phone_(nullptr)
 , btn_forget_password_(nullptr)
 , btn_verification_code_(nullptr)
 , btn_next_(nullptr)
 , edit_verification_code_(nullptr) 
 , edit_active_code_(nullptr)
 , verification_count_down_(0)
 , edit_password_(nullptr)
 , edit_confirm_password_(nullptr)
 , check_show_password_(nullptr)
 , btn_login_(nullptr)
 , btn_login2_(nullptr)
 , btn_confirm_(nullptr)
 , edit_input_user_(nullptr)
 , edit_input_password_(nullptr)
 , btn_verification_code2_(nullptr)
 , edit_phone2_(nullptr)
 , btn_next2_(nullptr) 
 , btn_login3_(nullptr)
 , edit_verification_code2_(nullptr)
 , verification_count_down2_(0)
 , lbl_password_prefix_(nullptr)
 , check_remember_password_(nullptr)
 , check_auto_login_(nullptr) {
}

CWndUser::~CWndUser() {
}

LRESULT CWndUser::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;

    switch (uMsg) {
    case WM_USERWND_MSG_GET_VERIFICATION: OnGetVerificationCode(wParam, lParam, bHandled); break;
    case WM_USERWND_MSG_GET_VERIFICATION2: OnGetVerificationCode2(wParam, lParam, bHandled); break;
    case WM_USERWND_MSG_USER_CHECKCODE: OnUserCheckCode(wParam, lParam, bHandled); break;
    case WM_USERWND_MSG_USER_REGISTER: OnUserRegister(wParam, lParam, bHandled); break;
    case WM_USERWND_MSG_USER_LOGIN: return OnUserLogin(wParam, lParam, bHandled);
    case WM_TIMER: OnTimer(wParam, lParam, bHandled); bHandled = FALSE; break;
    case WM_USERWND_MSG_USER_FIND_PASSWORD: return OnUserFindPassword(wParam, lParam, bHandled);
    case WM_USERWND_MSG_USER_CODE_LOGIN: return OnUserCodeLogin(wParam, lParam, bHandled);
    case WM_USERWND_MSG_USER_CHECKMOBILECODE: OnUserCheckMobileCode(wParam, lParam, bHandled); break;
    case WM_USERWND_MSG_USER_MODIFYPASSWORD: OnUserModifyPassword(wParam, lParam, bHandled); break;
    case WM_KEYDOWN: OnUserKeyDown(wParam, lParam, bHandled); break;
    default: bHandled = FALSE;
    }

    if (bHandled) {
        return lRes;
    }

    return __super::HandleMessage(uMsg, wParam, lParam);
}

LPCWSTR CWndUser::GetXmlPath() const{
	return L"WndUser.xml";
}

void CWndUser::InitWindow() {
    __super::InitWindow();

    if (!edit_input_user_ || !edit_input_password_ || !edit_phone2_ || !check_auto_login_ || !check_remember_password_) return;

    if (!CUserData::Instance()->HasFileAccount()) return;

    auto nUid = CUserData::Instance()->GetFileUserID();
    wstring strAccount = CUserData::Instance()->GetAccount();
    string strPhone = CUserData::Instance()->GetFileUserPhone();

    if (strAccount.empty()) strAccount = PublicLib::AToU(strPhone);

    if (strAccount.empty() && strPhone.empty()) {
        wchar_t szValue[128] = { 0 };
        swprintf_s(szValue, L"%d", nUid);
        edit_input_user_->SetText(szValue);
    }
    else {
        edit_phone2_->SetText(PublicLib::AToU(strPhone).c_str());
        edit_input_user_->SetText(strAccount.c_str());

        bool enabled = (edit_phone2_->GetText().GetLength() >= 11);
        btn_verification_code2_->SetEnabled(enabled);
    }

    check_auto_login_->Selected(CUserData::Instance()->GetAutoLogin());
    check_remember_password_->Selected(CUserData::Instance()->GetRememberPwd());

    string strPassword = CUserData::Instance()->GetFileUserPassword();

    if (!strPassword.empty()) edit_input_password_->SetText(PublicLib::AToU(strPassword).c_str());

    UpdateBtnLoginStatus();

    IntEditEvent();
}

void CWndUser::IntEditEvent() {
    if (edit_input_user_) edit_input_user_->Subscribe(DUI_MSGTYPE_TABCHAR, MakeDelegate([&]()->bool {
        if (edit_input_password_) {
            edit_input_password_->SetFocus();
            edit_input_password_->SetSel(0, -1);
        }
        return true;
    }));

    if (edit_input_password_) edit_input_password_->Subscribe(DUI_MSGTYPE_TABCHAR, MakeDelegate([&]()->bool {
        if (edit_input_user_) {
            edit_input_user_->SetFocus();
            edit_input_user_->SetSel(0, -1);
        }
        return true;
    }));

    if (edit_phone_) edit_phone_->Subscribe(DUI_MSGTYPE_TABCHAR, MakeDelegate([&]()->bool {
        if (edit_verification_code_) {
            edit_verification_code_->SetFocus();
            edit_verification_code_->SetSel(0, -1);
        }
        return true;
    }));

    if (edit_verification_code_) edit_verification_code_->Subscribe(DUI_MSGTYPE_TABCHAR, MakeDelegate([&]()->bool {
        if (edit_active_code_) {
            edit_active_code_->SetFocus();
            edit_active_code_->SetSel(0, -1);
        }
        return true;
    }));

    if (edit_active_code_) edit_active_code_->Subscribe(DUI_MSGTYPE_TABCHAR, MakeDelegate([&]()->bool {
        if (edit_phone_) {
            edit_phone_->SetFocus();
            edit_phone_->SetSel(0, -1);
        }
        return true;
    }));

    if (edit_phone2_) edit_phone2_->Subscribe(DUI_MSGTYPE_TABCHAR, MakeDelegate([&]()->bool {
        if (edit_verification_code2_) {
            edit_verification_code2_->SetFocus();
            edit_verification_code2_->SetSel(0, -1);
        }
        return true;
    }));

    if (edit_verification_code2_) edit_verification_code2_->Subscribe(DUI_MSGTYPE_TABCHAR, MakeDelegate([&]()->bool {
        if (edit_phone2_) {
            edit_phone2_->SetFocus();
            edit_phone2_->SetSel(0, -1);
        }
        return true;
    }));
}

LRESULT CWndUser::OnUserKeyDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (wParam == VK_RETURN) {
        if (layout_user_->GetCurSel() == 0) {
            if (btn_login_ && btn_login_->IsEnabled()) { 
                OnBtnClickLogin(nullptr);
            }
        }
    }

    return 0;
}

bool CWndUser::OnBtnClickBack(void* param) {
    if (!layout_user_) return true;
    layout_user_->SelectItem(0);
    UpdateWindowPos(false);
    return true;
}

bool CWndUser::OnBtnClickForgetPassword(void* param) {
    if (!btn_forget_password_) return true;

    CWndPassword* wnd_password = new CWndPassword();
    if (!wnd_password) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"malloc password window failed, nullptr!");
        return true;
    }

    wnd_password->Create(m_hWnd, false);

    QRect rc_volume(0, 0, 0, 0);
    GetClientRect(*wnd_password, &rc_volume);

    const RECT& rc = btn_forget_password_->GetPos();
    POINT pt = { (rc.left + rc.right - rc_volume.GetWidth()) / 2, rc.top - rc_volume.GetHeight() + 1 };
    ClientToScreen(m_hWnd, &pt);

    ::SetForegroundWindow(*wnd_password);
    ::MoveWindow(*wnd_password, pt.x, pt.y, rc_volume.GetWidth(), rc_volume.GetHeight(), FALSE);

    wnd_password->ShowWindow(true);

    return true;
}

bool CWndUser::OnBtnClickClose(void* param) {
    Close(IDCLOSE);
    return true;
}

bool CWndUser::OnBtnClickRegister(void* param) {
    if (layout_user_) {
        layout_user_->SelectItem(1);
        UpdateWindowPos(true);
    }
    return true;
}

bool CWndUser::OnEditInputUserChanged(void* param) {
    UpdateBtnLoginStatus();
    return true;
}

bool CWndUser::OnEditInputPasswordTextChanged(void* param) {
    UpdateBtnLoginStatus();
    return true;
}

bool CWndUser::OnEditReturn(void* param) {
    if (btn_login_ && btn_login_->IsEnabled()) { 
        OnBtnClickLogin(nullptr);
    }

    return true;
}

void CWndUser::UpdateBtnLoginStatus() {
    if (!btn_login_ || !edit_input_user_ || !edit_input_password_) return;

    auto password_length = edit_input_password_->GetText().GetLength();
    bool enabled = (password_length >= 8 && password_length <= 16 && !edit_input_user_->GetText().IsEmpty());
    btn_login_->SetEnabled(enabled);
}

void CWndUser::UpdateWindowPos(bool min_size) {
    auto sz = (min_size ? m_pm.GetMinInfo() : m_pm.GetMaxInfo());

    QRect rc;
    GetWindowRect(m_hWnd, &rc);

    rc.right = rc.left + sz.cx;

    ::SetWindowPos(m_hWnd, nullptr, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), SWP_NOZORDER | SWP_SHOWWINDOW);
}

bool CWndUser::OnBtnClickVerificationCode(void* param) {
    if (!edit_phone_) return true;

    wstring phone_number = edit_phone_->GetText().GetData();

    MSG msg = { 0 };
    msg.hwnd = m_hWnd;
    msg.message = WM_USERWND_MSG_GET_VERIFICATION;
    TaskCenter::CTaskCenter::Instance()->CreateGetVerificationCode(msg, phone_number, MOBILE_CODE_REG);

    return true;
}

bool CWndUser::OnBtnClickNext(void* param) {
    if (!edit_phone_ || !edit_verification_code_ || !edit_active_code_ || !btn_verification_code_) return true;

    KillTimer(m_hWnd, TIMER_ID_VERIFICATION_COUNT_DOWN);

    wstring phone_number = edit_phone_->GetText().GetData();
    wstring verification_code = edit_verification_code_->GetText().GetData();
    wstring active_code = edit_active_code_->GetText().GetData();

    MSG msg = { 0 };
    msg.hwnd = m_hWnd;
    msg.message = WM_USERWND_MSG_USER_CHECKCODE;
    TaskCenter::CTaskCenter::Instance()->CreateUserCheckCode(msg, phone_number, verification_code, active_code);

    btn_verification_code_->SetText(L"获取验证码");
    btn_verification_code_->SetEnabled(true);

    return true;
}

LRESULT CWndUser::OnGetVerificationCode(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    UINT_PTR nTask = (UINT_PTR)lParam;

    bool success = false;
    wstring str_msg;
    do {
        string strJson;
        if (!TaskCenter::CTaskCenter::Instance()->GetTaskResult(nTask, strJson)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"错误的任务ID");
            break;
        }

        Json::Reader rd;
        Json::Value vRoot;
        if (!rd.parse(strJson, vRoot)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"解析返回json失败");
            break;
        }

        string strCode = vRoot["code"].asString();
        if (strCode.compare("600") != 0) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"接口返回失败，code = %s", PublicLib::Utf8ToU(strCode).c_str());
            str_msg = PublicLib::Utf8ToU(vRoot["msg"].asString());
            break;
        }

        success = true;
    } while(false);

    if (!success) ShowToast(m_hWnd, str_msg.empty() ? L"获取验证码失败!" : str_msg.c_str());
    else {
        verification_count_down_ = 60;
        if (btn_verification_code_) btn_verification_code_->SetEnabled(false);
        UpdateBtnVerificationCode();
        ::SetTimer(m_hWnd, TIMER_ID_VERIFICATION_COUNT_DOWN, 1000, nullptr);
    }

    return 0;
}

LRESULT CWndUser::OnUserCheckCode(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    UINT_PTR nTask = (UINT_PTR)lParam;

    bool success = false;
    wstring str_msg;
    do {
        string strJson;
        if (!TaskCenter::CTaskCenter::Instance()->GetTaskResult(nTask, strJson)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"错误的任务ID");
            break;
        }

        Json::Reader rd;
        Json::Value vRoot;
        if (!rd.parse(strJson, vRoot)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"解析返回json失败");
            break;
        }

        string strCode = vRoot["code"].asString();
        if (strCode.compare("600") != 0) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"接口返回失败，code = %s", PublicLib::Utf8ToU(strCode).c_str());
            str_msg = PublicLib::Utf8ToU(vRoot["msg"].asString());
            break;
        }

        success = true;
    } while(false);

    if (!success) ShowToast(m_hWnd, str_msg.empty() ? L"验证失败" : str_msg.c_str());
    else {
        if (layout_user_) layout_user_->SelectItem(2);
        if (btn_login2_) btn_login2_->SetVisible(true);
        if (btn_confirm_) btn_confirm_->SetVisible(false);
        if (lbl_password_prefix_) lbl_password_prefix_->SetText(L"设置密码");
    }

    return 0;
}

bool CWndUser::OnEditPhoneTextChanged(void* param) {
    if (!edit_phone_ || !btn_verification_code_) return true;

    bool enabled = (edit_phone_->GetText().GetLength() >= 11);
    btn_verification_code_->SetEnabled(enabled);

    UpdateNextBtnStatus();

    return true;
}

bool CWndUser::OnEditVerificationCodeTextChanged(void* param) {
    UpdateNextBtnStatus();
    return true;
}

bool CWndUser::OnEditPhoneActiveCodeChanged(void* param) {
    UpdateNextBtnStatus();
    return true;
}

bool CWndUser::OnBtnClickActiveCode(void* param) {
    ShellExecute(NULL, L"open", URL_ACTIVE_CODE, NULL, NULL, SW_SHOW);
    return true;
}

bool CWndUser::OnBtnUsedItem(void* param) {
    CWndUserItem* pWnd = new CWndUserItem(URL_USER_USED_ITEM);
    if (!pWnd) return true;

    pWnd->Create(m_hWnd, false);
    pWnd->CenterWindow();
    pWnd->ShowModal();

    return true;
}

bool CWndUser::OnBtnPrivateItem(void* param) {
    CWndUserItem* pWnd = new CWndUserItem(URL_USER_PRIVATE_ITEM);
    if (!pWnd) return true;

    pWnd->Create(m_hWnd, false);
    pWnd->CenterWindow();
    pWnd->ShowModal();
    return true;
}

void CWndUser::UpdateNextBtnStatus() {
    if (!edit_phone_  || !btn_next_ || !edit_verification_code_ || !edit_active_code_) return;

    bool next_enable = (edit_phone_->GetText().GetLength() >= 11 && edit_verification_code_->GetText().GetLength() >= 6 && edit_active_code_->GetText().GetLength() >= 10);

    btn_next_->SetEnabled(next_enable);

    return;

}

LRESULT CWndUser::OnTimer(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    switch (wParam) {
    case TIMER_ID_VERIFICATION_COUNT_DOWN: {
        verification_count_down_--;
        if (verification_count_down_ <= 0) {
            KillTimer(m_hWnd, TIMER_ID_VERIFICATION_COUNT_DOWN);
            if (btn_verification_code_) btn_verification_code_->SetEnabled(true);
        }
        UpdateBtnVerificationCode();
        break;
    }
    case TIMER_ID_VERIFICATION_COUNT_DOWN2: {
        verification_count_down2_--;
        if (verification_count_down2_ <= 0) {
            KillTimer(m_hWnd, TIMER_ID_VERIFICATION_COUNT_DOWN2);
            if (btn_verification_code2_) btn_verification_code2_->SetEnabled(true);
        }
        UpdateBtnVerificationCode2();
        break;
    }
    default:
        break;
    }
    return 0;
}

LRESULT CWndUser::OnUserFindPassword(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (layout_user_) {
        layout_user_->SelectItem(3);
        UpdateWindowPos(true);

        if (btn_next2_) btn_next2_->SetVisible(true);
        if (btn_login3_) btn_login3_->SetVisible(false);
    }

    return 0;
}

LRESULT CWndUser::OnUserCodeLogin(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (layout_user_) {
        layout_user_->SelectItem(3);
        UpdateWindowPos(true);

        if (btn_next2_) btn_next2_->SetVisible(false);
        if (btn_login3_) btn_login3_->SetVisible(true);
    }

    return 0;
}

bool CWndUser::OnBtnClickBack3(void* param) {
    if (layout_user_) {
        layout_user_->SelectItem(0);
        UpdateWindowPos(false);
    }
    return true;
}

void CWndUser::UpdateBtnVerificationCode() {
    if (!btn_verification_code_) return;

    CStdString text;
    text.Format(verification_count_down_ > 0 ? L"倒计时%ds" : L"重新发送", verification_count_down_);

    btn_verification_code_->SetText(text.GetData());
}

bool CWndUser::OnBtnClickBack2(void* param){
    if (btn_login2_&& btn_login2_->IsVisible()) layout_user_->SelectItem(1);
    else if (btn_confirm_&& btn_confirm_->IsVisible()) layout_user_->SelectItem(3);
    return true;
}

bool CWndUser::OnCheckShowPassword(void* param) {
    if (!check_show_password_ || !edit_password_ || !edit_confirm_password_) return true;

    bool check = check_show_password_->IsSelected();
    edit_password_->SetPasswordMode(check);
    edit_confirm_password_->SetPasswordMode(check);

    return true;
}

bool CWndUser::OnEditPasswordChanged(void* param) {
    if (!btn_login2_ || !edit_password_ || !edit_confirm_password_ || !btn_confirm_) return true;

    auto password_length = edit_password_->GetText().GetLength();
    auto confirm_password_length = edit_confirm_password_->GetText().GetLength();

    bool enabled = (password_length >= 8 && password_length <= 16 
        && confirm_password_length >=8 && confirm_password_length <= 16);

    btn_login2_->SetEnabled(enabled);
    btn_confirm_->SetEnabled(enabled);

    return true;
}

bool CWndUser::IsPasswordConsistent() {
    if (!edit_password_ || !edit_confirm_password_) return false;

    if (_tcscmp(edit_password_->GetText(), edit_confirm_password_->GetText()) != 0) return false;

    return true;
}

bool CWndUser::OnBtnClickLogin2(void* param) {
    if (!edit_phone_ || !edit_verification_code_ || !edit_active_code_ || !btn_verification_code_ || !edit_password_ || !edit_confirm_password_) return true;

    if (!IsPasswordConsistent()) {
        ShowToast(m_hWnd, L"两次输入密码不一致");
        return true;
    }

    wstring phone_number = edit_phone_->GetText().GetData();
    wstring verification_code = edit_verification_code_->GetText().GetData();
    wstring active_code = edit_active_code_->GetText().GetData();
    wstring password = edit_password_->GetText().GetData();

    MSG msg = { 0 };
    msg.hwnd = m_hWnd;
    msg.message = WM_USERWND_MSG_USER_REGISTER;
    TaskCenter::CTaskCenter::Instance()->CreateUserRegister(msg, phone_number, verification_code, active_code, password);

    return true;
}

bool CWndUser::OnBtnClickConfirm(void* param) {
    if (!edit_phone2_ || !edit_verification_code2_ || !edit_password_ || !edit_confirm_password_) return true;

    if (!IsPasswordConsistent()) {
        ShowToast(m_hWnd, L"两次输入密码不一致");
        return true;
    }

    wstring phone_number = edit_phone2_->GetText().GetData();
    wstring code = edit_verification_code2_->GetText().GetData();
    wstring password = edit_password_->GetText().GetData();

    MSG msg = { 0 };
    msg.hwnd = m_hWnd;
    msg.message = WM_USERWND_MSG_USER_MODIFYPASSWORD;
    TaskCenter::CTaskCenter::Instance()->CreateUserModifyPassword(msg, phone_number, code, password);

    return true;
}

LRESULT CWndUser::OnUserModifyPassword(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    UINT_PTR nTask = (UINT_PTR)lParam;

    bool success = false;
    wstring str_msg;
    do {
        string strJson;
        if (!TaskCenter::CTaskCenter::Instance()->GetTaskResult(nTask, strJson)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"错误的任务ID");
            break;
        }

        Json::Reader rd;
        Json::Value vRoot;
        if (!rd.parse(strJson, vRoot)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"解析返回json失败");
            break;
        }

        string strCode = vRoot["code"].asString();
        if (strCode.compare("600") != 0) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"接口返回失败，code = %s", PublicLib::Utf8ToU(strCode).c_str());
            str_msg = PublicLib::Utf8ToU(vRoot["msg"].asString());
            break;
        }

        success = true;
    } while(false);

    if (!success) ShowToast(m_hWnd, str_msg.empty() ? L"修改密码失败" : str_msg.c_str());
    else {
        if (layout_user_) {
            layout_user_->SelectItem(0);
            UpdateWindowPos(false);
        }
    }

    return 0;
}

LRESULT CWndUser::OnUserRegister(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    UINT_PTR nTask = (UINT_PTR)lParam;

    bool success = false;
    wstring str_msg;
    do {
        string strJson;
        if (!TaskCenter::CTaskCenter::Instance()->GetTaskResult(nTask, strJson)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"错误的任务ID");
            break;
        }

        Json::Reader rd;
        Json::Value vRoot;
        if (!rd.parse(strJson, vRoot)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"解析返回json失败");
            break;
        }

        string strCode = vRoot["code"].asString();
        if (strCode.compare("600") != 0) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"接口返回失败，code = %s", PublicLib::Utf8ToU(strCode).c_str());
            str_msg = PublicLib::Utf8ToU(vRoot["msg"].asString());
            break;
        }

        success = true;
    } while(false);

    if (!success) ShowToast(m_hWnd, str_msg.empty() ? L"注册失败" : str_msg.c_str());
    else {
        if (!edit_phone_ || !edit_password_) return 0;

        wstring phone_number = edit_phone_->GetText().GetData();
        wstring password = edit_password_->GetText().GetData();

        MSG msg = { 0 };
        msg.hwnd = m_hWnd;
        msg.message = WM_USERWND_MSG_USER_LOGIN;
        TaskCenter::CTaskCenter::Instance()->CreateUserLoginTask(msg, phone_number, password);

        login_password_temp = password;
    }

    return 0;
}

bool CWndUser::OnBtnClickLogin(void* param) {
    if (!edit_input_user_ || !edit_input_password_) return true;

    wstring account_id = edit_input_user_->GetText().GetData();
    wstring password = edit_input_password_->GetText().GetData();

    MSG msg = { 0 };
    msg.hwnd = m_hWnd;
    msg.message = WM_USERWND_MSG_USER_LOGIN;
    TaskCenter::CTaskCenter::Instance()->CreateUserLoginTask(msg, account_id, password);

    login_password_temp = password;

    return 0;
}

LRESULT CWndUser::OnUserLogin(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    UINT_PTR nTask = (UINT_PTR)lParam;

    string strJson;
    if (!TaskCenter::CTaskCenter::Instance()->GetTaskResult(nTask, strJson)) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"错误的任务ID");
        ShowToast(m_hWnd, L"登录失败");
        return 0;
    }

    Json::Reader rd;
    Json::Value vRoot;
    if (!rd.parse(strJson, vRoot)) {
        ShowToast(m_hWnd, L"登录失败");
        return 0;
    }

    string strCode = vRoot["code"].asString();
    if (strCode.compare("600") != 0) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"接口返回失败，code = %s", PublicLib::Utf8ToU(strCode).c_str());
        wstring str_msg = PublicLib::Utf8ToU(vRoot["msg"].asString());
        ShowToast(m_hWnd, str_msg.empty() ? L"登录失败" : str_msg.c_str());
        return 0;
    }

    int nParamUid = 0;
    wstring strAccount;
    if (!TaskCenter::CTaskCenter::Instance()->GetUserLoginTaskParam(nTask, strAccount)) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"获取用户账号信息失败信息失败");
        return 0;
    }

    CUserData::Instance()->SetAccount(strAccount);

    Json::Value& vData = vRoot["data_info"];
    uint32_t nUid = vData["uid"].asUInt();
    string strToken = vData["token"].asString();

    // 验证账号信息
    MSG msg = { 0 };
    msg.hwnd = CGlobalData::Instance()->GetMainWnd();
    msg.message = WM_MAINWND_MSG_USERINFO;
    TaskCenter::CTaskCenter::Instance()->CreateUserInfoTask(msg, nUid, strToken);

    if (layout_user_ && layout_user_->GetCurSel() == 0) {
        if (check_auto_login_) CUserData::Instance()->SetAutoLogin(check_auto_login_->IsSelected());
        if (check_remember_password_) CUserData::Instance()->SetRememberPwd(check_remember_password_->IsSelected());

        CUserData::Instance()->SetAccountPassword(CUserData::Instance()->GetRememberPwd() ? PublicLib::UToA(login_password_temp) : "");
    }
    else {
        if (check_auto_login_) CUserData::Instance()->SetAutoLogin(false);
        if (check_remember_password_) CUserData::Instance()->SetRememberPwd(false);

        CUserData::Instance()->SetAccountPassword("");
    }

    Close();

    return 0;
}

bool CWndUser::OnEditPhoneTextChanged2(void* param) {
    if (!edit_phone2_ || !btn_verification_code2_) return true;

    bool enabled = (edit_phone2_->GetText().GetLength() >= 11);
    btn_verification_code2_->SetEnabled(enabled);

    if (btn_next2_&& btn_next2_->IsVisible()) UpdateNextBtnStatus2();
    else if (btn_login3_&& btn_login3_->IsVisible()) UpdateBtnLoginStatus3();

    return true;
}

bool CWndUser::OnEditVerificationCodeTextChanged2(void* param) {
    if (btn_next2_&& btn_next2_->IsVisible()) UpdateNextBtnStatus2();
    else if (btn_login3_&& btn_login3_->IsVisible()) UpdateBtnLoginStatus3();
    return true;
}

bool CWndUser::OnBtnClickVerificationCode2(void* param) {
    if (!edit_phone2_ || !btn_next2_ || !btn_login3_) return true;

    wstring phone_number = edit_phone2_->GetText().GetData();
    std::wstring check_type = ((btn_login3_ && btn_login3_->IsVisible()) ? MOBILE_CODE_LOGIN : MOBILE_CODE_FINDPWD);

    MSG msg = { 0 };
    msg.hwnd = m_hWnd;
    msg.message = WM_USERWND_MSG_GET_VERIFICATION2;
    TaskCenter::CTaskCenter::Instance()->CreateGetVerificationCode(msg, phone_number, check_type);

    return true;
}

void CWndUser::UpdateNextBtnStatus2() {
    if (!edit_phone2_  || !btn_next2_ || !edit_verification_code2_) return;

    bool next_enable = (edit_phone2_->GetText().GetLength() >= 11 && edit_verification_code2_->GetText().GetLength() >= 6);

    btn_next2_->SetEnabled(next_enable);

    return;
}

void CWndUser::UpdateBtnLoginStatus3() {
    if (!edit_phone2_  || !btn_login3_ || !edit_verification_code2_) return;

    bool login_enable = (edit_phone2_->GetText().GetLength() >= 11 && edit_verification_code2_->GetText().GetLength() >= 6);

    btn_login3_->SetEnabled(login_enable);
}

void CWndUser::UpdateBtnVerificationCode2() {
    if (!btn_verification_code2_) return;

    CStdString text;
    text.Format(verification_count_down2_ > 0 ? L"倒计时%ds" : L"重新发送", verification_count_down2_);

    btn_verification_code2_->SetText(text.GetData());
}

LRESULT CWndUser::OnGetVerificationCode2(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    UINT_PTR nTask = (UINT_PTR)lParam;

    bool success = false;
    wstring str_msg;
    do {
        string strJson;
        if (!TaskCenter::CTaskCenter::Instance()->GetTaskResult(nTask, strJson)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"错误的任务ID");
            break;
        }

        Json::Reader rd;
        Json::Value vRoot;
        if (!rd.parse(strJson, vRoot)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"解析返回json失败");
            break;
        }

        string strCode = vRoot["code"].asString();
        if (strCode.compare("600") != 0) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"接口返回失败，code = %s", PublicLib::Utf8ToU(strCode).c_str());
            str_msg = PublicLib::Utf8ToU(vRoot["msg"].asString());
            break;
        }

        success = true;
    } while(false);

    if (!success) ShowToast(m_hWnd, str_msg.empty() ? L"获取验证码失败!" : str_msg.c_str());
    else {
        verification_count_down2_ = 60;
        if (btn_verification_code2_) btn_verification_code2_->SetEnabled(false);
        UpdateBtnVerificationCode2();
        ::SetTimer(m_hWnd, TIMER_ID_VERIFICATION_COUNT_DOWN2, 1000, nullptr);
    }

    return 0;
}

bool CWndUser::OnBtnClickLogin3(void* param) {
    if (!edit_phone2_ || !edit_verification_code2_ || !btn_verification_code2_) return true;

    KillTimer(m_hWnd, TIMER_ID_VERIFICATION_COUNT_DOWN2);

    wstring phone_number = edit_phone2_->GetText().GetData();
    wstring code = edit_verification_code2_->GetText().GetData();

    MSG msg = { 0 };
    msg.hwnd = m_hWnd;
    msg.message = WM_USERWND_MSG_USER_LOGIN;
    TaskCenter::CTaskCenter::Instance()->CreateUserCodeLoginTask(msg, phone_number, code);

    btn_verification_code2_->SetText(L"获取验证码");
    btn_verification_code2_->SetEnabled(true);

    return 0;
}

bool CWndUser::OnBtnClickNext2(void* param) {
    if (!edit_phone2_ || !edit_verification_code2_ || !btn_verification_code2_) return true;

    KillTimer(m_hWnd, TIMER_ID_VERIFICATION_COUNT_DOWN2);

    wstring phone_number = edit_phone2_->GetText().GetData();
    wstring code = edit_verification_code2_->GetText().GetData();

    MSG msg = { 0 };
    msg.hwnd = m_hWnd;
    msg.message = WM_USERWND_MSG_USER_CHECKMOBILECODE;
    TaskCenter::CTaskCenter::Instance()->CreateUserCheckMobileCode(msg, phone_number, code, MOBILE_CODE_FINDPWD);

    btn_verification_code2_->SetText(L"获取验证码");
    btn_verification_code2_->SetEnabled(true);

    return true;
}

LRESULT CWndUser::OnUserCheckMobileCode(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    UINT_PTR nTask = (UINT_PTR)lParam;

    bool success = false;
    wstring str_msg;
    do {
        string strJson;
        if (!TaskCenter::CTaskCenter::Instance()->GetTaskResult(nTask, strJson)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"错误的任务ID");
            break;
        }

        Json::Reader rd;
        Json::Value vRoot;
        if (!rd.parse(strJson, vRoot)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"解析返回json失败");
            break;
        }

        string strCode = vRoot["code"].asString();
        if (strCode.compare("600") != 0) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"接口返回失败，code = %s", PublicLib::Utf8ToU(strCode).c_str());
            str_msg = PublicLib::Utf8ToU(vRoot["msg"].asString());
            break;
        }

        success = true;
    } while(false);

    if (!success) ShowToast(m_hWnd, str_msg.empty() ? L"验证失败" : str_msg.c_str());
    else {
        if (layout_user_) layout_user_->SelectItem(2);
        if (btn_login2_) btn_login2_->SetVisible(false);
        if (btn_confirm_) btn_confirm_->SetVisible(true);
        if (lbl_password_prefix_) lbl_password_prefix_->SetText(L"新密码");
    }

    return 0;
}