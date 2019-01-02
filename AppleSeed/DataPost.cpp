#include "stdafx.h"
#include "DataPost.h"
#include "GlobalData.h"
#include "DataCenter\TaskEnum.h"



void PostStartPC(bool bAuto)
{
	Json::Value vParams;
	//vParams[KEY_STATUS] = bAuto ? 0 : 1;
	CreateDataTask(EVENT_START_CLIENT, vParams, 0);
}

void PostDownloadGame(__int64 nGameID, int nGameType, DataStatus status, int nLoadWay/* = 1*/, int nSpeed/* = 0*/) {
	Json::Value vParams;
	vParams[KEY_GAME_ID] = nGameID;
	vParams[KEY_DOWNLOAD_STATUS] = status;
	vParams[KEY_DOWNLOAD_SPEED] = nSpeed;
	CreateDataTask(EVENT_GAME_DOWNLOAD, vParams, CUserData::Instance()->GetFileUserID());
}

void PostStartGame(__int64 nGameID, DataStatus status, int nUid) {
	Json::Value vParams;
	vParams[KEY_GAME_ID] = nGameID;
	vParams[KEY_DOWNLOAD_STATUS] = status;
	CreateDataTask(EVENT_START_GAME, vParams, nUid);
}

void PostInstallGame(__int64 nGameID, DataStatus status, int nUid) {
    Json::Value vParams;
    vParams[KEY_GAME_ID] = nGameID;
    vParams[KEY_DOWNLOAD_STATUS] = status;
    CreateDataTask(EVENT_INSTALL_GAME, vParams, nUid);
}

void PostClick(const char* event_name) {
    //CreateDataTask(event_name, Json::Value(Json::objectValue), CUserData::Instance()->GetUserID());
}

void PostUserLogin(int nUid) {
    Json::Value vParams;
    vParams["ipAddress"] = CUserData::Instance()->GetUserIp();
    CreateDataTask(EVENT_LOGIN, vParams, nUid);
}

void PostCrashDump(DWORD dwExceptCode) {
    Json::Value vParams;
    vParams[KEY_DUMP_TYPE] = 1;
    vParams[KEY_DUMP_CODE] = (unsigned int)dwExceptCode;
    CreateDataTask(EVENT_DUMP, vParams, CUserData::Instance()->GetFileUserID());
}

