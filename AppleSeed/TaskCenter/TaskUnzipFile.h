#pragma once
#include "Task.h"

namespace TaskCenter{
	class CTaskUnzipFile
		: public ITask
	{
	public:
		CTaskUnzipFile(const MSG& msg, UINT uMsg, const wstring& strZipFile, const wstring& strOutDir);
		virtual ~CTaskUnzipFile();
		void Init(const MSG& msg, UINT uMsg, const wstring& strZipFile, const wstring& strOutDir);
		void SetNeedExit(BOOL* pNeedExit);

	protected:
		virtual void Run();
		void OnLoadProgress(int nPercent);
		static void UncompressCallback(int nCurrentPercent, void* pParam);

	private:
		BOOL *m_pNeedExit;
		UINT m_uMsg;
		wstring m_strZipFile;
		wstring m_strOutDir;
	};
}
