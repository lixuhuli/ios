#include "StdAfx.h"
#include "GlobalData.h"
#include "resource.h"
#include <shlwapi.h>
#include "Regedit.h"


CGlobalData::CGlobalData(void)
	: m_bHasInstall(false)
{
}


CGlobalData::~CGlobalData(void)
{
}

void CGlobalData::Init()
{
	if (RegGetInstallInfo(REG_ROOT_NAME, m_strOldInstallPath, m_strOldVersion, m_strOldUpExeVersion, m_strChannel))
	{
		if (PathFileExists(m_strOldInstallPath.c_str()) && !m_strOldVersion.empty())
		{
			m_bHasInstall = true;
			GetDeviceGuid(m_strGuid);
		}
	}
}

void CGlobalData::Exit()
{

}

