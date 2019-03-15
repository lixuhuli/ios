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

    void UpdateCtrls(const KeyMapInfo& info);

public:
    virtual void InitControl();
    virtual CControlUI* CreateControl(LPCTSTR pstrClass) override;


protected:

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE_PAGE(lbl_name_, CLabelUI, this, L"lbl_name")
        DECLARE_CTRL_TYPE_PAGE(btn_combox_modify_, CButtonUI, this, L"btn_combox_modify")
        DECLARE_CTRL_TYPE_PAGE(btn_combox_delete_, CButtonUI, this, L"btn_combox_delete")
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
    END_BIND_CTRL()

private:

private:
    CLabelUI* lbl_name_;
    CButtonUI* btn_combox_modify_;
    CButtonUI* btn_combox_delete_;
};

#endif  // !#define (_KEYCOM_ITEM_INCLUDE_H_)  
