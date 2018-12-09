#pragma once
#include <windows.h>
#include <string>
using namespace std;

enum XYLOG_LEVEL
{
	LEVEL_DEAD	= 1,//致命日志
	LEVEL_ERROR = 2,//错误日志
	LEVEL_WARN	= 3,//警告日志
	LEVEL_INFO	= 4,//信息日志
	LEVEL_DEBUG = 5,//调试日志
};

enum XYLOG_OUTPUT_MODE
{
	OUTPUT_MODE_DATE = 1,//按日期打印日志。即程序每天生成一个日志文件，已追加的方式写入
	OUTPUT_MODE_SESSION,//按会话打印日志。每次启动程序生成一个日志文件
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
	
	//设置进程名
	void SetProcessName( const std::wstring& strProcessName );

	 /***********************************************************************************************
	函数名称:SetSyncWriteLog

	实现功能:设置是否同步打印日志，不设置默认异步打印日志

	参数列表:bSync:true:同步打印日志  false:异步打印日志

    返回值  :NA
    ***********************************************************************************************/    
	void SetSyncWriteLog(bool bSync = false);

	/***********************************************************************************************
	函数名称:SetOutputMode

	实现功能:设置是日志输出方式，默认按日期打印日志

	参数列表:emOutputMode:OUTPUT_MODE_DATE:默认按日期打印  
										   OUTPUT_MODE_SESSION:按会话打印日志

    返回值  :NA
    ***********************************************************************************************/    
	void SetOutputMode( XYLOG_OUTPUT_MODE emOutputMode = OUTPUT_MODE_DATE );

   /***********************************************************************************************
	函数名称:SetLogPath

	实现功能:设置日志路径，不设置默认日志打印在exe目录下的log目录，日志名为exe名称

	参数列表:strLogPath:日志路径 例如:c:\log\XYClient.log  日志名称会自动在后面添加年月日c:\log\XYClient2015-07-21.log

    返回值  :成功或失败
    ***********************************************************************************************/    
    bool SetLogPath(LPCTSTR strLogPath);    
    
     /***********************************************************************************************
	函数名称:SetLogSpace

	实现功能:设置所有日志所占最大磁盘空间,单位:M 不设置默认为500M,当超过此值时自动删除出当天外的所有日志

	参数列表:ulDiskSpace:日志所占最大磁盘空间,单位:M

    返回值  :成功或失败
    ***********************************************************************************************/
    bool SetLogSpace(unsigned long ulDiskSpace);

    /***********************************************************************************************
	函数名称:SetLogLevel

	实现功能:设置日志级别,不设置时默认日志级别为 调试日志

	参数列表:nLogLevel:日志级别

    返回值  :成功或失败
    ***********************************************************************************************/
    bool SetLogLevel(XYLOG_LEVEL nLogLevel);

	/***********************************************************************************************
	函数名称:SetNetLogLevel

	实现功能:设置网络日志级别,不设置时默认不打印网络日志

	参数列表:nLogLevel:网络日志级别

    返回值  :成功或失败
    ***********************************************************************************************/
    bool SetNetLogLevel(XYLOG_LEVEL nLogLevel);
    
    /***********************************************************************************************
	函数名称:SetLogFileMaxLen

	实现功能:设置每个日志文件的大小，单位:M 不设置默认单个日志文件大小为10M，超过此值时自动切换下一个日志打印

	参数列表:ulFileMaxLen:每个日志文件的最大长度

    返回值  :成功或失败
    ***********************************************************************************************/
    bool SetLogFileMaxLen(unsigned long ulFileMaxLen );
       
    /***********************************************************************************************
	函数名称:WriteString

	实现功能:写日志数据——字符串

	参数列表:str:日志内容  nLogLevel:日志等级 strFile:文件名(__FILEW__) nLine:代码行(__LINE__)

    返回值  :成功或失败
    ***********************************************************************************************/
    bool WriteString(LPCTSTR strLog,XYLOG_LEVEL nLogLevel,LPCTSTR strFile,int nLine);
    
    /***********************************************************************************************
	函数名称:WriteHex

	实现功能:写日志数据——十六进制打印数据
 
	参数列表:pCommand:日志内容  nCount:内容长度  str:日志行头说明
			      nLogLevel:日志等级     strFile:代码文件名(__FILEW__) nLine:代码行(__LINE__) 

    返回值  :成功或失败
    ***********************************************************************************************/
    bool WriteHex(BYTE* pCommand,int nCount,LPCTSTR strLog ,XYLOG_LEVEL nLogLevel,LPCTSTR strFile,int nLine);
    
    /***********************************************************************************************
	函数名称:WriteLog

	实现功能:写日志数据——可变参数日志打印

	参数列表:nLogLevel:日志级别  strFile:代码文件名(__FILEW__) nLine:代码行(__LINE__)  pszFormat:格式

    返回值  :变参打印日志
    ***********************************************************************************************/ 
    bool WriteLog(XYLOG_LEVEL nLogLevel,LPCTSTR strFile,int nLine,LPCTSTR pszFormat,...);
     
	 /***********************************************************************************************
	函数名称:GetLogPath

	实现功能:获取当前打印日志文件路径

	参数列表:NA

    返回值  :当前打印日志文件路径
    ***********************************************************************************************/ 
	wstring GetLogPath();

	 /***********************************************************************************************
	函数名称:StopLogOn

	实现功能:设置日志打印开关

	参数列表:bLogOn: true:打开日志打印  false:关闭日志打印

    返回值  :成功或失败
    ***********************************************************************************************/ 
	bool SetLogOn( bool bLogOn );

private:
    void*    m_pLog;
};

