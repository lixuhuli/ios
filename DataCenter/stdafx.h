// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>



// TODO: 在此处引用程序需要的其他头文件
#include <process.h>
#include <map>
#include <string>
#include <list>
using std::string;
using std::wstring;
using std::map;
using std::list;

#include <PublicLib/StrHelper.h>
#include <PublicLib/json/json-forwards.h>
#include <PublicLib/json/json.h>
#include <PublicLib/md5.h>
#include <PublicLib/HttpClient.h>
#include <PublicLib/System.h>
#include <AppHelper.h>


enum TaskType
{
	TaskBase = 0,
	TaskDataPost,
};
