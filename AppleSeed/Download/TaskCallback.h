#pragma once
#include <map>
using std::map;


//下载状态
enum TaskState
{
	Ts_Init = 0,	//初始状态
	Ts_Check,		//查询下载地址
	Ts_CheckEnd,	//查询完毕
	Ts_Wait,		//等待下载
	Ts_Loading,		//下载中
	Ts_Pause,		//暂停中
	Ts_Install,		//安装中
	Ts_Finish,		//完成
	Ts_Error,		//下载失败
    Ts_Remove,		//下载失败, 需要删除
	Ts_Delete,		//删除的下载
	Ts_End,
};

enum CallbackRet
{
	Cr_Continue = 0,	//继续回调
	Cr_Stop,			//停止回调
	Cr_Delete,			//删除下载
};

class ILoadCallback
{
public:
	virtual void LoadCallback(UINT_PTR nTask, void* lpParam, TaskState state, UINT_PTR nData) = 0;
};

typedef CallbackRet(CALLBACK *LoadCallback)(UINT_PTR nTask, void* lpParam, TaskState state, UINT_PTR nData);


class CCallbackList
{
public:
	CCallbackList()
	{

	}
	~CCallbackList()
	{

	}
	bool IsEmpty()
	{
		return m_cbMap.empty();
	}
	void Add(LoadCallback pcb, void* lpParam)
	{
		m_cbMap.insert(std::make_pair(pcb, lpParam));
	}
	void Remove(LoadCallback pcb)
	{
		auto itor = m_cbMap.find(pcb);
		if (itor != m_cbMap.end())
		{
			m_cbMap.erase(itor);
		}
	}
	void NotifyCallback(UINT_PTR nTask, TaskState state, UINT_PTR nData)
	{
		if (m_cbMap.empty())
			return;
		auto itor = m_cbMap.begin();
		for (; itor != m_cbMap.end(); ++itor)
		{
			itor->first(nTask, itor->second, state, nData);
		}
	}

protected:

private:
	map<LoadCallback, void*> m_cbMap;
};