#include "StdAfx.h"
#include "Log.h"
#include <Shlwapi.h>
#include <ShlObj.h>
#include <vector>
#include "System\System.h"
#include "StrHelper/StrHelper.h"



#define LOG_FILE_MAX_LENGTH                  (10 * 1024 * 1024)//Ĭ�ϵ�����־�ļ������ߴ�

	CStringA ConvertUnicodeToUTF8(LPCWSTR pszStr)
	{
		if (!pszStr)
			return "";

		int len = _tcslen(pszStr);
		CStringA sr;
		UINT uLimit = 0x7fffffff;
		if (uLimit <= 0 || len == 0)
		{
			return sr;
		}
		UINT utf8len = 0;
		for (int i = 0; i < len; ++i)
		{
			int marki = i;
			unsigned short us = pszStr[i];
			if (us == 0)
			{
				utf8len += 2;
			}
			else if (us <= 0x7f)
			{
				utf8len += 1;
			}
			else if (us <= 0x7ff)
			{
				utf8len += 2;
			}
			else if (0xd800 <= us && us <= 0xdbff && i + 1 < len)
			{
				unsigned short ul = pszStr[i + 1];
				if (0xdc00 <= ul && ul <= 0xdfff)
				{
					++i;
					utf8len += 4;
				}
				else
				{
					utf8len += 3;
				}
			}
			else
			{
				utf8len += 3;
			}
			if (utf8len > uLimit)
			{
				len = marki;
				break;
			}
		}

		PBYTE pc = (PBYTE)sr.GetBuffer(utf8len + 1);
		for (int i = 0; i < len; ++i)
		{
			unsigned short us = pszStr[i];
			if (us == 0)
			{
				*pc++ = 0xc0;
				*pc++ = 0x80;
			}
			else if (us <= 0x7f)
			{
				*pc++ = (char)us;
			}
			else if (us <= 0x7ff)
			{
				*pc++ = 0xc0 | (us >> 6);
				*pc++ = 0x80 | (us & 0x3f);
			}
			else if (0xd800 <= us && us <= 0xdbff && i + 1 < len)
			{
				unsigned short ul = pszStr[i + 1];
				if (0xdc00 <= ul && ul <= 0xdfff)
				{
					++i;
					UINT bc = (us - 0xD800) * 0x400 + (ul - 0xDC00) + 0x10000;
					*pc++ = (BYTE)(0xf0 | ((bc >> 18) & 0x07));
					*pc++ = (BYTE)(0x80 | ((bc >> 12) & 0x3f));
					*pc++ = (BYTE)(0x80 | ((bc >> 6) & 0x3f));
					*pc++ = (BYTE)(0x80 | ((bc)& 0x3f));
				}
				else
				{
					*pc++ = (BYTE)(0xe0 | ((us >> 12) & 0x0f));
					*pc++ = (BYTE)(0x80 | ((us >> 6) & 0x3f));
					*pc++ = (BYTE)(0x80 | ((us)& 0x3f));
				}
			}
			else
			{
				*pc++ = (BYTE)(0xe0 | ((us >> 12) & 0x0f));
				*pc++ = (BYTE)(0x80 | ((us >> 6) & 0x3f));
				*pc++ = (BYTE)(0x80 | ((us)& 0x3f));
			}
		}
		*pc++ = 0;
		sr.ReleaseBuffer();
		return sr;
	}

	namespace PublicLib
	{
		CLog::CLog(void)
		{
			m_nLogLevel = LEVEL_DEBUG;//��־����
			m_ulFileMaxLen = LOG_FILE_MAX_LENGTH;//������־�ļ�����С
			m_ulLogDiskSpace = 2 * LOG_FILE_MAX_LENGTH;//��־��ռ���̿ռ����ֵ
			m_bOpenSuccess = false;//��־�ļ���ǰ�Ƿ�򿪳ɹ�
			m_nLogCount = 0;
			m_bExit = false;
			m_hWriteLogThread = INVALID_HANDLE_VALUE;
			m_bSync = false;
			m_hWritLogEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			m_bStartOutputLog = true;
			m_emOutputMode = OUTPUT_MODE_DATE;
			m_nNetLogLevel = 0;
			InitializeCriticalSection(&m_Section);
		}

		CLog::~CLog(void)
		{
			m_bStartOutputLog = false;
			m_bExit = true;
			SetEvent(m_hWritLogEvent);
			if (INVALID_HANDLE_VALUE != m_hWriteLogThread)
			{
				if (WAIT_OBJECT_0 != WaitForSingleObject(m_hWriteLogThread, 5000))
				{
					DWORD dwExitCode = 0;
					GetExitCodeThread(m_hWriteLogThread, &dwExitCode);
					TerminateThread(m_hWriteLogThread, dwExitCode);
				}
				CloseHandle(m_hWriteLogThread);
			}
			//�ر���־�ļ�
			Close();
			CloseHandle(m_hWritLogEvent);
			DeleteCriticalSection(&m_Section);
		}

		void CLog::SetProcessName(const std::wstring& strProcessName)
		{
			m_strProcessName = (LPCSTR)ConvertUnicodeToUTF8(strProcessName.c_str());
		}

		void CLog::SetSyncWriteLog(bool bSync /*= false*/)
		{
			m_bSync = bSync;
		}

		void CLog::SetOutputMode(XYLOG_OUTPUT_MODE emOutputMode /*= OUTPUT_MODE_DATE */)
		{
			m_emOutputMode = emOutputMode;
		}

		bool CLog::SetLogPath(LPCTSTR strLogPath)
		{
			CString strTemp = strLogPath;
			strTemp.Replace('/', '\\');
			std::wstring strPath = (LPCTSTR)strTemp;
			int nPos = strPath.find_last_of(_T('\\'));
			if (-1 == nPos)
			{
				return false;
			}
			m_strLogPath = strPath.substr(0, nPos + 1);
			m_strFileName = strPath.substr(nPos + 1);
			nPos = m_strFileName.rfind(L'.');
			if (nPos != -1)
			{
				m_strFileName = m_strFileName.substr(0, nPos);
			}
			return true;
		}

		bool CLog::SetLogFileMaxLen(unsigned long ulFileMaxLen)
		{
			m_ulFileMaxLen = ulFileMaxLen * 1024 * 1024;
			return true;
		}

		bool CLog::SetLogLevel(XYLOG_LEVEL nLogLevel)
		{
			m_nLogLevel = nLogLevel;
			return true;
		}

		bool CLog::SetNetLogLevel(XYLOG_LEVEL nLogLevel)
		{
			m_nNetLogLevel = nLogLevel;
			return true;
		}

		bool CLog::SetLogSpace(unsigned long ulDiskSpace)
		{
			m_ulLogDiskSpace = ulDiskSpace * 1024 * 1024;
			return true;
		}

		bool CLog::Open()
		{
			if (m_bOpenSuccess)
			{
				return false;
			}

			TCHAR szBuf[MAX_PATH] = { 0 };
			GetModuleFileName(NULL, szBuf, MAX_PATH);
			CString strPath = szBuf;
			//��־·��Ϊ��
			if (m_strLogPath.empty())
			{
				int nIndex = strPath.ReverseFind('\\');
				CString strPathName;
				if (nIndex > 0)
				{
					strPathName = strPath.Left(nIndex + 1);
				}
				m_strLogPath = strPathName + _T("log\\");
			}
			//����logĿ¼
			::SHCreateDirectory(NULL, m_strLogPath.c_str());

			if (m_strFileName.empty())
			{
				m_strFileName = PathFindFileName(strPath);
				int nPos = m_strFileName.rfind(L'.');
				if (nPos != -1)
				{
					m_strFileName = m_strFileName.substr(0, nPos);
				}
			}

			return OpenLog();
		}

		bool CLog::Close()
		{
			if (m_bOpenSuccess)
			{
				EnterCriticalSection(&m_Section);
				CloseHandle(m_hLogFile);
				m_hLogFile = NULL;
				LeaveCriticalSection(&m_Section);
				m_bOpenSuccess = false;
			}
			return true;
		}

		bool CLog::WriteString(LPCTSTR str, XYLOG_LEVEL nLogLevel, LPCTSTR strFile, int nLine)
		{
			//�ж���־�ȼ�
			if (nLogLevel < LEVEL_DEAD || nLogLevel > m_nLogLevel)
			{
				return false;
			}
			AddLog(str, nLogLevel, strFile, nLine);
			return true;
		}

		bool CLog::WriteHex(BYTE* pCommand, int nCount, XYLOG_LEVEL nLogLevel, LPCTSTR str, LPCTSTR strFile, int nLine)
		{
			//�ж���־�ȼ�
			if (nLogLevel < LEVEL_DEAD || nLogLevel > m_nLogLevel)
			{
				return false;
			}
			//��ȡ��־ͷ
			CString strLogLine = str;

			CString strBuf;
			//��ָ���ʽ����16��������
			CString strTemp;
			for (int j = 0; j < nCount; j++)
			{
				strTemp.Format(_T("%02x "), pCommand[j]);
				strBuf.Append(strTemp);
			}

			//��16�������ݻ���
			if (strBuf.GetLength() > 48)
			{
				for (int i = 0; i < strBuf.GetLength(); i += 49)
				{
					strBuf.Insert(i, _T("\r\n"));
				}
			}
			AddLog(strLogLine + strBuf, nLogLevel, strFile, nLine);
			return true;
		}

		CString CLog::GetLogHeader(LPCTSTR strFile, int nLine, int nLogLevel)
		{
			if (m_strMacAddress.empty())
			{
				GetMacAddress(m_strMacAddress);
			}
			CString strCodeFileName = strFile;
			strCodeFileName = strCodeFileName.Right(strCodeFileName.GetLength() - strCodeFileName.ReverseFind('\\') - 1);

			SYSTEMTIME stTime;
			GetLocalTime(&stTime);
			DWORD dwThreadId = GetCurrentThreadId();
			CString str;
			switch (nLogLevel)
			{
			case LEVEL_DEAD:
				str = _T("DEAD");
				break;
			case LEVEL_ERROR:
				str = _T("ERROR");
				break;
			case LEVEL_WARN:
				str = _T("WARN");
				break;
			case LEVEL_INFO:
				str = _T("INFO");
				break;
			case LEVEL_DEBUG:
				str = _T("DEBUG");
				break;
			default:
				str = _T("UNKOWN");
				break;
			}
			CString strHeader;
			strHeader.Format(_T("Mac: %s | %04d-%02d-%02d %02d:%02d:%02d | %s | ThreadID:%d | File: %s | Line:%d:"), m_strMacAddress.c_str(), stTime.wYear, stTime.wMonth,
				stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond, str, dwThreadId, strCodeFileName, nLine);
			return strHeader;
		}

		bool CLog::WriteLog(XYLOG_LEVEL nLogLevel, LPCTSTR strFile, int nLine, LPCTSTR pszFormat, ...)
		{
			//��־���𲻹�
			if (nLogLevel < LEVEL_DEAD || nLogLevel > m_nLogLevel || NULL == pszFormat)
			{
				return false;
			}
			va_list varg;
			va_start(varg, pszFormat);
			bool bRet = WriteLog(nLogLevel, strFile, nLine, pszFormat, varg);
			va_end(varg);
			return bRet;
		}

		bool CLog::WriteLog(XYLOG_LEVEL nLogLevel, LPCTSTR strFile, int nLine, LPCTSTR pszFormat, va_list varg)
		{
			//��־���𲻹�
			if (nLogLevel < LEVEL_DEAD || nLogLevel > m_nLogLevel || NULL == pszFormat)
			{
				return false;
			}

			CString strContent;
			try
			{
				strContent.FormatV(pszFormat, varg);
			}
			catch (...)
			{
				return false;
			}

			//��ӵ���־��ӡ����
			AddLog(strContent, nLogLevel, strFile, nLine);
			return true;
		}

		bool CLog::SWitchLogFile()
		{
			//���Ự��ӡ��־����Ҫ������־
			if (OUTPUT_MODE_SESSION == m_emOutputMode)
			{
				return false;
			}
			//�ļ���С��������
			DWORD dwFileSize = GetFileSize(m_hLogFile, NULL);
			if (dwFileSize >= m_ulFileMaxLen)
			{
				CloseHandle(m_hLogFile);
				CString strNewFileName;
				CString str;
				str.Format(_T("%s%s%02d-%02d-%02d"), m_strLogPath.c_str(), m_strFileName.c_str(), m_stLastTime.wYear, m_stLastTime.wMonth, m_stLastTime.wDay);
				int nIndex = 1;
				while (true)
				{
					strNewFileName.Format(_T("%s(%d).log"), str, nIndex);
					if (!PathFileExists(strNewFileName))
					{
						break;
					}
					++nIndex;
				}
				MoveFileEx(str + _T(".log"), strNewFileName, MOVEFILE_REPLACE_EXISTING);
			}
			else
			{
				SYSTEMTIME stTime;
				GetLocalTime(&stTime);
				//����һ�δ�����־��ͬһ��
				if (m_stLastTime.wYear == stTime.wYear && m_stLastTime.wMonth == stTime.wMonth && m_stLastTime.wDay == stTime.wDay)
				{
					return true;
				}
				//�ر���־�ļ�
				CloseHandle(m_hLogFile);
			}

			//����Ƿ���Ҫ�����־
			CleanLogThreadContent();
			m_bOpenSuccess = false;
			//����־
			OpenLog();
			return true;
		}

		bool CLog::AddLog(CString strLog, XYLOG_LEVEL nLogLevel, LPCTSTR strFile, int nLine)
		{
			if (m_bExit || !m_bStartOutputLog)
			{
				return false;
			}
			//��ȡ��־ͷ
			CString strLogHeader = GetLogHeader(strFile, nLine, nLogLevel);
			CString strContent = strLogHeader + strLog + _T("\r\n");
			EnterCriticalSection(&m_Section);
			try
			{
				//�첽��ӡ��־
				if (!m_bSync)
				{
					if (INVALID_HANDLE_VALUE == m_hWriteLogThread)
					{
						m_hWriteLogThread = (HANDLE)_beginthreadex(NULL, 0, WriteLogProc, this, 0, NULL);
					}
					//����������1000����־���Ͷ��������һ����־
					if (m_listLog.size() > 1000)
					{
						m_listLog.pop_front();
					}
					stLogInfo info;
					info.strLogStr = strContent;
					info.nLogLevel = nLogLevel;
					m_listLog.push_back(info);
					SetEvent(m_hWritLogEvent);
				}
				else
				{
					//��־�ļ�δ��
					if (!m_bOpenSuccess)
					{
						CleanLogThreadContent();
						//����־�ļ�
						if (Open())
						{
							m_bOpenSuccess = true;
						}
						else
						{
							LeaveCriticalSection(&m_Section);
							return false;
						}
					}

					//��ӡ��־
					DWORD dwWrite;
					WriteFile(m_hLogFile, strContent.GetBuffer(), sizeof(TCHAR)* strContent.GetLength(), &dwWrite, NULL);

					m_nLogCount++;

					//ˢ����־������
					if (0 == m_nLogCount % 100)
					{
						FlushFileBuffers(m_hLogFile);
					}

					if (0 == m_nLogCount % 100)
					{
						SWitchLogFile();
						m_nLogCount = 0;
					}
				}
			}
			catch (...)
			{
			}
			LeaveCriticalSection(&m_Section);
			return true;
		}

		void CLog::CleanLogThreadContent()
		{
			//���Ự��ӡ��־����Ҫ������־
			if (OUTPUT_MODE_SESSION == m_emOutputMode)
			{
				return;
			}
			WIN32_FIND_DATA* pdata = (WIN32_FIND_DATA*)new char[sizeof(WIN32_FIND_DATA)+1];

			//�������Ƶ���־�������
			memset(pdata, 0, sizeof(WIN32_FIND_DATA)+1);
			HANDLE hFind = FindFirstFile((m_strLogPath + m_strFileName + _T("*.log")).c_str(), pdata);
			if (INVALID_HANDLE_VALUE != hFind)
			{
				DWORD dwSize = 0;
				vector<std::wstring> vecFileName;
				do
				{
					vecFileName.push_back(m_strLogPath + pdata->cFileName);
					//�����ļ���С
					dwSize += pdata->nFileSizeLow;
					memset(pdata, 0, sizeof(WIN32_FIND_DATA)+1);
				} while (FindNextFile(hFind, pdata));
				FindClose(hFind);

				//������־�ļ���С�Ѿ����������־����
				if (dwSize > m_ulLogDiskSpace)
				{
					for (int i = 0; i < int(vecFileName.size()); i++)
					{
						//ɾ����־
						DeleteFile((m_strLogPath + vecFileName[i]).c_str());
					}
				}
			}
			delete[] pdata;
			pdata = NULL;
		}

		bool CLog::OpenLog()
		{
			if (m_bOpenSuccess)
			{
				return false;
			}
			GetLocalTime(&m_stLastTime);
			CString strDate;
			switch (m_emOutputMode)
			{
				//���Ự�����־
			case OUTPUT_MODE_SESSION:
				strDate.Format(_T("%02d-%02d-%02d %02d%02d%02d%03d"), m_stLastTime.wYear, m_stLastTime.wMonth, m_stLastTime.wDay
					, m_stLastTime.wHour, m_stLastTime.wMinute, m_stLastTime.wSecond, m_stLastTime.wMilliseconds);
				break;
				//�����������־
				//case OUTPUT_MODE_DATE:
			default:
				strDate.Format(_T("%02d-%02d-%02d"), m_stLastTime.wYear, m_stLastTime.wMonth, m_stLastTime.wDay);
				break;
			}

			//��ȡ��־ȫ·��
			std::wstring strAllPath = m_strLogPath + m_strFileName + strDate.GetBuffer() + _T(".log");

			//����־�ļ�
			m_hLogFile = CreateFile(strAllPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
			if (NULL == m_hLogFile)
			{
				m_bOpenSuccess = false;
				return false;
			}
			m_strLogFileName = strAllPath;
			DWORD dwFileSize = GetFileSize(m_hLogFile, NULL);
			if (dwFileSize > 0)
			{
				SetFilePointer(m_hLogFile, 0, NULL, FILE_END);
			}
			else
			{
				DWORD dwUnicode = 0xFEFF;
				DWORD dwWritten;
				WriteFile(m_hLogFile, &dwUnicode, 2, &dwWritten, NULL);
			}
			m_bOpenSuccess = true;
			return true;
		}

		unsigned _stdcall CLog::WriteLogProc(void* param)
		{
			CLog* pLog = (CLog*)param;
			pLog->WriteLogThreadContent();
			return 0;
		}

		void CLog::WriteLogThreadContent()
		{
			while (!m_bExit && m_bStartOutputLog)
			{
				//����־��Ҫ��ӡ
				if (WAIT_OBJECT_0 != WaitForSingleObject(m_hWritLogEvent, 3000))
				{
					continue;
				}
				//�˳�
				if (m_bExit || !m_bStartOutputLog)
				{
					break;
				}

				//��־�ļ�δ��
				if (!m_bOpenSuccess)
				{
					CleanLogThreadContent();
					//����־�ļ�
					if (Open())
					{
						m_bOpenSuccess = true;
					}
					else
					{
						Sleep(100);
						continue;
					}
				}

				stLogInfo logInfo;
				EnterCriticalSection(&m_Section);
				if (!m_listLog.empty())
				{
					logInfo = m_listLog.front();
					m_listLog.pop_front();
				}
				//����־��ӡ
				else
				{
					ResetEvent(m_hWritLogEvent);
					LeaveCriticalSection(&m_Section);
					continue;
				}
				LeaveCriticalSection(&m_Section);

				DWORD dwWrite;
				WriteFile(m_hLogFile, (LPCTSTR)(logInfo.strLogStr), sizeof(TCHAR)* (logInfo.strLogStr.GetLength()), &dwWrite, NULL);
				m_nLogCount++;
				//ˢ����־������
				if (0 == m_nLogCount % 100)
				{
					FlushFileBuffers(m_hLogFile);
					SWitchLogFile();
					m_nLogCount = 0;
				}

				//д������־
				// 			if ( logInfo.nLogLevel <= m_nNetLogLevel )
				// 			{
				// 				ReportLogToServerS(m_strProcessName.c_str(),logInfo.strLogStr);
				// 			}
			}
		}

		bool CLog::SetLogOn(bool bLogOn)
		{
			m_bStartOutputLog = bLogOn;
			//ֹͣ��ӡ��־
			if (!bLogOn)
			{
				SetEvent(m_hWritLogEvent);
				if (INVALID_HANDLE_VALUE != m_hWriteLogThread)
				{
					if (WAIT_OBJECT_0 != WaitForSingleObject(m_hWriteLogThread, 3000))
					{
						DWORD dwExitCode = 0;
						GetExitCodeThread(m_hWriteLogThread, &dwExitCode);
						TerminateThread(m_hWriteLogThread, dwExitCode);
					}
					CloseHandle(m_hWriteLogThread);
					m_hWriteLogThread = INVALID_HANDLE_VALUE;
					Close();
				}
			}
			return true;
		}
	}