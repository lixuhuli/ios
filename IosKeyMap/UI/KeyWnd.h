#ifndef _KEY_WND_INCLUDE_H_
#define _KEY_WND_INCLUDE_H_

#pragma once
#include "dragdrophandle.h"

class CMDLDropSource : public CDropSource {
public:
    CMDLDropSource() {}
};

class CMDLDropTarget : public CDropTarget {
public:
    CMDLDropTarget() { ; }

    // from CDropTarget
    STDMETHOD(Drop)(LPDATAOBJECT pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
};

class CKeyWnd :
    public CWndBase {
public:
    CKeyWnd();
    virtual ~CKeyWnd();

protected:
    virtual LPCWSTR GetWndName() const override { return L"宇宙第一锤子模拟器"; }
    virtual LPCWSTR GetXmlPath() const override;
    virtual LPCTSTR GetWindowClassName() const override;
    virtual CControlUI* CreateControl(LPCTSTR pstrClass);
    virtual void InitWindow();
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

    virtual LRESULT OnSetCursor(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    bool OnClickBtnClose(void* lpParam);
    bool OnBtnToolHandle(void* param);
    bool OnBtnToolNormal(void* param);
    bool OnBtnToolRightRun(void* param);
    virtual void OnFinalMessage(HWND hWnd);


    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE(btn_tool_handle_, CButtonUI, L"btn_tool_handle")
        DECLARE_CTRL_TYPE(btn_tool_normal_, CButtonUI, L"btn_tool_normal")
        DECLARE_CTRL_TYPE(key_body_, CVerticalLayoutUI, L"key_body")
        DECLARE_CTRL_TYPE(opt_right_run_, COptionUI, L"opt_right_run")
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK(L"btn_close", &CKeyWnd::OnClickBtnClose)
        BIND_CTRL_CLICK(L"btn_tool_handle", &CKeyWnd::OnBtnToolHandle)
        BIND_CTRL_CLICK(L"btn_tool_normal", &CKeyWnd::OnBtnToolNormal)
        BIND_CTRL_CLICK(L"opt_right_run", &CKeyWnd::OnBtnToolRightRun)
    END_BIND_CTRL()

protected:
    bool OnToolEvent(void* param);

private:
    void OnDragDrop();

private:
    CButtonUI* btn_tool_handle_;
    CButtonUI* btn_tool_normal_;
    COptionUI* opt_right_run_;
    CVerticalLayoutUI* key_body_;

    CMDLDropSource  *m_pMDLDragDataSrc;
    CMDLDropTarget  *m_pMDLDropTarget;
};

#endif  // !#define (_KEY_WND_INCLUDE_H_)  