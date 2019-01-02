#pragma once
/*******************************************
*	数据统计事件、字段定义
*	2017年10月19日14:46:18
*	Yaojn
*/


////////////////////////////////////////////////////////////////////////////////////////////
//定义事件名称
#define EVENT_START_CLIENT			"startClient"
#define EVENT_GAME_DOWNLOAD			"gameDownload"
#define EVENT_START_GAME			"gameStart"
#define EVENT_INSTALL_GAME			"gameInstall"
#define EVENT_UNINSTALL				"clientRemove"
#define EVENT_INSTALL				"clientInstall"

#define EVENT_LOGIN					"userLogin"
#define EVENT_CREATE_ROOM			"createRoom"
#define EVENT_CLICK_LOBBY			"clickLobby"		//点击大厅游戏
#define EVENT_CLICK_COLL_PAGE		"clickComPage"		//点击大厅合集
#define EVENT_ROOM_NUMBER			"lobbyRoomNumber"	//大厅房间数
#define EVENT_DUMP					"collDetails"		//崩溃收集
#define EVENT_GAME_UNZIP			"gameUnzip"			//解压游戏
#define EVENT_START_RANK			"clickVsStart"		//开始排位赛


///////////////////////////////////////////////////////////////////////////////////////////
//定义数据字段
#define KEY_DEVICE_ID			"deviceId"
#define KEY_AD_ID				"adId"
#define KEY_USER_ID				"uid"
#define KEY_APP_VER				"appVersion"
#define KEY_OS_VER				"osVersion"
#define KEY_OS_TYPE				"osType"
#define KEY_MAC					"mac"
#define KEY_TIME				"clickTime"				//操作时间
#define KEY_GAME_ID				"gameId"
#define KEY_GAME_TYPE			"gameType"				//街机、FC、PSP、GBA
#define KEY_GAME_MODE			"gameMode"				//本地游戏、对战
#define KEY_ROOM_ID				"roomId"
#define KEY_GAME_VER			"gameVersion"			//游戏版本
#define KEY_ROOM_NUMBER			"roomNumber"			//房间数目
#define KEY_DOWNLOAD_SPEED		"downSpeed"			    //下载速度
#define KEY_DOWNLOAD_STATUS		"opStatus"				//下载状态（0.请求下载；1.下载失败；2.下载成功）
#define KEY_UNZIP_SPEED			"unzipSpeed"			//解压速度
#define KEY_UNZIP_STATUS		"opStatus"				//解压状态（0.请求解压；1：解压成功；2：解压失败）
#define KEY_STATUS				"status"				//状态0：自启动；1：手工启动
#define KEY_ROOM_TYPE			"matchLevel"			//房间级别（0：普通场；1：精英场）
		

//崩溃相关
#define KEY_DUMP_TYPE			"collType"				//崩溃类型（1：平台崩溃；2：pc模拟器崩溃）"
#define KEY_DUMP_CODE			"collReason"			//崩溃原因

