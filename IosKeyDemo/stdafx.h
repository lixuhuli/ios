// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>
#include <shellapi.h>
#include <ShlObj.h>
#include <shlwapi.h>
#include <WinSock2.h>
#include <Tlhelp32.h>
#pragma comment(lib, "ws2_32.lib")

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "../Duilib/UIlib.h"
#include "../IosKeyMap/IosKeySdk.h"

using namespace DuiLib;
using namespace ioskey;

#include "Resource.h"
#include "WndBase.h"

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
