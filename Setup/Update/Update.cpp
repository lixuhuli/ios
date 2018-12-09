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
	OUTPUT_XYLOG(LEVEL_INFO, L"�����Ը������̣�������ʱ�ļ�������****************");
	UPDATE_ITEM& params = CConfig::Instance()->GetUpdateSelf();
	wstring strTemp = strTempPath + params.m_strFileName + L".exe";
	wstring strUrl = CUpdateModule::Instance()->GetRootUrl() + params.m_strDirName + L"/" + params.m_strFileName;
	OUTPUT_XYLOG(LEVEL_INFO, L"������ʱ�����ļ���%s", strTemp.c_str());
	PublicLib::CHttpClient http;
	if (!http.DownloadFileForRetry(strUrl.c_str(), strTemp.c_str()))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"�����µĸ��³���ʧ�ܣ����ص�ַ��%s", strUrl.c_str());
		return false;
	}
	string strMd5 = PublicLib::CalFileMd5(strTemp);
	string& strFileMd5 = params.m_strMd5;
	if (_stricmp(strMd5.c_str(), strFileMd5.c_str()) != 0)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"�µĸ��³����ļ�MD5У��ʧ�ܣ�");
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
		CGlobalData::Instance()->m_strCmdLine = L"/update";//������ж�ع�����
	vOther["cmd"] = PublicLib::UToA(CGlobalData::Instance()->m_strCmdLine);
	vOther["updateversion"] = PublicLib::UToA(params.m_strVersion);
	vParam["other"] = vOther;
	CloseHandle(hProcess);
	Json::FastWriter wr;
	string strParam = wr.write(vParam);
	wstring strCmd = PublicLib::AToU(strParam);
	PublicLib::ShellExecuteRunas(strTemp.c_str(), strCmd.c_str(), NULL);
	OUTPUT_XYLOG(LEVEL_INFO, L"������ʱ�����ļ���ϵͳ�����룺%u", GetLastError());
	return true;
}

bool RunNewUpdate(const Json::Value& vPid, const Json::Value& vCmd, const Json::Value& vVersion)
{
	OUTPUT_XYLOG(LEVEL_INFO, L"������³������滻����");
	string strOldPid = vPid.asString();
	string strCmd = vCmd.asString();
	string strVersion = vVersion.asString();
	DWORD dwPid = atoi(strOldPid.c_str());
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwPid);
	if (hProcess)
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"����ѭ����ִ����ֹ�ɵĸ��³���");
		int nCount = 0;
		while(nCount<5)
		{
			TerminateProcess(hProcess, 0);
			if (WAIT_OBJECT_0 == WaitForSingleObject(hProcess, 3000))
			{
				OUTPUT_XYLOG(LEVEL_INFO, L"�ɵĸ��³���ɹ���ֹ��");
				break;
			}
			nCount++;
			if (nCount == 5)
			{
				OUTPUT_XYLOG(LEVEL_ERROR, L"�ɵĸ��³�����ֹʧ�ܣ�ϵͳ�����룺%u", GetLastError());
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
		OUTPUT_XYLOG(LEVEL_ERROR, L"�����ļ��ӣ�%s ����%s ʧ�ܣ�ϵͳ�����룺%u", szPath, strDesExe.c_str(), GetLastError());
		return false;
	}
	OUTPUT_XYLOG(LEVEL_INFO, L"�滻�����ļ���ɣ���ʼ���У�");
	//�޸ĸ����ļ��İ汾��
	wstring strNewVersion = PublicLib::AToU(strVersion);
	RegWriteUpdateexeVersion(REG_ROOT_NAME, strNewVersion);
	//�滻��ɺ�����
	wstring strShellCmd = PublicLib::AToU(strCmd);
// 	if (strShellCmd.empty())
// 		strShellCmd = L"/update";
	PublicLib::ShellExecuteRunas(strDesExe.c_str(), strShellCmd.c_str(), NULL);
	OUTPUT_XYLOG(LEVEL_INFO, L"�����µĸ����ļ���ϵͳ�����룺%u", GetLastError());
	return true;
}

UpdateStatus CheckUpdate(wstring& strError)
{
	wstring strChannelName = CGlobalData::Instance()->GetChannel();
	if (strChannelName.empty())
	{
		strChannelName = L"5funGameHall";
		//д��Ĭ������
		RegWriteDefChannel(REG_ROOT_NAME, strChannelName);
	}
	if (!GetUpdateRootPath(strChannelName))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"���ýӿڻ�ȡ����Ŀ¼��ַʧ�ܣ�");
		return UpdateError;
	}
	int nStatus = CUpdateModule::Instance()->GetUpdateStatus();
	OUTPUT_XYLOG(LEVEL_INFO, L"�ӿڷ�������״̬��%d ����״̬��1ǿ�� 2��ѡ 0����3�����£�", nStatus);//����״̬��1ǿ�� 2��ѡ 0����3�����£�
	if (nStatus == 0 || nStatus == 3)
		return UpdateNone;
	wstring strUrl = CUpdateModule::Instance()->GetRootUrl() + SCRIPT_FILE_NAME;
	wstring strTempPath = GetPatchPath() + L"\\";
	PublicLib::CHttpClient http;
	void* lpData = NULL;
	DWORD dwSize = 0;
	if (!http.DownLoadMem(strUrl.c_str(), &lpData, dwSize))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"���ظ�������ʧ�ܣ������ļ���ַ��%s", strUrl.c_str());
		strError = L"���ظ�������ʧ��";
		return UpdateError;
	}
	if (!CConfig::Instance()->LoadFromMem(lpData, dwSize) || !CConfig::Instance()->ParseUpdateSelf())
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"������������ʧ��");
		strError = L"������������ʧ��";
		return UpdateError;
	}

	if (CConfig::Instance()->GetUpdateSelf().m_strVersion != CGlobalData::Instance()->GetOldUpExeVersion())
	{//��Ҫ�Ը���
		RunTempUpdate(strTempPath);
		return UpdateSelf;
	}
	//�ԱȰ汾��
	PublicLib::CVersionHelper oldVer(CGlobalData::Instance()->GetOldVersion()), newVer(CUpdateModule::Instance()->GetUpdateVersion());
	OUTPUT_XYLOG(LEVEL_INFO, L"��ǰ�汾�ţ�%s�������汾�ţ�%s", CGlobalData::Instance()->GetOldVersion().c_str(), CUpdateModule::Instance()->GetUpdateVersion().c_str());
	if (newVer == oldVer)
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"�汾��һ�£��������");
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
	OUTPUT_XYLOG(LEVEL_INFO, L"���½ӿڷ������ݣ�%s", str.c_str());
#endif
	if (strJson.empty())
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"��ȡ���¸�Ŀ¼ʧ��");
		return false;
	}
	Json::Value vRoot;
	Json::Reader read;
	if (!read.parse(strJson, vRoot))
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"��������JSON����ʧ��");
		return false;
	}
	try
	{
		string strCode = vRoot["code"].asString();
		if (strCode.compare("600") != 0)
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"�ӿڷ���ʧ��");
			return false;
		}
		Json::Value& vData = vRoot["messages"]["data"];
		string strUrl = vData["down_url"].asString();
		if (strUrl.empty())
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"���ظ���Ŀ¼Ϊ��");
			return false;
		}
		if (strUrl[strUrl.size() - 1] != '/')
		{
			strUrl.append("/");
		}
		strUrl.append("32/");
		//strUrl = "http://192.168.70.59/5FunGameHall/1.0.0.385/32/";
		OUTPUT_XYLOG(LEVEL_INFO, L"��ȡ����Ŀ¼·����%s", PublicLib::Utf8ToU(strUrl).c_str());
		string strDesc = vData["info"].asString();//����˵��
		string strStatus = vData["upgrade_status"].asString();//����״̬��1ǿ�� 2��ѡ 0����3�����£�
		string strDate = vData["upgrade_time"].asString();
		string strVersion = vData["ver"].asString();
		int nStatus = atoi(strStatus.c_str());
		//ǿ��
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