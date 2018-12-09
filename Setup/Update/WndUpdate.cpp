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
		OUTPUT_XYLOG(LEVEL_INFO, L"����Ƿ���Ҫ����");
		CUpdateModule::Instance()->StartCheckUpdate();
		m_pTabMain->SelectItem(0);
	}
	else
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"�л�������ҳ��");
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
	strText.Format(L"�����°汾\n%sV%s�������ݣ�\n%s", SOFT_NAME, CUpdateModule::Instance()->GetUpdateVersion().c_str(), str.c_str());
	m_pRichEdit->SetText(strText);
	m_pTabMain->SelectItem(3);
	//m_nUpdateStatus ����״̬��1ǿ�� 2��ѡ 0����3�����£�
	if (CUpdateModule::Instance()->GetUpdateStatus() == 1)
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"����ǿ����������");
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
	case Step_Check://������
		{
			int nIndex = 0;
			switch (lParam)
			{
			case 0: nIndex = 2; break;	//����ʧ��
			case 1: Close(); break;		//�Ը���
			case 2: nIndex = 1; break;	//��ǰ�������°汾
			case 3: SwitchToUpdate(); nIndex = 3; break; //�ɸ���
			default: break;
			}
			m_pTabMain->SelectItem(nIndex);
		}
		break;
	case Step_Init://��ʼ����
		{
			m_pTabMain->SelectItem(4);
			HWND hWnd = FindWindow(GUI_CLASS_NAME, L"����Ϸ��");
			if (IsWindow(hWnd))
				::PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;
	case Step_Load://������
		{
			int nPercent = (int)lParam;
			if (nPercent == -1)
			{
				m_pLblStatus->SetText(L"���ظ����ļ�ʧ��...");
				break;
			}
			m_pLblStatus->SetText(L"�������ظ���");
			m_pProgress->SetValue(nPercent);
			CDuiString str;
			str.Format(L"%d%%", nPercent);
			m_pLblProgress->SetText(str);
		}
		break;
	case Step_Install://��װ��
		{
			int nPercent = (int)lParam;
			if (nPercent == -1)
			{
				m_pLblStatus->SetText(L"��װ�����ļ�ʧ�ܣ�����ִ�лع�...");
				break;
			}
			m_pLblStatus->SetText(L"���ڰ�װ����");
			m_pProgress->SetValue(nPercent);
			CDuiString str;
			str.Format(L"%d%%", nPercent);
			m_pLblProgress->SetText(str);
		}
		break;
	case Step_Rollback://�ع���
		{
			int nPercent = (int)lParam;
			m_pLblStatus->SetText(L"���ڻع�");
			m_pProgress->SetValue(nPercent);
			CDuiString str;
			str.Format(L"%d%%", nPercent);
			m_pLblProgress->SetText(str);
			if (nPercent<=0)
			{
				m_pLblErrorInfo->SetText(L"����ʧ�ܣ����Ժ�ر�����Ϸ��������");
				m_pTabMain->SelectItem(2);
			}
		}
		break;
	case Step_Regedit://����ע���
		break;
	case Step_Finish://�������
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