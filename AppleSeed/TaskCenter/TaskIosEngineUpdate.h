#pragma once
#include "Task.h"

namespace TaskCenter{
	class CTaskIosEngineUpdate
		: public ITask
	{
	public:
		CTaskIosEngineUpdate(const MSG& msg, UINT uMsg, const wstring& strVersion);
		virtual ~CTaskIosEngineUpdate();
		void Init(const MSG& msg, UINT uMsg, const wstring& strVersion);

	protected:
		virtual void Run();

	private:
		UINT m_uMsg;
		std::wstring m_strVersion;
	};
}
