#include "stdafx.h"
#include "IEproxy.h"
#include <atlstr.h>
#include <winhttp.h>
#pragma comment(lib, "Winhttp.lib")

namespace PublicLib{


	std::wstring GetIEProxy(const std::wstring& strUrl, std::wstring& strProxyBypass)
	{
		CString strTempUrl = strUrl.c_str();
		strTempUrl.MakeLower();
		if (0 != strTempUrl.Find(L"http"))
		{
			strTempUrl = "http://";
			strTempUrl += strUrl.c_str();
		}
		std::wstring strProxy;
		HINTERNET hSession = NULL;
		DWORD dwError = 0;

		WINHTTP_CURRENT_USER_IE_PROXY_CONFIG ieProxyConfig = { 0 };
		do
		{
			BOOL bAutoDetect = FALSE; //“自动检测设置”，但有时候即便选择上也会返回0，所以需要根据url判断
			//获取IE代理设置
			if (!::WinHttpGetIEProxyConfigForCurrentUser(&ieProxyConfig))
			{
				break;
			}

			if (ieProxyConfig.fAutoDetect || ieProxyConfig.lpszAutoConfigUrl != NULL)
			{
				bAutoDetect = TRUE;
			}

			if (bAutoDetect)
			{
				WINHTTP_AUTOPROXY_OPTIONS autoProxyOptions = { 0 };
				autoProxyOptions.lpszAutoConfigUrl = ieProxyConfig.lpszAutoConfigUrl;
				if (autoProxyOptions.lpszAutoConfigUrl != NULL)
				{
					autoProxyOptions.dwFlags = WINHTTP_AUTOPROXY_CONFIG_URL;
				}
				else
				{
					autoProxyOptions.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
					autoProxyOptions.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP | WINHTTP_AUTO_DETECT_TYPE_DNS_A;
				}
				autoProxyOptions.fAutoLogonIfChallenged = TRUE;

				hSession = ::WinHttpOpen(L"WinHttp", WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC);
				if (NULL == hSession)
				{
					break;
				}

				WINHTTP_PROXY_INFO autoProxyInfo = { 0 };
				try
				{
					bAutoDetect = ::WinHttpGetProxyForUrl(hSession, strTempUrl.GetString(), &autoProxyOptions, &autoProxyInfo);
				}
				catch (...)
				{
					bAutoDetect = FALSE;
				}

				if (!bAutoDetect)
				{
					dwError = GetLastError();
				}

				if (autoProxyInfo.lpszProxy != NULL)
				{
					strProxy = autoProxyInfo.lpszProxy;
					GlobalFree(autoProxyInfo.lpszProxy);
				}
				if (autoProxyInfo.lpszProxyBypass != NULL)
				{
					strProxyBypass = autoProxyInfo.lpszProxyBypass;
					GlobalFree(autoProxyInfo.lpszProxyBypass);
				}
			}
			else
			{
				if (NULL != ieProxyConfig.lpszProxy)
				{
					strProxy = ieProxyConfig.lpszProxy;
				}
				if (NULL != ieProxyConfig.lpszProxyBypass)
				{
					strProxyBypass = ieProxyConfig.lpszProxyBypass;
				}
			}

		} while (false);


		if (ieProxyConfig.lpszAutoConfigUrl != NULL)
		{
			::GlobalFree(ieProxyConfig.lpszAutoConfigUrl);
		}
		if (ieProxyConfig.lpszProxy != NULL)
		{
			::GlobalFree(ieProxyConfig.lpszProxy);
		}
		if (ieProxyConfig.lpszProxyBypass != NULL)
		{
			::GlobalFree(ieProxyConfig.lpszProxyBypass);
		}
		if (NULL != hSession)
		{
			WinHttpCloseHandle(hSession);
		}
		return strProxy;
	}

    bool GetProxyIpAndPort(const string& strProxy, std::string& strIp, int& nPort) {
        if (strProxy.empty()) return false;
        auto pos_start = strProxy.find("=");
        if (pos_start == string::npos) return false;

        auto pos = strProxy.find(":");
        if (pos == string::npos) return false;

        strIp = strProxy.substr(pos_start + 1, pos - pos_start - 1);

        auto pos_end = strProxy.find(";");
        if (pos_end == string::npos) pos_end = strProxy.length();

        nPort = atoi(strProxy.substr(pos + 1, pos_end - pos - 1).c_str());

        return true;
    }

}