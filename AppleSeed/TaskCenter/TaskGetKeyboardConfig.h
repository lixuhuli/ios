#pragma once
#include "Task.h"

namespace TaskCenter{
	class CTaskGetKeyBoardConfig
		: public ITask {
	public:
		CTaskGetKeyBoardConfig(const MSG& msg, const wstring& strUrl, const wstring& strDownloadPath, const wstring& strFileDir);
		virtual ~CTaskGetKeyBoardConfig();
		void Init(const MSG& msg, const wstring& strUrl, const wstring& strDownloadPath, const wstring& strFileDir);
		void SetNeedExit(BOOL* pNeedExit);

	protected:
		virtual void Run();

	private:
		BOOL *m_pNeedExit;
		wstring m_strUrl;
        wstring m_strDownloadPath;
		wstring m_strFileDir;
	};
}
