#pragma once
#include "MsgDefine.h"
#include "Database\DatabaseMgr.h"
#include "JsHelper.h"



inline void CDownloadMgr::PostProgressMsg(ITask* pTask)
{
	::PostMessage(m_hMsgWnd, WM_DLWND_MSG_LOAD_PROGRESS, 0, (LPARAM)pTask);
}

inline void CDownloadMgr::PostFinishMsg(ITask* pTask)
{
	::PostMessage(m_hMsgWnd, WM_DLWND_MSG_LOAD_FINISH, 0, (LPARAM)pTask);
}

inline void CDownloadMgr::PostErrorMsg(ITask* pTask)
{
	::PostMessage(m_hMsgWnd, WM_DLWND_MSG_LOAD_ERROR, 0, (LPARAM)pTask);
}

inline LRESULT CDownloadMgr::OnMsgLoadProgress(WPARAM wParam, LPARAM lParam)
{
	ITask* pTask = (ITask*)lParam;
	if (pTask->state != Ts_Loading)
		return 0;
	NotifyCallback(pTask);
	return 0;
}

inline void CDownloadMgr::CheckUrl(ITask* pTask, Tag_HelperThread tag)
{
	if (NULL == pTask)
		return;
	CWebHelperThread* pThread = m_webThMgr.Create();
	if (NULL == pThread)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"创建下载辅助线程失败");
		return;
	}
	pThread->Init(m_hMsgWnd, WM_DLWND_MSG_WEBHELPER, pTask, tag);
	pThread->ResumeThread();
	pTask->state = Ts_Check;
	NotifyCallback(pTask);
}

inline void CDownloadMgr::CheckIconUrl(ITask* pTask, Tag_HelperThread tag) {
    if (nullptr == pTask) return;

    CWebHelperThread* pThread = m_webThMgr.Create();
    if (NULL == pThread)
    {
        OUTPUT_XYLOG(LEVEL_ERROR, L"创建下载图标线程失败");
        return;
    }

    pThread->Init(m_hMsgWnd, WM_DLWND_MSG_WEBHELPER, pTask, tag);
    pThread->ResumeThread();
}

inline void CDownloadMgr::SetLoadCallback(ILoadCallback* pCallback, void* lpParam)
{
	m_pLoadCallback = pCallback;
	m_lpParam = lpParam;
}

inline void CDownloadMgr::DetachCallback()
{
	m_pLoadCallback = NULL;
	m_lpParam = NULL;
}

inline void CDownloadMgr::SetMaxLoadSpeed(int nSpeed)
{
	m_nMaxLoadSpeed = nSpeed;
}
