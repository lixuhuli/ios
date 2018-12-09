#include "Stdafx.h"
#include "DownloadList.h"



CDownloadList::CDownloadList(void)
{
}


CDownloadList::~CDownloadList(void)
{
	
}

void CDownloadList::AddLoadTask(ITask* pTask)
{
	if (NULL == pTask)
		return;
	m_loadMap.insert(std::make_pair(pTask->nGameID, pTask));
	m_loadList.push_back(pTask);
}

void CDownloadList::AddFinishTask(ITask* pTask)
{
	if (NULL == pTask)
		return;
	m_finishList.push_back(pTask);
}

void CDownloadList::DeleteLoadTask(ITask* pTask)
{
	if (NULL == pTask)
		return;
	TaskMapItor itor = m_loadMap.find(pTask->nGameID);
	if (itor != m_loadMap.end())
		m_loadMap.erase(itor);
	TaskListItor loadItor = std::find(m_loadList.begin(), m_loadList.end(), pTask);
	if (loadItor != m_loadList.end())
	{
		m_loadList.erase(loadItor);
		pTask->state = Ts_Delete;
		m_delList.push_back(pTask);
	}
}

void CDownloadList::DeleteFinishTask(ITask* pTask)
{
	if (NULL == pTask)
		return;
	TaskListItor loadItor = std::find(m_finishList.begin(), m_finishList.end(), pTask);
	if (loadItor != m_finishList.end())
	{
		m_finishList.erase(loadItor);
		pTask->state = Ts_Delete;
		m_delList.push_back(pTask);
	}
}

void CDownloadList::FinishTask(ITask* pTask)
{
	if (NULL == pTask)
		return;
	TaskMapItor itor = m_loadMap.find(pTask->nGameID);
	if (itor != m_loadMap.end())
		m_loadMap.erase(itor);
	TaskListItor loadItor = std::find(m_loadList.begin(), m_loadList.end(), pTask);
	if (loadItor != m_loadList.end())
	{
		m_loadList.erase(loadItor);
		m_finishList.push_back(pTask);
	}
}

void CDownloadList::Release()
{
	ReleaseTaskList(m_loadList);
	ReleaseTaskList(m_finishList);
	ReleaseTaskList(m_delList);
	m_loadMap.clear();
	OutputDebugString(L"下载列表数据析构完成+++++++++++++++++++++++++++++++++\n");
}

size_t CDownloadList::GetLoadCount()
{ 
	size_t nCount = 0;
	TaskListItor itor = m_loadList.begin();
	for ( ; itor!=m_loadList.end(); ++itor )
	{
		if ((*itor)->state == Ts_Loading && (*itor)->bShowOnLoadWnd)
		{
			nCount++;
		}
	}
	return nCount;
}

size_t CDownloadList::GetLoadPercent(OUT int& nPercent)
{
	__int64 nTotal = 0, nLoaded = 0;
	size_t nCount = 0, nInstall = 0;
	TaskListItor itor = m_loadList.begin();
	for (; itor != m_loadList.end(); ++itor)
	{
		ITask* pTask = (ITask*)(*itor);
		if (!pTask->bShowOnLoadWnd)
			continue;
		if (pTask->state <= Ts_Loading)
		{
			nTotal += pTask->nTotalSize;
			nLoaded += pTask->nLoadSize;
			nCount++;
		}
		else if (pTask->state == Ts_Pause)
		{
			nTotal += pTask->nTotalSize;
			nLoaded += pTask->nLoadSize;
		}
	}
	if (nTotal == 0)
	{
		if (nInstall > 0)
			nPercent = 100;
		else
			nPercent = 0;
	}
	else
		nPercent = (int)((nLoaded * 100) / nTotal);
	return nCount + nInstall;
}

ITask* CDownloadList::UpdateWaitList()
{
	TaskListItor itor = m_loadList.begin();
	for ( ; itor!=m_loadList.end(); ++itor )
	{
		if ((*itor)->state == Ts_Wait)
		{
			return (*itor);
		}
	}
	return NULL;
}

ITask* CDownloadList::UpdateLoadList()
{
	TaskListItor itor = m_loadList.begin();
	for (; itor != m_loadList.end(); ++itor)
	{
		if ((*itor)->state == Ts_Loading)
		{
			return (*itor);
		}
	}
	return NULL;
}

ITask* CDownloadList::GetTaskByID(__int64 nGameID)
{
	TaskMapItor itor = m_loadMap.find(nGameID);
	if (itor == m_loadMap.end())
		return NULL;
	return itor->second;
}

bool CDownloadList::IsLoadTaskExist(ITask* pTask)
{
	if (NULL == pTask)
		return false;
	TaskListItor itor = std::find(m_loadList.begin(), m_loadList.end(), pTask);
	return itor != m_loadList.end();
}

bool CDownloadList::IsFinishTaskExist(ITask* pTask)
{
	if (NULL == pTask)
		return false;
	TaskListItor itor = std::find(m_finishList.begin(), m_finishList.end(), pTask);
	return itor != m_finishList.end();
}

bool CDownloadList::IsTaskNameInFinishList(ITask* pTask)
{
	if (NULL == pTask)
		return false;
	TaskListItor itor = m_finishList.begin();
	for (; itor != m_finishList.end(); ++itor)
	{
		if ((*itor)->type != pTask->type)
			continue;
		if ((*itor)->strName == pTask->strName)
			return true;
	}
	return false;
}

void CDownloadList::ReleaseTaskList(TaskList& taskList)
{
	if (taskList.empty())
		return ;
	TaskListItor tli = taskList.begin();
	for ( ; tli != taskList.end(); ++tli )
	{
		ITask* pTask = *tli;
		delete pTask;
	}
	taskList.clear();
}