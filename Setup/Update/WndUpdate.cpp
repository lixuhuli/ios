#include "StdAfx.h"
#include "WndUpdate.h"
#include "UpdateModule.h"
#include "Config.h"
#include <Tlhelp32.h>

enum {
	WM_MSG_BASE = WM_USER + 611,
	WM_MSG_UPDATE,
};

CWndUpdate::CWndUpdate(bool bNeedCheck)
: m_pTabMain(NULL)
, m_pLblStatus(NULL)
, m_pProgress(NULL)
, slogan_(nullptr)
, btn_update_restart_(nullptr)
, btn_update_close_(nullptr)
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
	}
    else
    {
        OUTPUT_XYLOG(LEVEL_INFO, L"切换到升级页面");
        SwitchToUpdate();
    }
}

void CWndUpdate::SwitchToUpdate() {
    if (CUpdateModule::Instance()->GetUpdateStatus() == 1) {
        OUTPUT_XYLOG(LEVEL_INFO, L"进入强制升级流程");

        m_pLblStatus->SetText(L"更新中 %0");
        btn_update_restart_->SetVisible(false);
        btn_update_close_->SetVisible(false);
        m_pProgress->SetValue(0);

        CUpdateModule::Instance()->StartUpdate();
    }
}

LRESULT CWndUpdate::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_MSG_UPDATE)
		return OnMsgUpdate(wParam, lParam);
	return CWndBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CWndUpdate::OnMsgUpdate(WPARAM wParam, LPARAM lParam) {
    if (!m_pLblStatus || !m_pTabMain || !btn_update_restart_ || !btn_update_close_ || !m_pProgress) return 0;

	switch (wParam)
	{
	case Step_Check://检测更新
		{
			m_pTabMain->SelectItem(0);

            switch (lParam) {
            case 0: {
                m_pLblStatus->SetText(L"更新失败请重试...");
                btn_update_restart_->SetVisible(true);
                btn_update_close_->SetVisible(true);
            }
            break;
            case 1: Close(); break;
            case 2: {
                m_pLblStatus->SetText(L"恭喜，您当前为最新版本！");
                btn_update_restart_->SetVisible(false);
                btn_update_close_->SetVisible(true);
                m_pProgress->SetValue(100);
            }
            break;
            case 3: SwitchToUpdate(); break;  
            default: break;
            }
		}
		break;
	case Step_Init://初始化中
		{
			m_pTabMain->SelectItem(0);
			HWND hWnd = FindWindow(GUI_CLASS_NAME, L"果仁模拟器");
			if (IsWindow(hWnd))
				::PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;
	case Step_Load://下载中
		{
			int nPercent = (int)lParam;
			if (nPercent == -1) {
				m_pLblStatus->SetText(L"更新失败请重试...");
                btn_update_restart_->SetVisible(true);
                btn_update_close_->SetVisible(true);
				break;
			}

            nPercent = (int)((float)nPercent / (float)2.0 + 0.5);

            CStdString str_progress;
            str_progress.Format(L"更新中 %d%s", nPercent, "%");
			m_pLblStatus->SetText(str_progress.GetData());
			m_pProgress->SetValue(nPercent);
		}
		break;
	case Step_Install://安装中
		{
			int nPercent = (int)lParam;
			if (nPercent == -1)
			{
				m_pLblStatus->SetText(L"更新文件失败，执行回滚...");
				break;
			}

            nPercent = (int)((float)nPercent / (float)2.0 + 0.5) + 50;
            if (nPercent > 100) nPercent = 100;

            CStdString str_progress;
            str_progress.Format(nPercent == 100 ? L"更新成功 %d%s" : L"更新中 %d%s", nPercent, "%");
            m_pLblStatus->SetText(str_progress.GetData());
            m_pProgress->SetValue(nPercent);
		}
		break;
	case Step_Rollback://回滚中
		{
			int nPercent = (int)lParam;
			m_pLblStatus->SetText(L"正在回滚...");
			m_pProgress->SetValue(nPercent);

			if (nPercent <= 0) {
                m_pLblStatus->SetText(L"更新失败请重试...");
                btn_update_restart_->SetVisible(true);
                btn_update_close_->SetVisible(true);
			}
		}
		break;
	case Step_Regedit://更新注册表
		break;
	case Step_Finish://更新完成
		m_pTabMain->SelectItem(1);
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

bool CWndUpdate::OnNotifyBtnRun(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK) {
        wstring strExe = CUpdateModule::Instance()->GetInstallPath() + EXE_MAIN;
        CreateProcessByUser(strExe);
		Close();
	}
	return true;
}

bool CWndUpdate::OnNotifyBtnExit(void* lpParam) {
    TNotifyUI* pNotify = (TNotifyUI*)lpParam;
    if (pNotify && pNotify->sType == DUI_MSGTYPE_CLICK){
        Close();
    }
    return true;
}

bool CWndUpdate::OnClickUpdateRestart(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (pNotify && pNotify->sType == DUI_MSGTYPE_CLICK) {
        if (m_pLblStatus) m_pLblStatus->SetText(L"正在检测更新……");
        if (btn_update_restart_) btn_update_restart_->SetVisible(false);
        if (btn_update_close_) btn_update_close_->SetVisible(false);
        if (m_pProgress) m_pProgress->SetValue(0);
        
        if (m_bNeedCheck) {
            OUTPUT_XYLOG(LEVEL_INFO, L"检测是否需要升级");
            CUpdateModule::Instance()->StartCheckUpdate();
        }
        else
        {
            OUTPUT_XYLOG(LEVEL_INFO, L"切换到升级页面");
            SwitchToUpdate();
        }
    }
    return true;
}

bool CWndUpdate::OnClickUpdateClose(void* param) {
    TNotifyUI* pNotify = (TNotifyUI*)param;
    if (pNotify && pNotify->sType == DUI_MSGTYPE_CLICK) {
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

LPCTSTR CWndUpdate::CreateProcessByUser(const std::wstring& appCmd) {
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

ULONG CWndUpdate::CreateProcessEx2(LUID AuthenticationId, PCWSTR lpApplicationName, PWSTR lpCommandLine) {
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

ULONG CWndUpdate::CreateProcessEx1(HANDLE hProcess, PCWSTR lpApplicationName, PWSTR lpCommandLine) {
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
