#pragma once



typedef struct _EnterRoomParam
{
	int nGameType;
	__int64 nID;
	wstring strName;

}EnterRoomParam, *LPEnterRoomParam;

typedef struct _EnterLiveRoomParam
{
	int nRoomID;
	int nRoomType;

}EnterLiveRoomParam, *LPEnterLiveRoomParam;

typedef struct _PasswordWndParam
{
	bool bElite;
	int nBattleArea;
	int nRoomID;
	int nSimpleRoomType;
	__int64 nGameID;
	wstring strGameName;
}PasswordWndParam, *LPPasswordWndParam;

typedef struct _UnzipFinishParam
{
	bool bSuccess;
	int nSpeed;

}UnzipFinishParam, *LUnzipFinishParam;

typedef struct _CreateRoomParam
{
	bool bElite;
	bool bAllowMobileIn;

}CreateRoomParam, *LPCreateRoomParam;