#pragma once


/************************************************************************/
/* ����һЩȫ�ֱ���                                                     */
/************************************************************************/
#define DECLARE_STR(x,y)	static const wchar_t* x = y;


DECLARE_STR(GUI_CLASS_NAME,		L"GuiFoundation")

//��������
#ifdef APP_MAIN
DECLARE_STR(MAIN_WND_NAME,		L"����ģ����")
DECLARE_STR(MUTEX_NAME,			L"AppleSeedMetux")
DECLARE_STR(NOTIFY_ICON_NAME,	L"����ģ����")
#endif

//���³�����
#ifdef APP_UPDATE
DECLARE_STR(UPDATE_WND_NAME,	L"����ģ�������³���")
#define SCRIPT_FILE_NAME	L"UpdateScript.XML"	//�����ļ�����
#endif

//ж�س�����
#ifdef APP_UNINSTALL
DECLARE_STR(UNINSTALL_WND_NAME, L"����ģ����ж�س���")
#endif

//��װ������
#ifdef APP_SETUP
DECLARE_STR(SETUP_WND_NAME,		L"����ģ������װ����")
#define SPACE_NEED			(200<<20)		//������Ҫ200M��װ�ռ�
#endif

//��������
#define EXE_MAIN			L"AppleSeed.exe"
#define EXE_RENDER			L"AppleSeedWeb.exe"
#define EXE_UNINSTALL		L"Uninstall.exe"
#define EXE_UPDATE			L"Update.exe"
#define EXE_BUGREPORT		L"BugReport.exe"


#define SOFT_NAME			L"����ģ����"
#define SOFT_ROOT_DIR		L"AppleSeed"

//��ݷ�ʽ����
#define SHORTCUT_MAIN_NAME	L"����ģ����"

//ע���������
#define REG_ROOT_NAME		L"AppleSeed"

#define REG_COMPANY			L"�����γ�����Ƽ����޹�˾"