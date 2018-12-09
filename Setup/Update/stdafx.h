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
#include <ObjBase.h>
#include <Shlwapi.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <process.h>

// TODO:  在此处引用程序需要的其他头文件
#include <list>
#include <vector>
#include <string>
#include <map>
using std::list;
using std::vector;
using std::string;
using std::wstring;
using std::map;


#include <PublicLib/XYLog.h>
#include <PublicLib/StrHelper.h>
//#include <PublicLib/Markup.h>
#include <PublicLib/ProcessHelper.h>
#include <PublicLib/FileHelper.h>
#include <PublicLib/System.h>
#include <PublicLib/HttpClient.h>
#include <PublicLib/Shortcut.h>
#include <PublicLib/json/json-forwards.h>
#include <PublicLib/json/json.h>
#include <PublicLib/VersionHelper.h>
#include <PublicLib/DeCompress.h>
#include <PublicLib/tinyxml.h>
#include <PublicLib/md5.h>
#include <AppHelper.h>
#include <SoftDefine.h>


#include "../../Duilib/UIlib.h"
using namespace DuiLib;


enum UpdateStatus
{
	UpdateError = 0,	//更新失败
	UpdateSelf,			//自更新
	UpdateNone,			//已是最新版本
	UpdateNew,			//可更新
};