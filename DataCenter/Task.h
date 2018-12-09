#pragma once
#include <string>
using std::string;
using std::wstring;
#include "TaskEnum.h"

namespace DataCenter{
	//所有任务的基类
	class ITask
	{
	public:
		ITask()
			: m_type(TaskBase)
			, m_bUsed(TRUE)
		{
			
		}
		virtual ~ITask()
		{
			
		}
		virtual int Run() = 0;//线程中执行，基类自己实现请求
		virtual void Clear() {}
		TaskType GetTaskType()const		{ return m_type; }
		BOOL IsUsed()const				{ return m_bUsed; }
		void SetUse(BOOL bUse)			{ m_bUsed = bUse; }

	protected:
		BOOL m_bUsed;
		TaskType m_type;
	};
}