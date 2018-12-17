#ifndef _WND_MAIN_INCLUDE_H_
#define _WND_MAIN_INCLUDE_H_

#pragma once
#include "UICefWebkit.h"
#include "WebCtrlUI.h"
#include "MsgDefine.h"
#include "PublicLib/HttpLib.h"

namespace download{
    class DownloadInfo;
    class iDownloadInfo;
}

using namespace PublicLib;
class CPageDownloadUI;
class CWndMain :
    public CWndBase {
public:
    CWndMain();
    virtual ~CWndMain();

    void SetGameStatus(__int64 nGameID, int nStatus) {
        CDuiString str;
        str.Format(L"initGameBtnStatus();");

        if (web_home_page_)  web_home_page_->ExecuteJavascript(str.GetData());
    }

    // 模拟器状态信息 给外部调用
    base::WeakPtr<download::iDownloadInfo> download_info();

protected:
    virtual LPCWSTR GetWndName() const override { return L"果仁模拟器"; }
    virtual LPCWSTR GetXmlPath() const override;
    virtual LPCTSTR GetWindowClassName() const override;
    virtual CControlUI* CreateControl(LPCTSTR pstrClass);
    virtual void InitWindow();
    virtual void OnFinalMessage(HWND hWnd);
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual LRESULT OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnSizing(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMoving(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnDownloadMirrorSystem(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    virtual LRESULT OnMsgFileUnziping(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMsgFileUnzip(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMsgEmulatorAlready(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMsgLoadIosEngine(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMsgExit(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnTimer(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMsgUserInfo(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMsgCommon(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMsgUserIco(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMsgUpdateIosWndPos(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    virtual LRESULT OnMsgIosEngineUpdating(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMsgIosEngineUpdate(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    LRESULT OnMsgCopyData(WPARAM wParam, LPARAM lParam);
    LRESULT OnCopyDataStartGame(COPYDATASTRUCT* pCopyData);
    LRESULT OnCopyDataGameStatus(COPYDATASTRUCT* pCopyData);
    LRESULT OnCopyDataShowMsg(COPYDATASTRUCT* pCopyData);
    LRESULT OnCopyDataShowToast(COPYDATASTRUCT* pCopyData);
    LRESULT OnCopyDataShowConfirm(COPYDATASTRUCT* pCopyData);


    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE(btn_ios_restart_, CButtonUI, L"btn_ios_restart")
        DECLARE_CTRL_TYPE(btn_ios_home_, CButtonUI, L"btn_ios_home")
        DECLARE_CTRL_TYPE(web_home_page_, CWebCtrlUI, L"page_market")
        DECLARE_CTRL_TYPE(layout_main_, CTabLayoutUI, L"layout_main")
        DECLARE_CTRL_TYPE(opt_home_market_, COptionUI, L"opt_home_market")
        DECLARE_CTRL_TYPE(opt_home_bbs_, COptionUI, L"opt_home_bbs")
        DECLARE_CTRL_TYPE(opt_home_download_, COptionUI, L"opt_home_download")
        DECLARE_CTRL_TYPE(install_system_, CVerticalLayoutUI, L"install_system")
        DECLARE_CTRL_TYPE(select_folder_, CVerticalLayoutUI, L"select_folder")
        DECLARE_CTRL_TYPE(layout_install_, CTabLayoutUI, L"layout_install")
        DECLARE_CTRL_TYPE(lbl_install_status_, CLabelUI, L"lbl_install_status")
        DECLARE_CTRL_TYPE(lbl_install_speed_, CLabelUI, L"lbl_install_speed")
        DECLARE_CTRL_TYPE(progress_install_, CProgressUI, L"progress_install")
        DECLARE_CTRL_TYPE(btn_install_pause_, COptionUI, L"btn_install_pause")
        DECLARE_CTRL_TYPE(install_loading_, CControlUI, L"install_loading")
        DECLARE_CTRL_TYPE(btn_install_restart_, CButtonUI, L"btn_install_restart")
        DECLARE_CTRL_TYPE(btn_ios_volume_, CButtonUI, L"btn_ios_volume")
        DECLARE_CTRL_TYPE(btn_key_, CButtonUI, L"btn_key")
        DECLARE_CTRL_TYPE(btn_user_icon_, CButtonUI, L"lbl_user_ico")
        DECLARE_CTRL_TYPE(user_mask_ico_, CControlUI, L"user_mask_ico")
        DECLARE_CTRL_TYPE(client_layout_left_, CHorizontalLayoutUI, L"client_layout_left")
        DECLARE_CTRL_TYPE(client_layout_right_, CHorizontalLayoutUI, L"client_layout_right")
        DECLARE_CTRL_TYPE(client_iphone_emulator_, CVerticalLayoutUI, L"client_iphone_emulator")
        DECLARE_CTRL_TYPE(layout_update_, CVerticalLayoutUI, L"layout_update")
        DECLARE_CTRL_TYPE(progress_update_, CProgressUI, L"progress_update")
        DECLARE_CTRL_TYPE(lbl_update_status_, CLabelUI, L"lbl_update_status")
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK(L"btn_sys_close", &CWndMain::OnBtnClickClose)
        BIND_CTRL_CLICK(L"btn_sys_min", &CWndMain::OnBtnClickMin)
        BIND_CTRL_CLICK(L"btn_sys_max", &CWndMain::OnBtnClickMax)
        BIND_CTRL_CLICK(L"btn_sys_restore", &CWndMain::OnBtnClickRestore)
        BIND_CTRL_CLICK(L"btn_ios_restart", &CWndMain::OnBtnClickIosRestart)
        BIND_CTRL_CLICK(L"btn_ios_home", &CWndMain::OnBtnClickIosHome)
        BIND_CTRL_CLICK(L"btn_ios_volume", &CWndMain::OnBtnClickVolume)
        BIND_CTRL_CLICK(L"btn_install_home", &CWndMain::OnBtnClickIosHome)
        BIND_CTRL_CLICK(L"opt_home_market", &CWndMain::OnClickHomePage);
        BIND_CTRL_CLICK(L"opt_home_bbs", &CWndMain::OnClickHomeBbs);
        BIND_CTRL_CLICK(L"opt_home_download", &CWndMain::OnClickHomeDownload)
        BIND_CTRL_CLICK(L"btn_install_sys", &CWndMain::OnClickInstallSys)
        BIND_CTRL_CLICK(L"btn_install_cancel", &CWndMain::OnClickInstallCancel)
        BIND_CTRL_CLICK(L"btn_get_iso_sys_1", &CWndMain::OnClickGetIsoSys)
        BIND_CTRL_CLICK(L"btn_get_iso_sys_2", &CWndMain::OnClickGetIsoSys)
        BIND_CTRL_CLICK(L"btn_get_iso_sys_3", &CWndMain::OnClickGetIsoSys)
        BIND_CTRL_CLICK(L"btn_install_pause", &CWndMain::OnClickInstallPause)
        BIND_CTRL_CLICK(L"btn_install_close", &CWndMain::OnClickInstallClose)
        BIND_CTRL_CLICK(L"btn_install_select_key", &CWndMain::OnClickInstallSelectKey)
        BIND_CTRL_CLICK(L"btn_install_restart", &CWndMain::OnClickInstallRestart)
        BIND_CTRL_CLICK(L"btn_snap", &CWndMain::OnBtnClickSnap)
        BIND_CTRL_CLICK(L"btn_record", &CWndMain::OnBtnClickRecord)
        BIND_CTRL_CLICK(L"btn_key", &CWndMain::OnBtnClickKey)
        BIND_CTRL_CLICK(L"lbl_user_ico", &CWndMain::OnBtnClickUser);
        BIND_CTRL_EVENT(L"lbl_user_ico", DUI_MSGTYPE_MOUSEENTER, &CWndMain::OnEnterBtnUser);
        BIND_CTRL_EVENT(L"lbl_user_ico", DUI_MSGTYPE_MOUSELEAVE, &CWndMain::OnLeaveBtnUser);
    END_BIND_CTRL()

    bool OnBtnClickClose(void* lpParam);
    bool OnBtnClickMin(void* lpParam);
    bool OnBtnClickMax(void* lpParam);
    bool OnBtnClickRestore(void* lpParam);
    bool OnBtnClickIosRestart(void* param);
    bool OnBtnClickIosHome(void* param);
    bool OnBtnClickVolume(void* param);
    bool OnBtnClickUser(void* param);
    bool OnEnterBtnUser(void* param);
    bool OnLeaveBtnUser(void* param);

    bool OnClickHomePage(void* lpParam);
    bool OnClickHomeBbs(void* lpParam);
    bool OnClickHomeDownload(void* lpParam);
    bool OnClickInstallSys(void* param);
    bool OnClickInstallCancel(void* param);
    bool OnClickGetIsoSys(void* param);
    bool OnClickInstallPause(void* param);
    bool OnClickInstallClose(void* param);
    bool OnClickInstallSelectKey(void* param);
    bool OnClickInstallRestart(void* param);

    bool OnBtnClickSnap(void* param);
    bool OnBtnClickRecord(void* param);
    bool OnBtnClickKey(void* param);

    bool OnIphoneEmulatorSize(void* param);

protected:
    void InitTasks();

    void Exit(); // 程序退出唯一入口
    void OnWndClose(WPARAM wParam, LPARAM lParam, BOOL& bHandled); // 退出处理

    void AddNotifyIcon();
    void RemoveNotifyIcon();

    void ExitAccount();

    void SwitchPage(int page);

    void UpdateTopWebCtrls();

    void DownloadMirrorSystem();
    void UnzipMirrorSystem();

    void ShowUserCenter();

    void ShowClientLayoutLeft(bool show);

    void UpdateUserUI(bool bClear = false);
    void UpdateUserIco();

private:
    static void LoadMirrorSystemCallback(PublicLib::EnumDownloadState state, double dltotal, double dlnow, void * Userdata);
    void LoadMirrorSystemCallback(PublicLib::EnumDownloadState state, double dltotal, double dlnow);

    string GetUrlPackageName();
    string GetUrlInitIsoName();
    string GetUrlIsoName();

    bool LoadIosEngine();

    void PopupKeyWindow();

    void UpdateLoadingIcon();

    bool CheckEngineKey(const wstring& key_file);

    void ShowVolumeWnd();
    void ShowUserWnd();

    void SelectedHomePage(COptionUI* opt_select);

private:
    CCefWebkitUI *web_focus_;

    CTabLayoutUI* layout_main_;
    COptionUI* opt_home_market_;
    COptionUI* opt_home_bbs_;
    COptionUI* opt_home_download_;

    CWebCtrlUI* web_home_page_;

    CButtonUI* btn_ios_restart_;
    CButtonUI* btn_ios_home_;

    CButtonUI* btn_user_icon_;
    CControlUI* user_mask_ico_;

    NOTIFYICONDATA m_nd;

    std::unique_ptr<CPageDownloadUI> page_download_;

    // in emulator
    CVerticalLayoutUI* install_system_;
    CVerticalLayoutUI* select_folder_;
    CButtonUI* btn_install_sys_;
    CButtonUI* btn_install_cancel_;
    CTabLayoutUI* layout_install_;
    CLabelUI* lbl_install_status_;
    CLabelUI* lbl_install_speed_;
    CProgressUI *progress_install_;
    COptionUI* btn_install_pause_;
    CControlUI* install_loading_;
    CButtonUI* btn_install_restart_;
    int loading_frame_;

    CButtonUI* btn_ios_volume_;

    CButtonUI* btn_key_;

    CHorizontalLayoutUI* client_layout_left_;
    CHorizontalLayoutUI* client_layout_right_;
    CVerticalLayoutUI* client_iphone_emulator_;
    bool need_hide_left_layout_;

    scoped_refptr<download::DownloadInfo> download_info_;

    // 更新进度显示
    CVerticalLayoutUI* layout_update_;
    CProgressUI* progress_update_;
    CLabelUI* lbl_update_status_;
};

#endif  // !#define (_WND_MAIN_INCLUDE_H_)  