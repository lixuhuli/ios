// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "client_app_browser.h"
#include "include/base/cef_logging.h"
#include "include/cef_cookie.h"
#include "include/cef_command_line.h"
#include "include/cef_values.h"
#include "client_switches.h"


ClientAppBrowser::ClientAppBrowser() 
{
  
}

void ClientAppBrowser::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) 
{
	// 此参数解决多窗口问题
	// 	command_line->AppendSwitch("process-per-site");
	// 	command_line->AppendSwitch("enable-npapi");

	command_line->AppendSwitch("enable-syste\\m-flash");
	command_line->AppendSwitchWithValue("ppapi-flash-path", "pepflashplayer.dll");
	command_line->AppendSwitchWithValue("ppapi-flash-version", "22.0.0.209");
	//command_line->AppendSwitchWithValue("register-pepper-plugins", "pepflashplayer.dll;application/x-shockwave-flash");

}

void ClientAppBrowser::OnContextInitialized() 
{
  // Register cookieable schemes with the global cookie manager.
  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::GetGlobalManager(NULL);
  DCHECK(manager.get());
  manager->SetSupportedSchemes(cookieable_schemes_, NULL);
}

void ClientAppBrowser::OnBeforeChildProcessLaunch(
      CefRefPtr<CefCommandLine> command_line)
{

}

void ClientAppBrowser::OnRenderProcessThreadCreated(
    CefRefPtr<CefListValue> extra_info) 
{

}
