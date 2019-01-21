#pragma once


enum UserState {
	UsNone = 0,	// ³õÊ¼×´Ì¬
	UsLogin,	// ÕýÊ½ÕËºÅµÇÂ¼
};

class CUserData {
public:
	static CUserData* Instance() {
		static CUserData obj;
		return &obj;
	}

	void Init();
	void Exit();

    void ClearUserInfo();
    string GetUserIp();

    Json::Value& GetUserInfo() { return m_vUserInfo; }

	const wstring &GetUserPath();
	void SetUserPath(const wstring &val) { m_strUserPath = val; }

    UserState GetUserState() const { return m_userState; }
    void SetUserState(UserState val) { m_userState = val; }

    void ClearAccount();
    void SetFileAccount(int nUid, const string& strToken);
    bool HasFileAccount() { return m_nUserID > 0 && !m_strToken.empty(); }

    void SetAccountPhone(const string& strPhone);
    const string& GetFileUserPhone()const { return m_strPhone; }

    void SetAccountPassword(const string& strPassword);
    const string& GetFileUserPassword()const { return m_strPassword; }

    void SetAccount(const wstring& strAccount);
    const wstring& GetAccount()const { return m_strAccount; }

    void SetAutoLogin(const bool& bAutoLogin);
    const bool& GetAutoLogin()const { return m_bAutoLogin; }

    void SetRememberPwd(const bool& bRememberPwd);
    const bool& GetRememberPwd()const { return m_bRememberPwd; }

    int GetFileUserID()const { return m_nUserID; }

    const string& GetFileUserToken()const { return m_strToken; }

    bool GetLastHead(OUT wstring& strIcoPath, OUT wstring& strIcoUrl);
    void RecordLastHead(const std::wstring& icon, const std::wstring& url);

protected:
	CUserData();
	virtual ~CUserData();

private:
    int m_nUserID;
    string m_strToken;
    string m_strPhone;
    string m_strPassword;
    wstring m_strAccount;
    bool m_bAutoLogin;
    bool m_bRememberPwd;

    wstring m_strUserPath;
    UserState m_userState;
    Json::Value m_vUserInfo;
};

