#include "StdAfx.h"
#include "UserData.h"
#include "GlobalData.h"

#define INI_APP_COOKIE			    L"Cookie"
#define INI_KEY_COOKIE_ID		    L"cookie_id"
#define INI_KEY_COOKIE_MD5		    L"cookie_md5"
#define INI_KEY_COOKIE_PHONE	    L"cookie_phone"
#define INI_KEY_COOKIE_PASSWORD	    L"cookie_password"
#define INI_KEY_COOKIE_AUTOLOGIN    L"cookie_autologin"
#define INI_KEY_COOKIE_REMEMBERPWD  L"cookie_rememberpwd"

#define INI_USER_DATA			    L"UserData"
#define INI_KEY_HEAD_FILE		    L"HeadIconFile"
#define INI_KEY_HEAD_URL		    L"HeadIconUrl"

#define USER_COOKIE_PATH		    L"UserCookie"

CUserData::CUserData()
 : m_userState(UsNone)
 , m_nUserID(0)
 , m_bAutoLogin(false)
 , m_bRememberPwd(false) {
}


CUserData::~CUserData() {
}

void CUserData::Init() {
    wstring strIniPath = GetAppDataPath() + USER_COOKIE_PATH;
    SHCreateDirectory(NULL, strIniPath.c_str());
    strIniPath.append(L"\\CefCookies_Data");

    if (PathFileExists(strIniPath.c_str())) {
        wchar_t szValue[128] = { 0 };
        DWORD dwLen = GetPrivateProfileString(INI_APP_COOKIE, INI_KEY_COOKIE_ID, L"", szValue, 128, strIniPath.c_str());
        int nUid = _wtoi(szValue);
        if (nUid > 0) {
            dwLen = GetPrivateProfileString(INI_APP_COOKIE, INI_KEY_COOKIE_MD5, L"", szValue, 128, strIniPath.c_str());
            if (dwLen > 4) {
                m_nUserID = nUid;
                m_strToken = PublicLib::UToA(szValue);

                dwLen = GetPrivateProfileString(INI_APP_COOKIE, INI_KEY_COOKIE_PHONE, L"", szValue, 128, strIniPath.c_str());
                if (dwLen > 0) m_strPhone = PublicLib::UToA(szValue);

                dwLen = GetPrivateProfileString(INI_APP_COOKIE, INI_KEY_COOKIE_PASSWORD, L"", szValue, 128, strIniPath.c_str());
                if (dwLen > 0) m_strPassword = PublicLib::UToA(szValue);

                m_bAutoLogin = (GetPrivateProfileIntW(INI_APP_COOKIE, INI_KEY_COOKIE_AUTOLOGIN, 0, strIniPath.c_str()) == 1);
                m_bRememberPwd = (GetPrivateProfileIntW(INI_APP_COOKIE, INI_KEY_COOKIE_REMEMBERPWD, 0, strIniPath.c_str()) == 1);
            }
        }
    }

    ClearUserInfo();
}

void CUserData::Exit() {
	
}

void CUserData::ClearAccount() {
    m_strUserPath.clear();
}

void CUserData::SetFileAccount(int nUid, const string& strToken) {
    if (m_nUserID == nUid && m_strToken == strToken) return;

    m_nUserID = nUid;
    m_strToken = strToken;
    wstring strIniPath = GetAppDataPath() + USER_COOKIE_PATH;
    SHCreateDirectory(NULL, strIniPath.c_str());
    strIniPath.append(L"\\CefCookies_Data");
    wchar_t szValue[128] = { 0 };
    swprintf_s(szValue, L"%d", nUid);
    WritePrivateProfileString(INI_APP_COOKIE, INI_KEY_COOKIE_ID, szValue, strIniPath.c_str());
    swprintf_s(szValue, L"%s", PublicLib::AToU(strToken).c_str());
    WritePrivateProfileString(INI_APP_COOKIE, INI_KEY_COOKIE_MD5, szValue, strIniPath.c_str());
}

void CUserData::SetAccountPhone(const string& strPhone) {
    if (m_strPhone == strPhone) return;

    m_strPhone = strPhone;
    wstring strIniPath = GetAppDataPath() + USER_COOKIE_PATH;
    SHCreateDirectory(NULL, strIniPath.c_str());
    strIniPath.append(L"\\CefCookies_Data");
    wchar_t szValue[128] = { 0 };
    swprintf_s(szValue, L"%s", PublicLib::AToU(strPhone).c_str());
    WritePrivateProfileString(INI_APP_COOKIE, INI_KEY_COOKIE_PHONE, szValue, strIniPath.c_str());
}

void CUserData::SetAccountPassword(const string& strPassword) {
    if (m_strPassword == strPassword) return;

    m_strPassword = strPassword;
    wstring strIniPath = GetAppDataPath() + USER_COOKIE_PATH;
    SHCreateDirectory(NULL, strIniPath.c_str());
    strIniPath.append(L"\\CefCookies_Data");
    wchar_t szValue[128] = { 0 };
    swprintf_s(szValue, L"%s", PublicLib::AToU(strPassword).c_str());
    WritePrivateProfileString(INI_APP_COOKIE, INI_KEY_COOKIE_PASSWORD, szValue, strIniPath.c_str());
}

void CUserData::SetAutoLogin(const bool& bAutoLogin) {
    if (m_bAutoLogin == bAutoLogin) return;

    m_bAutoLogin = bAutoLogin;
    wstring strIniPath = GetAppDataPath() + USER_COOKIE_PATH;
    SHCreateDirectory(NULL, strIniPath.c_str());
    strIniPath.append(L"\\CefCookies_Data");
    wchar_t szValue[128] = { 0 };
    swprintf_s(szValue, L"%d", bAutoLogin ? 1 : 0);
    WritePrivateProfileString(INI_APP_COOKIE, INI_KEY_COOKIE_AUTOLOGIN, szValue, strIniPath.c_str());
}

void CUserData::SetRememberPwd(const bool& bRememberPwd) {
    if (m_bRememberPwd == bRememberPwd) return;

    m_bRememberPwd = bRememberPwd;
    wstring strIniPath = GetAppDataPath() + USER_COOKIE_PATH;
    SHCreateDirectory(NULL, strIniPath.c_str());
    strIniPath.append(L"\\CefCookies_Data");
    wchar_t szValue[128] = { 0 };
    swprintf_s(szValue, L"%d", bRememberPwd ? 1 : 0);
    WritePrivateProfileString(INI_APP_COOKIE, INI_KEY_COOKIE_REMEMBERPWD, szValue, strIniPath.c_str());
}

bool CUserData::GetLastHead(OUT wstring& strIcoPath, OUT wstring& strIcoUrl) {
    auto strUserPath = GetUserPath();
    strUserPath.append(L"\\data.bat");

    if (!PathFileExists(strUserPath.c_str())) return false;

    wchar_t szValue[MAX_PATH + 1] = { 0 };
    DWORD dwLen = GetPrivateProfileString(INI_USER_DATA, INI_KEY_HEAD_FILE, L"", szValue, MAX_PATH + 1, strUserPath.c_str());
    strIcoPath = szValue;

    memset(szValue, 0, sizeof(szValue));
    dwLen = GetPrivateProfileString(INI_USER_DATA, INI_KEY_HEAD_URL, L"", szValue, MAX_PATH + 1, strUserPath.c_str());
    strIcoUrl = szValue;

    return true;
}

void CUserData::RecordLastHead(const std::wstring& icon, const std::wstring& url) {
    if (icon.empty()) return;

    auto strUserPath = GetUserPath();
    strUserPath.append(L"\\data.bat");
    WritePrivateProfileString(INI_USER_DATA, INI_KEY_HEAD_FILE, icon.c_str(), strUserPath.c_str());
    WritePrivateProfileString(INI_USER_DATA, INI_KEY_HEAD_URL, url.c_str(), strUserPath.c_str());
}

void CUserData::ClearUserInfo() {
    // 初始化账号信息
    m_vUserInfo["uid"] = 0;             // 用户名或是手机号
    m_vUserInfo["token"] = "";			// token
    m_vUserInfo["nickname"] = "";		// 昵称
    m_vUserInfo["intro"] = "";
    m_vUserInfo["sex"] = 0;			    // 性别
    m_vUserInfo["country"] = "";        // 国家
    m_vUserInfo["mobile"] = "";			// 手机号码
    m_vUserInfo["address"] = "";		// 剩余升级经验
    m_vUserInfo["avatar"] = "";		    // 头像标识

    m_userState = UsNone;

    Json::FastWriter fw;
    string strJson = fw.write(m_vUserInfo);
    strcpy_s(((LPCEF_SHARE_DATA)g_shareData)->szUserInfo, strJson.c_str());
}

const wstring & CUserData::GetUserPath() {
	if (m_strUserPath.empty()) {
		m_strUserPath = GetAppDataPath() + L"temp";
		SHCreateDirectory(NULL, m_strUserPath.c_str());
	}

	return m_strUserPath;
}

string CUserData::GetUserIp() {
    char buf[MAX_PATH] = { 0 };    // 把网页中读出的数据放在此处
    char chTempIp[128] = { 0 };
    string user_ip;

    CStdString url;
    SYSTEMTIME sys_time;
    ::GetLocalTime(&sys_time);
    url.Format(L"http://%d.ip138.com/ic.asp", sys_time.wYear);

    auto exe_path = GetRunPathA();
    auto download_path = exe_path + "ip_temp.ini";

    PublicLib::CHttpClient http;
    if (!http.DownLoadFile(url.GetData(), PublicLib::AToU(download_path).c_str()))
        return "";

    // 将网页数据写入ip_temp.ini文件中
    FILE *fp = fopen(download_path.c_str(), "r");
    if (fp == nullptr) return "";

    fseek(fp, 0, SEEK_SET);
    fread(buf, 1, MAX_PATH, fp);
    fclose(fp);

    DeleteFileA(download_path.c_str());

    string ip_data = buf;
    auto start = ip_data.find_first_of("[");
    auto end = ip_data.find_first_of("]");

    if (start == string::npos || end == string::npos || start >= end) return "";

    user_ip = ip_data.substr(start + 1, end - start - 1);

    return user_ip;
}

