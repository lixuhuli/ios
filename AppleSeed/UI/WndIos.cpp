#include "StdAfx.h"
#include "WndIos.h"
#include "Ios/EngineInterface.h"
#include "GlobalData.h"
#include "Ios/IosMgr.h"

#define EVENT_OP_MOUSE_DOWN     10
#define EVENT_OP_MOUSE_UP       11
#define EVENT_OP_MOUSE_MOVE     12

#define EVENT_OP_KEY_DOWN		10
#define EVENT_OP_KEY_UP			11

CWndIos::CWndIos() {
    m_dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    m_dwExStyle = WS_EX_ACCEPTFILES;
    m_bShowShadow = false;
}

CWndIos::~CWndIos() {
}

LPCWSTR CWndIos::GetXmlPath() const {
    return L"WndIos.xml";
}

LPCTSTR CWndIos::GetWindowClassName() const {
    return L"IosWindow";
}

void CWndIos::InitWindow() {
    __super::InitWindow();
}

LRESULT CWndIos::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;

    switch (uMsg) {
    case WM_SIZING: lRes = OnSize(wParam, lParam, bHandled); break;
    case WM_MOVE: lRes = OnMoving(wParam, lParam, bHandled); break;
    case WM_MOUSEMOVE: lRes = OnMouseMove(wParam, lParam, bHandled); break;
    case WM_MOUSEWHEEL: lRes = OnMouseWheel(wParam, lParam, bHandled); break;
    case WM_LBUTTONDOWN: lRes = OnLButtonDown(wParam, lParam, bHandled); break;
    case WM_LBUTTONUP: lRes = OnLButtonUp(wParam, lParam, bHandled); break;
    case WM_KEYDOWN: lRes = OnKeyDown(wParam, lParam, bHandled); break;
    case WM_KEYUP: lRes = OnKeyUp(wParam, lParam, bHandled); break;
    case WM_RBUTTONDOWN: lRes = OnRButtonDown(wParam, lParam, bHandled); break;
    case WM_RBUTTONUP: lRes = OnRButtonUp(wParam, lParam, bHandled); break;
    case WM_DROPFILES: lRes = OnDropFiles(wParam, lParam, bHandled); break;
    default: bHandled = FALSE; break;
    }

    if (bHandled) {
        return lRes;
    }
    return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CWndIos::OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    QRect *lprc = (QRect*)lParam;
    if (lprc) CIosMgr::Instance()->UpdateKeyWnd(lprc);
    return 0;
}

LRESULT CWndIos::OnMoving(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    CIosMgr::Instance()->UpdateKeyWnd();
    return 0;
}

LRESULT CWndIos::OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    QPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

    if (wParam == MK_LBUTTON || wParam == MK_RBUTTON) {
        MouseOpSync(EVENT_OP_MOUSE_MOVE, pt.x, pt.y);
    }

    bHandled = FALSE;
    return 0;
}

LRESULT CWndIos::OnMouseWheel(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    QPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    //MouseOpSync(13, pt.x, pt.y);
    return 0;
}

void CWndIos::SetCaptionRect(RECT& rcCaption) {
    m_pm.SetCaptionRect(rcCaption);
}

LRESULT CWndIos::OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    QPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    MouseOpSync(EVENT_OP_MOUSE_DOWN, pt.x, pt.y);
    bHandled = FALSE;
    return 0;
}

LRESULT CWndIos::OnLButtonUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    QPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    MouseOpSync(EVENT_OP_MOUSE_UP, pt.x, pt.y);
    bHandled = FALSE;
    return 0;
}

LRESULT CWndIos::OnRButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    QPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    MouseOpSync(EVENT_OP_MOUSE_MOVE + 1, pt.x, pt.y);
    bHandled = FALSE;
    return 0;
}

LRESULT CWndIos::OnRButtonUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    QPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    MouseOpSync(EVENT_OP_MOUSE_MOVE + 2, pt.x, pt.y);
    bHandled = FALSE;
    return 0;
}

LRESULT CWndIos::OnKeyDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    bool controlled = false;
    auto key_value = wParam;

    //if ((ev->modifiers() == Qt::ControlModifier)) {
    //    controlled = true;
    //    if (ev->key() == Qt::Key_Control)
    //        controlled = false;
    //}

    if (controlled && key_value == 'H') {
        //HomeKeyDown();
    }
    else {
        KeyInputSync(EVENT_OP_KEY_DOWN, key_value);
    }

    return 0;
}

LRESULT CWndIos::OnKeyUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    bool controlled = false;
    auto key_value = wParam;

    //if ((ev->modifiers() == Qt::ControlModifier)) {
    //    controlled = true;
    //    if (ev->key() == Qt::Key_Control)
    //        controlled = false;
    //}

    if (controlled && key_value == 'H') {
        //HomeKeyUp();
    }
    else {
        KeyInputSync(EVENT_OP_KEY_UP, key_value);
    }
    return 0;
}

LRESULT CWndIos::OnDropFiles(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    HDROP hDrop = (HDROP)wParam;
    if (!hDrop) return 0;

    int nCount;
    TCHAR szPath[MAX_PATH + 1] = { 0 };

    nCount = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
    for (int i = 0; i < nCount; i++) {
        DragQueryFile(hDrop, i, szPath, MAX_PATH + 1);
        std::wstring file_path = szPath;

        auto pos = file_path.rfind(L".ipa");

        if (pos != wstring::npos && pos == file_path.length() - 4) 
            CIosMgr::Instance()->InstallApp(file_path);
    }

    DragFinish(hDrop);

    return 0;
}

void CWndIos::OnFinalMessage(HWND hWnd) {
    __super::OnFinalMessage(hWnd);
}

CControlUI* CWndIos::CreateControl(LPCTSTR pstrClass) {
    if (!pstrClass) return nullptr;

    std::wstring control_name = pstrClass;

    return nullptr;
}

void CWndIos::Exit() {
    Close();
}
