#pragma once
#include "Task.h"

enum Tag_HelperThread {
	Tht_Unkonw = 0,
	Tht_Url,

    Tht_Icon,
};

// ��ҳ�����װ����ѯ���ص�ַ�߳�
class CWebHelperThread
	: public PublicLib::CThread {
public:
	CWebHelperThread();
	~CWebHelperThread();
	ITask* GetTask();
	Tag_HelperThread GetTag();
	BOOL Init(HWND hWnd, UINT uMsg, ITask* pTask, Tag_HelperThread tag);

protected:
	virtual int Run(void * argument);//�߳�ִ�к���

	void RequestUrl();
    void RequestGameIconUrl();
	GameType GetGameTypeByTagList(const string& strJson);

    void DownLoadIcon(PublicLib::CHttpClient& http, Json::Value& vData);

private:
	HWND m_hWnd;
	UINT m_uMsg;
	ITask* m_pTask;
	Tag_HelperThread m_tag;
};