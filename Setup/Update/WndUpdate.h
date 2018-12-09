#pragma once
#include "WndBase.h"



class CWndUpdate :
	public CWndBase
{
public:
	CWndUpdate(bool bNeedCheck);
	~CWndUpdate();

protected:
	virtual LPCWSTR GetWndName()const				{ return UPDATE_WND_NAME; }
	virtual LPCWSTR GetXmlPath()const				{ return L"WndUpdate.xml"; }

	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgUpdate(WPARAM wParam, LPARAM lParam);

	bool OnNotifyBtnClose(void* lpParam);
	bool OnNotifyBtnMin(void* lpParam);
	bool OnNotifyBtnIgnore(void* lpParam);//下次再说
	bool OnNotifyBtnUpdate(void* lpParam);//立即更新
	bool OnNotifyBtnFinish(void* lpParam);//更新完成

	void SwitchToUpdate();

	BEGIN_INIT_CTRL()
		DECLARE_CTRL_TYPE(m_pTabMain, CTabLayoutUI*, L"tab_main")
		DECLARE_CTRL_TYPE(m_pLblStatus, CLabelUI*, L"lbl_status")
		DECLARE_CTRL_TYPE(m_pProgress, CProgressUI*, L"progress")
		DECLARE_CTRL_TYPE(m_pLblProgress, CLabelUI*, L"lbl_progress")
		DECLARE_CTRL(m_pRichEdit, L"richedit")
		DECLARE_CTRL_TYPE(m_pLblErrorInfo, CLabelUI*, L"lbl_error_info")
	END_INIT_CTRL()

	BEGIN_BIND_CTRL()
		BIND_CTRL(L"btn_close", &CWndUpdate::OnNotifyBtnClose);
		BIND_CTRL(L"btn_min", &CWndUpdate::OnNotifyBtnMin);
		BIND_CTRL(L"btn_ignore", &CWndUpdate::OnNotifyBtnIgnore);
		BIND_CTRL(L"btn_update", &CWndUpdate::OnNotifyBtnUpdate);
		BIND_CTRL(L"btn_finish", &CWndUpdate::OnNotifyBtnFinish);
	END_BIND_CTRL()

private:
	bool m_bNeedCheck;
	CTabLayoutUI *m_pTabMain;
	CLabelUI *m_pLblStatus;
	CLabelUI *m_pLblProgress;
	CLabelUI *m_pLblErrorInfo;
	CProgressUI *m_pProgress;
	CControlUI *m_pRichEdit;
};

wstring StrReplaceW(const wstring& strContent, const wstring& strTag, const wstring& strReplace);