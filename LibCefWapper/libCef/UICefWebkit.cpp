#include "StdAfx.h"
#include "UICefWebkit.h"
#include "include\cef_runnable.h"



CCefWebkitUI::CCefWebkitUI(LPCTSTR lpUrl /*= L""*/)
: m_strUrl(lpUrl)
, m_lpCallbackData(NULL)
, m_pCallback(NULL)
, m_bLoadError(false)
{
	m_cefHandles = new CCefHandler(this, "");
}

CCefWebkitUI::~CCefWebkitUI()
{

}

LPCTSTR CCefWebkitUI::GetClass() const
{
	return L"CefWebkitUI";
}

LPVOID CCefWebkitUI::GetInterface( LPCTSTR pstrName )
{
	if ( _tcscmp(pstrName, L"CefWebkit") == 0 )
		return static_cast<CCefWebkitUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

void CCefWebkitUI::SetPos( RECT rc )
{
	CControlUI::SetPos(rc);
	HWND hWnd = GetHostWnd();
	if (hWnd && IsWindow(hWnd)){
		int nWidth = rc.right - rc.left;
		int nHeight = rc.bottom - rc.top;
        if (nWidth == 0 || nHeight == 0) return;
		::MoveWindow(hWnd, rc.left, rc.top, nWidth, nHeight, TRUE);
	}
}

void CCefWebkitUI::DoInit()
{
	RECT rc = {0, 0, 0, 0};
	m_cefHandles->CreateBrowser(m_pManager->GetPaintWindow(), rc, m_strUrl.c_str());
}

void CCefWebkitUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	if ( _tcscmp(pstrName, _T("url")) == 0 )
		m_strUrl = pstrValue;
	else
		CControlUI::SetAttribute(pstrName, pstrValue);
}

void CCefWebkitUI::SetInternVisible( bool bVisible /*= true*/ )
{
	CControlUI::SetInternVisible(bVisible);
	HWND hWnd = GetHostWnd();
	if (hWnd)
		::ShowWindow(hWnd, bVisible?SW_SHOW:SW_HIDE);
}

void CCefWebkitUI::NeedUpdate()
{
	m_bUpdateNeeded = false;
}

void CCefWebkitUI::Invalidate()
{
	m_bUpdateNeeded = false;
}

bool CCefWebkitUI::IsUpdateNeeded() const
{
	return false;
}

HWND CCefWebkitUI::GetHostWnd()
{
	if (m_pWebBrowser.get())
	{
		CefRefPtr<CefBrowserHost> pHost = m_pWebBrowser->GetHost();
		if (pHost.get())
			return pHost->GetWindowHandle();
	}
	return NULL;
}

wstring CCefWebkitUI::GetLoadingUrl()
{
	if (m_pWebBrowser.get())
	{
		CefString strUrl = m_pWebBrowser->GetMainFrame()->GetURL();
		if (strUrl.empty())
			return m_strUrl;
		return strUrl.c_str();
	}
	return L"";
}

void CCefWebkitUI::Close(bool bForce/* = false*/)
{
	if (m_pWebBrowser.get())
	{
		CefRefPtr<CefBrowserHost> pHost = m_pWebBrowser->GetHost();
		if (pHost.get())
			pHost->CloseBrowser(bForce);
	}
}

void CCefWebkitUI::SetWebloadCallback( CCefWebCallback* pCallback, void* lpParam )
{
	m_lpCallbackData = lpParam;
	m_pCallback = pCallback;
}

CefRefPtr<CefBrowser> CCefWebkitUI::GetBrowser()
{
	return m_pWebBrowser;
}

void CCefWebkitUI::ExecuteJavascript( const wstring& strCode )
{
	if (m_pWebBrowser.get())
	{
		CefRefPtr<CefFrame> frame = m_pWebBrowser->GetMainFrame();
		if (frame.get())
		{
			CefString strCode(strCode.c_str()), strUrl(L"");
			frame->ExecuteJavaScript(strCode, strUrl, 0);
		}
	}
}

void CCefWebkitUI::Navigate(LPCTSTR lpUrl)
{
	if (!m_pWebBrowser.get() || (NULL == lpUrl))
		return ;
	CefRefPtr<CefFrame> pFrame = m_pWebBrowser->GetMainFrame();
	if ((_tcslen(lpUrl) > 0) && pFrame.get())
	{
		pFrame->LoadURL(lpUrl);
		m_strUrl = lpUrl;
	}
}

CefRefPtr<CefFrame> CCefWebkitUI::GetMainFrame()
{
	if (m_pWebBrowser.get())
	{
		return m_pWebBrowser->GetMainFrame();
	}
	return NULL;
}

void CCefWebkitUI::Reload()
{
	if (m_pWebBrowser.get())
	{
		m_pWebBrowser->Reload();
	}
}

void CCefWebkitUI::Refresh()
{
	if (m_pWebBrowser.get())
	{
		m_pWebBrowser->ReloadIgnoreCache();
	}
}

void CCefWebkitUI::GoBack()
{
	if (m_pWebBrowser.get() && m_pWebBrowser->CanGoBack())
		m_pWebBrowser->GoBack();
}

void CCefWebkitUI::GoForward()
{
	if (m_pWebBrowser.get() && m_pWebBrowser->CanGoForward())
		m_pWebBrowser->GoForward();
}

void CCefWebkitUI::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRePaint)
{
	HWND hWnd = GetHostWnd();
	if (hWnd && IsWindow(hWnd))
		::MoveWindow(hWnd, x, y, nWidth, nHeight, bRePaint);
}

void CCefWebkitUI::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
	m_pWebBrowser = browser;
}

void CCefWebkitUI::OnSetFullscreen(bool fullscreen)
{

}

void CCefWebkitUI::OnSetLoadingState(bool isLoading, bool canGoBack, bool canGoForward)
{
	if (m_pCallback)
		m_pCallback->OnSetLoadingState(this, m_lpCallbackData, isLoading, canGoBack, canGoForward);
}

void CCefWebkitUI::OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions)
{

}

bool CCefWebkitUI::OnOpenNewUrl(const std::wstring& url)
{
	if (m_pCallback)
		return m_pCallback->OnOpenNewUrl(this, m_lpCallbackData, url);
	return false;
}

void CCefWebkitUI::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
	if (m_pCallback)
		m_pCallback->OnLoadStart(this, m_lpCallbackData);
}

void CCefWebkitUI::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int nHttpCode)
{
	if (m_pCallback)
		m_pCallback->OnLoadEnd(this, m_lpCallbackData, browser, m_bLoadError);
	m_bLoadError = false;
}

void CCefWebkitUI::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int errorCode, const std::wstring& errorText, const std::wstring& failedUrl)
{
	if (errorCode == ERR_ABORTED)
		return;
	m_bLoadError = true;
	if (m_pCallback)
		m_pCallback->OnLoadError(this, m_lpCallbackData, frame);
}

bool CCefWebkitUI::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, const std::wstring& url)
{
	if (m_pCallback)
		return m_pCallback->OnBeforeBrowse(this, m_lpCallbackData, browser, url);
	return false;
}

bool CCefWebkitUI::CanGoBack()
{
	if (m_pWebBrowser.get())
		return m_pWebBrowser->CanGoBack();
	return false; 
}

bool CCefWebkitUI::CanGoForward()
{
	if (m_pWebBrowser.get())
		return m_pWebBrowser->CanGoForward();
	return false;
}