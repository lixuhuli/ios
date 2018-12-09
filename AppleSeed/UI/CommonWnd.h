#ifndef _COMMON_WND_INCLUDE_H_
#define _COMMON_WND_INCLUDE_H_

#pragma once

UINT ShowMsg(HWND hWnd, const wstring& strTitle, const wstring& strText, DWORD dwType);

void ShowToast(HWND hWnd, LPCWSTR lpText);

#endif  // !#define (_COMMON_WND_INCLUDE_H_)