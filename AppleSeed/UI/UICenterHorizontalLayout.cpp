#include "StdAfx.h"
#include "UICenterHorizontalLayout.h"

CCenterHorizontalLayoutUI::CCenterHorizontalLayoutUI() 
 : need_center_contrl_(nullptr)
 , left_contrl_(nullptr)
 , right_contrl_(nullptr){
}

CCenterHorizontalLayoutUI::~CCenterHorizontalLayoutUI(){
}

void CCenterHorizontalLayoutUI::Init() {
    __super::Init();
    left_contrl_ = GetItemAt(0);
    need_center_contrl_ = GetItemAt(1);
    right_contrl_ = GetItemAt(2);
}

LPCTSTR CCenterHorizontalLayoutUI::GetClass() const {
    return L"CenterHorizontalLayoutUI";
}

LPVOID CCenterHorizontalLayoutUI::GetInterface(LPCTSTR pstrName) {
    if( _tcscmp(pstrName, DUI_CTR_CENTER_HORIZONTALLAYOUT) == 0 ) return static_cast<CCenterHorizontalLayoutUI*>(this);
    return CContainerUI::GetInterface(pstrName);
}

void CCenterHorizontalLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
    __super::SetAttribute(pstrName, pstrValue);
}

void CCenterHorizontalLayoutUI::SetPos(RECT rc) {
    do {
        if (!left_contrl_ || !right_contrl_ || !need_center_contrl_) break;

        auto cxFixed = rc.right - rc.left;
        auto center_width = cxFixed - left_contrl_->GetMinWidth() - right_contrl_->GetMinWidth();

        if (center_width >= need_center_contrl_->GetMinWidth() && center_width <= need_center_contrl_->GetMaxWidth()) {
            need_center_contrl_->SetFixedWidth(center_width);
        }
        else if (center_width > need_center_contrl_->GetMaxWidth()) {
            need_center_contrl_->SetFixedWidth(need_center_contrl_->GetMaxWidth());
        }

    } while (false);

    __super::SetPos(rc);
}
