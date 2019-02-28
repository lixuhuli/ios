#include "stdafx.h"
#include "WndMain.h"
#include "MsgDefine.h"
#include "Download/Task.h"
#include "Database/DatabaseMgr.h"
#include "UserData.h"
#include "DataCenter/DataCenterApi.h"
#include "Download/DownloadMgr.h"
#include "CommonWnd.h"

//copydata��Ϣ���治Ҫ����
LRESULT CWndMain::OnMsgCopyData(WPARAM wParam, LPARAM lParam) {
	COPYDATASTRUCT* pCopyData = (COPYDATASTRUCT*)lParam;
	switch (pCopyData->dwData) {
    case ID_MW_COPYDATA_JS_STARTGAME : return OnCopyDataStartGame(pCopyData);
    case ID_MW_COPYDATA_JS_GAME_STATUS: return OnCopyDataGameStatus(pCopyData);
    case ID_MW_COPYDATA_JS_SHOWMSG: return OnCopyDataShowMsg(pCopyData);
    case ID_MW_COPYDATA_JS_SHOWTOAST: return OnCopyDataShowToast(pCopyData);
    case ID_MW_COPYDATA_JS_SHOWCONFIRM: return OnCopyDataShowConfirm(pCopyData);
	default: break;
	}

	return 0;
}

LRESULT CWndMain::OnCopyDataStartGame(COPYDATASTRUCT* pCopyData) {
    if (CUserData::Instance()->GetUserState() == UsNone) {
        ::PostMessage(m_hWnd, WM_MAINWND_MSG_COMMON, WpCommonLogin, 0);    // ��¼
        return 0;
    }

    string strJson((char*)pCopyData->lpData, pCopyData->cbData);
    try {
        Json::Reader read;
        Json::Value root;
        if (read.parse(strJson, root)) {
            string strGameID = root["gameId"].asString();
            wstring strName = PublicLib::Utf8ToU(root["gameName"].asString());
            string strVer = root["version"].asString();
            int nLoadWay = root.isMember("loadWay") ? root["loadWay"].asInt() : 1;

            wstring strNewVer = PublicLib::Utf8ToU(strVer);
            __int64 nGameID = _atoi64(strGameID.c_str());

            ITask* pTask = CDatabaseMgr::Instance()->GetGameInfo(nGameID);

            if (pTask && strNewVer != pTask->strVersion) {
                OUTPUT_XYLOG(LEVEL_INFO, L"��Ϸ��ǰ�汾��%s���������汾��%s����ʼ���²��", pTask->strVersion.c_str(), strNewVer.c_str());
                CDownloadMgr::Instance()->AddTask(Tht_Url, nGameID, strName, nLoadWay);
                return 0;
            }

            CDownloadMgr::Instance()->AddTask(Tht_Url, nGameID, strName, nLoadWay);
        }
    }
    catch (...)  {
        OUTPUT_XYLOG(LEVEL_ERROR, L"������js���͹�����json����ʧ��");
    }

    return 1;
}

LRESULT CWndMain::OnCopyDataGameStatus(COPYDATASTRUCT* pCopyData) {
    string strJson((char*)pCopyData->lpData, pCopyData->cbData);

    int nStatus = 0;
    try {
        Json::Value vRoot;
        Json::Reader rd;
        if (rd.parse(strJson, vRoot)) {
            Json::Value& vGameID = vRoot["gameId"];
            __int64 nGameID = 0;
            if (vGameID.isString())
                nGameID = _atoi64(vGameID.asString().c_str());
            else if (vGameID.isInt64())
                nGameID = vGameID.asInt64();

            string strVer = vRoot["version"].asString();
            wstring strVersion = PublicLib::Utf8ToU(strVer);
            nStatus = CDownloadMgr::Instance()->GetGameStatus(nGameID, strVersion);
        }
    }
    catch (...) {

    }

    return nStatus;
}

LRESULT CWndMain::OnCopyDataShowMsg(COPYDATASTRUCT* pCopyData) {
    string strJson((char*)pCopyData->lpData, pCopyData->cbData);

    try {
        Json::Value vRoot;
        Json::Reader rd;
        if (rd.parse(strJson, vRoot)) {
            string prefix = vRoot["prefix"].asString();
            ShowMsg(m_hWnd, L"��ʾ", PublicLib::Utf8ToU(prefix).c_str(), MB_OK);
        }
    }
    catch (...) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"����������ʾʧ�ܣ����ݣ�%s", PublicLib::Utf8ToU(strJson).c_str());
    }

    return 0;
}

LRESULT CWndMain::OnCopyDataShowToast(COPYDATASTRUCT* pCopyData) {
    string strJson((char*)pCopyData->lpData, pCopyData->cbData);

    try {
        Json::Value vRoot;
        Json::Reader rd;
        if (rd.parse(strJson, vRoot)) {
            string prefix = vRoot["prefix"].asString();
            ShowToast(m_hWnd, PublicLib::Utf8ToU(prefix).c_str());
        }
    }
    catch (...) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"����toast��ʾʧ�ܣ����ݣ�%s", PublicLib::Utf8ToU(strJson).c_str());
    }

    return 0;
}


LRESULT CWndMain::OnCopyDataShowConfirm(COPYDATASTRUCT* pCopyData) {
    string strJson((char*)pCopyData->lpData, pCopyData->cbData);

    int nStatus = IDCANCEL;
    try {
        Json::Value vRoot;
        Json::Reader rd;
        if (rd.parse(strJson, vRoot)) {
            string prefix = vRoot["prefix"].asString();
            nStatus = ShowMsg(m_hWnd, L"�˳�", PublicLib::Utf8ToU(prefix).c_str(), MB_OKCANCEL);
        }
    }
    catch (...) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"����toast��ʾʧ�ܣ����ݣ�%s", PublicLib::Utf8ToU(strJson).c_str());
    }

    return nStatus;
}
