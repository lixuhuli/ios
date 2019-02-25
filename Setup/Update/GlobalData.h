#pragma once



class CGlobalData
{
public:
	/****************************************************************************
	*需要多次调用的均定义成内联函数，提高执行效率
	*/
	static CGlobalData*	Instance()
	{
		static CGlobalData data;
		return &data;
	}

	void Init(const wstring& strOldInstallPath);
	void Exit();

	void SetCmdLine(const wstring& str) { m_strCmdLine = str; }
	const wstring& GetOldInstallPath()const { return m_strOldInstallPath; }
	const wstring& GetOldVersion()const { return m_strOldVersion; }
	const wstring& GetOldUpExeVersion()const { return m_strOldUpExeVersion; }
	const wstring& GetCmdLine()const { return m_strCmdLine; }
	const wstring& GetChannel()const { return m_strChannel; }
	const wstring& GetGuid()const { return m_strGuid; }
	bool HasInstall()const { return m_bHasInstall; }
	bool IsSelfUpdate()const { return m_strCmdLine.find(L"selfupdate") != wstring::npos; }

    const bool& NeedReboot() const { return need_reboot_; }
    void SetNeedReboot(const bool &val) { need_reboot_ = val; }

    const wstring& GetRebootCmdLine()const { return m_strRebootCmdLine_; }
    void SetRebootCmdLine(const wstring& str) { m_strRebootCmdLine_ = str; }

    const wstring& GetRebootDestExe()const { return m_strRebootDestExe_; }
    void SetRebootDestExe(const wstring& str) { m_strRebootDestExe_ = str; }

protected:
	CGlobalData(void);
	~CGlobalData(void);

public:
	wstring	m_strOldInstallPath;	//读取到的旧安装路径
	wstring	m_strOldVersion;		//读取到的旧版本号
	wstring	m_strOldUpExeVersion;	//自更新程序的版本号
	wstring m_strChannel;
	wstring	m_strCmdLine;
	bool	m_bHasInstall;			//是否已经安装

    bool need_reboot_;
    wstring	m_strRebootCmdLine_;
    wstring	m_strRebootDestExe_;

	wstring m_strGuid;
};


