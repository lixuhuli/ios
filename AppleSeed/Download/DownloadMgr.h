#pragma once
#include "DownloadList.h"
#include "HttpCache.h"
#include "WebHelper.h"
#include "ThreadMgr.h"

enum {
	GameUnload = 0,
	GameLoading,
	GameInstalling,
	GameInstalled,
	GameUpdate,
};

class AndroidMgr;
class CDownloadMgr
{
public:
	static CDownloadMgr* Instance()
	{
		static CDownloadMgr obj;
		return &obj;
	}
	bool Init(HINSTANCE hInst, size_t nMaxTaskCount, size_t nMaxLoadSpeed);
	void Exit();
	void SetLoadCallback(ILoadCallback* pCallback, void* lpParam);//�������ش��ڻص�
	void SetMaxLoadSpeed(int nSpeed);
	void DetachCallback();//�Ƴ����ش��ڻص�
    UINT_PTR AddTask(const Tag_HelperThread& tag, __int64 nGameID, const wstring& strName, int nLoadWay = 1);
	void DeleteLoadTask(UINT_PTR nTaskID, BOOL bDeleteDb, BOOL bUpdate);//ɾ�������е�����
	void DeleteFinishTask(UINT_PTR nTaskID, BOOL bDeleteDb);//ɾ������ɵ�����
	bool PauseTask(UINT_PTR nTaskID, BOOL bUpdateWaitList=TRUE);//��ͣ����
	bool ReloadTask(UINT_PTR nTaskID);//��������
	bool PauseWaitTask(UINT_PTR nTaskID);
	void UpdateWaitList();//���µȴ��б�
	int GetGameStatus(__int64 nGameID, const wstring& strVersion);
	bool ReInstallFinishTask(UINT_PTR nTaskID);
	const TaskList& GetLoadList()const	{ return m_taskList.GetLoadList(); }
	const TaskList& GetFinishList()const { return m_taskList.GetFinishList(); }
    const HWND& GetMsgWnd() { return m_hMsgWnd; }

    UINT_PTR DownloadIcon(const __int64& nGameID, const wstring& strName);

protected:
	CDownloadMgr();
	~CDownloadMgr();
	void LoadDbTask();
	bool StartTask(ITask* pTask);		//��ʼ����
	void NotifyCallback(ITask* pTask, UINT_PTR nData = 0);	//�ص�����״̬��ֻ�����߳��е���
	bool TryStartTask(ITask* pTask);	//���Կ�������
	void InstallTask(ITask* pTask);		//��װ����
	void FinishTask(ITask* pTask);		//�������
	void CheckUrl(ITask* pTask, Tag_HelperThread tag);
    void CheckIconUrl(ITask* pTask, Tag_HelperThread tag);
    int GetGameStatus(ITask* pTask);

	void PostProgressMsg(ITask* pTask);
	void PostFinishMsg(ITask* pTask);
	void PostErrorMsg(ITask* pTask);

	HWND CreateMessageWindow(HINSTANCE hInstance);
	static LRESULT __stdcall MessageWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void LoadCallback(PublicLib::EnumDownloadState state, double dltotal, double dlnow, void * Userdata);

	LRESULT OnMsgLoadProgress(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgLoadFinish(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgLoadError(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgTryStartTask(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgWebHelper(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgIpaInstalling(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgIpaInstall(WPARAM wParam, LPARAM lParam);
    LRESULT OnMsgInstallApking(WPARAM wParam, LPARAM lParam);
    LRESULT OnMsgInstallApk(WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);

private:
	HWND m_hMsgWnd;
	size_t m_nMaxLoadCount;		// ���ͬʱ������Ŀ
	size_t m_nMaxLoadSpeed;		// ��������ٶȣ�0��������
	CDownloadList m_taskList;
	CHttpCache m_httpCache;
	void* m_lpParam;
	ILoadCallback* m_pLoadCallback;
	ThreadMgr<CWebHelperThread> m_webThMgr;
    bool can_play_ready_;

    TaskMap list_icon_;
	static volatile BOOL m_bNeedExit;
};


#include "DownloadMgr.inl"