#ifndef _WND_IOS_INCLUDE_H_
#define _WND_IOS_INCLUDE_H_

#pragma once

class CWndIos :
    public CWndBase {
public:
    CWndIos();
    virtual ~CWndIos();

public:
    virtual LRESULT OnMouseWheel(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
    void SetCaptionRect(RECT& rcCaption);

protected:
    virtual LPCWSTR GetWndName() const override { return L"果仁模拟器"; }
    virtual LPCWSTR GetXmlPath() const override;
    virtual LPCTSTR GetWindowClassName() const override;
    virtual CControlUI* CreateControl(LPCTSTR pstrClass);
    virtual void InitWindow();
    virtual void OnFinalMessage(HWND hWnd);
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual LRESULT OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMoving(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnRButtonDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnRButtonUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnDropFiles(WPARAM wParam, LPARAM lParam, BOOL& bHandled);


    BEGIN_INIT_CTRL()
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
    END_BIND_CTRL()


protected:
    void Exit(); // 程序退出唯一入口

private:

};

#endif  // !#define (_WND_IOS_INCLUDE_H_)  