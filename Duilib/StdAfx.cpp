// stdafx.cpp : source file that includes just the standard includes
//	UIlib.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "StdAfx.h"


#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "comctl32.lib" )

bool SplitString(const wstring& strSource, wchar_t flag, vector<wstring>& paramList)
{
    if (strSource.empty())
        return false;
    paramList.clear();
    size_t nBeg = 0;
    size_t nFind = strSource.find(flag, nBeg);
    if (nFind == std::wstring::npos)
        paramList.push_back(strSource);
    else
    {
        while (true)
        {
            if (nFind != nBeg)
                paramList.push_back(strSource.substr(nBeg, nFind - nBeg));
            nBeg = nFind + 1;
            if (nBeg == strSource.size())
                break;
            nFind = strSource.find(flag, nBeg);
            if (nFind == std::wstring::npos)
            {
                paramList.push_back(wstring(strSource.begin() + nBeg, strSource.end()));
                break;
            }
        }
    }
    return true;
}

void SplitStringAlongWhitespace(const wstring& str, std::vector<wstring>* result) {
    result->clear();
    const size_t length = str.length();
    if (!length)
        return;

    bool last_was_ws = false;
    size_t last_non_ws_start = 0;
    for (size_t i = 0; i < length; ++i) {
        switch (str[i]) {
            // HTML 5 defines whitespace as: space, tab, LF, line tab, FF, or CR.
        case L' ':
        case L'\t':
        case L'\xA':
        case L'\xB':
        case L'\xC':
        case L'\xD':
            if (!last_was_ws) {
                if (i > 0) {
                    result->push_back(
                        str.substr(last_non_ws_start, i - last_non_ws_start));
                }
                last_was_ws = true;
            }
            break;

        default:  // Not a space character.
            if (last_was_ws) {
                last_was_ws = false;
                last_non_ws_start = i;
            }
            break;
        }
    }
    if (!last_was_ws) {
        result->push_back(
            str.substr(last_non_ws_start, length - last_non_ws_start));
    }
}