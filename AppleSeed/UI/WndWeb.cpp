#include "StdAfx.h"
#include "WndWeb.h"
#include "UrlDefine.h" 

CWndWeb::CWndWeb()
: web_ui_(nullptr) {
    m_dwStyle = UI_WNDSTYLE_FRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
}

CWndWeb::~CWndWeb() {
}

CCefWebkitUI* CWndWeb::web_ctrl(){
    return web_ui_;
}

void CWndWeb::InitWindow() {
	CWndBase::InitWindow();
}

LRESULT CWndWeb::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_CLOSE) {
        if (web_ui_) web_ui_->Close(true);
	}

	return CWndBase::HandleMessage(uMsg, wParam, lParam);
}

void CWndWeb::OnFinalMessage(HWND hWnd) {
    if (web_ui_) web_ui_->Close(true);
    web_ui_ = nullptr;

    return __super::OnFinalMessage(hWnd);
}

CControlUI* CWndWeb::CreateControl(LPCTSTR pstrClass) {
    std::wstring control_name = pstrClass;
    if (control_name == L"CefWebUI") return new CCefWebkitUI();

    return nullptr;
}

LPCWSTR CWndWeb::GetXmlPath() const {
	return L"WndWeb.xml";
}
