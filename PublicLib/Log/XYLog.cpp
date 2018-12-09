// LogPrint.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "XYLog.h"
#include "Log.h"
using PublicLib::CLog;

static CXYLog g_LogFile;

CXYLog::CXYLog()
{ 
	m_pLog = new CLog;
}

CXYLog::~CXYLog()
{
    if (NULL != m_pLog)
    {
        try
        {
			delete ((CLog*)m_pLog);
            m_pLog = NULL;
       }
        catch (...)
        {        	
        }        
    }
}

CXYLog* CXYLog::GetInstance()
{
	return &g_LogFile;
}

void CXYLog::SetProcessName( const std::wstring& strProcessName )
{
	if( NULL == m_pLog )
	{
		return;
	}
	((CLog*)m_pLog)->SetProcessName(strProcessName);
}

void CXYLog::SetSyncWriteLog( bool bSync /*= false*/ )
{
	if( NULL == m_pLog )
	{
		return;
	}
	((CLog*)m_pLog)->SetSyncWriteLog(bSync);
}

void CXYLog::SetOutputMode( XYLOG_OUTPUT_MODE emOutputMode /*= OUTPUT_MODE_DATE */ )
{
	if( NULL == m_pLog )
	{
		return;
	}
	((CLog*)m_pLog)->SetOutputMode(emOutputMode);
}

bool CXYLog::SetLogLevel( XYLOG_LEVEL nLogLevel )
{
	if( NULL == m_pLog )
	{
		return false;
	}
    return ((CLog*)m_pLog)->SetLogLevel(nLogLevel);
}

bool CXYLog::SetNetLogLevel( XYLOG_LEVEL nLogLevel )
{
	if( NULL == m_pLog )
	{
		return false;
	}
	return ((CLog*)m_pLog)->SetNetLogLevel(nLogLevel);
}

bool CXYLog::SetLogFileMaxLen( unsigned long ulFileMaxLen )
{
	if( NULL == m_pLog )
	{
		return false;
	}
    return ((CLog*)m_pLog)->SetLogFileMaxLen(ulFileMaxLen);
}

bool CXYLog::SetLogPath( LPCTSTR strLogPath )
{
	if( NULL == m_pLog )
	{
		return false;
	}
    return ((CLog*)m_pLog)->SetLogPath(strLogPath);
}

bool CXYLog::WriteString( LPCTSTR strLog,XYLOG_LEVEL nLogLevel,LPCTSTR strFile,int nLine )
{
	if( NULL == m_pLog )
	{
		return false;
	}
    return ((CLog*)m_pLog)->WriteString(strLog,nLogLevel,strFile,nLine);
}

bool CXYLog::WriteHex( BYTE* pCommand,int nCount,LPCTSTR strLog ,XYLOG_LEVEL nLogLevel,LPCTSTR strFile,int nLine )
{
	if( NULL == m_pLog )
	{
		return false;
	}
    return ((CLog*)m_pLog)->WriteHex(pCommand,nCount,nLogLevel,strLog,strFile,nLine);
}

bool CXYLog::WriteLog( XYLOG_LEVEL nLogLevel,LPCTSTR strFile,int nLine,LPCTSTR pszFormat,... )
{
	if( NULL == m_pLog )
	{
		return false;
	}
    va_list varg;
    va_start(varg,pszFormat);
    bool bRet = ((CLog*)m_pLog)->WriteLog(nLogLevel,strFile,nLine,pszFormat,varg);
    va_end(varg); 
	return bRet;
}

wstring CXYLog::GetLogPath()
{
	if( NULL == m_pLog )
	{
		return _T("");
	}
	return ((CLog*)m_pLog)->GetLogPath();
}

bool CXYLog::SetLogOn( bool bLogOn )
{
	if( NULL == m_pLog )
	{
		return false;
	}
	return ((CLog*)m_pLog)->SetLogOn(bLogOn);
}


bool CXYLog::SetLogSpace( unsigned long ulDiskSpace )
{
	if( NULL == m_pLog )
	{
		return false;
	}
    return ((CLog*)m_pLog)->SetLogSpace(ulDiskSpace);
}