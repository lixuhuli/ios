#include "StdAfx.h"
#include "WndSetup.h"
#include "SetupModule.h"
#include "WndMsgbox.h"
#include "RegBoot.h"
#include <DataCenter\DataCenterApi.h>
#include <Tlhelp32.h>

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

	// 设置默认安装路径
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
		str.Format(L"%s正在安装中，确定要退出安装吗？", SOFT_NAME);
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
	case Mw_Init://初始化
		m_pProgress->SetValue(0);
		break;
	case Mw_Unzip://解压中
		{
			int nPercent = (int)lParam;
			m_pProgress->SetValue(lParam);
		}
		break;
	case Mw_Regedit:
		m_pProgress->SetValue(100);
		break;
	case Mw_Fail://安装失败
		if (m_pLblStatus) m_pLblStatus->SetText(L"安装失败，请重试...");
        if (m_pLblStatus) m_pLblStatus->SetVisible(true);
        if (btn_install_restart_) btn_install_restart_->SetVisible(true);
        if (btn_install_close_) btn_install_close_->SetVisible(true);
        if (m_pProgress) m_pProgress->SetValue(0);
		break;
	case Mw_Finish://安装完成
		m_pTabMain->SelectItem(3);
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
	case ERR_CREATE_DIR://创建文件夹失败
		strMsg = L"创建安装目录失败";
		break;
	case ERR_NO_SPACE://空间不足
		strMsg = L"磁盘空间不足，请重新选择安装目录";
		break;
	case ERR_EMPTY:
		strMsg = L"请选择安装目录";
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
			//提醒文件夹创建失败
			::PostMessage(m_hWnd, WM_MSG_TIP, ERR_CREATE_DIR, 0);
			return true;
		}
		//检查磁盘空间
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
		//开始安装
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

        CreateProcessByUser(strPath);

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

LPCTSTR CWndSetup::CreateProcessByUser(const std::wstring& appCmd) {
    PWSTR lpApplicationName = nullptr;

    std::wstring s = appCmd;

    PWSTR lpCommandLine = (PWSTR)s.c_str();

    wchar_t errStr[MAX_PATH] ;

    HANDLE hToken;

    ULONG err = NOERROR;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        union {
            TOKEN_ELEVATION_TYPE tet;
            TOKEN_LINKED_TOKEN tlt;
        };

        ULONG rcb;

        if (GetTokenInformation(hToken, TokenElevationType, &tet, sizeof(tet), &rcb))
        {
            if (tet == TokenElevationTypeFull)
            {
                if (GetTokenInformation(hToken, TokenLinkedToken, &tlt, sizeof(tlt), &rcb))
                {
                    TOKEN_STATISTICS ts;

                    BOOL fOk = GetTokenInformation(tlt.LinkedToken, TokenStatistics, &ts, sizeof(ts), &rcb);

                    CloseHandle(tlt.LinkedToken);

                    if (fOk)
                    {
                        err = CreateProcessEx2(ts.AuthenticationId,
                            lpApplicationName,
                            lpCommandLine);
                    }
                    else
                    {
                        err = GetLastError();
                    }
                }
                else
                {
                    err = GetLastError();
                }
            }
            else
            {
                err = ERROR_ALREADY_ASSIGNED;
                PublicLib::ShellExecuteRunas(lpCommandLine, nullptr, nullptr);
            }
        }
        else
        {
            err = GetLastError();
        }

        CloseHandle(hToken);
    }
    else
    {
        err = GetLastError();
    }

    wsprintf(errStr, L"%lld", err);
    std::wstring errCode(errStr);
    return errCode.c_str();
}

ULONG CWndSetup::CreateProcessEx2(LUID AuthenticationId, PCWSTR lpApplicationName, PWSTR lpCommandLine) {
    ULONG err = ERROR_NOT_FOUND;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32W pe = { sizeof(pe) };

        ULONG rcb;

        if (Process32First(hSnapshot, &pe))
        {
            err = ERROR_NOT_FOUND;
            BOOL found = FALSE;

            do
            {
                if (pe.th32ProcessID && pe.th32ParentProcessID)
                {
                    if (HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_CREATE_PROCESS, FALSE, pe.th32ProcessID))
                    {
                        HANDLE hToken;

                        if (OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
                        {
                            TOKEN_STATISTICS ts;

                            if (GetTokenInformation(hToken, TokenStatistics, &ts, sizeof(ts), &rcb))
                            {
                                if (ts.AuthenticationId.LowPart == AuthenticationId.LowPart &&
                                    ts.AuthenticationId.HighPart == AuthenticationId.HighPart)
                                {
                                    found = TRUE;

                                    err = CreateProcessEx1(hProcess,
                                        lpApplicationName,
                                        lpCommandLine);
                                }
                            }
                            CloseHandle(hToken);
                        }

                        CloseHandle(hProcess);
                    }
                }

            } while (!found && Process32Next(hSnapshot, &pe));
        }
        else
        {
            err = GetLastError();
        }

        CloseHandle(hSnapshot);
    }
    else
    {
        err = GetLastError();
    }

    return err;
}

ULONG CWndSetup::CreateProcessEx1(HANDLE hProcess, PCWSTR lpApplicationName, PWSTR lpCommandLine) {
    SIZE_T Size = 0;

    STARTUPINFOEX si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    InitializeProcThreadAttributeList(0, 1, 0, &Size);

    ULONG err = GetLastError();

    if (err = ERROR_INSUFFICIENT_BUFFER)
    {
        si.lpAttributeList = (PPROC_THREAD_ATTRIBUTE_LIST)alloca(Size);

        if (InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &Size))
        {
            if (UpdateProcThreadAttribute(si.lpAttributeList, 0,
                PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &hProcess, sizeof(hProcess), 0, 0) &&
                CreateProcessW(lpApplicationName, lpCommandLine, 0, 0, 0,
                EXTENDED_STARTUPINFO_PRESENT, 0, 0, &si.StartupInfo, &pi))
            {
                CloseHandle(pi.hThread);
                CloseHandle(pi.hProcess);
            }
            else
            {
                err = GetLastError();
            }

            DeleteProcThreadAttributeList(si.lpAttributeList);
        }
        else
        {
            err = GetLastError();
        }
    }
    else
    {
        err = GetLastError();
    }

    return err;
}
