#include "Download/TaskCallback.h"
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 The Sun.AC Authors . All Rights Reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DOWNLOAD_ITEM_INCLUDE_H_
#define _DOWNLOAD_ITEM_INCLUDE_H_

struct ITask;
class CDownloadItemUI
    : public CVerticalLayoutUI {
public:
    enum Type{
        loading = 0,
        history
    };

public:
    CDownloadItemUI();
    virtual ~CDownloadItemUI();

    virtual void Init() override;
    virtual void DoEvent(TEventUI& event) override;
    virtual void SetPos(RECT rc) override;

public:
    virtual void InitControl();
    virtual void InitItemState(const ITask *task);
    void UpdateTaskInfo(UINT_PTR nTask);
    void ChangeItemLoadState(UINT_PTR nTask, TaskState state, __int64 nTotalSize, __int64 nLoadSize, int nSpeed, UINT_PTR nData); // 更改下载任务状态
    void OnItemInstall(UINT_PTR nTask); // 更改安装任务状态
    void OnItemFinish(UINT_PTR nTask);  // 更改安装任务状态
    void UpdateUninstallBtnStatus(bool show);
    void UpdateUpdateBtnStatus(const wstring& strVer);

    virtual bool AddDownloadItem(const ITask *task);
    virtual void UpdateGameIcon(const wstring& strName, const wstring& strVersion);

    void SetType(const Type& type) { type_ = type; }
    const Type& GetType() { return type_; }

    CButtonUI* btn_update() { return btn_update_; };
    CButtonUI* btn_pause() { return btn_pause_; };
    CButtonUI* btn_delete() { return btn_delete_; };

    bool need_update();

protected:
    enum { TIMER_ID_INSTALLING = 16 };
    void OnTimer(int nTimerID);

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE_PAGE(label_game_icon_, CControlUI, this, L"label_icon")
        DECLARE_CTRL_TYPE_PAGE(label_game_discribe_, CLabelUI, this, L"label_discribe")
        DECLARE_CTRL_TYPE_PAGE(progress_percent_, CControlUI, this, L"progress_percent")
        DECLARE_CTRL_TYPE_PAGE(btn_pause_, CButtonUI, this, L"btn_pause")
        DECLARE_CTRL_TYPE_PAGE(btn_update_, CButtonUI, this, L"btn_update")
        DECLARE_CTRL_TYPE_PAGE(btn_delete_, CButtonUI, this, L"btn_delete")
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK_PAGE(L"btn_pause", this, &CDownloadItemUI::OnClickBtnPause)
    END_BIND_CTRL()

    bool OnClickBtnPause(void* param);

private:
    void OnInstallingIpa();

private:
    CControlUI* label_game_icon_;
    CLabelUI* label_game_discribe_;
    CControlUI* progress_percent_;

    CButtonUI* btn_pause_;
    CButtonUI* btn_update_;
    CButtonUI* btn_delete_;

    Type type_;
    int install_frame_;

    bool need_update_;
};

#endif  // !#define (_GAME_ITEM_INCLUDE_H_)  
