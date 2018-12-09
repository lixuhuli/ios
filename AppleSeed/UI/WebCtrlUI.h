#ifndef __UIWEBCTRL_H__
#define __UIWEBCTRL_H__

#pragma once
#include "UICefWebkit.h"
#include <functional>

#define  UI_WEB_CTRL   L"WebCtrl"

enum ChildPage
{
    RpLoading = 0,
    RpError,
    RpFinish,
};

class CCefWebkitUI;
class CWebCtrlUI :
    public CCefWebCallback,
    public CVerticalLayoutUI, 
    public DuiLib::IDialogBuilderCallback {
public:
    typedef std::function<void(CWebCtrlUI* web_ctrl)> WEB_CALL_BACK;
public:
    CWebCtrlUI() {};
    CWebCtrlUI(CPaintManagerUI *pManager, LPCTSTR lpUrl = L"");
    virtual ~CWebCtrlUI(void);

public:
    virtual void Init() override;
    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

public:
    void SetWebCallBack(const WEB_CALL_BACK& web_call_back);
    void SetImmediateBrowse(bool immediate = true);
    void SetNeedWaiteBrowse(bool need_wait = true);
    void SetFirstNeedCallBack(bool first_need_callback = true);
    bool GetImmediateBrowse() { return immediate_; };

    void NavigateUrl(const std::wstring& url = L"");
    const std::wstring& GetInitUrl();
    void SetInitUrl(const std::wstring& url);
    std::wstring GetWebUrl();

    void ExecuteJavascript(const wstring& strCode);
    CCefWebkitUI* GetCefWebkit() { return m_pCefWebkit; };

    void Close(bool bForce = false);
    void Reload(bool not_callback = false);
    void GoBack();

protected:
    virtual CControlUI* CreateControl(LPCTSTR pstrClass) override;

    virtual void OnLoadStart(CCefWebkitUI* pWeb, void* lpParam) override;                                                             // 开始加载
    virtual void OnLoadEnd(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefBrowser> browser, bool bLoadError) override;               // 加载完成
    virtual void OnLoadError(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefFrame> pFrame) override;                                 // 加载失败
    virtual bool OnOpenNewUrl(CCefWebkitUI* pWeb, void* lpParam, const std::wstring& url) override;                                   // 打开新窗口
    virtual bool OnBeforeBrowse(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefBrowser> browser, const std::wstring& url) override;
    virtual void OnSetLoadingState(CCefWebkitUI* pWeb, void* lpParam, bool isLoading, bool canGoBack, bool canGoForward) override;    // 回调加载状态

    bool OnClickBtnRefresh(void* lpParam);
    void ExecListJavascript();

    BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK_PAGE(L"btn_web_refresh", this, &CWebCtrlUI::OnClickBtnRefresh);
    END_BIND_CTRL()

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE_PAGE(m_pTab, CTabLayoutUI, this, L"web_tab_store");
        DECLARE_CTRL_TYPE_PAGE(m_pCefWebkit, CCefWebkitUI, this, L"web_cef_kit");
    END_INIT_CTRL()

private:
    CCefWebkitUI* m_pCefWebkit;
    CTabLayoutUI* m_pTab;
    std::wstring init_url_;
    bool immediate_;
    bool need_wait_;
    bool first_need_callback_;
    bool not_need_callback_;
    std::list<std::wstring>	list_exec_code_;
    WEB_CALL_BACK web_call_back_;
};

#endif // __UIWEBCTRL_H__