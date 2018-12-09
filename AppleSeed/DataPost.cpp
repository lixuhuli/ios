#include "stdafx.h"
#include "DataPost.h"
#include "GlobalData.h"



void PostStartPC(bool bAuto)
{
	Json::Value vParams;
	vParams[KEY_OS_VER] = PublicLib::UToUtf8(CGlobalData::Instance()->GetOSVersion());
	vParams[KEY_OS_TYPE] = PublicLib::UToUtf8(CGlobalData::Instance()->GetOSType());
	vParams[KEY_MAC] = PublicLib::UToUtf8(CGlobalData::Instance()->GetMac());
	vParams[KEY_STATUS] = bAuto ? 0 : 1;
	//CreateDataTask(EVENT_START_CLIENT, vParams, CUserData::Instance()->GetFileUserID());
}

void PostDownloadGame(__int64 nGameID, int nGameType, DataStatus status, int nLoadWay/* = 1*/, int nSpeed/* = 0*/)
{
	Json::Value vParams;
	vParams[KEY_GAME_ID] = nGameID;
	vParams[KEY_GAME_TYPE] = nGameType;
	vParams[KEY_DOWNLOAD_STATUS] = status;
    vParams[KEY_DOWNLOAD_LOADWAY] = nLoadWay;
	vParams[KEY_DOWNLOAD_SPEED] = nSpeed;
	//CreateDataTask(EVENT_GAME_DOWNLOAD, vParams, CUserData::Instance()->GetUserID());
}

void PostStartGame(__int64 nGameID, int nGameType, int nGameMode, int nRoomType, int nRoomID/* = 0*/)
{
	Json::Value vParams;
	vParams[KEY_GAME_ID] = nGameID;
	vParams[KEY_GAME_TYPE] = nGameType;
	vParams[KEY_GAME_MODE] = nGameMode;
	vParams[KEY_ROOM_ID] = nRoomID;
	vParams[KEY_ROOM_TYPE] = nRoomType;
	//CreateDataTask(EVENT_START_GAME, vParams, CUserData::Instance()->GetUserID());
}

void PostStartBattle(__int64 nGameID, int nGameType, const wstring& strGameNname) {
    Json::Value vParams;
    vParams[KEY_GAME_ID] = nGameID;
    vParams[KEY_GAME_TYPE] = nGameType;
    vParams["bannerPosition"] = "";
    vParams["firstModule"] = PublicLib::AToUtf("对战大厅");
    vParams["secoendModule"] = PublicLib::AToUtf("本地游戏");
    vParams["thirdModule"] = PublicLib::UToUtf8(strGameNname);
    vParams["fourModule"] = "";
    //CreateDataTask("clickNewIndexPage", vParams, CUserData::Instance()->GetUserID());
}

void PostClick(const char* event_name) {
    //CreateDataTask(event_name, Json::Value(Json::objectValue), CUserData::Instance()->GetUserID());
}

