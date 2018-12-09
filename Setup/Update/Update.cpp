#include "stdafx.h"
#include "Update.h"
#include "resource.h"
#include "GlobalData.h"
#include "Config.h"
#include "Regedit.h"
#include "WndUpdate.h"
#include "UpdateModule.h"
#include <algorithm>
#include "Url.h"



bool RunTempUpdate(const wstring& strTempPath)
{
	OUTPUT_XYLOG(LEVEL_INFO, L"进入自更新流程，下载临时文件并运行****************");
	UPDATE_ITEM& params = CConfig::Instance()->GetUpdateSelf();
	wstring strTemp = strTempPath + params.m_strFileName + L".exe";
	wstring strUrl = CUpdateModule::Instance()->GetRootUrl() + params.m_strDirName + L"/" + params.m_strFileName;
	OUTPUT_XYLOG(LEVEL_INFO, L"下载临时更新文件：%s", strTemp.c_str());
	PublicLib::CHttpClient http;
	if (!http.DownloadFileForRetry(strUrl.c_str(), strTemp.c_str()))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"下载新的更新程序失败，下载地址：%s", strUrl.c_str());
		return false;
	}
	string strMd5 = PublicLib::CalFileMd5(strTemp);
	string& strFileMd5 = params.m_strMd5;
	if (_stricmp(strMd5.c_str(), strFileMd5.c_str()) != 0)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"新的更新程序文件MD5校验失败！");
		return false;
	}
	Json::Value vOther, vParam;
	vParam["quiet"] = "true";
	vParam["selfupdate"] = "true";
	HANDLE hProcess = GetCurrentProcess();
	char szPid[20] = {0};
	sprintf_s(szPid, "%u", GetProcessId(hProcess));
	vOther["pid"] = szPid;
	if (CGlobalData::Instance()->m_strCmdLine.find(L"tempunsetup") != wstring::npos)
		CGlobalData::Instance()->m_strCmdLine = L"/update";//处理下卸载过来的
	vOther["cmd"] = PublicLib::UToA(CGlobalData::Instance()->m_strCmdLine);
	vOther["updateversion"] = PublicLib::UToA(params.m_strVersion);
	vParam["other"] = vOther;
	CloseHandle(hProcess);
	Json::FastWriter wr;
	string strParam = wr.write(vParam);
	wstring strCmd = PublicLib::AToU(strParam);
	PublicLib::ShellExecuteRunas(strTemp.c_str(), strCmd.c_str(), NULL);
	OUTPUT_XYLOG(LEVEL_INFO, L"运行临时更新文件，系统错误码：%u", GetLastError());
	return true;
}

bool RunNewUpdate(const Json::Value& vPid, const Json::Value& vCmd, const Json::Value& vVersion)
{
	OUTPUT_XYLOG(LEVEL_INFO, L"进入更新程序自替换流程");
	string strOldPid = vPid.asString();
	string strCmd = vCmd.asString();
	string strVersion = vVersion.asString();
	DWORD dwPid = atoi(strOldPid.c_str());
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwPid);
	if (hProcess)
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"进入循环，执行终止旧的更新程序！");
		int nCount = 0;
		while(nCount<5)
		{
			TerminateProcess(hProcess, 0);
			if (WAIT_OBJECT_0 == WaitForSingleObject(hProcess, 3000))
			{
				OUTPUT_XYLOG(LEVEL_INFO, L"旧的更新程序成功终止！");
				break;
			}
			nCount++;
			if (nCount == 5)
			{
				OUTPUT_XYLOG(LEVEL_ERROR, L"旧的更新程序终止失败，系统错误码：%u", GetLastError());
			}
			Sleep(10);
		}
		CloseHandle(hProcess);
	}
	wstring strDesExe = CGlobalData::Instance()->GetOldInstallPath() + L"\\" + EXE_UPDATE;
	wchar_t szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	if (!CopyFile(szPath, strDesExe.c_str(), FALSE))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"复制文件从：%s 到：%s 失败，系统错误码：%u", szPath, strDesExe.c_str(), GetLastError());
		return false;
	}
	OUTPUT_XYLOG(LEVEL_INFO, L"替换更新文件完成，开始运行！");
	//修改更新文件的版本号
	wstring strNewVersion = PublicLib::AToU(strVersion);
	RegWriteUpdateexeVersion(REG_ROOT_NAME, strNewVersion);
	//替换完成后，运行
	wstring strShellCmd = PublicLib::AToU(strCmd);
// 	if (strShellCmd.empty())
// 		strShellCmd = L"/update";
	PublicLib::ShellExecuteRunas(strDesExe.c_str(), strShellCmd.c_str(), NULL);
	OUTPUT_XYLOG(LEVEL_INFO, L"运行新的更新文件，系统错误码：%u", GetLastError());
	return true;
}

UpdateStatus CheckUpdate(wstring& strError)
{
	wstring strChannelName = CGlobalData::Instance()->GetChannel();
	if (strChannelName.empty())
	{
		strChannelName = L"5funGameHall";
		//写入默认渠道
		RegWriteDefChannel(REG_ROOT_NAME, strChannelName);
	}
	if (!GetUpdateRootPath(strChannelName))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"调用接口获取更新目录地址失败！");
		return UpdateError;
	}
	int nStatus = CUpdateModule::Instance()->GetUpdateStatus();
	OUTPUT_XYLOG(LEVEL_INFO, L"接口返回升级状态：%d 更新状态（1强制 2可选 0或者3不更新）", nStatus);//更新状态（1强制 2可选 0或者3不更新）
	if (nStatus == 0 || nStatus == 3)
		return UpdateNone;
	wstring strUrl = CUpdateModule::Instance()->GetRootUrl() + SCRIPT_FILE_NAME;
	wstring strTempPath = GetPatchPath() + L"\\";
	PublicLib::CHttpClient http;
	void* lpData = NULL;
	DWORD dwSize = 0;
	if (!http.DownLoadMem(strUrl.c_str(), &lpData, dwSize))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"下载更新配置失败，更新文件地址：%s", strUrl.c_str());
		strError = L"下载更新配置失败";
		return UpdateError;
	}
	if (!CConfig::Instance()->LoadFromMem(lpData, dwSize) || !CConfig::Instance()->ParseUpdateSelf())
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"解析更新配置失败");
		strError = L"解析更新配置失败";
		return UpdateError;
	}

	if (CConfig::Instance()->GetUpdateSelf().m_strVersion != CGlobalData::Instance()->GetOldUpExeVersion())
	{//需要自更新
		RunTempUpdate(strTempPath);
		return UpdateSelf;
	}
	//对比版本号
	PublicLib::CVersionHelper oldVer(CGlobalData::Instance()->GetOldVersion()), newVer(CUpdateModule::Instance()->GetUpdateVersion());
	OUTPUT_XYLOG(LEVEL_INFO, L"当前版本号：%s，升级版本号：%s", CGlobalData::Instance()->GetOldVersion().c_str(), CUpdateModule::Instance()->GetUpdateVersion().c_str());
	if (newVer == oldVer)
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"版本号一致，无需更新");
		return UpdateNone;
	}
	return UpdateNew;
}

bool GetUpdateRootPath(const wstring& strChannelName)
{
	Json::Value vParam, vMsg;
	PublicLib::CVersionHelper ver(CGlobalData::Instance()->GetOldVersion());
	char szVersion[64] = {0};
	sprintf_s(szVersion, "%d_%d.%d.%d", ver.m_nBuildNumber, ver.m_nMajorNumber, ver.m_nMinorNumber, ver.m_nRevisionNumber);
	vParam["version"] = szVersion;
	string strGuid = PublicLib::UToA(CGlobalData::Instance()->GetGuid());
	vParam["deviceid"] = strGuid;
	string strSign = strGuid + "gzRN53VWRF9BYUXomg2014";
	PublicLib::MD5 md5(strSign);
	vParam["sign"] = md5.md5();
	vMsg["args"]["type"] = 2;
	vMsg["args"]["plugin"] = 1;
	vMsg["args"]["tag_id"] = 0;
	vParam["messages"] = vMsg;
	Json::FastWriter fw;
	string strPost = fw.write(vParam);
	PublicLib::CHttpClient http;
	string strJson = http.Request(URL_UPDATE, PublicLib::Post, strPost.c_str());
	wstring str = PublicLib::Utf8ToU(strJson);
#ifdef _DEBUG
	OUTPUT_XYLOG(LEVEL_INFO, L"更新接口返回数据：%s", str.c_str());
#endif
	if (strJson.empty())
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"获取更新根目录失败");
		return false;
	}
	Json::Value vRoot;
	Json::Reader read;
	if (!read.parse(strJson, vRoot))
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"解析返回JSON数据失败");
		return false;
	}
	try
	{
		string strCode = vRoot["code"].asString();
		if (strCode.compare("600") != 0)
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"接口访问失败");
			return false;
		}
		Json::Value& vData = vRoot["messages"]["data"];
		string strUrl = vData["down_url"].asString();
		if (strUrl.empty())
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"返回更新目录为空");
			return false;
		}
		if (strUrl[strUrl.size() - 1] != '/')
		{
			strUrl.append("/");
		}
		strUrl.append("32/");
		//strUrl = "http://192.168.70.59/5FunGameHall/1.0.0.385/32/";
		OUTPUT_XYLOG(LEVEL_INFO, L"获取到根目录路径：%s", PublicLib::Utf8ToU(strUrl).c_str());
		string strDesc = vData["info"].asString();//升级说明
		string strStatus = vData["upgrade_status"].asString();//更新状态（1强制 2可选 0或者3不更新）
		string strDate = vData["upgrade_time"].asString();
		string strVersion = vData["ver"].asString();
		int nStatus = atoi(strStatus.c_str());
		//强升
		//nStatus = 1;
		CUpdateModule::Instance()->SetUpdateStatus(nStatus);
		CUpdateModule::Instance()->SetRootUrl(PublicLib::Utf8ToU(strUrl));
		CUpdateModule::Instance()->SetUpdateDesc(PublicLib::Utf8ToU(strDesc));
		CUpdateModule::Instance()->SetUpdateDate(PublicLib::Utf8ToU(strDate));
		CUpdateModule::Instance()->SetUpdateVersion(PublicLib::Utf8ToU(strVersion));
	}
	catch (...)
	{

	}
	return true;
}