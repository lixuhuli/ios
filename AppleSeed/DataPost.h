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
    StatusSuccess,
	StatusFail,
	StatusPause,
};


void PostStartPC(bool bAuto);

void PostStartApp(DataStatus status, int nUid);

void PostDownloadGame(__int64 nGameID, int nGameType, DataStatus status, int nLoadWay = 1, int nSpeed = 0);

void PostStartGame(__int64 nGameID, DataStatus status, int nUid);

void PostInstallGame(__int64 nGameID, DataStatus status, int nUid);

void PostClick(const char* event_name);

// 用户登录打点
void PostUserLogin(int nUid);

// 程序崩溃
void PostCrashDump(DWORD dwExceptCode);