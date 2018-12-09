#include "StdAfx.h"
#include "ExceptionHandler.h"
#include <DbgHelp.h>
#include <SoftDefine.h>
#include "DataPost.h"
#pragma comment(lib, "DbgHelp")


wstring g_strExceptionPos=TEXT("undefined");

void DisableSetUnhandledExceptionFilter()
{
	void *addr = (void*)GetProcAddress(LoadLibrary(_T("kernel32.dll")), "SetUnhandledExceptionFilter");
	if (addr)
	{
		unsigned char code[16];
		int size = 0;
		code[size++] = 0x33;
		code[size++] = 0xC0;
		code[size++] = 0xC2;
		code[size++] = 0x04;
		code[size++] = 0x00;

		DWORD dwOldFlag, dwTempFlag;
		VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &dwOldFlag);
		WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
		VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
	}
}

CExceptionHandler::CExceptionHandler(void)
	:m_emProcessType(Process_Type_Unknow)
{
}

CExceptionHandler::~CExceptionHandler(void)
{
	::WSACleanup();
}

CExceptionHandler* CExceptionHandler::Instance()
{
	static CExceptionHandler instance;
	return &instance;
}

void CExceptionHandler::Init( LPTSTR lpCmdLine )
{
	m_strProcessCmdLine = lpCmdLine;
	HANDLE hSingleMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME);
	if ( hSingleMutex )
	{
		if ( std::wstring::npos != m_strProcessCmdLine.find(L"--type=gpu") )
		{
			m_emProcessType = Process_Type_LibCef_Gpu;
		} 
		else if( std::wstring::npos != m_strProcessCmdLine.find(L"--type=renderer") )
		{
			m_emProcessType = Process_Type_LibCef_Render;
		}
		else if( std::wstring::npos != m_strProcessCmdLine.find(L"--type=plugin") )
		{
			m_emProcessType = Process_Type_LibCef_Plugin;
		}		
		else if( std::wstring::npos != m_strProcessCmdLine.find(L"--type") )
		{
			m_emProcessType = Process_Type_LibCef;
		}
		else
		{
			m_emProcessType = Process_Type_Other;
		}
	}
	else
	{
		m_emProcessType = Process_Type_Main;
	}
	SetUnhandledExceptionFilter(CExceptionHandler::MyCrashHandler);
	DisableSetUnhandledExceptionFilter();	
}

const std::wstring& CExceptionHandler::GetProcessCmdLine()
{
	return m_strProcessCmdLine;
}

XYClient_Process_Type CExceptionHandler::GetProcessType()
{
	return m_emProcessType;
}

wstring CExceptionHandler::TransExpCode( DWORD ExceptionCode )
{
	switch(ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION: return _T("EXCEPTION_ACCESS_VIOLATION");
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return _T("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
	case EXCEPTION_BREAKPOINT: return _T("EXCEPTION_BREAKPOINT");
	case EXCEPTION_DATATYPE_MISALIGNMENT: return _T("EXCEPTION_DATATYPE_MISALIGNMENT");
	case EXCEPTION_FLT_DENORMAL_OPERAND: return _T("EXCEPTION_FLT_DENORMAL_OPERAND");
	case EXCEPTION_FLT_DIVIDE_BY_ZERO: return _T("EXCEPTION_FLT_DIVIDE_BY_ZERO");
	case EXCEPTION_FLT_INEXACT_RESULT: return _T("EXCEPTION_FLT_INEXACT_RESULT");
	case EXCEPTION_FLT_INVALID_OPERATION: return _T("EXCEPTION_FLT_INVALID_OPERATION");
	case EXCEPTION_FLT_OVERFLOW: return _T("EXCEPTION_FLT_OVERFLOW");
	case EXCEPTION_FLT_STACK_CHECK: return _T("EXCEPTION_FLT_STACK_CHECK");
	case EXCEPTION_FLT_UNDERFLOW: return _T("EXCEPTION_FLT_UNDERFLOW");
	case EXCEPTION_ILLEGAL_INSTRUCTION: return _T("EXCEPTION_ILLEGAL_INSTRUCTION");
	case EXCEPTION_IN_PAGE_ERROR: return _T("EXCEPTION_IN_PAGE_ERROR");
	case EXCEPTION_INT_DIVIDE_BY_ZERO: return _T("EXCEPTION_INT_DIVIDE_BY_ZERO");
	case EXCEPTION_INT_OVERFLOW: return _T("EXCEPTION_INT_OVERFLOW");
	case EXCEPTION_INVALID_DISPOSITION: return _T("EXCEPTION_INVALID_DISPOSITION");
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: return _T("EXCEPTION_NONCONTINUABLE_EXCEPTION");
	case EXCEPTION_PRIV_INSTRUCTION: return _T("EXCEPTION_PRIV_INSTRUCTION");
	case EXCEPTION_SINGLE_STEP: return _T("EXCEPTION_SINGLE_STEP");
	case EXCEPTION_STACK_OVERFLOW: return _T("EXCEPTION_STACK_OVERFLOW");
	default:_T("EXCEPTION_UNDEFINED");
	}
	return _T("EXCEPTION_UNDEFINED");
}

wstring CExceptionHandler::GetAppName()
{
	wstring AppName;
	WCHAR FileName[MAX_PATH];
	if(GetModuleFileName(NULL,FileName,MAX_PATH))
	{
		AppName = FileName;
		AppName = AppName.substr(AppName.find_last_of(_T('\\')) + 1);
	}
	return AppName;
}

wstring CExceptionHandler::GetLocalIP()
{
	wstring wstrIP;
	
	char hostname[256];
	if (gethostname(hostname,sizeof(hostname)) == SOCKET_ERROR) return wstrIP;
	
	HOSTENT* host = gethostbyname(hostname);
	if (host==NULL) return wstrIP;

	string strIP;
	strIP = inet_ntoa(*(in_addr*)*host->h_addr_list);
	wstrIP = PublicLib::AToU(strIP);
	return wstrIP;
}

LONG CALLBACK CExceptionHandler::MyCrashHandler( struct _EXCEPTION_POINTERS *ExceptionInfo )
{
	if(NULL == ExceptionInfo)
	{
		return EXCEPTION_EXECUTE_HANDLER;
	}

	try
	{
		wstring sAppDataDir = GetAppDataPath();
		wstring strAppVersion = GetApplicationVersion();
		wstring strRunPath = GetRunPathW();
		wstring datFile = sAppDataDir + _T("errfile.dat");
		HANDLE hFile = ::CreateFile( datFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD dwCode = 0;
		if( hFile != INVALID_HANDLE_VALUE)
		{
			wstring strBuffer(_T("ExceptionReport:\r\n\r\n"));
			strBuffer += _T("AppName: ") + CExceptionHandler::GetAppName() + _T("\r\n");
			strBuffer += _T("AppVersion: ") + strAppVersion + _T("\r\n");

			wstring strName, strVersion;
			PublicLib::GetOSVersion(strName, strVersion);
			strBuffer += L"SystemOSMainInfo: " + strName + _T("\r\n") ;
			strBuffer += L"SystemOSVersion: " + strVersion + L"\r\n";
			strBuffer += L"LocalIP: " + GetLocalIP() + L"\r\n";
// 			str.Format(_T("CmdLine:%s\r\n"),CExceptionHandler::Instance()->GetProcessCmdLine().c_str());
// 			Buf += str.GetString();
// 			str.Format(_T("ProcessType:%d\r\n"),CExceptionHandler::Instance()->GetProcessType());
// 			Buf += str.GetString();

			strBuffer += _T("\r\n-----------------------------------------------------------------------------------------------\r\n");
			strBuffer += _T("\r\nExceptionRecord:\r\n\r\n");
			_EXCEPTION_RECORD* pExceptionRecord = ExceptionInfo->ExceptionRecord;
			while (pExceptionRecord)
			{
				strBuffer += _T("ExceptionCode: ") + CExceptionHandler::TransExpCode(pExceptionRecord->ExceptionCode) + _T("\r\n");
				dwCode = pExceptionRecord->ExceptionCode;
				wstring wstrFlags = (pExceptionRecord->ExceptionFlags == 0) ? _T("EXCEPTION_CONTINUABLE") : _T("EXCEPTION_NONCONTINUABLE");
				strBuffer += _T("ExceptionFlags: ") + wstrFlags + _T("\r\n");
				wchar_t szTemp[100] = { 0 };
				swprintf_s(szTemp, _T("ExceptionAddress: 0x%X\r\n"), pExceptionRecord->ExceptionAddress);
				strBuffer += _T("ExceptionAddress: ");
				strBuffer.append(szTemp);
				for(DWORD i = 0 ; i < pExceptionRecord->NumberParameters ; i++)
				{
					//str.Format(_T("ExceptionInformation[%d]:%d"),i,pExceptionRecord->ExceptionInformation[i]);
					swprintf_s(szTemp, _T("ExceptionInformation[%d]: %d\r\n"), i, pExceptionRecord->ExceptionInformation[i]);
					strBuffer.append(szTemp);
				}
				pExceptionRecord = pExceptionRecord->ExceptionRecord;
			}

			DWORD dWriteNum;
			WriteFile(hFile,strBuffer.c_str(),strBuffer.length() * sizeof(wchar_t),&dWriteNum,NULL);
			::CloseHandle(hFile);
		}
		PostCrashDump(dwCode);
		MINIDUMP_TYPE mdt = MiniDumpNormal;//MiniDumpNormal, MiniDumpWithFullMemory
		hFile = NULL;
		wstring dmpFile = sAppDataDir + _T("errfile.dmp");
		hFile = ::CreateFile(dmpFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile != INVALID_HANDLE_VALUE)
		{
			MINIDUMP_EXCEPTION_INFORMATION einfo;
			einfo.ThreadId = ::GetCurrentThreadId();
			einfo.ExceptionPointers = ExceptionInfo;
			einfo.ClientPointers = FALSE;
			::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, mdt, &einfo, NULL, NULL);
			::CloseHandle(hFile);
		}
		wstring strErrHandlerExe = strRunPath + EXE_BUGREPORT;
		wchar_t szExePath[MAX_PATH];
		GetModuleFileName(NULL, szExePath, MAX_PATH);
		wstring exeCmd = wstring(_T("\"")) + datFile + _T("\" \"") + dmpFile + _T("\" \"") + szExePath + _T("\" \"") + strAppVersion + _T("\" \"AppleSeed\"");
		OUTPUT_XYLOG(LEVEL_ERROR, L"程序异常崩溃，启动异常捕获程序传入参数：%s", exeCmd.c_str());
		//"C:\Users\yaojn.5FUN\AppData\Roaming\5funGameHall\errfile.dat" "C:\Users\yaojn.5FUN\AppData\Roaming\5funGameHall\errfile.dmp" "F:\Work\PC\bin\Release\5funGameHall.exe" "1.0.0.300"
		ShellExecute(NULL, _T("open"), strErrHandlerExe.c_str(), exeCmd.c_str(), NULL, SW_SHOWNORMAL);
	}
	catch(...)
	{

	}
	return EXCEPTION_EXECUTE_HANDLER;
}