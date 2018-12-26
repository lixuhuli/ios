#pragma once
#include "Task.h"

namespace TaskCenter{
	class CTaskGetKeyBoardConfig
		: public ITask {
	public:
		CTaskGetKeyBoardConfig(const MSG& msg, const string& strAppId, const wstring& strDownloadPath, const wstring& strFileDir);
		virtual ~CTaskGetKeyBoardConfig();
		void Init(const MSG& msg, const string& strAppId, const wstring& strDownloadPath, const wstring& strFileDir);
		void SetNeedExit(BOOL* pNeedExit);

        const string& GetAppId() const { return m_strAppId; }

	protected:
		virtual void Run();

	private:
		BOOL *m_pNeedExit;
		string m_strAppId;
        wstring m_strDownloadPath;
		wstring m_strFileDir;
	};
}
