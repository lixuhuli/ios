#include "StdAfx.h"
#include "SetupModule.h"
#include "resource.h"
#include "Regedit.h"
#include "DataCenter\DataCenterApi.h"
#include <PublicLib\StrHelper.h>
#include <PublicLib\FileHelper.h>
#include <Msi.h>

#define UPGRADE_CODE     L"{968426B1-BAB9-4A07-8EC6-8ECAC6B7A98E}"
#define COMPONENT_PLAYER L"{FEB8943E-5D60-4E2D-846F-458207019D40}"

static int WINAPI MyInstallUIHandler(LPVOID pvContext, UINT iMessageType, LPCWSTR szMessage) {
    return CSetupModule::Instance()->InstallUICallBack(pvContext, iMessageType, szMessage);
}

BOOL CSetupModule::s_bExit = FALSE;
CSetupModule::CSetupModule(void)
 : m_nInsState(StepInit)
 , m_hInstThread(NULL)
 , m_hMsgWnd(NULL)
 , m_uMsg(0)
 , m_bQuiet(false)
 , first_time_(true)
 , total_progress_(0)
 , current_pos_(0)
 , forward_progress_(true)
 , install_in_progress_(false)
 , script_in_progress_(false) {
}

CSetupModule::~CSetupModule(void)
{
	PublicLib::SafeKillThread(&m_hInstThread, 0);
}

bool CSetupModule::Init(HINSTANCE hInstance)
{
	wchar_t szVersion[20] = {0};
	if (LoadString(hInstance, IDS_VERSION, szVersion, 20) > 0)
		m_strNewVersion = szVersion;
	if (LoadString(hInstance, IDS_VERSION_UPDATE, szVersion, 20) > 0)
		m_strUpVersion = szVersion;
	InitAdId(hInstance);
	SetDataCenterVersion(PublicLib::UToA(m_strNewVersion).c_str());
	wstring strInstPath, strVersion, strUpVersion, strChannel;
	if (RegGetInstallInfo(REG_ROOT_NAME, strInstPath, strVersion, strUpVersion, strChannel))
	{
		if (PathFileExists(strInstPath.c_str()) && !strVersion.empty())
		{
			//已经安装了
			m_strOldInstPath = strInstPath;
		}
	}
	int nCount = _countof(g_pConflict);
	m_vecConflict.reserve(nCount);
	for (int i = 0; i < nCount; ++i)
	{
		m_vecConflict.push_back(g_pConflict[i]);
	}
	return true;
}

void CSetupModule::InitAdId(HINSTANCE hInstance)
{
	bool bFind = false;
	while(true)
	{
		wchar_t szPath[MAX_PATH];
		GetModuleFileName(NULL, szPath, MAX_PATH);
		wstring strExePath(szPath);
		int nBegin = strExePath.rfind('\\');
		if (nBegin == wstring::npos)
			break;
		wstring strName = strExePath.substr(nBegin+1);
		nBegin = strName.find('_');
		if (nBegin == wstring::npos)
			break;
		int nEnd = strName.find('_', nBegin+1);
		if (nEnd == wstring::npos || nEnd-nBegin<2)
			break;
		m_strChannelName = strName.substr(nBegin+1, nEnd-nBegin-1);
		bFind = true;
		for(size_t i=0;i<m_strChannelName.size();++i)
		{
			if (!iswdigit(m_strChannelName[i]))
			{
				bFind = false;
				break;
			}
		}
		break;
	}
	if (!bFind)
	{
		wchar_t szChannel[100] = {0};
		if (LoadString(hInstance, IDS_CHANNEL, szChannel, 100) > 0)
		{
			m_strChannelName = szChannel;
		}
	}
	int nAdID = _wtoi(m_strChannelName.c_str());
	SetDataCenterID(nAdID);
}

bool CSetupModule::Install(wstring& strError)
{
	bool bResult = false;
	::PostMessage(m_hMsgWnd, m_uMsg, Mw_Init, 0);
	while (true)
	{
		PublicLib::KillProcessFromList(m_vecConflict);
		Sleep(200);
		PublicLib::KillProcessFromList(m_vecConflict);

        PublicLib::KillProcessByName(L"YBoxHeadless.exe");
        PublicLib::KillProcessByName(L"YBoxSVC.exe");

		SHCreateDirectory(NULL, m_strInstPath.c_str());

        wstring file_path = GetAppDataPath() + L"temp";
        SHCreateDirectory(nullptr, file_path.c_str());
        HANDLE hprocess = ::GetCurrentProcess();
        bool is_64bit_system = IsWow64ProcessEx(hprocess);
        file_path.append(is_64bit_system ? L"\\amd64.msi" : L"\\x86.msi");
        if (!ExtractRawFile(is_64bit_system ? IDR_MSI_X64 : IDR_MSI_X86, file_path)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"解压安装资源失败，提取msi文件失败");
            break;
        }

        if (!InstallMsi(file_path)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"安装msi文件失败, 文件路径：%s", file_path);
            break;
        }

		//从资源安装
        //if (!UnCompressResource(strError))
        //{
        //    OUTPUT_XYLOG(LEVEL_ERROR, L"解压安装资源失败，Error=%s", strError.c_str());
        //    break;
        //}

		//if (!WriteRegedit(strError))
		//{
		//	OUTPUT_XYLOG(LEVEL_ERROR, L"写注册表失败，Error=%s", strError.c_str());
		//	break;
		//}
		::PostMessage(m_hMsgWnd, m_uMsg, Mw_Regedit, 0);
		CreateShellLink(m_strInstPath);
		PublicLib::RefreshIcons();
		bResult = true;
		break;
	}
	::PostMessage(m_hMsgWnd, m_uMsg, bResult ? Mw_Finish : Mw_Fail, 0);
	m_nInsState = bResult ? StepFinish : StepError;
	return bResult;
}

int CSetupModule::InstallUICallBack(LPVOID pvContext, UINT iMessageType, LPCWSTR szMessage) {
    // File costing is skipped when applying Patch(es) and INSTALLUILEVEL is NONE.
    // Workaround: Set INSTALLUILEVEL to anything but NONE only once.
    if (!first_time_) {
        UINT r1 = MsiSetInternalUI(INSTALLUILEVEL_BASIC, NULL);
        first_time_ = false;
    }

    if (!szMessage) return 0;

    INSTALLMESSAGE msg = (INSTALLMESSAGE)(0xFF000000 & (UINT)iMessageType);
    UINT flags = (0x00FFFFFF & iMessageType);

    switch (msg) {
    case INSTALLMESSAGE_FATALEXIT: 
        OUTPUT_XYLOG(LEVEL_ERROR, L"安装资源包失败，错误信息：%s", szMessage);
        return 0;
    case INSTALLMESSAGE_ERROR: 
        MessageBeep(flags & MB_ICONMASK);
        OUTPUT_XYLOG(LEVEL_ERROR, L"安装资源包失败，信息错误，错误信息：%s", szMessage);
        return 0;
    case INSTALLMESSAGE_WARNING:
        OUTPUT_XYLOG(LEVEL_WARN, L"警告，警告信息：%s", szMessage);
        return 0;
    case INSTALLMESSAGE_USER:
        OUTPUT_XYLOG(LEVEL_ERROR, L"安装资源包失败，用户信息错误，错误信息：%s", szMessage);
        return MessageBox(NULL, szMessage, TEXT("Message"), flags);
    case INSTALLMESSAGE_INFO:
        OUTPUT_XYLOG(LEVEL_INFO, L"%s", szMessage);
        return IDOK;
    case INSTALLMESSAGE_FILESINUSE:
    case INSTALLMESSAGE_RMFILESINUSE:
        OUTPUT_XYLOG(LEVEL_ERROR, L"安装资源包失败，文件正在使用 ：%s", szMessage);
        return 0;
    case INSTALLMESSAGE_RESOLVESOURCE:
        return 0;
    case INSTALLMESSAGE_OUTOFDISKSPACE:
        OUTPUT_XYLOG(LEVEL_ERROR, L"%s", szMessage);
        return IDOK;
    case INSTALLMESSAGE_ACTIONSTART:
        return IDOK;
    case INSTALLMESSAGE_ACTIONDATA:
        return IDOK;
    case INSTALLMESSAGE_PROGRESS: {
        std::wstring progress_msg = szMessage;
        int fields[4] = { 0, 0, 0, 0 };
        if (ParseProgressString(progress_msg, fields)) {
            switch(fields[0]) {
            case 0: {
                // field 1 = 0, field 2 = total number of ticks, field 3 = direction, field 4 = in progress
                total_progress_ = fields[1];

                forward_progress_ = (fields[2] == 0 ? true : false);

                if (script_in_progress_) install_in_progress_ = true;

                current_pos_ = 0;

                script_in_progress_ = (fields[3] == 1) ? true : false;

                break;
            }
            case 1:
                break;
            case 2: {
                if (0 == total_progress_) break;

                static int s_lastProgress = 0;
                int progress = 0;


                current_pos_ += fields[1];

                // field 1 = 2,field 2 will contain the number of ticks the bar has moved
                // movement direction determined by g_bForwardProgress set by reset progress msg
                progress = int((double)current_pos_ * 100.0 / (double)total_progress_);
                if (install_in_progress_) {
                    if (m_hMsgWnd) ::PostMessage(m_hMsgWnd, m_uMsg, Mw_Unzip, (LPARAM)progress);
                }   

                break;
            }
            case 3:
                break;
            default:
                break;
            }

        }
        return IDOK;
    }
    case INSTALLMESSAGE_COMMONDATA: 
        return IDOK;
    case INSTALLMESSAGE_INITIALIZE:
        return IDOK;
    case INSTALLMESSAGE_TERMINATE:
        return IDOK;
    case INSTALLMESSAGE_SHOWDIALOG:
        return IDOK;
    default:
        break;
    }

    return 0;
}

bool CSetupModule::ParseProgressString(const std::wstring& message, int* fields) {
    if (message.empty()) return false;

    auto cut_msg = message;
    auto pos = cut_msg.find(L":");
    while (pos != wstring::npos) {
        int index = _wtoi(cut_msg.substr(0, pos).c_str());

        auto space_pos = cut_msg.find(L" ");
        if (space_pos == wstring::npos) return false;
        cut_msg = cut_msg.substr(space_pos + 1, cut_msg.length() - space_pos - 1);

        space_pos = cut_msg.find(L" ");
        if (space_pos == wstring::npos) return false;

        if (index == 1) {
            const wchar_t* ch_field = cut_msg.c_str();
            if (!ch_field || 0 == isdigit(*ch_field)) return false;
        }

        int field = _wtoi(cut_msg.substr(0, space_pos).c_str());

        if (space_pos + 1 >= cut_msg.length()) cut_msg = L"";
        else cut_msg = cut_msg.substr(space_pos + 1, cut_msg.length() - space_pos - 1);

        switch (index) {
        case 1: 
            fields[0] = field;
            break;
        case 2:
            fields[1] = field;
            if (fields[0] == 2 || fields[0] == 3) return true;
            break;
        case 3:
            fields[2] = field;
            if (fields[0] == 1) return true;
            break;
        case 4:
            fields[3] = field;
            return true;
        default:
            return false;
        }

        if (cut_msg.empty()) break;
        pos = cut_msg.find(L":");
    }

    return true;
}

bool CSetupModule::InstallMsi(const std::wstring& file_path) {
    MsiSetInternalUI(INSTALLUILEVEL(INSTALLUILEVEL_NONE|INSTALLUILEVEL_SOURCERESONLY), NULL);

    MsiSetExternalUIW(MyInstallUIHandler,
        INSTALLLOGMODE_PROGRESS|INSTALLLOGMODE_FATALEXIT|INSTALLLOGMODE_ERROR
        |INSTALLLOGMODE_WARNING|INSTALLLOGMODE_USER|INSTALLLOGMODE_INFO
        |INSTALLLOGMODE_RESOLVESOURCE|INSTALLLOGMODE_OUTOFDISKSPACE
        |INSTALLLOGMODE_ACTIONSTART|INSTALLLOGMODE_ACTIONDATA
        |INSTALLLOGMODE_COMMONDATA|INSTALLLOGMODE_PROGRESS|INSTALLLOGMODE_INITIALIZE
        |INSTALLLOGMODE_TERMINATE|INSTALLLOGMODE_SHOWDIALOG, 
        TEXT("MsiInstaller"));

    int state = 0;
    wchar_t productCode[256] = { 0 };
    if (MsiEnumRelatedProducts(UPGRADE_CODE, 0, 0, productCode) == ERROR_SUCCESS) {
        wstring strPath;
        wchar_t install_path[MAX_PATH + 1] = { 0 };
        DWORD valueBufSize = sizeof(install_path);
        if (MsiGetComponentPathW(productCode, COMPONENT_PLAYER, install_path, &valueBufSize) == INSTALLSTATE_LOCAL) {
            strPath = install_path;
        }

        wchar_t username[1024] = { 0 };
        DWORD usernameSize = sizeof(username);
        GetUserName(username, &usernameSize);

        MsiSourceListClearAll(productCode, username, 0);

        UINT addSource = MsiSourceListAddSource(productCode, username, 0, file_path.c_str());

        //state = MsiReinstallProduct(productCode,
        //    REINSTALLMODE_FILEREPLACE | REINSTALLMODE_USERDATA | REINSTALLMODE_MACHINEDATA |
        //    REINSTALLMODE_SHORTCUT | REINSTALLMODE_PACKAGE);

        state = MsiConfigureProductW(productCode, INSTALLLEVEL_DEFAULT, INSTALLSTATE_ABSENT);

        if (state != ERROR_SUCCESS) return false;

        if (!strPath.empty()) PublicLib::RemoveDir(strPath.c_str());
    }

    std::wstring install_path_prop = L"INSTALLDIR=\"";
    install_path_prop += m_strInstPath;
    install_path_prop += L"\"";

    PublicLib::RemoveDir(m_strInstPath.c_str());
    state = MsiInstallProductW(file_path.c_str(), install_path_prop.c_str());

    return (state == ERROR_SUCCESS || state == ERROR_SUCCESS_REBOOT_REQUIRED);
}

bool CSetupModule::ExtractRawFile(uint16_t res_id, std::wstring file_path) {
    try {
        HRSRC res = FindResource(nullptr, MAKEINTRESOURCE(res_id), L"MSI");
        if (res == nullptr) return false;

        uint32_t dwSize = SizeofResource(nullptr, res);
        if (dwSize == 0) return false;

        HGLOBAL hRes = LoadResource(nullptr, res);
        if (hRes == nullptr) return false;

        void *lpFile = LockResource(hRes);
        if (lpFile == nullptr) return false;

        HANDLE hFile = CreateFile(file_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        HANDLE hFilemap = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, 0, dwSize, nullptr);
        if (hFilemap == nullptr) return false;

        void *lpBaseAddress = MapViewOfFile(hFilemap, FILE_MAP_WRITE, 0, 0, 0);
        CopyMemory(lpBaseAddress, lpFile, dwSize);
        UnmapViewOfFile(lpBaseAddress);
        CloseHandle(hFilemap);
        CloseHandle(hFile);

        return true;    
    } catch(...) {
        return false;
    }
}

void CSetupModule::UncompressCallback(int nCurPercent, void* lpParam)
{
    CSetupModule* pModule = (CSetupModule*)lpParam;
    if (pModule->m_hMsgWnd) 
        ::PostMessage(pModule->m_hMsgWnd, pModule->m_uMsg, Mw_Unzip, (LPARAM)nCurPercent);
}

bool CSetupModule::UnCompressResource(wstring& strError)
{
	//HRSRC hSrc = ::FindResource(NULL, MAKEINTRESOURCE(IDR_DAT1), L"DAT");
	//if (hSrc == NULL)
	//{
	//	OUTPUT_XYLOG(LEVEL_ERROR, L"获取安装包资源失败，错误码：%u", GetLastError());
	//	return false;
	//}

	//DWORD dwSize = ::SizeofResource(NULL, hSrc);
	//HGLOBAL hglobal = ::LoadResource(NULL, hSrc);
	//BYTE* pData = (BYTE*)::LockResource(hglobal);
	//if (NULL == pData)
	//{
	//	OUTPUT_XYLOG(LEVEL_ERROR, L"提取安装包资源失败，错误码：%u", GetLastError());
	//	return false;
	//}
	//DeCompressParams params;
	//params.lpProgressCallback	= UncompressCallback;
	//params.lpParam				= this;
	//params.lpNeedExit			= &s_bExit;
	//if (!PublicLib::DeCompressMem(pData, dwSize, m_strInstPath.c_str(), params))
	//{
	//	OUTPUT_XYLOG(LEVEL_ERROR, L"解压失败，错误信息：%s", params.strError.c_str());
	//	return false;
	//}
	return true;
}

bool CSetupModule::WriteRegedit(wstring& strError)
{
	//写卸载注册表
	//INSTALL_INFO info;
	//info.strKeyName		= REG_ROOT_NAME;
	//info.strExePath		= m_strInstPath + EXE_MAIN;
	//info.strDisplayName = SOFT_NAME;
	//info.strVersion		= m_strNewVersion;
	//info.strPublisher	= REG_COMPANY;
	//info.strUninstPath	= m_strInstPath + EXE_UNINSTALL;
	//info.strUrl			= REG_URL;
	//info.strUpExeVersion= m_strUpVersion;
	//info.strChannel		= m_strChannelName;
	//if (!RegWriteUninstInfo(info))
	//{
	//	strError = L"注册表写入安装信息失败";
	//	OUTPUT_XYLOG(LEVEL_ERROR, L"注册表写入安装信息失败，错误码：%u", GetLastError());
	//	return false;
	//}
	//if (!RegWriteList(REG_ROOT_NAME, m_vecConflict, RegConflictList))
	//{
	//	strError = L"注册表写入冲突列表失败";
	//	OUTPUT_XYLOG(LEVEL_ERROR, L"注册表写入冲突列表失败，错误码：%u", GetLastError());
	//	return false;
	//}
	//wstring strKeyPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	//HKEY hKey = NULL;
	//if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, strKeyPath.c_str(), 0, KEY_ALL_ACCESS, &hKey))
	//{
	//	RegDeleteValue(hKey, REG_ROOT_NAME);
	//	RegCloseKey(hKey);
	//}
	return true;
}

void CSetupModule::CreateShellLink(const wstring& strInstallPath)
{
	wstring strMainExePath = strInstallPath + EXE_MAIN;
	PublicLib::CShortcutHelper sh;
	sh.SetResPath(strMainExePath.c_str());
// 	if (_tcslen(ShortcuteParam)>0)
// 		sh.SetArguments(ShortcuteParam);
	sh.CreateFileLinkAtSpecialPath(SOFT_NAME);
	WCHAR szPath[MAX_PATH+1] = {0};
	if ( SHGetSpecialFolderPath(NULL, szPath, CSIDL_PROGRAMS, FALSE) )
	{
		wcscat_s(szPath, L"\\");
		wcscat_s(szPath, SOFT_NAME);
		CreateDirectory(szPath, NULL);
		wcscat_s(szPath, L"\\");
		wstring strProPath(szPath);
		wcscat_s(szPath, SOFT_NAME);
		wcscat_s(szPath, L".lnk");
		sh.CreateFileLink(szPath);
		//卸载
		PublicLib::CShortcutHelper shUninst;
		strMainExePath = strInstallPath + EXE_UNINSTALL;
		shUninst.SetResPath(strMainExePath.c_str());
		wstring strIcoPath = strInstallPath + L"Unsetup.ico";
		shUninst.SetIconPath(strIcoPath.c_str());
		//shUninst.SetArguments(L"/unsetup");
		strProPath.append(L"卸载");
		strProPath.append(SOFT_NAME);
		strProPath.append(L".lnk");
		shUninst.CreateFileLink(strProPath.c_str());
		if (PublicLib::IsOsOverVista())
			PublicLib::PinToWin7TaskBar(strInstallPath.c_str(), EXE_MAIN);
	}
}

bool CSetupModule::StartSetup()
{
	m_nInsState = StepInstall;
	m_hInstThread = (HANDLE)_beginthreadex(NULL, 0, InstallThread, this, 0, NULL);
	return m_hInstThread != NULL;
}

void CSetupModule::StopSetup()
{
	s_bExit = TRUE;
	PublicLib::SafeKillThread(&m_hInstThread, 1000);
}

UINT WINAPI CSetupModule::InstallThread(void* lpParam)
{
	CoInitialize(NULL);
	CSetupModule* pModule = (CSetupModule*)lpParam;
	wstring strError;
	bool bResult = pModule->Install(strError);
	CoUninitialize();
	return 0;
}

bool CSetupModule::IsShowUpdateWnd() {
    wstring strRunPath = GetInstallPath();
    wstring config_path = strRunPath + _T("config.dat");
    return (GetPrivateProfileInt(L"startup", L"showupdatelog", 1, config_path.c_str()) == 1);
}

void CSetupModule::SetShowUpdateWnd(bool show) {
    wstring strRunPath = GetInstallPath();
    wstring config_path = strRunPath + _T("config.dat");
    wchar_t szValue[128] = { 0 };
    swprintf_s(szValue, L"%d", show ? 1 : 0);
    WritePrivateProfileString(L"startup", L"showupdatelog", szValue, config_path.c_str());
}