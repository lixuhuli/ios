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
#include <time.h>
#include <assert.h>
#include <process.h>
#include <stdint.h>
#include <list>
#include <map>
using std::list;
using std::map;

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "../Duilib/UIlib.h"
using namespace DuiLib;

#include "Resource.h"
#include "WndBase.h"

#include <AppHelper.h>
#include <AutoLock.h>
#include <PublicLib/XYLog.h>
#include <PublicLib/StrHelper.h>
#include <PublicLib/Common.h>
#include <PublicLib/json/json-forwards.h>
#include <PublicLib/json/json.h>
#include <PublicLib/md5.h>
#include <PublicLib/HttpClient.h>
#include <PublicLib/Markup.h>
#include <PublicLib/System.h>
#include <PublicLib/FileHelper.h>
#include <PublicLib/ProcessHelper.h>
#include <atlfile.h>
#include "CPP/Common/Common.h"

#define MAX_USER_INFO_SIZE		1024
#define MAX_CLIENT_INFO_SIZE	256
typedef struct CEF_SHARE_DATA_ {
    HWND hMainWnd;
    UINT nLoadCount;
    char szUserInfo[MAX_USER_INFO_SIZE + 1];
    char szClientInfo[MAX_CLIENT_INFO_SIZE + 1];
}CEF_SHARE_DATA, *LPCEF_SHARE_DATA;

static LPCWSTR g_pShareGuid = L"{B6364C8A-47E4-4FD1-9E0D-658DB9CFC5A9}";
extern CAtlFileMapping<CEF_SHARE_DATA> g_shareData;

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�

inline SIZE GetPaintTextSize(CPaintManagerUI* pManager, LPCTSTR strText, int nFontIndex)
{
    if (NULL == pManager)
    {
        SIZE sz = { 0, 0 };
        return sz;
    }
    HDC hDC = pManager->GetPaintDC();
    HFONT hOldFont = (HFONT)SelectObject(hDC, pManager->GetFont(nFontIndex));
    RECT rc = { 0 };
    ::DrawText(hDC, strText, -1, &rc, DT_LEFT | DT_SINGLELINE | DT_CALCRECT | DT_NOPREFIX);
    ::SelectObject(hDC, hOldFont);
    SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };
    return sz;
}