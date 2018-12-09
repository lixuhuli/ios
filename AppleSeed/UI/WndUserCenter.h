#ifndef _WND_USER_CENTER_INCLUDE_H_
#define _WND_USER_CENTER_INCLUDE_H_

#pragma once
#include "cef_bind.h"
//#include "uilib_util.h"

class CWndUserCenter
    : public base::SupportsWeakPtr<CWndUserCenter>
    , public CWndBase {
public:
    CWndUserCenter();
    virtual ~CWndUserCenter();

public:

protected:
	virtual LPCWSTR GetWndName() const override { return L"用户中心"; }
	virtual LPCWSTR GetXmlPath() const override;
    virtual void InitWindow() override;

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
    virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    bool OnClickSwitchAccount(void* param);
    bool OnClickPersonalSetting(void* param);

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE(personal_switch_account_, CButtonUI, L"personal_switch_account")
        DECLARE_CTRL_TYPE(personal_setting_, CButtonUI, L"personal_setting")
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK(L"personal_switch_account", &CWndUserCenter::OnClickSwitchAccount)
        BIND_CTRL_CLICK(L"personal_setting", &CWndUserCenter::OnClickPersonalSetting)
    END_BIND_CTRL()

protected:
    

private:
    CButtonUI* personal_setting_;
    CButtonUI* personal_switch_account_;
};

#endif  // !#define (_WND_USER_CENTER_INCLUDE_H_) 