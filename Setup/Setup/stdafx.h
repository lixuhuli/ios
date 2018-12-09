// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>
#include <ShlObj.h>
#include <ShellAPI.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include <../../Duilib/UIlib.h>
using namespace DuiLib;


#include <AppHelper.h>
#include <SoftDefine.h>


#include <PublicLib/XYLog.h>
#include <PublicLib/System.h>
#include <PublicLib/DeCompress.h>
#include <PublicLib/Shortcut.h>
#include <PublicLib/ProcessHelper.h>
#include <PublicLib/json/json-forwards.h>
#include <PublicLib/json/json.h>


#include <vector>
#include <string>
using std::string;
using std::wstring;
using std::vector;



static const wchar_t* g_pConflict[] = {
	EXE_MAIN,
	EXE_RENDER,
	EXE_UPDATE,
	EXE_BUGREPORT,
};