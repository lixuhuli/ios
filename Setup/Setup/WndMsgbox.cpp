#include "stdafx.h"
#include "WndMsgbox.h"


CWndMsgbox::CWndMsgbox(const wstring& strMsg)
: m_strMsg(strMsg)
{
	m_bEscapExit = true;
}

CWndMsgbox::~CWndMsgbox()
{

}

void CWndMsgbox::InitWindow()
{
	CControlUI* pControl = m_pm.FindControl(L"btn_ok");
	if ( pControl )
		pControl->OnNotify += MakeDelegate(this, &CWndMsgbox::OnNotifyBtnYes);
	pControl = m_pm.FindControl(L"btn_close");
	if ( pControl )
		pControl->OnNotify += MakeDelegate(this, &CWndMsgbox::OnNotifyBtnClose);
	pControl = m_pm.FindControl(L"text_msg");
	if ( pControl )
		pControl->SetText(m_strMsg.c_str());

}

bool CWndMsgbox::OnNotifyBtnClose(void* pParam)
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pParam;
	if(pNotifyUI->sType != DUI_MSGTYPE_CLICK) 
		return true;
	Close(IDCLOSE);
	return true;
}

bool CWndMsgbox::OnNotifyBtnYes(void* pParam)
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pParam;
	if(pNotifyUI->sType != DUI_MSGTYPE_CLICK) 
		return true;
	Close(IDYES);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndMsgbox2::CWndMsgbox2(const wstring& strMsg)
: m_strMsg(strMsg)
{
	//m_bShowShadow = false;
}

CWndMsgbox2::~CWndMsgbox2()
{

}

void CWndMsgbox2::InitWindow()
{
	CControlUI* pControl = m_pm.FindControl(L"btn_ok");
	if (pControl)
		pControl->OnNotify += MakeDelegate(this, &CWndMsgbox2::OnNotifyBtnYes);
	pControl = m_pm.FindControl(L"btn_close");
	if (pControl)
		pControl->OnNotify += MakeDelegate(this, &CWndMsgbox2::OnNotifyBtnClose);
	pControl = m_pm.FindControl(L"btn_no");
	if (pControl)
		pControl->OnNotify += MakeDelegate(this, &CWndMsgbox2::OnNotifyBtnNo);
	pControl = m_pm.FindControl(L"text_msg");
	if (pControl)
		pControl->SetText(m_strMsg.c_str());
}

bool CWndMsgbox2::OnNotifyBtnClose(void* pParam)
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pParam;
	if (pNotifyUI->sType != DUI_MSGTYPE_CLICK)
		return true;
	Close(IDCLOSE);
	return true;
}

bool CWndMsgbox2::OnNotifyBtnYes(void* pParam)
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pParam;
	if (pNotifyUI->sType != DUI_MSGTYPE_CLICK)
		return true;
	Close(IDYES);
	return true;
}

bool CWndMsgbox2::OnNotifyBtnNo(void* pParam)
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pParam;
	if (pNotifyUI->sType != DUI_MSGTYPE_CLICK)
		return true;
	Close(IDNO);
	return true;
}
