#include "StdAfx.h"
#include "KeyWnd.h"
#include "dragdrophandle.h"
#include "RemoteHandleUI.h"
#include "NormalUI.h"
#include "RightMouseMoveUI.h"
#include "KeyEditUI.h"
#include "Ios/IosMgr.h"
#include "Ios/scene_info.h"
#include "GlobalData.h"

CKeyWnd::CKeyWnd()
 : btn_tool_handle_(nullptr)
 , btn_tool_normal_(nullptr)
 , key_body_(nullptr)
 , panel_tools_(nullptr)
 , opt_right_run_(nullptr)
 , browser_mode_(true)
 , m_pMDLDropTarget(nullptr)
 , m_pMDLDragDataSrc(nullptr) {
    m_dwStyle = UI_WNDSTYLE_DIALOG | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    m_pMDLDragDataSrc = new CMDLDropSource();
}

CKeyWnd::~CKeyWnd() {
    scene_info_ = nullptr;
    scene_bak_info_ = nullptr;
}

LPCWSTR CKeyWnd::GetXmlPath() const {
    return L"KeyMap/KeyWnd.xml";
}

LPCTSTR CKeyWnd::GetWindowClassName() const {
    return L"KeyWnd";
}

base::WeakPtr<emulator::iSceneInfo> CKeyWnd::scene_info() {
    if (scene_info_) return scene_info_->AsWeakPtr();
    return base::WeakPtr<emulator::iSceneInfo>();
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

    scene_info_ = new emulator::SceneInfo();
    scene_info_->loadScene("D:\\PC\\trunk\\ios\\bin\\Debug\\sgame");

    scene_bak_info_ = scene_info_;

    scene_info_->right_mouse_off();

    if (btn_tool_handle_) btn_tool_handle_->OnEvent += MakeDelegate(this, &CKeyWnd::OnToolEvent);

    LoadNormalKey();
}

void CKeyWnd::SetBrowserMode(bool browser_mode) {
    if (browser_mode_ == browser_mode) return;

    browser_mode_ = browser_mode;
    if (browser_mode_) {
        if (panel_tools_) panel_tools_->SetVisible(false);
        if (key_body_) key_body_->SetBkColor(0);
    }
    else {
        if (panel_tools_) panel_tools_->SetVisible(true);
        if (key_body_) key_body_->SetBkColor(0x80000000);
    }
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
    SetBrowserMode(true);
    CIosMgr::Instance()->UpdateKeyWnd();
    return true;
}

CControlUI* CKeyWnd::CreateNormalKey() {
    if (!key_body_) return nullptr;

    CDialogBuilder builder;
    auto normal_ctrl = (CNormalUI*)builder.Create(L"KeyMap/body/NormalCtrl.xml", (UINT)0, this, &m_pm);
    if (!normal_ctrl) return nullptr;

    key_body_->Add(normal_ctrl);

    auto edit_key = normal_ctrl->edit_key();
    if (edit_key) edit_key->Subscribe(DUI_MSGTYPE_CHARCHANGED, MakeDelegate(this, &CKeyWnd::OnEditKeyChanged));

    return normal_ctrl;
}

bool CKeyWnd::OnBtnToolHandle(void* param) {
    if (!key_body_) return true;

    auto handle_ctrl = m_pm.FindSubControlByClass(key_body_, L"RemoteHandleUI");
    if (!handle_ctrl) {
        CDialogBuilder builder;
        handle_ctrl = (CRemoteHandleUI*)builder.Create(L"KeyMap/body/HandleCtrl.xml", (UINT)0, this, &m_pm);
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

    auto normal_ctrl = (CNormalUI*)CreateNormalKey();
    if (!normal_ctrl) return true;

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

bool CKeyWnd::OnBtnSave(void* param) {
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

bool CKeyWnd::OnEditKeyChanged(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (!pNotify || !pNotify->pSender || !scene_bak_info_ || !key_body_) return true;

    CKeyEditUI* edit_key = dynamic_cast<CKeyEditUI*>(pNotify->pSender);
    if (!edit_key) return true;

    if (edit_key->GetTag() == edit_key->GetKeyValue()) return true;

    wstring keyboard = edit_key->GetText().GetData();

    if (keyboard.empty()) return true;

    if (scene_bak_info_->DeleteKey(emulator::NORMAL_KEY, edit_key->GetKeyValue())) {
        auto key_string = CGlobalData::Instance()->GetKeyboardStr(edit_key->GetKeyValue());
        auto normal_ctrl = key_body_->FindSubControl(key_string.c_str());
        if (normal_ctrl) key_body_->Remove(normal_ctrl);
    }

    auto normal_ctrl = edit_key->GetParent();
    if (!normal_ctrl) return true;

    if (!scene_bak_info_->set_normal_key(edit_key->GetTag(), edit_key->GetKeyValue(), PublicLib::UToUtf8(keyboard))) {
        QRect rc = normal_ctrl->GetPos();

        emulator::ItemInfo item;
        item.itemType = emulator::NORMAL_KEY;
        item.nItemPosX = rc.left;
        item.nItemPosY = rc.top;
        item.nItemFingerCount = 1;
        item.nItemWidth = rc.GetWidth();
        item.nItemHeight = rc.GetHeight();

        emulator::KeyInfo info; 
        info.nValue = edit_key->GetKeyValue();
        info.strDescription = "自定义技能";
        info.strKeyString = PublicLib::UToUtf8(keyboard);

        item.keys.push_back(info);
        scene_bak_info_->AddItem(item);

        edit_key->SetTag(edit_key->GetKeyValue());
        normal_ctrl->SetName(keyboard.c_str());
    }

    edit_key->SetTag(edit_key->GetKeyValue());
    normal_ctrl->SetName(keyboard.c_str());

    return true;
}

void CKeyWnd::LoadNormalKey() {
    if (!scene_info_ || !key_body_) return;

    auto items = scene_info_->GetKeyItemGather(emulator::NORMAL_KEY);

    auto it = items.begin();

    for (; it != items.end(); it++) {
        auto normal_ctrl = (CNormalUI*)CreateNormalKey();
        if (!normal_ctrl) break;

        auto rc_body = key_body_->GetPos();

        auto width = normal_ctrl->GetFixedWidth();
        auto height = normal_ctrl->GetFixedHeight();

        QRect rc;
        rc.left = it->nItemPosX;
        rc.top = it->nItemPosY;
        rc.right = rc.left + width;
        rc.bottom = rc.top + height;

        normal_ctrl->SetPos(rc);
        normal_ctrl->SetName(PublicLib::Utf8ToU(it->keys[0].strKeyString).c_str());

        auto edit_key = normal_ctrl->edit_key();

        if (edit_key) {
            edit_key->SetTag(it->keys[0].nValue);
            edit_key->SetKeyValue(it->keys[0].nValue);
            edit_key->CControlUI::SetText(PublicLib::Utf8ToU(it->keys[0].strKeyString).c_str());
        }
    }

}