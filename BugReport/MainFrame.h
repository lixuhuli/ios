#pragma once
#include "WndBase.h"

void UploadZip();

class CMainFrame : 
	public CWndBase
{
public:
	CMainFrame(void);
	~CMainFrame(void);

protected:
	virtual LPCWSTR GetWndName()const { return L"悟饭游戏厅错误上报程序"; };
	virtual LPCWSTR GetXmlPath()const { return L"BugReport.xml"; }
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool OnNotifyBtnOK(void* lpParam);

	static unsigned int WINAPI SendThread(void* pParam);

	void PostVersionAndTime();
	void QuitAndRestart();
	BEGIN_INIT_CTRL()
		DECLARE_CTRL_TYPE(m_pBtnReOpen, CCheckBoxUI*, L"checkbox_restart")
		DECLARE_CTRL(m_pTextTitle, L"text_title")
	END_INIT_CTRL()
	BEGIN_BIND_CTRL()
		BIND_CTRL(L"btn_close", &CMainFrame::OnNotifyBtnOK)
		BIND_CTRL(L"btn_ok", &CMainFrame::OnNotifyBtnOK)
	END_BIND_CTRL()

private:
	bool b_isSendingDeump;
	CCheckBoxUI *m_pBtnReOpen;
	CControlUI *m_pTextTitle;
};
