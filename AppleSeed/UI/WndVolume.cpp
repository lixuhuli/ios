#include "StdAfx.h"
#include "WndVolume.h"
#include "CommonWnd.h"
#include "UserData.h"
#include "ParamDefine.h"
#include "MsgDefine.h"
#include "GlobalData.h"
#include "UrlDefine.h"

CWndVolume::CWndVolume() {
    m_bEscape = true;
}

CWndVolume::~CWndVolume() {
}

LRESULT CWndVolume::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
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

LRESULT CWndVolume::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    auto lr = __super::OnCreate(uMsg, wParam, lParam, bHandled);
    if (lr != -1) m_pm.SetLayeredWithPicture(true);
    return lr;
}

LPCWSTR CWndVolume::GetXmlPath() const{
	return L"WndVolume.xml";
}

void CWndVolume::InitWindow() {
    __super::InitWindow();
}

LRESULT CWndVolume::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
    Close(IDCLOSE);
    bHandled = TRUE;
    return 0;
}
