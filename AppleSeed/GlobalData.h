#pragma once

/*
* VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
* 0x40 : unassigned
* VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
*/
enum
{
    G_VK_0 = 0x30,
    G_VK_1 = 0x31,
    G_VK_2,
    G_VK_3,
    G_VK_4,
    G_VK_5,
    G_VK_6,
    G_VK_7,
    G_VK_8,
    G_VK_9 = 0x39,
};

enum
{
    G_VK_A = 0x41,
    G_VK_B,
    G_VK_C,
    G_VK_D,
    G_VK_E,
    G_VK_F,
    G_VK_G,
    G_VK_H,
    G_VK_I,
    G_VK_J,
    G_VK_K,
    G_VK_L,
    G_VK_M,
    G_VK_N,
    G_VK_O,
    G_VK_P,
    G_VK_Q,
    G_VK_R,
    G_VK_S,
    G_VK_T,
    G_VK_U,
    G_VK_V,
    G_VK_W,
    G_VK_X,
    G_VK_Y,
    G_VK_Z = 0x5A,
};

static const int g_nMaxSpeed = 4096;
class CGlobalData {
public:
	static CGlobalData* Instance() {
		static CGlobalData obj;
		return &obj;
	}
	

public:
	CGlobalData();
	virtual ~CGlobalData();

public:
    HWND GetMainWnd() { return m_hWndMain; }
    void SetMainWnd(HWND hWnd) { m_hWndMain = hWnd; }
    void SetMainWndPtr(CWndBase* pWnd) { m_pWndMain = pWnd; }
    CWndBase* GetMainWndPtr();

    //接口公共参数
    void InitPhpParams(OUT Json::Value& vRoot);

public:
    void Init();
    void Exit();

    const wstring& GetRunPath()const { return m_strRunPath; }
    const wstring& GetAppVersion()const { return m_strAppVersion; }
    const wstring& GetGuid()const { return m_strGuid; }
    const wstring& GetMac()const { return m_strMacAddr; }

    const wstring& GetOSName()const { return m_strOSName; }
    const wstring& GetOSVersion()const { return m_strOSVersion; }
    const wstring& GetOSType()const { return m_strOSType; }

    const wstring& GetDocPath()const;
    wstring GetDefLoadPath();

    const string &GetUserIcoPath() const { return m_strUserIcoPath; }
    void SetUserIcoPath(const string &val) { m_strUserIcoPath = val; }

    const bool& NeedReboot() const { return need_reboot_; }
    void SetNeedReboot(const bool &val) { need_reboot_ = val; }

    const bool& ProgrammeMode() const { return programme_mode_; }
    void SetProgrammeMode(const bool &val) { programme_mode_ = val; }

    const int& GetCpuType() const { return cpu_type_; }

    std::wstring GetKeyboardStr(int key_code);

    bool IsCheckShowPerOptimiz();
    void SetCheckShowPerOptimiz(bool check);

public:
    void WriteSerialToDocument(const char* serial, size_t size);

private:
    void GetCpuInfo();

protected:
    void InitKeyboardMapping();

private:
    wstring m_strRunPath;
    wstring m_strAppVersion;
    wstring m_strGuid;
    wstring m_strMacAddr;

    wstring m_strOSName;
    wstring m_strOSVersion;
    wstring m_strOSType;

    HWND m_hWndMain;
    HWND m_hWndIos;

    wstring m_strDataPath;
    wstring m_strDocPath;

    int cpu_type_;   // 0:intel  1:AMD

    CWndBase *m_pWndMain;

    string m_strUserIcoPath;

    bool need_reboot_;
    bool programme_mode_;

    map<int, std::wstring> keyboard_mapping_;
};
