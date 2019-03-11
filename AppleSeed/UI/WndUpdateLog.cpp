#include "StdAfx.h"
#include "WndUpdateLog.h"
#include "UrlDefine.h" 
#include "TaskCenter/TaskCenter.h"
#include "HttpInterface.h"

#define WM_MSG_GET_UPDATELOG  1001

CWndUpdateLog::CWndUpdateLog()
 : label_prifix_(nullptr)
 , label_version_(nullptr)
 , label_icon_(nullptr) {
	m_bEscape = true;
}

CWndUpdateLog::~CWndUpdateLog() {
}

bool CWndUpdateLog::OnClickBtnClose(void* lpParam) {
    Close(IDCLOSE);
	return true;
}

void CWndUpdateLog::InitWindow() {
	__super::InitWindow();

    auto version = GetApplicationVersion();

    //version = L"2.0.0.2276";

    MSG msg = { 0 };
    msg.hwnd = m_hWnd;
    msg.message = WM_MSG_GET_UPDATELOG;
    TaskCenter::CTaskCenter::Instance()->CreateGetUpdateLogTask(msg, version);
}

LRESULT CWndUpdateLog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;

    switch (uMsg) {
    case WM_MSG_GET_UPDATELOG: lRes = OnMsgGetUpdateLog(uMsg, wParam, lParam, bHandled); break;
    default: 
        bHandled = FALSE;
        break;
    }

    if (bHandled) {
        return lRes;
    }

	return CWndBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CWndUpdateLog::OnMsgGetUpdateLog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    bHandled = TRUE;

    if (!label_prifix_ || !label_version_ | !label_icon_) return 0;

    UINT_PTR nTask = (UINT_PTR)lParam;
    string strJson;
    if (!TaskCenter::CTaskCenter::Instance()->GetTaskResult(nTask, strJson)) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"错误的任务ID");
        return 0;
    }

    Json::Value vRoot;
    Json::Reader rd;
    if (!rd.parse(strJson, vRoot)) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"json解析失败");
        return 0;
    }

    string strCode = vRoot["code"].asString();
    if (strCode.compare("600") != 0) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"接口返回失败，code = %s", PublicLib::Utf8ToU(strCode).c_str());
        return 0;
    }

    Json::Value& vData = vRoot["data_info"][0];

    if (vData.isNull()) return 0;

    string label = vData["label"].asString();
    auto temp = PublicLib::Utf8ToU(label);

    auto version = L"版本号：" + GetApplicationVersion();
    wstring prefix = PublicLib::StrReplaceW(temp, L"\\r\\n", L"\r\n");;

    label_prifix_->SetText(prefix.c_str());
    label_version_->SetText(version.c_str());

    bool show = false;// (vData["is_show"].asString().compare("1") == 0);
    if (!show) return 0;

    wstring strPath = GetAppDataPath() + L"UpdateLog\\";
    if (!PathFileExists(strPath.c_str()))
        SHCreateDirectory(NULL, strPath.c_str());

    PublicLib::CHttpClient http;
    wstring strIcoUrl = L"http://ctimg2018.5fun.com/upload/" + PublicLib::Utf8ToU(vData["pic_local"].asString());
    wstring strIcoPath = strPath + GetApplicationVersion() + L"_";
    strIcoPath.append(PublicLib::Utf8ToU(vData["times"].asString()));
    strIcoPath.append(L".png");
    if (!PathFileExists(strIcoPath.c_str()) && !http.DownLoadFile(strIcoUrl.c_str(), strIcoPath.c_str())) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"图标下载失败，url = %s", strIcoUrl.c_str());
    }
    else {
        label_icon_->SetVisible(true);
        label_icon_->SetBkImage(strIcoPath.c_str());
    }

    return 0;
}

CControlUI* CWndUpdateLog::CreateControl(LPCTSTR pstrClass) {
    std::wstring control_name = pstrClass;
    //if (control_name == L"CefWebUI") return new CCefWebkitUI();

    return nullptr;
}

LPCWSTR CWndUpdateLog::GetXmlPath() const {
	return L"WndUpdateLog.xml";
}
