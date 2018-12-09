#ifndef _WND_MSG_INCLUDE_H_
#define _WND_MSG_INCLUDE_H_

#pragma once

class CWndMsg :
	public CWndBase {
public:
	CWndMsg(const wstring& strTitle, const wstring& strText, DWORD dwType);
	virtual ~CWndMsg();

protected:
	virtual LPCWSTR GetWndName() const { return L"Ã· æµØ¥∞"; }
	virtual LPCWSTR GetXmlPath() const;
	virtual void InitWindow();

	bool OnClickBtnOK(void* lpParam);
    bool OnClickBtnCancel(void* lpParam);

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE(lbl_text_, CLabelUI, L"lbl_text")
        DECLARE_CTRL_TYPE(btn_ok_, CButtonUI, L"btn_ok")
        DECLARE_CTRL_TYPE(btn_cancel_, CButtonUI, L"btn_cancel")
        DECLARE_CTRL_TYPE(ctrl_division_, CControlUI, L"ctrl_division")
    END_INIT_CTRL()

	BEGIN_BIND_CTRL()
		BIND_CTRL_CLICK(L"btn_ok", &CWndMsg::OnClickBtnOK)
        BIND_CTRL_CLICK(L"btn_cancel", &CWndMsg::OnClickBtnCancel)
	END_BIND_CTRL()

private:
	wstring m_strText;
    CLabelUI* lbl_text_;

    CButtonUI* btn_ok_;
    CButtonUI* btn_cancel_;
    CControlUI* ctrl_division_;

    DWORD m_dwSHowStyle;
};

#endif  // !#define (_WND_MSG_INCLUDE_H_)