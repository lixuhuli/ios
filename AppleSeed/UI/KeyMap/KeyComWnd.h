#ifndef _KEY_COM_WND_INCLUDE_H_
#define _KEY_COM_WND_INCLUDE_H_

#pragma once
#include "KeyWnd.h"

class CKeyComWnd 
    : public CWndBase {
public:
    CKeyComWnd(const std::vector<KeyMapInfo>& info);
    virtual ~CKeyComWnd();

protected:
	virtual LPCWSTR GetWndName() const override { return L"keycommap"; }
	virtual LPCWSTR GetXmlPath() const override;
    virtual void InitWindow() override;

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
    virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    bool OnClickBtnCreateKeymap(void* param);
    bool OnListKeyMapSelected(void* param);

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE(list_combox_keymap_, CListUI, L"list_combox_keymap")
        DECLARE_CTRL_TYPE(btn_combox_create_, CButtonUI, L"btn_combox_create")
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK(L"btn_combox_create", &CKeyComWnd::OnClickBtnCreateKeymap);
        BIND_CTRL_EVENT(L"list_combox_keymap", DUI_MSGTYPE_ITEMSELECT, &CKeyComWnd::OnListKeyMapSelected)
    END_BIND_CTRL()

private:
    void UpdateKeyCtrls();

private:
    CListUI* list_combox_keymap_;
    CButtonUI* btn_combox_create_;

    std::vector<KeyMapInfo> keymap_info_;  // º¸≈Ã”≥…‰∫œºØ
};

#endif  // !#define (_KEY_COM_WND_INCLUDE_H_) 