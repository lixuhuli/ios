#ifndef _KEY_WND_INCLUDE_H_
#define _KEY_WND_INCLUDE_H_

#pragma once
#include "dragdrophandle.h"
#include "cef_weak_ptr.h"

namespace emulator{
    class SceneInfo;
    class iSceneInfo;
}

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

public:
    bool BrowserMode() { return browser_mode_; }
    void SetBrowserMode(bool browser_mode);

    // 模拟器状态信息 给外部调用
    base::WeakPtr<emulator::iSceneInfo> scene_info();

protected:
    virtual LPCWSTR GetWndName() const override { return L"果仁模拟器"; }
    virtual LPCWSTR GetXmlPath() const override;
    virtual LPCTSTR GetWindowClassName() const override;
    virtual CControlUI* CreateControl(LPCTSTR pstrClass);
    virtual void InitWindow();
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

    virtual LRESULT OnSetCursor(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    virtual void OnFinalMessage(HWND hWnd);

    bool OnClickBtnClose(void* lpParam);
    bool OnBtnToolHandle(void* param);
    bool OnBtnToolNormal(void* param);
    bool OnBtnToolRightRun(void* param);
    bool OnBtnToolIntelligent(void* param);
    bool OnBtnSave(void* param);
    bool OnBtnDelete(void* param);
    
    bool OnEditKeyChanged(void* param);
    bool OnEditRightMouseChanged(void* param);
    bool OnEditIntelligentChanged(void* param);

    bool OnSliderRightMouseValueChanged(void* param);
    bool OnSliderIntelligentChanged(void* param);

    bool OnOptIntelligentSwitch(void* param);
    bool OnSliderKeyTransChanged(void* param);

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE(btn_tool_handle_, CButtonUI, L"btn_tool_handle")
        DECLARE_CTRL_TYPE(btn_tool_normal_, CButtonUI, L"btn_tool_normal")
        DECLARE_CTRL_TYPE(key_body_, CVerticalLayoutUI, L"key_body")
        DECLARE_CTRL_TYPE(opt_right_run_, COptionUI, L"opt_right_run")
        DECLARE_CTRL_TYPE(panel_tools_, CVerticalLayoutUI, L"panel_tools")
        DECLARE_CTRL_TYPE(key_slider_trans_, CSliderUI, L"key_slider_trans")
        DECLARE_CTRL_TYPE(lbl_trans_percent_, CLabelUI, L"lbl_trans_percent")
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK(L"btn_close", &CKeyWnd::OnClickBtnClose)
        BIND_CTRL_CLICK(L"btn_tool_handle", &CKeyWnd::OnBtnToolHandle)
        BIND_CTRL_CLICK(L"btn_tool_normal", &CKeyWnd::OnBtnToolNormal)
        BIND_CTRL_CLICK(L"opt_right_run", &CKeyWnd::OnBtnToolRightRun)
        BIND_CTRL_CLICK(L"btn_tool_intelligent", &CKeyWnd::OnBtnToolIntelligent)
        BIND_CTRL_CLICK(L"btn_save", &CKeyWnd::OnBtnSave)
        BIND_CTRL_EVENT(L"key_slider_trans", DUI_MSGTYPE_VALUECHANGED, &CKeyWnd::OnSliderKeyTransChanged)
    END_BIND_CTRL()

protected:
    bool OnToolEvent(void* param);

private:
    void OnDragDrop();

    void LoadKeyItems();
    void UpdateItemsPos();

    CControlUI* CreateNormalKey();
    CControlUI* CreateRightMouse();
    CControlUI* CreateIntelligent();

private:
    CButtonUI* btn_tool_handle_;
    CButtonUI* btn_tool_normal_;
    COptionUI* opt_right_run_;
    CVerticalLayoutUI* key_body_;
    CVerticalLayoutUI* panel_tools_;
    CSliderUI* key_slider_trans_;
    CLabelUI* lbl_trans_percent_;

    CMDLDropSource  *m_pMDLDragDataSrc;
    CMDLDropTarget  *m_pMDLDropTarget;

    bool browser_mode_;

    scoped_refptr<emulator::SceneInfo> scene_info_;
    scoped_refptr<emulator::SceneInfo> scene_bak_info_;
};

#endif  // !#define (_KEY_WND_INCLUDE_H_)  