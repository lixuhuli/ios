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

#include "../Duilib/UIlib.h"
using namespace DuiLib;

#include "Resource.h"
#include "WndBase.h"

#include <PublicLib/XYLog.h>

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

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
