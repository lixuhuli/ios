#include "StdAfx.h"
#include "WndToast.h"

CWndToast::CWndToast(const wstring& strText)
 :m_pLabelText(NULL)
 ,m_strText(strText)
 ,m_nAlpha(230)
 ,m_uTimerElapse(50) {
}

CWndToast::~CWndToast(void){
}

LPCWSTR CWndToast::GetXmlPath() const {
	return L"WndToast.xml";
}

LRESULT CWndToast::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg) {
	case WM_TIMER: OnTimer(wParam, lParam); break;
	default:
		break;
	}

	return CWndBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CWndToast::OnTimer(WPARAM wParam, LPARAM lParam) {
    if (wParam == TIMER_ID_GRADUAL_CHANGE) {
        m_nAlpha <=160 ? (m_nAlpha -= 8):(m_nAlpha -= 3);
        if (m_nAlpha <= 0) {
            KillTimer(m_hWnd, TIMER_ID_GRADUAL_CHANGE);
            Close();
        }
        else ::SetLayeredWindowAttributes(m_hWnd, 0, m_nAlpha, LWA_ALPHA);
    }

    return 0;
}

void CWndToast::InitWindow() {
    __super::InitWindow();

	if (m_pLabelText) m_pLabelText->SetText(m_strText.c_str());

    LONG lStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
    ::SetWindowLong(m_hWnd, GWL_EXSTYLE, lStyle | WS_EX_LAYERED);
    ::SetLayeredWindowAttributes(m_hWnd, 0, m_nAlpha, LWA_ALPHA);
    ::SetTimer(m_hWnd, TIMER_ID_GRADUAL_CHANGE, m_uTimerElapse, NULL);

    QRect rc;
    GetClientRect(m_hWnd, &rc);
    int nWidth = m_strText.size() * 24;
    int nheight = rc.GetHeight();
    if (nWidth < rc.GetWidth()) nWidth = rc.GetWidth();
    ::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, nWidth, nheight, SWP_NOMOVE);
}

