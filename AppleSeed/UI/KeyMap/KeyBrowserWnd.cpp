#include "StdAfx.h"
#include "KeyBrowserWnd.h"

CKeyBrowserWnd::CKeyBrowserWnd() {
    m_dwStyle = UI_WNDSTYLE_DIALOG | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    m_bShowShadow = false;
}

CKeyBrowserWnd::~CKeyBrowserWnd() {
}

DuiLib::CControlUI* CKeyBrowserWnd::GetRoot() {
    return m_pm.GetRoot();
}

LPCWSTR CKeyBrowserWnd::GetXmlPath() const {
    return L"KeyMap/KeyBrowserWnd.xml";
}

LPCTSTR CKeyBrowserWnd::GetWindowClassName() const {
    return L"KeyBrowser";
}

void CKeyBrowserWnd::InitWindow() {
    __super::InitWindow();
}

LRESULT CKeyBrowserWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;

    //switch (uMsg) {
    //default: bHandled = FALSE; break;
    //}

    //if (bHandled) {
    //    return lRes;
    //}
    return __super::HandleMessage(uMsg, wParam, lParam);
}

void CKeyBrowserWnd::OnFinalMessage(HWND hWnd) {
    __super::OnFinalMessage(hWnd);
}

CControlUI* CKeyBrowserWnd::CreateControl(LPCTSTR pstrClass) {
    if (!pstrClass) return nullptr;

    std::wstring control_name = pstrClass;

    return nullptr;
}
