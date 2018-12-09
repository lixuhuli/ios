#pragma once
#include "WebCtrlUI.h"

class CWebCtrlUI;
class CWndWeb :
	public CWndBase
{
public:
    CWndWeb();
	virtual ~CWndWeb();

public:
    CCefWebkitUI* web_ctrl();

protected:
	virtual LPCWSTR GetWndName() const { return L""; }
	virtual LPCWSTR GetXmlPath() const;
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void OnFinalMessage(HWND hWnd) override;
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE(web_ui_, CCefWebkitUI, L"web_ui")
	END_INIT_CTRL()

	BEGIN_BIND_CTRL()
	END_BIND_CTRL()

private:
    CCefWebkitUI* web_ui_;
};

