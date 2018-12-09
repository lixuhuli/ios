#ifndef _WND_USER_ITEM_INCLUDE_H_
#define _WND_USER_ITEM_INCLUDE_H_

#pragma once
#include "WndWebBase.h"

class CWndUserItem
    : public CWndWebBase {
public:
    CWndUserItem(const std::wstring& url);
    virtual ~CWndUserItem();

public:

protected:
	virtual LPCWSTR GetWndName() const override { return L"用户条款"; }
	virtual LPCWSTR GetXmlPath() const override;
    virtual void InitWindow() override;

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE(web_, CWebUI, L"web_user_item")
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK(L"btn_close", &CWndUserItem::OnBtnClickClose);
    END_BIND_CTRL()


protected:
    bool OnBtnClickClose(void* param);

private:
    std::wstring url_;
};

#endif  // !#define (_WND_USER_ITEM_INCLUDE_H_) 