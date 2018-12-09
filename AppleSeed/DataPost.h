#pragma once
#include <DataCenter\DataCenterApi.h>
#include "UserData.h"
/*****************************
*�û�����ͳ�ư�װ��
*2017��10��19��
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

// ��ʼ��սģʽ
void PostStartBattle(__int64 nGameID, int nGameType, const wstring& strGameNname);

void PostClick(const char* event_name);

//�û���¼���
inline void PostUserLogin(int nUid)
{
	Json::Value vParams;
    vParams["userIp"] = CUserData::Instance()->GetUserIp();
	//CreateDataTask(EVENT_LOGIN, vParams, nUid);
}

//��������
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

//��Ϸ��ѹ
inline void PostUnzipGame(__int64 nGameID, int nGameType, DataStatus status, int nSpeed = 0)
{
	Json::Value vParams;
	vParams[KEY_GAME_ID] = nGameID;
	vParams[KEY_GAME_TYPE] = nGameType;
	vParams[KEY_UNZIP_STATUS] = status;
	vParams[KEY_UNZIP_SPEED] = nSpeed;
	//CreateDataTask(EVENT_GAME_UNZIP, vParams, CUserData::Instance()->GetUserID());
}

//�������
inline void PostCrashDump(DWORD dwExceptCode)
{
	Json::Value vParams;
	vParams[KEY_DUMP_TYPE] = 1;
	vParams[KEY_DUMP_CODE] = (unsigned int)dwExceptCode;
	//CreateDataTask(EVENT_DUMP, vParams, CUserData::Instance()->GetUserID());
}

//��ʼ��λ��
inline void PostStartRank(__int64 nGameID, const char* pGameVer, int nGameType, int nGameMode)
{
	Json::Value vParams;
	vParams[KEY_GAME_ID] = nGameID;
	vParams[KEY_GAME_VER] = pGameVer;
	vParams[KEY_GAME_TYPE] = nGameType;
	vParams[KEY_GAME_MODE] = nGameMode;
	//CreateDataTask(EVENT_START_RANK, vParams, CUserData::Instance()->GetUserID());
}