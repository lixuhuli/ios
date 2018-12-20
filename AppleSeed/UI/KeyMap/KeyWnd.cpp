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
#include "IntelligentUI.h"

CKeyWnd::CKeyWnd()
 : btn_tool_handle_(nullptr)
 , btn_tool_normal_(nullptr)
 , key_body_(nullptr)
 , panel_tools_(nullptr)
 , opt_right_run_(nullptr)
 , browser_mode_(true)
 , m_pMDLDropTarget(nullptr)
 , m_pMDLDragDataSrc(nullptr)
 , key_slider_trans_(nullptr)
 , lbl_trans_percent_(nullptr) {
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
    scene_info_->loadScene("D:\\IOS\\ios\\bin\\Debug\\ioskeymap\\com.tencent.smoba");

    scene_bak_info_ = scene_info_->cloner();

    if (btn_tool_handle_) btn_tool_handle_->OnEvent += MakeDelegate(this, &CKeyWnd::OnToolEvent);

    if (key_slider_trans_) key_slider_trans_->SetValue(scene_info_->opacity());

    CStdString str;
    str.Format(L"%d%s", scene_info_->opacity(), L"%");
    lbl_trans_percent_->SetText(str.GetData());

    LoadKeyItems();

    if (key_body_) key_body_->OnSize += MakeDelegate(this, &CKeyWnd::OnKeyBodySize);
}

void CKeyWnd::SetBrowserMode(bool browser_mode) {
    if (browser_mode_ == browser_mode) return;

    browser_mode_ = browser_mode;
    if (browser_mode_) {
        if (panel_tools_) panel_tools_->SetVisible(false);
        if (key_body_) key_body_->SetBkColor(0);
        LoadKeyItems();
    }
    else {
        if (panel_tools_) panel_tools_->SetVisible(true);
        if (key_body_) key_body_->SetBkColor(0x80000000);
    }

    if (!key_body_) return;

    auto opacity = ((browser_mode_ && scene_info_) ? scene_info_->opacity() : 100);

    for (int i = 0; i < key_body_->GetCount(); i++) {
        auto it = dynamic_cast<Ikey*>(key_body_->GetItemAt(i));
        if (it) it->UpdateBrowserMode(browser_mode, opacity);
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
    
    normal_ctrl->Subscribe(DUI_MSGTYPE_POS_CHANGED, MakeDelegate(this, &CKeyWnd::OnKeyPosChanged));

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

    CenterKey(handle_ctrl);
    KeyToScreen(handle_ctrl);

    return true;
}

bool CKeyWnd::OnBtnToolNormal(void* param) {
    if (!key_body_ || !scene_info_) return true;

    auto normal_ctrl = (CNormalUI*)CreateNormalKey();
    if (!normal_ctrl) return true;

    CenterKey(normal_ctrl);
    KeyToScreen(normal_ctrl);

    return true;
}

void CKeyWnd::CenterKey(CControlUI* control) {
    if (!key_body_ || !control) return;

    auto rc_body = key_body_->GetPos();

    auto width = control->GetFixedWidth();
    auto height = control->GetFixedHeight();

    QRect rc;
    rc.left = (rc_body.left + rc_body.right - width) / 2;
    rc.right = rc.left + width;
    rc.top = (rc_body.top + rc_body.bottom - height) / 2;
    rc.bottom = rc.top + height;

    control->SetPos(rc);
}

void CKeyWnd::KeyToScreen(CControlUI* control) {
    if (!control) return;

    auto rc = control->GetPos();

    QPoint point(rc.left, rc.top);

    KeyToScreen(&point);

    auto it = dynamic_cast<Ikey*>(control);

    if (it) it->SetScreenPosX(point.x);
    if (it) it->SetScreenPosY(point.y);
}

bool CKeyWnd::KeyToScreen(LPPOINT point) {
    if (!point || !key_body_) return false;

    auto screen_width = scene_info_->screen_width();
    auto screen_height = scene_info_->screen_height();

    if (screen_width <= 0 || screen_height <= 0) return false;

    if (CIosMgr::Instance()->IosWndScale() <= 0.000001) return false;

    auto rc_body = key_body_->GetPos();

    auto game_height = rc_body.GetHeight();
    auto game_width  = int((double)game_height / CIosMgr::Instance()->IosWndScale() + 0.5);

    int offset_x = 0;
    int offset_y = 0;
    if (game_width < rc_body.GetWidth()) {
        offset_x = (rc_body.GetWidth() - game_width) / 2;
    }
    else if (game_width > rc_body.GetWidth()) {
        game_width = rc_body.GetWidth();
        game_height = int((double)game_width * CIosMgr::Instance()->IosWndScale() + 0.5);
        offset_y = (rc_body.GetHeight() - game_height) / 2;
    }

    point->x = (int)((double)(point->x - rc_body.left - offset_x) *  (double)screen_width / (double)game_width + 0.5);
    point->y = (int)((double)(point->y - rc_body.top - offset_y) *  (double)screen_height / (double)game_height + 0.5);

    return true;
}

DuiLib::CControlUI* CKeyWnd::CreateRightMouse() {
    if (!key_body_) return nullptr;

    CDialogBuilder builder;
    auto right_ctrl = (CRightMouseMoveUI*)builder.Create(L"KeyMap/body/RightMouseMove.xml", (UINT)0, this, &m_pm);
    if (!right_ctrl) return nullptr;

    key_body_->Add(right_ctrl);

    auto edit_key = right_ctrl->edit_key();
    if (edit_key) edit_key->Subscribe(DUI_MSGTYPE_CHARCHANGED, MakeDelegate(this, &CKeyWnd::OnEditRightMouseChanged));

    auto slider_mouse = right_ctrl->slider_mouse();
    if (slider_mouse) slider_mouse->Subscribe(DUI_MSGTYPE_VALUECHANGED, MakeDelegate(this, &CKeyWnd::OnSliderRightMouseValueChanged));

    right_ctrl->Subscribe(DUI_MSGTYPE_POS_CHANGED, MakeDelegate(this, &CKeyWnd::OnKeyPosChanged));

    return right_ctrl;
}

bool CKeyWnd::OnBtnToolRightRun(void* param) {
    if (!key_body_ || !opt_right_run_) return true;

    if (!opt_right_run_->IsSelected()) {
        auto right_ctrl = CreateRightMouse();
        if (!right_ctrl) return true;

        auto rc_body = key_body_->GetPos();

        auto width = right_ctrl->GetFixedWidth();
        auto height = right_ctrl->GetFixedHeight();

        QRect rc;
        rc.left = (rc_body.left + rc_body.right - width) / 2;
        rc.right = rc.left + width;
        rc.top = (rc_body.top + rc_body.bottom - height) / 2;
        rc.bottom = rc.top + height;

        right_ctrl->SetPos(rc);

        //emulator::ItemInfo item;
        //item.itemType = emulator::RIGHT_MOUSE_MOVE;
        //item.nItemPosX = 159;
        //item.nItemPosY = 539;
        //item.nItemWidth = rc.GetWidth();
        //item.nItemHeight = rc.GetHeight();

        //emulator::KeyInfo info; 
        //info.nValue = -9;
        //info.strDescription = "右键行走";
        //info.strKeyString = "鼠标右键";
        //item.keys.push_back(info);

        //info.nValue = 88;
        //info.strDescription = "中断行走";
        //info.strKeyString = "X";
        //info.nPointX = 209;
        //info.
        //item.keys.push_back(info);

        //scene_bak_info_->AddItem(item);

        //edit_key->SetTag(edit_key->GetKeyValue());
        //normal_ctrl->SetName(keyboard.c_str());
    }
    else {
        std::string key_string;
        if (scene_bak_info_->DeleteRightMouse(key_string) && !key_string.empty()) {
            auto normal_ctrl = key_body_->FindSubControl(PublicLib::Utf8ToU(key_string).c_str());
            if (normal_ctrl) key_body_->Remove(normal_ctrl);
        }
    }

    return true;
}

DuiLib::CControlUI* CKeyWnd::CreateIntelligent() {
    if (!key_body_) return nullptr;

    CDialogBuilder builder;
    auto intelligent_ctrl = (CIntelligentUI*)builder.Create(L"KeyMap/body/IntelligentCtrl.xml", (UINT)0, this, &m_pm);
    if (!intelligent_ctrl) return nullptr;

    key_body_->Add(intelligent_ctrl);

    auto edit_key = intelligent_ctrl->edit_key();
    if (edit_key) edit_key->Subscribe(DUI_MSGTYPE_CHARCHANGED, MakeDelegate(this, &CKeyWnd::OnEditIntelligentChanged));

    auto slider_mouse = intelligent_ctrl->slider_mouse();
    if (slider_mouse) slider_mouse->Subscribe(DUI_MSGTYPE_VALUECHANGED, MakeDelegate(this, &CKeyWnd::OnSliderIntelligentChanged));

    auto opt_switch = intelligent_ctrl->opt_switch();
    if (opt_switch) opt_switch->Subscribe(DUI_MSGTYPE_CLICK, MakeDelegate(this, &CKeyWnd::OnOptIntelligentSwitch));

    intelligent_ctrl->Subscribe(DUI_MSGTYPE_POS_CHANGED, MakeDelegate(this, &CKeyWnd::OnKeyPosChanged));

    return intelligent_ctrl;
}

bool CKeyWnd::OnBtnToolIntelligent(void* param) {
    if (!key_body_) return true;

    auto intelligent_ctrl = (CIntelligentUI*)CreateIntelligent();
    if (!intelligent_ctrl) return true;

    CenterKey(intelligent_ctrl);
    KeyToScreen(intelligent_ctrl);

    return true;
}

bool CKeyWnd::OnBtnSave(void* param) {
    if (!scene_info_ || !scene_bak_info_) return true;

    scene_info_ = scene_bak_info_;

    wstring file_path = CGlobalData::Instance()->GetRunPath() + L"ioskeymap";

    if (!PathFileExists(file_path.c_str())) SHCreateDirectory(NULL, file_path.c_str());

    file_path += L"\\com.tencent.smoba";

    scene_info_->saveScene(PublicLib::UToUtf8(file_path).c_str());

    CIosMgr::Instance()->UpdateKeyMap(file_path);

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
    else if (control_name == L"Intelligent") return new CIntelligentUI();

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

    auto screen_width = scene_info_->screen_width();
    auto screen_height = scene_info_->screen_height();
    if (screen_width <= 0 || screen_height <= 0) return true;

    QRect rc_body = key_body_->GetPos();
    if (rc_body.GetWidth() == 0 || rc_body.GetHeight() == 0) return true;

    wstring keyboard = edit_key->GetText().GetData();

    if (keyboard.empty()) return true;

    if (scene_bak_info_->DeleteKey(edit_key->GetKeyValue())) {
        auto key_string = CGlobalData::Instance()->GetKeyboardStr(edit_key->GetKeyValue());
        auto normal_ctrl = key_body_->FindSubControl(key_string.c_str());
        if (normal_ctrl) key_body_->Remove(normal_ctrl);
    }

    auto normal_ctrl = edit_key->GetParent();
    if (!normal_ctrl) return true;

    if (!scene_bak_info_->set_key(emulator::NORMAL_KEY, edit_key->GetTag(), edit_key->GetKeyValue(), PublicLib::UToUtf8(keyboard))) {
        QRect rc = normal_ctrl->GetPos();

        emulator::ItemInfo item;
        item.itemType = emulator::NORMAL_KEY;
        item.nItemPosX = rc.left * screen_width / rc_body.GetWidth();
        item.nItemPosY = rc.top * screen_height / rc_body.GetHeight();
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
    }

    edit_key->SetTag(edit_key->GetKeyValue());
    normal_ctrl->SetName(keyboard.c_str());
    normal_ctrl->SetTag(edit_key->GetKeyValue());

    return true;
}

bool CKeyWnd::OnEditRightMouseChanged(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (!pNotify || !pNotify->pSender || !scene_bak_info_ || !key_body_) return true;

    CKeyEditUI* edit_key = dynamic_cast<CKeyEditUI*>(pNotify->pSender);
    if (!edit_key) return true;

    if (edit_key->GetTag() == edit_key->GetKeyValue()) return true;

    auto screen_width = scene_info_->screen_width();
    auto screen_height = scene_info_->screen_height();
    if (screen_width <= 0 || screen_height <= 0) return true;

    QRect rc_body = key_body_->GetPos();
    if (rc_body.GetWidth() == 0 || rc_body.GetHeight() == 0) return true;

    wstring keyboard = edit_key->GetText().GetData();

    if (keyboard.empty()) return true;

    if (scene_bak_info_->DeleteKey(edit_key->GetKeyValue())) {
        auto key_string = CGlobalData::Instance()->GetKeyboardStr(edit_key->GetKeyValue());
        auto normal_ctrl = key_body_->FindSubControl(key_string.c_str());
        if (normal_ctrl) key_body_->Remove(normal_ctrl);
    }

    auto right_mouse = edit_key->GetParent();
    if (!right_mouse) return true;

    if (scene_bak_info_->set_key(emulator::RIGHT_MOUSE_MOVE, edit_key->GetTag(), edit_key->GetKeyValue(), PublicLib::UToUtf8(keyboard))) {
        edit_key->SetTag(edit_key->GetKeyValue());
        right_mouse->SetName(keyboard.c_str());
        right_mouse->SetTag(edit_key->GetKeyValue());
    }

    return true;
}

bool CKeyWnd::OnEditIntelligentChanged(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (!pNotify || !pNotify->pSender || !scene_bak_info_ || !key_body_) return true;

    CKeyEditUI* edit_key = dynamic_cast<CKeyEditUI*>(pNotify->pSender);
    if (!edit_key) return true;

    if (edit_key->GetTag() == edit_key->GetKeyValue()) return true;

    auto screen_width = scene_info_->screen_width();
    auto screen_height = scene_info_->screen_height();
    if (screen_width <= 0 || screen_height <= 0) return true;

    QRect rc_body = key_body_->GetPos();
    if (rc_body.GetWidth() == 0 || rc_body.GetHeight() == 0) return true;

    wstring keyboard = edit_key->GetText().GetData();

    if (keyboard.empty()) return true;

    if (scene_bak_info_->DeleteKey(edit_key->GetKeyValue())) {
        auto key_string = CGlobalData::Instance()->GetKeyboardStr(edit_key->GetKeyValue());
        auto normal_ctrl = key_body_->FindSubControl(key_string.c_str());
        if (normal_ctrl) key_body_->Remove(normal_ctrl);
    }

    auto intelligent = edit_key->GetParent();
    if (!intelligent) return true;

    if (scene_bak_info_->set_key(emulator::INTELLIGENT_CASTING_KEY, edit_key->GetTag(), edit_key->GetKeyValue(), PublicLib::UToUtf8(keyboard))) {
        edit_key->SetTag(edit_key->GetKeyValue());
        intelligent->SetName(keyboard.c_str());
        intelligent->SetTag(edit_key->GetKeyValue());
    }

    return true;
}

bool CKeyWnd::OnSliderRightMouseValueChanged(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (!pNotify || !pNotify->pSender || !scene_bak_info_) return true;

    CSliderUI* slider_mouse = dynamic_cast<CSliderUI*>(pNotify->pSender);
    if (!slider_mouse) return true;

    scene_bak_info_->set_right_mouse_value(slider_mouse->GetValue());

    return true;
}

bool CKeyWnd::OnSliderIntelligentChanged(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (!pNotify || !pNotify->pSender || !scene_bak_info_) return true;

    CSliderUI* slider_mouse = dynamic_cast<CSliderUI*>(pNotify->pSender);
    if (!slider_mouse) return true;

    auto intelligent = (CIntelligentUI*)slider_mouse->GetParent();
    if (!intelligent) return true;

    auto edit_key = intelligent->edit_key();
    if (!edit_key) return true;

    scene_bak_info_->set_intelligent_value(edit_key->GetKeyValue(), slider_mouse->GetValue());

    return true;
}

bool CKeyWnd::OnOptIntelligentSwitch(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (!pNotify || !pNotify->pSender || !scene_bak_info_) return true;

    COptionUI* opt_switch = dynamic_cast<COptionUI*>(pNotify->pSender);
    if (!opt_switch) return true;

    auto intelligent = (CIntelligentUI*)opt_switch->GetParent()->GetParent();
    if (!intelligent) return true;

    auto edit_key = intelligent->edit_key();
    if (!edit_key) return true;

    scene_bak_info_->set_intelligent_switch_on(edit_key->GetKeyValue(), !opt_switch->IsSelected());

    return true;
}

bool CKeyWnd::OnSliderKeyTransChanged(void* param) {
    if (!key_slider_trans_ || !scene_bak_info_) return true;
    if (scene_bak_info_) scene_bak_info_->set_opacity(key_slider_trans_->GetValue());

    CStdString str;
    str.Format(L"%d%s", scene_bak_info_->opacity(), L"%");
    lbl_trans_percent_->SetText(str.GetData());

    return true;
}

void CKeyWnd::LoadKeyItems() {
    if (!scene_info_ || !key_body_) return;

    key_body_->RemoveAll();

    auto opacity = scene_info_->opacity();

    auto items = scene_info_->GetKeyItems();
    auto it = items.begin();
    for (; it != items.end(); it++) {
        if (it->itemType == emulator::NORMAL_KEY) {
            if (it->keys.size() < 1) continue;

            auto normal_ctrl = (CNormalUI*)CreateNormalKey();
            if (!normal_ctrl) continue;

            normal_ctrl->SetName(PublicLib::Utf8ToU(it->keys[0].strKeyString).c_str());
            normal_ctrl->SetTag(it->keys[0].nValue);
            normal_ctrl->UpdateBrowserMode(true, opacity);
            normal_ctrl->SetScreenPosX(it->nItemPosX);
            normal_ctrl->SetScreenPosY(it->nItemPosY);
            normal_ctrl->SetKeyType(it->itemType);

            auto edit_key = normal_ctrl->edit_key();

            if (edit_key) {
                edit_key->SetTag(it->keys[0].nValue);
                edit_key->SetKeyValue(it->keys[0].nValue);
                edit_key->CControlUI::SetText(PublicLib::Utf8ToU(it->keys[0].strKeyString).c_str());
            }
        }
        else if (it->itemType == emulator::RIGHT_MOUSE_MOVE) {
            if (it->keys.size() < 2) continue;

            auto right_ctrl = (CRightMouseMoveUI*)CreateRightMouse();
            if (!right_ctrl) continue;

            right_ctrl->SetName(PublicLib::Utf8ToU(it->keys[1].strKeyString).c_str());
            right_ctrl->SetTag(it->keys[1].nValue);
            right_ctrl->UpdateBrowserMode(true, opacity);
            right_ctrl->SetScreenPosX(it->nItemPosX);
            right_ctrl->SetScreenPosY(it->nItemPosY);
            right_ctrl->SetKeyType(it->itemType);

            auto edit_key = right_ctrl->edit_key();
            if (edit_key) {
                edit_key->SetTag(it->keys[1].nValue);
                edit_key->SetKeyValue(it->keys[1].nValue);
                edit_key->CControlUI::SetText(PublicLib::Utf8ToU(it->keys[1].strKeyString).c_str());
            }

            auto slider_mouse = right_ctrl->slider_mouse();
            if (slider_mouse) slider_mouse->SetValue(it->nItemSlider);

            if (opt_right_run_) opt_right_run_->Selected(true);
        }
        else if (it->itemType == emulator::INTELLIGENT_CASTING_KEY) {
            if (it->keys.size() < 1) continue;

            auto intelligent_ctrl = (CIntelligentUI*)CreateIntelligent();
            if (!intelligent_ctrl) continue;

            intelligent_ctrl->SetName(PublicLib::Utf8ToU(it->keys[0].strKeyString).c_str());
            intelligent_ctrl->SetTag(it->keys[0].nValue);
            intelligent_ctrl->UpdateBrowserMode(true, opacity);
            intelligent_ctrl->SetScreenPosX(it->nItemPosX);
            intelligent_ctrl->SetScreenPosY(it->nItemPosY);
            intelligent_ctrl->SetKeyType(it->itemType);

            auto edit_key = intelligent_ctrl->edit_key();
            if (edit_key) {
                edit_key->SetTag(it->keys[0].nValue);
                edit_key->SetKeyValue(it->keys[0].nValue);
                edit_key->CControlUI::SetText(PublicLib::Utf8ToU(it->keys[0].strKeyString).c_str());
            }

            auto slider_mouse = intelligent_ctrl->slider_mouse();
            if (slider_mouse) slider_mouse->SetValue(it->nItemSlider);

            auto opt_switch = intelligent_ctrl->opt_switch();
            if (opt_switch) opt_switch->Selected(it->nItemRightMoveStop == 0);
        }
        
    }
}

bool CKeyWnd::OnKeyBodySize(void* param) {
    UpdateItemsPos();
    return true;
}

void CKeyWnd::UpdateItemsPos() {
    if (!key_body_ || !scene_info_) return;

    auto screen_width = scene_info_->screen_width();
    auto screen_height = scene_info_->screen_height();

    if (screen_width <= 0 || screen_height <= 0) return;
    if (CIosMgr::Instance()->IosWndScale() <= 0.000001) return;

    QRect rc_body = key_body_->GetPos();

    auto game_height = rc_body.GetHeight();
    auto game_width  = int((double)game_height / CIosMgr::Instance()->IosWndScale() + 0.5);

    int offset_x = 0;
    int offset_y = 0;
    if (game_width < rc_body.GetWidth()) {
        offset_x = (rc_body.GetWidth() - game_width) / 2;
    }
    else if (game_width > rc_body.GetWidth()) {
        game_width = rc_body.GetWidth();
        game_height = int((double)game_width * CIosMgr::Instance()->IosWndScale() + 0.5);
        offset_y = (rc_body.GetHeight() - game_height) / 2;
    }

    for (int i = 0; i < key_body_->GetCount(); i++) {
        auto it = key_body_->GetItemAt(i);
        if (!it) continue;

        auto key = dynamic_cast<Ikey*>(it);
        if (!key) continue;

        auto width = it->GetFixedWidth();
        auto height = it->GetFixedHeight();

        QRect rc;
        rc.left = rc_body.left + (LONG)((double)key->ScreenPosX() * (double)game_width / (double)screen_width + 0.5) + offset_x;
        rc.top = rc_body.top + (LONG)((double)key->ScreenPosY() * (double)game_height / (double)screen_height + 0.5) + offset_y;
        rc.right = rc.left + width;
        rc.bottom = rc.top + height;

        it->SetPos(rc);
    }
}

bool CKeyWnd::OnKeyPosChanged(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (!pNotify || !pNotify->pSender || !scene_bak_info_) return true;

    auto it = dynamic_cast<Ikey*>(pNotify->pSender);
    if (!it) return true;

    auto control = pNotify->pSender;
    if (!control) return true;

    KeyToScreen(control);

    switch (it->KeyType()) {
    case emulator::NORMAL_KEY: {
        auto normal_ctrl = (CNormalUI*)control;
        if (!normal_ctrl) break;
        auto edit_key = normal_ctrl->edit_key();
        if (!edit_key) break;

        auto rc = normal_ctrl->GetPos();
        QPoint point(rc.left, rc.top);
        if (!KeyToScreen(&point)) break;

        auto rc_edit = edit_key->GetPos();

        point.x = point.x + int(double(rc_edit.GetWidth()) / 2.0 + 0.5);
        point.y = point.y + int(double(rc_edit.GetHeight()) / 2.0 + 0.5);

        scene_bak_info_->set_key_pos(normal_ctrl->GetTag(), it->ScreenPosX(), it->ScreenPosY(), point);
    }
    break;
    case emulator::INTELLIGENT_CASTING_KEY: {
        auto intelligent_ctrl = (CIntelligentUI*)control;
        if (!intelligent_ctrl) break;
        auto edit_key = intelligent_ctrl->edit_key();
        if (!edit_key) break;

        auto rc = intelligent_ctrl->GetPos();
        QPoint point(rc.left, rc.top);
        if (!KeyToScreen(&point)) break;

        auto rc_edit = edit_key->GetPos();

        point.x = point.x + int(double(rc_edit.GetWidth()) / 2.0 + 0.5) + rc_edit.left - rc.left;
        point.y = point.y + int(double(rc_edit.GetHeight()) / 2.0 + 0.5) + rc_edit.top - rc.top;

        scene_bak_info_->set_key_pos(edit_key->GetKeyValue(), it->ScreenPosX(), it->ScreenPosY(), point);
    }
    break;
    case emulator::RIGHT_MOUSE_MOVE: {
        auto right_ctrl = (CRightMouseMoveUI*)control;
        if (!right_ctrl) break;
        auto edit_key = right_ctrl->edit_key();
        if (!edit_key) break;

        auto rc = right_ctrl->GetPos();
        QPoint point(rc.left, rc.top);
        if (!KeyToScreen(&point)) break;

        auto rc_edit = edit_key->GetPos();

        point.x = point.x + int(double(rc_edit.GetWidth()) / 2.0 + 0.5) + rc_edit.left - rc.left;
        point.y = point.y + int(double(rc_edit.GetHeight()) / 2.0 + 0.5) + rc_edit.top - rc.top;

        scene_bak_info_->set_key_pos(edit_key->GetKeyValue(), it->ScreenPosX(), it->ScreenPosY(), point);
        scene_bak_info_->set_key_pos(-9, point);
    }
    break;
    default:
        break;
    }

    //scene_bak_info_->set_key_pos(control->GetTag(), it->ScreenPosX(), it->ScreenPosY());

    return true;
}
