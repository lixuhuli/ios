#include "StdAfx.h"
#include "RemoteHandleUI.h"
#include "MsgDefine.h"

CRemoteHandleUI::CRemoteHandleUI()
 : m_uButtonState(0)
 , m_iSepWidth(1)
 , direct_(0)
 , drag_(false)
 , btn_hand_circle_(nullptr)
 , edit_key_1_(nullptr) 
 , edit_key_2_(nullptr)
 , edit_key_3_(nullptr)
 , edit_key_4_(nullptr)
 , btn_hand_close_(nullptr) {
    ptLastMouse.x = ptLastMouse.y = 0;
    ::ZeroMemory(&m_rcNewPos, sizeof(m_rcNewPos));
}


CRemoteHandleUI::~CRemoteHandleUI() {
}

void CRemoteHandleUI::Init() {
    InitControls();
    BindControls();
}

LPCTSTR CRemoteHandleUI::GetClass() const {
    return L"RemoteHandleUI";
}

LPVOID CRemoteHandleUI::GetInterface(LPCTSTR pstrName) {
    if (_tcscmp(pstrName, UI_REMOTE_HANDLE) == 0) return static_cast<CRemoteHandleUI*>(this);
    return CContainerUI::GetInterface(pstrName);
}

void CRemoteHandleUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
    if (_tcscmp(pstrName, _T("sepwidth")) == 0) SetSepWidth(_ttoi(pstrValue));
    //else if (_tcscmp(pstrName, _T("immediate")) == 0) SetImmediateBrowse(_tcscmp(pstrValue, _T("true")) == 0);
    //else if (_tcscmp(pstrName, _T("needwait")) == 0) SetNeedWaiteBrowse(_tcscmp(pstrValue, _T("true")) == 0);
    //else if (_tcscmp(pstrName, _T("firstneedcallback")) == 0) SetFirstNeedCallBack(_tcscmp(pstrValue, _T("true")) == 0);
    __super::SetAttribute(pstrName, pstrValue);
}

void CRemoteHandleUI::SetSepWidth(int iWidth)
{
    m_iSepWidth = iWidth;
}

int CRemoteHandleUI::GetSepWidth() const
{
    return m_iSepWidth;
}

UINT CRemoteHandleUI::GetControlFlags() const {
    if (IsEnabled() && m_iSepWidth != 0) return UIFLAG_SETCURSOR;
    else return 0;
}

int CRemoteHandleUI::GetRegionStatus(POINT ptMouse) {
    if (ptMouse.x >= m_rcItem.left && ptMouse.x < m_rcItem.left + m_iSepWidth) {
        if (ptMouse.y >= m_rcItem.top && ptMouse.y < m_rcItem.top + m_iSepWidth) return 4;
        else if (ptMouse.y >= m_rcItem.bottom - m_iSepWidth && ptMouse.y < m_rcItem.bottom) return 7;
        else return 0;
    }

    if (ptMouse.y >= m_rcItem.top && ptMouse.y < m_rcItem.top + m_iSepWidth) {
        if (ptMouse.x >= m_rcItem.right - m_iSepWidth && ptMouse.x < m_rcItem.right) return 5;
        else return 1;
    }

    if (ptMouse.x >= m_rcItem.right - m_iSepWidth && ptMouse.x < m_rcItem.right) {
        if (ptMouse.y >= m_rcItem.bottom - m_iSepWidth && ptMouse.y < m_rcItem.bottom) return 6;
        else return 2;
    }

    if (ptMouse.y >= m_rcItem.bottom - m_iSepWidth && ptMouse.y < m_rcItem.bottom) return 3;

    return 8;
}

void CRemoteHandleUI::DoEvent(TEventUI& event) {
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
            auto state = GetRegionStatus(event.ptMouse);
            if (state != 8) {
                auto cursor = IDC_SIZEWE;
                if (state == 1 || state == 3) {
                    cursor = IDC_SIZENS;
                }
                else if (state == 4 || state == 6) {
                    cursor = IDC_SIZENWSE;
                }
                else if (state == 5 || state == 7) {
                    cursor = IDC_SIZENESW;
                }
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(cursor)));
                return;
            }
        }
    }

    __super::DoEvent(event);
}

void CRemoteHandleUI::SetPos(RECT rc) {
    __super::SetPos(rc);

    QRect rc_ctrl = rc;
    if (btn_hand_circle_) {
        rc_ctrl.Deflate(24, 24);
        btn_hand_circle_->SetPos(rc_ctrl);
    }

    if (edit_key_1_) {
        rc_ctrl.left = rc.left + 8;
        rc_ctrl.right = rc_ctrl.left + 39;
        rc_ctrl.top = (rc.top + rc.bottom - 39) / 2;
        rc_ctrl.bottom = rc_ctrl.top + 39;
        edit_key_1_->SetPos(rc_ctrl);
    }

    if (edit_key_2_) {
        rc_ctrl.left = (rc.left + rc.right - 39) / 2;
        rc_ctrl.right = rc_ctrl.left + 39;
        rc_ctrl.top = rc.top + 8;
        rc_ctrl.bottom = rc_ctrl.top + 39;
        edit_key_2_->SetPos(rc_ctrl);
    }

    if (edit_key_3_) {
        rc_ctrl.left = rc.right - 8 - 39;
        rc_ctrl.right = rc_ctrl.left + 39;
        rc_ctrl.top = (rc.top + rc.bottom - 39) / 2;
        rc_ctrl.bottom = rc_ctrl.top + 39;
        edit_key_3_->SetPos(rc_ctrl);
    }

    if (edit_key_4_) {
        rc_ctrl.left = (rc.left + rc.right - 39) / 2;
        rc_ctrl.right = rc_ctrl.left + 39;
        rc_ctrl.top = rc.bottom  - 8 - 39;
        rc_ctrl.bottom = rc_ctrl.top + 39;
        edit_key_4_->SetPos(rc_ctrl);
    }

    if (btn_hand_close_) {
        rc_ctrl.left = rc.right - 8 - 19;
        rc_ctrl.right = rc_ctrl.left + 19;
        rc_ctrl.top = rc.top + 8;
        rc_ctrl.bottom = rc_ctrl.top + 19;
        btn_hand_close_->SetPos(rc_ctrl);
    }
}

void CRemoteHandleUI::PaintStatusImage(HDC hDC) {
    __super::PaintStatusImage(hDC);

    DrawBorderRect(hDC);
}

void CRemoteHandleUI::DrawBorderRect(HDC hDC) {
    //  »æÖÆ±ß¿ò
    //CRenderEngine::DrawRect(hDC, m_rcItem, 1, GetAdjustColor(0xFF969696));
}

void CRemoteHandleUI::OnLButtonDown(UINT nFlags, QPoint point) {
    auto direct = GetRegionStatus(point);
    m_uButtonState |= UISTATE_CAPTURED;
    ptLastMouse = point;
    m_rcNewPos = m_rcItem;
    direct_ = direct;
}

void CRemoteHandleUI::OnMouseMove(UINT nFlags, QPoint point) {
    if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
        LONG cx = point.x - ptLastMouse.x;
        LONG cy = point.y - ptLastMouse.y;
        ptLastMouse = point;
        RECT rc = m_rcNewPos;

        if (direct_ != 8) {
            auto UpdateNewPos = [&](bool add, int distance) -> void {
                if (add) {
                    m_rcNewPos.left -= distance;
                    m_rcNewPos.right += distance;
                    m_rcNewPos.top -= distance;
                    m_rcNewPos.bottom += distance;
                    SetPos(m_rcNewPos);
                }
                else {
                    auto max_dis = (m_rcNewPos.right - m_rcNewPos.left - GetMinWidth()) / 2;
                    if (distance > max_dis) distance = max_dis;

                    max_dis = (m_rcNewPos.bottom - m_rcNewPos.top - GetMinHeight()) / 2;
                    if (distance > max_dis) distance = max_dis;

                    m_rcNewPos.left += distance;
                    m_rcNewPos.right -= distance;
                    m_rcNewPos.top += distance;
                    m_rcNewPos.bottom -= distance;
                    SetPos(m_rcNewPos);
                }
            };

            int distance = 0;
            if (direct_ == 0) {
                distance = abs(cx);
                UpdateNewPos(cx < 0, distance);
            }
            else if (direct_ == 1) {
                distance = abs(cy);
                UpdateNewPos(cy < 0, distance);
            }
            else if (direct_ == 2) {
                distance = abs(cx);
                UpdateNewPos(cx > 0, distance);
            }
            else if (direct_ == 3) {
                distance = abs(cy);
                UpdateNewPos(cy > 0, distance);
            }
            else if (direct_ == 4) {
                if (cx < 0 && cy < 0) {
                    distance = max(abs(cx), abs(cy));
                    UpdateNewPos(true, distance);
                }
                else if (cx > 0 && cy > 0) {
                    distance = max(abs(cx), abs(cy));
                    UpdateNewPos(false, distance);
                }
            }
            else if (direct_ == 5) {
                if (cx > 0 && cy < 0) {
                    distance = max(abs(cx), abs(cy));
                    UpdateNewPos(true, distance);
                }
                else if (cx < 0 && cy > 0) {
                    distance = max(abs(cx), abs(cy));
                    UpdateNewPos(false, distance);
                }
            }
            else if (direct_ == 6) {
                if (cx > 0 && cy > 0) {
                    distance = max(abs(cx), abs(cy));
                    UpdateNewPos(true, distance);
                }
                else if (cx < 0 && cy < 0) {
                    distance = max(abs(cx), abs(cy));
                    UpdateNewPos(false, distance);
                }
            }
            else if (direct_ == 7) {
                if (cx < 0 && cy > 0) {
                    distance = max(abs(cx), abs(cy));
                    UpdateNewPos(true, distance);
                }
                else if (cx > 0 && cy < 0) {
                    distance = max(abs(cx), abs(cy));
                    UpdateNewPos(false, distance);
                }
            }

            return;
        }

        m_rcNewPos.left += cx;
        m_rcNewPos.right += cx;
        m_rcNewPos.top += cy;
        m_rcNewPos.bottom += cy;
        SetPos(m_rcNewPos);
        ::SetCursor(::LoadCursor(NULL, IDC_HAND));
        return;
    }
}

bool CRemoteHandleUI::OnClickBtnClose(void* param) {
    if (!m_pManager) return true;
    ::PostMessage(m_pManager->GetPaintWindow(), WM_KEYWND_MSG_REMOVE_KEY, (LPARAM)((CControlUI*)this), (LPARAM)KeyType());
    return true;
}

void CRemoteHandleUI::UpdateBrowserMode(bool browser_mode, int opacity/* = 100*/) {
    if (btn_hand_close_) btn_hand_close_->SetVisible(!browser_mode);
    auto transparent = Ikey::GetTransparent(opacity);
    if (btn_hand_circle_) btn_hand_circle_->SetTransparent(transparent);
    if (edit_key_1_) edit_key_1_->SetTransparent(transparent);
    if (edit_key_2_) edit_key_2_->SetTransparent(transparent);
    if (edit_key_3_) edit_key_3_->SetTransparent(transparent);
    if (edit_key_4_) edit_key_4_->SetTransparent(transparent);

    SetEnabled(!browser_mode);
    if (btn_hand_circle_) btn_hand_circle_->SetEnabled(!browser_mode);
    if (edit_key_1_) edit_key_1_->SetEnabled(!browser_mode);
    if (edit_key_2_) edit_key_2_->SetEnabled(!browser_mode);
    if (edit_key_3_) edit_key_3_->SetEnabled(!browser_mode);
    if (edit_key_4_) edit_key_4_->SetEnabled(!browser_mode);
}