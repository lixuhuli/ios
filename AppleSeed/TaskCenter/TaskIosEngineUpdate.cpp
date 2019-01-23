#include "StdAfx.h"
#include "TaskIosEngineUpdate.h"
#include "ParamDefine.h"
#include "GlobalData.h"
#include "HttpInterface.h"
#include "Ios/IosMgr.h"

namespace TaskCenter{
	CTaskIosEngineUpdate::CTaskIosEngineUpdate(const MSG& msg, UINT uMsg, const wstring& strVersion)
		: ITask(msg)
        , m_uMsg(uMsg)
		, m_strVersion(strVersion) {
		m_type = TaskIosEngineUpdate;
		m_bUsed = TRUE;
	}

	CTaskIosEngineUpdate::~CTaskIosEngineUpdate() {
	}

	void CTaskIosEngineUpdate::Init(const MSG& msg, UINT uMsg, const wstring& strVersion) {
		m_msg = msg;
		m_uMsg = uMsg;
		m_bUsed = TRUE;
		m_strVersion = strVersion;
	}

	void CTaskIosEngineUpdate::Run() {
        PublicLib::CHttpClient http;
        http.AddHttpHeader(L"version", L"1_1.0");
        http.AddHttpHeader(L"Device-Id", L"356261050135555");
        http.AddHttpHeader(L"Unique-Code", L"1AEE004A-73D0-4427-B1C4-B771A5CA1732");
        http.AddHttpHeader(L"Access-Token", L"A2838C35DAAAE9058E521930B8D27F9A0538C7F8D7CD248A263820E6A6B27FB12058141FDA3CEF5D98BDC24C53F07B2B");
        http.AddHttpHeader(L"Guoren-App-Interal-Tags", L"Salem501200");
        
        wstring strPost;
        strPost.append(L"gr_version=");
        strPost += m_strVersion;

        string strData = PublicLib::UToUtf8(strPost);

        string strJson = http.Request(URL_CHECK_IOS_ENGINE_UPDATE, PublicLib::Post, strData.c_str(), strData.size());
        bool success = false;
        BOOL need_reboot = FALSE;
        do {
            if (strJson.empty()){
                OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
                break;
            }

            Json::Reader rd;
            Json::Value vRoot;
            if (!rd.parse(strJson, vRoot)) {
                OUTPUT_XYLOG(LEVEL_ERROR, L"解析返回json失败");
                break;
            }

            string strCode = vRoot["code"].asString();
            if (strCode.compare("600") != 0) {
                OUTPUT_XYLOG(LEVEL_ERROR, L"接口返回失败，code = %s", PublicLib::Utf8ToU(strCode).c_str());
                break;
            }

            Json::Value& vData = vRoot["data_info"];
            size_t down_count = vData.size();
            if (down_count <= 0) {
                ::PostMessage(m_msg.hwnd, m_uMsg, -1, 0);
                m_msg.hwnd = nullptr;
                return;
            }

            ::PostMessage(m_msg.hwnd, m_msg.message, 0, 0);

            if (down_count > 2) {
                do {
                    strJson = http.Request(URL_CHECK_IOS_ENGINE_ALL_UPDATE, PublicLib::Post, strData.c_str(), strData.size());

                    if (strJson.empty()){
                        OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
                        break;
                    }

                    Json::Value vRootFull;
                    if (!rd.parse(strJson, vRootFull)) {
                        OUTPUT_XYLOG(LEVEL_ERROR, L"解析返回json失败");
                        break;
                    }

                    strCode = vRootFull["code"].asString();
                    if (strCode.compare("600") != 0) {
                        OUTPUT_XYLOG(LEVEL_ERROR, L"接口返回失败，code = %s", PublicLib::Utf8ToU(strCode).c_str());
                        break;
                    }

                    Json::Value& vDataFull = vRootFull["data_info"];
                    size_t full_count = vDataFull.size();
                    if (full_count <= 0) {
                        OUTPUT_XYLOG(LEVEL_ERROR, L"没有更新的数据包");
                        break;
                    }

                    success = true;

                    ::PostMessage(m_msg.hwnd, m_msg.message, 0, 30);

                    string down_url = vDataFull[0]["downUrl"].asString();
                    if (down_url.empty()) {
                        success = false;
                        break;
                    }

                    auto pos = down_url.rfind("/");
                    if (pos == string::npos) {
                        success = false;
                        break;
                    }

                    string file_name = down_url.substr(pos + 1, down_url.length() - pos - 1);

                    std::wstring strUpdatePath = GetAppDataPath() + L"UpdatePack\\";
                    if (!PathFileExists(strUpdatePath.c_str())) SHCreateDirectory(NULL, strUpdatePath.c_str());
                    strUpdatePath += PublicLib::Utf8ToU(file_name);

                    if (!http.DownLoadFile(PublicLib::Utf8ToU(down_url).c_str(), strUpdatePath.c_str())) {
                        success = false;
                        break;
                    }

                    ::PostMessage(m_msg.hwnd, m_msg.message, 0, 60);

                    BOOL restart = FALSE;
                    if (0 != CIosMgr::Instance()->UpdatePackage(strUpdatePath, restart)) {
                        success = false;
                        break;
                    }

                    if (restart) need_reboot = TRUE;

                    ::PostMessage(m_msg.hwnd, m_msg.message, 0, 100);
                    DeleteFile(strUpdatePath.c_str());

                } while(false);
            }

            if (success) break;
            
            success = true;

            for (size_t i = 0; i < down_count; ++i) {
                string down_url = vData[i]["downUrl"].asString();
                if (down_url.empty()) {
                    success = false;
                    break;
                }

                auto pos = down_url.rfind("/");
                if (pos == string::npos) {
                    success = false;
                    break;
                }

                string file_name = down_url.substr(pos + 1, down_url.length() - pos - 1);

                std::wstring strUpdatePath = GetAppDataPath() + L"UpdatePack\\";
                if (!PathFileExists(strUpdatePath.c_str())) SHCreateDirectory(NULL, strUpdatePath.c_str());
                strUpdatePath += PublicLib::Utf8ToU(file_name);

                if (!http.DownLoadFile(PublicLib::Utf8ToU(down_url).c_str(), strUpdatePath.c_str())) {
                    success = false;
                    break;
                }

                BOOL restart = FALSE;
                if (0 != CIosMgr::Instance()->UpdatePackage(strUpdatePath, restart)) {
                    success = false;
                    break;
                }

                if (restart) need_reboot = TRUE;

                int nPercent = (int)(((float)(i + 1)) * 100.0 / (float)down_count);
                ::PostMessage(m_msg.hwnd, m_msg.message, 0, nPercent);
                DeleteFile(strUpdatePath.c_str());
            }

        } while(false);

        if (!success){
            ::PostMessage(m_msg.hwnd, m_msg.message, 0, -1);

            Sleep(3500);

            ::PostMessage(m_msg.hwnd, m_uMsg, 0, 0);
            m_msg.hwnd = nullptr;
            return;
        }
        else Sleep(1500);

		::PostMessage(m_msg.hwnd, m_uMsg, 1, (need_reboot == TRUE ? 1 : 0));

		m_msg.hwnd = nullptr;
	}

}