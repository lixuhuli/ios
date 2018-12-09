#include "StdAfx.h"
#include "WebCtrlUI.h"


CWebCtrlUI::CWebCtrlUI(CPaintManagerUI *pManager, LPCTSTR lpUrl/* = L""*/)
    : m_pCefWebkit(nullptr)
    , m_pTab(nullptr)
    , need_wait_(true)
    , immediate_(true)
    , init_url_(lpUrl) 
    , not_need_callback_(false)
    , first_need_callback_(true) {
    CDialogBuilder builder;
    auto root = static_cast<CContainerUI*>(builder.Create(L"WebCtrl.xml", (UINT)0, this, pManager));
    if (nullptr != root) this->Add(root);
    else {
        this->RemoveAll();
        return;
    }
}

CWebCtrlUI::~CWebCtrlUI(void) {
    if (m_pCefWebkit) m_pCefWebkit->Close();
    m_pCefWebkit = nullptr;

    list_exec_code_.clear();
}

void CWebCtrlUI::Init() {
    InitControls();
    BindControls();

    // ³õÊ¼»¯ÍøÒ³
    if (m_pCefWebkit && m_pManager) {
        m_pCefWebkit->SetWebloadCallback(this, m_pManager->GetPaintWindow());
        if (immediate_ && !init_url_.empty()) m_pCefWebkit->Navigate(init_url_.c_str());
    }
}

LPCTSTR CWebCtrlUI::GetClass() const {
    return L"WebCtrlUI";
}

LPVOID CWebCtrlUI::GetInterface(LPCTSTR pstrName) {
    if( _tcscmp(pstrName, UI_WEB_CTRL) == 0 ) return static_cast<CWebCtrlUI*>(this);
    return CContainerUI::GetInterface(pstrName);
}

void CWebCtrlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
    if (_tcscmp(pstrName, L"initurl") == 0) init_url_ = pstrValue;
    else if (_tcscmp(pstrName, _T("immediate")) == 0) SetImmediateBrowse(_tcscmp(pstrValue, _T("true")) == 0);
    else if (_tcscmp(pstrName, _T("needwait")) == 0) SetNeedWaiteBrowse(_tcscmp(pstrValue, _T("true")) == 0);
    else if (_tcscmp(pstrName, _T("firstneedcallback")) == 0) SetFirstNeedCallBack(_tcscmp(pstrValue, _T("true")) == 0);
    __super::SetAttribute(pstrName, pstrValue);
}

void CWebCtrlUI::SetWebCallBack(const WEB_CALL_BACK& web_call_back) {
    web_call_back_ = web_call_back;
}

void CWebCtrlUI::SetImmediateBrowse(bool immediate /*= true*/) {
    immediate_ = immediate;
}

void CWebCtrlUI::SetNeedWaiteBrowse(bool need_wait /*= true*/) {
    need_wait_ = need_wait;
}

void CWebCtrlUI::SetFirstNeedCallBack(bool first_need_callback /*= true*/) {
    first_need_callback_ = first_need_callback;
}

DuiLib::CControlUI* CWebCtrlUI::CreateControl(LPCTSTR pstrClass) {
    std::wstring control_name = pstrClass;
    if (control_name == L"CefWebUI") return new CCefWebkitUI();
    return nullptr;
}

bool CWebCtrlUI::OnClickBtnRefresh(void* lpParam) {
    if (m_pCefWebkit) m_pCefWebkit->Refresh();
    return true;
}

void CWebCtrlUI::OnLoadStart(CCefWebkitUI* pWeb, void* lpParam) {
    if (pWeb == m_pCefWebkit) {
        if (m_pTab) m_pTab->SelectItem(need_wait_ ? RpLoading : RpFinish);

        if (!first_need_callback_) {
            first_need_callback_ = true;
            return;
        }

        if (not_need_callback_) {
            not_need_callback_ = false;
            return;
        }

        if (web_call_back_) web_call_back_(this);
    }
}

void CWebCtrlUI::OnLoadEnd(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefBrowser> browser, bool bLoadError) {
    if (pWeb == m_pCefWebkit) {
        if (m_pTab) {
            if (need_wait_ || bLoadError) m_pTab->SelectItem(bLoadError ? RpError : RpFinish);
        }
        if (!bLoadError && !list_exec_code_.empty()) ExecListJavascript();
    }
}

void CWebCtrlUI::OnLoadError(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefFrame> pFrame) {

}

bool CWebCtrlUI::OnOpenNewUrl(CCefWebkitUI* pWeb, void* lpParam, const std::wstring& url) {
    ShellExecute(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOW);
    return true;
}

bool CWebCtrlUI::OnBeforeBrowse(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefBrowser> browser, const std::wstring& url) {
    return false;
}

void CWebCtrlUI::OnSetLoadingState(CCefWebkitUI* pWeb, void* lpParam, bool isLoading, bool canGoBack, bool canGoForward) {
}

void CWebCtrlUI::NavigateUrl(const std::wstring& url/* = L""*/) {
    if (!m_pCefWebkit) return;

    if (url.empty()) {
        if (!init_url_.empty() && m_pCefWebkit->GetLoadingUrl() != init_url_) {
            m_pTab->SelectItem(RpLoading);
            m_pCefWebkit->Navigate(init_url_.c_str());
        }
    }
    else {
        if (init_url_.empty()) init_url_ = url;
        if (m_pCefWebkit->GetLoadingUrl() != url) {
            m_pTab->SelectItem(RpLoading);
            m_pCefWebkit->Navigate(url.c_str());
        }
    }
}

std::wstring CWebCtrlUI::GetWebUrl() {
    return m_pCefWebkit ? m_pCefWebkit->GetLoadingUrl() : L"";
}

const std::wstring& CWebCtrlUI::GetInitUrl() {
    return init_url_;
}

void CWebCtrlUI::SetInitUrl(const std::wstring& url) {
    init_url_ = url;
}

void CWebCtrlUI::ExecuteJavascript(const wstring& strCode) {
    if (!m_pCefWebkit) return;

    list_exec_code_.push_back(strCode);

    if (m_pTab->GetCurSel() != 2) return;

    ExecListJavascript();
}

void CWebCtrlUI::ExecListJavascript() {
    if (!m_pCefWebkit || list_exec_code_.empty()) return;

    for each (auto it in list_exec_code_) {
        m_pCefWebkit->ExecuteJavascript(it);
    }

    list_exec_code_.clear();
}

void CWebCtrlUI::Close(bool bForce /*= false*/) {
    if (m_pCefWebkit) m_pCefWebkit->Close();
}

void CWebCtrlUI::Reload(bool not_callback/* = false*/) {
    not_need_callback_ = not_callback;
    if (m_pCefWebkit) m_pCefWebkit->Reload();
}

void CWebCtrlUI::GoBack() {
    if (m_pCefWebkit) m_pCefWebkit->GoBack();
}
