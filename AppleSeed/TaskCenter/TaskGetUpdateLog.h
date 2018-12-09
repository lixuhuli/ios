#pragma once
#include "Task.h"

namespace TaskCenter{
	class CTaskGetUpdateLog
		: public ITask
	{
	public:
        CTaskGetUpdateLog(const MSG& msg);
		~CTaskGetUpdateLog();
		void Init(const MSG& msg);
		void SetNeedExit(BOOL* pNeedExit);

	protected:
		virtual void Run();

	private:
	};
}
