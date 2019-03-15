#ifndef _KEY_COM_WND_INCLUDE_H_
#define _KEY_COM_WND_INCLUDE_H_

#pragma once
#include "KeyWnd.h"

class CKeyComWnd 
    : public CWndBase {
public:
    CKeyComWnd();
    virtual ~CKeyComWnd();

public:
    void UpdateKeyCtrls(const std::vector<KeyMapInfo>& keymap_info);

protected:
	virtual LPCWSTR GetWndName() const override { return L"keycommap"; }
	virtual LPCWSTR GetXmlPath() const override;
    virtual void InitWindow() override;

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
    virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    bool OnClickSwitchAccount(void* param);
    bool OnClickPersonalSetting(void* param);

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE(list_combox_keymap_, CListUI, L"list_combox_keymap")
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
        //BIND_CTRL_CLICK(L"personal_switch_account", &CKeyComWnd::OnClickSwitchAccount)
        //BIND_CTRL_CLICK(L"personal_setting", &CKeyComWnd::OnClickPersonalSetting)
    END_BIND_CTRL()

protected:
    

private:
    CListUI* list_combox_keymap_;
};

#endif  // !#define (_KEY_COM_WND_INCLUDE_H_) 