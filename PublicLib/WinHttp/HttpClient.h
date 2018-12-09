/********************************************************
*Desc：简单封装下WinHttp API接口，提供HTTP请求、下载功能
*Author：Jelinyao
*Date：	2015-5
*/

#pragma once
#include <Windows.h>
#include "Winhttp.h"
#include <string>
#include <map>
using std::string;
using std::wstring;
using std::map;

namespace PublicLib
{
	typedef LPVOID HINTERNET;
	class CHttpClientLoadCallback
	{
	public:
		virtual void	OnHttpLoading(const DWORD dwTotalSize, const DWORD dwLoadSize, void* lpParam) = 0;
	};

	class CHttpClientStopCallback
	{
	public:
		virtual BOOL	IsNeedStop() = 0;
	};

	enum RequestType
	{
		Post,
		Get,
	};

	enum HttpContentType
	{
		ContentDefault = 0,
		ContentJson,
		ContentFile,
	};

	class CHttpClient
	{
	public:
		CHttpClient(HttpContentType type=ContentDefault);
		virtual ~CHttpClient(void);
		bool	Init();
		void	Release();
		bool	DownLoadFile(LPCTSTR lpUrl, LPCTSTR lpFilePath);
		bool	DownLoadMem(LPCTSTR lpUrl, OUT void** lpBuffer, OUT DWORD& dwSize);
		string	Request(LPCTSTR lpUrl, RequestType type, const char* lpPostData = NULL, DWORD dwDataSize=0);
		void	SetLoadCallback(CHttpClientLoadCallback* pCallback, void* lpParam)	{ m_pLoadCallback = pCallback; m_lpParam = lpParam; }
		void	SetTimeOut(int dwConnectTime, int dwSendTime, int dwRecvTime);
		bool	DownloadFileForRetry(LPCTSTR lpUrl, const wstring& strFilePath, int nTryCount = 3);//下载函数，提供超时机制，默认重试3次
		LPCTSTR	GetError()		{ return m_strError.c_str(); }
		void	SetStopCallback(CHttpClientStopCallback* pCallback)	{ m_pStopCallback = pCallback; }
		void	AddHttpHeader(LPCTSTR lpKey, LPCTSTR lpValue);
		void	ClearHttpHeaders();

	protected:
		//init
		bool	InitConnect(LPCTSTR lpUrl, RequestType type, const char* lpPostData = NULL, DWORD dwDataSize=0);
		bool	ConnectHttpServer(LPCTSTR lpIP, WORD wPort);
		bool	CreateHttpRequest(LPCTSTR lpPage, RequestType type, DWORD dwFlag = 0);
		bool	SendHttpRequest(const char* lpPostData = NULL, DWORD dwSize = 0);
		//query 
		bool	QueryRawHeaders(OUT wstring& strHeaders);
		bool	QueryContentLength(OUT DWORD& dwLength);
		//error
		void	SetError(LPCTSTR lpError)
		{
			m_strError = lpError;
			wchar_t szCode[20] = { 0 };
			swprintf_s(szCode, L"系统错误码：%u", GetLastError());
			m_strError.append(szCode);
		}
	private:
		HINTERNET	m_hInternet;
		HINTERNET	m_hConnect;
		HINTERNET	m_hRequest;
		void*		m_lpParam;
		int			m_nConnTimeout;
		int			m_nSendTimeout;
		int			m_nRecvTimeout;
		wstring		m_strError;
		HttpContentType m_contentType;
		CHttpClientLoadCallback*	m_pLoadCallback;
		CHttpClientStopCallback*	m_pStopCallback;
		map<wstring, wstring> m_mapHeaders;

	};

	inline void CloseInternetHandle(HINTERNET* hInternet)
	{
		if (*hInternet)
		{
			WinHttpCloseHandle(*hInternet);
			*hInternet = NULL;
		}
	}
}