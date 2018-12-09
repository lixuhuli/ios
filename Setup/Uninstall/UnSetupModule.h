#pragma once

enum {
	Step_Init = 0,
	Step_DelReg,
	Step_DelShortcut,
	Step_DelFiles,
	Step_Finish,
};


class CUnSetupModule
{
protected:
	CUnSetupModule(void);
	~CUnSetupModule(void);

public:
	static CUnSetupModule*	Instance()
	{
		static CUnSetupModule instance;
		return &instance;
	}
	void SetMsg(HWND hWnd, UINT uMsg);
	const wstring& GetInstallPath()const				{ return m_strInstallPath; }

	bool GetInstallInfo();
	bool StartUnSetup();											//�����߳�ж��
	void UnSetup(wstring& strError);								//ж�����̺���

protected:
	void EnumFiles(const wchar_t* pDirPath);					//ö�ٰ�װ·���µ������ļ��С��ļ�
	void ClearShortcut();										//�����ݷ�ʽ
	void ClearShortcutSubstitutedProduct();
	void RemoveFiles();											//ɾ���ļ�
	static UINT WINAPI UninstThread(void* lpParam);				//ж���̺߳���

private:
	UINT m_uMsg;
	HWND m_hMsgWnd;		//��Ϣ���մ���
	HANDLE	m_hThread;	//ж���߳̾��
	wstring m_strVersion;		//����汾��
	wstring	m_strInstallPath;	//��װλ��
	wstring	m_strChannelName;	//��������
	vector<wstring>	m_vecDirs;	//ж��·�����ļ����б�
	vector<wstring>	m_vecFiles;	//ж��·�����ļ��б�

};
