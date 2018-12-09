#pragma once
#include "TaskEnum.h"
#include <list>
#include <string>
using std::list;
using std::wstring;



namespace DataCenter{

	typedef list<UINT_PTR>::iterator TaskItor;
	typedef list<UINT_PTR>::const_iterator TaskConstItor;
	class CDataCenter
	{
	public:
		static CDataCenter* Instance();
		void Init(const char* pToken);
		void Exit();

		int GetAdID()const { return m_nAdID; }
		void SetAdID(int nID) { m_nAdID = nID;}
		void SetPCVersion(const string& strVersion) { m_strPCVersion = strVersion; }
		const wstring& GetGuid()const { return m_strGuid; }
		const wstring& GetToken()const { return m_strToken; }
		string& GetPCVersion() { return m_strPCVersion; }
		const string& GetMacAddress()const { return m_strMac;}
		void SetToken(const wstring& strToken) { m_strToken = strToken; }

		void CreatePostTask(const char* pEventName, const Json::Value& vParams=Json::Value(Json::objectValue), int nUid=0);

	protected:
		CDataCenter();
		~CDataCenter();
		void InitPCConfig();
		bool InitWorkThread();
		bool InitPostToken();
		void Work();
		static UINT __stdcall Thread(void* lpParam);

	private:
		int m_nAdID;
		string m_strMac;
		wstring m_strGuid;
		wstring m_strToken;
		string m_strPCVersion;
		list<UINT_PTR>	m_taskList;
		UINT m_nThreadID;
		HANDLE m_hThread;
	};
}
