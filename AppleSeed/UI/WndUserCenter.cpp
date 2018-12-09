#include "StdAfx.h"
#include "WndUserCenter.h"
#include "CommonWnd.h"
#include "UserData.h"
#include "ParamDefine.h"
#include "MsgDefine.h"
#include "GlobalData.h"
#include "UrlDefine.h"

CWndUserCenter::CWndUserCenter()
 : personal_setting_(nullptr)
 , personal_switch_account_(nullptr){
	m_bEscape = true;
}

CWndUserCenter::~CWndUserCenter() {
}

LRESULT CWndUserCenter::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;

    switch (uMsg) {
    case WM_KILLFOCUS: lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
    case WM_RBUTTONDOWN:
    case WM_CONTEXTMENU:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK: {
        return 0;
    }
    default: bHandled = FALSE;
    }

    if (bHandled) {
        return lRes;
    }

    if (uMsg == WM_KEYDOWN){
        if (wParam == VK_ESCAPE) {
            Close();
        }
    }

    return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CWndUserCenter::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    auto lr = __super::OnCreate(uMsg, wParam, lParam, bHandled);
    if (lr != -1) m_pm.SetLayeredWithPicture(true);
    return lr;
}

LPCWSTR CWndUserCenter::GetXmlPath() const{
	return L"WndUserCenter.xml";
}

void CWndUserCenter::InitWindow() {
    __super::InitWindow();

    //if (!personal_head_ || !personal_nick_name_) return;

    //Json::Value& vUser = CUserData::Instance()->GetUserInfo();
    //string strName = vUser["avatarSrc"].asString();
    //string strUrl;
    //if (strName.find("http") != string::npos) strUrl = strName;
    //else {
    //    strUrl = URL_USER_IMG;
    //    strUrl += ("headportrait/" + strName + ".png");
    //}

    //PublicLib::MD5 md5(strUrl);
    //string strMd5 = md5.md5();
    //const wstring &strUserPath = CUserData::Instance()->GetUserPath();
    //wstring strIcoPath = strUserPath + L"\\" + PublicLib::AToU(strMd5) + L".png";

    //auto SetHeadIcon = [&](const wstring& strIcoPath) -> void {
    //    CDuiString image;
    //    image.Format(L"file='%s'", strIcoPath.c_str());
    //    personal_head_->SetBkImage(image.GetData());
    //    //personal_head_->SetBkImage(L"file='UserCenter/system_1.png'");
    //};

    //if (PathFileExists(strIcoPath.c_str())) SetHeadIcon(strIcoPath);
    //else {
    //    wstring icon_path;
    //    wstring icon_url;
    //    CUserData::Instance()->GetLastHead(icon_path, icon_url);
    //    if (PathFileExists(icon_path.c_str())) SetHeadIcon(icon_path);
    //}

    //wstring strNickName = PublicLib::Utf8ToU(vUser["nickname"].asString());
    //personal_nick_name_->SetText(strNickName.c_str());
    //personal_nick_name_->SetToolTip(strNickName.c_str());
}

LRESULT CWndUserCenter::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
    Close(IDCLOSE);
    bHandled = TRUE;
    return 0;
}

bool CWndUserCenter::OnClickPersonalSetting(void* param){
    ::PostMessage(CGlobalData::Instance()->GetMainWnd(), WM_MAINWND_MSG_COMMON, WpCommonUserInfo, 0);
    Close(IDCLOSE);
    return true;
}

bool CWndUserCenter::OnClickSwitchAccount(void* param) {
    ::PostMessage(CGlobalData::Instance()->GetMainWnd(), WM_MAINWND_MSG_COMMON, WpCommonSwitchAccount, 0);
    Close(IDCLOSE);
    return true;
}
