#include "stdafx.h"
#include "TaskDataPost.h"
#include "DataPostDefine.h"
#include "DataCenter.h"
#include <time.h>



namespace DataCenter{

	CTaskDataPost::CTaskDataPost(int nUid, const char* pEventName, const Json::Value& vParams)
		: m_nUid(nUid)
		, m_strEventName(pEventName)
		, m_vParams(vParams)
	{
		m_type = TaskDataPost;
		m_bUsed = TRUE;
	}

	CTaskDataPost::~CTaskDataPost()
	{

	}

	void CTaskDataPost::Init(int nUid, const char* pEventName, const Json::Value& vParams)
	{
		m_nUid = nUid;
		m_strEventName = pEventName;
		m_vParams = vParams;
		m_bUsed = TRUE;
	}

	int CTaskDataPost::Run()
	{
        int nCode = 0;
        PublicLib::CHttpClient http(PublicLib::ContentJson);
        http.AddHttpHeader(L"X-WuFan-App-ID", g_pStrAppID);
        http.AddHttpHeader(L"X-WuFan-Client-ID", CDataCenter::Instance()->GetGuid().c_str());

        Json::Value vData;
        vData[KEY_USER_ID] = m_nUid;
        vData[KEY_DEVICE_ID] = PublicLib::UToA(CDataCenter::Instance()->GetGuid());
        vData[KEY_TIME] = time(NULL);
        vData[KEY_MAC] = CDataCenter::Instance()->GetMacAddress();
        vData[KEY_APP_VER] = CDataCenter::Instance()->GetPCVersion();

        Json::Value::Members vecMems = m_vParams.getMemberNames();
        for (size_t i = 0; i < vecMems.size(); ++i) {
            string& strName = vecMems[i];
            vData[strName] = m_vParams[strName];
        }

        Json::Value vPost;
        vPost["event"] = m_strEventName;
        vPost["data"] = vData;
        Json::Value vArray(Json::arrayValue);
        vArray.append(vPost);
        Json::FastWriter fw;
        string strPost = fw.write(vPost);
        string strRet = http.Request(URL_POST, PublicLib::Post, strPost.c_str());
        try {
            Json::Reader read;
            Json::Value vRoot;
            if (read.parse(strRet, vRoot)) {
                nCode = vRoot["code"].asInt();
            }
        }
        catch (...) {
        }

		return nCode;
	}
}