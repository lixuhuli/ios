#include "stdafx.h"
#include "UpdateModule.h"
#include "Config.h"
#include "GlobalData.h"
#include "Regedit.h"
#include "Update.h"


BOOL CUpdateModule::s_bExit = FALSE;

CUpdateModule::CUpdateModule()
	: m_hThread(NULL)
	, m_hMsgWnd(NULL)
	, m_hCheckThread(NULL)
	, m_uMsg(0)
	, m_nUpdateStatus(0)
{

}

CUpdateModule::~CUpdateModule()
{
	PublicLib::SafeKillThread(&m_hThread, 0);
	PublicLib::SafeKillThread(&m_hCheckThread, 0);
	ReleaseReplaceList();
}

void CUpdateModule::SetMsg(HWND hWnd, UINT uMsg)
{
	m_hMsgWnd = hWnd;
	m_uMsg = uMsg;
}

void CUpdateModule::StartUpdate()
{
	m_strInstallPath = CGlobalData::Instance()->GetOldInstallPath();
	int nSize = m_strInstallPath.size();
	if (nSize < 1)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"没有找到安装位置");
		return;
	}
	if (m_strInstallPath[nSize-1] != '\\')
		m_strInstallPath.append(L"\\");
	if (m_hMsgWnd)
		::PostMessage(m_hMsgWnd, m_uMsg, Step_Init, 0);
	m_hThread = (HANDLE) _beginthreadex(NULL, 0, UpdateThread, this, 0, NULL);
}

void CUpdateModule::StopUpdate()
{
	s_bExit = TRUE;
	PublicLib::SafeKillThread(&m_hThread, 3000);
	PublicLib::SafeKillThread(&m_hCheckThread, 100);
}

UINT __stdcall CUpdateModule::UpdateThread( void* lpParam )
{
	CUpdateModule* pModule = (CUpdateModule*)lpParam;
	CoInitialize(NULL);
	wstring strError;
	LPARAM lParam = 1;
	if (!CConfig::Instance()->ParseFileList(&s_bExit))
	{
		if (s_bExit)
		{
			OUTPUT_XYLOG(LEVEL_INFO, L"用户主动退出升级");
			return 0;
		}
		OUTPUT_XYLOG(LEVEL_ERROR, L"解析更新文件列表失败");
		strError = L"解析更新文件列表失败";
		lParam = 0;
	}
	else
	{
		pModule->m_strTempPath = GetPatchPath() + L"UpdateTemp\\";
		SHCreateDirectory(NULL, pModule->m_strTempPath.c_str());
		bool bRet = pModule->Update(strError);
		if (s_bExit)
		{
			OUTPUT_XYLOG(LEVEL_INFO, L"用户主动退出升级");
			return 0;
		}
		if (!bRet)
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"更新失败，错误信息：%s", strError.c_str());
			lParam = 0;
		}
		PublicLib::RemoveDir(pModule->m_strTempPath.c_str());
	}
	if (s_bExit) return true;
	PublicLib::RefreshIcons();
	CoUninitialize();
	return 0;
}

bool CUpdateModule::Update(wstring& strError)
{
	CConfig::Instance()->ParseConflictList();
	if (s_bExit)
		return true;
	if (CConfig::Instance()->GetUpdateItemList().empty())
	{
		if (m_hMsgWnd)
			::PostMessage(m_hMsgWnd, m_uMsg, Step_Finish, 0);
		return true;
	}
	SHCreateDirectory(NULL, m_strInstallPath.c_str());
	if (!DownloadPackagePart(strError))
	{
		if (m_hMsgWnd)
			::PostMessage(m_hMsgWnd, m_uMsg, Step_Load, -1);
		OUTPUT_XYLOG(LEVEL_ERROR, L"下载更新差异化文件失败，错误信息：%s", strError.c_str());
		return false;
	}
	if (s_bExit)
		return true;
	CConfig::Instance()->KillConflict();
	if (s_bExit)
		return true;
	if (!ReplaceFiles(strError))
	{//替换文件出错，执行回滚
		if (m_hMsgWnd)
			::PostMessage(m_hMsgWnd, m_uMsg, Step_Install, -1);
		Sleep(1000);
		OUTPUT_XYLOG(LEVEL_ERROR, L"替换文件失败，执行回滚，错误信息：%s，系统错误码：%u", strError.c_str(), GetLastError());
		Rollback(strError);
		return false;
	}
	if (s_bExit)
		return true;
	if (!UpdateRegedit(strError))
	{
		if (m_hMsgWnd)
			::PostMessage(m_hMsgWnd, m_uMsg, Step_Regedit, -1);
		OUTPUT_XYLOG(LEVEL_ERROR, L"更新注册表失败，错误信息：%s，系统错误码：%u", strError.c_str(), GetLastError());
		return false;
	}
	if (m_hMsgWnd)
		::PostMessage(m_hMsgWnd, m_uMsg, Step_Finish, 0);
	return true;
}

bool CUpdateModule::DownloadPackagePart(wstring& strError)
{
	list<LPUPDATE_FILE_ITEM>& updateList = CConfig::Instance()->GetUpdateItemList();
	if (updateList.empty())
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"更新文件列表为空");
		strError = L"更新文件列表为空";
		return false;
	}
	if (m_hMsgWnd)
		::PostMessage(m_hMsgWnd, m_uMsg, Step_Load, 0);
	wstring strZipDir	= m_strTempPath + L"Zip\\";
	CreateDirectory(strZipDir.c_str(), NULL);
	wstring strUnzipRootDir = m_strTempPath + L"Unzip\\";
	CreateDirectory(strUnzipRootDir.c_str(), NULL);
	wstring strBackupDir = m_strTempPath + L"Backup\\";
	CreateDirectory(strBackupDir.c_str(), NULL);
	if (s_bExit) return true;
	PublicLib::CHttpClient http;
	http.SetStopCallback(this);
	wstring strUrl, strZipFile, strUnzipFile, strBackupFile, strUnzipDir;
	string strMD5;
	float fCount = (float)updateList.size();
	wstring strRootDir = m_strRootUrl + CConfig::Instance()->GetZipListDir() + L"/";
	list<LPUPDATE_FILE_ITEM>::iterator itor = updateList.begin();
	DeCompressParams params;
	params.lpNeedExit = &s_bExit;
	for ( int i=1 ; itor != updateList.end(); ++ itor, ++i )
	{
		if (s_bExit)
			return true;
		strUrl	= strRootDir + (*itor)->strZipName;
		strZipFile = strZipDir + (*itor)->strZipName;
		if (!http.DownloadFileForRetry(strUrl.c_str(), strZipFile.c_str()))
		{
			strError = L"下载文件失败，文件URL：" + strUrl;
			return false;
		}
		if (s_bExit) 
			return true;
		//解压7Z文件
		strUnzipDir = strUnzipRootDir + (*itor)->strDir;
		SHCreateDirectory(NULL, strUnzipDir.c_str());
		if (!PublicLib::DeCompressFile(strZipFile, strUnzipDir, params))
		{
			strError = params.strError;
			OUTPUT_XYLOG(LEVEL_ERROR, L"解压失败，错误信息：%s", strError.c_str());
			return false;
		}
		if (s_bExit)
			return true;
		//校验解压后的文件
		strUnzipDir.append(L"\\");
		strUnzipFile = strUnzipDir + (*itor)->strName;
		strMD5 = PublicLib::CalFileMd5(strUnzipFile);
		if (_stricmp(strMD5.c_str(), (*itor)->strMD5.c_str()) != 0)
		{
			strError = L"校验解压文件失败，文件URL：" + strUrl + L", strMD5 = " + PublicLib::Utf8ToU(strMD5) + L", config list strMD5 = " + PublicLib::Utf8ToU((*itor)->strMD5);
			return false;
		}
		if (s_bExit) return true;
		//做备份
		if ((*itor)->bReplace)
		{
			wstring& strDir = (*itor)->strDir;
			strBackupFile = strBackupDir + strDir;
			if ( !strDir.empty() )
				SHCreateDirectory(NULL, strBackupFile.c_str());
			strBackupFile.append(L"\\");
			strBackupFile += (*itor)->strName;
			wstring strOldFile = m_strInstallPath + L"\\" + (*itor)->strShortPath;
			if ( PathFileExists(strOldFile.c_str()) && !CopyFile(strOldFile.c_str(), strBackupFile.c_str(), FALSE) )
			{
				strError = L"备份文件失败， 文件路径：" + strOldFile;
				return false;
			}
		}
		if (s_bExit) return true;
		//加入替换列表
		LPREPLACE_FILE_ITEM lpItem = new REPLACE_FILE_ITEM;
		lpItem->strResPath	= strUnzipFile;
		lpItem->strDesPath	= (*itor)->strShortPath;
		if ( (*itor)->bReplace )
			lpItem->strBackupPath= strBackupFile;
		lpItem->strDir		= (*itor)->strDir;
		lpItem->bReplace	= false;
		m_ReplaceList.push_back(lpItem);
		if (s_bExit) return true;
		if (m_hMsgWnd)
			::PostMessage(m_hMsgWnd, m_uMsg, Step_Load, (LPARAM)(100 * (i / fCount)));
	}
	if (m_hMsgWnd)
		::PostMessage(m_hMsgWnd, m_uMsg, Step_Load, (LPARAM)100);
	Sleep(100);
	return true;
}

bool CUpdateModule::ReplaceFiles(wstring& strError)
{
	if (m_hMsgWnd)
	{
		::PostMessage(m_hMsgWnd, m_uMsg, Step_Install, 0);
		Sleep(100);
	}
	if (s_bExit) return true;
	float fCount = (float)m_ReplaceList.size();
	wstring strDirPath, strDesPath;
	list<LPREPLACE_FILE_ITEM>::iterator itor = m_ReplaceList.begin();
	for ( int i=1; itor != m_ReplaceList.end(); ++itor,++i )
	{
		if (s_bExit) return true;
		strDirPath = m_strInstallPath + L"\\" + (*itor)->strDir;
		SHCreateDirectory(NULL, strDirPath.c_str());
		strDesPath = m_strInstallPath + L"\\" + (*itor)->strDesPath;
		if (!CopyFile((*itor)->strResPath.c_str(), strDesPath.c_str(), FALSE))
		{
			strError = L"替换文件失败，文件路径：" + strDesPath;
			Sleep(1000);
			return false;
		}
		(*itor)->bReplace = true;
		if ( m_hMsgWnd )
			::PostMessage(m_hMsgWnd, m_uMsg, Step_Install, (LPARAM)(100 * i / fCount));
		if ( s_bExit ) return true;
		Sleep(100);
	}
	if (m_hMsgWnd)
		::PostMessage(m_hMsgWnd, m_uMsg, Step_Install, (LPARAM)100);
	Sleep(100);
	return true;
}

void CUpdateModule::OnHttpLoading( const DWORD dwTotalSize, const DWORD dwLoadSize, void* lpParam)
{
	HWND hWnd = (HWND)lpParam;
	if (hWnd && dwTotalSize>0)
	{
		int nPercent = (int)(100*(dwLoadSize/float(dwTotalSize)));
		::PostMessage(hWnd, m_uMsg, 0, nPercent);
	}
}

BOOL CUpdateModule::IsNeedStop()
{
	return s_bExit;
}

bool CUpdateModule::UpdateRegedit(wstring& strError)
{
	INSTALL_INFO info;
	info.strKeyName = REG_ROOT_NAME;
	info.strExePath = m_strInstallPath + EXE_MAIN;
	info.strDisplayName = SHORTCUT_MAIN_NAME;
	info.strVersion = m_strUpdateVersion;
	info.strPublisher = REG_COMPANY;
	info.strUninstPath = m_strInstallPath + EXE_UNINSTALL;
	info.strUrl = REG_URL;
	info.strChannel	= CGlobalData::Instance()->GetChannel();
	info.dwAutoRun = 1;
	bool bRet = RegWriteUninstInfo(info);
	if (!bRet)
	{
		strError = L"写入卸载信息注册表位置失败";
		OUTPUT_XYLOG(LEVEL_ERROR, L"写入卸载信息注册表失败，系统错误码：%u", GetLastError());
	}
	if (!RegWriteList(REG_ROOT_NAME, CConfig::Instance()->GetConflictList(), RegConflictList))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"写入冲突列表注册表失败，系统错误码：%u", GetLastError());
	}
	//修复卸载快捷方式
	//UpdateShortcuts();
	return bRet;
}

void CUpdateModule::ReleaseReplaceList()
{
	list<LPREPLACE_FILE_ITEM>::iterator itor = m_ReplaceList.begin();
	for ( ; itor != m_ReplaceList.end(); ++itor )
	{
		LPREPLACE_FILE_ITEM lpItem = *itor;
		delete lpItem;
	}
	m_ReplaceList.clear();
}


void CUpdateModule::UpdateShortcuts()
{
	WCHAR szPath[MAX_PATH+1] = {0};
	if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_PROGRAMS, FALSE))
	{
		wcscat_s(szPath, L"\\");
		wcscat_s(szPath, SOFT_NAME);
		CreateDirectory(szPath, NULL);
		wcscat_s(szPath, L"\\");
		wstring strProPath(szPath);
		strProPath.append(L"卸载");
		strProPath.append(SOFT_NAME);
		strProPath.append(L".lnk");
		CComPtr<IShellLink> pLink;
		HRESULT hr = pLink.CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER);   
		if ( FAILED(hr) ) 
			return ;
		CComPtr<IPersistFile> pFile;
		hr = pLink->QueryInterface(IID_IPersistFile,(void**)&pFile);  
		if ( FAILED(hr) ) 
			return ;
		hr = pFile->Load(strProPath.c_str(), STGM_READ);
		if ( FAILED(hr) ) 
			return ;
		wchar_t szArg[MAX_PATH];
		if ( FAILED(pLink->GetArguments(szArg, MAX_PATH)))
			return ;
		wstring strArg(szArg);
		if (strArg.find(L"/unsetup") != wstring::npos)
			return ;
		PublicLib::CShortcutHelper shUninst;
		wstring strMainExePath = m_strInstallPath + EXE_UNINSTALL;
		shUninst.SetResPath(strMainExePath.c_str());
		wstring strIcoPath = m_strInstallPath + L"Unsetup.ico";
		shUninst.SetIconPath(strIcoPath.c_str());
		shUninst.SetArguments(L"/unsetup");
		shUninst.CreateFileLink(strProPath.c_str());
	}
}

bool CUpdateModule::Rollback( wstring& strError )
{
	list<LPREPLACE_FILE_ITEM>::iterator itor = m_ReplaceList.begin();
	float fCount = (float)m_ReplaceList.size();
	wstring strDesPath;
	for ( int i=1; itor != m_ReplaceList.end(); ++itor,++i )
	{
		if ((*itor)->bReplace)
		{
			strDesPath = m_strInstallPath + L"\\" + (*itor)->strResPath;
			wstring& strBackupPath = (*itor)->strBackupPath;
			if (strBackupPath.empty())
				DeleteFile(strDesPath.c_str());
			else
				CopyFile(strBackupPath.c_str(), strDesPath.c_str(), FALSE);
		}
		if (m_hMsgWnd)
			::PostMessage(m_hMsgWnd, m_uMsg, Step_Rollback, (LPARAM)(100 - 100 * i / fCount));
		Sleep(100);
	}
	return true;
}

UINT __stdcall CUpdateModule::CheckUpdateThread(void* lpParam)
{
	wstring strError;
	int nRet = CheckUpdate(strError);
	CUpdateModule* pModule = (CUpdateModule*)lpParam;
	if (pModule->m_hMsgWnd)
		PostMessage(pModule->m_hMsgWnd, pModule->m_uMsg, Step_Check, nRet);
	return 0;
}

void CUpdateModule::StartCheckUpdate()
{
	m_hCheckThread = (HANDLE)_beginthreadex(NULL, 0, CheckUpdateThread, this, 0, NULL);
}



