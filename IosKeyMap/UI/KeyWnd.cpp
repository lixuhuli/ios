#include "StdAfx.h"
#include "KeyWnd.h"
#include "dragdrophandle.h"
#include "RemoteHandleUI.h"
#include "NormalUI.h"
#include "RightMouseMoveUI.h"
#include "KeyEditUI.h"

CKeyWnd::CKeyWnd()
 : btn_tool_handle_(nullptr)
 , btn_tool_normal_(nullptr)
 , key_body_(nullptr)
 , opt_right_run_(nullptr)
 , m_pMDLDropTarget(nullptr)
 , m_pMDLDragDataSrc(nullptr) {
    m_dwStyle = UI_WNDSTYLE_DIALOG | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    m_pMDLDragDataSrc = new CMDLDropSource();
}

CKeyWnd::~CKeyWnd() {
}

LPCWSTR CKeyWnd::GetXmlPath() const {
    return L"KeyWnd.xml";
}

LPCTSTR CKeyWnd::GetWindowClassName() const {
    return L"KeyWnd";
}

void CKeyWnd::InitWindow() {
    __super::InitWindow();

    auto parent = GetParent(m_hWnd);
    if (!parent || !::IsWindow(parent)) return;

    //m_pMDLDropTarget = new CMDLDropTarget();
    //if (m_pMDLDropTarget != nullptr) {
    //    CoLockObjectExternal(m_pMDLDropTarget, TRUE, TRUE);
    //    RegisterDragDrop(parent, m_pMDLDropTarget);
    //}

    if (btn_tool_handle_) btn_tool_handle_->OnEvent += MakeDelegate(this, &CKeyWnd::OnToolEvent);
}

LRESULT CKeyWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;
    switch (uMsg) {
    case WM_SETCURSOR: lRes = OnSetCursor(wParam, lParam, bHandled);
    default: bHandled = FALSE;
    }

    if (bHandled) {
        return lRes;
    }
    return __super::HandleMessage(uMsg, wParam, lParam);
}

bool CKeyWnd::OnClickBtnClose(void* lpParam) {
    Close(IDCLOSE);
    return true;
}

bool CKeyWnd::OnBtnToolHandle(void* param) {
    if (!key_body_) return true;

    auto handle_ctrl = m_pm.FindSubControlByClass(key_body_, L"RemoteHandleUI");
    if (!handle_ctrl) {
        CDialogBuilder builder;
        handle_ctrl = (CRemoteHandleUI*)builder.Create(L"body/HandleCtrl.xml", (UINT)0, this, &m_pm);
        if (!handle_ctrl) return true;
        key_body_->Add(handle_ctrl);
    }

    auto rc_body = key_body_->GetPos();

    auto width = handle_ctrl->GetTag();
    auto height = handle_ctrl->GetTag();

    QRect rc;
    rc.left = (rc_body.left + rc_body.right - width) / 2;
    rc.right = rc.left + width;
    rc.top = (rc_body.top + rc_body.bottom - height) / 2;
    rc.bottom = rc.top + height;

    handle_ctrl->SetPos(rc);

    return true;
}

bool CKeyWnd::OnBtnToolNormal(void* param) {
    if (!key_body_) return true;

    CDialogBuilder builder;
    auto normal_ctrl = (CNormalUI*)builder.Create(L"body/NormalCtrl.xml", (UINT)0, this, &m_pm);
    if (!normal_ctrl) return true;
    key_body_->Add(normal_ctrl);

    auto rc_body = key_body_->GetPos();

    auto width = normal_ctrl->GetFixedWidth();
    auto height = normal_ctrl->GetFixedHeight();

    QRect rc;
    rc.left = (rc_body.left + rc_body.right - width) / 2;
    rc.right = rc.left + width;
    rc.top = (rc_body.top + rc_body.bottom - height) / 2;
    rc.bottom = rc.top + height;

    normal_ctrl->SetPos(rc);

    return true;
}

bool CKeyWnd::OnBtnToolRightRun(void* param) {
    if (!key_body_) return true;

    CDialogBuilder builder;
    auto normal_ctrl = (CNormalUI*)builder.Create(L"body/RightMouseMove.xml", (UINT)0, this, &m_pm);
    if (!normal_ctrl) return true;
    key_body_->Add(normal_ctrl);

    auto rc_body = key_body_->GetPos();

    auto width = normal_ctrl->GetFixedWidth();
    auto height = normal_ctrl->GetFixedHeight();

    QRect rc;
    rc.left = (rc_body.left + rc_body.right - width) / 2;
    rc.right = rc.left + width;
    rc.top = (rc_body.top + rc_body.bottom - height) / 2;
    rc.bottom = rc.top + height;

    normal_ctrl->SetPos(rc);

    return true;
}

LRESULT CKeyWnd::OnSetCursor(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    return 0;
}

void CKeyWnd::OnFinalMessage(HWND hWnd) {
    __super::OnFinalMessage(hWnd);
}

bool CKeyWnd::OnToolEvent(void* param) {
    TEventUI* event = (TEventUI*)param;
    if (!event || !event->pSender) return true;

    if (event->Type == UIEVENT_BUTTONDOWN) {
        //OnDragDrop();
        return true;
    }

    return true;
}

void CKeyWnd::OnDragDrop() {
    char sz[BUF_LEN] = { '\0' };
    //_stprintf_s(sz, BUF_LEN - 1, _T("%d"), 1);

    //_stprintf_s(sz, "%d", 1);
    DWORD dwEffect;

    LPDATAOBJECT pDataObject;
    pDataObject = (LPDATAOBJECT) new CDataObject(sz);
    // This drag source only allows copying of data. Move and link is not allowed.
    DoDragDrop(pDataObject, m_pMDLDragDataSrc, DROPEFFECT_COPY, &dwEffect);
    pDataObject->Release();
}

CControlUI* CKeyWnd::CreateControl(LPCTSTR pstrClass) {
    if (!pstrClass) return nullptr;

    std::wstring control_name = pstrClass;
    if (control_name == L"RemoteHandle") return new CRemoteHandleUI();
    else if (control_name == L"Normal") return new CNormalUI();
    else if (control_name == L"RightMouseMove") return new CRightMouseMoveUI();
    else if (control_name == L"KeyEdit") return new CKeyEditUI();

    return nullptr;
}

LRESULT CKeyWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    auto lr = __super::OnCreate(uMsg, wParam, lParam, bHandled);
    if (lr != -1) m_pm.SetLayeredWithPicture(true);
    return lr;
}

STDMETHODIMP CMDLDropTarget::Drop(LPDATAOBJECT pDataObj, DWORD grfKeyState,
    POINTL pt, LPDWORD pdwEffect) {



    return E_FAIL;
}