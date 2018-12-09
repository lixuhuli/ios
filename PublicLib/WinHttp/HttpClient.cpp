#include "StdAfx.h"
#include "HttpClient.h"
#pragma comment(lib, "Winhttp")
#include "../IEProxy/IEproxy.h"
#include "StrHelper/StrHelper.h"
#include <assert.h>

namespace PublicLib{
#define HTTP_READBUF_LEN	1024*1024		//1MµÄ½ÓÊÕ»º³åÇø

	CHttpClient::CHttpClient(HttpContentType type/* = ContentDefault*/)
		:m_hInternet(NULL)
		, m_hConnect(NULL)
		, m_hRequest(NULL)
		, m_pLoadCallback(NULL)
		, m_lpParam(NULL)
		, m_nConnTimeout(5000)
		, m_nSendTimeout(5000)
		, m_nRecvTimeout(5000)
		, m_pStopCallback(NULL)
		, m_contentType(type)
	{
		Init();
	}

	CHttpClient::~CHttpClient(void)
	{
		Release();
	}

	bool CHttpClient::Init()
	{
		m_hInternet = ::WinHttpOpen(
			L"Microsoft Internet Explorer",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS,
			0);
		if (NULL == m_hInternet)
		{
			SetError(L"WinHttpOpen³õÊ¼»¯Ê§°Ü");
			return false;
		}
		::WinHttpSetTimeouts(m_hInternet, 0, m_nConnTimeout, m_nSendTimeout, m_nRecvTimeout);
		return true;
	}

	bool CHttpClient::InitConnect(LPCTSTR lpUrl, RequestType type, const char* lpPostData/*=NULL*/, DWORD dwDataSize/*=0*/)
	{
		Release();
		if (!Init())
			return false;
		wstring strHostName, strPage;
		WORD wPort;
		MyParseUrlW(wstring(lpUrl), strHostName, strPage, wPort);
		wstring str = L"http://" + strHostName;
		wstring strProxyBass;
		wstring strProxy = GetIEProxy(str, strProxyBass);
		if (!strProxy.empty())
		{
			WINHTTP_PROXY_INFO proxyInfo;
			memset(&proxyInfo, 0, sizeof(proxyInfo));
			proxyInfo.dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
			proxyInfo.lpszProxy = const_cast<wchar_t*>(strProxy.c_str());
			//proxyInfo.lpszProxyBypass = strProxy.c_str();
			// Set the proxy information for this session.
			 WinHttpSetOption(m_hInternet, WINHTTP_OPTION_PROXY, &proxyInfo, sizeof(proxyInfo));
		}
		if (!ConnectHttpServer(strHostName.c_str(), wPort))
		{
			SetError(L"ConnectHttpServerÊ§°Ü");
			return false;
		}
		if (!CreateHttpRequest(strPage.c_str(), type))
		{
			SetError(L"CreateHttpRequestÊ§°Ü");
			return false;
		}
		if (!SendHttpRequest(lpPostData, dwDataSize))
		{
			SetError(L"SendHttpRequestÊ§°Ü");
			return false;
		}
		if (!WinHttpReceiveResponse(m_hRequest, NULL))
		{
			SetError(L"WinHttpReceiveResponseÊ§°Ü");
			return false;
		}
		return true;
	}

	void CHttpClient::Release()
	{
		CloseInternetHandle(&m_hRequest);
		CloseInternetHandle(&m_hConnect);
		CloseInternetHandle(&m_hInternet);
	}

	bool CHttpClient::ConnectHttpServer(LPCTSTR lpIP, WORD wPort)
	{
		m_hConnect = ::WinHttpConnect(m_hInternet, lpIP, wPort, 0);
		return m_hConnect != NULL;
	}

	bool CHttpClient::CreateHttpRequest(LPCTSTR lpPage, RequestType type, DWORD dwFlag/*=0*/)
	{
		wchar_t* pVerb = (type == Get) ? L"GET" : L"POST";
		m_hRequest = ::WinHttpOpenRequest(
			m_hConnect,
			pVerb,
			lpPage,
			NULL,
			WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			dwFlag);
		return m_hRequest != NULL;
	}

	void CHttpClient::SetTimeOut(int dwConnectTime, int dwSendTime, int dwRecvTime)
	{
		m_nConnTimeout = dwConnectTime;
		m_nSendTimeout = dwSendTime;
		m_nRecvTimeout = dwRecvTime;
	}

	bool CHttpClient::DownLoadFile(LPCTSTR lpUrl, LPCTSTR lpFilePath)
	{
		Release();
		if (!Init())
			return false;
		bool bRet = false;
		DWORD dwBytesToRead = 0, dwFileSize = 0, dwReadSize = 0, dwRecvSize = 0;
		if (!InitConnect(lpUrl, Get))
			return false;
		if (!QueryContentLength(dwFileSize))
		{
			SetError(L"QueryContentLength²éÑ¯HTTP³¤¶ÈÊ§°Ü");
			return false;
		}
		wstring strHeaders;
		bool bQuery = QueryRawHeaders(strHeaders);
		if (bQuery && (strHeaders.find(L"404") != wstring::npos))
		{
			SetError(L"QueryRawHeaders404´íÎó");
			return bRet;
		}
		HANDLE hFile = CreateFile(lpFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			OutputDebugString(L"´´½¨ÏÂÔØÎÄ¼þÊ§°Ü£¡\n");
			return bRet;
		}
		SetFilePointer(hFile, dwFileSize, 0, FILE_BEGIN);
		SetEndOfFile(hFile);
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		if (!::WinHttpQueryDataAvailable(m_hRequest, &dwBytesToRead))
		{
			CloseHandle(hFile);
			DeleteFile(lpFilePath);
			return bRet;
		}
		if (m_pStopCallback && m_pStopCallback->IsNeedStop())
		{
			CloseHandle(hFile);
			DeleteFile(lpFilePath);
			return bRet;
		}
		void* lpBuff = malloc(HTTP_READBUF_LEN);
		while (true)
		{
			if (m_pStopCallback && m_pStopCallback->IsNeedStop())
				break;
			if (dwBytesToRead > HTTP_READBUF_LEN)
			{
				free(lpBuff);
				lpBuff = malloc(dwBytesToRead);
			}
			if (!::WinHttpReadData(m_hRequest, lpBuff, dwBytesToRead, &dwReadSize))
				break;
			if (m_pStopCallback && m_pStopCallback->IsNeedStop())
				break;
			DWORD dwWriteByte;
			if (!WriteFile(hFile, lpBuff, dwReadSize, &dwWriteByte, NULL) || (dwReadSize != dwWriteByte))
				break;
			dwRecvSize += dwReadSize;
			if (m_pLoadCallback)
				m_pLoadCallback->OnHttpLoading(dwFileSize, dwRecvSize, m_lpParam);
			if (!::WinHttpQueryDataAvailable(m_hRequest, &dwBytesToRead))
				break;
			if (dwBytesToRead <= 0)
			{
				bRet = true;
				break;
			}
		}
		free(lpBuff);
		CloseHandle(hFile);
		if (!bRet)
		{//ÏÂÔØÊ§°Ü£¬É¾³ýÎÄ¼þ
			DeleteFile(lpFilePath);
		}
		return bRet;
	}

	bool CHttpClient::DownLoadMem(LPCTSTR lpUrl, OUT void** lpBuffer, OUT DWORD& dwSize)
	{
		bool bResult = false;
		if (!InitConnect(lpUrl, Get))
			return false;
		DWORD dwLength = 0, dwBytesToRead = 0, dwReadSize = 0, dwRecvSize = 0;
		if (!QueryContentLength(dwLength))
		{
			SetError(L"QueryContentLength²éÑ¯HTTP³¤¶ÈÊ§°Ü");
			return false;
		}
		wstring strHeaders;
		bool bQuery = QueryRawHeaders(strHeaders);
		if (bQuery && (strHeaders.find(L"404") != wstring::npos))
		{
			SetError(L"QueryRawHeaders404´íÎó");
			return false;
		}
		if (!::WinHttpQueryDataAvailable(m_hRequest, &dwBytesToRead))
			return false;
		BYTE* lpData = (BYTE*)malloc(dwLength);
		void* lpBuff = malloc(HTTP_READBUF_LEN);
		while (true)
		{
			if (dwBytesToRead > HTTP_READBUF_LEN)
			{
				free(lpBuff);
				lpBuff = malloc(dwBytesToRead);
			}
			if (!::WinHttpReadData(m_hRequest, lpBuff, dwBytesToRead, &dwReadSize))
				break;
			memcpy(lpData + dwRecvSize, lpBuff, dwReadSize);
			dwRecvSize += dwReadSize;
			if (!::WinHttpQueryDataAvailable(m_hRequest, &dwBytesToRead))
				break;
			if (dwBytesToRead <= 0)
			{
				bResult = true;
				break;
			}
		}
		free(lpBuff);
		if (bResult)
		{
			*lpBuffer = lpData;
			dwSize = dwRecvSize;
		}
		else
			free(lpData);
		return bResult;
	}

	string CHttpClient::Request(LPCTSTR lpUrl, RequestType type, const char* lpPostData/* = NULL*/, DWORD dwDataSize/*=0*/)
	{
		string strRet;
		if (!InitConnect(lpUrl, type, lpPostData, dwDataSize))
			return strRet;
		DWORD dwBytesToRead, dwReadSize;
		void* lpBuff = malloc(HTTP_READBUF_LEN);
		bool bFinish = false;
		while (true)
		{
			if (!::WinHttpQueryDataAvailable(m_hRequest, &dwBytesToRead))
				break;
			if (dwBytesToRead <= 0)
			{
				bFinish = true;
				break;
			}
			if (dwBytesToRead > HTTP_READBUF_LEN)
			{
				free(lpBuff);
				lpBuff = malloc(dwBytesToRead);
			}
			if (!::WinHttpReadData(m_hRequest, lpBuff, dwBytesToRead, &dwReadSize))
				break;
			strRet.append((const char*)lpBuff, dwReadSize);
		}
		free(lpBuff);
		if (!bFinish)
			strRet.clear();
		return strRet;
	}

	bool CHttpClient::DownloadFileForRetry(LPCTSTR lpUrl, const wstring& strFilePath, int nTryCount /*= 3*/)
	{
		assert(nTryCount > 0);
		bool bRet = false;
		while (nTryCount > 0)
		{
			if (DownLoadFile(lpUrl, strFilePath.c_str()))
			{
				bRet = true;
				break;
			}
			//DWORD dwErrCode = GetLastError();
			//if ( ERROR_WINHTTP_TIMEOUT == GetLastError() )
			nTryCount--;
			Sleep(1000);	//ÐÝÃß1ÃëÖÓÔÙ³¢ÊÔ
		}
		return bRet;
	}

	void CHttpClient::AddHttpHeader(LPCTSTR lpKey, LPCTSTR lpValue)
	{
		m_mapHeaders[lpKey] = lpValue;
	}

	void CHttpClient::ClearHttpHeaders()
	{
		m_mapHeaders.clear();
	}

	bool CHttpClient::QueryRawHeaders(OUT wstring& strHeaders)
	{
		bool bRet = false;
		DWORD dwSize;
		BOOL bResult = ::WinHttpQueryHeaders(m_hRequest, WINHTTP_QUERY_RAW_HEADERS, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &dwSize, WINHTTP_NO_HEADER_INDEX);
		if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
		{
			wchar_t* lpData = (wchar_t*)malloc(dwSize);
			bResult = ::WinHttpQueryHeaders(m_hRequest, WINHTTP_QUERY_RAW_HEADERS, WINHTTP_HEADER_NAME_BY_INDEX, lpData, &dwSize, WINHTTP_NO_HEADER_INDEX);
			if (bResult)
			{
				strHeaders = lpData;
				bRet = true;
			}
			free(lpData);
		}
		return bRet;
	}

	bool CHttpClient::QueryContentLength(OUT DWORD& dwLength)
	{
		bool bRet = false;
		wchar_t szBuffer[30] = { 0 };
		DWORD dwSize = 30 * sizeof(wchar_t);
		if (::WinHttpQueryHeaders(m_hRequest, WINHTTP_QUERY_CONTENT_LENGTH, WINHTTP_HEADER_NAME_BY_INDEX, szBuffer, &dwSize, WINHTTP_NO_HEADER_INDEX))
		{
			TCHAR *p = NULL;
			dwLength = wcstoul(szBuffer, &p, 10);
			bRet = true;
		}
		return bRet;
	}

	bool CHttpClient::SendHttpRequest(const char* lpPostData/*=NULL*/, DWORD dwSize/* = 0*/)
	{
		BOOL bRet = WinHttpAddRequestHeaders(m_hRequest, L"User-Agent:Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.75 Safari/537.36\r\n",
			-1L, WINHTTP_ADDREQ_FLAG_ADD_IF_NEW);
		LPTSTR lpContentType = NULL;
		switch (m_contentType)
		{
		case PublicLib::ContentDefault: lpContentType = L"Content-Type:application/x-www-form-urlencoded; charset=UTF-8\r\n"; break;
		case PublicLib::ContentJson: lpContentType = L"Content-Type: application/json"; break;
		case PublicLib::ContentFile:
		default:break;
		}
		if (lpContentType)
			WinHttpAddRequestHeaders(m_hRequest, lpContentType, -1L, WINHTTP_ADDREQ_FLAG_ADD_IF_NEW);
		for (auto itor = m_mapHeaders.begin(); itor != m_mapHeaders.end(); ++itor)
		{
			wstring strHeader(itor->first);
			strHeader.append(L":");
			strHeader.append(itor->second);
			strHeader.append(L"\r\n");
			WinHttpAddRequestHeaders(m_hRequest, strHeader.c_str(), -1L, WINHTTP_ADDREQ_FLAG_ADD);
		}
		if (dwSize == 0 && lpPostData != NULL)
		{
			dwSize = strlen(lpPostData);
		}
		return ::WinHttpSendRequest(m_hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, (LPVOID)lpPostData, dwSize, dwSize, NULL) == TRUE;
	}

}

