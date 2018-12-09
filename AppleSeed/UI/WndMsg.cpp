#include "StdAfx.h"
#include "WndMsg.h"

CWndMsg::CWndMsg(const wstring& strTitle, const wstring& strText, DWORD dwType)
 : m_strText(strText)
 , lbl_text_(nullptr)
 , m_dwSHowStyle(dwType)
 , btn_ok_(nullptr) 
 , btn_cancel_(nullptr) {
	m_bEscape = true;
}

CWndMsg::~CWndMsg() {
}

LPCWSTR CWndMsg::GetXmlPath() const {
	return L"WndMsg.xml";
}

void CWndMsg::InitWindow() {
	__super::InitWindow();
    
    if (!btn_ok_ || !lbl_text_ || !btn_cancel_ || !ctrl_division_) return;

    if (lbl_text_) lbl_text_->SetText(m_strText.c_str());

    if ((m_dwSHowStyle & 0xF) == MB_OK) {
        ctrl_division_->SetVisible(false);
        btn_cancel_->SetVisible(false);
    }

    if (m_dwSHowStyle & MB_OKCANCEL) {
        btn_ok_->SetText(L"È·¶¨");
    }
}

bool CWndMsg::OnClickBtnOK(void* lpParam) {
	Close(IDOK);
	return true;
}

bool CWndMsg::OnClickBtnCancel(void* lpParam) {
    Close(IDCANCEL);
    return true;
}
