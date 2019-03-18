#include "StdAfx.h"
#include "KeyComItem.h"
#include "Ios\IosMgr.h"
#include "MsgDefine.h"

CKeyComItemUI::CKeyComItemUI()
 : edit_name_(nullptr)
 , btn_combox_modify_(nullptr)
 , btn_combox_delete_(nullptr) {
}

CKeyComItemUI::~CKeyComItemUI() {
}

void CKeyComItemUI::InitControl() {
    CDialogBuilder builder;
    CControlUI* control = builder.Create(L"KeyMap/KeyComItem.xml", 0, this, m_pManager);
    if (!control) this->RemoveAll();
    else this->Add(control);

    InitControls();
    BindControls();
}

void CKeyComItemUI::Init() {
    __super::Init();
}

void CKeyComItemUI::DoEvent(TEventUI& event) {
    __super::DoEvent(event);
}

void CKeyComItemUI::SetPos(RECT rc) {
    __super::SetPos(rc);
}

DuiLib::CControlUI* CKeyComItemUI::CreateControl(LPCTSTR pstrClass) {
    std::wstring control_name = pstrClass;
    return nullptr;
}

void CKeyComItemUI::UpdateKeyCtrl(const KeyMapInfo& info) {
    if (!edit_name_ || !btn_combox_modify_ || !btn_combox_delete_) return;

    edit_name_->SetText(info.name_.c_str());
    btn_combox_modify_->SetVisible(info.index_ > 0 ? true : false);
    btn_combox_delete_->SetVisible(info.index_ > 0 ? true : false);
}

bool CKeyComItemUI::OnClickComboxDelete(void* param) {
    if (!m_pManager) return true;
    auto key_wnd = CIosMgr::Instance()->GetKeyWndPtr();
    if (!key_wnd) return true;

    ::PostMessage(*key_wnd, WM_KEYWND_MSG_REMOVE_ITEM, GetIndex(), GetInheritableTag());
    ::PostMessage(m_pManager->GetPaintWindow(), WM_CLOSE, 0, 0);
    return true;
}

bool CKeyComItemUI::OnClickComboxModify(void* param) {
    if (!edit_name_ || !btn_combox_modify_) return true;

    if (!btn_combox_modify_->IsSelected()) {
        edit_name_->SetFocus();
        edit_name_->SetSel(0, -1);
        edit_name_->SetMouseEnabled(true);
        btn_combox_modify_->SetTag(1);
    }
    else {
        if (btn_combox_modify_->GetTag() == 0) {
            edit_name_->SetFocus();
            edit_name_->SetSel(0, -1);
            edit_name_->SetMouseEnabled(true);
            btn_combox_modify_->SetTag(1);
        }
        else {
            edit_name_->SetMouseEnabled(false);
            btn_combox_modify_->SetTag(0);
        }
    }

    return true;
}

bool CKeyComItemUI::OnEditNameKillfocus(void* param) {
    if (!edit_name_ || !m_pManager) return true;

    wstring old_name = GetName().GetData();
    wstring new_name = edit_name_->GetText().GetData();

    if (old_name != new_name) {
        auto key_wnd = CIosMgr::Instance()->GetKeyWndPtr();
        if (key_wnd) key_wnd->ChangeKeyMapInfo(GetIndex(), GetInheritableTag(), new_name);
        SetName(new_name.c_str());
    }

    HWND wnd = m_pManager->GetPaintWindow();

    QPoint pt;
    GetCursorPos(&pt);

    QRect rc;
    GetWindowRect(wnd, &rc);

    if (!::PtInRect(&rc, pt)) {
        ::PostMessage(wnd, WM_CLOSE, 0, 0);
    }

    return true;
}
