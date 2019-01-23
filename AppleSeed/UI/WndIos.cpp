#include "StdAfx.h"
#include "WndIos.h"
#include "Ios/EngineInterface.h"
#include "GlobalData.h"
#include "Ios/IosMgr.h"
#include "CommonWnd.h"

#define EVENT_OP_MOUSE_DOWN     10
#define EVENT_OP_MOUSE_UP       11
#define EVENT_OP_MOUSE_MOVE     12

#define EVENT_OP_KEY_DOWN		10
#define EVENT_OP_KEY_UP			11

CWndIos::CWndIos() {
    m_dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    m_dwExStyle = 0;
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
    case WM_KILLFOCUS : EnableKeyMap(false); break;
    case WM_SETFOCUS : EnableKeyMap(true); break;
    default: bHandled = FALSE; break;
    }

    if (bHandled) {
        return lRes;
    }
    return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CWndIos::OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    QRect *lprc = (QRect*)lParam;
    return 0;
}

LRESULT CWndIos::OnMoving(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    return 0;
}

LRESULT CWndIos::OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    QPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    MouseOpSync(EVENT_OP_MOUSE_MOVE, pt.x, pt.y);
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

        if (PathIsDirectory(file_path.c_str())) continue;

        auto pos = file_path.rfind(L".");

        if (pos == wstring::npos) continue;

        auto prefix = file_path.substr(pos + 1, file_path.length() - pos - 1); 

        if (prefix == L"ipa")  CIosMgr::Instance()->InstallApp(file_path);
        else if (prefix == L"pack") {
            BOOL restart = FALSE;
            auto status = CIosMgr::Instance()->UpdatePackage(file_path, restart);
            wstring str_msg = (status == 0 ? L"更新成功：" : L"更新失败：");
            str_msg += file_path;
            ShowToast(CGlobalData::Instance()->GetMainWnd(), str_msg.c_str());
        }
        else {
            OPENFILENAME ofn;  
            ZeroMemory(&ofn, sizeof(ofn));  
            TCHAR szFile[MAX_PATH + 1] = { 0 };

            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = CGlobalData::Instance()->GetMainWnd(); 
            ofn.lpstrFilter = L"All(*.*)\0*.*\0\0";
            ofn.lpstrCustomFilter = nullptr;
            ofn.nMaxCustFilter = 0;
            ofn.nFilterIndex = 1;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrInitialDir = NULL;
            ofn.lpstrTitle = L"Save File As";
            ofn.nFileOffset = 0;
            ofn.nFileExtension = 0;
            ofn.lpstrDefExt = prefix.c_str();
            ofn.lCustData = 0;
            ofn.Flags = OFN_OVERWRITEPROMPT;
            ofn.lpfnHook = (LPOFNHOOKPROC)(FARPROC)nullptr;
            ofn.lpTemplateName = nullptr;

            if (GetSaveFileName(&ofn)) {
                ShowToast(CGlobalData::Instance()->GetMainWnd(), CIosMgr::Instance()->SaveEngineReport(file_path, szFile) ? L"保存成功" : L"保存失败");
            }
        }
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
