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
        const wstring& GetGuid()const { return m_strGuid; }
		void SetPCVersion(const string& strVersion) { m_strPCVersion = strVersion; }
		string& GetPCVersion() { return m_strPCVersion; }
		const string& GetMacAddress()const { return m_strMac;}

		void CreatePostTask(const char* pEventName, const Json::Value& vParams=Json::Value(Json::objectValue), int nUid=0);

	protected:
		CDataCenter();
		~CDataCenter();

        bool InitWorkThread();
        void Work();
        static UINT __stdcall Thread(void* lpParam);

	private:
		int m_nAdID;
		string m_strMac;
        wstring m_strGuid;
		string m_strPCVersion;
		list<UINT_PTR>	m_taskList;
        UINT m_nThreadID;
        HANDLE m_hThread;
	};
}
