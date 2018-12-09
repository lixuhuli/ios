#ifndef _WND_MAIN_INCLUDE_H_
#define _WND_MAIN_INCLUDE_H_

#pragma once

class CWndMain :
    public CWndBase {
public:
    CWndMain();
    virtual ~CWndMain();

protected:
    virtual LPCWSTR GetWndName() const override { return L"isoÄ£ÄâÆ÷"; }
    virtual LPCWSTR GetXmlPath() const override;
    virtual LPCTSTR GetWindowClassName() const override;
    virtual CControlUI* CreateControl(LPCTSTR pstrClass);
    virtual void InitWindow();
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual LRESULT OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMoving(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    bool OnClickBtnClose(void* lpParam);
    bool OnClickBtnKey(void* param);
    virtual void OnFinalMessage(HWND hWnd);


    BEGIN_INIT_CTRL()
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK(L"btn_close", &CWndMain::OnClickBtnClose)
        BIND_CTRL_CLICK(L"key_btn", &CWndMain::OnClickBtnKey)
    END_BIND_CTRL()

private:
    void UpdatePos(const QRect* lprc = nullptr);

    iIosKey* ios_key_;
};

#endif  // !#define (_WND_MAIN_INCLUDE_H_)  