#include "StdAfx.h"
#include "TaskGetUpdateLog.h"
#include "PublicLib/VersionHelper.h"
#include "GlobalData.h"
#include "HttpInterface.h"

namespace TaskCenter{
    CTaskGetUpdateLog::CTaskGetUpdateLog(const MSG& msg, const wstring& ver)
		: ITask(msg)
        , version_(ver){
		m_type = TaskGetUpdateLog;
		m_bUsed = TRUE;
	}

    CTaskGetUpdateLog::~CTaskGetUpdateLog() {
	}

	void CTaskGetUpdateLog::Init(const MSG& msg, const wstring& ver) {
		m_msg = msg;
        version_ = ver;
		m_bUsed = TRUE;
	}

	void CTaskGetUpdateLog::Run() {
        PublicLib::CHttpClient http;
        http.AddHttpHeader(L"version", L"1_1.0");
        http.AddHttpHeader(L"Device-Id", L"356261050135555");
        http.AddHttpHeader(L"Unique-Code", L"1AEE004A-73D0-4427-B1C4-B771A5CA1732");
        http.AddHttpHeader(L"Access-Token", L"A2838C35DAAAE9058E521930B8D27F9A0538C7F8D7CD248A263820E6A6B27FB12058141FDA3CEF5D98BDC24C53F07B2B");
        http.AddHttpHeader(L"Guoren-App-Interal-Tags", L"Salem501200");

        wstring strPost;
        strPost.append(L"ver=");
        strPost += version_;

        string strData = PublicLib::UToUtf8(strPost);

        m_strResult = http.Request(URL_GET_UPDATE_LOG, PublicLib::Post, strData.c_str(), strData.size());
        if (m_strResult.empty()) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
        }
	}

}