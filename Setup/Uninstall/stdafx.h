// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>
#include <ShellAPI.h>
#include <ShlObj.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include <vector>
#include <string>
using std::vector;
using std::string;
using std::wstring;

#include <PublicLib/XYLog.h>
#include <PublicLib/AutoRegkey.h>
#include <PublicLib/StrHelper.h>
#include <PublicLib/System.h>
#include <PublicLib/ProcessHelper.h>
#include <PublicLib/FileHelper.h>
#include <PublicLib/json/json-forwards.h>
#include <PublicLib/json/json.h>
#include "Regedit.h"
#include <AppHelper.h>
#include <SoftDefine.h>

#include "../../Duilib/UIlib.h"
using namespace DuiLib;

