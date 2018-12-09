#pragma once
#include "Task.h"
#include "HttpInterface.h"


namespace TaskCenter{

	class CTaskGetServer
		:public ITask
	{
	public:
		CTaskGetServer(const MSG& msg, int nUid, const string& strToken)
			: ITask(msg)
			, m_nUid(nUid)
			, m_strToken(strToken)
		{
			m_type = TaskGetServer;
			m_bUsed = TRUE;
		}
		void Init(const MSG& msg, int nUid, const string& strToken)
		{
			m_msg = msg;
			m_nUid = nUid;
			m_strToken = strToken;
			m_bUsed = TRUE;
		}

	protected:
		virtual void Run()
		{
			PublicLib::CHttpClient http;
			wstring strUrl(URL_GETSERVER);
			strUrl.append(L"uid=");
			wchar_t szUid[100] = { 0 };
			swprintf_s(szUid, L"%u&token=%s", m_nUid, PublicLib::Utf8ToU(m_strToken).c_str());
			strUrl.append(szUid);
			m_strResult = http.Request(strUrl.c_str(), PublicLib::Get);
		}

	private:
		int m_nUid;
		string m_strToken;
	};


	class CTaskGetGameList
		:public ITask
	{
	public:
		CTaskGetGameList(const MSG& msg, int nType, __int64 nID)
			: ITask(msg)
			, m_nIDType(nType)
			, m_nID(nID)
		{
			m_type = TaskGetGameList;
			m_bUsed = TRUE;
		}
		void Init(const MSG& msg, int nType, __int64 nID)
		{
			m_msg = msg;
			m_nIDType = nType;
			m_nID = nID;
			m_bUsed = TRUE;
		}

	protected:
		virtual void Run()
		{
			PublicLib::CHttpClient http;
			Json::Value vParam, vMsg;
			CGlobalData::Instance()->InitPhpParams(vParam);
			vMsg["args"]["id"] = m_nID;
			vMsg["args"]["type"] = m_nIDType;
			vParam["messages"] = vMsg;
			Json::FastWriter fw;
			string strJson = fw.write(vParam);
			m_strResult = http.Request(URL_GET_GAMELIST, PublicLib::Post, strJson.c_str(), strJson.size());
		}

	private:
		int m_nIDType;//类型(1 游戏 2合集)
		__int64 m_nID;
	};

	class CTaskCreateRoom
		:public ITask
	{
	public:
		CTaskCreateRoom(const MSG& msg, int nUid, const string& strToken, __int64 nGameID)
			: ITask(msg)
			, m_nUid(nUid)
			, m_strToken(strToken)
			, m_nGameID(nGameID)
		{
			m_type = TaskCreateRoom;
			m_bUsed = TRUE;
		}

		void Init(const MSG& msg, int nUid, const string& strToken, __int64 nGameID)
		{
			m_msg = msg;
			m_nUid = nUid;
			m_strToken = strToken;
			m_nGameID = nGameID;
			m_bUsed = TRUE;
		}

	protected:
		virtual void Run()
		{
			PublicLib::CHttpClient http;
			wstring strUrl(URL_CREATE_ROOM);
			wchar_t szCmd[200] = { 0 };
			swprintf_s(szCmd, L"uid=%d&token=%s&game_id=%I64d", m_nUid, PublicLib::AToU(m_strToken).c_str(), m_nGameID);
			strUrl.append(szCmd);
			m_strResult = http.Request(strUrl.c_str(), PublicLib::Get);
		}

	private:
		int m_nUid;
		__int64 m_nGameID;
		string m_strToken;
	};
}