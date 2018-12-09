#pragma once
#include "WndBase.h"





class CWndSetup :
	public CWndBase
{
public:
	CWndSetup();
	~CWndSetup();

protected:
	virtual LPCWSTR GetWndName()const				{ return SETUP_WND_NAME; }
	virtual LPCWSTR GetXmlPath()const				{ return L"WndSetup.xml"; }
	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnMsgInstall(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgTip(WPARAM wParam, LPARAM lParam);

    bool OnBtnCreate(void* lpParam);//����
	bool OnNotifyBtnClose(void* lpParam);//�ر�
    bool OnNotifyBtnExit(void* lpParam);//�˳�
    bool OnNotifyBtnCancel(void* lpParam);//ȡ��
	bool OnNotifyBtnMin(void* lpParam);
	bool OnNotifyBtnInstall(void* lpParam);//һ����װ
	bool OnNotifyBtnSelf(void* lpParam);//�Զ��尲װ
	bool OnNotifyBtnSelect(void* lpParam);//���
	bool OnNotifyBtnRun(void* lpParam);//����
    bool OnClickInstallRestart(void* param);
    bool OnClickInstallClose(void* param);

	wstring GetInstallPath(bool& bInstallOld);
	__int64 GetInstallFreeSpace(const wstring& strPath);
	void BootRun();

	BEGIN_BIND_CTRL()
		BIND_CTRL(L"btn_min", &CWndSetup::OnNotifyBtnMin)
        BIND_CTRL(L"btn_create", &CWndSetup::OnBtnCreate)
		BIND_CTRL(L"btn_close", &CWndSetup::OnNotifyBtnClose)
        BIND_CTRL(L"btn_exit", &CWndSetup::OnNotifyBtnExit)
        BIND_CTRL(L"btn_exit2", &CWndSetup::OnNotifyBtnExit)
        BIND_CTRL(L"btn_cancel", &CWndSetup::OnNotifyBtnCancel)
        BIND_CTRL(L"btn_install_restart", &CWndSetup::OnClickInstallRestart)
        BIND_CTRL(L"btn_install_close", &CWndSetup::OnClickInstallClose)
	END_BIND_CTRL()
	BEGIN_INIT_CTRL()
		DECLARE_CTRL_TYPE(m_pEditPath, CEditUI*, L"edit_path")
		DECLARE_CTRL_TYPE(m_pTabMain, CTabLayoutUI*, L"tab_main")
		DECLARE_CTRL_TYPE(m_pProgress, CProgressUI*, L"progress")
		DECLARE_CTRL_TYPE(m_pOptBootrun, COptionUI*, L"opt_bootrun")
        DECLARE_CTRL_TYPE(slogan_, CVerticalLayoutUI*, L"slogan")
		DECLARE_CTRL_TYPE(m_pLblStatus, CLabelUI*, L"lbl_status")
        DECLARE_CTRL_TYPE(btn_install_restart_, CButtonUI*, L"btn_install_restart")
        DECLARE_CTRL_TYPE(btn_install_close_, CButtonUI*, L"btn_install_close")
	END_INIT_CTRL()

private:
	CEditUI *m_pEditPath;
	COptionUI *m_pOptBootrun;
	CProgressUI *m_pProgress;
	CTabLayoutUI *m_pTabMain;
	CLabelUI *m_pLblStatus;

    CVerticalLayoutUI* slogan_;
    CStoryBoard* animation_linear_;

    CButtonUI* btn_install_restart_;
    CButtonUI* btn_install_close_;
};

