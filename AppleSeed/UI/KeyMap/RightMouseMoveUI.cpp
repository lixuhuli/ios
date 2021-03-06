#include "StdAfx.h"
#include "RightMouseMoveUI.h"

CRightMouseMoveUI::CRightMouseMoveUI()
 : m_uButtonState(0)
 , edit_key_(nullptr)
 , slider_mouse_(nullptr) 
 , mouse_copy_(nullptr) {
    ptLastMouse.x = ptLastMouse.y = 0;
    ::ZeroMemory(&m_rcNewPos, sizeof(m_rcNewPos));
}


CRightMouseMoveUI::~CRightMouseMoveUI() {
}

void CRightMouseMoveUI::Init() {
    InitControls();
    BindControls();
}

LPCTSTR CRightMouseMoveUI::GetClass() const {
    return L"RightMouseMoveUI";
}

LPVOID CRightMouseMoveUI::GetInterface(LPCTSTR pstrName) {
    if (_tcscmp(pstrName, UI_RIGHT_MOUSEMOVE) == 0) return static_cast<CRightMouseMoveUI*>(this);
    return CContainerUI::GetInterface(pstrName);
}

void CRightMouseMoveUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
    __super::SetAttribute(pstrName, pstrValue);
}

UINT CRightMouseMoveUI::GetControlFlags() const {
    if (IsEnabled()) return UIFLAG_SETCURSOR;
    else return 0;
}

void CRightMouseMoveUI::DoEvent(TEventUI& event) {
    if (event.Type == UIEVENT_BUTTONDOWN) {
        if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled()) {
            OnLButtonDown(event.wParam, event.ptMouse);
        }
        return;
    }

    if (event.Type == UIEVENT_BUTTONUP) {
        if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
            m_uButtonState &= ~UISTATE_CAPTURED;
            m_rcItem = m_rcNewPos;
            return;
        }
    }

    if (event.Type == UIEVENT_MOUSEMOVE) {
        if (IsEnabled()) {
            OnMouseMove(0, event.ptMouse);
        }
        return;
    }

    if (event.Type == UIEVENT_SETCURSOR) {
        if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled()) {
                return;
        }
    }

    __super::DoEvent(event);
}

void CRightMouseMoveUI::SetPos(RECT rc) {
    __super::SetPos(rc);
}

void CRightMouseMoveUI::OnLButtonDown(UINT nFlags, QPoint point) {
    m_uButtonState |= UISTATE_CAPTURED;
    ptLastMouse = point;
    m_rcNewPos = m_rcItem;
}

void CRightMouseMoveUI::OnMouseMove(UINT nFlags, QPoint point) {
    if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
        LONG cx = point.x - ptLastMouse.x;
        LONG cy = point.y - ptLastMouse.y;
        ptLastMouse = point;
        RECT rc = m_rcNewPos;

        m_rcNewPos.left += cx;
        m_rcNewPos.right += cx;
        m_rcNewPos.top += cy;
        m_rcNewPos.bottom += cy;
        SetPos(m_rcNewPos);
        ::SetCursor(::LoadCursor(NULL, IDC_HAND));
        m_pManager->SendNotify(this, DUI_MSGTYPE_POS_CHANGED);
        return;
    }
}

void CRightMouseMoveUI::UpdateBrowserMode(bool browser_mode, int opacity/* = 100*/) {
    auto transparent = Ikey::GetTransparent(opacity);
    if (edit_key_) edit_key_->SetTransparent(transparent);
    if (slider_mouse_) slider_mouse_->SetTransparent(transparent);
    if (mouse_copy_) mouse_copy_->SetTransparent(transparent);

    SetEnabled(!browser_mode);
    if (edit_key_) edit_key_->SetEnabled(!browser_mode);
    if (slider_mouse_) slider_mouse_->SetEnabled(!browser_mode);
    if (mouse_copy_) mouse_copy_->SetEnabled(!browser_mode);
}