#pragma once

namespace TaskCenter{

	enum TaskType
	{
		TaskBase = 0,
		TaskSearchAuto,
		TaskRecommend,
		TaskGetServer,
		TaskUserLogin,
		TaskUserInfo,
        TaskGetVerificationCode,
        TaskGetUserCheckCode,
        TaskGetUserCheckMobileCode,
        TaskGetUserRegister,
        TaskGetUserLogin,
        TaskGetUserCodeLogin,
        TaskGetUserModifyPassword,
		TaskGetGameList,
		TaskUserIco,
		TaskPlayerIco,
		TaskUnzipGame,
		TaskBootRun,
		TaskCreateRoom,
		TaskDataPost,
		TaskDelDir,
		TaskIMCreateToken,
		TaskIMUpdateInfo,
		TaskIMCheckMsg,
		TaskGameTabList,
		TaskGameRecommand,
		TaskGameRecommandIco,
		TaskContestID,
		TaskDownloadFile,
		TaskUrl,
		TaskUploadRecord,
		TaskUploadAvatar,
		TaskCheckGameUpdate,
        TaskInstallAndroidEmulator,
        TaskGetUpdateLog,
        TaskUnzipFile,
        TaskIosEngineUpdate,
	};


	enum PostEventType
	{
		EventStart = 0,//启动
		EventDownload,//下载游戏
		EventStartGame,//启动游戏
		EventUninstall,//卸载
	};

	typedef struct _PlayerIcoParam
	{
		__int32 nUid;
		__int32 nFlag;
		void	*lpData;
		string strUrl;
		wstring strSavePath;

	}PlayerIcoParam, *LPlayerIcoParam;
}