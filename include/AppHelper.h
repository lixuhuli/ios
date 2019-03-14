#pragma once
#include <Shlobj.h>
#include <ShellAPI.h>
#include <CommDlg.h>
#include <string>
#include <shlwapi.h>
#include <tchar.h>
using std::string;
using std::wstring;
#pragma comment(lib, "Version.lib")

#define APP_DIR_NAME		L"AppleSeed"



inline void ActiveWnd(HWND hWnd)
{
	if (IsIconic(hWnd))
	{
		::PostMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, NULL);
	}
	if (!IsWindowVisible(hWnd))
	{
		::ShowWindow(hWnd, SW_SHOW);
	}
	::SetForegroundWindow(hWnd);
}
//获取exe运行路径 最后面带 '\'
inline string GetRunPathA()
{
	char szPath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, szPath, MAX_PATH);
	string strPath(szPath);
	int nPos = strPath.find_last_of("\\");
	strPath = strPath.substr(0, nPos + 1);
	return strPath;
}

//获取exe运行路径 最后面带 '\'
inline wstring GetRunPathW()
{
	wchar_t szPath[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, szPath, MAX_PATH);
	wstring strPath(szPath);
	int nPos = strPath.find_last_of(L"\\");
	strPath = strPath.substr(0, nPos + 1);
	return strPath;
}

//获取应用程序文档路径
inline wstring GetDocumentPath()
{
	wchar_t szUserFolder[MAX_PATH] = { 0 };
	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szUserFolder);
	std::wstring strFolder = szUserFolder;
	strFolder.append(L"\\");
	strFolder.append(APP_DIR_NAME);
	SHCreateDirectory(NULL, strFolder.c_str());
	return strFolder;
}

inline wstring GetSysTempPath()
{
	wchar_t szTemp[MAX_PATH] = { 0 };
	GetTempPath(MAX_PATH, szTemp);
	return wstring(szTemp);
}

//获取patch路径
inline wstring GetPatchPath()
{
	wstring strPatchPath = GetDocumentPath() + L"\\Patch";
	SHCreateDirectory(NULL, strPatchPath.c_str());
	return strPatchPath;
}

//获取应用程序日志路径
inline wstring GetLogPath()
{
	wstring strLogPath = GetDocumentPath() + L"\\Log";
	SHCreateDirectory(NULL, strLogPath.c_str());
	return strLogPath;
}

//获取应用程序数据路径 带"\\"
inline wstring GetAppDataPath()
{
	wchar_t szPath[MAX_PATH] = { 0 };
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);
	wstring strDataPath(szPath);
	strDataPath.append(L"\\");
	strDataPath.append(APP_DIR_NAME);
	SHCreateDirectory(NULL, strDataPath.c_str());
	strDataPath.append(L"\\");
	return strDataPath;
}

//获取游戏图标下载路径
inline wstring GetGamesIcoPath()
{
	wstring strPath = GetAppDataPath() + L"Icons\\Download";
	if (!PathFileExists(strPath.c_str()))
		SHCreateDirectory(NULL, strPath.c_str());
	return strPath;
}

//获取130*130尺寸游戏图标路径
inline wstring GetGamesIco130Path()
{
    wstring strPath = GetAppDataPath() + L"Icons\\130";
    if (!PathFileExists(strPath.c_str()))
        SHCreateDirectory(NULL, strPath.c_str());
    return strPath;
}

inline wstring GetPlayerIcoPath()
{
	wstring strPath = GetAppDataPath() + L"Icons\\Player\\";
	if (!PathFileExists(strPath.c_str()))
		SHCreateDirectory(NULL, strPath.c_str());
	return strPath;
}

//获取游戏图标下载路径
inline wstring GetGamesLogoPath()
{
    wstring strPath = GetAppDataPath() + L"logos\\Download";
    if (!PathFileExists(strPath.c_str()))
        SHCreateDirectory(NULL, strPath.c_str());
    return strPath;
}

//获取应用程序默认安装位置
inline wstring GetProgramFilePath(bool bIs64Bit)
{
	wchar_t szPath[MAX_PATH] = { 0 };
	if (bIs64Bit)
		ExpandEnvironmentStrings(L"%ProgramW6432%", szPath, MAX_PATH);
	else
		ExpandEnvironmentStrings(L"%ProgramFiles%", szPath, MAX_PATH);
	return szPath;//CSIDL_PROGRAM_FILESX86
}

inline wstring GetApplicationVersion(const wstring& strFileName = L"") {
    wchar_t FileName[MAX_PATH] = { 0 };
    if (strFileName.empty()) {
        if (GetModuleFileNameW(NULL, FileName, MAX_PATH) <= 0) return L"";
    }
    else wsprintf(FileName, L"%s", strFileName.c_str());

    std::wstring Version;
    DWORD dwHandle;
    DWORD VerInfoSize = GetFileVersionInfoSize(FileName, &dwHandle);
    if (VerInfoSize)
    {
        wchar_t* VerInfo = new wchar_t[VerInfoSize / sizeof(wchar_t)];
        if (GetFileVersionInfo(FileName, NULL, VerInfoSize, VerInfo))
        {
            VS_FIXEDFILEINFO* pFileInfo;
            UINT FileInfoLength;
            if (VerQueryValue(VerInfo, L"\\", (LPVOID*)&pFileInfo, &FileInfoLength))
            {
                wchar_t szVersion[100] = { 0 };
                swprintf_s(szVersion, L"%d.%d.%d.%d", HIWORD(pFileInfo->dwProductVersionMS),
                    LOWORD(pFileInfo->dwProductVersionMS),
                    HIWORD(pFileInfo->dwProductVersionLS),
                    LOWORD(pFileInfo->dwProductVersionLS));
                Version = szVersion;
            }
        }
        delete[] VerInfo;
    }

	return Version;
}

//打开Windows浏览文件框   图片(*.jpg)\0*.jpg\0\0
inline bool	ShowSelectFileDlg(HWND hWnd, LPCWSTR lpFilter, OUT wstring& strPath)
{
	wstring strBootPath;
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH + 1] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
	ofn.lpstrFilter = lpFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	//ofn.lpstrInitialDir = kInfo.bootpath.c_str();    
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
	if (GetOpenFileName(&ofn))
	{
		strPath = ofn.lpstrFile;
		return true;
	}
	return false;
}

inline bool	ShowSelectMultipleFileDlg(HWND hWnd, LPCWSTR lpFilter, OUT vector<wstring>& fileList)
{
	wstring strBootPath;
	OPENFILENAME ofn;
	TCHAR szFile[200 * MAX_PATH] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
	ofn.lpstrFilter = lpFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	//ofn.lpstrInitialDir = kInfo.bootpath.c_str();    
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
	if (GetOpenFileName(&ofn))
	{
		wchar_t szPath[MAX_PATH + 1] = { 0 };
		lstrcpyn(szPath, szFile, ofn.nFileOffset);
		szPath[ofn.nFileOffset] = '\0';
		int nLen = lstrlen(szPath);
		if (szPath[nLen - 1] != '\\')
		{
			lstrcat(szPath, TEXT("\\"));
		}
		wchar_t* p = szFile + ofn.nFileOffset; //把指针移到第一个文件
		wstring strPath;
		while (*p)
		{
			strPath = szPath;
			strPath.append(p);
			fileList.push_back(strPath);
			p += lstrlen(p) + 1;				//移至下一个文件
		}
		return true;
	}
	return false;
}

//打开Windows保存文件框   图片(*.jpg)\0*.jpg\0\0
inline bool	ShowSaveFileDlg(HWND hWnd, LPCWSTR lpFilter, IN OUT wstring& strPath)
{
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH + 1] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
	ofn.lpstrFilter = lpFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
	wsprintf(szFile, strPath.c_str());
	if (GetSaveFileName(&ofn))
	{
		strPath = ofn.lpstrFile;
		return true;
	}
	return false;
}

//打开Windows文件夹浏览框
inline bool	ShowBrowserFolderDlg(HWND hWnd, OUT wstring& strPath)
{
	BROWSEINFO BrowseInfo = { 0 };
	LPITEMIDLIST pItemIdList = NULL;
	WCHAR szPath[MAX_PATH] = { 0 };
	BrowseInfo.hwndOwner = hWnd;
	BrowseInfo.lpszTitle = L"浏览：";
	BrowseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	pItemIdList = SHBrowseForFolder(&BrowseInfo);
	if (NULL == pItemIdList || !IsWindow(hWnd))
		return false;
	SHGetPathFromIDList(pItemIdList, szPath);//把项目标识列表转化成目录       
	strPath = szPath;
	if (strPath[strPath.size() - 1] != '\\')
		strPath.append(L"\\");
	return true;
}

inline void GetDesktopWndRect(OUT RECT& rc)
{
	APPBARDATA data = { sizeof(APPBARDATA) };
	SHAppBarMessage(ABM_GETTASKBARPOS, &data);
	int nBarWidth = data.rc.right - data.rc.left;
	int nBarHeight = data.rc.bottom - data.rc.top;
	RECT rcDestTop = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
	switch (data.uEdge)
	{
	case ABE_LEFT: rcDestTop.left += nBarWidth; break;
	case ABE_TOP: rcDestTop.top += nBarHeight; break;
	case ABE_RIGHT: rcDestTop.right -= nBarWidth; break;
	case ABE_BOTTOM: rcDestTop.bottom -= nBarHeight; break;
	default:
		break;
	}
	rc = rcDestTop; 
}

inline bool CreateGuid(OUT wstring& strGuid)
{
	GUID id;
	WCHAR buff[40] = { '\0' };
	CHAR buff2[40] = { '\0' };
	if (FAILED(CoCreateGuid(&id)))
		return false;
	StringFromGUID2(id, buff, 40);
	strGuid.assign(buff);
	strGuid = strGuid.substr(1, strGuid.size() - 2);
	return true;
}

inline bool GetDeviceGuid(OUT wstring& strGuid)
{
	wstring strGuidPath = GetAppDataPath() + L"\\guid";
	FILE* fp = NULL;
	if (PathFileExists(strGuidPath.c_str()))
	{
		_wfopen_s(&fp, strGuidPath.c_str(), L"rb");
		if (NULL == fp)
			return false;
		wchar_t szBuffer[40] = { 0 };
		fread(szBuffer, sizeof(wchar_t)* 40, 1, fp);
		strGuid.assign(szBuffer);
		fclose(fp);
		return true;
	}
	//文件不存在，创建GUID
	if (!CreateGuid(strGuid))
	{
		OutputDebugString(L"GetDeviceGuid:创建GUID失败\n");
		return false;
	}
	_wfopen_s(&fp, strGuidPath.c_str(), L"wb+");
	if (NULL == fp)
		return false;
	fwrite(strGuid.c_str(), sizeof(wchar_t)*strGuid.size(), 1, fp);
	fclose(fp);
	return true;
}

inline DWORD StringToColor(const string& strColor)
{
	DWORD dwColor = 0xffffffff;
	try
	{
		if (strColor.size()>1)
		{
			string str = strColor.substr(1);
			char* pstr = NULL;
			dwColor = 0xff000000 + strtoul(str.c_str(), &pstr, 16);
		}
	}
	catch (...)
	{

	}
	return dwColor;
}

inline wstring SecondToString(int nSeconds)
{
	if (nSeconds < 0)
		return L"00:00";
	int nHour = nSeconds / 3600;
	if (nHour>24)
		nHour = 24;
	nSeconds = nSeconds % 3600;
	wchar_t szText[20];
	if (nHour>0)
		swprintf_s(szText, L"%02d:%02d:%02d", nHour, nSeconds / 60, nSeconds % 60);
	else
		swprintf_s(szText, L"%02d:%02d", nSeconds / 60, nSeconds % 60);
	return wstring(szText);
}

inline bool CheckNeedUpdateVer(wstring strNewVerCode, wstring strOldVerCode) {
    if (strNewVerCode.empty() || _tcscmp(strNewVerCode.c_str(), L"1") == 0) return false;

    if (strOldVerCode.empty()) return true;

    int nNewVer = _ttoi(strNewVerCode.c_str());
    int nOldVer = _ttoi(strOldVerCode.c_str());
    if (nNewVer > nOldVer) return true;

    return false;
}

inline bool IsWow64ProcessEx(HANDLE hProcess) {
    BOOL wow64 = FALSE;
    typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process;

    // IsWow64Process is not available on all supported versions of Windows.
    // Use GetModuleHandle to get a handle to the DLL that contains the function
    // and GetProcAddress to get a pointer to the function if available.
    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(L"kernel32"), "IsWow64Process");
    if (nullptr != fnIsWow64Process) {
        if (!fnIsWow64Process(hProcess, &wow64))
            wow64 = FALSE;
    }

    return (wow64 == TRUE);
}

inline bool IsShowUpdateWnd() {
    wstring strRunPath = GetRunPathW();
    wstring config_path = strRunPath + _T("config.dat");
    return (GetPrivateProfileInt(L"startup", L"showupdatelog", 1, config_path.c_str()) == 1);
}

inline void SetShowUpdateWnd(bool show) {
    wstring strRunPath = GetRunPathW();
    wstring config_path = strRunPath + _T("config.dat");
    wchar_t szValue[128] = { 0 };
    swprintf_s(szValue, L"%d", show ? 1 : 0);
    WritePrivateProfileString(L"startup", L"showupdatelog", szValue, config_path.c_str());
}

inline bool IsShowPerOptimizWnd() {
    wstring strRunPath = GetRunPathW();
    wstring config_path = strRunPath + _T("config.dat");
    return (GetPrivateProfileInt(L"startup", L"showperoptimiz", 1, config_path.c_str()) == 1);
}

inline void SetShowPerOptimizWnd(bool show) {
    wstring strRunPath = GetRunPathW();
    wstring config_path = strRunPath + _T("config.dat");
    wchar_t szValue[128] = { 0 };
    swprintf_s(szValue, L"%d", show ? 1 : 0);
    WritePrivateProfileString(L"startup", L"showperoptimiz", szValue, config_path.c_str());
}