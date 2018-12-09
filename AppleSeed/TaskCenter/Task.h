#pragma once
#include <string>
using std::string;
using std::wstring;
#include "TaskEnum.h"

namespace TaskCenter{
	//所有任务的基类
	class ITask
	{
	public:
		ITask(const MSG& msg)
			: m_type(TaskBase)
			, m_msg(msg)
			, m_hThread(NULL)
			, m_bUsed(TRUE)
		{
		}
		ITask()
			: m_type(TaskBase)
			, m_hThread(NULL)
			, m_bUsed(TRUE)
		{
			ZeroMemory(&m_msg, sizeof(MSG));
		}
		virtual ~ITask()
		{
			if (m_hThread)
			{
				CloseHandle(m_hThread);
				m_hThread = NULL;
			}
		}
		virtual void Run() = 0;//线程中执行，基类自己实现请求
		void SetThread(HANDLE hThread)	{ m_hThread = hThread; }
		HANDLE GetThread()const			{ return m_hThread; }
		TaskType GetTaskType()const		{ return m_type; }
		const MSG& GetMsg()const		{ return m_msg; }
		const string& GetResult()const	{ return m_strResult; }
		BOOL IsUsed()const				{ return m_bUsed; }
		void SetUse(BOOL bUse)			{ m_bUsed = bUse; }
		void Cancel()					{ m_msg.hwnd = NULL; }
		virtual void Clear() {
			m_strResult.clear();
			if (m_hThread) {
				CloseHandle(m_hThread);
				m_hThread = nullptr;
			}
		}

	protected:
		BOOL m_bUsed;
		MSG m_msg;
		TaskType m_type;
		string m_strResult;
		HANDLE m_hThread;
	};
}