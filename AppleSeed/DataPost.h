#pragma once
#include <DataCenter\DataCenterApi.h>
#include "UserData.h"
/*****************************
*用户数据统计包装层
*2017年10月19日
*Yaojn
*/

enum DataStatus
{
	StatusBegin = 0,
	StatusFail,
	StatusSuccess,
};


void PostStartPC(bool bAuto);

void PostDownloadGame(__int64 nGameID, int nGameType, DataStatus status, int nLoadWay = 1, int nSpeed = 0);

void PostStartGame(__int64 nGameID, int nGameType, int nGameMode, int nRoomType, int nRoomID = 0);

// 开始对战模式
void PostStartBattle(__int64 nGameID, int nGameType, const wstring& strGameNname);

void PostClick(const char* event_name);

//用户登录打点
inline void PostUserLogin(int nUid)
{
	Json::Value vParams;
    vParams["userIp"] = CUserData::Instance()->GetUserIp();
	//CreateDataTask(EVENT_LOGIN, vParams, nUid);
}

//创建房间
inline void PostCreateRoom(__int64 nGameID, const char* pGameVer, int nGameType, int nGameMode, int nRoomType)
{
	Json::Value vParams;
	vParams[KEY_GAME_ID] = nGameID;
	vParams[KEY_GAME_VER] = pGameVer;
	vParams[KEY_GAME_TYPE] = nGameType;
	vParams[KEY_GAME_MODE] = nGameMode;
	vParams[KEY_ROOM_TYPE] = nRoomType;
	//CreateDataTask(EVENT_CREATE_ROOM, vParams, CUserData::Instance()->GetUserID());
}

//游戏解压
inline void PostUnzipGame(__int64 nGameID, int nGameType, DataStatus status, int nSpeed = 0)
{
	Json::Value vParams;
	vParams[KEY_GAME_ID] = nGameID;
	vParams[KEY_GAME_TYPE] = nGameType;
	vParams[KEY_UNZIP_STATUS] = status;
	vParams[KEY_UNZIP_SPEED] = nSpeed;
	//CreateDataTask(EVENT_GAME_UNZIP, vParams, CUserData::Instance()->GetUserID());
}

//程序崩溃
inline void PostCrashDump(DWORD dwExceptCode)
{
	Json::Value vParams;
	vParams[KEY_DUMP_TYPE] = 1;
	vParams[KEY_DUMP_CODE] = (unsigned int)dwExceptCode;
	//CreateDataTask(EVENT_DUMP, vParams, CUserData::Instance()->GetUserID());
}

//开始排位赛
inline void PostStartRank(__int64 nGameID, const char* pGameVer, int nGameType, int nGameMode)
{
	Json::Value vParams;
	vParams[KEY_GAME_ID] = nGameID;
	vParams[KEY_GAME_VER] = pGameVer;
	vParams[KEY_GAME_TYPE] = nGameType;
	vParams[KEY_GAME_MODE] = nGameMode;
	//CreateDataTask(EVENT_START_RANK, vParams, CUserData::Instance()->GetUserID());
}