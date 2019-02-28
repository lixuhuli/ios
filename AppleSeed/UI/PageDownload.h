#ifndef __PAGE_DOWNLOAD_H__
#define __PAGE_DOWNLOAD_H__

#pragma once
#include "cef_weak_ptr.h"
#include "Download/TaskCallback.h"
#include "CallBack/appleseed_callback.h"
#include "DownloadItem.h"

struct ITask;
class CPageLayoutUI;
class CDownloadItemUI;
class CPageDownloadUI
    : public base::SupportsWeakPtr<CPageDownloadUI>
    , public ILoadCallback
    , public IDialogBuilderCallback
    , public AppleSeedCallback{
public:
    CPageDownloadUI(CControlUI* root);
    virtual ~CPageDownloadUI();
public:
    virtual CControlUI* CreateControl(LPCTSTR name) override;

    virtual void Init();
    virtual void Clean();

    bool AddToDownloadLayout(UINT_PTR nTask, const CDownloadItemUI::Type& type);
    void RemoveLoadLayout(ITask* pTask, const CDownloadItemUI::Type& type);
    void RemoveLoadLayout(CDownloadItemUI* item, const CDownloadItemUI::Type& type);

    void UpdateLoadLayout();

    void ShowUninstallBtnStatus(bool show);

protected:
    void OnTimer(int nTimerID, CButtonUI* btn_pause);
    enum { TIMER_ID_BUTTON_LONGDOWN = 17 };

    BEGIN_INIT_CTRL()
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
    END_BIND_CTRL()

    virtual void LoadCallback(UINT_PTR nTask, void* lpParam, TaskState state, UINT_PTR nData) override;

    // 回调处理
    virtual void UpdateGameIcon(const __int64& nGameID, const wstring& strName, const wstring& strVersion) override;
    virtual void ApplicationRemoved(const __int64& nGameID) override;

    bool OnClickItemBtnUpdate(void* param, void* param_item);
    bool OnClickItemBtnDelete(void* param, void* param_item);
    bool OnItemBtnPauseEvent(void* param);

private:
    void LoadDownloadData(const CDownloadItemUI::Type& type);

    CDownloadItemUI* CreateDownloadItem(ITask* task, const CDownloadItemUI::Type& type);

    int GetDownloadItemIndex(const CDownloadItemUI::Type& type);

    void UpdateLayoutPage();

private:
    CVerticalLayoutUI* root_;
    CTabLayoutUI* layout_page_;
    CPageLayoutUI* layout_page_game_;

    bool shwo_del_button_;
};

#endif // __PAGE_GAME_H__