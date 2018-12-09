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
#include <ObjBase.h>
#include <Shlwapi.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <process.h>

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
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
	UpdateError = 0,	//����ʧ��
	UpdateSelf,			//�Ը���
	UpdateNone,			//�������°汾
	UpdateNew,			//�ɸ���
};