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
#include "UrlDefine.h"
#include "KeyComWnd.h"

CKeyWnd::CKeyWnd(const string& key_id)
 : btn_tool_handle_(nullptr)
 , btn_tool_normal_(nullptr)
 , key_body_(nullptr)
 , key_id_(key_id)
 , panel_tools_(nullptr)
 , opt_right_run_(nullptr)
 , browser_mode_(true)
 , browser_wnd_(nullptr)
 , combox_keyboard_(nullptr)
 , combox_keymap_(nullptr)
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
    if (!combox_keymap_) return;

    int selected_index = combox_keymap_->GetTag();
    if (selected_index < 0) return;

    if (selected_index < 0 || selected_index >= (int)keymap_info_.size()) return;

    auto key_info = keymap_info_[selected_index];

    scene_info_ = new emulator::SceneInfo();
    wstring file_path = key_info.file_;
    if (!PathFileExists(file_path.c_str())) scene_info_->set_pack_id(key_id_);
    else {
        scene_info_->loadScene(PublicLib::UToA(file_path).c_str());
        if (scene_info_->pack_id().empty()) scene_info_->set_pack_id(key_id_);
    }

    scene_bak_info_ = scene_info_->cloner();

    CIosMgr::Instance()->UpdateKeyMap(key_info.file_);
}

void CKeyWnd::ReadCloudKeyboardToCombox() {
    if (!combox_keymap_) return;

    keymap_info_.clear();

    auto config = CIosMgr::Instance()->GetKeyMapDir() + L"\\conf.ini";

    wchar_t szValue[1024] = { 0 };
    DWORD dwLen = GetPrivateProfileString(L"setting", L"record", L"", szValue, 1024, config.c_str());
    if (dwLen <= 0) return;

    auto UpdateKeyboardCombox = [&](const std::vector<std::wstring>& _records) -> void {
        auto it = _records.begin();
        for (; it != _records.end(); it++) {
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

            memset(szValue, 0, sizeof(szValue));
            dwLen = GetPrivateProfileString(record.c_str(), L"default", L"", szValue, 1024, config.c_str());

            wstring key_default = szValue;
            if (key_default.empty()) key_default = PublicLib::Utf8ToU(key_id_) + L"_default";
            key_default =  CIosMgr::Instance()->GetKeyMapDir() + L"\\" + key_default;

            KeyMapInfo key_info = CreateKeyInfo(key_name, key_file, key_default);

            keymap_info_.push_back(key_info);
        }
    };

    wstring records = szValue;
    std::vector<std::wstring> vec_records; 
    PublicLib::SplitStringW(records, L",", vec_records);
    UpdateKeyboardCombox(vec_records);
    SelectKeyItem(keymap_info_.size() > 0 ? 0 : -1);


    auto defined_config = CIosMgr::Instance()->GetKeyMapDir() + L"\\conf.xml";

    PublicLib::CMarkupEx xml;
    if (!xml.Load(defined_config.c_str())) return;

    if (xml.FindElem(L"list")) {
        xml.IntoElem();
        while (xml.FindElem(L"record")) {
            wstring key_name = xml.GetAttrib(L"name");
            wstring key_file = xml.GetAttrib(L"file");
            wstring key_default = xml.GetAttrib(L"default");
            int key_tag = _wtoi(xml.GetAttrib(L"tag").c_str());
            int key_index = _wtoi(xml.GetAttrib(L"index").c_str());

            KeyMapInfo key_info = CreateKeyInfo(key_name, key_file, key_default, key_tag, key_index);

            keymap_info_.push_back(key_info);
        }

        xml.OutOfElem();
    }
}

void CKeyWnd::SelectKeyItem(int index) {
    if (index < 0 || index >= (int)keymap_info_.size()) {
        combox_keymap_->SetTag(-1);
        combox_keymap_->SetText(L"");
    }
    else {
        auto key_info = keymap_info_[index];
        combox_keymap_->SetTag(index);
        combox_keymap_->SetText(key_info.name_.c_str());
    }
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
    rc.top -= 60;
    rc.bottom += 60;

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
    case WM_KEYWND_MSG_REMOVE_ITEM: lRes = OnRemoveItem(wParam, lParam, bHandled); break;
    default: bHandled = FALSE;
    }

    if (bHandled) {
        return lRes;
    }
    return __super::HandleMessage(uMsg, wParam, lParam);
}

bool CKeyWnd::OnClickBtnLocal(void* lpParam) {
    auto key_dir = CIosMgr::Instance()->GetKeyMapDir();
    ::ShellExecute(nullptr, L"open", key_dir.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    return true;
}

bool CKeyWnd::OnClickBtnHelp(void* lpParam) {
    ShellExecute(NULL, L"open", URL_KEYMAP_USING_METHOD, NULL, NULL, SW_SHOW);
    return true;
}

bool CKeyWnd::OnClickBtnClose(void* lpParam) {
    if (scene_info_) scene_bak_info_ = scene_info_->cloner();
    SetBrowserMode(true);
    return true;
}

CControlUI* CKeyWnd::CreateHandleKey() {
    if (!key_body_) return nullptr;

    CDialogBuilder builder;
    auto handle_ctrl = (CRemoteHandleUI*)builder.Create(L"KeyMap/body/HandleCtrl.xml", (UINT)0, this, &m_pm);
    if (!handle_ctrl) return nullptr;
    key_body_->Add(handle_ctrl);

    auto edit_key = handle_ctrl->edit_key_1();
    if (edit_key) edit_key->Subscribe(DUI_MSGTYPE_CHARCHANGED, MakeDelegate(this, &CKeyWnd::OnEditHandleCtrlChanged));

    edit_key = handle_ctrl->edit_key_2();
    if (edit_key) edit_key->Subscribe(DUI_MSGTYPE_CHARCHANGED, MakeDelegate(this, &CKeyWnd::OnEditHandleCtrlChanged));

    edit_key = handle_ctrl->edit_key_3();
    if (edit_key) edit_key->Subscribe(DUI_MSGTYPE_CHARCHANGED, MakeDelegate(this, &CKeyWnd::OnEditHandleCtrlChanged));

    edit_key = handle_ctrl->edit_key_4();
    if (edit_key) edit_key->Subscribe(DUI_MSGTYPE_CHARCHANGED, MakeDelegate(this, &CKeyWnd::OnEditHandleCtrlChanged));

    handle_ctrl->Subscribe(DUI_MSGTYPE_POS_CHANGED, MakeDelegate(this, &CKeyWnd::OnKeyPosChanged));

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

    auto DeleteKey = [&](int key_value) -> void {
        if (scene_bak_info_->DeleteKey(key_value)) {
            auto normal_ctrl = FindKeyUI(key_value);
            if (normal_ctrl) {
                auto key = dynamic_cast<Ikey*>(normal_ctrl);
                if (key && key->KeyType() == emulator::RIGHT_MOUSE_MOVE && opt_right_run_) opt_right_run_->Selected(false);
                key_body_->Remove(normal_ctrl);
            }
        }
    };

    DeleteKey(87);
    DeleteKey(65);
    DeleteKey(83);
    DeleteKey(68);

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
    btn_tool_handle_->SetTag(0);

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
            auto normal_ctrl = FindKeyUI(info.nValue);
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
    if (!scene_info_ || !scene_bak_info_ || !combox_keymap_) return true;

    scene_info_ = scene_bak_info_->cloner();

    auto index = combox_keymap_->GetTag();
    if (index < 0 || index >= (int)keymap_info_.size()) return true;

    auto key_info = keymap_info_[index];

    scene_info_->saveScene(PublicLib::UToA(key_info.file_).c_str());

    CIosMgr::Instance()->UpdateKeyMap(key_info.file_.c_str());

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

bool CKeyWnd::OnBtnRestore(void* param) {
    if (!combox_keymap_) return true;

    int index = combox_keymap_->GetTag();

    if (index < 0 || index >= (int)keymap_info_.size()) return true;

    auto key_info = keymap_info_[index];

    scene_info_ = new emulator::SceneInfo();
    wstring file_path = key_info.default_;
    if (!PathFileExists(file_path.c_str())) scene_info_->set_pack_id(key_id_);
    else {
        scene_info_->loadScene(PublicLib::UToA(file_path).c_str());
        if (scene_info_->pack_id().empty()) scene_info_->set_pack_id(key_id_);
    }

    scene_bak_info_ = scene_info_->cloner();

    scene_info_->saveScene(PublicLib::UToA(key_info.file_).c_str());

    CIosMgr::Instance()->UpdateKeyMap(key_info.file_);

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

LRESULT CKeyWnd::OnRemoveItem(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    if (!combox_keyboard_) return 0;

    int index = lParam;
    if (index < 0) return 0;

    CListLabelElementUI* key_elem = (CListLabelElementUI*)combox_keyboard_->GetItemAt(index);
    if (!key_elem) return true;

    auto config = CIosMgr::Instance()->GetKeyMapDir() + L"\\conf.ini";

    WCHAR szDefined[MAX_PATH] = { 0 };
    wsprintf(szDefined, L"defined_%d", key_elem->GetInheritableTag());

    wstring strDefined = szDefined;

    wchar_t szValue[1024] = { 0 };
    DWORD dwLen = GetPrivateProfileString(L"setting", L"defined", L"", szValue, 1024, config.c_str());

    wstring defineds = szValue;

    auto pos = defineds.find(strDefined.c_str());

    defineds = PublicLib::StrReplaceW(defineds, strDefined, L"");

    if (pos != wstring::npos) {
        if (pos == 0) {
            if (defineds.length() > 0) defineds.erase(0, 1);
        }
        else {
            defineds.erase(pos - 1, 1);
        }

        WritePrivateProfileString(L"setting", L"defined", defineds.c_str(), config.c_str());
        WritePrivateProfileString(strDefined.c_str(), L"name", L"", config.c_str());
        WritePrivateProfileString(strDefined.c_str(), L"file", L"", config.c_str());
        WritePrivateProfileString(strDefined.c_str(), L"default", L"", config.c_str());
        WritePrivateProfileString(strDefined.c_str(), L"tag", L"", config.c_str());
        WritePrivateProfileString(strDefined.c_str(), L"index", L"", config.c_str());
    }

    ::DeleteFile(key_elem->GetUserData().GetData());
    ::DeleteFile(key_elem->GetInheritableUserData().GetData());

    if (index > 1) combox_keyboard_->SelectItem(index - 1);

    combox_keyboard_->RemoveAt(index);

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
        auto normal_ctrl = FindKeyUI(edit_key->GetKeyValue());
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
        auto normal_ctrl = FindKeyUI(edit_key->GetKeyValue());
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

        info.nValue = -5;
        info.strDescription = PublicLib::AToUtf("中断施法");
        info.strKeyString = PublicLib::AToUtf("-5");
        info.nPointX = 1205;
        info.nPointY = 200;
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

bool CKeyWnd::OnEditHandleCtrlChanged(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (!pNotify || !pNotify->pSender || !scene_bak_info_ || !key_body_ || !btn_tool_handle_) return true;

    CKeyEditUI* edit_key = dynamic_cast<CKeyEditUI*>(pNotify->pSender);
    if (!edit_key) return true;

    if (edit_key->GetTag() == edit_key->GetKeyValue()) return true;

    wstring keyboard = edit_key->GetText().GetData();

    if (keyboard.empty()) return true;

    auto handle_ctrl = (CRemoteHandleUI*)edit_key->GetParent();
    if (!handle_ctrl) return true;

    auto edit_key_1 = handle_ctrl->edit_key_1();
    auto edit_key_2 = handle_ctrl->edit_key_2();
    auto edit_key_3 = handle_ctrl->edit_key_3();
    auto edit_key_4 = handle_ctrl->edit_key_4();

    auto CheckEditKey = [&](CKeyEditUI* edit_key_x, int key_value, const std::string& key_string) -> bool {
        if (!edit_key_x) return false;

        if (edit_key->GetTag() == edit_key_x->GetTag()) return false;

        if (edit_key->GetKeyValue() == edit_key_x->GetKeyValue()) {
            scene_bak_info_->set_key(emulator::HANDLE_KEY, edit_key_x->GetKeyValue(), key_value, key_string);

            edit_key_x->SetTag(key_value);
            edit_key_x->SetKeyValue(key_value);
            edit_key_x->CControlUI::SetText(L"");

            scene_bak_info_->set_key(emulator::HANDLE_KEY, edit_key->GetTag(), edit_key->GetKeyValue(), PublicLib::UToUtf8(keyboard));
            edit_key->SetTag(edit_key->GetKeyValue());
            edit_key->CControlUI::SetText(keyboard.c_str());

            if (edit_key->GetTag() == edit_key_1->GetTag()) {
                handle_ctrl->SetName(keyboard.c_str());
                handle_ctrl->SetTag(edit_key->GetKeyValue());
            }
            
            return true;
        }

        return false;
    };

    if (CheckEditKey(edit_key_1, -1, "-1")) return true;
    if (CheckEditKey(edit_key_2, -2, "-2")) return true;
    if (CheckEditKey(edit_key_3, -3, "-3")) return true;
    if (CheckEditKey(edit_key_4, -4, "-4")) return true;

    if (scene_bak_info_->DeleteKey(edit_key->GetKeyValue())) {
        auto normal_ctrl = FindKeyUI(edit_key->GetKeyValue());
        if (normal_ctrl) {
            auto key = dynamic_cast<Ikey*>(normal_ctrl);
            if (key && key->KeyType() == emulator::RIGHT_MOUSE_MOVE && opt_right_run_) opt_right_run_->Selected(false);
            key_body_->Remove(normal_ctrl);
        }
    }        

    scene_bak_info_->set_key(emulator::HANDLE_KEY, edit_key->GetTag(), edit_key->GetKeyValue(), PublicLib::UToUtf8(keyboard));

    edit_key->SetTag(edit_key->GetKeyValue());

    if (edit_key->GetTag() == edit_key_1->GetTag()) {
        handle_ctrl->SetName(keyboard.c_str());
        handle_ctrl->SetTag(edit_key->GetKeyValue());
    }

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
        auto normal_ctrl = FindKeyUI(edit_key->GetKeyValue());
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
    handle_ctrl->SetFixedWidth(item.nItemWidth);
    handle_ctrl->SetFixedHeight(item.nItemHeight);
    handle_ctrl->SetHasMapMemory(true);

    auto edit_key = handle_ctrl->edit_key_1();
    if (edit_key) {
        edit_key->SetTag(item.keys[0].nValue);
        edit_key->SetKeyValue(item.keys[0].nValue);
        if (item.keys[0].nValue > 0) edit_key->CControlUI::SetText(PublicLib::Utf8ToU(item.keys[0].strKeyString).c_str());
    }

    edit_key = handle_ctrl->edit_key_2();
    if (edit_key) {
        edit_key->SetTag(item.keys[1].nValue);
        edit_key->SetKeyValue(item.keys[1].nValue);
        if (item.keys[1].nValue > 0) edit_key->CControlUI::SetText(PublicLib::Utf8ToU(item.keys[1].strKeyString).c_str());
    }

    edit_key = handle_ctrl->edit_key_3();
    if (edit_key) {
        edit_key->SetTag(item.keys[2].nValue);
        edit_key->SetKeyValue(item.keys[2].nValue);
        if (item.keys[2].nValue > 0) edit_key->CControlUI::SetText(PublicLib::Utf8ToU(item.keys[2].strKeyString).c_str());
    }

    edit_key = handle_ctrl->edit_key_4();
    if (edit_key) {
        edit_key->SetTag(item.keys[3].nValue);
        edit_key->SetKeyValue(item.keys[3].nValue);
        if (item.keys[3].nValue > 0) edit_key->CControlUI::SetText(PublicLib::Utf8ToU(item.keys[3].strKeyString).c_str());
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
        else if (it->itemType == emulator::HANDLE_KEY) {
            if (it->keys.size() < 4) continue;

            auto handle_ctrl = (CRemoteHandleUI*)CreateHandleKey();
            if (!handle_ctrl) continue;

            InitRemoteHandle(handle_ctrl, *it);
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

    auto rc = control->GetPos();
    QPoint point(rc.left, rc.top);
    if (!KeyToScreen(&point)) return true;

    if (it) it->SetScreenPosX(point.x);
    if (it) it->SetScreenPosY(point.y);

    if (!it->HasMapMemory()) return true;

    switch (it->KeyType()) {
    case emulator::NORMAL_KEY: {
        auto normal_ctrl = (CNormalUI*)control;
        if (!normal_ctrl) break;
        auto edit_key = normal_ctrl->edit_key();
        if (!edit_key) break;

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

        auto rc_edit = edit_key->GetPos();

        point.x = point.x + int(double(rc_edit.GetWidth()) / 2.0 + 0.5) + rc_edit.left - rc.left;
        point.y = point.y + int(double(rc_edit.GetHeight()) / 2.0 + 0.5) + rc_edit.top - rc.top;

        scene_bak_info_->set_key_pos(edit_key->GetKeyValue(), it->ScreenPosX(), it->ScreenPosY(), point);
        scene_bak_info_->set_key_pos(-9, point);
    }
    break;
    case emulator::HANDLE_KEY: {
        auto handle_ctrl = (CRemoteHandleUI*)control;
        if (!handle_ctrl) break;

        auto SetKeyPos = [&](CKeyEditUI* edit_key, QPoint pt) -> void {
            if (!edit_key) return;

            auto rc_edit = edit_key->GetPos();

            pt.x = pt.x + int(double(rc_edit.GetWidth()) / 2.0 + 0.5) + rc_edit.left - rc.left;
            pt.y = pt.y + int(double(rc_edit.GetHeight()) / 2.0 + 0.5) + rc_edit.top - rc.top;

            scene_bak_info_->set_key_pos(edit_key->GetKeyValue(), it->ScreenPosX(), it->ScreenPosY(), pt);
        };

        auto edit_key = handle_ctrl->edit_key_1();
        SetKeyPos(edit_key, point);

        edit_key = handle_ctrl->edit_key_2();
        SetKeyPos(edit_key, point);

        edit_key = handle_ctrl->edit_key_3();
        SetKeyPos(edit_key, point);

        edit_key = handle_ctrl->edit_key_4();
        SetKeyPos(edit_key, point);

        scene_bak_info_->set_item_width(edit_key->GetKeyValue(), rc.GetWidth());
        scene_bak_info_->set_item_height(edit_key->GetKeyValue(), rc.GetHeight());
    }
    break;
    default:
        break;
    }

    return true;
}

bool CKeyWnd::OnComboxKeyboardSelected(void* param) {
    if (!combox_keyboard_ || combox_keyboard_->GetCount() < 2) return true;

    if (combox_keyboard_->GetCurSel() == 0) {
        int nNum = GetUserDefinedNum();
        if (nNum < 1) {
            combox_keyboard_->SelectItem(1);
            return true;
        }

        if (!AddDefinedKeyBoard(key_id_, nNum)) {
            combox_keyboard_->SelectItem(1);
            return true;
        }

        combox_keyboard_->SelectItem(combox_keyboard_->GetCount() - 1);

        return true;
    }

    UpdateSceneInfo();

    LoadKeyItems();

    UpdateCtrls();

    return true;
}

bool CKeyWnd::OnComboxKeyboardClick(void* param) {
    TNotifyUI* notify = (TNotifyUI*)param;
    if (!notify || !notify->pSender || !combox_keyboard_) return true;

    auto control = notify->pSender;
    QRect del_pos = control->GetPos();

    del_pos.left += 165;
    del_pos.top += 8;
    del_pos.right = del_pos.left + 20;
    del_pos.bottom = del_pos.top + 20;

    if (::PtInRect(&del_pos, notify->ptMouse)) {
        int index = combox_keyboard_->GetItemIndex(control);
        ::PostMessage(m_hWnd, WM_KEYWND_MSG_REMOVE_ITEM, (LPARAM)((CControlUI*)control), (LPARAM)index);
    }

    return true;
}

bool CKeyWnd::OnClickBtnComKeyMap(void* param) {
    if (!combox_keymap_ || !browser_wnd_) return true;

    CKeyComWnd* key_com_wnd = new CKeyComWnd();
    if (!key_com_wnd) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"malloc key combox window failed, nullptr!");
        return true;
    }

    const RECT& rc = combox_keymap_->GetPos();
    POINT pt = { rc.left, rc.bottom };
    ClientToScreen(*browser_wnd_, &pt);

    key_com_wnd->Create(*browser_wnd_, false);

    auto height = (keymap_info_.size() + 1) * 35;

    ::MoveWindow(*key_com_wnd, pt.x, pt.y, 200, height, FALSE);

    key_com_wnd->UpdateKeyCtrls(keymap_info_);

    key_com_wnd->ShowWindow(true);

    return true;
}

CControlUI* CKeyWnd::FindKeyUI(int key_value) {
    if (!key_body_) return nullptr;

    auto IsKeyEdit = [&](CKeyEditUI* edit_key_x) -> bool {
        if (!edit_key_x) return false;

        if (edit_key_x->GetKeyValue() == key_value) return true;

        return false;
    };

    for (int i = 0; i < key_body_->GetCount(); i++) {
        auto normal_ctrl = key_body_->GetItemAt(i);
        if (!normal_ctrl) continue;

        auto key = dynamic_cast<Ikey*>(normal_ctrl);

        if (key && key->KeyType() == emulator::HANDLE_KEY) {
            auto handle_ctrl = (CRemoteHandleUI*)normal_ctrl;
            if (!handle_ctrl) continue;

            auto edit_key_1 = handle_ctrl->edit_key_1();
            auto edit_key_2 = handle_ctrl->edit_key_2();
            auto edit_key_3 = handle_ctrl->edit_key_3();
            auto edit_key_4 = handle_ctrl->edit_key_4();

            if (IsKeyEdit(edit_key_1)) return handle_ctrl;
            if (IsKeyEdit(edit_key_2)) return handle_ctrl;
            if (IsKeyEdit(edit_key_3)) return handle_ctrl;
            if (IsKeyEdit(edit_key_4)) return handle_ctrl;
        }
        else {
            if (key_value == normal_ctrl->GetTag()) return normal_ctrl;
        }
    }

    return nullptr;
}

KeyMapInfo CKeyWnd::CreateKeyInfo(const wstring& key_name, const wstring& key_file, const wstring& key_default, int tag/* = 0*/, int index/* = 0*/) {
    KeyMapInfo key_info;
    key_info.name_ = key_name;
    key_info.file_ = key_file;
    key_info.default_ = key_default;
    key_info.tag_ = tag;
    key_info.index_ = index;
    return key_info;
}

void CKeyWnd::CreateDefinedKeyFile() {
    PublicLib::CMarkupEx xml;
    xml.SetDoc(L"<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    xml.AddElem(L"list");
    xml.IntoElem();
    xml.OutOfElem();
    wstring config = CIosMgr::Instance()->GetKeyMapDir() + L"\\conf.xml";
    xml.Save(config);
}

bool CKeyWnd::AddDefinedKeyBoard(const string& app_id, int nNum) {
    if (!combox_keymap_) return false;

    auto config = CIosMgr::Instance()->GetKeyMapDir() + L"\\conf.xml";

    if (!::PathFileExists(config.c_str())) CreateDefinedKeyFile();

    PublicLib::CMarkupEx xml;
    if (!xml.Load(config.c_str())) {
        OUTPUT_XYLOG(LEVEL_INFO, L"打开自定义键盘映射配置文件失败");
        return false;
    }

    if (!xml.FindElem(L"list")) return false;

    xml.IntoElem();
    std::map<int, int> mapIndexs;
    while (xml.FindElem(L"record")) {
        int temp_index = _wtoi(xml.GetAttrib(L"index").c_str());
        mapIndexs[temp_index] = 0;
    }

    int index = 1;
    for (auto it = mapIndexs.begin(); it != mapIndexs.end(); ) {
        if (it->first != index) break;

        it++;
        index++;
    }

    if (index > USER_DEFINED_KEYMAP_COUNT) {
        xml.OutOfElem();
        return false;
    }

    char szDefined[MAX_PATH] = { 0 };
    sprintf(szDefined, "defined_%d", index);

    string strDefined = szDefined;

    wstring key_file = PublicLib::Utf8ToU(app_id + "_" + strDefined);
    wstring key_default = key_file + L"_default";

    WCHAR szName[MAX_PATH] = { 0 };
    wsprintf(szName, index == 1 ? L"我的配置（本地）" : L"我的配置（本地%d）", index);

    wstring key_name = szName;

    KeyMapInfo key_info = CreateKeyInfo(key_name, CIosMgr::Instance()->GetKeyMapDir() + L"\\" + key_file, CIosMgr::Instance()->GetKeyMapDir() + L"\\" + key_default, nNum, index);

    keymap_info_.push_back(key_info);

    xml.AddElem(L"record");
    xml.SetAttrib(L"name", key_name);
    xml.SetAttrib(L"file_", key_file);
    xml.SetAttrib(L"default", key_default);
    xml.SetAttrib(L"tag", nNum);
    xml.SetAttrib(L"index", index);

    xml.OutOfElem();

    return true;
}

int CKeyWnd::GetUserDefinedNum() {
    if (!combox_keyboard_ || combox_keyboard_->GetCount() < 2) return -1;

    std::map<int, int> mapNums;
    for (int i = 0; i < combox_keyboard_->GetCount(); i++) {
        CListLabelElementUI* key_elem = (CListLabelElementUI*)combox_keyboard_->GetItemAt(i);
        if (!key_elem) continue;
        if (key_elem->GetTag() == 0) continue;

        mapNums[key_elem->GetTag()] = 0;
    }

    if (mapNums.empty()) return 1;

    int nNum = 1;
    auto it = mapNums.begin();
    while (it != mapNums.end()) {
        if (it->first != nNum) return nNum;

        it++;
        nNum++;
    }

    return nNum;
}

int CKeyWnd::GetUserDefinedIndex(const wstring& defineds) {
    if (defineds.empty()) return 1;

    std::vector<std::wstring> vec_defineds; 
    PublicLib::SplitStringW(defineds, L",", vec_defineds);

    std::map<int, int> mapIndexs;
    for (auto it = vec_defineds.begin(); it != vec_defineds.end(); it++) {
        wstring defined = *it;

        auto pos = defined.rfind(L"_");
        if (pos == wstring::npos) continue;

        auto temp_index = _ttoi(defined.substr(pos + 1, defined.length() - pos - 1).c_str());

        mapIndexs[temp_index] = 0;
    }

    int index = 1;
    for (auto it = mapIndexs.begin(); it != mapIndexs.end(); ) {
        if (it->first != index) return index;

        it++;
        index++;
    }

    return index;
}

