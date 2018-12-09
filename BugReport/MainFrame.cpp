#include "StdAfx.h"
#include "MainFrame.h"
#include "wininet.h"
#include <process.h>
#include <string>
#include "ControlEx.h"
#include <shellapi.h>
#include "resource.h"
#include <atlstr.h>


using namespace std;
#pragma comment(lib,"wininet.lib")
#pragma comment(lib,"ws2_32.lib")


#define WM_EXIT_MSG		WM_USER+200

bool HttpAccess(wstring& sUrl, wstring& object, string& sReq, string& sResp,unsigned short port = 80)
{
	try
	{
		CStringW strUrl = sUrl.c_str();
		if( object.length() > 0 )
		{
			if(object.at(0) == L'/') strUrl += object.c_str();
			else strUrl += (L'/' + object).c_str();
		}
		CStringA strPost = sReq.c_str();

		PublicLib::HttpRequestPost httpPost;
		httpPost.Init(NULL);
		httpPost.SetMaxRetry(1);
		httpPost.SetHttpPort(port);
		httpPost.AddSendData(strPost);
		httpPost.Request(strUrl);

		bool bRet = httpPost.RecvContent(sResp) ? true : false;
		DWORD dwStatus =httpPost.GetStatusCode();

		return bRet;
	}
	catch(...)
	{
		return false;
	}
	return true;
}

void EnumDirectoryFile(wstring& dir,wstring Wildcard,vector<wstring> &vctFiles, bool bSubDir)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind;
	wstring wstrDir = dir + Wildcard;
	hFind = ::FindFirstFile(wstrDir.c_str(), &fd);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			wstrDir = fd.cFileName;
			if (wstrDir != _T(".") && wstrDir != _T(".."))
			{
				if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if(bSubDir && ( -1 != wstrDir.find(L"XYSDK") || -1 != wstrDir.find(L"XYClient")) )
					{
						//说明需要枚举下级子目录
						wstrDir = dir + fd.cFileName+_T("\\");
						SYSTEMTIME stLocalTime;
						GetLocalTime(&stLocalTime);
						CString strFilter;
						strFilter.Format(_T("*%02d-%02d-%02d.log"),stLocalTime.wYear,stLocalTime.wMonth,stLocalTime.wDay);
						EnumDirectoryFile(wstrDir, strFilter.GetString(), vctFiles, bSubDir);
					}
				}
				else
				{
					if ( -1 != wstrDir.find(L"XYClient3") || -1 != wstrDir.find(L"XYLogicCtrl") )
					{
						vctFiles.push_back(dir + fd.cFileName);
					}
				}
			}

		}
		while(::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
}

CMainFrame::CMainFrame(void)
{
	m_dwExStyle = WS_EX_TOPMOST;
}

CMainFrame::~CMainFrame(void)
{
}

void CMainFrame::InitWindow()
{
	CWndBase::InitWindow();
	b_isSendingDeump = false;
	if (g_exeType.compare(L"Emulator") == 0)//GameHall
	{
		m_pBtnReOpen->SetVisible(false);
		m_pTextTitle->SetText(L"悟饭模拟器遇到问题需要关闭，对此引起的不便");
		g_dumpProcess = DptEmulator;
	}
	else if (g_exeType.compare(L"Cafe") == 0)
	{
		m_pBtnReOpen->SetVisible(true);
		m_pTextTitle->SetText(L"悟饭游戏厅网吧版遇到问题需要关闭，对此引起的不便");
		g_dumpProcess = DptCafe;
	}
    else if (g_exeType.compare(L"AppleSeed") == 0)
    {
        m_pBtnReOpen->SetVisible(true);
        m_pTextTitle->SetText(L"果仁模拟器遇到问题需要关闭，对此引起的不便");
        g_dumpProcess = DptAppleSeed;
    }
	else
	{
		m_pTextTitle->SetText(L"悟饭游戏厅遇到问题需要关闭，对此引起的不便");
		g_dumpProcess = DptMain;
	}
}

LRESULT CMainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_TIMER:
		{
			if(wParam == 0)
				QuitAndRestart();
			break;
		}
	case WM_EXIT_MSG:
		{
			DeleteFile(g_datFilePath.c_str());
			DeleteFile(g_dumpFilePath.c_str());
			DeleteFile(g_zipFilePath.c_str());
			Close();
			if ( wParam == 0 )
				QuitAndRestart();
			PostQuitMessage(1);
			break;
		}
	default:
		break;
	}
	return CWndBase::HandleMessage(uMsg, wParam, lParam);

}

bool CMainFrame::OnNotifyBtnOK(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		if (!b_isSendingDeump)
		{
			ShowWindow(false, false);
			HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, SendThread, this, NULL, NULL);
			CloseHandle(hThread);
		}
	}
	return true;
}

static unsigned _stdcall BugReportPro(void* param)
{
	string* pSRequest = (string*)param;
	wstring sUrl(L"http://www.xyzsq.com");
	wstring sObject = _T("/sdk/soft_breakdown_log.php");
	string sResp;
	HttpAccess(sUrl, sObject, *pSRequest, sResp);
	delete pSRequest;
	pSRequest = NULL;

	return 0;
}

void CMainFrame::PostVersionAndTime()
{
	string* pSReq = new string;

	time_t rawtime;
	time(&rawtime);
	char buf[100] = {0};
	sprintf_s(buf, "%I64d", rawtime);
	string strPost;

	string version = UToA(g_AppVersion);
	
	strPost.append(version);
	strPost.append("0");
	strPost.append(buf);

//  	string tsrTest = GetMD5AsHex("582df15de91b3f12d8e710073e43f4f8"+strPost);

	MD5 md5("582df15de91b3f12d8e710073e43f4f8"+strPost);
	string md5Result = md5.md5();
	pSReq->append("ver=");
	pSReq->append(version);
	pSReq->append("&type=0");
	pSReq->append("&time=");
	pSReq->append(buf);
	pSReq->append("&sign=");
	pSReq->append(md5Result);

 	HANDLE _handle = (HANDLE)_beginthreadex(NULL, 0, BugReportPro, pSReq, 0, NULL);
 	CloseHandle(_handle);
}

string wstring2string(const wstring& wstr)
{
	string result;  
	//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);  
	char* buffer = new char[len + 1];  
	//宽字节编码转换成多字节编码  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);  
	buffer[len] = '\0';  
	result.append(buffer);  
	delete[] buffer;  
	return result;  
}

wstring string2wstring(string str)  
{  
	wstring result;   
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);  
	wchar_t* buffer = new wchar_t[len + 1];  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);  
	buffer[len] = '\0';
	result.append(buffer);  
	delete[] buffer;  
	return result;  
}

wstring GetPathName(wstring& Path)
{
	wstring Name;
	wstring::size_type index = Path.find_last_of(L'\\');
	if(index != wstring::npos)
		Name = Path.substr(index + 1);
	else
		Name = Path;
	return Name;
}

unsigned int WINAPI CMainFrame::SendThread(void* pParam)
{
	CMainFrame *pthis = (CMainFrame *) pParam;
	pthis->b_isSendingDeump=true;

	//压缩文件
	PublicLib::CFileZip_LibZip zipFile;
	::DeleteFile(g_zipFilePath.c_str());
	if(zipFile.Create(g_zipFilePath.c_str()))
	{
		zipFile.AppendFile(g_datFilePath.c_str(),GetPathName(g_datFilePath).c_str());
		zipFile.AppendFile(g_dumpFilePath.c_str(),GetPathName(g_dumpFilePath).c_str());
		zipFile.Close();
	}

	//Http上传文件
	HINTERNET hHttpOpen = InternetOpen(L"",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,NULL);
	if(hHttpOpen)
	{
		HINTERNET hHttpConn = InternetConnect(hHttpOpen, L"bugreport.5fun.com" ,INTERNET_DEFAULT_HTTP_PORT,NULL,NULL,INTERNET_SERVICE_HTTP,NULL,NULL);
		if(hHttpConn)
		{
			wstring szBugReportLink = L"/pc/index";
            switch (g_dumpProcess)
            {
            case DptMain:
                szBugReportLink += (L"?xyclient_version=" + g_AppVersion + L"_main");
                break;
            case DptEmulator:
                szBugReportLink += (L"?xyclient_version=" + g_AppVersion + L"_emulator");
                break;
            case DptCafe:
                szBugReportLink += (L"?xyclient_version=" + g_AppVersion + L"_cafe");
                break;
            case DptAppleSeed:
                szBugReportLink += (L"?xyclient_version=" + g_AppVersion + L"_appleseed");
                break;
            default:
                szBugReportLink += (L"?xyclient_version=" + g_AppVersion);
                break;
            }

			HINTERNET hHttpReq = HttpOpenRequest(hHttpConn,_T("POST"),szBugReportLink.c_str(),NULL,NULL,NULL,INTERNET_FLAG_NO_CACHE_WRITE,NULL);
			if(hHttpReq)
			{
				string CRLF = "\r\n";
				string Boundary = "-----------------------------67491722032265";//随机字符串
				string StartBoundary = "--" + Boundary + CRLF; //头部多两个-
				string EndBoundary = "--" + Boundary + "--" + CRLF;//头部尾部都多两个-

				CStringA strContentType;
				strContentType.Format("Content-Type: multipart/form-data; boundary=%s%s", Boundary.c_str(), CRLF.c_str());
				//Http::Header,仅需要content_type模块来说明将使用mime协议实现多表单上传
				//wstring content_type = boost::str(boost::wformat(_T("Content-Type: multipart/form-data; boundary=%s%s")) % string2wstring(Boundary) % string2wstring(CRLF));

				wstring Header;
				Header += AToU(string(strContentType.GetBuffer()));

				//ReadFile
				char* FileBuf = NULL;
				DWORD FileBufSize = 0;
				HANDLE hRFile = ::CreateFile( g_zipFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if(hRFile != INVALID_HANDLE_VALUE)
				{
					DWORD FileSize = GetFileSize(hRFile,NULL);
					FileBufSize = FileSize + CRLF.length();//+CRLF
					FileBuf = new char[FileBufSize];
					memset(FileBuf,0,FileBufSize);
					DWORD FRLen;
					ReadFile(hRFile,FileBuf,FileSize,&FRLen,NULL);
					memcpy(&FileBuf[FileSize],CRLF.c_str(),CRLF.length());
					::CloseHandle(hRFile);
				}

				//Http::Content
				//对应multipart/form-data，采用固定格式填写表单:
				//------------cH2gL6ei4KM7Ef1cH2Ij5gL6gL6Ef1										//StartBoundary,头部多两个-
				//	Content-Disposition: form-data; name="Filename"									//每一行都以CRLF结尾
				//																					//开始填值前空一行
				//	test.zip																		//值
				//	------------cH2gL6ei4KM7Ef1cH2Ij5gL6gL6Ef1										//NormalBoundary
				//	Content-Disposition: form-data; name="FileData"; filename="test.zip"			//文件字段需要多声明一个保存在服务器上的名字
				//	Content-Type: application/octet-stream											//还需要多声明其值将会是二进制流形式

				//	PK																			//文件内容,乱码
				//------------cH2gL6ei4KM7Ef1cH2Ij5gL6gL6Ef1--										//EndBoundary,头尾都多两个-
				SYSTEMTIME Time = {0};
				GetLocalTime(&Time);
				CStringA strTime;
				strTime.Format("%04d_%02d_%02d_%02d_%02d_%02d", Time.wYear % Time.wMonth % Time.wDay % Time.wHour % Time.wMinute % Time.wSecond);

				string ServerFileName;
				switch (g_dumpProcess)
				{
				case DptMain:
					ServerFileName = "errfile_main_";
					break;
				case DptEmulator:
					ServerFileName = "errfile_emulator_";
					break;
				case DptCafe:
					ServerFileName = "errfile_cafe_";
					break;
                case DptAppleSeed:
                    ServerFileName = "errfile_appleseed_";
                    break;
				}
				//string ServerFileName(g_dumpProcess==DptMain?"errfile_main_":"errfile_emulator_");
				ServerFileName += strTime + (".zip");
				//string ContentDisposition = boost::str(boost::format("Content-Disposition: form-data; name=\"logfile\"; filename=\"%s\"%s") % ServerFileName % CRLF);

				CStringA str;
				str.Format("Content-Disposition: form-data; name=\"logfile\"; filename=\"%s\"%s", ServerFileName.c_str(), CRLF.c_str());
				string ContentDisposition(str.GetBuffer());

				string ContentType = "Content-Type: application/octet-stream" + CRLF + CRLF;
				DWORD ContentLength = StartBoundary.length() + ContentDisposition.length() + ContentType.length() + FileBufSize + EndBoundary.length();
				char* Content = new char[ContentLength];

				memcpy(Content,StartBoundary.c_str(),StartBoundary.length());
				long index = StartBoundary.length();
				memcpy(&Content[index],ContentDisposition.c_str(),ContentDisposition.length());
				index += ContentDisposition.length();
				memcpy(&Content[index],ContentType.c_str(),ContentType.length());
				index += ContentType.length();
				if(FileBuf)
				{
					memcpy(&Content[index],FileBuf,FileBufSize);
					index += FileBufSize;
				}
				memcpy(&Content[index],EndBoundary.c_str(),EndBoundary.length());

				BOOL flag = HttpSendRequest(hHttpReq,Header.c_str(),Header.length(),(LPVOID)Content,ContentLength);
				//DWORD code = GetLastError();

				if(FileBuf) delete[] FileBuf;
				delete[] Content;

				InternetCloseHandle(hHttpReq);
			}
			InternetCloseHandle(hHttpConn);
		}
		InternetCloseHandle(hHttpOpen);
	}
	pthis->PostMessage(WM_EXIT_MSG, 0);
	return 0;
}

void CMainFrame::QuitAndRestart()
{
	if(m_pBtnReOpen && m_pBtnReOpen->IsVisible() && m_pBtnReOpen->GetCheck())
	{
		if(g_AppPath != L"")
			ShellExecute(NULL,_T("open"),g_AppPath.c_str(),NULL,NULL,SW_SHOWNORMAL);
	}
	b_isSendingDeump=false;
}
