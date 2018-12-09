#include "StdAfx.h"
#include "WndPassword.h"
#include "MsgDefine.h"

CWndPassword::CWndPassword() {
    m_bEscape = true;
}

CWndPassword::~CWndPassword() {
}

LRESULT CWndPassword::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
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

LRESULT CWndPassword::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    auto lr = __super::OnCreate(uMsg, wParam, lParam, bHandled);
    if (lr != -1) m_pm.SetLayeredWithPicture(true);
    return lr;
}

LPCWSTR CWndPassword::GetXmlPath() const{
	return L"WndPassword.xml";
}

void CWndPassword::InitWindow() {
    __super::InitWindow();
}

LRESULT CWndPassword::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
    Close(IDCLOSE);
    bHandled = TRUE;
    return 0;
}

bool CWndPassword::OnClickFindPassword(void* param) {
    auto parent = GetParent(m_hWnd);
    if (!parent) return true;

    ::PostMessage(parent, WM_USERWND_MSG_USER_FIND_PASSWORD, 0, 0);
    Close(IDCLOSE);
    return true;
}

bool CWndPassword::OnClickMessageLogin(void* param) {
    auto parent = GetParent(m_hWnd);
    if (!parent) return true;

    ::PostMessage(parent, WM_USERWND_MSG_USER_CODE_LOGIN, 0, 0);
    Close(IDCLOSE);
    return true;
}
