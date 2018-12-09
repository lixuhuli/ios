#pragma once


enum XYClient_Process_Type
{
	Process_Type_Main = 0,//主进程
	Process_Type_LibCef_Gpu,//libcef GPU进程
	Process_Type_LibCef_Render,//libcef 渲染进程
	Process_Type_LibCef_Plugin,//libcef插件进程
	Process_Type_LibCef,//libcef其它进程
	Process_Type_Other,//其他类型进程
	Process_Type_Unknow,//其他未知类型进程
};

class CExceptionHandler
{
public:
	static CExceptionHandler* Instance();


	void Init(LPTSTR lpCmdLine);
	const std::wstring& GetProcessCmdLine();
	XYClient_Process_Type GetProcessType();

protected:
	CExceptionHandler(void);
	~CExceptionHandler(void);

	static wstring TransExpCode(DWORD ExceptionCode);
	static wstring GetAppName();
	static wstring GetLocalIP();
	static LONG CALLBACK MyCrashHandler(struct _EXCEPTION_POINTERS *ExceptionInfo);

private:
	XYClient_Process_Type m_emProcessType;//XYClient进程类型
	std::wstring m_strProcessCmdLine;//进程命令行
};
