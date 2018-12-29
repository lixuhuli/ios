#include "StdAfx.h"
#include "PageDownload.h"
#include "GlobalData.h"
#include "PageLayout.h"
#include "Download/DownloadMgr.h"
#include "CallBack/callback_mgr.h"
#include "Ios/IosMgr.h"

CPageDownloadUI::CPageDownloadUI(CControlUI* root)
 : root_(dynamic_cast<CVerticalLayoutUI*>(root))
 , layout_page_(nullptr)
 , layout_page_game_(nullptr) {
     CCallBackMgr::Instance()->AddObserver(AppleSeedCallback::BasePtr());
}

CPageDownloadUI::~CPageDownloadUI() {
    CCallBackMgr::Instance()->RemoveObserver(AppleSeedCallback::BasePtr());
}

void CPageDownloadUI::Init() {
    if (!root_) return;

    auto paint_manager = root_->GetManager();
    if (!paint_manager) return;

    CDialogBuilder builder;
    auto page_game_root = (CTabLayoutUI*)builder.Create(L"PageDownload.xml", 0, this, paint_manager);
    if (!page_game_root) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"创建游戏页签错误");
        return;
    }

    if (page_game_root) root_->Add(page_game_root);
    layout_page_ = page_game_root;

    InitControls();
    BindControls();

    DECLARE_CTRL_TYPE_PAGE(layout_page_game_, CPageLayoutUI, root_, L"layout_page_game")

    LoadDownloadData(CDownloadItemUI::loading);
    LoadDownloadData(CDownloadItemUI::history);

    UpdateLayoutPage();

    CDownloadMgr::Instance()->SetLoadCallback(this, root_->GetManager()->GetPaintWindow());
}

void CPageDownloadUI::Clean() {
}

DuiLib::CControlUI* CPageDownloadUI::CreateControl(LPCTSTR name) {
    if (!name) return nullptr;

    std::wstring control_name = name;
    if (control_name == L"PageLayout") return new CPageLayoutUI();
    else if (control_name == L"DownloadItem") return new CDownloadItemUI();

    return nullptr;
}

void CPageDownloadUI::LoadCallback(UINT_PTR nTask, void* lpParam, TaskState state, UINT_PTR nData) {
    ITask* pTask = (ITask*)nTask;
    if (!pTask) return;

    switch (state) {
    case Ts_Init: {
        AddToDownloadLayout(nTask, CDownloadItemUI::loading);
        UpdateLayoutPage();
        break;
    }
    case Ts_CheckEnd: {
        // 更新图标等信息
        CDuiString name;
        name.Format(L"%I64d", pTask->nGameID);

        CDownloadItemUI* item = (CDownloadItemUI*)layout_page_game_->FindSubControl(name.GetData());
        if (item) item->UpdateTaskInfo(nTask);

        break;
    }
    case Ts_Install: {
        // 更新图标等信息
        CDuiString name;
        name.Format(L"%I64d", pTask->nGameID);

        CDownloadItemUI* item = (CDownloadItemUI*)layout_page_game_->FindSubControl(name.GetData());
        if (item) item->OnItemInstall(nTask);

        break;
    }
    case Ts_Finish: {
        // 下载完成
        CDuiString name;
        name.Format(L"%I64d", pTask->nGameID);

        CDownloadItemUI* item = (CDownloadItemUI*)layout_page_game_->FindSubControl(name.GetData());
        if (item) item->OnItemFinish(nTask);

        RemoveLoadLayout(pTask, CDownloadItemUI::loading);
        AddToDownloadLayout(nTask, CDownloadItemUI::history);
        UpdateLayoutPage();
        break;
    }
    case Ts_Delete: { 
        RemoveLoadLayout(pTask, CDownloadItemUI::loading);
        UpdateLayoutPage();
        break;
    }
    case Ts_Remove: {
        RemoveLoadLayout(pTask, CDownloadItemUI::loading);
        CDownloadMgr::Instance()->DeleteLoadTask(nTask, TRUE, TRUE);
        UpdateLayoutPage();
        break;
    }
    default: {
        CDuiString name;
        name.Format(L"%I64d", pTask->nGameID);

        CDownloadItemUI* item = (CDownloadItemUI*)layout_page_game_->FindSubControl(name.GetData());
        if (item) item->ChangeItemLoadState(nTask, state, pTask->nTotalSize, pTask->nLoadSize, pTask->nSpeed, nData);
    }
    break;

    }
}

void CPageDownloadUI::LoadDownloadData(const CDownloadItemUI::Type& type) {
    //std::wstring scroll_attr = L"showbutton1=\"false\" showbutton2=\"false\" width=\"0\"";
    //auto vscrollbar_ptr = list_download->GetVerticalScrollBar();
    //if (vscrollbar_ptr) vscrollbar_ptr->ApplyAttributeList(scroll_attr.c_str());

    const TaskList& task_List = (type == CDownloadItemUI::loading ? CDownloadMgr::Instance()->GetLoadList() : CDownloadMgr::Instance()->GetFinishList());

    for each (auto it in task_List) {
        ITask* task = it;
        if (!task) continue;

        CDownloadItemUI* item = CreateDownloadItem(task, type);
        if (!item) {
            OUTPUT_XYLOG(LEVEL_ERROR, ERROR, L"create list_download_item failed!");
            continue;
        }
    }
}

bool CPageDownloadUI::AddToDownloadLayout(UINT_PTR nTask, const CDownloadItemUI::Type& type) {
    ITask* task = (ITask*)nTask;
    if (!task) return false;

    CDownloadItemUI* item = CreateDownloadItem(task, type);
    if (!item) {
        OUTPUT_XYLOG(LEVEL_ERROR, ERROR, L"create list_downloading_item failed!");
        return false;
    }

    return true;
}

void CPageDownloadUI::RemoveLoadLayout(ITask* pTask, const CDownloadItemUI::Type& type) {
    if (!pTask || !layout_page_game_) return;

    CDuiString name;
    name.Format(L"%I64d", pTask->nGameID);

    CDownloadItemUI* item = (CDownloadItemUI*)layout_page_game_->FindSubControl(name.GetData());
    if (!item) return;

    RemoveLoadLayout(item, type);
}

void CPageDownloadUI::RemoveLoadLayout(CDownloadItemUI* item, const CDownloadItemUI::Type& type) {
    if (!layout_page_game_) return;
    layout_page_game_->Remove(item);
}

int CPageDownloadUI::GetDownloadItemIndex(const CDownloadItemUI::Type& type) {
    if (!layout_page_game_) return -1;

    if (CDownloadItemUI::loading == type) return 0;

    int index = 0;
    for (int i = 0; i < layout_page_game_->GetCount(); i++) {
        auto item = (CDownloadItemUI*)layout_page_game_->GetItemAt(i);
        if (!item) continue;

        if (item->GetType() == CDownloadItemUI::loading) index++;
    }

    return index;
}

CDownloadItemUI* CPageDownloadUI::CreateDownloadItem(ITask* task, const CDownloadItemUI::Type& type) {
    if (!task || !root_ || !layout_page_game_) return nullptr;

    auto index = GetDownloadItemIndex(type);
    if (index < 0) return nullptr;

    if (type == CDownloadItemUI::loading) {
        RemoveLoadLayout(task, CDownloadItemUI::history);
    }

    auto paint_manager = root_->GetManager();
    if (!paint_manager) return nullptr;

    CDialogBuilder builder;
    CDownloadItemUI* item = (CDownloadItemUI*)builder.Create(L"DownloadItem.xml", 0, this, paint_manager);
    if (!item) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"创建游戏下载页签错误");
        return nullptr;
    }

    CDuiString name;
    name.Format(L"%I64d", task->nGameID);

    item->InitControl();
    item->SetType(type);
    item->AddDownloadItem(task);
    item->InitItemState(task);
    item->SetTag((UINT_PTR)task);
    item->SetName(name.GetData());

    layout_page_game_->AddAt(item, index);

    return item;
}

void CPageDownloadUI::UpdateGameIcon(const __int64& nGameID, const wstring& strName, const wstring& strVersion) {
    if (!layout_page_game_) return;

    CDuiString name;
    name.Format(L"%I64d", nGameID);

    auto SetGameIcon = [&](CPageLayoutUI* list_download, const wstring& strName, const wstring& strVersion) -> void {
        if (!list_download) return;

        CDownloadItemUI* item = (CDownloadItemUI*)list_download->FindSubControl(name.GetData());
        if (!item) return;

        item->UpdateGameIcon(strName, strVersion);
    };

    SetGameIcon(layout_page_game_, strName, strVersion);
}

void CPageDownloadUI::ApplicationRemoved(const __int64& nGameID) {
    if (!layout_page_game_) return;

    CDuiString name;
    name.Format(L"%I64d", nGameID);

    CDownloadItemUI* item = (CDownloadItemUI*)layout_page_game_->FindSubControl(name.GetData());
    if (!item) return;

    UINT_PTR nTask = item->GetTag();
    RemoveLoadLayout(item, CDownloadItemUI::history);

    CDownloadMgr::Instance()->DeleteFinishTask(nTask, TRUE);

    UpdateLayoutPage();
}

void CPageDownloadUI::UpdateLayoutPage() {
    if (layout_page_ && layout_page_game_) layout_page_->SelectItem(layout_page_game_->GetCount() > 0 ? 0 : 1);
}

void CPageDownloadUI::UpdateLoadLayout() {
    if (!layout_page_game_) return;

    std::vector<string> engine_apps;

    if (!CIosMgr::Instance()->GetEngineApplications(engine_apps)) return;

    int index = 0;
    for (int i = 0; i < layout_page_game_->GetCount(); i++) {
        auto item = (CDownloadItemUI*)layout_page_game_->GetItemAt(i);
        if (!item || item->GetType() != CDownloadItemUI::history) continue;

        UINT_PTR nTask = item->GetTag();

        ITask* task = (ITask*)nTask;
        if (!task) continue;

        if (task->strPkgName.empty()) {
            task = CDatabaseMgr::Instance()->GetGameInfo(task->nGameID);
            if (task->strPkgName.empty()) continue;
        }

        auto it = std::find_if(engine_apps.begin(), engine_apps.end(),
            [&](const string& node) {
                return node == task->strPkgName;
        });

        if (it != engine_apps.end()) continue;

        auto nGameID = task->nGameID;

        CDatabaseMgr::Instance()->DeleteGameInfo(nGameID);

        SetWebGameStatus(nGameID, GameUnload);

        RemoveLoadLayout(item, CDownloadItemUI::history);

        CDownloadMgr::Instance()->DeleteFinishTask(nTask, TRUE);
    }

    UpdateLayoutPage();
}
