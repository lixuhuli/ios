#include "StdAfx.h"
#include "WndSetup.h"
#include "SetupModule.h"
#include "WndMsgbox.h"
#include "RegBoot.h"
#include <DataCenter\DataCenterApi.h>


enum {
	WM_MSG_BASE = WM_USER + 611,
	WM_MSG_INSTALL,
	WM_MSG_TIP,
	WM_MSG_CLOSE,
};

enum{
	ERR_CREATE_DIR = 0,
	ERR_NO_SPACE,
	ERR_EMPTY,
};

CWndSetup::CWndSetup()
: m_pTabMain(NULL)
, slogan_(nullptr)
, m_pOptBootrun(NULL)
, m_pEditPath(NULL)
, m_pProgress(NULL)
, animation_linear_(nullptr)
, btn_install_restart_(nullptr)
, btn_install_close_(nullptr)
{
	m_bShowShadow = false;
}


CWndSetup::~CWndSetup()
{
}

void CWndSetup::OnFinalMessage(HWND hWnd)
{
	delete this;
	PostQuitMessage(0);
}

void CWndSetup::InitWindow()
{
	CWndBase::InitWindow();
	CControlUI* pCtrl = m_pm.FindControl(L"btn_install");
	ASSERT(pCtrl);
	pCtrl->OnNotify += MakeDelegate(this, &CWndSetup::OnNotifyBtnInstall);

	pCtrl = m_pm.FindControl(L"btn_select");
	ASSERT(pCtrl);
	pCtrl->OnNotify += MakeDelegate(this, &CWndSetup::OnNotifyBtnSelect);

	pCtrl = m_pm.FindControl(L"btn_run");
	ASSERT(pCtrl);
	pCtrl->OnNotify += MakeDelegate(this, &CWndSetup::OnNotifyBtnRun);

	// ����Ĭ�ϰ�װ·��
	bool bInstallOld = false;
	wstring strPath = GetInstallPath(bInstallOld);
	m_pEditPath->SetReadOnly(bInstallOld);
	m_pEditPath->SetText(strPath.c_str());

    if (slogan_) {
        animation_linear_ = slogan_->AddStoryBoard(L"linear_show");
        CFadeAnimator* fade_show = new CFadeAnimator(slogan_, CTween::LINEAR, 3000, 0, 255, false);
        animation_linear_->AddAnimator(1, fade_show, 0);

        animation_linear_->Subscribe(EventStoryboardStarted, MakeDelegate([this]()->bool {
            if (slogan_) slogan_->SetVisible(true);
            return true;
        }));

        animation_linear_->Subscribe(EventStoryboardEnd, MakeDelegate([this]()->bool {
            //if (slogan_) slogan_->SetVisible(true);
            return true;
        }));

        animation_linear_->Start();
    }
}

LRESULT CWndSetup::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_MSG_INSTALL)
		return OnMsgInstall(wParam, lParam);
	if (uMsg == WM_MSG_TIP)
		return OnMsgTip(wParam, lParam);
	if (uMsg == WM_MSG_CLOSE)
	{
		CDuiString str;
		str.Format(L"%s���ڰ�װ�У�ȷ��Ҫ�˳���װ��", SOFT_NAME);
		CWndMsgbox2* pWnd = new CWndMsgbox2(str.GetData());
		pWnd->Create(m_hWnd);
		pWnd->CenterWindow();
		if (IDYES == pWnd->ShowModal() && CSetupModule::Instance()->GetInsState() == StepInstall)
		{
			CSetupModule::Instance()->StopSetup();
			Close();
		}
		return 0;
	}
	return CWndBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CWndSetup::OnMsgInstall(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case Mw_Init://��ʼ��
		m_pProgress->SetValue(0);
		break;
	case Mw_Unzip://��ѹ��
		{
			int nPercent = (int)lParam;
			m_pProgress->SetValue(lParam);
		}
		break;
	case Mw_Regedit:
		m_pProgress->SetValue(100);
		break;
	case Mw_Fail://��װʧ��
		if (m_pLblStatus) m_pLblStatus->SetText(L"��װʧ�ܣ�������...");
        if (m_pLblStatus) m_pLblStatus->SetVisible(true);
        if (btn_install_restart_) btn_install_restart_->SetVisible(true);
        if (btn_install_close_) btn_install_close_->SetVisible(true);
        if (m_pProgress) m_pProgress->SetValue(0);
		break;
	case Mw_Finish://��װ���
		m_pTabMain->SelectItem(3);
		CreateDataTask(EVENT_INSTALL);
        CSetupModule::Instance()->SetShowUpdateWnd(true);
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CWndSetup::OnMsgTip(WPARAM wParam, LPARAM lParam)
{
	wstring strMsg;
	switch (wParam)
	{
	case ERR_CREATE_DIR://�����ļ���ʧ��
		strMsg = L"������װĿ¼ʧ��";
		break;
	case ERR_NO_SPACE://�ռ䲻��
		strMsg = L"���̿ռ䲻�㣬������ѡ��װĿ¼";
		break;
	case ERR_EMPTY:
		strMsg = L"��ѡ��װĿ¼";
		break;
	default:
		break;
	}
	if (strMsg.empty())
		return 0;
	CWndMsgbox *pMsgWnd = new CWndMsgbox(strMsg);
	pMsgWnd->Create(m_hWnd);
	pMsgWnd->CenterWindow();
	pMsgWnd->ShowModal();
	return 0;
}

bool CWndSetup::OnBtnCreate(void* lpParam) {
    TNotifyUI* pNotify = (TNotifyUI*)lpParam;
    if (pNotify->sType == DUI_MSGTYPE_CLICK) {
        m_pTabMain->SelectItem(1);
    }
    return true;
}

bool CWndSetup::OnNotifyBtnClose(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		if (CSetupModule::Instance()->GetInsState() == StepInstall)
		{
			::PostMessage(m_hWnd, WM_MSG_CLOSE, 0, 0);
			return true;
		}
		if (CSetupModule::Instance()->GetInsState() == StepFinish)
		{
			BootRun();
		}
		Close();
	}
	return true;
}

bool CWndSetup::OnNotifyBtnExit(void* lpParam) {
    TNotifyUI* pNotify = (TNotifyUI*)lpParam;
    if (pNotify && pNotify->sType == DUI_MSGTYPE_CLICK){
        Close();
    }
    return true;
}

bool CWndSetup::OnNotifyBtnCancel(void* lpParam) {
    TNotifyUI* pNotify = (TNotifyUI*)lpParam;
    if (pNotify && pNotify->sType == DUI_MSGTYPE_CLICK){
        if (m_pTabMain) m_pTabMain->SelectItem(0);
    }
    return true;
}

bool CWndSetup::OnNotifyBtnMin(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
	return true;
}

bool CWndSetup::OnNotifyBtnInstall(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		wstring strPath(m_pEditPath->GetText().GetData());
		if (strPath.empty())
		{
			::PostMessage(m_hWnd, WM_MSG_TIP, ERR_EMPTY, 0);
			return true;
		}
		SHCreateDirectory(NULL, strPath.c_str());
		if (!PathFileExists(strPath.c_str()))
		{
			//�����ļ��д���ʧ��
			::PostMessage(m_hWnd, WM_MSG_TIP, ERR_CREATE_DIR, 0);
			return true;
		}
		//�����̿ռ�
		__int64 nFreeSpace = GetInstallFreeSpace(strPath);
		if (nFreeSpace < SPACE_NEED)
		{
			::PostMessage(m_hWnd, WM_MSG_TIP, ERR_NO_SPACE, 0);
			return true;
		}
		if(strPath[strPath.size() - 1] != L'\\') 
			strPath.append(L"\\");
		int nPos = strPath.find_last_of(SOFT_ROOT_DIR);
		if ((nPos == wstring::npos ) || (nPos+2 != strPath.size()))
		{
			strPath.append(SOFT_ROOT_DIR);
			strPath.append(L"\\");
			SHCreateDirectory(NULL, strPath.c_str());
		}
		m_pTabMain->SelectItem(2);
		//��ʼ��װ
		CSetupModule::Instance()->SetInstallPath(strPath);
		CSetupModule::Instance()->SetMsgWnd(m_hWnd, WM_MSG_INSTALL);
		CSetupModule::Instance()->StartSetup();
	}
	return true;
}

bool CWndSetup::OnNotifyBtnSelf(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		m_pTabMain->SelectItem(1);
	}
	return true;
}

bool CWndSetup::OnNotifyBtnSelect(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		wstring strPath;
		if (ShowBrowserFolderDlg(m_pm.GetPaintWindow(), strPath))
		{
			strPath.append(SOFT_ROOT_DIR);
			strPath.append(L"\\");
			m_pEditPath->SetText(strPath.c_str());
		}
	}
	return true;
}

bool CWndSetup::OnNotifyBtnRun(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		wstring strPath = CSetupModule::Instance()->GetInstallPath() + SOFT_ROOT_DIR + L"\\" + EXE_MAIN;
		ShellExecute(NULL, L"open", strPath.c_str(), NULL, NULL, SW_SHOW);
		BootRun();
		Close();
	}
	return true;
}

bool CWndSetup::OnClickInstallRestart(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (pNotify && pNotify->sType == DUI_MSGTYPE_CLICK) {
        if (m_pLblStatus) m_pLblStatus->SetText(L"");
        if (m_pLblStatus) m_pLblStatus->SetVisible(false);
        if (btn_install_restart_) btn_install_restart_->SetVisible(false);
        if (btn_install_close_) btn_install_close_->SetVisible(false);
        if (m_pProgress) m_pProgress->SetValue(0);
        CSetupModule::Instance()->StartSetup();
    }
    return true;
}

bool CWndSetup::OnClickInstallClose(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (pNotify && pNotify->sType == DUI_MSGTYPE_CLICK) {
        if (m_pLblStatus) m_pLblStatus->SetText(L"");
        if (m_pLblStatus) m_pLblStatus->SetVisible(false);
        if (btn_install_restart_) btn_install_restart_->SetVisible(false);
        if (btn_install_close_) btn_install_close_->SetVisible(false);
        if (m_pProgress) m_pProgress->SetValue(0);
        m_pTabMain->SelectItem(1);
    }

    return true;
}

wstring CWndSetup::GetInstallPath(bool& bInstallOld)
{
	wstring strPath = CSetupModule::Instance()->GetOldInstallPath();
	if (PathFileExists(strPath.c_str()))
	{
		if (strPath[strPath.size() - 1] != '\\')
			strPath.append(L"\\");
		bInstallOld = true;
		return strPath;
	}
	wstring strProgram = GetProgramFilePath(false);
	strProgram.append(L"\\");
	strProgram.append(SOFT_ROOT_DIR);
	strProgram.append(L"\\");
	bInstallOld = false;
	return strProgram;
}

__int64 CWndSetup::GetInstallFreeSpace(const wstring& strPath)
{
	ULARGE_INTEGER uiAvalaible, uiTotal, uiFree;
	GetDiskFreeSpaceEx(strPath.c_str(), &uiAvalaible, &uiTotal, &uiFree);
	return (__int64)uiFree.QuadPart;
}

void CWndSetup::BootRun()
{
	wstring strPath = CSetupModule::Instance()->GetInstallPath() + EXE_MAIN;
	if (m_pOptBootrun && m_pOptBootrun->IsSelected())
	{
		::ShowWindow(m_hWnd, SW_HIDE);
		wstring strCmd(L"\"");
		strCmd += strPath;
		strCmd.append(L"\"");
		strCmd.append(L" /bootrun");
		AddBootRun(REG_ROOT_NAME, strCmd);
	}
}
