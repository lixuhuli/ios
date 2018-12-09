#include "StdAfx.h"
#include "WndMain.h"

CWndMain::CWndMain() 
    : ios_key_(nullptr) {
    m_dwStyle = UI_WNDSTYLE_FRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    m_bShowShadow = true;
}

CWndMain::~CWndMain() {
    ios_destroy_key();
    ios_key_ = nullptr;
}

LPCWSTR CWndMain::GetXmlPath() const {
    return L"demo/WndMain.xml";
}

LPCTSTR CWndMain::GetWindowClassName() const {
    return L"IosWndMain";
}

void CWndMain::InitWindow() {
    __super::InitWindow();

    ios_key_ = ios_create_key();
    if (ios_key_) {
        ios_key_->CreateWnd(m_hWnd, QRect(0, 0, 0, 0));
        ios_key_->Show(false);
    }
}

LRESULT CWndMain::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;

    switch (uMsg) {
    case WM_SIZING: lRes = OnSize(wParam, lParam, bHandled);
    case WM_MOVE: lRes = OnMoving(wParam, lParam, bHandled);
    default: bHandled = FALSE;
    }

    if (bHandled) {
        return lRes;
    }
    return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CWndMain::OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (!ios_key_ || !ios_key_->IsVisible()) return 0;

    QRect *lprc = (QRect*)lParam;
    if (lprc) UpdatePos(lprc);
    return 0;
}

LRESULT CWndMain::OnMoving(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (!ios_key_ || !ios_key_->IsVisible()) return 0;

    UpdatePos();
    return 0;
}

void CWndMain::UpdatePos(const QRect* lprc /*= nullptr*/) {
    if (!ios_key_) return;

    QRect rc;
    if (lprc) rc = *lprc;
    else GetWindowRect(m_hWnd, &rc);

    rc.left -= 280;
    rc.right -= 40;
    
    ios_key_->SetPos(rc);
}

bool CWndMain::OnClickBtnClose(void* lpParam) {
    Close(IDCLOSE);
    return true;
}

bool CWndMain::OnClickBtnKey(void* param) {
    if (ios_key_ && !ios_key_->IsVisible()) {
        UpdatePos();
        ios_key_->Show(true);
    }
    return true;
}

void CWndMain::OnFinalMessage(HWND hWnd) {
    __super::OnFinalMessage(hWnd);
}

CControlUI* CWndMain::CreateControl(LPCTSTR pstrClass) {
    if (!pstrClass) return nullptr;

    std::wstring control_name = pstrClass;

    return nullptr;
}