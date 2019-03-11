#pragma once
#include "Task.h"

namespace TaskCenter{
	class CTaskGetUpdateLog
		: public ITask
	{
	public:
        CTaskGetUpdateLog(const MSG& msg, const wstring& ver);
		~CTaskGetUpdateLog();

		void Init(const MSG& msg, const wstring& ver);
		void SetNeedExit(BOOL* pNeedExit);

	protected:
		virtual void Run();

	private:
        wstring version_;
	};
}
