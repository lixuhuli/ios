#include "StdAfx.h"
#include "DownloadItem.h"
#include "GlobalData.h"
#include "MsgDefine.h"
#include "Download/Task.h"
#include "GameIconHelper.h"
#include "Download/DownloadMgr.h"

CDownloadItemUI::CDownloadItemUI()
 : type_(loading)
 , install_frame_(0)
 , label_game_icon_(nullptr) 
 , progress_percent_(nullptr)
 , btn_pause_(nullptr)
 , btn_update_(nullptr)
 , btn_delete_(nullptr)
 , label_game_discribe_(nullptr) {
}

CDownloadItemUI::~CDownloadItemUI() {
}

void CDownloadItemUI::InitControl() {
    InitControls();
    BindControls();
}

void CDownloadItemUI::Init() {
    __super::Init();
}

void CDownloadItemUI::DoEvent(TEventUI& event) {
    switch (event.Type) {
    case UIEVENT_TIMER:
        OnTimer(event.wParam);
        break;
    default:
        break;
    }
    __super::DoEvent(event);
}

void CDownloadItemUI::SetPos(RECT rc) {
    __super::SetPos(rc);
}

void CDownloadItemUI::InitItemState(const ITask *task) {

}

void CDownloadItemUI::UpdateTaskInfo(UINT_PTR nTask) {
    ITask* task = (ITask*)nTask;
    if (!task) return;

    AddDownloadItem(task);
}

bool CDownloadItemUI::AddDownloadItem(const ITask *task) {
    if (!task) return false;

    auto icon_file_name = GetGameIcon130File(task->strName, task->strVersion);
    if (PathFileExists(icon_file_name.c_str())) UpdateGameIcon(task->strName, task->strVersion);
    else CDownloadMgr::Instance()->DownloadIcon(task->nGameID, task->strName);

    if (type_ == loading && progress_percent_) progress_percent_->SetVisible(true);
    if (type_ == history && btn_update_) btn_update_->SetVisible(true);

    if (label_game_discribe_) {
        label_game_discribe_->SetText(task->strName.c_str());
        label_game_discribe_->SetToolTip(task->strName.c_str());

        auto sz = GetPaintTextSize(m_pManager, task->strName.c_str(), label_game_discribe_->GetFont());
        if (sz.cx  > 130) label_game_discribe_->SetFixedHeight(label_game_discribe_->GetTag());
    }

    return true;
}

void CDownloadItemUI::ChangeItemLoadState(UINT_PTR nTask, TaskState state, __int64 nTotalSize, __int64 nLoadSize, int nSpeed, UINT_PTR nData) {
    if (!label_game_discribe_  || !progress_percent_ || !btn_pause_) return;

    CDuiString strStateText;

    switch (state) {
    case Ts_Loading: {
        // 正在下载
        strStateText = L"正在载入...";
        
        if (nTotalSize <= 0) progress_percent_->SetBkImage(L"file='download/bk.png'");
        else {
            CDuiString strPercent = progress_percent_->GetUserData();
            if (nLoadSize < nTotalSize) {
                int count = (int)((double)nLoadSize * (double)228 / (double)nTotalSize);
                if (count < 1) count = 1;
                strPercent.Format(L"%s file='download/loading/%.3d.png'", progress_percent_->GetUserData().GetData(), count);
            }

            progress_percent_->SetBkImage(strPercent.GetData());
        }
    }
    break;
    case Ts_Error: {
        strStateText = nData == 100 ? L"安装失败" : L"下载失败";
        m_pManager->KillTimer(this, TIMER_ID_INSTALLING);
    }
    break;
    case Ts_Pause: {
        CDuiString strPercent = progress_percent_->GetUserData();
        strPercent.Format(L"%s file='download/pause.png' dest='29,29,101,101'", progress_percent_->GetUserData().GetData());
        progress_percent_->SetBkImage(strPercent.GetData());
        btn_pause_->SetTag(1);
        strStateText = L"暂停下载...";
    }
    break;
    case Ts_Wait: {
        strStateText = L"等待下载...";
    }
    break;
    case Ts_Check: {
        strStateText = L"等待下载...";
    }
    break;
    default: break;
    }

    label_game_discribe_->SetText(strStateText);
    label_game_discribe_->SetTextColor(state == Ts_Loading ? 0xFFF57519 : 0xFF404040);
}

void CDownloadItemUI::OnItemInstall(UINT_PTR nTask) {
    if (!m_pManager) return;

    if (btn_pause_) btn_pause_->SetEnabled(false);
    if (btn_delete_) btn_delete_->SetVisible(false);
    m_pManager->SetTimer(this, TIMER_ID_INSTALLING, 100);
    install_frame_ = 228;

    CDuiString strStateText = L"正在安装...";
    CDuiString strPercent;
    strPercent.Format(L"%s file='download/loading/228.png'", progress_percent_->GetUserData().GetData());
    progress_percent_->SetBkImage(strPercent.GetData());
    label_game_discribe_->SetText(strStateText);
    label_game_discribe_->SetTextColor(0xFF404040);
}

void CDownloadItemUI::OnItemFinish(UINT_PTR nTask) {
    if (!m_pManager) return;

    m_pManager->KillTimer(this, TIMER_ID_INSTALLING);
}

void CDownloadItemUI::UpdateUninstallBtnStatus(bool show) {
    if (btn_pause_ && !btn_pause_->IsEnabled()) show = false;
    if (btn_delete_) btn_delete_->SetVisible(show);
}

void CDownloadItemUI::UpdateGameIcon(const wstring& strName, const wstring& strVersion) {
    if (!label_game_icon_) return;

    auto icon_file_name = GetGameIcon130File(strName, strVersion);
    if (PathFileExists(icon_file_name.c_str())) {
        CStdString icon_file;
        icon_file.Format(L"file='%s'", icon_file_name.c_str());
        label_game_icon_->SetBkImage(icon_file_name.c_str());
    }
}

void CDownloadItemUI::OnTimer(int nTimerID) {
    if (nTimerID == TIMER_ID_INSTALLING) {
        OnInstallingIpa();
    }
}

bool CDownloadItemUI::OnClickBtnPause(void* param) {
    if (!btn_pause_) return true;

    if (btn_delete_ && btn_delete_->IsVisible()) return true;

    if (btn_pause_->GetTag() == 1) {
        CDownloadMgr::Instance()->ReloadTask(GetTag());
        btn_pause_->SetTag(0);
    }
    else CDownloadMgr::Instance()->PauseTask(GetTag());

    return true;
}

void CDownloadItemUI::OnInstallingIpa() {
    install_frame_++;

    CDuiString strStateText = L"正在安装...";
    CDuiString strPercent;
    strPercent.Format(L"%s file='download/loading/%.3d.png'", progress_percent_->GetUserData().GetData(), install_frame_);
    progress_percent_->SetBkImage(strPercent.GetData());
    label_game_discribe_->SetText(strStateText);
    label_game_discribe_->SetTextColor(0xFF404040);

    if (install_frame_ >= (int)progress_percent_->GetTag()) {
        m_pManager->KillTimer(this, TIMER_ID_INSTALLING);
    }
}

