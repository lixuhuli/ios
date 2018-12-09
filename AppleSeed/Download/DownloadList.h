/***********************************************
*	���������б�ģ�飬�ṩ�ӿڵ�������
*	Ҧ����
*	�������нӿڵĵ��ö���ͬһ�߳��е��ã��Ƴ���
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
	void AddLoadTask(ITask* pTask);																				//��������
	void AddFinishTask(ITask* pTask);																			//�������
	void DeleteLoadTask(ITask* pTask);																			//ɾ������
	void DeleteFinishTask(ITask* pTask);																		//ɾ�����
	void FinishTask(ITask* pTask);																				//���һ������
	void Release();																								//�ͷ��ڴ�
	size_t GetLoadCount();																						//��ȡ�������ص���Ŀ
	size_t GetLoadPercent(OUT int& nPercent);																	//��ȡ���غͰ�װ��Ŀ�Ͱٷֱ�
	ITask* UpdateWaitList();																					//��ȡ���ڵȴ�������
	ITask* UpdateLoadList();																					//��ȡ�������ص�����
	ITask* GetTaskByID(__int64 nGameID);																//ͨ��ID��ѯ�����Ƿ����
	bool IsLoadTaskExist(ITask* pTask);																			//��ѯ�����Ƿ����
	bool IsFinishTaskExist(ITask* pTask);																		//��ѯ��������Ƿ����
	bool IsTaskNameInFinishList(ITask* pTask);
	const TaskList& GetLoadList()const		{ return m_loadList; }
	const TaskList& GetFinishList()const	{ return m_finishList; }

protected:
	void ReleaseTaskList(TaskList& taskList);																	//��ղ����������б�

private:
	TaskMap	 m_loadMap;
	TaskList m_loadList;
	TaskList m_finishList;
	TaskList m_delList;
};
