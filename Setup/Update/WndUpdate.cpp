#include "StdAfx.h"
#include "WndUpdate.h"
#include "UpdateModule.h"
#include "Config.h"


enum {
	WM_MSG_BASE = WM_USER + 611,
	WM_MSG_UPDATE,
};

CWndUpdate::CWndUpdate(bool bNeedCheck)
: m_pTabMain(NULL)
, m_pLblStatus(NULL)
, m_pProgress(NULL)
, m_bNeedCheck(bNeedCheck)
{
	m_bShowShadow = false;
}


CWndUpdate::~CWndUpdate()
{
	
}

void CWndUpdate::OnFinalMessage(HWND hWnd)
{
	delete this;
	PostQuitMessage(0);
}

void CWndUpdate::InitWindow()
{
	CWndBase::InitWindow();
	CUpdateModule::Instance()->SetMsg(m_hWnd, WM_MSG_UPDATE);
	if (m_bNeedCheck)
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"检测是否需要升级");
		CUpdateModule::Instance()->StartCheckUpdate();
		m_pTabMain->SelectItem(0);
	}
	else
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"切换到升级页面");
		SwitchToUpdate();
	}
}

void CWndUpdate::SwitchToUpdate()
{
	const wstring &strDesc = CUpdateModule::Instance()->GetUpdateDesc();
	wstring str = StrReplaceW(strDesc, L"\n", L"");
	str = StrReplaceW(str, L"<br/>", L"\n");
	if (!str.empty() && str[str.size()-1] == '\n')
		str.erase(str.end()-1);
	CDuiString strText;
	strText.Format(L"发现新版本\n%sV%s更新内容：\n%s", SOFT_NAME, CUpdateModule::Instance()->GetUpdateVersion().c_str(), str.c_str());
	m_pRichEdit->SetText(strText);
	m_pTabMain->SelectItem(3);
	//m_nUpdateStatus 更新状态（1强制 2可选 0或者3不更新）
	if (CUpdateModule::Instance()->GetUpdateStatus() == 1)
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"进入强制升级流程");
		m_pTabMain->SelectItem(4);
		CUpdateModule::Instance()->StartUpdate();
	}
}

LRESULT CWndUpdate::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_MSG_UPDATE)
		return OnMsgUpdate(wParam, lParam);
	return CWndBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CWndUpdate::OnMsgUpdate(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case Step_Check://检测更新
		{
			int nIndex = 0;
			switch (lParam)
			{
			case 0: nIndex = 2; break;	//更新失败
			case 1: Close(); break;		//自更新
			case 2: nIndex = 1; break;	//当前已是最新版本
			case 3: SwitchToUpdate(); nIndex = 3; break; //可更新
			default: break;
			}
			m_pTabMain->SelectItem(nIndex);
		}
		break;
	case Step_Init://初始化中
		{
			m_pTabMain->SelectItem(4);
			HWND hWnd = FindWindow(GUI_CLASS_NAME, L"悟饭游戏厅");
			if (IsWindow(hWnd))
				::PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;
	case Step_Load://下载中
		{
			int nPercent = (int)lParam;
			if (nPercent == -1)
			{
				m_pLblStatus->SetText(L"下载更新文件失败...");
				break;
			}
			m_pLblStatus->SetText(L"正在下载更新");
			m_pProgress->SetValue(nPercent);
			CDuiString str;
			str.Format(L"%d%%", nPercent);
			m_pLblProgress->SetText(str);
		}
		break;
	case Step_Install://安装中
		{
			int nPercent = (int)lParam;
			if (nPercent == -1)
			{
				m_pLblStatus->SetText(L"安装更新文件失败，即将执行回滚...");
				break;
			}
			m_pLblStatus->SetText(L"正在安装更新");
			m_pProgress->SetValue(nPercent);
			CDuiString str;
			str.Format(L"%d%%", nPercent);
			m_pLblProgress->SetText(str);
		}
		break;
	case Step_Rollback://回滚中
		{
			int nPercent = (int)lParam;
			m_pLblStatus->SetText(L"正在回滚");
			m_pProgress->SetValue(nPercent);
			CDuiString str;
			str.Format(L"%d%%", nPercent);
			m_pLblProgress->SetText(str);
			if (nPercent<=0)
			{
				m_pLblErrorInfo->SetText(L"更新失败，请稍后关闭悟饭游戏厅后重试");
				m_pTabMain->SelectItem(2);
			}
		}
		break;
	case Step_Regedit://更新注册表
		break;
	case Step_Finish://更新完成
		m_pTabMain->SelectItem(5);
        SetShowUpdateWnd(true);
		break;
	default:
		break;
	}
	return 0;
}

bool CWndUpdate::OnNotifyBtnClose(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		Close();
	}
	return true;
}

bool CWndUpdate::OnNotifyBtnMin(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
	return true;
}

bool CWndUpdate::OnNotifyBtnIgnore(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		Close();
	}
	return true;
}

bool CWndUpdate::OnNotifyBtnUpdate(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		m_pTabMain->SelectItem(4);
		CUpdateModule::Instance()->StartUpdate();
	}
	return true;
}

bool CWndUpdate::OnNotifyBtnFinish(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		wstring strExe = CUpdateModule::Instance()->GetInstallPath() + EXE_MAIN;
		PublicLib::ShellExecuteRunas(strExe.c_str(), NULL, NULL);
		Close();
	}
	return true;
}


wstring StrReplaceW(const wstring& strContent, const wstring& strTag, const wstring& strReplace)
{
	size_t nBegin = 0, nFind = 0;
	nFind = strContent.find(strTag, nBegin);
	if (nFind == wstring::npos)
		return strContent;
	size_t nTagLen = strTag.size();
	wstring strRet;
	while (true)
	{
		strRet.append(strContent.begin() + nBegin, strContent.begin() + nFind);
		strRet.append(strReplace);
		nBegin = nFind + nTagLen;
		nFind = strContent.find(strTag, nBegin);
		if (nFind == wstring::npos)
		{
			strRet.append(strContent.begin() + nBegin, strContent.end());
			break;
		}
	}
	return strRet;
}