#pragma once


enum XYClient_Process_Type
{
	Process_Type_Main = 0,//������
	Process_Type_LibCef_Gpu,//libcef GPU����
	Process_Type_LibCef_Render,//libcef ��Ⱦ����
	Process_Type_LibCef_Plugin,//libcef�������
	Process_Type_LibCef,//libcef��������
	Process_Type_Other,//�������ͽ���
	Process_Type_Unknow,//����δ֪���ͽ���
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
	XYClient_Process_Type m_emProcessType;//XYClient��������
	std::wstring m_strProcessCmdLine;//����������
};
