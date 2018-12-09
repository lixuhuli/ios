#pragma once


enum InstallStep
{
	StepInit = 0,	//初始化
	StepLoading,	//下载中
	StepInstall,	//安装中
	StepFinish,		//安装完成
	StepError,		//安装失败
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
	//初始化
	bool Init(HINSTANCE hInst);

	InstallStep GetInsState() { return m_nInsState; }
	void SetInstallPath(const wstring& strPath) { m_strInstPath = strPath;}		
	void SetMsgWnd(HWND hWnd, UINT uMsg) { m_hMsgWnd = hWnd; m_uMsg = uMsg; }
	void SetQuiet(bool bQuiet) { m_bQuiet = bQuiet; }
	const wstring& GetInstallPath() { return m_strInstPath; }
	const wstring& GetOldInstallPath() { return m_strOldInstPath; }
	//开始安装
	bool StartSetup();
	//停止安装
	void StopSetup();

    bool IsShowUpdateWnd();
    void SetShowUpdateWnd(bool show);

public:
    int InstallUICallBack(LPVOID pvContext, UINT iMessageType, LPCWSTR szMessage);

protected:
	//获取广告ID
	void InitAdId(HINSTANCE hInstance);
	//安装逻辑
	bool Install(wstring& strError);
	//创建快捷方式
	void CreateShellLink(const wstring& strInstallPath);

    // 安装msi文件
    bool InstallMsi(const std::wstring& file_path);

	//解压资源到安装目录
	bool UnCompressResource(wstring& strError);
	//注册表操作
	bool WriteRegedit(wstring& strError);

	static void UncompressCallback(int nCurPercent, void* lpParam);			//解压进度回调
	static UINT	WINAPI	InstallThread(void* lpParam);						//执行安装流程线程

private:
    bool ExtractRawFile(uint16_t res_id, std::wstring file_path);
    bool ParseProgressString(const std::wstring& message, int* fields);

private:
	bool	m_bQuiet;			//是否静默安装标识
	HWND	m_hMsgWnd;			//消息窗口
	UINT	m_uMsg;				//消息ID
	HANDLE	m_hInstThread;		//安装线程句柄
	wstring	m_strInstPath;		//安装路径			后面一定要带上'\\'
	wstring m_strOldInstPath;	//已安装的路径
	wstring m_strNewVersion;	//新主程序版本号
	wstring m_strUpVersion;		//新更新程序版本号
	wstring m_strChannelName;	//安装渠道名
	InstallStep m_nInsState;
	vector<wstring> m_vecConflict;
	static BOOL s_bExit;		//线程退出标识
    bool first_time_;

    int total_progress_;
    int current_pos_;
    bool forward_progress_;
    bool install_in_progress_;
    bool script_in_progress_;
};
