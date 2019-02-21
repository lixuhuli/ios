#include "StdAfx.h"
#include "WndUninstall.h"
#include "UnSetupModule.h"
#include <DataCenter\DataCenterApi.h>
#include <process.h>

enum {
	WM_MSG_BASE = WM_USER + 611,
	WM_MSG_UNINSTALL,
};

UINT WINAPI ThreadDelDir(void* lpParam)
{
	HWND hWnd = (HWND)lpParam;
	std::wstring strDir = GetDocumentPath();
	if (!strDir.empty() && PathFileExists(strDir.c_str()))
	{
		int nPos = strDir.rfind(APP_DIR_NAME);
		if (nPos != wstring::npos)
		{
			OUTPUT_XYLOG(LEVEL_INFO, L"开始清理文件夹：%s", strDir.c_str());
			PublicLib::RemoveDir(strDir.c_str());
		}
	}
	::PostMessage(hWnd, WM_CLOSE, 0, 0);
	return 0;
}

CWndUninstall::CWndUninstall()
: m_pTabSelect(NULL)
, m_pProgress(NULL)
, m_pOptDelConfig(NULL)
{
	m_bShowShadow = false;
}


CWndUninstall::~CWndUninstall()
{
}

void CWndUninstall::OnFinalMessage(HWND hWnd)
{
	delete this;
	PostQuitMessage(0);
}

void CWndUninstall::InitWindow()
{
	CWndBase::InitWindow();
	m_pTabSelect = dynamic_cast<CTabLayoutUI*>(m_pm.FindControl(L"tab_select"));
	ASSERT(m_pTabSelect);
	CControlUI *pCtrl = m_pm.FindControl(L"btn_help");
	ASSERT(pCtrl);
	pCtrl->OnNotify += MakeDelegate(this, &CWndUninstall::OnNotifyBtnHelp);
	pCtrl = m_pm.FindControl(L"btn_reinstall");
	ASSERT(pCtrl);
	pCtrl->OnNotify += MakeDelegate(this, &CWndUninstall::OnNotifyBtnReinstall);
	pCtrl = m_pm.FindControl(L"btn_uninstall");
	ASSERT(pCtrl);
	pCtrl->OnNotify += MakeDelegate(this, &CWndUninstall::OnNotifyBtnUnInstall);
	pCtrl = m_pm.FindControl(L"btn_finish");
	ASSERT(pCtrl);
	pCtrl->OnNotify += MakeDelegate(this, &CWndUninstall::OnNotifyBtnFinish);
}

LRESULT CWndUninstall::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_MSG_UNINSTALL)
		return OnMsgUninstall(wParam, lParam);
	return CWndBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CWndUninstall::OnMsgUninstall(WPARAM wParam, LPARAM lParam)
{
	int nPercent = (int)lParam;
	if (nPercent < 0)
		nPercent = 0;
	else if (nPercent>100)
		nPercent = 100;
	m_pProgress->SetValue(nPercent);
	switch (wParam)
	{
	case Step_Init:
		break;
	case Step_DelReg:
		break;
	case Step_DelShortcut:
		break;
	case Step_DelFiles:
		break;
	case Step_Finish://卸载完成
		{
			CreateDataTask(EVENT_UNINSTALL);
			m_pTabSelect->SelectItem(4);
		}
		break;
	default:
		break;
	}
	return 0;
}

bool CWndUninstall::OnNotifyBtnClose(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		Close();
	}
	return true;
}

bool CWndUninstall::OnNotifyBtnMin(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
	return true;
}

bool CWndUninstall::OnNotifyBtnHelp(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		wstring strQQLink(L"https://jq.qq.com/?_wv=1027&k=5IIsEtp");
		ShellExecute(NULL, L"open", strQQLink.c_str(), NULL, NULL, SW_SHOW);
		Close();
	}
	return true;
}

bool CWndUninstall::OnNotifyBtnReinstall(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		//跳转网页
		//ShellExecute(NULL, L"open", PC_URL, NULL, NULL, SW_SHOW);
		Close();
	}
	return true;
}

bool CWndUninstall::OnNotifyBtnUnInstall(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		//开始卸载
		m_pTabSelect->SelectItem(3);
		CUnSetupModule::Instance()->SetMsg(m_hWnd, WM_MSG_UNINSTALL);
		CUnSetupModule::Instance()->StartUnSetup();
	}
	return true;
}

bool CWndUninstall::OnNotifyBtnFinish(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		if (m_pOptDelConfig->IsSelected())
		{
			//删除数据库
			HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadDelDir, m_hWnd, 0, NULL);
			CloseHandle(hThread);
			ShowWindow(false, false);
			return true;
		}
		std::wstring strDir = GetDocumentPath() + L"\\Data.dat";
		DeleteFile(strDir.c_str());
		Close();
	}
	return true;
}

bool CWndUninstall::OnNotifyBtnNext(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		if (pNotify->pSender->GetName().Find(L"1") != -1)
		{
			m_pTabSelect->SelectItem(1);
		}
		else if (pNotify->pSender->GetName().Find(L"2") != -1)
		{
			m_pTabSelect->SelectItem(2);
		}
	}
	return true;
}

bool CWndUninstall::OnNotifyBtnBack(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		if (pNotify->pSender->GetName().Find(L"2") != -1)
		{
			m_pTabSelect->SelectItem(0);
		}
		else if (pNotify->pSender->GetName().Find(L"3") != -1)
		{
			m_pTabSelect->SelectItem(1);
		}
	}
	return true;
}

bool CWndUninstall::OnNotifyOptHelp(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_SELECTCHANGED)
	{
		m_pTabSelect->SelectItem(0);
	}
	return true;
}

bool CWndUninstall::OnNotifyOptInstall(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_SELECTCHANGED)
	{
		m_pTabSelect->SelectItem(1);
	}
	return true;
}

bool CWndUninstall::OnNotifyOptUnInstall(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_SELECTCHANGED)
	{
		m_pTabSelect->SelectItem(2);
	}
	return true;
}
