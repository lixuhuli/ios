#pragma once
#include "stdafx.h"
#include "CommonWnd.h"
#include "WndMsg.h"
#include "WndToast.h"

UINT ShowMsg(HWND hWnd, const wstring& strTitle, const wstring& strText, DWORD dwType){
	CWndMsg* pWnd = new CWndMsg(strTitle, strText, dwType);
	HWND hWndMsg = pWnd->CreateModalWnd(hWnd);
	return CWndBase::ShowModalWnd(hWndMsg);
}

void ShowToast(HWND hWnd, LPCWSTR lpText) {
    CWndToast* pWnd = new CWndToast(lpText);
    HWND hToastWnd = pWnd->Create(hWnd);
    pWnd->CenterWindow();
    ::SetWindowPos(hToastWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
}