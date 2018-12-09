#pragma once
#include "WndBase.h"


class CWndMsgbox
	:public CWndBase
{
public:
	CWndMsgbox(const wstring& strMsg);
	~CWndMsgbox();
	void	SetMsg(LPCTSTR lpMsg)			{ m_strMsg = lpMsg; }

protected:
	virtual LPCWSTR GetWndName()const		{ return L"弹窗提示"; }
	virtual LPCWSTR GetXmlPath()const		{ return L"MsgWnd1.xml"; }
	virtual void	InitWindow();

	bool	OnNotifyBtnClose(void* pParam);
	bool	OnNotifyBtnYes(void* pParam);

private:
	wstring		m_strMsg;
};

class CWndMsgbox2
	:public CWndBase
{
public:
	CWndMsgbox2(const wstring& strMsg);
	~CWndMsgbox2();
	void	SetMsg(LPCTSTR lpMsg)			{ m_strMsg = lpMsg; }

protected:
	virtual LPCWSTR GetWndName()const		{ return L"弹窗提示"; }
	virtual LPCWSTR GetXmlPath()const		{ return L"MsgWnd2.xml"; }
	virtual void	InitWindow();

	bool	OnNotifyBtnClose(void* pParam);
	bool	OnNotifyBtnYes(void* pParam);
	bool	OnNotifyBtnNo(void* pParam);

private:
	wstring		m_strMsg;
};