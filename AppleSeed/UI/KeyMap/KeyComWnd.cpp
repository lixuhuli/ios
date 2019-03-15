#include "StdAfx.h"
#include "KeyComWnd.h"
#include "CommonWnd.h"
#include "UserData.h"
#include "ParamDefine.h"
#include "MsgDefine.h"
#include "GlobalData.h"
#include "UrlDefine.h"
#include "KeyComItem.h"

CKeyComWnd::CKeyComWnd()
 : list_combox_keymap_(nullptr) {
	m_bEscape = true;
}

CKeyComWnd::~CKeyComWnd() {
}

LRESULT CKeyComWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
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

LRESULT CKeyComWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    auto lr = __super::OnCreate(uMsg, wParam, lParam, bHandled);
    //if (lr != -1) m_pm.SetLayeredWithPicture(true);
    return lr;
}

LPCWSTR CKeyComWnd::GetXmlPath() const{
	return L"KeyMap/KeyComWnd.xml";
}

void CKeyComWnd::InitWindow() {
    __super::InitWindow();
}

void CKeyComWnd::UpdateKeyCtrls(const std::vector<KeyMapInfo>& keymap_info) {
    if (keymap_info.empty() || !list_combox_keymap_) return;

    for (auto it = keymap_info.begin(); it != keymap_info.end(); it++) {
        CKeyComItemUI* key_item =  new CKeyComItemUI();

        if (!key_item) {
            OUTPUT_XYLOG(LEVEL_ERROR, ERROR, L"malloc key com item failed!");
            continue;;
        }

        key_item->InitControl();
        key_item->SetUserData(it->file_.c_str());
        key_item->SetInheritableUserData(it->default_.c_str());
        key_item->SetTag(it->tag_);
        key_item->SetInheritableTag(it->index_);
        key_item->SetName(it->name_.c_str());
        key_item->UpdateCtrls(*it);

        list_combox_keymap_->Add(key_item);
    }
}

LRESULT CKeyComWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
    Close(IDCLOSE);
    bHandled = TRUE;
    return 0;
}

bool CKeyComWnd::OnClickPersonalSetting(void* param){
    ::PostMessage(CGlobalData::Instance()->GetMainWnd(), WM_MAINWND_MSG_COMMON, WpCommonUserInfo, 0);
    Close(IDCLOSE);
    return true;
}

bool CKeyComWnd::OnClickSwitchAccount(void* param) {
    ::PostMessage(CGlobalData::Instance()->GetMainWnd(), WM_MAINWND_MSG_COMMON, WpCommonSwitchAccount, 0);
    Close(IDCLOSE);
    return true;
}
