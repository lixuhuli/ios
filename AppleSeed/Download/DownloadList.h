/***********************************************
*	下载任务列表模块，提供接口调度任务
*	姚佳宁
*	由于所有接口的调用都在同一线程中调用，移除锁
*	2016-3-23 14:28:21
*/
#pragma once
#include <list>
#include <algorithm>
#include "Task.h"
using std::list;
using std::map;
//#include "AutoLock.h"


typedef list<ITask*>					TaskList;
typedef list<ITask*>::iterator			TaskListItor;
typedef list<ITask*>::const_iterator	TaskListCItor;
typedef map<__int64, ITask*>				TaskMap;
typedef map<__int64, ITask*>::iterator	TaskMapItor;
class CDownloadList
{
public:
	CDownloadList(void);
	~CDownloadList(void);
	void AddLoadTask(ITask* pTask);																				//新增下载
	void AddFinishTask(ITask* pTask);																			//新增完成
	void DeleteLoadTask(ITask* pTask);																			//删除下载
	void DeleteFinishTask(ITask* pTask);																		//删除完成
	void FinishTask(ITask* pTask);																				//完成一个任务
	void Release();																								//释放内存
	size_t GetLoadCount();																						//获取正在下载的数目
	size_t GetLoadPercent(OUT int& nPercent);																	//获取下载和安装数目和百分比
	ITask* UpdateWaitList();																					//获取正在等待的任务
	ITask* UpdateLoadList();																					//获取正在下载的任务
	ITask* GetTaskByID(__int64 nGameID);																//通过ID查询任务是否存在
	bool IsLoadTaskExist(ITask* pTask);																			//查询任务是否存在
	bool IsFinishTaskExist(ITask* pTask);																		//查询完成任务是否存在
	bool IsTaskNameInFinishList(ITask* pTask);
	const TaskList& GetLoadList()const		{ return m_loadList; }
	const TaskList& GetFinishList()const	{ return m_finishList; }

protected:
	void ReleaseTaskList(TaskList& taskList);																	//清空并析构下载列表

private:
	TaskMap	 m_loadMap;
	TaskList m_loadList;
	TaskList m_finishList;
	TaskList m_delList;
};
