#pragma once
#include "WndBase.h"


class CWndUninstall :
	public CWndBase
{
public:
	CWndUninstall();
	~CWndUninstall();

protected:
	virtual LPCWSTR GetWndName()const				{ return UNINSTALL_WND_NAME; }
	virtual LPCWSTR GetXmlPath()const				{ return L"WndUnsetup.xml"; }

	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgUninstall(WPARAM wParam, LPARAM lParam);

	bool OnNotifyBtnClose(void* lpParam);
	bool OnNotifyBtnMin(void* lpParam);
	bool OnNotifyBtnHelp(void* lpParam);//咨询客服
	bool OnNotifyBtnReinstall(void* lpParam);//一键重装
	bool OnNotifyBtnUnInstall(void* lpParam);//卸载
	bool OnNotifyBtnFinish(void* lpParam);//卸载完成
	bool OnNotifyBtnNext(void* lpParam);
	bool OnNotifyBtnBack(void* lpParam);

	bool OnNotifyOptHelp(void* lpParam);
	bool OnNotifyOptInstall(void* lpParam);
	bool OnNotifyOptUnInstall(void* lpParam);

	BEGIN_BIND_CTRL()
		BIND_CTRL(L"opt_help", &CWndUninstall::OnNotifyOptHelp)
		BIND_CTRL(L"opt_install", &CWndUninstall::OnNotifyOptInstall)
		BIND_CTRL(L"opt_uninstall", &CWndUninstall::OnNotifyOptUnInstall)
		BIND_CTRL(L"btn_sys_min", &CWndUninstall::OnNotifyBtnMin)
		BIND_CTRL(L"btn_sys_close", &CWndUninstall::OnNotifyBtnClose)
		BIND_CTRL(L"btn_next1", &CWndUninstall::OnNotifyBtnNext)
		BIND_CTRL(L"btn_next2", &CWndUninstall::OnNotifyBtnNext)
		//BIND_CTRL(L"btn_next3", &CWndUninstall::OnNotifyBtnNext)
		//BIND_CTRL(L"btn_back1", &CWndUninstall::OnNotifyBtnBack)
		BIND_CTRL(L"btn_back2", &CWndUninstall::OnNotifyBtnBack)
		BIND_CTRL(L"btn_back3", &CWndUninstall::OnNotifyBtnBack)
	END_BIND_CTRL()
	BEGIN_INIT_CTRL()
		DECLARE_CTRL_TYPE(m_pOptDelConfig, COptionUI*, L"opt_del_config")
		DECLARE_CTRL_TYPE(m_pProgress, CProgressUI*, L"progress")
	END_INIT_CTRL()

private:
	CTabLayoutUI *m_pTabSelect;
	CProgressUI *m_pProgress;
	COptionUI *m_pOptDelConfig;
};

