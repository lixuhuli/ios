#include "StdAfx.h"
#include "TaskCenter.h"
#include "Task.h"
#include "TaskSearch.h"
#include <algorithm>
#include "TaskGameHall.h"
#include "TaskUser.h"
#include "TaskUnzipGame.h"
#include "TaskUtils.h"
#include "TaskIM.h"
#include "TeaskContest.h"
#include "TaskGetUpdateLog.h"
#include "TaskUnzipFile.h"
#include "TaskIosEngineUpdate.h"

namespace TaskCenter{


	CTaskCenter* CTaskCenter::Instance()
	{
		static CTaskCenter obj;
		return &obj;
	}

	inline ITask* GetFreeTaskByType(const list<UINT_PTR>& taskList, TaskType type)
	{
		TaskConstItor itor = taskList.begin();
		for (; itor != taskList.end(); ++itor)
		{
			ITask* pTask = (ITask*)(*itor);
			if (pTask->GetTaskType() == type && !pTask->IsUsed())
				return pTask;
		}
		return NULL;
	}

	CTaskCenter::CTaskCenter()
		: m_bNeedExit(FALSE)
	{
	}


	CTaskCenter::~CTaskCenter()
	{
	}

	void CTaskCenter::Init()
	{
		OUTPUT_XYLOG(LEVEL_INFO, L"初始化CTaskCenter");
	}

	void CTaskCenter::Exit()
	{
		m_bNeedExit = TRUE;
		TaskItor itor = m_taskList.begin();
		for (; itor != m_taskList.end(); ++itor)
		{
			ITask* pTask = (ITask*)(*itor);
			if (pTask->GetTaskType() == TaskPlayerIco)
			{
				CTaskPlayerIco* pPlayerTask = dynamic_cast<CTaskPlayerIco*>(pTask);
				if (pPlayerTask)
				{
					pPlayerTask->Stop();
				}
			}
			WaitForSingleObject(pTask->GetThread(), 10*1000);
			delete pTask;
		}
		m_taskList.clear();
		OUTPUT_XYLOG(LEVEL_INFO, L"反初始化CTaskCenter完成");
	}

	bool CTaskCenter::GetTaskResult(UINT_PTR nTaskID, OUT string& strResult)
	{
		TaskItor itor = std::find(m_taskList.begin(), m_taskList.end(), nTaskID);
		if (itor == m_taskList.end())
		{
			return false;
		}
		ITask* pTask = (ITask*)nTaskID;
		strResult = pTask->GetResult();
		return true;
	}

	void CTaskCenter::DetachTask(UINT_PTR nTaskID)
	{
		TaskItor itor = std::find(m_taskList.begin(), m_taskList.end(), nTaskID);
		if (itor == m_taskList.end())
			return;
		ITask* pTask = (ITask*)nTaskID;
		pTask->SetUse(FALSE);
	}

	void CTaskCenter::CancelTask(UINT_PTR nTaskID)
	{
		TaskItor itor = std::find(m_taskList.begin(), m_taskList.end(), nTaskID);
		if (itor == m_taskList.end())
			return;
		ITask* pTask = (ITask*)nTaskID;
		pTask->Cancel();
	}

	UINT_PTR CTaskCenter::CreateSearchTask(const MSG& msg, const wstring& strWord, bool bAuto, int nPage)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskSearchAuto);
		if (NULL == pTask)
		{
			pTask = new CTaskSearch(msg, strWord, bAuto, nPage);
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskSearch*)pTask)->Init(msg, strWord, bAuto, nPage);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	bool CTaskCenter::GetSearchTaskWord(UINT_PTR nTaskID, OUT wstring& strWord)
	{
		TaskItor itor = std::find(m_taskList.begin(), m_taskList.end(), nTaskID);
		if (itor == m_taskList.end())
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"没有找到对应的任务");
			return false;
		}
		ITask* pTask = (ITask*)nTaskID;
		if (NULL == pTask || pTask->GetTaskType() != TaskSearchAuto)
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"任务类型不正确");
			return false;
		}
		CTaskSearch* pTaskSearch = dynamic_cast<CTaskSearch*>(pTask);
		if (NULL == pTaskSearch)
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"任务指针转换失败");
			return false;
		}
		strWord = pTaskSearch->GetSearchWord();
		return true;
	}

	UINT_PTR CTaskCenter::CreateRecommendTask(const MSG& msg, int nPage)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskRecommend);
		if (NULL == pTask)
		{
			pTask = new CTaskRecommend(msg, nPage);
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskRecommend*)pTask)->Init(msg, nPage);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	UINT_PTR CTaskCenter::CreateGetServerTask(const MSG& msg, int nUid, const string& strToken)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskGetServer);
		if (NULL == pTask)
		{
			pTask = new CTaskGetServer(msg, nUid, strToken);
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskGetServer*)pTask)->Init(msg, nUid, strToken);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

    UINT_PTR CTaskCenter::CreateGetVerificationCode(const MSG& msg, const wstring& strPhoneNumber, const wstring& strCheckType) {
        ITask* pTask = GetFreeTaskByType(m_taskList, TaskGetVerificationCode);
        if (nullptr == pTask) {
            pTask = new CTaskGetVerificationCode(msg, strPhoneNumber, strCheckType);
            m_taskList.push_back((UINT_PTR)pTask);
        }
        else
        {
            pTask->Clear();
            ((CTaskGetVerificationCode*)pTask)->Init(msg, strPhoneNumber, strCheckType);
        }
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
        pTask->SetThread(hThread);
        return (UINT_PTR)pTask;
    }

    UINT_PTR CTaskCenter::CreateUserCheckCode(const MSG& msg, const wstring& strPhoneNumber, const wstring& strVerificationCode, const wstring& strActiveCode) {
        ITask* pTask = GetFreeTaskByType(m_taskList, TaskGetUserCheckCode);
        if (nullptr == pTask) {
            pTask = new CTaskUserCheckCode(msg, strPhoneNumber, strVerificationCode, strActiveCode);
            m_taskList.push_back((UINT_PTR)pTask);
        }
        else
        {
            pTask->Clear();
            ((CTaskUserCheckCode*)pTask)->Init(msg, strPhoneNumber, strVerificationCode, strActiveCode);
        }
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
        pTask->SetThread(hThread);
        return (UINT_PTR)pTask;
    }

    UINT_PTR CTaskCenter::CreateUserCheckMobileCode(const MSG& msg, const wstring& strPhoneNum, const wstring& strCode, const wstring& strCheckType) {
        ITask* pTask = GetFreeTaskByType(m_taskList, TaskGetUserCheckMobileCode);
        if (nullptr == pTask) {
            pTask = new CTaskUserCheckMobileCode(msg, strPhoneNum, strCode, strCheckType);
            m_taskList.push_back((UINT_PTR)pTask);
        }
        else
        {
            pTask->Clear();
            ((CTaskUserCheckMobileCode*)pTask)->Init(msg, strPhoneNum, strCode, strCheckType);
        }
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
        pTask->SetThread(hThread);
        return (UINT_PTR)pTask;
    }

    UINT_PTR CTaskCenter::CreateUserRegister(const MSG& msg, const wstring& strPhoneNumber, const wstring& strVerificationCode, const wstring& strActiveCode, const wstring& strPassword) {
        ITask* pTask = GetFreeTaskByType(m_taskList, TaskGetUserRegister);
        if (nullptr == pTask) {
            pTask = new CTaskUserRegister(msg, strPhoneNumber, strVerificationCode, strActiveCode, strPassword);
            m_taskList.push_back((UINT_PTR)pTask);
        }
        else
        {
            pTask->Clear();
            ((CTaskUserRegister*)pTask)->Init(msg, strPhoneNumber, strVerificationCode, strActiveCode, strPassword);
        }
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
        pTask->SetThread(hThread);
        return (UINT_PTR)pTask;
    }

    UINT_PTR CTaskCenter::CreateUserLoginTask(const MSG& msg, const wstring& strAccountId, const wstring& strPassword) {
        ITask* pTask = GetFreeTaskByType(m_taskList, TaskUserLogin);
        if (NULL == pTask)
        {
            pTask = new CTaskUserLogin(msg, strAccountId, strPassword);
            m_taskList.push_back((UINT_PTR)pTask);
        }
        else
        {
            pTask->Clear();
            ((CTaskUserLogin*)pTask)->Init(msg, strAccountId, strPassword);
        }
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
        pTask->SetThread(hThread);
        return (UINT_PTR)pTask;
    }

    UINT_PTR CTaskCenter::CreateUserCodeLoginTask(const MSG& msg, const wstring& strPhoneNum, const wstring& strCode) {
        ITask* pTask = GetFreeTaskByType(m_taskList, TaskGetUserCodeLogin);
        if (NULL == pTask)
        {
            pTask = new CTaskUserCodeLogin(msg, strPhoneNum, strCode);
            m_taskList.push_back((UINT_PTR)pTask);
        }
        else
        {
            pTask->Clear();
            ((CTaskUserCodeLogin*)pTask)->Init(msg, strPhoneNum, strCode);
        }
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
        pTask->SetThread(hThread);
        return (UINT_PTR)pTask;
    }

    UINT_PTR CTaskCenter::CreateUserModifyPassword(const MSG& msg, const wstring& strPhoneNumber, const wstring& strCode, const wstring& strPassword) {
        ITask* pTask = GetFreeTaskByType(m_taskList, TaskGetUserModifyPassword);
        if (nullptr == pTask) {
            pTask = new CTaskUserModifyPassword(msg, strPhoneNumber, strCode, strPassword);
            m_taskList.push_back((UINT_PTR)pTask);
        }
        else
        {
            pTask->Clear();
            ((CTaskUserModifyPassword*)pTask)->Init(msg, strPhoneNumber, strCode, strPassword);
        }
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
        pTask->SetThread(hThread);
        return (UINT_PTR)pTask;
    }

    UINT_PTR CTaskCenter::CreateUserInfoTask(const MSG& msg, int nUid, const string& strToken, bool bUpdateInfo/* = false*/) {
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskUserInfo);
		if (nullptr == pTask) {
			pTask = new CTaskUserInfo(msg, nUid, strToken, bUpdateInfo);
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else {
			pTask->Clear();
			((CTaskUserInfo*)pTask)->Init(msg, nUid, strToken, bUpdateInfo);
		}

		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	bool CTaskCenter::IsUpdateUserInfoTask(UINT_PTR nTaskID, OUT bool& bUpdateInfo) {
		TaskItor itor = std::find(m_taskList.begin(), m_taskList.end(), nTaskID);
		if (itor == m_taskList.end())
			return false;
		ITask* pTask = (ITask*)nTaskID;
		if (NULL == pTask || pTask->GetTaskType() != TaskUserInfo)
			return false;
		CTaskUserInfo* pTaskUser = dynamic_cast<CTaskUserInfo*>(pTask);
		if (NULL == pTaskUser)
			return false;
		bUpdateInfo = pTaskUser->IsUpdateInfo();
		return true;
	}

	bool CTaskCenter::GetUserInfoTaskParam(UINT_PTR nTaskID, OUT int& nUid, OUT string& strToken) {
		TaskItor itor = std::find(m_taskList.begin(), m_taskList.end(), nTaskID);
		if (itor == m_taskList.end())
			return false;
		ITask* pTask = (ITask*)nTaskID;
		if (NULL == pTask || pTask->GetTaskType() != TaskUserInfo)
			return false;
		CTaskUserInfo* pTaskUser = dynamic_cast<CTaskUserInfo*>(pTask);
		if (NULL == pTaskUser)
			return false;
		pTaskUser->GetParam(nUid, strToken);
		return true;
	}

	UINT_PTR CTaskCenter::CreateGetGameListTask(const MSG& msg, __int64 nID, int nIDType)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskGetGameList);
		if (NULL == pTask)
		{
			pTask = new CTaskGetGameList(msg, nIDType, nID);
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskGetGameList*)pTask)->Init(msg, nIDType, nID);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	UINT_PTR CTaskCenter::CreateUserIcoTask(const MSG& msg, const wstring& strUrl, const wstring& strUserPath)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskUserIco);
		if (NULL == pTask)
		{
			pTask = new CTaskUserIco(msg, strUrl, strUserPath);
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskUserIco*)pTask)->Init(msg, strUrl, strUserPath);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	bool CTaskCenter::GetUserIcoPath(UINT_PTR nTaskID, OUT wstring& strIcoPath, OUT wstring& strIcoUrl)
	{
		TaskItor itor = std::find(m_taskList.begin(), m_taskList.end(), nTaskID);
		if (itor == m_taskList.end())
			return false;
		ITask* pTask = (ITask*)nTaskID;
		if (NULL == pTask || pTask->GetTaskType() != TaskUserIco)
			return false;
		CTaskUserIco* pTaskUser = dynamic_cast<CTaskUserIco*>(pTask);
		if (NULL == pTaskUser)
			return false;
		strIcoPath = pTaskUser->GetIcoPath();
        strIcoUrl = pTaskUser->GetIcoUrl();
		return true;
	}

	UINT_PTR CTaskCenter::CreateUnzipGameTask(const MSG& msg, UINT uMsg, UINT_PTR nTaskID, const wstring& strZipFile, const wstring& strOutDir) {
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskUnzipGame);
		if (nullptr == pTask) {
			CTaskUnzipGame *pUnzipTask = new CTaskUnzipGame(msg, uMsg, nTaskID, strZipFile, strOutDir);
			pUnzipTask->SetNeedExit(&m_bNeedExit);
			pTask = pUnzipTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else {
			pTask->Clear();
			((CTaskUnzipGame*)pTask)->Init(msg, uMsg, nTaskID, strZipFile, strOutDir);
		}

		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, nullptr);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

    UINT_PTR CTaskCenter::CreateUnzipFileTask(const MSG& msg, UINT uMsg, const wstring& strZipFile, const wstring& strOutDir) {
        ITask* pTask = GetFreeTaskByType(m_taskList, TaskUnzipFile);
        if (nullptr == pTask) {
            CTaskUnzipFile *pUnzipTask = new CTaskUnzipFile(msg, uMsg, strZipFile, strOutDir);
            pUnzipTask->SetNeedExit(&m_bNeedExit);
            pTask = pUnzipTask;
            m_taskList.push_back((UINT_PTR)pTask);
        }
        else {
            pTask->Clear();
            ((CTaskUnzipFile*)pTask)->Init(msg, uMsg, strZipFile, strOutDir);
        }

        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, nullptr);
        pTask->SetThread(hThread);
        return (UINT_PTR)pTask;
    }

    UINT_PTR CTaskCenter::CreateIosEngineUpdateTask(const MSG& msg, UINT uMsg, const wstring& strVersion) {
        ITask* pTask = GetFreeTaskByType(m_taskList, TaskIosEngineUpdate);
        if (nullptr == pTask) {
            CTaskIosEngineUpdate *pIosEngineUpdateTask = new CTaskIosEngineUpdate(msg, uMsg, strVersion);
            pTask = pIosEngineUpdateTask;
            m_taskList.push_back((UINT_PTR)pTask);
        }
        else {
            pTask->Clear();
            ((CTaskIosEngineUpdate*)pTask)->Init(msg, uMsg, strVersion);
        }

        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, nullptr);
        pTask->SetThread(hThread);
        return (UINT_PTR)pTask;
    }

    UINT_PTR CTaskCenter::CreateBootRunTask(bool bBootRun)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskBootRun);
		if (NULL == pTask)
		{
			CTaskBootRun *pBootTask = new CTaskBootRun(bBootRun);
			pTask = pBootTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskBootRun*)pTask)->Init(bBootRun);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	UINT_PTR CTaskCenter::CreateRoomTask(const MSG& msg, int nUid, const string& strToken, __int64 nGameID)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskCreateRoom);
		if (NULL == pTask)
		{
			CTaskCreateRoom *pBootTask = new CTaskCreateRoom(msg, nUid, strToken, nGameID);
			pTask = pBootTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskCreateRoom*)pTask)->Init(msg, nUid, strToken, nGameID);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	UINT_PTR CTaskCenter::CreateDelDirTask(const list<wstring>& listDirs, const list<wstring>& listFiles)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskDelDir);
		if (NULL == pTask)
		{
			CTaskDelDir *pDelTask = new CTaskDelDir(listDirs, listFiles);
			pTask = pDelTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskDelDir*)pTask)->Init(listDirs, listFiles);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	UINT_PTR CTaskCenter::CreateGameTabListTask(const MSG& msg)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskGameTabList);
		if (NULL == pTask)
		{
			CTaskGameTabList *pDelTask = new CTaskGameTabList(msg);
			pTask = pDelTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskGameTabList*)pTask)->Init(msg);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	UINT_PTR CTaskCenter::CreateGameRecommandTask(const MSG& msg)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskGameRecommand);
		if (NULL == pTask)
		{
			CTaskGameRecommand *pDelTask = new CTaskGameRecommand(msg);
			pTask = pDelTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskGameRecommand*)pTask)->Init(msg);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	UINT_PTR CTaskCenter::CreateGameRecommandIcoTask(const MSG& msg, const wstring& strIcoUrl, const wstring& strSavePath)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskGameRecommandIco);
		if (NULL == pTask)
		{
			CTaskGameRecommandIco *pDelTask = new CTaskGameRecommandIco(msg, strIcoUrl, strSavePath);
			pTask = pDelTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskGameRecommandIco*)pTask)->Init(msg, strIcoUrl, strSavePath);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	UINT_PTR CTaskCenter::CreateDownloadFileTask(const MSG& msg, const string& strUrl, const wstring& strSavePath)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskDownloadFile);
		if (NULL == pTask)
		{
			CTaskDownloadFile *pLoadTask = new CTaskDownloadFile(msg, strUrl, strSavePath);
			pTask = pLoadTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskDownloadFile*)pTask)->Init(msg, strUrl, strSavePath);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	UINT_PTR CTaskCenter::CreateUrlTask(const MSG& msg, const wstring& strUrl)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskUrl);
		if (NULL == pTask)
		{
			CTaskUrl *pUrlTask = new CTaskUrl(msg, strUrl);
			pTask = pUrlTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskUrl*)pTask)->Init(msg, strUrl);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

    UINT_PTR CTaskCenter::CreateGetUpdateLogTask(const MSG& msg) {
        ITask* pTask = GetFreeTaskByType(m_taskList, TaskGetUpdateLog);
        if (NULL == pTask)
        {
            CTaskGetUpdateLog *pUrlTask = new CTaskGetUpdateLog(msg);
            pTask = pUrlTask;
            m_taskList.push_back((UINT_PTR)pTask);
        }
        else
        {
            pTask->Clear();
            ((CTaskGetUpdateLog*)pTask)->Init(msg);
        }
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
        pTask->SetThread(hThread);
        return (UINT_PTR)pTask;
    }

    UINT_PTR CTaskCenter::CreateCheckGameUpdateTask(const MSG& msg, const std::map<string, string>& gameList)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskCheckGameUpdate);
		if (NULL == pTask)
		{
			CTaskCheckGameUpdate *pUrlTask = new CTaskCheckGameUpdate(msg, gameList);
			pTask = pUrlTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskCheckGameUpdate*)pTask)->Init(msg, gameList);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	// 	UINT_PTR CTaskCenter::CreateUploadAvatarTask(const MSG& msg, int nUid, const string& strToken, const wstring& strImgPath)
// 	{
// 		ITask* pTask = GetFreeTaskByType(m_taskList, TaskUploadAvatar);
// 		if (NULL == pTask)
// 		{
// 			CTaskUploadAvatar *pUploadTask = new CTaskUploadAvatar(msg, nUid, strToken, strImgPath);
// 			pTask = pUploadTask;
// 			m_taskList.push_back((UINT_PTR)pTask);
// 		}
// 		else
// 		{
// 			pTask->Clear();
// 			((CTaskUploadAvatar*)pTask)->Init(msg, nUid, strToken, strImgPath);
// 		}
// 		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
// 		pTask->SetThread(hThread);
// 		return (UINT_PTR)pTask;
// 	}

	bool CTaskCenter::GetDownloadFileTaskPath(UINT_PTR nTaskID, OUT wstring& strPath)
	{
		TaskItor itor = std::find(m_taskList.begin(), m_taskList.end(), nTaskID);
		if (itor == m_taskList.end())
			return false;
		ITask* pTask = (ITask*)nTaskID;
		CTaskDownloadFile* pLoadTask = dynamic_cast<CTaskDownloadFile*>(pTask);
		if (NULL == pLoadTask)
			return false;
		strPath = pLoadTask->GetSavePath();
		return true;
	}

	UINT_PTR CTaskCenter::CreatePlayerIcoTask(const MSG& msg)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskPlayerIco);
		if (NULL == pTask)
		{
			CTaskPlayerIco *pPlayerTask = new CTaskPlayerIco(msg);
			pTask = pPlayerTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskPlayerIco*)pTask)->Init(msg);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	bool CTaskCenter::AddPlayerIco(UINT_PTR nTaskID, LPlayerIcoParam lpParam)
	{
		TaskItor itor = std::find(m_taskList.begin(), m_taskList.end(), nTaskID);
		if (itor == m_taskList.end())
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"没有找到对应的任务");
			return false;
		}
		ITask* pTask = (ITask*)nTaskID;
		if (NULL == pTask || pTask->GetTaskType() != TaskPlayerIco)
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"任务类型不正确");
			return false;
		}
		CTaskPlayerIco* pTaskPlayer = dynamic_cast<CTaskPlayerIco*>(pTask);
		if (NULL == pTaskPlayer)
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"任务指针转换失败");
			return false;
		}
		return pTaskPlayer->AddPlayerIco(lpParam);
	}

	bool CTaskCenter::StopPlayerIcoTask(UINT_PTR nTaskID)
	{
		TaskItor itor = std::find(m_taskList.begin(), m_taskList.end(), nTaskID);
		if (itor == m_taskList.end())
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"没有找到对应的任务");
			return false;
		}
		ITask* pTask = (ITask*)nTaskID;
		if (NULL == pTask || pTask->GetTaskType() != TaskPlayerIco)
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"任务类型不正确");
			return false;
		}
		CTaskPlayerIco* pTaskPlayer = dynamic_cast<CTaskPlayerIco*>(pTask);
		if (NULL == pTaskPlayer)
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"任务指针转换失败");
			return false;
		}
		pTaskPlayer->Stop();
		return true;
	}

	UINT_PTR CTaskCenter::CreateContestTask(const MSG& msg, int nUid, __int64 nGameID, const string& strToken, int nFlag)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskContestID);
		if (NULL == pTask)
		{
			CTaskContest *pContestTask = new CTaskContest(msg, nUid, nGameID, strToken, nFlag);
			pTask = pContestTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskContest*)pTask)->Init(msg, nUid, nGameID, strToken, nFlag);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	UINT_PTR CTaskCenter::CreateUploadRecordTask(const MSG& msg, const wstring& strPath, const string& strName)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskUploadRecord);
		if (NULL == pTask)
		{
			CTaskUploadRecord *pUploadTask = new CTaskUploadRecord(msg, strPath, strName);
			pTask = pUploadTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskUploadRecord*)pTask)->Init(msg, strPath, strName);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	bool CTaskCenter::GetUploadTaskPath(UINT_PTR nTaskID, OUT wstring& strPath)
	{
		TaskItor itor = std::find(m_taskList.begin(), m_taskList.end(), nTaskID);
		if (itor == m_taskList.end())
			return false;
		ITask* pTask = (ITask*)nTaskID;
		if (NULL == pTask || pTask->GetTaskType() != TaskUploadRecord)
			return false;
		CTaskUploadRecord* pTaskUpload = dynamic_cast<CTaskUploadRecord*>(pTask);
		if (NULL == pTaskUpload)
			return false;
		strPath = pTaskUpload->GetRecordPath();
		return true;
	}

	bool CTaskCenter::HasTaskRunning(TaskType type)
	{
		for (auto itor = m_taskList.begin(); itor != m_taskList.end(); ++itor)
		{
			ITask* pTask = (ITask*)(*itor);
			if (pTask->GetTaskType() == type && pTask->IsUsed())
				return true;
		}
		return false;
	}

// 	bool CTaskCenter::GetUploadAvatarTaskPath(UINT_PTR nTaskID, OUT wstring& strImgPath)
// 	{
// 		TaskItor itor = std::find(m_taskList.begin(), m_taskList.end(), nTaskID);
// 		if (itor == m_taskList.end())
// 			return false;
// 		ITask* pTask = (ITask*)nTaskID;
// 		if (NULL == pTask || pTask->GetTaskType() != TaskUploadAvatar)
// 			return false;
// 		CTaskUploadAvatar* pTaskUpload = dynamic_cast<CTaskUploadAvatar*>(pTask);
// 		if (NULL == pTaskUpload)
// 			return false;
// 		strImgPath = pTaskUpload->GetImgPath();
// 		return true;
// 	}

	UINT_PTR CTaskCenter::CreateIMTokenTask(const MSG& msg, const Json::Value& vUserInfo)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskIMCreateToken);
		if (NULL == pTask)
		{
			CTaskIMToken *pIMTask = new CTaskIMToken(msg, vUserInfo);
			pTask = pIMTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskIMToken*)pTask)->Init(msg, vUserInfo);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	UINT_PTR CTaskCenter::CreateIMUpdateInfoTask(const string& strJsonInfo)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskIMUpdateInfo);
		if (NULL == pTask)
		{
			CTaskIMUpdateInfo *pIMTask = new CTaskIMUpdateInfo(strJsonInfo);
			pTask = pIMTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskIMUpdateInfo*)pTask)->Init(strJsonInfo);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	UINT_PTR CTaskCenter::CreateIMCheckMsgTask(const MSG& msg, int nUid, const string& strToken, const string& strMsg)
	{
		ITask* pTask = GetFreeTaskByType(m_taskList, TaskIMCheckMsg);
		if (NULL == pTask)
		{
			CTaskIMCheckMsg *pIMTask = new CTaskIMCheckMsg(msg, nUid, strToken, strMsg);
			pTask = pIMTask;
			m_taskList.push_back((UINT_PTR)pTask);
		}
		else
		{
			pTask->Clear();
			((CTaskIMCheckMsg*)pTask)->Init(msg, nUid, strToken, strMsg);
		}
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread, pTask, 0, NULL);
		pTask->SetThread(hThread);
		return (UINT_PTR)pTask;
	}

	UINT __stdcall CTaskCenter::Thread(void* lpParam)
	{
		ITask* pTask = (ITask*)lpParam;
		pTask->Run();
		const MSG& msg = pTask->GetMsg();
		if (IsWindow(msg.hwnd))
			::PostMessage(msg.hwnd, msg.message, msg.wParam, (LPARAM)(UINT_PTR)pTask);
		else
			pTask->SetUse(FALSE);
		return 0;
	}
}