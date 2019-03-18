#include "Download/TaskCallback.h"
#include "KeyWnd.h"
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 The Sun.AC Authors . All Rights Reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KEYCOM_ITEM_INCLUDE_H_
#define _KEYCOM_ITEM_INCLUDE_H_

class CKeyComItemUI
    : public CListContainerElementUI
    , public DuiLib::IDialogBuilderCallback {
public:
    CKeyComItemUI();
    virtual ~CKeyComItemUI();

    virtual void Init() override;
    virtual void DoEvent(TEventUI& event) override;
    virtual void SetPos(RECT rc) override;

    void UpdateKeyCtrl(const KeyMapInfo& info);

public:
    virtual void InitControl();
    virtual CControlUI* CreateControl(LPCTSTR pstrClass) override;

protected:

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE_PAGE(btn_combox_modify_, COptionUI, this, L"btn_combox_modify")
        DECLARE_CTRL_TYPE_PAGE(btn_combox_delete_, CButtonUI, this, L"btn_combox_delete")
        DECLARE_CTRL_TYPE_PAGE(edit_name_, CEditUI, this, L"edit_name")
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK_PAGE(L"btn_combox_delete", this, &CKeyComItemUI::OnClickComboxDelete)
        BIND_CTRL_CLICK_PAGE(L"btn_combox_modify", this, &CKeyComItemUI::OnClickComboxModify)
        BIND_CTRL_EVENT_PAGE(L"edit_name", this, DUI_MSGTYPE_WND_KILLFOCUS, &CKeyComItemUI::OnEditNameKillfocus)
    END_BIND_CTRL()

    bool OnClickComboxDelete(void* param);
    bool OnClickComboxModify(void* param);

    bool OnEditNameKillfocus(void* param);

private:
    CEditUI* edit_name_;
    COptionUI* btn_combox_modify_;
    CButtonUI* btn_combox_delete_;
};

#endif  // !#define (_KEYCOM_ITEM_INCLUDE_H_)  
