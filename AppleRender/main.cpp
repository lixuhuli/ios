#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
#include <SDKDDKVer.h>
#include <Windows.h>
#include "CefBase/util.h"
#include "CefBase/CefAppEx.h"
#include "CefBase/client_app.h"
#include "CefBase/client_app_browser.h"
#include "CefBase/client_app_other.h"
#include "JsBind/JsBind.h"




int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CefMainArgs main_args(hInstance);
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromString(::GetCommandLineW());
	CefRefPtr<CefApp> app;
	ClientApp::ProcessType process_type = ClientApp::GetProcessType(command_line);
	void* sandbox_info = NULL;
	if (process_type == ClientApp::RendererProcess)
	{
		CCefAppEx* pApp = new CCefAppEx();
		CMyJsBindCallback jsCallback;
		pApp->SetJsCallback(&jsCallback);
		app = pApp;
		return CefExecuteProcess(main_args, app, sandbox_info);
	}
	if (process_type == ClientApp::BrowserProcess)
	{
		app = new ClientAppBrowser();
	}
	else if (process_type == ClientApp::OtherProcess)
	{
		app = new ClientAppOther();
	}
	int exit_code = CefExecuteProcess(main_args, app, sandbox_info);
	return exit_code;
}