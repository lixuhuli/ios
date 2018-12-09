#pragma once
#include "Task.h"

namespace TaskCenter{
	class CTaskUnzipGame
		: public ITask
	{
	public:
		CTaskUnzipGame(const MSG& msg, UINT uMsg, UINT_PTR nTaskID, const wstring& strZipFile, const wstring& strOutDir);
		~CTaskUnzipGame();
		void Init(const MSG& msg, UINT uMsg, UINT_PTR nTaskID, const wstring& strZipFile, const wstring& strOutDir);
		void SetNeedExit(BOOL* pNeedExit);

	protected:
		virtual void Run();
		void OnLoadProgress(int nPercent);
		static void UncompressCallback(int nCurrentPercent, void* pParam);

	private:
		BOOL *m_pNeedExit;
		UINT m_uMsg;
		UINT_PTR m_nTaskID;
		wstring m_strZipFile;
		wstring m_strOutDir;
	};
}
