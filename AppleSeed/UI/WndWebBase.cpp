#include "StdAfx.h"
#include "WndWebBase.h"

CWndWebBase::CWndWebBase()
: web_(nullptr) {
    m_dwStyle = UI_WNDSTYLE_DIALOG | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
}

CWndWebBase::~CWndWebBase() {
}

LRESULT CWndWebBase::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    auto lr = __super::OnCreate(uMsg, wParam, lParam, bHandled);
    if (lr != -1) m_pm.SetLayeredWithPicture(true);
    return lr;
}

LRESULT CWndWebBase::OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    QRect *lprc = (QRect*)lParam;
    //if (lprc) UpdatePos(lprc);
    return 0;
}

LRESULT CWndWebBase::OnMoving(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    UpdatePos();
    return 0;
}

void CWndWebBase::UpdatePos(const QRect* lprc /*= nullptr*/) {
    if (!web_) return;

    QRect rc = web_->GetPos();
    if (rc.GetWidth() == 0 || rc.GetHeight() == 0) return;

    POINT pt = { rc.left, rc.top };
    ClientToScreen(m_hWnd, &pt);

    QRect rc_web(pt.x, pt.y, pt.x + rc.GetWidth(), pt.y + rc.GetHeight());
    web_->UpdatePos(rc_web);
}

void CWndWebBase::InitWindow() {
	__super::InitWindow();
}

LRESULT CWndWebBase::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
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

CControlUI* CWndWebBase::CreateControl(LPCTSTR pstrClass) {
    std::wstring control_name = pstrClass;
    if (control_name == L"Web") return new CWebUI();
    return nullptr;
}
