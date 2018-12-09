#include "stdafx.h"
#include "Common.h"
#include <Shobjidl.h>

namespace PublicLib{
	wstring SizeToString(__int64 nSize)
	{
		if (nSize < 10)
			return L"0 K";
		double fValue = 0.0;
		wstring strExt;
		wchar_t szText[65];
		if (nSize < 1024)
		{
			swprintf_s(szText, L"%d B", nSize);
			return wstring(szText);
		}
		if (nSize < 1024 * 1024)
		{
			int nValue = (int)(nSize / 1024);
			swprintf_s(szText, L"%d K", nValue);
			return wstring(szText);
		}
		if (nSize < 1024 * 1024 * 1024)
		{
			fValue = nSize / (1024.0*1024.0);
			strExt = L" M";
		}
		else
		{
			fValue = nSize / (1024.0*1024.0*1024.0);
			strExt = L" G";
		}
		swprintf_s(szText, L"%0.1f%s", fValue, strExt.c_str());
		return wstring(szText);
	}

	wstring SpeedToString(int nSpeed)
	{
		if (nSpeed <= 0)
			return L"0 K/S";
		double fValue = 0.0;
		wstring strExt;
		wchar_t szText[65] = { 0 };
		if (nSpeed < 1024)
		{
			swprintf_s(szText, L"%d K/S", nSpeed);
			return wstring(szText);
		}
		if (nSpeed < 1024 * 1024)
		{
			fValue = nSpeed / 1024.0;
			strExt = L" M/S";
		}
		else
		{
			fValue = nSpeed / (1024.0*1024.0);
			strExt = L" G/S";
		}
		swprintf_s(szText, L"%0.2f%s", fValue, strExt.c_str());
		return wstring(szText);
	}

	BOOL ShowOnTaskbar(HWND hWnd, BOOL bShow)
	{
		HRESULT hr;
		ITaskbarList* pTaskbarList;
		hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList, (void**)&pTaskbarList);
		if (SUCCEEDED(hr))
		{
			pTaskbarList->HrInit();
			if (bShow)
				pTaskbarList->AddTab(hWnd);
			else
				pTaskbarList->DeleteTab(hWnd);
			pTaskbarList->Release();
			return TRUE;
		}
		return FALSE;
	}

	//¼ÓÃÜ×Ö·û´®
	wstring EncryptStr(const wstring &SrcStr)
	{
		if (SrcStr.empty()) return SrcStr;
		wstring   Result, str;
		int   i, j;
		int   ikey = 0;
		BYTE   bM;
		Result = SrcStr;
		if (SrcStr.empty())
		{
			return L"";
		}
		BYTE   KEY[] = { 0x01, 0x2E, 0x6e, 0x6d };
		for (i = 0; i < (int)SrcStr.length(); i++)
		{
			bM = KEY[ikey] + SrcStr[i];
			if (bM>255)
			{
				bM = (BYTE)((int)bM - 256);
			}
			Result[i] = bM;
			ikey++;
			if (ikey >= 4)ikey = 0;
		}
		wstring strTemp = Result;
		Result = _T("");
		for (i = 0; i < (int)strTemp.length(); i++)
		{
			j = (BYTE)strTemp[i];
			str = L"11";
			str[0] = (65 + j / 26);
			str[1] = (65 + j % 26);
			Result += str;
		}
		return  Result;
	}

	//½âÃÜ×Ö·û´®
	wstring DecryptStr(const wstring &SrcStr)
	{
		if (SrcStr.empty()) return SrcStr;
		wstring   Result, str;
		int   i, j;
		int   ikey = 0;
		BYTE   bM;
		if (SrcStr.empty())
		{
			return L"";
		}
		BYTE   KEY[] = { 0x01, 0x2E, 0x6e, 0x6d };
		Result = L"";

		for (i = 0; i < (int)SrcStr.length() / 2; i++)
		{
			j = ((BYTE)SrcStr[2 * i] - 65) * 26;
			j += (BYTE)SrcStr[2 * i + 1] - 65;
			str = L"1";
			str[0] = j;
			Result += str;
		}
		wstring strTemp = Result;
		for (i = 0; i < (int)strTemp.length(); i++)
		{
			bM = (BYTE)strTemp[i];
			if (bM >= KEY[ikey]){
				bM = bM - KEY[ikey];
			}
			else
				bM = 256 + bM - KEY[ikey];
			Result[i] = bM;
			ikey++;
			if (ikey >= 4)ikey = 0;
		}
		return Result;
	}

}//namespace