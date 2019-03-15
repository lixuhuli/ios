#include "StdAfx.h"
#include "KeyComItem.h"

CKeyComItemUI::CKeyComItemUI()
 : lbl_name_(nullptr)
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
    //switch (event.Type) {
    //case UIEVENT_TIMER:
    //    OnTimer(event.wParam);
    //    break;
    //default:
    //    break;
    //}
    __super::DoEvent(event);
}

void CKeyComItemUI::SetPos(RECT rc) {
    __super::SetPos(rc);
}

DuiLib::CControlUI* CKeyComItemUI::CreateControl(LPCTSTR pstrClass) {
    std::wstring control_name = pstrClass;
    return nullptr;
}

void CKeyComItemUI::UpdateCtrls(const KeyMapInfo& info) {
    if (!lbl_name_) return;

    lbl_name_->SetText(info.name_.c_str());
}
