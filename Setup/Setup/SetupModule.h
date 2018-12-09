#pragma once


enum InstallStep
{
	StepInit = 0,	//��ʼ��
	StepLoading,	//������
	StepInstall,	//��װ��
	StepFinish,		//��װ���
	StepError,		//��װʧ��
};


enum MsgWparam
{
	Mw_Init = 0,
	Mw_Unzip,
	Mw_Loading,
	Mw_Regedit,
	Mw_Fail,
	Mw_Finish,
};

class CSetupModule
{
public:
	CSetupModule(void);
	~CSetupModule(void);

	static CSetupModule* Instance()
	{
		static CSetupModule instance;
		return &instance;
	}
	//��ʼ��
	bool Init(HINSTANCE hInst);

	InstallStep GetInsState() { return m_nInsState; }
	void SetInstallPath(const wstring& strPath) { m_strInstPath = strPath;}		
	void SetMsgWnd(HWND hWnd, UINT uMsg) { m_hMsgWnd = hWnd; m_uMsg = uMsg; }
	void SetQuiet(bool bQuiet) { m_bQuiet = bQuiet; }
	const wstring& GetInstallPath() { return m_strInstPath; }
	const wstring& GetOldInstallPath() { return m_strOldInstPath; }
	//��ʼ��װ
	bool StartSetup();
	//ֹͣ��װ
	void StopSetup();

    bool IsShowUpdateWnd();
    void SetShowUpdateWnd(bool show);

public:
    int InstallUICallBack(LPVOID pvContext, UINT iMessageType, LPCWSTR szMessage);

protected:
	//��ȡ���ID
	void InitAdId(HINSTANCE hInstance);
	//��װ�߼�
	bool Install(wstring& strError);
	//������ݷ�ʽ
	void CreateShellLink(const wstring& strInstallPath);

    // ��װmsi�ļ�
    bool InstallMsi(const std::wstring& file_path);

	//��ѹ��Դ����װĿ¼
	bool UnCompressResource(wstring& strError);
	//ע������
	bool WriteRegedit(wstring& strError);

	static void UncompressCallback(int nCurPercent, void* lpParam);			//��ѹ���Ȼص�
	static UINT	WINAPI	InstallThread(void* lpParam);						//ִ�а�װ�����߳�

private:
    bool ExtractRawFile(uint16_t res_id, std::wstring file_path);
    bool ParseProgressString(const std::wstring& message, int* fields);

private:
	bool	m_bQuiet;			//�Ƿ�Ĭ��װ��ʶ
	HWND	m_hMsgWnd;			//��Ϣ����
	UINT	m_uMsg;				//��ϢID
	HANDLE	m_hInstThread;		//��װ�߳̾��
	wstring	m_strInstPath;		//��װ·��			����һ��Ҫ����'\\'
	wstring m_strOldInstPath;	//�Ѱ�װ��·��
	wstring m_strNewVersion;	//��������汾��
	wstring m_strUpVersion;		//�¸��³���汾��
	wstring m_strChannelName;	//��װ������
	InstallStep m_nInsState;
	vector<wstring> m_vecConflict;
	static BOOL s_bExit;		//�߳��˳���ʶ
    bool first_time_;

    int total_progress_;
    int current_pos_;
    bool forward_progress_;
    bool install_in_progress_;
    bool script_in_progress_;
};
