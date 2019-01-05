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
#include "MsgDefine.h"
#include "KeyBrowserWnd.h"

CKeyWnd::CKeyWnd()
 : btn_tool_handle_(nullptr)
 , btn_tool_normal_(nullptr)
 , key_body_(nullptr)
 , panel_tools_(nullptr)
 , opt_right_run_(nullptr)
 , browser_mode_(true)
 , browser_wnd_(nullptr)
 , combox_keyboard_(nullptr)
 , m_pMDLDropTarget(nullptr)
 , m_pMDLDragDataSrc(nullptr)
 , key_slider_trans_(nullptr)
 , lbl_trans_percent_(nullptr)
 , btn_tool_intelligent_(nullptr) {
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
    browser_wnd_ = new CKeyBrowserWnd;
    if (!browser_wnd_) {
        CIosMgr::Instance()->CloseKeyWnd();
        return;
    }

    browser_wnd_->Create(CGlobalData::Instance()->GetMainWnd(), false);
    browser_wnd_->ShowWindow(false);
    UpdateBrowserWnd(nullptr);

    panel_tools_ = (CVerticalLayoutUI*)browser_wnd_->GetRoot();

    if (!panel_tools_) {
        CIosMgr::Instance()->CloseKeyWnd();
        return;
    }

    __super::InitWindow();

    auto parent = GetParent(m_hWnd);
    if (!parent || !::IsWindow(parent)) return;

    //m_pMDLDropTarget = new CMDLDropTarget();
    //if (m_pMDLDropTarget != nullptr) {
    //    CoLockObjectExternal(m_pMDLDropTarget, TRUE, TRUE);
    //    RegisterDragDrop(parent, m_pMDLDropTarget);
    //}

    ReadCloudKeyboardToCombox();

    UpdateSceneInfo();

    LoadKeyItems();

    UpdateCtrls();

    if (btn_tool_handle_) btn_tool_handle_->OnEvent += MakeDelegate(this, &CKeyWnd::OnToolEvent);

    if (key_body_) key_body_->OnSize += MakeDelegate(this, &CKeyWnd::OnKeyBodySize);
}

void CKeyWnd::UpdateCtrls() {
    if (!lbl_trans_percent_ || !key_slider_trans_ || !scene_info_) return;

    key_slider_trans_->SetValue(scene_info_->opacity());

    CStdString str;
    str.Format(L"%d%s", scene_info_->opacity(), L"%");
    lbl_trans_percent_->SetText(str.GetData());
}

void CKeyWnd::UpdateSceneInfo() {
    if (!combox_keyboard_ || combox_keyboard_->GetCount() < 0) return;

    auto index = combox_keyboard_->GetCurSel();
    if (index < 0) return;

    CListLabelElementUI* key_elem = (CListLabelElementUI*)combox_keyboard_->GetItemAt(index);
    if (!key_elem) return;

    scene_info_ = new emulator::SceneInfo();
    scene_info_->loadScene(PublicLib::UToUtf8(key_elem->GetUserData().GetData()).c_str());

    scene_bak_info_ = scene_info_->cloner();

    CIosMgr::Instance()->UpdateKeyMap(key_elem->GetUserData().GetData());
}

void CKeyWnd::ReadCloudKeyboardToCombox() {
    if (!combox_keyboard_) return;

    combox_keyboard_->RemoveAll();

    auto config = CIosMgr::Instance()->GetKeyMapDir() + L"\\conf.ini";

    wchar_t szValue[1024] = { 0 };
    DWORD dwLen = GetPrivateProfileString(L"setting", L"record", L"", szValue, 1024, config.c_str());
    if (dwLen <= 0) return;

    wstring records = szValue;

    std::vector<std::wstring> vec_records; 
    PublicLib::SplitStringW(records, L",", vec_records);

    auto it = vec_records.begin();
    for (; it != vec_records.end(); it++) {
        auto record = *it;

        memset(szValue, 0, sizeof(szValue));
        dwLen = GetPrivateProfileString(record.c_str(), L"name", L"", szValue, 1024, config.c_str());
        if (dwLen <= 0) continue;

        wstring key_name = szValue;

        memset(szValue, 0, sizeof(szValue));
        dwLen = GetPrivateProfileString(record.c_str(), L"file", L"", szValue, 1024, config.c_str());
        if (dwLen <= 0) continue;

        wstring key_file = szValue;
        key_file =  CIosMgr::Instance()->GetKeyMapDir() + L"\\" + key_file;

        CListLabelElementUI* key_elem = new CListLabelElementUI;
        if (!key_elem) continue;

        key_elem->SetFixedHeight(35);
        key_elem->SetName(key_name.c_str());
        key_elem->SetUserData(key_file.c_str());
        key_elem->SetText(key_name.c_str());

        combox_keyboard_->Add(key_elem);
    }
    
    if (combox_keyboard_->GetCount() > 0) combox_keyboard_->SelectItemV2(0);
}

void CKeyWnd::SetBrowserMode(bool browser_mode) {
    if (browser_mode_ == browser_mode) return;

    browser_mode_ = browser_mode;
    if (browser_mode_) {
        if (key_body_) key_body_->SetBkColor(0);
        LoadKeyItems();

        if (browser_wnd_) browser_wnd_->ShowWindow(false);
        if (btn_tool_handle_) btn_tool_handle_->SetTag(0);
        if (btn_tool_normal_) btn_tool_normal_->SetTag(0);
        if (btn_tool_intelligent_) btn_tool_intelligent_->SetTag(0);
    }
    else {
        if (panel_tools_) panel_tools_->SetVisible(true);
        if (key_body_) key_body_->SetBkColor(0x80000000);
        if (browser_wnd_) browser_wnd_->ShowWindow(true);
    }

    if (!key_body_) return;

    auto opacity = ((browser_mode_ && scene_info_) ? scene_info_->opacity() : 100);

    for (int i = 0; i < key_body_->GetCount(); i++) {
        auto it = dynamic_cast<Ikey*>(key_body_->GetItemAt(i));
        if (it) it->UpdateBrowserMode(browser_mode, opacity);
    }
}

void CKeyWnd::UpdateBrowserWnd(const QRect* lprc /*= nullptr*/) {
    if (!browser_wnd_) return;

    QRect rc;
    if (!lprc) GetWindowRect(m_hWnd, &rc);
    else rc = *lprc;

    rc.right = rc.left;
    rc.left -= 280;

    MoveWindow(*browser_wnd_, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
}

void CKeyWnd::CloseBrowserWnd() {
    if (browser_wnd_) {
        browser_wnd_->Close(IDCLOSE);
        browser_wnd_ = nullptr;
    }
}

LRESULT CKeyWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;
    switch (uMsg) {
    case WM_SETCURSOR: lRes = OnSetCursor(wParam, lParam, bHandled); break;
    case WM_KEYWND_MSG_REMOVE_KEY: lRes = OnRemoveKey(wParam, lParam, bHandled); break;
    default: bHandled = FALSE;
    }

    if (bHandled) {
        return lRes;
    }
    return __super::HandleMessage(uMsg, wParam, lParam);
}

bool CKeyWnd::OnClickBtnClose(void* lpParam) {
    scene_bak_info_ = scene_info_->cloner();
    SetBrowserMode(true);
    return true;
}

CControlUI* CKeyWnd::CreateHandleKey() {
    if (!key_body_) return nullptr;

    CDialogBuilder builder;
    auto handle_ctrl = (CRemoteHandleUI*)builder.Create(L"KeyMap/body/HandleCtrl.xml", (UINT)0, this, &m_pm);
    if (!handle_ctrl) return nullptr;
    key_body_->Add(handle_ctrl);

    handle_ctrl->SetKeyType(emulator::HANDLE_KEY);

    return handle_ctrl;
}

bool CKeyWnd::OnBtnToolHandle(void* param) {
    if (!key_body_ || !btn_tool_handle_) return true;
    if (btn_tool_handle_->GetTag() == 1) return true;

    auto handle_ctrl = (CRemoteHandleUI*)CreateHandleKey();
    if (!handle_ctrl) return true;

    auto edit_key_1 = handle_ctrl->edit_key_1();
    auto edit_key_2 = handle_ctrl->edit_key_2();
    auto edit_key_3 = handle_ctrl->edit_key_3();
    auto edit_key_4 = handle_ctrl->edit_key_4();

    if (!edit_key_1 || !edit_key_2 || !edit_key_3 || !edit_key_4) return true;

    CenterKey(handle_ctrl);
    btn_tool_handle_->SetTag(1);

    QRect rc = handle_ctrl->GetPos();
    QPoint point(rc.left, rc.top);
    if (!KeyToScreen(&point)) return true;

    emulator::ItemInfo item;
    item.itemType = emulator::HANDLE_KEY;
    item.nItemPosX = point.x;
    item.nItemPosY = point.y;
    item.nItemWidth = rc.GetWidth();
    item.nItemHeight = rc.GetHeight();

    emulator::KeyInfo info; 
    auto rc_edit = edit_key_1->GetPos();
    info.nValue = 87;
    info.strDescription = PublicLib::AToUtf("");
    info.strKeyString = PublicLib::AToUtf("W");
    info.nPointX = point.x + int(double(rc_edit.GetWidth()) / 2.0 + 0.5) + rc_edit.left - rc.left;;
    info.nPointY = point.y + int(double(rc_edit.GetHeight()) / 2.0 + 0.5) + rc_edit.top - rc.top;
    item.keys.push_back(info);

    rc_edit = edit_key_2->GetPos();
    info.nValue = 65;
    info.strDescription = PublicLib::AToUtf("");
    info.strKeyString = PublicLib::AToUtf("A");
    info.nPointX = point.x + int(double(rc_edit.GetWidth()) / 2.0 + 0.5) + rc_edit.left - rc.left;;
    info.nPointY = point.y + int(double(rc_edit.GetHeight()) / 2.0 + 0.5) + rc_edit.top - rc.top;
    item.keys.push_back(info);

    rc_edit = edit_key_3->GetPos();
    info.nValue = 83;
    info.strDescription = PublicLib::AToUtf("");
    info.strKeyString = PublicLib::AToUtf("S");
    info.nPointX = point.x + int(double(rc_edit.GetWidth()) / 2.0 + 0.5) + rc_edit.left - rc.left;;
    info.nPointY = point.y + int(double(rc_edit.GetHeight()) / 2.0 + 0.5) + rc_edit.top - rc.top;
    item.keys.push_back(info);

    rc_edit = edit_key_4->GetPos();
    info.nValue = 68;
    info.strDescription = PublicLib::AToUtf("");
    info.strKeyString = PublicLib::AToUtf("D");
    info.nPointX = point.x + int(double(rc_edit.GetWidth()) / 2.0 + 0.5) + rc_edit.left - rc.left;;
    info.nPointY = point.y + int(double(rc_edit.GetHeight()) / 2.0 + 0.5) + rc_edit.top - rc.top;
    item.keys.push_back(info);

    scene_bak_info_->AddItem(item);

    InitRemoteHandle(handle_ctrl, item);
    btn_tool_normal_->SetTag(0);

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

    normal_ctrl->SetKeyType(emulator::NORMAL_KEY);

    return normal_ctrl;
}

bool CKeyWnd::OnBtnToolNormal(void* param) {
    if (!key_body_ || !scene_info_ || !btn_tool_normal_) return true;
    if (btn_tool_normal_->GetTag() == 1) return true;

    auto normal_ctrl = (CNormalUI*)CreateNormalKey();
    if (!normal_ctrl) return true;

    CenterKey(normal_ctrl);
    KeyToScreen(normal_ctrl);
    btn_tool_normal_->SetTag(1);

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

bool CKeyWnd::KeyToScreen(CControlUI* control) {
    if (!control) return false;

    auto rc = control->GetPos();

    QPoint point(rc.left, rc.top);

    if (!KeyToScreen(&point)) return false;

    auto it = dynamic_cast<Ikey*>(control);

    if (it) it->SetScreenPosX(point.x);
    if (it) it->SetScreenPosY(point.y);

    return true;
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

    right_ctrl->SetKeyType(emulator::RIGHT_MOUSE_MOVE);

    return right_ctrl;
}

bool CKeyWnd::OnBtnToolRightRun(void* param) {
    if (!key_body_ || !opt_right_run_) return true;

    if (!opt_right_run_->IsSelected()) {
        auto right_ctrl = (CRightMouseMoveUI*)CreateRightMouse();
        if (!right_ctrl) return true;

        emulator::ItemInfo item;
        item.itemType = emulator::RIGHT_MOUSE_MOVE;
        item.nItemPosX = 159;
        item.nItemPosY = 539;
        item.nItemSlider = 24;
        item.nItemWidth = right_ctrl->GetFixedWidth();
        item.nItemHeight = right_ctrl->GetFixedHeight();

        emulator::KeyInfo info; 
        info.nValue = -9;
        info.strDescription = PublicLib::AToUtf("右键行走");
        info.strKeyString = PublicLib::AToUtf("鼠标右键");
        info.nPointX = 209;
        info.nPointY = 594;
        item.keys.push_back(info);

        info.nValue = 88;
        info.strDescription = PublicLib::AToUtf("中断行走");
        info.strKeyString = PublicLib::AToUtf("X");
        info.nPointX = 209;
        info.nPointY = 594;
        item.keys.push_back(info);

        // 首先要删除对应的key值
        if (scene_bak_info_->DeleteKey(info.nValue)) {
            auto key_string = CGlobalData::Instance()->GetKeyboardStr(info.nValue);
            auto normal_ctrl = key_body_->FindSubControl(key_string.c_str());
            if (normal_ctrl) key_body_->Remove(normal_ctrl);
        }

        scene_bak_info_->AddItem(item);

        InitRightMouse(right_ctrl, item);

        
    }
    else {
        for (int i = 0; i < key_body_->GetCount(); i++) {
            auto it = key_body_->GetItemAt(i);
            if (!it) continue;
            auto key = dynamic_cast<Ikey*>(it);
            if (!key) continue;

            if (key->KeyType() == emulator::RIGHT_MOUSE_MOVE) {
                scene_bak_info_->DeleteKey(emulator::RIGHT_MOUSE_MOVE, it->GetTag());
                key_body_->Remove(it);
                break;
            }
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

    intelligent_ctrl->SetKeyType(emulator::INTELLIGENT_CASTING_KEY);

    return intelligent_ctrl;
}

bool CKeyWnd::OnBtnToolIntelligent(void* param) {
    if (!key_body_ || !btn_tool_intelligent_) return true;
    if (btn_tool_intelligent_->GetTag() == 1) return true;

    auto intelligent_ctrl = (CIntelligentUI*)CreateIntelligent();
    if (!intelligent_ctrl) return true;

    CenterKey(intelligent_ctrl);
    KeyToScreen(intelligent_ctrl);
    btn_tool_intelligent_->SetTag(1);

    return true;
}

bool CKeyWnd::OnBtnSave(void* param) {
    if (!scene_info_ || !scene_bak_info_) return true;

    scene_info_ = scene_bak_info_->cloner();

    auto index = combox_keyboard_->GetCurSel();
    if (index < 0) return true;

    CListLabelElementUI* key_elem = (CListLabelElementUI*)combox_keyboard_->GetItemAt(index);
    if (!key_elem) return true;

    scene_info_->saveScene(PublicLib::UToUtf8(key_elem->GetUserData().GetData()).c_str());

    CIosMgr::Instance()->UpdateKeyMap(key_elem->GetUserData().GetData());

    SetBrowserMode(true);

    return true;
}

bool CKeyWnd::OnBtnDelete(void* param) {
    if (btn_tool_handle_) btn_tool_handle_->SetTag(0);
    if (btn_tool_normal_) btn_tool_normal_->SetTag(0);
    if (btn_tool_intelligent_) btn_tool_intelligent_->SetTag(0);
    scene_bak_info_ = scene_info_->cloner();
    LoadKeyItems();
    return true;
}

LRESULT CKeyWnd::OnSetCursor(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    bHandled = FALSE;
    return 0;
}

LRESULT CKeyWnd::OnRemoveKey(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    CControlUI* control = (CControlUI*)(wParam);
    if (!control) return 0;

    auto key = dynamic_cast<Ikey*>(control);
    if (!key || !scene_bak_info_ || !key_body_) return 0;

    if (!key->HasMapMemory()) {
        auto key_type = key->KeyType();
        switch (key_type) {
        case emulator::NORMAL_KEY:
            if (btn_tool_normal_) btn_tool_normal_->SetTag(0);
            break;
        case emulator::INTELLIGENT_CASTING_KEY:
            if (btn_tool_intelligent_) btn_tool_intelligent_->SetTag(0);
            break;
        case emulator::HANDLE_KEY:
            if (btn_tool_handle_) btn_tool_handle_->SetTag(0);
            break;
        default:
            break;
        }
    }
    else scene_bak_info_->DeleteKey(control->GetTag());

    key_body_->Remove(control);

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
    if (!pNotify || !pNotify->pSender || !scene_bak_info_ || !key_body_ || !btn_tool_normal_) return true;

    CKeyEditUI* edit_key = dynamic_cast<CKeyEditUI*>(pNotify->pSender);
    if (!edit_key) return true;

    if (edit_key->GetTag() == edit_key->GetKeyValue()) return true;

    wstring keyboard = edit_key->GetText().GetData();

    if (keyboard.empty()) return true;

    if (scene_bak_info_->DeleteKey(edit_key->GetKeyValue())) {
        auto key_string = CGlobalData::Instance()->GetKeyboardStr(edit_key->GetKeyValue());
        auto normal_ctrl = key_body_->FindSubControl(key_string.c_str());
        if (normal_ctrl) {
            auto key = dynamic_cast<Ikey*>(normal_ctrl);
            if (key && key->KeyType() == emulator::RIGHT_MOUSE_MOVE && opt_right_run_) opt_right_run_->Selected(false);
            key_body_->Remove(normal_ctrl);
        }
    }

    auto normal_ctrl = (CNormalUI*)edit_key->GetParent();
    if (!normal_ctrl) return true;

    if (!scene_bak_info_->set_key(emulator::NORMAL_KEY, edit_key->GetTag(), edit_key->GetKeyValue(), PublicLib::UToUtf8(keyboard))) {
        QRect rc = normal_ctrl->GetPos();
        QPoint point(rc.left, rc.top);
        if (!KeyToScreen(&point)) return true;

        emulator::ItemInfo item;
        item.itemType = emulator::NORMAL_KEY;
        item.nItemPosX = point.x;
        item.nItemPosY = point.y;
        item.nItemFingerCount = 1;
        item.nItemWidth = rc.GetWidth();
        item.nItemHeight = rc.GetHeight();

        emulator::KeyInfo info; 
        auto rc_edit = edit_key->GetPos();
        info.nValue = edit_key->GetKeyValue();
        info.strDescription = PublicLib::AToUtf("自定义技能");
        info.strKeyString = PublicLib::UToUtf8(keyboard);
        info.nPointX = point.x + int(double(rc_edit.GetWidth()) / 2.0 + 0.5);
        info.nPointY = point.y + int(double(rc_edit.GetHeight()) / 2.0 + 0.5);

        item.keys.push_back(info);
        scene_bak_info_->AddItem(item);

        InitNormalKey(normal_ctrl, item);
        btn_tool_normal_->SetTag(0);

        return true;
    }

    edit_key->SetTag(edit_key->GetKeyValue());
    normal_ctrl->SetName(keyboard.c_str());
    normal_ctrl->SetTag(edit_key->GetKeyValue());

    return true;
}

bool CKeyWnd::OnEditIntelligentChanged(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (!pNotify || !pNotify->pSender || !scene_bak_info_ || !key_body_ || !btn_tool_intelligent_) return true;

    CKeyEditUI* edit_key = dynamic_cast<CKeyEditUI*>(pNotify->pSender);
    if (!edit_key) return true;

    if (edit_key->GetTag() == edit_key->GetKeyValue()) return true;

    wstring keyboard = edit_key->GetText().GetData();

    if (keyboard.empty()) return true;

    if (scene_bak_info_->DeleteKey(edit_key->GetKeyValue())) {
        auto key_string = CGlobalData::Instance()->GetKeyboardStr(edit_key->GetKeyValue());
        auto normal_ctrl = key_body_->FindSubControl(key_string.c_str());
        if (normal_ctrl) {
            auto key = dynamic_cast<Ikey*>(normal_ctrl);
            if (key && key->KeyType() == emulator::RIGHT_MOUSE_MOVE && opt_right_run_) opt_right_run_->Selected(false);
            key_body_->Remove(normal_ctrl);
        }
    }

    auto intelligent = (CIntelligentUI*)edit_key->GetParent();
    if (!intelligent) return true;

    if (!scene_bak_info_->set_key(emulator::INTELLIGENT_CASTING_KEY, edit_key->GetTag(), edit_key->GetKeyValue(), PublicLib::UToUtf8(keyboard))) {
        QRect rc = intelligent->GetPos();
        QPoint point(rc.left, rc.top);
        if (!KeyToScreen(&point)) return true;

        emulator::ItemInfo item;
        item.itemType = emulator::INTELLIGENT_CASTING_KEY;
        item.nItemPosX = point.x;
        item.nItemPosY = point.y;
        item.nItemWidth = rc.GetWidth();
        item.nItemHeight = rc.GetHeight();
        item.nItemRightMoveStop = 0;
        item.nItemSlider = 250;

        emulator::KeyInfo info; 
        auto rc_edit = edit_key->GetPos();
        info.nValue = edit_key->GetKeyValue();
        info.strDescription = PublicLib::AToUtf("技能智能施法");
        info.strKeyString = PublicLib::UToUtf8(keyboard);
        info.nPointX = point.x + int(double(rc_edit.GetWidth()) / 2.0 + 0.5) + rc_edit.left - rc.left;
        info.nPointY = point.y + int(double(rc_edit.GetHeight()) / 2.0 + 0.5) + rc_edit.top - rc.top;

        item.keys.push_back(info);
        scene_bak_info_->AddItem(item);

        InitIntelligent(intelligent, item);
        btn_tool_intelligent_->SetTag(0);

        return true;
    }

    edit_key->SetTag(edit_key->GetKeyValue());
    intelligent->SetName(keyboard.c_str());
    intelligent->SetTag(edit_key->GetKeyValue());

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

    if (!scene_bak_info_->set_key(emulator::RIGHT_MOUSE_MOVE, edit_key->GetTag(), edit_key->GetKeyValue(), PublicLib::UToUtf8(keyboard))) {
        
    }

    edit_key->SetTag(edit_key->GetKeyValue());
    right_mouse->SetName(keyboard.c_str());
    right_mouse->SetTag(edit_key->GetKeyValue());

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

void CKeyWnd::InitRightMouse(CControlUI* control, const emulator::ItemInfo& item) {
    if (item.keys.size() < 2) return;
    if (!control || !scene_info_) return;
    auto right_ctrl = (CRightMouseMoveUI*)control;
    if (!right_ctrl) return;

    auto opacity = (browser_mode_ ? scene_info_->opacity() : 100);

    right_ctrl->SetName(PublicLib::Utf8ToU(item.keys[1].strKeyString).c_str());
    right_ctrl->SetTag(item.keys[1].nValue);
    right_ctrl->UpdateBrowserMode(browser_mode_, opacity);
    right_ctrl->SetScreenPosX(item.nItemPosX);
    right_ctrl->SetScreenPosY(item.nItemPosY);
    right_ctrl->SetKeyType(item.itemType);
    right_ctrl->SetHasMapMemory(true);

    auto edit_key = right_ctrl->edit_key();
    if (edit_key) {
        edit_key->SetTag(item.keys[1].nValue);
        edit_key->SetKeyValue(item.keys[1].nValue);
        edit_key->CControlUI::SetText(PublicLib::Utf8ToU(item.keys[1].strKeyString).c_str());
    }

    auto slider_mouse = right_ctrl->slider_mouse();
    if (slider_mouse) slider_mouse->SetValue(item.nItemSlider);
}

void CKeyWnd::InitNormalKey(CControlUI* control, const emulator::ItemInfo& item) {
    if (item.keys.size() < 1) return;
    if (!control || !scene_info_) return;
    auto normal_ctrl = (CNormalUI*)control;
    if (!normal_ctrl) return;

    auto opacity = (browser_mode_ ? scene_info_->opacity() : 100);

    normal_ctrl->SetName(PublicLib::Utf8ToU(item.keys[0].strKeyString).c_str());
    normal_ctrl->SetTag(item.keys[0].nValue);
    normal_ctrl->UpdateBrowserMode(browser_mode_, opacity);
    normal_ctrl->SetScreenPosX(item.nItemPosX);
    normal_ctrl->SetScreenPosY(item.nItemPosY);
    normal_ctrl->SetKeyType(item.itemType);
    normal_ctrl->SetHasMapMemory(true);

    auto edit_key = normal_ctrl->edit_key();

    if (edit_key) {
        edit_key->SetTag(item.keys[0].nValue);
        edit_key->SetKeyValue(item.keys[0].nValue);
        edit_key->CControlUI::SetText(PublicLib::Utf8ToU(item.keys[0].strKeyString).c_str());
    }
}

void CKeyWnd::InitIntelligent(CControlUI* control, const emulator::tagItemInfo& item) {
    if (item.keys.size() < 1) return;
    if (!control || !scene_info_) return;
    auto intelligent_ctrl = (CIntelligentUI*)control;
    if (!intelligent_ctrl) return;

    auto opacity = (browser_mode_ ? scene_info_->opacity() : 100);

    intelligent_ctrl->SetName(PublicLib::Utf8ToU(item.keys[0].strKeyString).c_str());
    intelligent_ctrl->SetTag(item.keys[0].nValue);
    intelligent_ctrl->UpdateBrowserMode(browser_mode_, opacity);
    intelligent_ctrl->SetScreenPosX(item.nItemPosX);
    intelligent_ctrl->SetScreenPosY(item.nItemPosY);
    intelligent_ctrl->SetKeyType(item.itemType);
    intelligent_ctrl->SetHasMapMemory(true);

    auto edit_key = intelligent_ctrl->edit_key();
    if (edit_key) {
        edit_key->SetTag(item.keys[0].nValue);
        edit_key->SetKeyValue(item.keys[0].nValue);
        edit_key->CControlUI::SetText(PublicLib::Utf8ToU(item.keys[0].strKeyString).c_str());
    }

    auto slider_mouse = intelligent_ctrl->slider_mouse();
    if (slider_mouse) slider_mouse->SetValue(item.nItemSlider);

    auto opt_switch = intelligent_ctrl->opt_switch();
    if (opt_switch) opt_switch->Selected(item.nItemRightMoveStop == 0);
}

void CKeyWnd::InitRemoteHandle(CControlUI* control, const emulator::tagItemInfo& item) {
    if (item.keys.size() < 4) return;
    if (!control || !scene_info_) return;
    auto handle_ctrl = (CRemoteHandleUI*)control;
    if (!handle_ctrl) return;

    auto opacity = (browser_mode_ ? scene_info_->opacity() : 100);

    handle_ctrl->SetName(PublicLib::Utf8ToU(item.keys[0].strKeyString).c_str());
    handle_ctrl->SetTag(item.keys[0].nValue);
    handle_ctrl->UpdateBrowserMode(browser_mode_, opacity);
    handle_ctrl->SetScreenPosX(item.nItemPosX);
    handle_ctrl->SetScreenPosY(item.nItemPosY);
    handle_ctrl->SetKeyType(item.itemType);
    handle_ctrl->SetHasMapMemory(true);

    auto edit_key = handle_ctrl->edit_key_1();
    if (edit_key) {
        edit_key->SetTag(item.keys[0].nValue);
        edit_key->SetKeyValue(item.keys[0].nValue);
        edit_key->CControlUI::SetText(PublicLib::Utf8ToU(item.keys[0].strKeyString).c_str());
    }

    edit_key = handle_ctrl->edit_key_2();
    if (edit_key) {
        edit_key->SetTag(item.keys[1].nValue);
        edit_key->SetKeyValue(item.keys[1].nValue);
        edit_key->CControlUI::SetText(PublicLib::Utf8ToU(item.keys[1].strKeyString).c_str());
    }

    edit_key = handle_ctrl->edit_key_3();
    if (edit_key) {
        edit_key->SetTag(item.keys[2].nValue);
        edit_key->SetKeyValue(item.keys[2].nValue);
        edit_key->CControlUI::SetText(PublicLib::Utf8ToU(item.keys[2].strKeyString).c_str());
    }

    edit_key = handle_ctrl->edit_key_4();
    if (edit_key) {
        edit_key->SetTag(item.keys[3].nValue);
        edit_key->SetKeyValue(item.keys[3].nValue);
        edit_key->CControlUI::SetText(PublicLib::Utf8ToU(item.keys[3].strKeyString).c_str());
    }
}

void CKeyWnd::LoadKeyItems() {
    if (!scene_info_ || !key_body_ || !opt_right_run_) return;

    key_body_->RemoveAll();
    opt_right_run_->Selected(false);

    auto items = scene_info_->GetKeyItems();
    auto it = items.begin();
    for (; it != items.end(); it++) {
        if (it->itemType == emulator::NORMAL_KEY) {
            if (it->keys.size() < 1) continue;

            auto normal_ctrl = (CNormalUI*)CreateNormalKey();
            if (!normal_ctrl) continue;

            InitNormalKey(normal_ctrl, *it);
        }
        else if (it->itemType == emulator::RIGHT_MOUSE_MOVE) {
            if (it->keys.size() < 2) continue;

            auto right_ctrl = (CRightMouseMoveUI*)CreateRightMouse();
            if (!right_ctrl) continue;

            InitRightMouse(right_ctrl, *it);

            opt_right_run_->Selected(true);
        }
        else if (it->itemType == emulator::INTELLIGENT_CASTING_KEY) {
            if (it->keys.size() < 1) continue;

            auto intelligent_ctrl = (CIntelligentUI*)CreateIntelligent();
            if (!intelligent_ctrl) continue;

            InitIntelligent(intelligent_ctrl, *it);
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

    if (!it->HasMapMemory()) return true;

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

    return true;
}

bool CKeyWnd::OnComboxKeyboard(void* param) {
    UpdateSceneInfo();

    LoadKeyItems();

    UpdateCtrls();

    return true;
}
