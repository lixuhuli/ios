#include "StdAfx.h"
#include "TaskGetUpdateLog.h"
#include "PublicLib/VersionHelper.h"
#include "GlobalData.h"
#include "HttpInterface.h"

namespace TaskCenter{
    CTaskGetUpdateLog::CTaskGetUpdateLog(const MSG& msg)
		: ITask(msg) {
		m_type = TaskGetUpdateLog;
		m_bUsed = TRUE;
	}

    CTaskGetUpdateLog::~CTaskGetUpdateLog() {
	}

	void CTaskGetUpdateLog::Init(const MSG& msg) {
		m_msg = msg;
		m_bUsed = TRUE;
	}

	void CTaskGetUpdateLog::Run() {
        Json::Value vParam, vMsg;
        PublicLib::CVersionHelper ver(GetApplicationVersion());
        char szVersion[64] = { 0 };
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
        m_strResult = http.Request(URL_GET_UPDATE_LOG, PublicLib::Post, strPost.c_str());
        if (m_strResult.empty()) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
        }
	}

}