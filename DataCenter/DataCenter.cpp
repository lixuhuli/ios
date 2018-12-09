#include "StdAfx.h"
#include "DataCenter.h"
#include "Task.h"
#include <algorithm>
#include "TaskDataPost.h"

namespace DataCenter{


	enum
	{
		WM_ADD_TASK = WM_USER+100,
		WM_EXIT,
	};

	CDataCenter* CDataCenter::Instance()
	{
		static CDataCenter obj;
		return &obj;
	}

	inline ITask* GetFreeTaskByType(const list<UINT_PTR>& taskList, TaskType type)
	{
		TaskConstItor itor = taskList.begin();
		for (; itor != taskList.end(); ++itor)
		{
			ITask* pTask = (ITask*)(*itor);
			if (pTask->GetTaskType() == type && !pTask->IsUsed())
			{
				return pTask;
			}
		}
		return NULL;
	}

	CDataCenter::CDataCenter()
		: m_hThread(NULL)
		, m_nThreadID(0)
	{
	}


	CDataCenter::~CDataCenter()
	{
	}

	void CDataCenter::Init(const char* pToken)
	{
		if (pToken)
		{
			m_strToken = PublicLib::AToU(pToken);
		}
		GetDeviceGuid(m_strGuid);
		wstring strMac;
		PublicLib::GetMacAddress(strMac);
		m_strMac = PublicLib::UToUtf8(strMac);
		InitPCConfig();
		InitWorkThread();
	}

	void CDataCenter::Exit()
	{
		bool bKill = false;
		if (m_hThread)
		{
			::PostThreadMessage(m_nThreadID, WM_EXIT, 0, 0);
			if (WAIT_TIMEOUT == WaitForSingleObject(m_hThread, 5 * 1000))
			{
				TerminateThread(m_hThread, 666);
				OutputDebugString(L"数据线程强制终止完毕！");
				bKill = true;
			}
			OutputDebugString(L"数据线程退出完毕！");
			CloseHandle(m_hThread);
		}
		if (!bKill)
		{
			TaskItor itor = m_taskList.begin();
			for (; itor != m_taskList.end(); ++itor)
			{
				ITask* pTask = (ITask*)(*itor);
				delete pTask;
			}
			m_taskList.clear();
		}
	}

	void CDataCenter::InitPCConfig()
	{
		HKEY hKey = NULL;
		wstring strKeyPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\5funGameHall");
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, strKeyPath.c_str(), 0, KEY_READ, &hKey))
		{
			char szBuff[100] = { 0 };
			DWORD dwSize = 100;
			DWORD dwType = 0;
			if (ERROR_SUCCESS == RegQueryValueExA(hKey, "DisplayVersion", NULL, &dwType, (LPBYTE)szBuff, &dwSize))
				m_strPCVersion.assign(szBuff);
			if (ERROR_SUCCESS == RegQueryValueExA(hKey, "ChannelName", NULL, &dwType, (LPBYTE)szBuff, &dwSize))
				m_nAdID = atoi(szBuff);
			RegCloseKey(hKey);
		}
	}

	bool CDataCenter::InitWorkThread()
	{
		if (m_hThread)
			return false;
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, this, 0, &m_nThreadID);
		return m_hThread != NULL;
	}

	bool CDataCenter::InitPostToken()
	{
		bool bRet = false;
		PublicLib::CHttpClient http(PublicLib::ContentJson);
		http.AddHttpHeader(L"X-WuFan-App-ID", g_pStrAppID);
		http.AddHttpHeader(L"X-WuFan-Client-ID", CDataCenter::Instance()->GetGuid().c_str());
		wstring strSign = wstring(g_pStrAppID) + L"_" + CDataCenter::Instance()->GetGuid() + L"_" + wstring(g_pStrPriKey);
		PublicLib::MD5 md5(PublicLib::UToA(strSign));
		wstring strMd5 = PublicLib::AToU(md5.md5());
		wstring strUrl(URL_GET_TOKEN);
		strUrl.append(L"?auth=");
		strUrl += strMd5;
		string strJson = http.Request(strUrl.c_str(), PublicLib::Get);
		try
		{
			Json::Reader read;
			Json::Value vRoot;
			if (read.parse(strJson, vRoot))
			{
				int nCode = vRoot["errorCode"].asInt();
				if (nCode == 0)
				{
					string strToken = vRoot["data"]["token"].asString();
					if (!strToken.empty())
					{
						m_strToken = PublicLib::Utf8ToU(strToken);
						bRet = true;
					}
				}
			}
		}
		catch (...)
		{
		}
		return bRet;
	}

	void CDataCenter::Work()
	{
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message == WM_EXIT)
				break;
			if (msg.message == WM_ADD_TASK)
			{
				if (m_strToken.empty())
				{
					InitPostToken();
				}
				ITask* pTask = (ITask*)msg.lParam;
				if (pTask)
				{
					if (403 == pTask->Run())
						m_strToken.clear();
					pTask->SetUse(FALSE);
				}
			}
		}
	}

	void CDataCenter::CreatePostTask(const char* pEventName, const Json::Value& vParams/* = Json::Value(Json::objectValue)*/, int nUid/* = 0*/)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskDataPost);
		if (NULL == pTask)
		{
			CTaskDataPost *pPostTask = new CTaskDataPost(nUid, pEventName, vParams);
			pTask = pPostTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskDataPost*)pTask)->Init(nUid, pEventName, vParams);
		}
		::PostThreadMessage(m_nThreadID, WM_ADD_TASK, 0, (LPARAM)pTask);
	}

	UINT __stdcall CDataCenter::Thread(void* lpParam)
	{
		CDataCenter* pModule = (CDataCenter*)lpParam;
		pModule->Work();
		return 0;
	}


}