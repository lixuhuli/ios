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
       , m_nThreadID(0) {
	}


	CDataCenter::~CDataCenter()
	{
	}

	void CDataCenter::Init(const char* pToken)
	{
		wstring strMac;
		PublicLib::GetMacAddress(strMac);
		m_strMac = PublicLib::UToUtf8(strMac);
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

    bool CDataCenter::InitWorkThread()
    {
        if (m_hThread)
            return false;
        m_hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, this, 0, &m_nThreadID);
        return m_hThread != NULL;
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
                ITask* pTask = (ITask*)msg.lParam;
                if (pTask)
                {
                    pTask->Run();
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