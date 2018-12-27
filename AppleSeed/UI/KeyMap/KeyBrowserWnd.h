#ifndef _KEY_BROWSER_WND_INCLUDE_H_
#define _KEY_BROWSER_WND_INCLUDE_H_

#pragma once

class CKeyBrowserWnd :
    public CWndBase {
public:
    CKeyBrowserWnd();
    virtual ~CKeyBrowserWnd();

public:
    CControlUI* GetRoot();

protected:
    virtual LPCWSTR GetWndName() const override { return L""; }
    virtual LPCWSTR GetXmlPath() const override;
    virtual LPCTSTR GetWindowClassName() const override;
    virtual CControlUI* CreateControl(LPCTSTR pstrClass);
    virtual void InitWindow();
    virtual void OnFinalMessage(HWND hWnd);
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);


    BEGIN_INIT_CTRL()
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
    END_BIND_CTRL()

private:

};

#endif  // !#define (_KEY_BROWSER_WND_INCLUDE_H_)  