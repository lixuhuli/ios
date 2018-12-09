#ifndef _WND_TOAST_INCLUDE_H_
#define _WND_TOAST_INCLUDE_H_

#pragma once

class CWndToast:
	public CWndBase {
public:
	CWndToast(const wstring& strText);
	virtual ~CWndToast(void);

protected:
    enum { TIMER_ID_GRADUAL_CHANGE = 601 };
	virtual LPCWSTR GetWndName() const { return L"Toast"; }
	virtual LPCWSTR GetXmlPath() const;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	virtual void InitWindow();

protected:
    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE(m_pLabelText, CLabelUI, L"lbl_text")
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
    END_BIND_CTRL()

private:
	int	m_nAlpha;
	UINT m_uTimerElapse;
	wstring	m_strText;
	CLabelUI* m_pLabelText;
};

#endif  // !#define (_WND_TOAST_INCLUDE_H_)