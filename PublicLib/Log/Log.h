
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

		//���ý�����
		void SetProcessName(const std::wstring& strProcessName);

		//����ͬ����ӡ��־or�첽��ӡ��־
		void SetSyncWriteLog(bool bSync = false);

		//��������־�����ʽ��Ĭ�ϰ����ڴ�ӡ��־
		void SetOutputMode(XYLOG_OUTPUT_MODE emOutputMode = OUTPUT_MODE_DATE);

		//������־·��
		bool SetLogPath(LPCTSTR strLogPath);

		//������־����	
		bool SetLogLevel(XYLOG_LEVEL emLogLevel);

		//����������־����
		bool SetNetLogLevel(XYLOG_LEVEL nLogLevel);

		//����������־��ռ�����̿ռ�,Ĭ��Ϊ500M����λ:M
		bool SetLogSpace(unsigned long ulDiskSpace);

		//����ÿ����־�ļ��Ĵ�С����λ:M ������ Ĭ�ϵ�����־�ļ���СΪ10M
		bool SetLogFileMaxLen(unsigned long ulFileMaxLen);

		//д�ַ���
		bool WriteString(LPCTSTR str, XYLOG_LEVEL nLogLevel, LPCTSTR strFile, int nLine);

		//ʮ�����ƴ�ӡָ��
		bool WriteHex(BYTE* pCommand, int nCount, XYLOG_LEVEL nLogLevel, LPCTSTR str, LPCTSTR strFile, int nLine);

		//�ɱ������־��ӡ
		bool WriteLog(XYLOG_LEVEL nLogLevel, LPCTSTR strFile, int nLine, LPCTSTR pszFormat, ...);

		//�ɱ������־��ӡ
		bool WriteLog(XYLOG_LEVEL nLogLevel, LPCTSTR strFile, int nLine, LPCTSTR pszFormat, va_list varg);

		//��ʱ�����־�̺߳���
		void CleanLogThreadContent();

		//��ȡ��ǰ��ӡ��־�ļ���
		const std::wstring & GetLogPath(){ return m_strLogFileName; }

		//��ӡ��־�̺߳���
		void WriteLogThreadContent();

		bool SetLogOn(bool bLogOn);

	private:

		//���ļ�
		bool Open();

		//�ر��ļ�
		bool Close();

		//��ȡ��־��ͷ
		CString GetLogHeader(LPCTSTR strFile, int nLine, int nLogLevel);

		//�Ƿ������־�ļ�
		bool SWitchLogFile();

		//�����־
		bool AddLog(CString strLog, XYLOG_LEVEL nLogLevel, LPCTSTR strFile, int nLine);

		bool OpenLog();

		//��ӡ��־�߳�
		static unsigned _stdcall WriteLogProc(void* param);

	private:
		bool                        m_bStartOutputLog;//��ʼ��ӡ��־
		XYLOG_OUTPUT_MODE m_emOutputMode;//��־�����ʽ
		int                            m_nLogLevel;//��־����
		int                            m_nNetLogLevel;//������־����
		int                            m_nLogSaveDays;//��־��������
		unsigned long          m_ulLogDiskSpace;//��־��ռ���̿ռ����ֵ
		unsigned long          m_ulFileMaxLen;//������־�ļ�����С
		std::wstring                    m_strLogPath;//��־·��
		std::wstring                    m_strFileName;//��־�ļ���
		std::wstring                    m_strLogFileName;//��־·��
		bool                        m_bOpenSuccess;//��־�ļ���ǰ�Ƿ�򿪳ɹ�
		SYSTEMTIME          m_stLastTime;//��һ�δ�����־�ļ���ʱ��
		HANDLE                 m_hLogFile;//��־�ļ����
		CRITICAL_SECTION m_Section;//�ٽ�������
		int                           m_nLogCount;//��ӡ��־������
		bool                        m_bExit;
		bool                        m_bSync;//�Ƿ�ͬ����ӡ��־
		std::wstring             m_strMacAddress;//����Mac��ַ
		std::string             m_strProcessName;//������

		HANDLE                 m_hWriteLogThread;//�첽��ӡ��־�߳̾��
		std::list<stLogInfo>          m_listLog;//�첽��ӡ��־������
		HANDLE                 m_hWritLogEvent;//��ӡ��־�¼�
	};


}