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
	bool OnNotifyBtnRun(void* lpParam);//更新完成
    bool OnNotifyBtnExit(void* lpParam);//退出
    bool OnClickUpdateRestart(void* param);
    bool OnClickUpdateClose(void* param);

    void SwitchToUpdate();

	BEGIN_INIT_CTRL()
		DECLARE_CTRL_TYPE(m_pTabMain, CTabLayoutUI*, L"tab_main")
		DECLARE_CTRL_TYPE(m_pLblStatus, CLabelUI*, L"lbl_status")
		DECLARE_CTRL_TYPE(m_pProgress, CProgressUI*, L"progress")
        DECLARE_CTRL_TYPE(slogan_, CVerticalLayoutUI*, L"slogan")
        DECLARE_CTRL_TYPE(btn_update_restart_, CButtonUI*, L"btn_update_restart")
        DECLARE_CTRL_TYPE(btn_update_close_, CButtonUI*, L"btn_update_close")
	END_INIT_CTRL()

	BEGIN_BIND_CTRL()
		BIND_CTRL(L"btn_close", &CWndUpdate::OnNotifyBtnClose);
		BIND_CTRL(L"btn_min", &CWndUpdate::OnNotifyBtnMin);
        BIND_CTRL(L"btn_exit", &CWndUpdate::OnNotifyBtnExit)
		BIND_CTRL(L"btn_run", &CWndUpdate::OnNotifyBtnRun);
        BIND_CTRL(L"btn_update_restart", &CWndUpdate::OnClickUpdateRestart)
        BIND_CTRL(L"btn_update_close", &CWndUpdate::OnClickUpdateClose)
	END_BIND_CTRL()

private:
    LPCTSTR CreateProcessByUser(const std::wstring& appCmd);
    ULONG CreateProcessEx2(LUID AuthenticationId, PCWSTR lpApplicationName, PWSTR lpCommandLine);
    ULONG CreateProcessEx1(HANDLE hProcess, PCWSTR lpApplicationName, PWSTR lpCommandLine);

private:
	bool m_bNeedCheck;
	CTabLayoutUI *m_pTabMain;
	CLabelUI *m_pLblStatus;
	CProgressUI *m_pProgress;

    CVerticalLayoutUI* slogan_;

    CButtonUI* btn_update_restart_;
    CButtonUI* btn_update_close_;
};

wstring StrReplaceW(const wstring& strContent, const wstring& strTag, const wstring& strReplace);