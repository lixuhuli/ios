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


//替换文件列表节点信息
typedef struct _REPLACE_FILE_ITEM
{
	wstring strResPath;		//源文件路径
	wstring strDesPath;		//目标文件路径
	wstring strBackupPath;	//备份文件路径
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
	//内联函数
	wstring	GetInstallPath()							{ return m_strInstallPath; }
	void SetMsg(HWND hWnd, UINT uMsg);
	void StartUpdate();															//开启线程更新
	void StopUpdate();															//关闭更新线程
	void StartCheckUpdate();													//开启线程检测更新

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
	static UINT __stdcall UpdateThread(void* lpParam);								//更新线程函数
	static UINT __stdcall CheckUpdateThread(void* lpParam);							//检测更新线程
	bool	Update(wstring& strError);												//更新流程函数

	//**********************************************************************
	//*更新的几个步奏
	bool	DownloadPackagePart(wstring& strError);									//下载更新，增量升级部分
	bool	ReplaceFiles(wstring& strError);										//执行替换文件
	bool	Rollback(wstring& strError);											//执行回滚操作
	bool	UpdateRegedit(wstring& strError);										//更新注册表信息

	//***********************************************************************
	virtual void OnHttpLoading(const DWORD dwTotalSize, const DWORD dwLoadSize, void* lpParam);	//下载的回调
	virtual BOOL IsNeedStop();																	//下载回调，是否停止更新	
	void	ReleaseReplaceList();																//释放更新列表数据结构
	void	UpdateShortcuts();																	//更新快捷方式


private:
	HWND m_hMsgWnd;				//消息接收窗口	
	UINT m_uMsg;				//消息ID
	HANDLE	m_hThread;			//更新线程句柄
	HANDLE	m_hCheckThread;		//检测更新线程句柄
	wstring m_strPackagePath;	//下载安装包的位置
	wstring	m_strInstallPath;	//程序安装位置
	wstring	m_strTempPath;		//下载临时文件位置

	int m_nUpdateStatus;
	wstring	m_strRootUrl;//服务器更新程序根目录
	wstring m_strUpdateDesc;//更新说明
	wstring m_strUpdateDate;//更新时间
	wstring m_strUpdateVersion;//更新版本号
	list<LPREPLACE_FILE_ITEM>	m_ReplaceList;	//需要更新替换的文件列表
	static BOOL s_bExit;		//线程退出标识
};
