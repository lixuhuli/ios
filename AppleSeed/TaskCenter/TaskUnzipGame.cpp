#include "StdAfx.h"
#include "TaskUnzipGame.h"
#include <PublicLib\DeCompress.h>
#include "ParamDefine.h"


namespace TaskCenter{
	CTaskUnzipGame::CTaskUnzipGame(const MSG& msg, UINT uMsg, UINT_PTR nTaskID, const wstring& strZipFile, const wstring& strOutDir)
		: ITask(msg)
		, m_pNeedExit(NULL)
		, m_nTaskID(nTaskID)
		, m_strZipFile(strZipFile)
		, m_strOutDir(strOutDir)
		, m_uMsg(uMsg)
	{
		m_type = TaskUnzipGame;
		m_bUsed = TRUE;
	}

	CTaskUnzipGame::~CTaskUnzipGame()
	{
	}

	void CTaskUnzipGame::Init(const MSG& msg, UINT uMsg, UINT_PTR nTaskID, const wstring& strZipFile, const wstring& strOutDir)
	{
		m_msg = msg;
		m_uMsg = uMsg;
		m_bUsed = TRUE;
		m_nTaskID = nTaskID;
		m_strZipFile = strZipFile;
		m_strOutDir = strOutDir;
	}

	void CTaskUnzipGame::SetNeedExit(BOOL* pNeedExit)
	{
		m_pNeedExit = pNeedExit;
	}

	void CTaskUnzipGame::Run() {
		bool bSuccess = true;
		DeCompressParams params;
		params.lpProgressCallback = UncompressCallback;
		params.lpParam = this;
		params.lpNeedExit = m_pNeedExit;

		if (!PublicLib::DeCompressZipFile(m_strZipFile, m_strOutDir, params)) {
			OUTPUT_XYLOG(LEVEL_ERROR, L"解压失败：文件：%s，解压到：%s，错误信息：%s", m_strZipFile.c_str(), m_strOutDir.c_str(), params.strError.c_str());
			bSuccess = false;
		}

		LUnzipFinishParam lpParam = new UnzipFinishParam;
		lpParam->bSuccess = bSuccess;
		lpParam->nSpeed = params.nSpeed;
		::PostMessage(m_msg.hwnd, m_uMsg, (WPARAM)m_nTaskID, (LPARAM)lpParam);
		m_msg.hwnd = nullptr;
	}

	void CTaskUnzipGame::OnLoadProgress(int nPercent)
	{
		if (nPercent < 0)
			nPercent = 0;
		else if (nPercent>100)
			nPercent = 100;
		::PostMessage(m_msg.hwnd, m_msg.message, (WPARAM)m_nTaskID, nPercent);
	}

	void CTaskUnzipGame::UncompressCallback(int nCurrentPercent, void* pParam)
	{
		CTaskUnzipGame* pTask = (CTaskUnzipGame*)pParam;
		if (pTask)
			pTask->OnLoadProgress(nCurrentPercent);
	}

}