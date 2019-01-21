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

// �û���¼���
void PostUserLogin(int nUid);

// �������
void PostCrashDump(DWORD dwExceptCode);