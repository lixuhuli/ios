#ifndef _KEY_WND_INCLUDE_H_
#define _KEY_WND_INCLUDE_H_

#pragma once
#include "dragdrophandle.h"
#include "cef_weak_ptr.h"

namespace emulator{
    class SceneInfo;
    class iSceneInfo;
    struct tagItemInfo;
}

struct KeyMapInfo {
    wstring name_;
    wstring file_;
    wstring default_;
    int tag_;
    int index_;

    KeyMapInfo(): tag_(0), index_(0) {}
};

#define USER_DEFINED_KEYMAP_COUNT    int(5)

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

class CKeyBrowserWnd;
class CKeyWnd :
    public CWndBase {
public:
    CKeyWnd(const string& key_id);
    virtual ~CKeyWnd();

public:
    bool BrowserMode() { return browser_mode_; }
    void SetBrowserMode(bool browser_mode);
    void UpdateBrowserWnd(const QRect* lprc = nullptr);
    void CloseBrowserWnd();

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
    virtual LRESULT OnRemoveKey(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnRemoveItem(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    virtual void OnFinalMessage(HWND hWnd);

    bool OnClickBtnLocal(void* lpParam);
    bool OnClickBtnHelp(void* lpParam);
    bool OnClickBtnClose(void* lpParam);

    bool OnBtnToolHandle(void* param);
    bool OnBtnToolNormal(void* param);
    bool OnBtnToolRightRun(void* param);
    bool OnBtnToolIntelligent(void* param);
    bool OnBtnSave(void* param);
    bool OnBtnDelete(void* param);
    bool OnBtnRestore(void* param);
    
    bool OnEditKeyChanged(void* param);
    bool OnEditRightMouseChanged(void* param);
    bool OnEditIntelligentChanged(void* param);
    bool OnEditHandleCtrlChanged(void* param);

    bool OnSliderRightMouseValueChanged(void* param);
    bool OnSliderIntelligentChanged(void* param);

    bool OnOptIntelligentSwitch(void* param);
    bool OnSliderKeyTransChanged(void* param);

    bool OnKeyPosChanged(void* param);

    bool OnComboxKeyboardSelected(void* param);
    bool OnComboxKeyboardClick(void* param);

    bool OnClickBtnComKeyMap(void* param);

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE_PAGE(btn_tool_handle_, CButtonUI, panel_tools_, L"btn_tool_handle")
        DECLARE_CTRL_TYPE_PAGE(btn_tool_normal_, CButtonUI, panel_tools_, L"btn_tool_normal")
        DECLARE_CTRL_TYPE_PAGE(btn_tool_intelligent_, CButtonUI, panel_tools_, L"btn_tool_intelligent")
        DECLARE_CTRL_TYPE(key_body_, CVerticalLayoutUI, L"key_body")
        DECLARE_CTRL_TYPE_PAGE(opt_right_run_, COptionUI, panel_tools_, L"opt_right_run")
        DECLARE_CTRL_TYPE_PAGE(key_slider_trans_, CSliderUI, panel_tools_, L"key_slider_trans")
        DECLARE_CTRL_TYPE_PAGE(lbl_trans_percent_, CLabelUI, panel_tools_, L"lbl_trans_percent")
        DECLARE_CTRL_TYPE_PAGE(combox_keymap_, CButtonUI, panel_tools_, L"combox_keymap")
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK_PAGE(L"btn_local", panel_tools_, &CKeyWnd::OnClickBtnLocal)
        BIND_CTRL_CLICK_PAGE(L"bnt_help", panel_tools_, &CKeyWnd::OnClickBtnHelp)
        BIND_CTRL_CLICK_PAGE(L"btn_close", panel_tools_, &CKeyWnd::OnClickBtnClose)
        BIND_CTRL_CLICK_PAGE(L"btn_tool_handle", panel_tools_, &CKeyWnd::OnBtnToolHandle)
        BIND_CTRL_CLICK_PAGE(L"btn_tool_normal", panel_tools_, &CKeyWnd::OnBtnToolNormal)
        BIND_CTRL_CLICK_PAGE(L"opt_right_run", panel_tools_, &CKeyWnd::OnBtnToolRightRun)
        BIND_CTRL_CLICK_PAGE(L"btn_tool_intelligent", panel_tools_, &CKeyWnd::OnBtnToolIntelligent)
        BIND_CTRL_CLICK_PAGE(L"btn_save", panel_tools_, &CKeyWnd::OnBtnSave)
        BIND_CTRL_CLICK_PAGE(L"btn_restore", panel_tools_, &CKeyWnd::OnBtnRestore)
        BIND_CTRL_EVENT_PAGE(L"key_slider_trans", panel_tools_, DUI_MSGTYPE_VALUECHANGED, &CKeyWnd::OnSliderKeyTransChanged)
        BIND_CTRL_EVENT_PAGE(L"key_slider_trans", panel_tools_, DUI_MSGTYPE_VALUECHANGING, &CKeyWnd::OnSliderKeyTransChanged)
        BIND_CTRL_CLICK_PAGE(L"combox_keymap", panel_tools_, &CKeyWnd::OnClickBtnComKeyMap)
    END_BIND_CTRL()

protected:
    bool OnToolEvent(void* param);
    bool OnKeyBodySize(void* param);

private:
    void OnDragDrop();

    void LoadKeyItems();
    void UpdateItemsPos();

    CControlUI* CreateHandleKey();
    CControlUI* CreateNormalKey();
    CControlUI* CreateRightMouse();
    CControlUI* CreateIntelligent();

    void InitIntelligent(CControlUI* control, const emulator::tagItemInfo& item);
    void InitNormalKey(CControlUI* control, const emulator::tagItemInfo& item);
    void InitRightMouse(CControlUI* control, const emulator::tagItemInfo& item);
    void InitRemoteHandle(CControlUI* control, const emulator::tagItemInfo& item);

    void CenterKey(CControlUI* control);
    bool KeyToScreen(CControlUI* control);
    bool KeyToScreen(LPPOINT point);

    void ReadCloudKeyboardToCombox();

    void UpdateSceneInfo();

    void UpdateCtrls();

    CControlUI* FindKeyUI(int key_value);

    KeyMapInfo CreateKeyInfo(const wstring& key_name, const wstring& key_file, const wstring& key_default, int tag = 0, int index = 0);

    bool AddDefinedKeyBoard(const string& app_id, int nNum);

    void SelectKeyItem(int index);

    void CreateDefinedKeyFile();

    int GetUserDefinedNum();

    int GetUserDefinedIndex(const wstring& defineds);

private:
    CButtonUI* btn_tool_handle_;
    CButtonUI* btn_tool_normal_;
    CButtonUI* btn_tool_intelligent_;
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

    CKeyBrowserWnd* browser_wnd_;

    string key_id_;

    CComboBoxUI* combox_keyboard_;

    CButtonUI* combox_keymap_;
    std::vector<KeyMapInfo> keymap_info_;  // 键盘映射合集
};

#endif  // !#define (_KEY_WND_INCLUDE_H_)  