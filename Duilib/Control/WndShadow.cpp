// WndShadow.h : header file
//
// Version 0.3
//
// Copyright (c) 2006-2007 Mingliang Zhu, All Rights Reserved.
//
// mailto:perry@live.com
//
//
// This source file may be redistributed unmodified by any means PROVIDING 
// it is NOT sold for profit without the authors expressed written 
// consent, and providing that this notice and the author's name and all 
// copyright notices remain intact. This software is by no means to be 
// included as part of any third party components library, or as part any
// development solution that offers MFC extensions that are sold for profit. 
// 
// If the source code is used in any commercial applications then a statement 
// along the lines of:
// "Portions Copyright (c) 2006-2007 Mingliang Zhu"
// must be included in the "Startup Banner", "About Box" or "Printed
// Documentation". This software is provided "as is" without express or
// implied warranty. Use it at your own risk! The author accepts no 
// liability for any damage/loss of business that this product may cause.
//
/////////////////////////////////////////////////////////////////////////////
//****************************************************************************

//****************************************************************************
// Update history--
//
// Version 0.3, 2007-06-14
//    -The shadow is made Windows Vista Aero awareness.
//    -Fixed a bug that causes the shadow to appear abnormally on Windows Vista.
//    -Fixed a bug that causes the shadow to appear abnormally if parent window
//     is initially minimized or maximized
//
// Version 0.2, 2006-11-23
//    -Fix a critical issue that may make the shadow fail to work under certain
//     conditions, e.g., on Win2000, on WinXP or Win2003 without the visual
//     theme enabled, or when the frame window does not have a caption bar.
//
// Version 0.1, 2006-11-10
//    -First release
//****************************************************************************

#include "stdafx.h"
#include "WndShadow.h"
#include "math.h"
#include "crtdbg.h"

// Some extra work to make this work in VC++ 6.0

// walk around the for iterator scope bug of VC++6.0
#ifdef _MSC_VER
#if _MSC_VER == 1200
#define for if(false);else for
#endif
#endif

// Some definitions for VC++ 6.0 without newest SDK
#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x00080000
#endif

#ifndef AC_SRC_ALPHA
#define AC_SRC_ALPHA 0x01
#endif

#ifndef ULW_ALPHA
#define ULW_ALPHA 0x00000002
#endif

namespace DuiLib
{
// Vista aero related message
#ifndef WM_DWMCOMPOSITIONCHANGED
#define WM_DWMCOMPOSITIONCHANGED 0x031E
#endif

HINSTANCE CWndShadow::s_hInstance = (HINSTANCE)INVALID_HANDLE_VALUE;
const TCHAR *strWndClassName = _T("PerryShadowWnd");
const TCHAR *g_pPropName	 = _T("CWndShadow-This");

CWndShadow::CWndShadow(void)
: m_hWnd(NULL)
, m_OriParentProc(NULL)
, m_pManager(NULL) 
, m_bPrepared(false)
{
	ZeroMemory(m_szBkImage, sizeof(TCHAR)*(MAX_PATH+1));
	ZeroMemory(&m_rcParentWnd, sizeof(m_rcParentWnd));
	ZeroMemory(&m_szShadowWnd, sizeof(m_szShadowWnd));
}

CWndShadow::~CWndShadow(void)
{
}

bool CWndShadow::Initialize(HINSTANCE hInstance)
{
	s_hInstance = hInstance;

	// Register window class for shadow window
	WNDCLASSEX wcex;

	memset(&wcex, 0, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= DefWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= strWndClassName;
	wcex.hIconSm		= NULL;
	RegisterClassEx(&wcex);
	return true;
}

void CWndShadow::SetPrepared()
{
	if ( IsZoomed(m_hParentWnd) )
		ShowWindow(m_hWnd, FALSE);
	if ( m_bPrepared )
		return ;
	m_bPrepared = true;
	Update();
	//计算阴影窗口的位置
	int nx = m_rcParentWnd.left - m_rcBorder.left;
	int ny = m_rcParentWnd.top - m_rcBorder.top;
	::MoveWindow(m_hWnd, nx, ny, m_szShadowWnd.cx, m_szShadowWnd.cy, FALSE);
}

void CWndShadow::SetBkImage(const TCHAR* pBkImage)
{
	_tcscpy_s(m_szBkImage, pBkImage);
}

void CWndShadow::SetBorder(RECT rcBorder)
{
	m_rcBorder.left = rcBorder.left;
	m_rcBorder.right = rcBorder.right;
	m_rcBorder.top = rcBorder.top;
	m_rcBorder.bottom = rcBorder.bottom;
}

void CWndShadow::Create(HWND hParentWnd, CPaintManagerUI* pManager)
{
	m_pManager = pManager;
	m_hParentWnd	= hParentWnd;
	_ASSERT(s_hInstance != INVALID_HANDLE_VALUE);
	//创建一个阴影窗口
	m_hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, strWndClassName, NULL, WS_POPUP, \
		CW_USEDEFAULT, 0, 0, 0, hParentWnd, NULL, s_hInstance, NULL);

	m_Status = SS_ENABLED;	//设置属性为不可用状态
	Show(hParentWnd);	//
	SetProp(hParentWnd, g_pPropName, this);
	// Replace the original WndProc of parent window to steal messages
	m_OriParentProc = GetWindowLong(hParentWnd, GWL_WNDPROC);//得到父窗口的 消息循环函数地址

#pragma warning(disable: 4311)	// temporrarily disable the type_cast warning in Win32
	SetWindowLong(hParentWnd, GWL_WNDPROC, (LONG)ParentProc);//设置 父窗口消息循环新地址先走阴影窗口的消息循环
#pragma warning(default: 4311)
}

LRESULT CALLBACK CWndShadow::ParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWndShadow* pThis = (CWndShadow*)GetProp(hwnd, g_pPropName);
	ASSERT(pThis);
	WNDPROC pDefProc = (WNDPROC)pThis->m_OriParentProc;
	switch(uMsg)
	{
	case WM_MOVE:
		{
			if ( IsZoomed(hwnd) || !IsWindowVisible(pThis->m_hWnd) )
				break;
			GetWindowRect(pThis->m_hParentWnd, &pThis->m_rcParentWnd);
			pThis->m_szShadowWnd.cx = pThis->m_rcParentWnd.right - pThis->m_rcParentWnd.left + pThis->m_rcBorder.left + pThis->m_rcBorder.right;
			pThis->m_szShadowWnd.cy = pThis->m_rcParentWnd.bottom - pThis->m_rcParentWnd.top + pThis->m_rcBorder.top + pThis->m_rcBorder.bottom;
			int nx = pThis->m_rcParentWnd.left - pThis->m_rcBorder.left;
			int ny = pThis->m_rcParentWnd.top - pThis->m_rcBorder.top;
			::MoveWindow(pThis->m_hWnd, nx, ny, pThis->m_szShadowWnd.cx, pThis->m_szShadowWnd.cy, pThis->m_bPrepared?TRUE:FALSE);
			ShowWindow(pThis->m_hWnd, SW_NORMAL);
		}
		break;
	case WM_SIZE:
		{
			switch( wParam )
			{
			case SIZE_RESTORED:
				pThis->m_bPrepared = false;
				pThis->SetPrepared();
				break;
			case SIZE_MAXIMIZED:
			case SIZE_MINIMIZED:
				ShowWindow(pThis->m_hWnd, SW_HIDE);
				break;
			default:
				break;
			}
		}
		break;
	case WM_SIZING:
		pThis->Update();
		break;
	case WM_EXITSIZEMOVE:
		if( pThis->m_bPrepared && IsWindowVisible(pThis->m_hWnd) )
			pThis->Update();
		break;
	case WM_DESTROY:
		pThis->Destroy();
		break;
	case WM_SHOWWINDOW:
		if ( wParam == TRUE )
			pThis->Update();
		else
			ShowWindow(pThis->m_hWnd, SW_HIDE);
		break;
	}
	return pDefProc(hwnd, uMsg, wParam, lParam);
}

void CWndShadow::Update()
{
	if (IsZoomed(m_hParentWnd) || _tcslen(m_szBkImage) == 0)
		return;
	GetWindowRect(m_hParentWnd, &m_rcParentWnd);
	m_szShadowWnd.cx = m_rcParentWnd.right - m_rcParentWnd.left + m_rcBorder.left + m_rcBorder.right;
	m_szShadowWnd.cy = m_rcParentWnd.bottom - m_rcParentWnd.top + m_rcBorder.top + m_rcBorder.bottom;
	HDC hDC = GetDC(m_hWnd);
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, m_szShadowWnd.cx, m_szShadowWnd.cy);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hMemBmp);
	RECT rcPaint;
	rcPaint.left = 0;
	rcPaint.right = m_szShadowWnd.cx;
	rcPaint.top = 0;
	rcPaint.bottom = m_szShadowWnd.cy;
	CRenderEngine::DrawImageString(hMemDC, m_pManager, rcPaint, rcPaint, m_szBkImage, NULL);
	POINT ptDst = {m_rcParentWnd.left - m_rcBorder.left, m_rcParentWnd.top - m_rcBorder.top};
	POINT ptSrc = {0, 0};
	BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	MoveWindow(m_hWnd, ptDst.x, ptDst.y, m_szShadowWnd.cx, m_szShadowWnd.cy, FALSE);
	BOOL bRet = ::UpdateLayeredWindow(m_hWnd, NULL, &ptDst, &m_szShadowWnd, hMemDC, &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
	SelectObject(hMemDC, hOldBmp);
	DeleteDC(hMemDC);
	DeleteObject(hMemBmp);
	ReleaseDC(m_hWnd, hDC);
	ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
}

void CWndShadow::Show(HWND hParentWnd)
{
	// 清空aero特效属性
	m_Status &= SS_ENABLED | SS_DISABLEDBYAERO;

	if ( (m_Status & SS_ENABLED) && !(m_Status & SS_DISABLEDBYAERO) )	//只有不可用状态的时候才去获取窗口样式
	{
		// 得到窗口的样式
		LONG lParentStyle = GetWindowLong(hParentWnd, GWL_STYLE);

		if ( WS_VISIBLE & lParentStyle )	// 如果父窗口是显示的 那么继续判断 窗口是否最小化和最大化 设置阴影窗口是否显示
		{
			m_Status |= SS_PARENTVISIBLE;

			if ( !((WS_MAXIMIZE | WS_MINIMIZE) & lParentStyle) )	//最大化和最小化的时候 不需要现实阴影窗口
				m_Status |= SS_VISABLE;
		}
	}

	if ( m_Status & SS_VISABLE )
	{
		if( m_bPrepared )
			ShowWindow(m_hWnd, SW_SHOWNA);
		Update();
	}
	else
		ShowWindow(m_hWnd, SW_HIDE);
}

void CWndShadow::Destroy()
{
	RemoveProp(m_hWnd, g_pPropName);
	DestroyWindow(m_hWnd);
}

}