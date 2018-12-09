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
	bool StartUnSetup();											//开启线程卸载
	void UnSetup(wstring& strError);								//卸载流程函数

protected:
	void EnumFiles(const wchar_t* pDirPath);					//枚举安装路径下的所有文件夹、文件
	void ClearShortcut();										//清理快捷方式
	void ClearShortcutSubstitutedProduct();
	void RemoveFiles();											//删除文件
	static UINT WINAPI UninstThread(void* lpParam);				//卸载线程函数

private:
	UINT m_uMsg;
	HWND m_hMsgWnd;		//消息接收窗口
	HANDLE	m_hThread;	//卸载线程句柄
	wstring m_strVersion;		//程序版本号
	wstring	m_strInstallPath;	//安装位置
	wstring	m_strChannelName;	//渠道名称
	vector<wstring>	m_vecDirs;	//卸载路径下文件夹列表
	vector<wstring>	m_vecFiles;	//卸载路径下文件列表

};
