#pragma once


enum{
	Step_None = 0,
	Step_Check,
	Step_Init,
	Step_Load,
	Step_Install,
	Step_Rollback,
	Step_Regedit,
	Step_Finish,
};


//�滻�ļ��б�ڵ���Ϣ
typedef struct _REPLACE_FILE_ITEM
{
	wstring strResPath;		//Դ�ļ�·��
	wstring strDesPath;		//Ŀ���ļ�·��
	wstring strBackupPath;	//�����ļ�·��
	wstring strDir;
	bool	bReplace;

}REPLACE_FILE_ITEM, *LPREPLACE_FILE_ITEM;

class CUpdateModule
	: public PublicLib::CHttpClientStopCallback
{
public:
	static CUpdateModule*	Instance()
	{
		static CUpdateModule module;
		return &module;
	}
	//****************************************************
	//��������
	wstring	GetInstallPath()							{ return m_strInstallPath; }
	void SetMsg(HWND hWnd, UINT uMsg);
	void StartUpdate();															//�����̸߳���
	void StopUpdate();															//�رո����߳�
	void StartCheckUpdate();													//�����̼߳�����

	const wstring &GetRootUrl() const { return m_strRootUrl; }
	void SetRootUrl(const wstring &val) { m_strRootUrl = val; }
	const wstring &GetUpdateDesc() const { return m_strUpdateDesc; }
	void SetUpdateDesc(const wstring &val) { m_strUpdateDesc = val; }
	const wstring &GetUpdateDate() const { return m_strUpdateDate; }
	void SetUpdateDate(const wstring &val) { m_strUpdateDate = val; }
	const wstring &GetUpdateVersion() const { return m_strUpdateVersion; }
	void SetUpdateVersion(const wstring &val) { m_strUpdateVersion = val; }
	int GetUpdateStatus() const { return m_nUpdateStatus; }
	void SetUpdateStatus(int val) { m_nUpdateStatus = val; }


protected:
	CUpdateModule();
	~CUpdateModule();
	static UINT __stdcall UpdateThread(void* lpParam);								//�����̺߳���
	static UINT __stdcall CheckUpdateThread(void* lpParam);							//�������߳�
	bool	Update(wstring& strError);												//�������̺���

	//**********************************************************************
	//*���µļ�������
	bool	DownloadPackagePart(wstring& strError);									//���ظ��£�������������
	bool	ReplaceFiles(wstring& strError);										//ִ���滻�ļ�
	bool	Rollback(wstring& strError);											//ִ�лع�����
	bool	UpdateRegedit(wstring& strError);										//����ע�����Ϣ

	//***********************************************************************
	virtual void OnHttpLoading(const DWORD dwTotalSize, const DWORD dwLoadSize, void* lpParam);	//���صĻص�
	virtual BOOL IsNeedStop();																	//���ػص����Ƿ�ֹͣ����	
	void	ReleaseReplaceList();																//�ͷŸ����б����ݽṹ
	void	UpdateShortcuts();																	//���¿�ݷ�ʽ


private:
	HWND m_hMsgWnd;				//��Ϣ���մ���	
	UINT m_uMsg;				//��ϢID
	HANDLE	m_hThread;			//�����߳̾��
	HANDLE	m_hCheckThread;		//�������߳̾��
	wstring m_strPackagePath;	//���ذ�װ����λ��
	wstring	m_strInstallPath;	//����װλ��
	wstring	m_strTempPath;		//������ʱ�ļ�λ��

	int m_nUpdateStatus;
	wstring	m_strRootUrl;//���������³����Ŀ¼
	wstring m_strUpdateDesc;//����˵��
	wstring m_strUpdateDate;//����ʱ��
	wstring m_strUpdateVersion;//���°汾��
	list<LPREPLACE_FILE_ITEM>	m_ReplaceList;	//��Ҫ�����滻���ļ��б�
	static BOOL s_bExit;		//�߳��˳���ʶ
};
