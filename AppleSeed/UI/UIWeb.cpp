#include "StdAfx.h"
#include "UIWeb.h"
#include "WndWeb.h"

CWebUI::CWebUI()
 : wnd_web_(nullptr) {
}

CWebUI::~CWebUI() {
    wnd_web_ = nullptr;
}

void CWebUI::Init() {
    __super::Init();

    if (!wnd_web_) wnd_web_ = new CWndWeb;
    if (wnd_web_) {
        wnd_web_->Create(m_pManager->GetPaintWindow(), false);
        wnd_web_->ShowWindow(false);
    }
}

LPCTSTR CWebUI::GetClass() const {
    return L"WebUI";
}

LPVOID CWebUI::GetInterface(LPCTSTR pstrName) {
    if (_tcscmp(pstrName, DUI_UI_WEB) == 0) return static_cast<CWebUI*>(this);
    return CControlUI::GetInterface(pstrName);
}

void CWebUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
    __super::SetAttribute(pstrName, pstrValue);
}

void CWebUI::SetPos(RECT rc) {
    __super::SetPos(rc);
    if (!wnd_web_) return;

    HWND hWnd = wnd_web_->GetHWND();
    if (hWnd && IsWindow(hWnd)) {
        int nWidth = rc.right - rc.left;
        int nHeight = rc.bottom - rc.top;
        if (nWidth == 0 || nHeight == 0) return;

        POINT pt = { rc.left, rc.top };
        ClientToScreen(m_pManager->GetPaintWindow(), &pt);
        QRect rc_wnd(pt.x, pt.y, pt.x + nWidth, pt.y + nHeight);
        UpdatePos(rc_wnd);
    }
}

void CWebUI::SetFocus() {
    auto web_ctrl = wnd_web_->web_ctrl();
    if (web_ctrl) ::SetFocus(web_ctrl->GetHostWnd());
}

void CWebUI::UpdatePos(const QRect& rc) {
    if (!wnd_web_) return;

    HWND hWnd = wnd_web_->GetHWND();
    if (hWnd && IsWindow(hWnd)) {
        ::SetWindowPos(hWnd, nullptr, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), SWP_NOZORDER | SWP_SHOWWINDOW);
    }
}

void CWebUI::Navigate(LPCTSTR lpUrl) {
    if (!wnd_web_) return;

    auto web_ctrl = wnd_web_->web_ctrl();
    if (!web_ctrl) return;

    web_ctrl->Navigate(lpUrl);
}
