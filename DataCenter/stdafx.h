// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>



// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
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
