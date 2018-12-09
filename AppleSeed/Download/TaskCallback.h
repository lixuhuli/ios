#pragma once
#include <map>
using std::map;


//����״̬
enum TaskState
{
	Ts_Init = 0,	//��ʼ״̬
	Ts_Check,		//��ѯ���ص�ַ
	Ts_CheckEnd,	//��ѯ���
	Ts_Wait,		//�ȴ�����
	Ts_Loading,		//������
	Ts_Pause,		//��ͣ��
	Ts_Install,		//��װ��
	Ts_Finish,		//���
	Ts_Error,		//����ʧ��
    Ts_Remove,		//����ʧ��, ��Ҫɾ��
	Ts_Delete,		//ɾ��������
	Ts_End,
};

enum CallbackRet
{
	Cr_Continue = 0,	//�����ص�
	Cr_Stop,			//ֹͣ�ص�
	Cr_Delete,			//ɾ������
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