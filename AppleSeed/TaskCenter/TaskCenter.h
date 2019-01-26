/***************************************
*Desc:	任务管理中心
*Author:姚佳宁
*Date:	2017年6月21日10:58:12
*/
#pragma once
#include "TaskEnum.h"



namespace TaskCenter{

	typedef list<UINT_PTR>::iterator TaskItor;
	typedef list<UINT_PTR>::const_iterator TaskConstItor;
	class CTaskCenter
	{
	public:
		static CTaskCenter* Instance();
		void Init();
		void Exit();
		bool GetTaskResult(UINT_PTR nTaskID, OUT string& strResult);
		void DetachTask(UINT_PTR nTaskID);
		void CancelTask(UINT_PTR nTaskID);
		UINT_PTR CreateSearchTask(const MSG& msg, const wstring& strWord, bool bAuto, int nPage);
		bool GetSearchTaskWord(UINT_PTR nTaskID, OUT wstring& strWord);
		UINT_PTR CreateRecommendTask(const MSG& msg, int nPage);
		UINT_PTR CreateGetServerTask(const MSG& msg, int nUid, const string& strToken);
        UINT_PTR CreateGetVerificationCode(const MSG& msg, const wstring& strPhoneNumber, const wstring& strCheckType);
        UINT_PTR CreateUserCheckCode(const MSG& msg, const wstring& strPhoneNumber, const wstring& strVerificationCode, const wstring& strActiveCode);
        UINT_PTR CreateUserCheckMobileCode(const MSG& msg, const wstring& strPhoneNum, const wstring& strCode, const wstring& strCheckType);
        UINT_PTR CreateUserRegister(const MSG& msg, const wstring& strPhoneNumber, const wstring& strVerificationCode, const wstring& strActiveCode, const wstring& strPassword);
        UINT_PTR CreateUserLoginTask(const MSG& msg, const wstring& strAccountId, const wstring& strPassword);
        bool GetUserLoginTaskParam(UINT_PTR nTaskID, OUT wstring& strAccountId);
        UINT_PTR CreateUserCodeLoginTask(const MSG& msg, const wstring& strPhoneNum, const wstring& strCode);
        UINT_PTR CreateUserModifyPassword(const MSG& msg, const wstring& strPhoneNumber, const wstring& strCode, const wstring& strPassword);
		UINT_PTR CreateUserInfoTask(const MSG& msg, int nUid, const string& strToken, bool bUpdateInfo=false);
		bool IsUpdateUserInfoTask(UINT_PTR nTaskID, OUT bool& bUpdateInfo);
		bool GetUserInfoTaskParam(UINT_PTR nTaskID, OUT int& nUid, OUT string& strToken);
		UINT_PTR CreateGetGameListTask(const MSG& msg, __int64 nID, int nIDType);//类型(1 游戏 2合集)
		UINT_PTR CreateUserIcoTask(const MSG& msg, const wstring& strUrl, const wstring& strUserPath);
		bool GetUserIcoPath(UINT_PTR nTaskID, OUT wstring& strIcoPath, OUT wstring& strIcoUrl);
		UINT_PTR CreateUnzipGameTask(const MSG& msg, UINT uMsg, UINT_PTR nTaskID, const wstring& strZipFile, const wstring& strOutDir);
        UINT_PTR CreateUnzipFileTask(const MSG& msg, UINT uMsg, const wstring& strZipFile, const wstring& strOutDir);
        UINT_PTR CreateIosEngineUpdateTask(const MSG& msg, UINT uMsg, const wstring& strVersion);
        UINT_PTR CreateGetKeyBoardConfigTask(const MSG& msg, const string& strAppId, const wstring& strDownloadPath, const wstring& strFileDir);
        bool GetKeyBoardConfigAppId(UINT_PTR nTaskID, OUT string& strAppId);
		UINT_PTR CreateBootRunTask(bool bBootRun);
		UINT_PTR CreateRoomTask(const MSG& msg, int nUid, const string& strToken, __int64 nGameID);
		UINT_PTR CreateDelDirTask(const list<wstring>& listDirs, const list<wstring>& listFiles);
		UINT_PTR CreateGameTabListTask(const MSG& msg);
		UINT_PTR CreateGameRecommandTask(const MSG& msg);
		UINT_PTR CreateGameRecommandIcoTask(const MSG& msg, const wstring& strIcoUrl, const wstring& strSavePath);
		UINT_PTR CreateDownloadFileTask(const MSG& msg, const string& strUrl, const wstring& strSavePath);
		UINT_PTR CreateUrlTask(const MSG& msg, const wstring& strUrl);
        UINT_PTR CreateGetUpdateLogTask(const MSG& msg);
		UINT_PTR CreateCheckGameUpdateTask(const MSG& msg, const std::map<string, string>& gameList);
		//UINT_PTR CreateUploadAvatarTask(const MSG& msg, int nUid, const string& strToken, const wstring& strImgPath);
		bool GetDownloadFileTaskPath(UINT_PTR nTaskID, OUT wstring& strPath);
		bool HasTaskRunning(TaskType type);
		//bool GetUploadAvatarTaskPath(UINT_PTR nTaskID, OUT wstring& strImgPath);

		//IM
		UINT_PTR CreateIMTokenTask(const MSG& msg, const Json::Value& vUserInfo);
		UINT_PTR CreateIMUpdateInfoTask(const string& strJsonInfo);
		UINT_PTR CreateIMCheckMsgTask(const MSG& msg, int nUid, const string& strToken, const string& strMsg);
		//Player icos
		UINT_PTR CreatePlayerIcoTask(const MSG& msg);
		bool AddPlayerIco(UINT_PTR nTaskID, LPlayerIcoParam lpParam);
		bool StopPlayerIcoTask(UINT_PTR nTaskID);
		//game contest
		UINT_PTR CreateContestTask(const MSG& msg, int nUid, __int64 nGameID, const string& strToken, int nFlag);
		UINT_PTR CreateUploadRecordTask(const MSG& msg, const wstring& strPath, const string& strName);
		bool GetUploadTaskPath(UINT_PTR nTaskID, OUT wstring& strPath);

	protected:
		CTaskCenter();
		CTaskCenter(const CTaskCenter& obj){}
		~CTaskCenter();
		static UINT __stdcall Thread(void* lpParam);

	private:
		BOOL m_bNeedExit;
		list<UINT_PTR>	m_taskList;
	};
}
