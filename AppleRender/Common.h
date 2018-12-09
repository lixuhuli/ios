#pragma once
#include <string>
#include "AppHelper.h"
#include "SoftDefine.h"
using std::wstring;


inline wstring GetMainExeVersion()
{
	wstring strPath = GetRunPathW() + EXE_MAIN;
	std::wstring Version;
	DWORD dwHandle;
	DWORD VerInfoSize = GetFileVersionInfoSize(strPath.c_str(), &dwHandle);
	if (VerInfoSize)
	{
		wchar_t* VerInfo = new wchar_t[VerInfoSize / sizeof(wchar_t)];
		if (GetFileVersionInfo(strPath.c_str(), NULL, VerInfoSize, VerInfo))
		{
			VS_FIXEDFILEINFO* pFileInfo;
			UINT FileInfoLength;
			if (VerQueryValue(VerInfo, L"\\", (LPVOID*)&pFileInfo, &FileInfoLength))
			{
				wchar_t szVersion[100] = { 0 };
				swprintf_s(szVersion, L"%d.%d.%d.%d", HIWORD(pFileInfo->dwProductVersionMS),
					LOWORD(pFileInfo->dwProductVersionMS),
					HIWORD(pFileInfo->dwProductVersionLS),
					LOWORD(pFileInfo->dwProductVersionLS));
				Version = szVersion;
			}
		}
		delete[] VerInfo;
	}
	return Version;
}

inline bool CopyToClipboard(const wstring& strData)
{
	if (strData.empty())
		return false;
	if (!OpenClipboard(NULL))
	{
		DWORD dwError = GetLastError();
		return false;//´ò¿ª¼ôÇÐ°åÊ§°Ü
	}
	EmptyClipboard();
	SIZE_T nSize = strData.size() + 1;
	HGLOBAL hClip = GlobalAlloc(GMEM_DDESHARE, nSize * sizeof(wchar_t));
	PTSTR pszBuf = (PTSTR)GlobalLock(hClip);
	wcscpy_s(pszBuf, nSize, strData.c_str());
	GlobalUnlock(hClip);
	SetClipboardData(CF_UNICODETEXT, hClip);
	CloseClipboard();
	return true;
}