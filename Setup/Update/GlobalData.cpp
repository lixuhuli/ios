#include "StdAfx.h"
#include "GlobalData.h"
#include "resource.h"
#include <shlwapi.h>
#include "Regedit.h"


CGlobalData::CGlobalData(void)
	: m_bHasInstall(false)
    , need_reboot_(false)
{
}


CGlobalData::~CGlobalData(void)
{
}

void CGlobalData::Init(const wstring& strOldInstallPath) {
    if (strOldInstallPath.empty()) m_strOldInstallPath = GetRunPathW();
    else m_strOldInstallPath = strOldInstallPath;

    if (m_strOldInstallPath.rfind(L"\\") == m_strOldInstallPath.length() - 1) 
        m_strOldInstallPath = m_strOldInstallPath.substr(0, m_strOldInstallPath.length() - 1);

    m_strOldVersion = GetApplicationVersion(m_strOldInstallPath + L"\\" + EXE_MAIN);
    m_strOldUpExeVersion = GetApplicationVersion(m_strOldInstallPath + L"\\" + EXE_UPDATE);
    m_strChannel = REG_ROOT_NAME;

    if (PathFileExists(m_strOldInstallPath.c_str()) && !m_strOldVersion.empty()) {
        m_bHasInstall = true;
        GetDeviceGuid(m_strGuid);
    }
}

void CGlobalData::Exit()
{

}

