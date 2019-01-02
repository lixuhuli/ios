#include "StdAfx.h"
#include "DownloadMgr.h"
#include "MsgDefine.h"
#include "Database/DatabaseMgr.h"
#include "TaskCenter/TaskCenter.h"
#include "DataPost.h"
//#include "CommonWnd.h"
#include "ParamDefine.h"
#include "../CallBack/callback_mgr.h"
#include "Ios/IosMgr.h"
#include "CallBack/callback_mgr.h"

using namespace PublicLib;

enum{
	TIMER_ID_DLWND_BASE = 600,
	TIMER_ID_DLWND_RUN,
};

#define ZIP_FFIX   ".zip"
#define APK_FFIX   ".apk"
#define EXE_FFIX   ".exe"


volatile BOOL CDownloadMgr::m_bNeedExit = 0;
CDownloadMgr::CDownloadMgr()
: m_nMaxLoadCount(5)
, m_nMaxLoadSpeed(0)
, m_hMsgWnd(nullptr)
, can_play_ready_(false) {
}


CDownloadMgr::~CDownloadMgr() {
}

bool CDownloadMgr::Init(HINSTANCE hInst, size_t nMaxTaskCount, size_t nMaxLoadSpeed)
{
	if (nMaxTaskCount < 1)
		nMaxTaskCount = 6;
	m_nMaxLoadCount = nMaxTaskCount;
	if (nMaxLoadSpeed < 0)
		nMaxLoadSpeed = 0;
	m_nMaxLoadSpeed = nMaxLoadSpeed;
	HWND hWnd = CreateMessageWindow(hInst);
	if (NULL == hWnd)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"初始化下载中心失败，错误：创建消息处理窗口失败！");
		return false;
	}
	m_hMsgWnd = hWnd;
	LoadDbTask();
	::SetTimer(m_hMsgWnd, TIMER_ID_DLWND_RUN, 1 * 1000, NULL);

	return true;
}

void CDownloadMgr::LoadDbTask()
{
	vector<ITask*> taskList;
	size_t i = 0;
	if (CDatabaseMgr::Instance()->GetLoadTasks(taskList))
	{
		for (; i < taskList.size(); ++i)
		{
			m_taskList.AddLoadTask(taskList[i]);
		}
	}
	if (CDatabaseMgr::Instance()->GetFinishTasks(taskList))
	{
		for (i = 0; i < taskList.size(); ++i)
		{
			m_taskList.AddFinishTask(taskList[i]);
		}
	}
}

void CDownloadMgr::Exit()
{
	m_bNeedExit = TRUE;
	DetachCallback();
	m_taskList.Release();
    list_icon_.clear();
	OUTPUT_XYLOG(LEVEL_INFO, L"下载中心退出");
}

UINT_PTR CDownloadMgr::DownloadIcon(const __int64& nGameID, const wstring& strName) {
    TaskMapItor it = list_icon_.find(nGameID);
    if (it != list_icon_.end()) return (UINT_PTR)it->second;

    if (!CDatabaseMgr::Instance()->CheckFreeSpace()) {
        ::PostMessage(CGlobalData::Instance()->GetMainWnd(), WM_MAINWND_MSG_COMMON, WpCommonDiskNoSpace, 0);
        return 0;
    }

    ITask* pTask = new ITask;
    if (!pTask) return 0;

    pTask->nGameID = nGameID;
    pTask->strName = strName;
    list_icon_.insert(std::make_pair(pTask->nGameID, pTask));
    CheckIconUrl(pTask, Tht_Icon);

    return (UINT_PTR)pTask;
}

UINT_PTR CDownloadMgr::AddTask(const Tag_HelperThread& tag, __int64 nGameID, const wstring& strName, int nLoadWay/* = 1*/) {
	ITask* pOldTask = m_taskList.GetTaskByID(nGameID);
	if (pOldTask) {
		if (pOldTask->state == Ts_Pause || pOldTask->state == Ts_Error){   
            // 继续下载
			if (pOldTask->strUrl.empty()) {
				CheckUrl(pOldTask, tag);
			}
			else PostMessage(m_hMsgWnd, WM_DLWND_MSG_TASK_TRY_START, 0, (LPARAM)pOldTask);
		}

		OUTPUT_XYLOG(LEVEL_INFO, L"任务已经存在");
		return (UINT_PTR)pOldTask;
	}

	if (!CDatabaseMgr::Instance()->CheckFreeSpace()) {
		::PostMessage(CGlobalData::Instance()->GetMainWnd(), WM_MAINWND_MSG_COMMON, WpCommonDiskNoSpace, 0);
		return NULL;
	}

	ITask* pTask = new ITask;
	pTask->nGameID = nGameID;
	pTask->strName = strName;
    pTask->nLoadWay = nLoadWay;

    switch (tag) {
    case Tht_Unkonw:
    case Tht_Url: pTask->type = Ft_Game; break;
    case Tht_Icon: pTask->type = Ft_Icon; break;
    default:
        break;
    }

	NotifyCallback(pTask);
	m_taskList.AddLoadTask(pTask);

    CheckUrl(pTask, tag);

	return (UINT_PTR)pTask;
}

void CDownloadMgr::DeleteLoadTask(UINT_PTR nTaskID, BOOL bDeleteDb, BOOL bUpdate)
{
	ITask* pTask = (ITask*)nTaskID;
	if (!m_taskList.IsLoadTaskExist(pTask))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"删除任务失败，任务对象ID不存在");
		return;
	}
	if (pTask->pHttp)
	{
		pTask->pHttp->NeedStop();
		m_httpCache.Add(pTask->pHttp);
		pTask->pHttp = NULL;
	}
	if (bDeleteDb)
		CDatabaseMgr::Instance()->DeleteLoadTask(pTask);
	m_taskList.DeleteLoadTask(pTask);
	NotifyCallback(pTask);
	if (bUpdate)
		UpdateWaitList();
	wstring strTempFile = pTask->strSavePath + L"_tmp";
	if (PathFileExists(strTempFile.c_str()))
		DeleteFile(strTempFile.c_str());
	if (PathFileExists(pTask->strSavePath.c_str()))
		DeleteFile(pTask->strSavePath.c_str());
}

void CDownloadMgr::DeleteFinishTask(UINT_PTR nTaskID, BOOL bDeleteDb)
{
	ITask* pTask = (ITask*)nTaskID;
	if (!m_taskList.IsFinishTaskExist(pTask))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"任务不存在");
		return;
	}
	if (bDeleteDb)
		CDatabaseMgr::Instance()->DeleteFinishTask(pTask);
// 	UpdateLoadingCount();
	m_taskList.DeleteFinishTask(pTask);
}

bool CDownloadMgr::PauseTask(UINT_PTR nTaskID, BOOL bUpdateWaitList/* = TRUE*/)
{
	ITask* pTask = (ITask*)nTaskID;
	if (!m_taskList.IsLoadTaskExist(pTask))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"任务不存在");
		return false;
	}
	if (pTask->state == Ts_Loading)
	{//暂停下载
		if (pTask->pHttp)
		{
			pTask->pHttp->Stop();
			delete pTask->pHttp;
			//m_httpCache.Add(pTask->pHttp);
			pTask->pHttp = NULL;
		}
		pTask->state = Ts_Pause;
		if (bUpdateWaitList)
			UpdateWaitList();
	}
    PostDownloadGame(pTask->nGameID, pTask->gameType, StatusPause, pTask->nLoadWay, pTask->nSpeed);
	pTask->state = Ts_Pause;
	NotifyCallback(pTask);
	return true;
}

bool CDownloadMgr::ReloadTask(UINT_PTR nTaskID)
{
	ITask* pTask = (ITask*)nTaskID;
	if (!m_taskList.IsLoadTaskExist(pTask))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"任务不存在");
		return false;
	}
	if (pTask->strUrl.empty())
	{
		pTask->state = Ts_Error;
		NotifyCallback(pTask);
		return false;
	}
	if (pTask->state == Ts_Pause || pTask->state == Ts_Error)
	{
		TryStartTask(pTask);
		return true;
	}
	return false;
}

bool CDownloadMgr::PauseWaitTask(UINT_PTR nTaskID)
{
	ITask* pTask = (ITask*)nTaskID;
	if (!m_taskList.IsLoadTaskExist(pTask))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"任务不存在");
		return false;
	}
	if (pTask->state != Ts_Wait)
		return false;
	pTask->state = Ts_Pause;
	NotifyCallback(pTask);
	return true;
}

bool CDownloadMgr::TryStartTask(ITask* pTask)
{
    // 文件存在，删除并重新下载
    if (::PathFileExists(pTask->strSavePath.c_str())) ::DeleteFile(pTask->strSavePath.c_str());

	//if (PathFileExists(pTask->strSavePath.c_str())) {
 //       // 文件存在，直接安装
	//	pTask->nTotalSize = PublicLib::GetFileSizeW(pTask->strSavePath);
	//	::PostMessage(m_hMsgWnd, WM_DLWND_MSG_LOAD_FINISH, 0, (LPARAM)pTask);
	//	return true;
	//}

	if (pTask->bShowOnLoadWnd) {
		size_t nLoad = m_taskList.GetLoadCount();
		if (nLoad < m_nMaxLoadCount) {   
            // 直接开始下载
			StartTask(pTask);
		}
		else {
            // 等待下载
			pTask->state = Ts_Wait;
			NotifyCallback(pTask);
		}
	}
	else {
        // 直接下载
		StartTask(pTask);
	}

	return false;
}

void CDownloadMgr::InstallTask(ITask* pTask) {
	pTask->state = Ts_Install;
    CIosMgr::Instance()->InstallApp((UINT_PTR)pTask);
    PostInstallGame(pTask->nGameID, StatusBegin, CUserData::Instance()->GetFileUserID());
	NotifyCallback(pTask);
}

void CDownloadMgr::FinishTask(ITask* pTask)
{
	pTask->state = Ts_Finish;
	// 移除下载列表，加入完成列表
	NotifyCallback(pTask);
	// 加入安装列表
	CDatabaseMgr::Instance()->InsertGameInfo(pTask);
	CDatabaseMgr::Instance()->DeleteLoadTask(pTask);
	if (m_taskList.IsTaskNameInFinishList(pTask))
	{
		CDatabaseMgr::Instance()->DeleteFinishTask(pTask);
	}
	CDatabaseMgr::Instance()->InsertFinishTask(pTask);
	m_taskList.FinishTask(pTask); 
	//::PostMessage(CGlobalData::Instance()->GetMainWnd(), WM_MAINWND_MSG_PAGE_LOCALGAMES, WpLGAddGame, (LPARAM)pTask);
	::PostMessage(CGlobalData::Instance()->GetMainWnd(), WM_MAINWND_MSG_GAME_CHANGED, WpLGAddGame, (LPARAM)pTask);
	UpdateWaitList();
}

bool CDownloadMgr::StartTask(ITask* pTask)
{
	PostDownloadGame(pTask->nGameID, pTask->gameType, StatusBegin, pTask->nLoadWay);
	bool bRet = false;
	CHttpDownload * pHttp = NULL;
	while (true)
	{
		string strUrl = /*PublicLib::UtfToA(*/pTask->strUrl/*)*/;
		wstring strSavePath = pTask->strSavePath;
		if (strUrl.empty() || strSavePath.empty())
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"下载地址为空");
			break;
		}
		int nFind = strSavePath.rfind('\\');
		if (nFind == string::npos)
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"保存路径不正确");
			break;
		}
		wstring strDir = strSavePath.substr(0, nFind);
		SHCreateDirectory(NULL, strDir.c_str());
		pHttp = new CHttpDownload();
		if (!pHttp->Initialize(strUrl, strSavePath, TRUE, pTask, LoadCallback, m_nMaxLoadSpeed))
		{//初始化下载失败
			OUTPUT_XYLOG(LEVEL_ERROR, L"初始化下载任务失败，下载地址：%s，保存路径：%s", AToU(pTask->strUrl).c_str(), pTask->strSavePath.c_str());
			break;
		}
		if (!pHttp->Start())
		{//开始下载失败
			OUTPUT_XYLOG(LEVEL_ERROR, L"开始下载任务失败，下载地址：%s，保存路径：%s", AToU(pTask->strUrl).c_str(), pTask->strSavePath.c_str());
			break;
		}
		bRet = true;
		break;
	}
	if (!bRet)
	{//失败
		if (pHttp)
		{
			delete pHttp;
			OUTPUT_XYLOG(LEVEL_INFO, L"删除下载对象");
		}
		OnMsgLoadError(0, (LPARAM)pTask);
		return false;
	}
	pTask->pHttp = pHttp;
	pTask->state = Ts_Loading;
	NotifyCallback(pTask);
	return true;
}

void CDownloadMgr::UpdateWaitList()
{
	while (!m_bNeedExit)
	{
		size_t nLoad = m_taskList.GetLoadCount();
		if (nLoad == m_nMaxLoadCount)
			break;
		if (nLoad > m_nMaxLoadCount)
		{
			ITask* pTask = m_taskList.UpdateLoadList();
			if (NULL == pTask)
				break;
			if (pTask->pHttp)
			{
				pTask->pHttp->NeedStop();
				m_httpCache.Add(pTask->pHttp);
				pTask->pHttp = NULL;
			}
			pTask->state = Ts_Wait;
			NotifyCallback(pTask);
			continue;
		}
		ITask* pTask = m_taskList.UpdateWaitList();
		if (NULL == pTask)
			break;
		StartTask(pTask);
	}
}

int CDownloadMgr::GetGameStatus(__int64 nGameID, const wstring& strVersion) {
    ITask* pTask = m_taskList.GetTaskByID(nGameID);
    if (pTask) {
        return GetGameStatus(pTask);
    }

	pTask = CDatabaseMgr::Instance()->GetGameInfo(nGameID);
	if (pTask) {
		if (strVersion == pTask->strVersion)
			return GameInstalled;
		return  GameUpdate;
	}

	return GameUnload;
}

bool CDownloadMgr::ReInstallFinishTask(UINT_PTR nTaskID) {
	ITask* pTask = (ITask*)nTaskID;
    if (!pTask) return false;

	if (!m_taskList.IsFinishTaskExist(pTask)) {
		OUTPUT_XYLOG(LEVEL_ERROR, L"不存在的任务ID");
		return false;
	}

	if (PathFileExists(pTask->strSavePath.c_str()))
		DeleteFile(pTask->strSavePath.c_str());

	::PostMessage(CGlobalData::Instance()->GetMainWnd(), WM_MAINWND_MSG_PAGE_LOCALGAMES, WpLGDeleteGame, (LPARAM)pTask);

	AddTask((Tag_HelperThread)pTask->GetTag(), pTask->nGameID, pTask->strName);

	CDatabaseMgr::Instance()->DeleteFinishTask(pTask);

	m_taskList.DeleteFinishTask(pTask);

	return true;
}

void CDownloadMgr::LoadCallback(PublicLib::EnumDownloadState state, double dltotal, double dlnow, void * Userdata)
{
	if (m_bNeedExit || NULL == Userdata)
		return ;
	ITask* pTask = (ITask*)Userdata;
	switch( state )
	{
	case STATE_DOWNLOADING://下载中
		{
			if (dltotal<1)
				return ;
			if (pTask->nTotalSize == 0)
				pTask->nTotalSize = (__int64)dltotal;
			__int64 nAdd = int(dlnow - pTask->nLoadSize);
			pTask->nLoadSize = (__int64)dlnow;
			__int64 nCurTime = (__int64)time(NULL);
			if (pTask->nLastTime == 0 || nCurTime == pTask->nLastTime)
			{
				pTask->nLastTime = nCurTime;
				pTask->nSize += nAdd;
				return ;
			}
			CDownloadMgr::Instance()->PostProgressMsg(pTask);
			if (!pTask->bShowOnLoadWnd)
			{
				return;
			}
			//计算下载速度
			pTask->nSpeed = (int)((pTask->nSize / (nCurTime - pTask->nLastTime)) / 1024);
			pTask->nSize = 0;
			pTask->nLastTime = nCurTime;
			CDatabaseMgr::Instance()->UpdateLoadTask(pTask);
			if (m_bNeedExit)
				return;
		}
		break;
	case STATE_DOWNLOAD_HAS_FINISHED://下载完成
		{
			pTask->nDate = time(NULL);
			//pTask->state = Ts_WaitInst;
			//CDatabaseMgr::Instance()->UpdateLoadTask(pTask);
			if (m_bNeedExit)
				return;
			CDownloadMgr::Instance()->PostFinishMsg(pTask);
// 			if (pTask->bShowOnLoadWnd)
// 				::PostMessage(CDownloadMgr::Instance()->m_hMsgWnd, WM_DLWND_MSG_UPDATE_LOADED, 0, 0);
		}
		break;
	case STATE_DOWNLOAD_HAS_FAILED://下载失败
		{
			CDownloadMgr::Instance()->PostErrorMsg(pTask);
			if (m_bNeedExit)
				return;
			pTask->state = Ts_Error;
			CDatabaseMgr::Instance()->UpdateLoadTask(pTask);
			if (m_bNeedExit)
				return;
// 			if (pTask->bShowOnLoadWnd)
// 				::PostMessage(CDownloadMgr::Instance()->m_hMsgWnd, WM_DLWND_MSG_UPDATE_LOADED, 0, 0);
			OUTPUT_XYLOG(LEVEL_ERROR, L"任务下载失败，下载地址：%s", AToU(pTask->strUrl).c_str());
		}
		break;
	}
}

LRESULT CDownloadMgr::OnMsgTryStartTask(WPARAM wParam, LPARAM lParam)
{
	ITask* pTask = (ITask*)lParam;
	if (!m_taskList.IsLoadTaskExist(pTask))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"任务不存在");
		return 0;
	}
	TryStartTask(pTask);
	return 0;
}

LRESULT CDownloadMgr::OnMsgWebHelper(WPARAM wParam, LPARAM lParam) {
	CWebHelperThread* pThread = (CWebHelperThread*)wParam;
	if (!pThread) {
		OUTPUT_XYLOG(LEVEL_ERROR, L"参数错误，CWebHelperThread指针为空");
		return 0;
	}

	BOOL bRet = (BOOL)lParam;
	Tag_HelperThread tag = pThread->GetTag();
	ITask* pTask = pThread->GetTask();
	if (!pTask) {
		OUTPUT_XYLOG(LEVEL_ERROR, L"参数错误，ITask指针为空");
		return 0;
	}

    m_webThMgr.Delete(pThread);

    if (pTask->type == Ft_Icon) {
        OUTPUT_XYLOG(LEVEL_INFO, L"游戏图标下载完成，不做处理");

        TaskMapItor it = list_icon_.find(pTask->nGameID);
        if (it != list_icon_.end()) list_icon_.erase(it);

        auto observers = CCallBackMgr::Instance()->Observers();
        if (observers) observers->Go(&AppleSeedCallback::UpdateGameIcon, pTask->nGameID, pTask->strName, pTask->strVersion);
        return 0;
    }

	if (pTask->state == Ts_Delete) {
        // 任务已经被删除
		OUTPUT_XYLOG(LEVEL_INFO, L"任务已经被删除，不做处理");
		return 0;
	}

	if (bRet) {
		// 填充savepath
        auto pos = pTask->strUrl.rfind(".");
        if (pos == wstring::npos) {
            pTask->state = Ts_Remove;
            NotifyCallback(pTask, 100);
            return 0;
        }

        auto file_ffix = PublicLib::AToU(pTask->strUrl.substr(pos, pTask->strUrl.length() - pos));

        CDuiString save_path;
        save_path.Format(L"%I64d", pTask->nGameID);
        pTask->strSavePath = CDatabaseMgr::Instance()->GetLoadPath() + L"\\" + save_path.GetData() + pTask->strVersion + file_ffix;
		pTask->state = Ts_CheckEnd;
		NotifyCallback(pTask);
		//加入数据库
		pTask->state = Ts_Wait;
		if (tag == Tht_Url)
			CDatabaseMgr::Instance()->InsertLoadTask(pTask);
		TryStartTask(pTask);
	}
	else {
        // 获取url失败
		pTask->state = Ts_Error;
		NotifyCallback(pTask);
	}

	return 0;
}

LRESULT CDownloadMgr::OnMsgIpaInstalling(WPARAM wParam, LPARAM lParam)
{
	ITask* pTask = (ITask*)wParam;
	NotifyCallback(pTask, lParam);
	return 0;
}

LRESULT CDownloadMgr::OnMsgIpaInstall(WPARAM wParam, LPARAM lParam) {
	ITask* pTask = (ITask*)wParam;
    if (!pTask) return 0;

	if (lParam == 0) {
        // 安装完成
		FinishTask(pTask);
        ::DeleteFile(pTask->strSavePath.c_str());
        PostInstallGame(pTask->nGameID, StatusSuccess, CUserData::Instance()->GetFileUserID());
	}
	else {   
        // 安装失败
		pTask->state = Ts_Error;
		NotifyCallback(pTask, 100);
        PostInstallGame(pTask->nGameID, StatusFail, CUserData::Instance()->GetFileUserID());
	}

	return 0;
}

LRESULT CDownloadMgr::OnMsgInstallApking(WPARAM wParam, LPARAM lParam) {
    ITask* pTask = (ITask*)wParam;
    NotifyCallback(pTask, lParam);
    return 0;
}

LRESULT CDownloadMgr::OnMsgInstallApk(WPARAM wParam, LPARAM lParam) {
    ITask* pTask = (ITask*)wParam;
    if (!pTask) return 0;

    bool success = (lParam == 1);
    if (success)
    {
        // 安装完成
        FinishTask(pTask);
        // 		CDuiString str;
        // 		str.Format(L"%s已下载完成，请重新操作", pTask->strName.c_str());
        // 		ShowBottomBar(str);
        //PostUnzipGame(pTask->nGameID, pTask->gameType, StatusSuccess, lpParam->nSpeed);
        //CUserData::Instance()->AddGameLoadCount();
        ::DeleteFile(pTask->strSavePath.c_str());
    }
    else {
        // 安装失败
        pTask->state = Ts_Error;
        NotifyCallback(pTask, 100);
        //PostUnzipGame(pTask->nGameID, pTask->gameType, StatusFail, lpParam->nSpeed);
    }

    return 0;
}

LRESULT CDownloadMgr::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if (wParam == TIMER_ID_DLWND_RUN)
	{
		::KillTimer(m_hMsgWnd, TIMER_ID_DLWND_RUN);
		const TaskList& loadList = m_taskList.GetLoadList();
		for (auto itor = loadList.begin(); itor != loadList.end();++itor)
		{
			ITask* pTask = *itor;
            NotifyCallback(pTask);
		}
	}
	return 0;
}

HWND CDownloadMgr::CreateMessageWindow(HINSTANCE hInstance)
{
	static const wchar_t kWndClass[] = L"ClientMessageWindow";
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = MessageWndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = kWndClass;
	RegisterClassEx(&wc);
	return CreateWindow(kWndClass, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, hInstance, 0);
}

LRESULT CDownloadMgr::MessageWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message > WM_DLWND_MSG_BEGIN)
	{
		if (m_bNeedExit)
			return 0;
		switch (message)
		{
		case WM_DLWND_MSG_LOAD_PROGRESS: CDownloadMgr::Instance()->OnMsgLoadProgress(wParam, lParam); break;
		case WM_DLWND_MSG_LOAD_FINISH: CDownloadMgr::Instance()->OnMsgLoadFinish(wParam, lParam); break;
		case WM_DLWND_MSG_LOAD_ERROR: CDownloadMgr::Instance()->OnMsgLoadError(wParam, lParam); break;
		case WM_DLWND_MSG_TASK_TRY_START: CDownloadMgr::Instance()->OnMsgTryStartTask(wParam, lParam); break;
		case WM_DLWND_MSG_WEBHELPER: CDownloadMgr::Instance()->OnMsgWebHelper(wParam, lParam); break;
		case WM_DLWND_MSG_IPA_INSTALLING: CDownloadMgr::Instance()->OnMsgIpaInstalling(wParam, lParam); break;
		case WM_DLWND_MSG_IPA_INSTALL: CDownloadMgr::Instance()->OnMsgIpaInstall(wParam, lParam); break;
        case WM_DLWND_MSG_INSTALL_APKING: CDownloadMgr::Instance()->OnMsgInstallApking(wParam, lParam); break;
        case WM_DLWND_MSG_INSTALL_APK: CDownloadMgr::Instance()->OnMsgInstallApk(wParam, lParam); break;
		default: break;
		}
		return 0;
	}
	if (message == WM_TIMER)
		CDownloadMgr::Instance()->OnTimer(wParam, lParam);
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CDownloadMgr::OnMsgLoadFinish(WPARAM wParam, LPARAM lParam)
{
	ITask* pTask = (ITask*)lParam;
	if (pTask->pHttp)
	{//线程已退出直接释放
		delete pTask->pHttp;
		pTask->pHttp = NULL;
	}
	PostDownloadGame(pTask->nGameID, pTask->gameType, StatusSuccess, pTask->nLoadWay, pTask->nSpeed);

    if (pTask->type == Ft_Game){
        // 安装安卓游戏
        InstallTask(pTask);
    }

	return 0;
}

LRESULT CDownloadMgr::OnMsgLoadError(WPARAM wParam, LPARAM lParam)
{
	ITask* pTask = (ITask*)lParam;
	if (pTask->pHttp)
	{//线程已退出直接释放
		delete pTask->pHttp;
		pTask->pHttp = NULL;
	}
	pTask->state = Ts_Error;
	NotifyCallback(pTask);
	UpdateWaitList();
	PostDownloadGame(pTask->nGameID, pTask->gameType, StatusFail, pTask->nLoadWay, pTask->nSpeed);
	return 0;
}

void CDownloadMgr::NotifyCallback(ITask* pTask, UINT_PTR nData/* = 0*/)
{
	if (m_bNeedExit)
		return;
	if (m_pLoadCallback && IsWindow((HWND)m_lpParam))
	{
		m_pLoadCallback->LoadCallback((UINT_PTR)pTask, m_lpParam, pTask->state, nData);
	}
	pTask->cbList.NotifyCallback((UINT_PTR)pTask, pTask->state, nData);
	int nStatus = GetGameStatus(pTask);
	SetWebGameStatus(pTask->nGameID, nStatus);
}

int CDownloadMgr::GetGameStatus(ITask* pTask) {
    int nStatus = GameUnload;
    if (!pTask) return nStatus;

    switch (pTask->state)
    {
    case Ts_Error:
    case Ts_Remove:
    case Ts_Delete:
        nStatus = GameUnload; break;
    case Ts_Finish: nStatus = GameInstalled; break;
    case Ts_Loading:
    case Ts_Install:
    case Ts_Check:
    case Ts_Pause:
    case Ts_Wait:
        nStatus = GameLoading;
        break;
    default:
        break;
    }

    return nStatus;
}

