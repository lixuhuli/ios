#pragma once



namespace PublicLib
{
	wstring	SizeToString(__int64 nSize);
	wstring SpeedToString(int nSpeed);
	BOOL ShowOnTaskbar(HWND hWnd, BOOL bShow);
	wstring EncryptStr(const wstring &strSrc);
	wstring DecryptStr(const wstring &strSrc);
}