#include "stdafx.h"
#include "WebHelper.h"
#include "GlobalData.h"
#include "HttpInterface.h"
#include <PublicLib\HttpClient.h>
#include "GameIconHelper.h"
#include "Database/DatabaseMgr.h"
#include "CxImg/xImageApi.h"
#include "CxImg/ximage.h"
#include <gdiplus.h>


CWebHelperThread::CWebHelperThread()
	: PublicLib::CThread()
	, m_pTask(NULL)
	, m_hWnd(NULL)
	, m_uMsg(NULL)
	, m_tag(Tht_Unkonw)
{

}

CWebHelperThread::~CWebHelperThread()
{

}


ITask* CWebHelperThread::GetTask()
{
	return m_pTask;
}

Tag_HelperThread CWebHelperThread::GetTag()
{
	return m_tag;
}

BOOL CWebHelperThread::Init(HWND hWnd, UINT uMsg, ITask* pTask, Tag_HelperThread tag)
{
	m_hWnd = hWnd;
	m_uMsg = uMsg;
	m_pTask = pTask;
	m_tag = tag;
	m_bHasExited = FALSE;
    return CreateThread("网页安装查询下载地址线程", nullptr);
}

int CWebHelperThread::Run(void * argument) {
	switch(m_tag) {
	case Tht_Unkonw:
		break;
	case Tht_Url:
		RequestUrl();
		break;
    case Tht_Icon:
        RequestGameIconUrl();
        break;
    }
	return 0;
}

void CWebHelperThread::RequestGameIconUrl() {
    char szData[256] = { 0 };
    sprintf_s(szData, "game_id=%I64d", m_pTask->nGameID);
    string strParam = PublicLib::AToUtf(szData);
    LPARAM lParam = FALSE;
    try {
        PublicLib::CHttpClient http;
        while (true) {
            Json::Value vRoot;

            auto RequestUrlInfo = [&](LPCTSTR lpUrl) -> bool {
                http.AddHttpHeader(L"version", L"1_1.0");
                http.AddHttpHeader(L"Device-Id", L"356261050135555");
                http.AddHttpHeader(L"Unique-Code", L"1AEE004A-73D0-4427-B1C4-B771A5CA1732");
                http.AddHttpHeader(L"Access-Token", L"A2838C35DAAAE9058E521930B8D27F9A0538C7F8D7CD248A263820E6A6B27FB12058141FDA3CEF5D98BDC24C53F07B2B");
                http.AddHttpHeader(L"Guoren-App-Interal-Tags", L"Salem501200");

                string strJson = http.Request(lpUrl, PublicLib::Post, strParam.c_str(), strParam.size());
                if (strJson.empty()) {
                    m_pTask->state = Ts_Error;
                    OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
                    return false;
                }

                Json::Reader rd;
                if (!rd.parse(strJson, vRoot)) {
                    m_pTask->state = Ts_Error;
                    OUTPUT_XYLOG(LEVEL_ERROR, L"解析返回json失败");
                    return false;
                }

                string strCode = vRoot["code"].asString();
                if (strCode.compare("600") != 0) {
                    m_pTask->state = Ts_Error;
                    OUTPUT_XYLOG(LEVEL_ERROR, L"接口返回失败，code = %s", PublicLib::Utf8ToU(strCode).c_str());
                    return false;
                }

                return true;
            };

            if (!RequestUrlInfo(URL_REQUEST_GAME)) break;

            Json::Value& vData = vRoot["data_info"];

            string strVerName = vData["game_name"].asString();
            string strVersion = vData["game_ver"].asString();

            m_pTask->strVersion = PublicLib::Utf8ToU(strVersion);
            m_pTask->type = Ft_Icon;

            DownLoadIcon(http, vData);

            lParam = TRUE;
            break;
        }
    }
    catch (...) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"Json解析失败！");
    }

    if (m_bNeedExit)
        return;
    if (IsWindow(m_hWnd))
        ::PostMessage(m_hWnd, m_uMsg, (WPARAM)this, lParam);
}

void CWebHelperThread::RequestUrl() {
    char szData[256] = { 0 };
    sprintf_s(szData, "game_id=%I64d", m_pTask->nGameID);
    string strParam = PublicLib::AToUtf(szData);
	LPARAM lParam = FALSE;
	try {
		PublicLib::CHttpClient http;
		while (true) {
            Json::Value vRoot;

            auto RequestUrlInfo = [&](LPCTSTR lpUrl) -> bool {
                http.AddHttpHeader(L"version", L"1_1.0");
                http.AddHttpHeader(L"Device-Id", L"356261050135555");
                http.AddHttpHeader(L"Unique-Code", L"1AEE004A-73D0-4427-B1C4-B771A5CA1732");
                http.AddHttpHeader(L"Access-Token", L"A2838C35DAAAE9058E521930B8D27F9A0538C7F8D7CD248A263820E6A6B27FB12058141FDA3CEF5D98BDC24C53F07B2B");
                http.AddHttpHeader(L"Guoren-App-Interal-Tags", L"Salem501200");

                string strJson = http.Request(lpUrl, PublicLib::Post, strParam.c_str(), strParam.size());
                if (strJson.empty()) {
                    m_pTask->state = Ts_Error;
                    OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
                    return false;
                }

                Json::Reader rd;
                if (!rd.parse(strJson, vRoot)) {
                    m_pTask->state = Ts_Error;
                    OUTPUT_XYLOG(LEVEL_ERROR, L"解析返回json失败");
                    return false;
                }

                string strCode = vRoot["code"].asString();
                if (strCode.compare("600") != 0) {
                    m_pTask->state = Ts_Error;
                    OUTPUT_XYLOG(LEVEL_ERROR, L"接口返回失败，code = %s", PublicLib::Utf8ToU(strCode).c_str());
                    return false;
                }

                return true;
            };

            if (!RequestUrlInfo(URL_REQUEST_GAME)) break;

			Json::Value& vData = vRoot["data_info"];

            string strName = vData["game_name"].asString();
            string strVersion = vData["game_ver"].asString();

            m_pTask->type = Ft_Game;
            m_pTask->strVersion = PublicLib::Utf8ToU(strVersion);
			wstring strTemp = PublicLib::Utf8ToU(strName);
			strTemp = PublicLib::StrReplaceW(strTemp, L"?", L"_");
			strTemp = PublicLib::StrReplaceW(strTemp, L"/", L"_");
			m_pTask->strName = strTemp;
			m_pTask->strUrl = vData["game_down_load_url"].asString();//down_url_remote
			m_pTask->strGameInfo = vData["game_describe"].asString();
            m_pTask->strIconUrl = vData["game_ico_cy"].asString();

            DownLoadIcon(http, vData);

			lParam = TRUE;
			break;
		}
	}
	catch (...) {
		OUTPUT_XYLOG(LEVEL_ERROR, L"Json解析失败！");
	}

	if (m_bNeedExit)
		return;
	if (m_pTask->state == Ts_Delete)
		return;
	if (IsWindow(m_hWnd))
		::PostMessage(m_hWnd, m_uMsg, (WPARAM)this, lParam);
}

void CompressImageFile(const wchar_t* src_file, int new_width, const wchar_t* dst_file) {
    if (new_width == 0) return;

    CxImage img;
    CxImage press_img;

    do {
        img.Load(src_file, CXIMAGE_SUPPORT_PNG);
        if (!img.IsValid()) break;

        //CRenderEngine::DrawRoundRectAntiAlias(hbmp, hDC, oldItemRect.left, oldItemRect.top, 5, 5, 0, 0, 50);

        auto width = img.GetWidth();
        auto height = img.GetHeight();
        if (width == 0 || height == 0) break;

        height = int((float)(height * new_width) / (float)width);

        img.Resample(new_width, height, 0, &press_img);
        if (!press_img.IsValid()) break;

        auto type = press_img.GetType();
        if (type == CXIMAGE_FORMAT_PNG) {
            press_img.IncreaseBpp(8);
            press_img.Save(dst_file, CXIMAGE_FORMAT_PNG);
        }
        else if (type == CXIMAGE_FORMAT_JPG) {
            press_img.Save(dst_file, CXIMAGE_FORMAT_JPG);
        }

    } while (false);

    img.Destroy();
    press_img.Destroy();
}

void CWebHelperThread::DownLoadIcon(PublicLib::CHttpClient& http, Json::Value& vData) {
    wstring strIcoUrl = PublicLib::Utf8ToU(vData["game_ico_cy"].asString());
    wstring strIcoPath = GetGameIconFile(m_pTask->strName, m_pTask->strVersion);
    if (!PathFileExists(strIcoPath.c_str()) && !http.DownLoadFile(strIcoUrl.c_str(), strIcoPath.c_str())) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"图标下载失败，url = %s", strIcoUrl.c_str());
    }
    else {
        wstring str130Path = GetGameIcon130File(m_pTask->strName, m_pTask->strVersion);
        if (!PathFileExists(str130Path.c_str()))
        {
            CompressImageFileW(strIcoPath.c_str(), 130, 130, 100, str130Path.c_str());
        }
    }

    wstring strPicUrl = PublicLib::Utf8ToU(vData["game_big_pic"].asString());
    if (strPicUrl.empty()) {
        strPicUrl = PublicLib::Utf8ToU(vData["game_cover_pic"].asString());
    }

    wstring strPicPath = GetGameLogoFile(m_pTask->strName, m_pTask->strVersion);
    if (!PathFileExists(strPicPath.c_str()) && !http.DownLoadFile(strPicUrl.c_str(), strPicPath.c_str())) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"图标下载失败，url = %s", strPicUrl.c_str());
    }
    else {
        // 图标存在
    }
}