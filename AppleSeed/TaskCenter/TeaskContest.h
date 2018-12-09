#pragma once
#include "Task.h"

namespace TaskCenter{
//生成场次ID
#define URL_GET_CONTESTID			L"http://anv3btapi.papa91.com/lobby/game_rank/generatePlayCode"//L"http://testbattle.papa91.com/lobby/game_rank/generatePlayCode"

//获取排行榜信息
#define URL_GET_CONTEST_RANKING		L"http://anv3btapi.papa91.com/lobby/game_rank/index"//L"http://testbattle.papa91.com/lobby/game_rank/index"

//排位赛上传
#define URL_UPLOAD_RANKRECOED		L"http://anv3btapi.papa91.com/lobby/game_rank/fileUpload"//L"http://testbattle.papa91.com/lobby/game_rank/fileUpload"


	class CTaskContest
		:public ITask
	{
		enum
		{
			TaskGetID = 0,
			TaskGetRanking,
		};
	public:
		CTaskContest(const MSG& msg, int nUid, __int64 nGameID, const string& strToken, int nFlag)
			: ITask(msg)
			, m_nUid(nUid)
			, m_nGameID(nGameID)
			, m_strToken(strToken)
			, m_nFlag(nFlag)
		{
			m_type = TaskContestID;
			m_bUsed = TRUE;
		}

		void Init(const MSG& msg, int nUid, __int64 nGameID, const string& strToken, int nFlag)
		{
			m_msg = msg;
			m_bUsed = TRUE;
			m_nUid = nUid;
			m_nGameID = nGameID;
			m_strToken = strToken;
			m_nFlag = nFlag;
		}

	protected:
		virtual void Run()
		{
			wstring strUrl(m_nFlag == TaskGetID ? URL_GET_CONTESTID : URL_GET_CONTEST_RANKING);
			wchar_t szParam[200];
			swprintf_s(szParam, L"?uid=%d&gameid=%I64d&token=%s", m_nUid, m_nGameID, PublicLib::AToU(m_strToken).c_str());
			strUrl.append(szParam);
			PublicLib::CHttpClient http;
			m_strResult = http.Request(strUrl.c_str(), PublicLib::Get);
		}

	private:
		int m_nFlag;
		int m_nUid;
		__int64 m_nGameID;
		string m_strToken;
	};

	class CTaskUploadRecord
		:public ITask
	{
	public:
		CTaskUploadRecord(const MSG& msg, const wstring& strPath, const string& strName)
			: ITask(msg)
			, m_strRecordPath(strPath)
			, m_strName(strName)
		{
			m_bUsed = TRUE;
			m_type = TaskUploadRecord;
		}
		void Init(const MSG& msg, const wstring& strPath, const string& strName)
		{
			m_msg = msg;
			m_strRecordPath = strPath;
			m_strName = strName;
			m_bUsed = TRUE;
		}
		const wstring& GetRecordPath() { return m_strRecordPath; }

	protected:
		virtual void Run()
		{
			FILE* fp = NULL;
			_wfopen_s(&fp, m_strRecordPath.c_str(), L"rb");
			if (NULL == fp)
			{
				OUTPUT_XYLOG(LEVEL_ERROR, L"打开文件失败，文件路径：%s", m_strRecordPath.c_str());
				return;
			}
			fseek(fp, 0, SEEK_END);
			size_t nSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			if (nSize == 0)
			{
				fclose(fp);
				return;
			}
			char* pData = (char*)malloc(nSize);
			size_t nRead = fread(pData, 1, nSize, fp);
			fclose(fp);
			string CRLF = "\r\n";
			string Boundary = "-----------------------------67491722032265";
			string StartBoundary = "--" + Boundary + CRLF; //头部多两个-
			string EndBoundary = "--" + Boundary + "--" + CRLF;//头部尾部都多两个-
			string strPostData = StartBoundary + "Content-Disposition: form-data; name=\"file_name\"\r\n\r\n" + m_strName + CRLF;
			strPostData += StartBoundary + "Content-Disposition: form-data; name=\"file\"; filename=\"" + m_strName + "\"" + CRLF
				+ "Content-Type: application/octet-stream" + CRLF + CRLF;
			strPostData.append(pData, nRead);
			strPostData += CRLF;
			strPostData += EndBoundary;
			free(pData);
			PublicLib::CHttpClient http(PublicLib::ContentFile);
			wstring str(L"multipart/form-data; boundary=");
			str += PublicLib::AToU(Boundary);
			http.AddHttpHeader(L"Cache-Control", L"no-cache");
			http.AddHttpHeader(L"Accept-Encoding", L"gzip, deflate");
			http.AddHttpHeader(L"Content-Type", str.c_str());
			m_strResult = http.Request(URL_UPLOAD_RANKRECOED, PublicLib::Post, strPostData.c_str(), strPostData.size());
		}

	private:
		string m_strName;
		wstring m_strRecordPath;
	};
}