#include "StdAfx.h"
#include "IntelligentUI.h"

CIntelligentUI::CIntelligentUI()
 : m_uButtonState(0)
 , btn_hand_close_(nullptr)
 , edit_key_(nullptr)
 , slider_mouse_(nullptr)
 , opt_switch_(nullptr) {
    ptLastMouse.x = ptLastMouse.y = 0;
    ::ZeroMemory(&m_rcNewPos, sizeof(m_rcNewPos));
}


CIntelligentUI::~CIntelligentUI() {
}

void CIntelligentUI::Init() {
    InitControls();
    BindControls();
}

LPCTSTR CIntelligentUI::GetClass() const {
    return L"IntelligentUI";
}

LPVOID CIntelligentUI::GetInterface(LPCTSTR pstrName) {
    if (_tcscmp(pstrName, UI_INTELLIGENT) == 0) return static_cast<CIntelligentUI*>(this);
    return CContainerUI::GetInterface(pstrName);
}

void CIntelligentUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
    __super::SetAttribute(pstrName, pstrValue);
}

UINT CIntelligentUI::GetControlFlags() const {
    if (IsEnabled()) return UIFLAG_SETCURSOR;
    else return 0;
}

void CIntelligentUI::DoEvent(TEventUI& event) {
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

void CIntelligentUI::SetPos(RECT rc) {
    __super::SetPos(rc);
}

void CIntelligentUI::OnLButtonDown(UINT nFlags, QPoint point) {
    m_uButtonState |= UISTATE_CAPTURED;
    ptLastMouse = point;
    m_rcNewPos = m_rcItem;
}

void CIntelligentUI::OnMouseMove(UINT nFlags, QPoint point) {
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
        return;
    }
}

bool CIntelligentUI::OnClickBtnClose(void* param) {
    auto parent = (CContainerUI*)GetParent();
    if (!parent) return true;
    parent->Remove(this);
    return true;
}

void CIntelligentUI::UpdateBrowserMode(bool browser_mode, int opacity/* = 100*/) {
    if (btn_hand_close_) btn_hand_close_->SetVisible(!browser_mode);
    auto transparent = Ikey::GetTransparent(opacity);
    if (edit_key_) edit_key_->SetTransparent(transparent);
    if (slider_mouse_) slider_mouse_->SetTransparent(transparent);
    if (opt_switch_) opt_switch_->SetTransparent(transparent);

    SetEnabled(!browser_mode);
    if (edit_key_) edit_key_->SetEnabled(!browser_mode);
    if (slider_mouse_) slider_mouse_->SetEnabled(!browser_mode);
    if (opt_switch_) opt_switch_->SetEnabled(!browser_mode);
}