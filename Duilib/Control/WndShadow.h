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

#pragma once

namespace DuiLib
{
typedef BOOL	(WINAPI *pfnUpdateLayeredWindow)(HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags);
class UILIB_API CWndShadow
{
	enum ShadowStatus
	{
		SS_ENABLED = 1,	// Shadow is enabled, if not, the following one is always false
		SS_VISABLE = 1 << 1,	// Shadow window is visible
		SS_PARENTVISIBLE = 1 << 2,	// Parent window is visible, if not, the above one is always false
		SS_DISABLEDBYAERO = 1 << 3	// Shadow is enabled, but do not show because areo is enabled
	};
public:
	CWndShadow(void);
	virtual ~CWndShadow(void);

	static bool Initialize(HINSTANCE hInstance);
	void Create(HWND hParentWnd ,CPaintManagerUI* pManager = NULL);
	void SetPrepared();
	void SetBkImage(const TCHAR* pBkImage);
	void SetBorder(RECT rcBorder);
	HWND GetHwnd() { return m_hWnd; }

protected:
	static LRESULT CALLBACK ParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Update();
	void Show(HWND hParent);
	void Destroy();

private:
	RECT	m_rcParentWnd;
	SIZE	m_szShadowWnd;
	LONG	m_OriParentProc;
	BYTE	m_Status;
	TCHAR	m_szBkImage[MAX_PATH + 1];
	CPaintManagerUI* m_pManager;
	bool m_bPrepared;
	RECT m_rcBorder;
	HWND m_hWnd;
	HWND m_hParentWnd;
	static HINSTANCE s_hInstance;
};

}