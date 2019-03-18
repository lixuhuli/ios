#ifndef __IOS_MGR_H__
#define __IOS_MGR_H__

#pragma once
#include "cef_weak_ptr.h"

namespace emulator{
    class EmulatorStateInfo;
    class iEmulatorStateInfo;
    enum EMULATOR_STATE;
}
class CIosThread;
class CWndIos;
class CKeyWnd;
class CIosMgr
    : public base::SupportsWeakPtr<CIosMgr> {
public:
    static CIosMgr* Instance();

public:
    CIosMgr();
    virtual ~CIosMgr();

    bool Init();
    void Exit();

public:
    void CreateWndIos(HWND hParentWnd);
    void CreateKeyWnd(HWND hParentWnd, const string& strAppId);
    void UpdateIosWnd(const QRect* lprc = nullptr);
    void UpdateKeyWnd(const QRect* lprc = nullptr);

    void UpdateBrowserMode(bool browser_mode);

    void OnPackUpdating(int percent);
    void OnPackUpdate(int status, bool reboot);

    void OnGetKeyboard(WPARAM wParam, LPARAM lParam);

    const bool& HorScreenMode() { return hor_screen_mode_; }
    const double& IosWndScale() { return ios_scale_; }

    // 模拟器状态信息 给外部调用
    base::WeakPtr<emulator::iEmulatorStateInfo> emulator_state_info();

    void CloseKeyWnd();

    void UpdateIosWndStatus();

    bool IsCPUVTOpened(bool& support);

    void WriteBaseKeyBoard(const string& app_id);

public:
    bool IosEngineOn(const string& iso_file);
    bool IosCheckLicense(const char *serial);
    void IosRestart();
    void IosHome();
    bool IosSnap(const std::string& save_path);

    void InstallApp(const UINT_PTR& task_ptr);
    void InstallApp(const std::wstring& file_path);

    int UpdatePackage(const std::wstring& file_path, BOOL& restart);

    int UpdateKeyMap(const std::wstring& file_path);

    int EngineCallback(int status, uintptr_t param1, uintptr_t param2);

    wstring GetKeyMapDir();

    bool GetEngineApplications(std::vector<string>& engine_apps);

    bool SaveEngineReport(const std::wstring& input_file, const std::wstring& output_file);

    void CreateEngineOffTask();

    CKeyWnd* GetKeyWndPtr();

protected:
    int InstallAppThread(void * argument);
    int InstallFileThread(void * argument);
    int EngineOffThread(void * argument);

    void OnEngineOn();
    void OnEngineOff(int state);
    void OnScreenSizeChanged(uintptr_t param1, uintptr_t param2);
    void OnForegroundAppChanged(uintptr_t param1, uintptr_t param2);
    void OnApplicationAdded(uintptr_t param1, uintptr_t param2);
    void OnApplicationModified(uintptr_t param1, uintptr_t param2);
    void OnApplicationRemoved(uintptr_t param1, uintptr_t param2);
    void OnGetUidAndToken(uintptr_t param1, uintptr_t param2);
    void OnEmulationQuit(uintptr_t param1, uintptr_t param2);

    void OnInstallApp(const UINT_PTR& task_ptr, int state);
    void OnInstallFile(const UINT_PTR& task_ptr, int state);

private:
    void CreateInstallAppTask(const UINT_PTR& task_ptr);
    void CreateInstallFileTask(const UINT_PTR& task_ptr);

    void StartInstallApp();

    void CheckEngineUpdate();

    bool HasKeyMapFile();  

private:
    CWndIos* ios_wnd_;
    CKeyWnd* key_wnd_;

    bool hor_screen_mode_;
    double ios_scale_;
    bool engine_on_;

    CSize wnd_size_;

    std::unique_ptr<CIosThread> install_app_thread_;
    std::unique_ptr<CIosThread> engine_off_thread_;
    scoped_refptr<emulator::EmulatorStateInfo> emulator_state_info_;

    std::list<UINT_PTR>	list_install_task_;
    std::list<UINT_PTR>	list_install_file_;
};

#endif // __IOS_MGR_H__