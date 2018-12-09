#pragma once
#include "UICefWebkit.h"
#include "UIWeb.h"

class CWndWebBase :
	public CWndBase
{
public:
    CWndWebBase();
	virtual ~CWndWebBase();

protected:
	virtual LPCWSTR GetWndName() const { return L""; }
	virtual LPCWSTR GetXmlPath() const { return L""; }
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

    virtual LRESULT OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMoving(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BEGIN_INIT_CTRL()
	END_INIT_CTRL()

	BEGIN_BIND_CTRL()
	END_BIND_CTRL()

private:
    void UpdatePos(const QRect* lprc = nullptr);

protected:
    CWebUI *web_;
};

