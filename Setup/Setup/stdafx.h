// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>
#include <ShlObj.h>
#include <ShellAPI.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 在此处引用程序需要的其他头文件
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