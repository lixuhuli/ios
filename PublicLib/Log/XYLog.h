#pragma once
#include <windows.h>
#include <string>
using namespace std;

enum XYLOG_LEVEL
{
	LEVEL_DEAD	= 1,//������־
	LEVEL_ERROR = 2,//������־
	LEVEL_WARN	= 3,//������־
	LEVEL_INFO	= 4,//��Ϣ��־
	LEVEL_DEBUG = 5,//������־
};

enum XYLOG_OUTPUT_MODE
{
	OUTPUT_MODE_DATE = 1,//�����ڴ�ӡ��־��������ÿ������һ����־�ļ�����׷�ӵķ�ʽд��
	OUTPUT_MODE_SESSION,//���Ự��ӡ��־��ÿ��������������һ����־�ļ�
};

#define SET_PROCESS_NAME_EX(g_pXyLog,strProcessName)        g_pXyLog->SetProcessName(strProcessName)
#define SET_XYLOG_ON_EX(g_pXyLog,bLogOn)                             g_pXyLog->SetLogOn(bLogOn)
#define SET_XYLOG_OUT_MODE_EX(g_pXyLog,nOutMode)           g_pXyLog->SetOutputMode(nOutMode)
#define SET_XYLOG_PATH_EX(g_pXyLog,pszLogPath)                    g_pXyLog->SetLogPath(pszLogPath)
#define SET_XYNETLOG_LEVEL_EX(g_pXyLog,nLogLevel)                      g_pXyLog->SetNetLogLevel(nLogLevel)
#define SET_XYLOG_LEVEL_EX(g_pXyLog,nLogLevel)                      g_pXyLog->SetLogLevel(nLogLevel)
#define SET_XYLOG_FILE_MAX_LEN_EX(g_pXyLog,ulFileMaxLen)    g_pXyLog->SetLogFileMaxLen(ulFileMaxLen)
#define SET_XYLOG_LOG_SPACE_EX(g_pXyLog,ulDiskSpace)          g_pXyLog->SetLogSpace(ulDiskSpace)
#define OUTPUT_XYLOG_STRING_EX(g_pXyLog,strLog,nLogLevel) g_pXyLog->WriteString(strLog,nLogLevel,__FILEW__,__LINE__)
#define OUTPUT_XYLOG_EX(g_pXyLog,nLogLevel,format, ...)          g_pXyLog->WriteLog(nLogLevel,__FILEW__,__LINE__,format, ##__VA_ARGS__)

#define SET_PROCESS_NAME(strProcessName)        SET_PROCESS_NAME_EX(CXYLog::GetInstance(),strProcessName)
#define SET_XYLOG_ON(bLogOn)                           SET_XYLOG_ON_EX(CXYLog::GetInstance(),bLogOn)
#define SET_XYLOG_OUT_MODE(nOutMode)          SET_XYLOG_OUT_MODE_EX(CXYLog::GetInstance(),nOutMode)
#define SET_XYLOG_PATH(pszLogPath)                    SET_XYLOG_PATH_EX(CXYLog::GetInstance(),pszLogPath)
#define SET_XYNETLOG_LEVEL(nLogLevel)               SET_XYNETLOG_LEVEL_EX(CXYLog::GetInstance(),nLogLevel)
#define SET_XYLOG_LEVEL(nLogLevel)                      SET_XYLOG_LEVEL_EX(CXYLog::GetInstance(),nLogLevel)
#define SET_XYLOG_FILE_MAX_LEN(ulFileMaxLen)    SET_XYLOG_FILE_MAX_LEN_EX(CXYLog::GetInstance(),ulFileMaxLen)
#define SET_XYLOG_LOG_SPACE(ulDiskSpace)          SET_XYLOG_LOG_SPACE_EX(CXYLog::GetInstance(),ulDiskSpace)
#define OUTPUT_XYLOG_STRING(strLog,nLogLevel)  OUTPUT_XYLOG_STRING_EX(CXYLog::GetInstance(),strLog,nLogLevel)
#define OUTPUT_XYLOG(nLogLevel,format, ...)           OUTPUT_XYLOG_EX(CXYLog::GetInstance(),nLogLevel,format, ##__VA_ARGS__)

class CXYLog
{
public:
	CXYLog(void);
    ~CXYLog();

public:
	static CXYLog* GetInstance();
	
	//���ý�����
	void SetProcessName( const std::wstring& strProcessName );

	 /***********************************************************************************************
	��������:SetSyncWriteLog

	ʵ�ֹ���:�����Ƿ�ͬ����ӡ��־��������Ĭ���첽��ӡ��־

	�����б�:bSync:true:ͬ����ӡ��־  false:�첽��ӡ��־

    ����ֵ  :NA
    ***********************************************************************************************/    
	void SetSyncWriteLog(bool bSync = false);

	/***********************************************************************************************
	��������:SetOutputMode

	ʵ�ֹ���:��������־�����ʽ��Ĭ�ϰ����ڴ�ӡ��־

	�����б�:emOutputMode:OUTPUT_MODE_DATE:Ĭ�ϰ����ڴ�ӡ  
										   OUTPUT_MODE_SESSION:���Ự��ӡ��־

    ����ֵ  :NA
    ***********************************************************************************************/    
	void SetOutputMode( XYLOG_OUTPUT_MODE emOutputMode = OUTPUT_MODE_DATE );

   /***********************************************************************************************
	��������:SetLogPath

	ʵ�ֹ���:������־·����������Ĭ����־��ӡ��exeĿ¼�µ�logĿ¼����־��Ϊexe����

	�����б�:strLogPath:��־·�� ����:c:\log\XYClient.log  ��־���ƻ��Զ��ں������������c:\log\XYClient2015-07-21.log

    ����ֵ  :�ɹ���ʧ��
    ***********************************************************************************************/    
    bool SetLogPath(LPCTSTR strLogPath);    
    
     /***********************************************************************************************
	��������:SetLogSpace

	ʵ�ֹ���:����������־��ռ�����̿ռ�,��λ:M ������Ĭ��Ϊ500M,��������ֵʱ�Զ�ɾ�����������������־

	�����б�:ulDiskSpace:��־��ռ�����̿ռ�,��λ:M

    ����ֵ  :�ɹ���ʧ��
    ***********************************************************************************************/
    bool SetLogSpace(unsigned long ulDiskSpace);

    /***********************************************************************************************
	��������:SetLogLevel

	ʵ�ֹ���:������־����,������ʱĬ����־����Ϊ ������־

	�����б�:nLogLevel:��־����

    ����ֵ  :�ɹ���ʧ��
    ***********************************************************************************************/
    bool SetLogLevel(XYLOG_LEVEL nLogLevel);

	/***********************************************************************************************
	��������:SetNetLogLevel

	ʵ�ֹ���:����������־����,������ʱĬ�ϲ���ӡ������־

	�����б�:nLogLevel:������־����

    ����ֵ  :�ɹ���ʧ��
    ***********************************************************************************************/
    bool SetNetLogLevel(XYLOG_LEVEL nLogLevel);
    
    /***********************************************************************************************
	��������:SetLogFileMaxLen

	ʵ�ֹ���:����ÿ����־�ļ��Ĵ�С����λ:M ������Ĭ�ϵ�����־�ļ���СΪ10M��������ֵʱ�Զ��л���һ����־��ӡ

	�����б�:ulFileMaxLen:ÿ����־�ļ�����󳤶�

    ����ֵ  :�ɹ���ʧ��
    ***********************************************************************************************/
    bool SetLogFileMaxLen(unsigned long ulFileMaxLen );
       
    /***********************************************************************************************
	��������:WriteString

	ʵ�ֹ���:д��־���ݡ����ַ���

	�����б�:str:��־����  nLogLevel:��־�ȼ� strFile:�ļ���(__FILEW__) nLine:������(__LINE__)

    ����ֵ  :�ɹ���ʧ��
    ***********************************************************************************************/
    bool WriteString(LPCTSTR strLog,XYLOG_LEVEL nLogLevel,LPCTSTR strFile,int nLine);
    
    /***********************************************************************************************
	��������:WriteHex

	ʵ�ֹ���:д��־���ݡ���ʮ�����ƴ�ӡ����
 
	�����б�:pCommand:��־����  nCount:���ݳ���  str:��־��ͷ˵��
			      nLogLevel:��־�ȼ�     strFile:�����ļ���(__FILEW__) nLine:������(__LINE__) 

    ����ֵ  :�ɹ���ʧ��
    ***********************************************************************************************/
    bool WriteHex(BYTE* pCommand,int nCount,LPCTSTR strLog ,XYLOG_LEVEL nLogLevel,LPCTSTR strFile,int nLine);
    
    /***********************************************************************************************
	��������:WriteLog

	ʵ�ֹ���:д��־���ݡ����ɱ������־��ӡ

	�����б�:nLogLevel:��־����  strFile:�����ļ���(__FILEW__) nLine:������(__LINE__)  pszFormat:��ʽ

    ����ֵ  :��δ�ӡ��־
    ***********************************************************************************************/ 
    bool WriteLog(XYLOG_LEVEL nLogLevel,LPCTSTR strFile,int nLine,LPCTSTR pszFormat,...);
     
	 /***********************************************************************************************
	��������:GetLogPath

	ʵ�ֹ���:��ȡ��ǰ��ӡ��־�ļ�·��

	�����б�:NA

    ����ֵ  :��ǰ��ӡ��־�ļ�·��
    ***********************************************************************************************/ 
	wstring GetLogPath();

	 /***********************************************************************************************
	��������:StopLogOn

	ʵ�ֹ���:������־��ӡ����

	�����б�:bLogOn: true:����־��ӡ  false:�ر���־��ӡ

    ����ֵ  :�ɹ���ʧ��
    ***********************************************************************************************/ 
	bool SetLogOn( bool bLogOn );

private:
    void*    m_pLog;
};

