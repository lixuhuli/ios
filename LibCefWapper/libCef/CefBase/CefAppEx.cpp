// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//#include "StdAfx.h"
#include "CefAppEx.h"
#include <string>
#include "CefHandler.h"
#include "util.h"
#include "../include/cef_browser.h"
#include "../include/cef_command_line.h"



CCefAppEx::CCefAppEx() 
: m_pJsCallback(NULL)
{

}

void CCefAppEx::OnContextInitialized() 
{

}

void CCefAppEx::OnContextCreated( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context )
{
	context->Enter();
	if (m_pJsCallback)
		m_pJsCallback->OnInitJavascript(browser, frame, context);
	context->Exit();
}

void CCefAppEx::OnWebKitInitialized()
{
	
}

bool CCefAppEx::OnProcessMessageReceived( CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message )
{
	return false;
}

void CCefAppEx::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{

}
