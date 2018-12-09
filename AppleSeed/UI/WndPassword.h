#ifndef _WND_PASSWORD_INCLUDE_H_
#define _WND_PASSWORD_INCLUDE_H_

#pragma once

class CWndPassword
    : public CWndBase {
public:
    CWndPassword();
    virtual ~CWndPassword();

public:

protected:
	virtual LPCWSTR GetWndName() const override { return L"Íü¼ÇÃÜÂë"; }
	virtual LPCWSTR GetXmlPath() const override;
    virtual void InitWindow() override;

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
    virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE(btn_find_password_, CButtonUI, L"btn_find_password")
        DECLARE_CTRL_TYPE(btn_message_login_, CButtonUI, L"btn_message_login")
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK(L"btn_find_password", &CWndPassword::OnClickFindPassword)
        BIND_CTRL_CLICK(L"btn_message_login", &CWndPassword::OnClickMessageLogin)
    END_BIND_CTRL()

protected:
    bool OnClickFindPassword(void* param);
    bool OnClickMessageLogin(void* param);

private:
    CButtonUI* btn_find_password_;
    CButtonUI* btn_message_login_;
};

#endif  // !#define (_WND_PASSWORD_INCLUDE_H_) 