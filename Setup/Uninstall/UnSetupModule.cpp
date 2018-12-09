#include "StdAfx.h"
#include "UnSetupModule.h"
#include <process.h>
#include <PublicLib/System.h>
#include <PublicLib/Shortcut.h>
#include <PublicLib/StrHelper.h>
#include <PublicLib/FileHelper.h>
#include <AppHelper.h>
using namespace PublicLib;
#include "Regedit.h"
#include <RegBoot.h>


CUnSetupModule::CUnSetupModule(void)
	: m_hThread(NULL)
	, m_hMsgWnd(NULL)
	, m_uMsg(0)
{

}

CUnSetupModule::~CUnSetupModule(void)
{
	SafeKillThread(&m_hThread, 0);
}

void CUnSetupModule::SetMsg(HWND hWnd, UINT uMsg)
{
	m_hMsgWnd = hWnd;
	m_uMsg = uMsg;
}

bool CUnSetupModule::GetInstallInfo()
{
	wstring strOldPath, strOldVersion, strOldUpVersion, strChannel;
	if (RegGetInstallInfo(REG_ROOT_NAME, strOldPath, strOldVersion, strOldUpVersion, strChannel))
	{
		if (PathFileExists(strOldPath.c_str()) && !strOldVersion.empty())
		{
			m_strInstallPath = strOldPath;
			m_strChannelName = strChannel;
			m_strVersion = strOldVersion;
			return true;
		}
	}
	return false;
}

bool CUnSetupModule::StartUnSetup()
{
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, UninstThread, this, 0, NULL);
	return m_hThread != NULL;
}

UINT WINAPI CUnSetupModule::UninstThread(void* lpParam)
{
	CoInitialize(NULL);
	CUnSetupModule* pModule = (CUnSetupModule*)lpParam;
	wstring strError;
	pModule->UnSetup(strError);
	CoUninitialize();
	return 0;
}

void CUnSetupModule::ClearShortcut()
{
	if (IsOsOverVista())
	{
		UnpinToWin7TaskBar(m_strInstallPath.c_str(), EXE_MAIN);
	}
	CShortcutHelper::RemoveShortcutAtSpecialPath(SHORTCUT_MAIN_NAME);
	CShortcutHelper::RemoveShortcutAtSpecialPath(SHORTCUT_MAIN_NAME, CSIDL_STARTMENU);
	WCHAR szPath[MAX_PATH+1]={0};
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_PROGRAMS, FALSE);
	wcscat_s(szPath, L"\\");
	wcscat_s(szPath, SHORTCUT_MAIN_NAME);
	RemoveDir(szPath);
}

void CUnSetupModule::RemoveFiles()
{
	if (m_strInstallPath.empty() || !PathFileExists(m_strInstallPath.c_str()))
		return ;
	EnumFiles(m_strInstallPath.c_str());
	size_t nCount = m_vecDirs.size() + m_vecFiles.size();
	if (nCount == 0)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"安装目录为空");
		return;
	}
	float fRate = 90/(float)nCount;
	size_t i = 0;
	for (; i<m_vecFiles.size(); ++i )
	{
		if (FALSE ==  DeleteFile(m_vecFiles[i].c_str()))
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"删除文件：%s失败，系统错误码：%u", m_vecFiles[i].c_str(), GetLastError());
		}
		if (m_hMsgWnd) 
			::PostMessage(m_hMsgWnd, m_uMsg, Step_DelFiles, (LPARAM)(10 + (i + 1)*fRate));
	}
	for ( size_t j=0; j<m_vecDirs.size(); ++j )
	{
		RemoveDirectory(m_vecDirs[j].c_str());
		if (m_hMsgWnd) 
			::PostMessage(m_hMsgWnd, m_uMsg, Step_DelFiles, (LPARAM)(10 + (i + j + 1)*fRate));
	}
	RemoveDirectory(m_strInstallPath.c_str());
}

void CUnSetupModule::EnumFiles(const wchar_t* pDirPath)
{
	WIN32_FIND_DATA	wfd;
	wchar_t szFindPath[MAX_PATH] = { 0 };
	swprintf_s(szFindPath, L"%s\\*", pDirPath);
	HANDLE hFile=FindFirstFile(szFindPath, &wfd);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		wchar_t szDirPath[MAX_PATH] = { 0 };
		while(FindNextFile(hFile, &wfd))
		{
			if (wcscmp(wfd.cFileName, L".") == 0 || wcscmp(wfd.cFileName, L"..") == 0)
				continue;
			memset(szDirPath, 0, MAX_PATH);
			swprintf_s(szDirPath, L"%s\\%s", pDirPath, wfd.cFileName);
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				EnumFiles(szDirPath);
				m_vecDirs.push_back(szDirPath);
				continue;
			}
			m_vecFiles.push_back(szDirPath);
		}
		FindClose(hFile);
	}
}

void CUnSetupModule::UnSetup(wstring& strError)
{
	int nLen = m_strInstallPath.size();
	if (m_strInstallPath[nLen-1] != '\\')
		m_strInstallPath.append(L"\\");
	vector<wstring> comList;
	RegReadList(REG_ROOT_NAME, comList, RegConflictList);
	PublicLib::KillProcessFromList(comList);
	Sleep(100);
	if (m_hMsgWnd) 
		::PostMessage(m_hMsgWnd, m_uMsg, Step_Init, (LPARAM)3);
	//清理注册表
	RegDelUninstInfo(REG_ROOT_NAME);
	DelBootRun(REG_ROOT_NAME);
	if (m_hMsgWnd) 
		::PostMessage(m_hMsgWnd, m_uMsg, Step_DelReg, (LPARAM)6);
	//清理快捷方式
	ClearShortcut();
	if (m_hMsgWnd) 
		::PostMessage(m_hMsgWnd, m_uMsg, Step_DelShortcut, (LPARAM)10);
	//删除文件
	RemoveFiles();
	if (m_hMsgWnd) 
		::PostMessage(m_hMsgWnd, m_uMsg, Step_DelFiles, 100);
	if (m_hMsgWnd)
		::PostMessage(m_hMsgWnd, m_uMsg, Step_Finish, 100);
}
