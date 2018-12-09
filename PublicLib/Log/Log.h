
#pragma once
#include <list>
#include <atlstr.h>
#include "XYLog.h"

namespace PublicLib{

	struct stLogInfo
	{
		CString strLogStr;
		XYLOG_LEVEL nLogLevel;
	};

	class CLog
	{
	public:
		CLog(void);
		~CLog(void);

		//设置进程名
		void SetProcessName(const std::wstring& strProcessName);

		//设置同步打印日志or异步打印日志
		void SetSyncWriteLog(bool bSync = false);

		//设置是日志输出方式，默认按日期打印日志
		void SetOutputMode(XYLOG_OUTPUT_MODE emOutputMode = OUTPUT_MODE_DATE);

		//设置日志路径
		bool SetLogPath(LPCTSTR strLogPath);

		//设置日志级别	
		bool SetLogLevel(XYLOG_LEVEL emLogLevel);

		//设置网络日志级别
		bool SetNetLogLevel(XYLOG_LEVEL nLogLevel);

		//设置所有日志所占最大磁盘空间,默认为500M，单位:M
		bool SetLogSpace(unsigned long ulDiskSpace);

		//设置每个日志文件的大小，单位:M 不设置 默认单个日志文件大小为10M
		bool SetLogFileMaxLen(unsigned long ulFileMaxLen);

		//写字符串
		bool WriteString(LPCTSTR str, XYLOG_LEVEL nLogLevel, LPCTSTR strFile, int nLine);

		//十六进制打印指令
		bool WriteHex(BYTE* pCommand, int nCount, XYLOG_LEVEL nLogLevel, LPCTSTR str, LPCTSTR strFile, int nLine);

		//可变参数日志打印
		bool WriteLog(XYLOG_LEVEL nLogLevel, LPCTSTR strFile, int nLine, LPCTSTR pszFormat, ...);

		//可变参数日志打印
		bool WriteLog(XYLOG_LEVEL nLogLevel, LPCTSTR strFile, int nLine, LPCTSTR pszFormat, va_list varg);

		//定时清除日志线程函数
		void CleanLogThreadContent();

		//获取当前打印日志文件名
		const std::wstring & GetLogPath(){ return m_strLogFileName; }

		//打印日志线程函数
		void WriteLogThreadContent();

		bool SetLogOn(bool bLogOn);

	private:

		//打开文件
		bool Open();

		//关闭文件
		bool Close();

		//获取日志行头
		CString GetLogHeader(LPCTSTR strFile, int nLine, int nLogLevel);

		//是否更换日志文件
		bool SWitchLogFile();

		//添加日志
		bool AddLog(CString strLog, XYLOG_LEVEL nLogLevel, LPCTSTR strFile, int nLine);

		bool OpenLog();

		//打印日志线程
		static unsigned _stdcall WriteLogProc(void* param);

	private:
		bool                        m_bStartOutputLog;//开始打印日志
		XYLOG_OUTPUT_MODE m_emOutputMode;//日志输出方式
		int                            m_nLogLevel;//日志级别
		int                            m_nNetLogLevel;//网络日志级别
		int                            m_nLogSaveDays;//日志保留天数
		unsigned long          m_ulLogDiskSpace;//日志所占磁盘空间最大值
		unsigned long          m_ulFileMaxLen;//单个日志文件最大大小
		std::wstring                    m_strLogPath;//日志路径
		std::wstring                    m_strFileName;//日志文件名
		std::wstring                    m_strLogFileName;//日志路径
		bool                        m_bOpenSuccess;//日志文件当前是否打开成功
		SYSTEMTIME          m_stLastTime;//上一次创建日志文件的时间
		HANDLE                 m_hLogFile;//日志文件句柄
		CRITICAL_SECTION m_Section;//临界区对象
		int                           m_nLogCount;//打印日志的条数
		bool                        m_bExit;
		bool                        m_bSync;//是否同步打印日志
		std::wstring             m_strMacAddress;//机器Mac地址
		std::string             m_strProcessName;//进程名

		HANDLE                 m_hWriteLogThread;//异步打印日志线程句柄
		std::list<stLogInfo>          m_listLog;//异步打印日志缓冲区
		HANDLE                 m_hWritLogEvent;//打印日志事件
	};


}