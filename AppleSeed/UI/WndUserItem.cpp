#include "StdAfx.h"
#include "WndUserItem.h"
#include "WebCtrlUI.h"

CWndUserItem::CWndUserItem(const std::wstring& url)
 : url_(url){
}

CWndUserItem::~CWndUserItem() {
}

LRESULT CWndUserItem::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;

    switch (uMsg) {
    case WM_SETFOCUS: {
        if (web_) {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(m_hWnd, &pt);

            RECT rc = { 608, 17, 625, 57 };
            if (PtInRect(&rc, pt)) break;

            web_->SetFocus();

            return 0;
        }
        break;
    }
    default: bHandled = FALSE;
    }

    if (bHandled) {
        return lRes;
    }

    return __super::HandleMessage(uMsg, wParam, lParam);
}

LPCWSTR CWndUserItem::GetXmlPath() const{
	return L"WndUserItem.xml";
}

void CWndUserItem::InitWindow() {
    __super::InitWindow();
    if (web_ && !url_.empty()) web_->Navigate(url_.c_str());
}

bool CWndUserItem::OnBtnClickClose(void* param) {
    Close(IDCLOSE);
    return true;
}
