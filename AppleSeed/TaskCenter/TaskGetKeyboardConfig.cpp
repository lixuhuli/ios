#include "StdAfx.h"
#include "TaskGetKeyboardConfig.h"
#include <PublicLib\DeCompress.h>
#include "ParamDefine.h"
#include "HttpInterface.h"


namespace TaskCenter{
	CTaskGetKeyBoardConfig::CTaskGetKeyBoardConfig(const MSG& msg, const string& strAppId, const wstring& strDownloadPath, const wstring& strFileDir)
		: ITask(msg)
		, m_pNeedExit(NULL)
		, m_strAppId(strAppId)
        , m_strDownloadPath(strDownloadPath)
		, m_strFileDir(strFileDir) {
		m_type = TaskGetKeyboardConfig;
		m_bUsed = TRUE;
	}

	CTaskGetKeyBoardConfig::~CTaskGetKeyBoardConfig() {
	}

	void CTaskGetKeyBoardConfig::Init(const MSG& msg, const string& strAppId, const wstring& strDownloadPath, const wstring& strFileDir) {
		m_msg = msg;
		m_bUsed = TRUE;
		m_strAppId = strAppId;
        m_strDownloadPath = strDownloadPath;
		m_strFileDir = strFileDir;
	}

	void CTaskGetKeyBoardConfig::SetNeedExit(BOOL* pNeedExit) {
		m_pNeedExit = pNeedExit;
	}

	void CTaskGetKeyBoardConfig::Run() {
        if (m_strAppId.empty() || m_strDownloadPath.empty() || m_strFileDir.empty()) return;

        PublicLib::CHttpClient http;

        auto strUrl = URL_GET_KEYBOARD + PublicLib::AToU(m_strAppId) + L".zip";
        if (!http.DownLoadFile(strUrl.c_str(), m_strDownloadPath.c_str())) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"下载映射文件失败：文件地址：%s，下载到：%s", strUrl.c_str(), m_strDownloadPath.c_str());
            m_strResult = "0";
            return;
        }

		DeCompressParams params;
		params.lpProgressCallback = nullptr;
		params.lpParam = this;
		params.lpNeedExit = m_pNeedExit;

        if (!PublicLib::DeCompressZipFile(m_strDownloadPath, m_strFileDir, params)) {
            OUTPUT_XYLOG(LEVEL_ERROR, L"解压失败：文件：%s，解压到：%s，错误信息：%s", m_strDownloadPath.c_str(), m_strFileDir.c_str(), params.strError.c_str());
            m_strResult = "0";
            ::DeleteFile(m_strDownloadPath.c_str());
            return;
        }

        m_strResult = "1";
        ::DeleteFile(m_strDownloadPath.c_str());
	}
}