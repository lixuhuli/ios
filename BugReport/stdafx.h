// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <objbase.h>

#include "../Duilib/UIlib.h"
using namespace DuiLib;
#include "AppHelper.h"




enum DumpProcessType
{
	DptMain = 0,
	DptEmulator,
	DptCafe,
    DptAppleSeed,
};

extern wstring g_datFilePath;
extern wstring g_dumpFilePath;
extern wstring g_zipFilePath;
extern wstring g_AppPath;
extern wstring g_AppVersion;
//extern wstring g_logFilePath;
extern wstring g_exeType;
extern DumpProcessType g_dumpProcess;
// TODO: reference additional headers your program requires here


#include <PublicLib/HttpLib.h>
#include <PublicLib/FileZip.h>
#include <PublicLib/StrHelper.h>
#include <PublicLib/md5.h>
// #include <PublicLib/json/json-forwards.h>
// #include <PublicLib/json/json.h>
// #include <DataCenter/DataCenterApi.h>
using namespace PublicLib;

