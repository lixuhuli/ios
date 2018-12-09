#pragma once
#include "CefBase/util.h"
#include "CefBase/CefHandler.h"
#include "CefBase/CefAppEx.h"
#include "CefBase/CEfV8Handler.h"
#include "CefBase/Delegate.h"

class CCefWebkitUI;
//网页加载状态的回调
class CCefWebCallback
{
public:
	virtual void OnLoadStart(CCefWebkitUI* pWeb, void* lpParam)														= 0;
	virtual void OnLoadEnd(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefBrowser> browser, bool bLoadError)		= 0;
	virtual void OnLoadError(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefFrame> pFrame)							= 0;
	virtual bool OnOpenNewUrl(CCefWebkitUI* pWeb, void* lpParam, const std::wstring& url)							= 0;
	virtual bool OnBeforeBrowse(CCefWebkitUI* pWeb, void* lpParam, CefRefPtr<CefBrowser> browser, const std::wstring& url) = 0;
	virtual void OnSetLoadingState(CCefWebkitUI* pWeb, void* lpParam, bool isLoading, bool canGoBack, bool canGoForward) = 0;
};



class CCefWebkitUI
	: public CControlUI
	, public Delegate
	
{
public:
	CCefWebkitUI(LPCTSTR lpUrl = L"");
	virtual ~CCefWebkitUI();

	HWND	GetHostWnd();
	wstring	GetLoadingUrl();
	void	Close(bool bForce = false);
	void	SetWebloadCallback(CCefWebCallback* pCallback, void* lpParam);
	void	ExecuteJavascript(const wstring& strCode);
	CefRefPtr<CefBrowser>	GetBrowser();
	CefRefPtr<CefFrame>		GetMainFrame();
	bool	CanGoBack();
	bool	CanGoForward();
	void	MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRePaint);
	void	Navigate(LPCTSTR lpUrl);
	void	Refresh();
	void	Reload();
	void	GoBack();
	void	GoForward();

protected:
	virtual LPCTSTR	GetClass() const;
	virtual LPVOID	GetInterface(LPCTSTR pstrName);
	virtual void	SetPos(RECT rc);
	virtual void	DoInit();
	virtual void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual void	SetInternVisible(bool bVisible = true);

	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser);
	virtual void OnSetFullscreen(bool fullscreen);
	virtual void OnSetLoadingState(bool isLoading, bool canGoBack, bool canGoForward);
	virtual void OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions);
	virtual bool OnOpenNewUrl(const std::wstring& url);
	virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame);
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int nHttpCode);
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int errorCode, const std::wstring& errorText, const std::wstring& failedUrl);
	virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, const std::wstring& url);

private:
	bool m_bLoadError;
	wstring	m_strUrl;
	void*	m_lpCallbackData;
	CCefWebCallback	*m_pCallback;
	CefRefPtr<CefBrowser>	m_pWebBrowser;
	CefRefPtr<CCefHandler>	m_cefHandles;
};