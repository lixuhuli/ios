#pragma once


/************************************************************************/
/* 定义一些全局变量                                                     */
/************************************************************************/
#define DECLARE_STR(x,y)	static const wchar_t* x = y;


DECLARE_STR(GUI_CLASS_NAME,		L"GuiFoundation")

//主程序定义
#ifdef APP_MAIN
DECLARE_STR(MAIN_WND_NAME,		L"果仁模拟器")
DECLARE_STR(MUTEX_NAME,			L"AppleSeedMetux")
DECLARE_STR(NOTIFY_ICON_NAME,	L"果仁模拟器")
#endif

//更新程序定义
#ifdef APP_UPDATE
DECLARE_STR(UPDATE_WND_NAME,	L"果仁模拟器更新程序")
#define SCRIPT_FILE_NAME	L"UpdateScript.XML"	//配置文件名称
#endif

//卸载程序定义
#ifdef APP_UNINSTALL
DECLARE_STR(UNINSTALL_WND_NAME, L"果仁模拟器卸载程序")
#endif

//安装程序定义
#ifdef APP_SETUP
DECLARE_STR(SETUP_WND_NAME,		L"果仁模拟器安装程序")
#define SPACE_NEED			(200<<20)		//至少需要200M安装空间
#endif

//主程序名
#define EXE_MAIN			L"AppleSeed.exe"
#define EXE_RENDER			L"AppleSeedWeb.exe"
#define EXE_UNINSTALL		L"Uninstall.exe"
#define EXE_UPDATE			L"Update.exe"
#define EXE_BUGREPORT		L"BugReport.exe"


#define SOFT_NAME			L"果仁模拟器"
#define SOFT_ROOT_DIR		L"AppleSeed"

//快捷方式名称
#define SHORTCUT_MAIN_NAME	L"果仁模拟器"

//注册表主键名
#define REG_ROOT_NAME		L"AppleSeed"

#define REG_COMPANY			L"嘉兴游辰网络科技有限公司"