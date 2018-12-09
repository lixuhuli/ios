#pragma once
#include "Task.h"


namespace TaskCenter{
	class CTaskIMToken
		:public ITask
	{
	public:
		CTaskIMToken(const MSG& msg, const Json::Value& vUserInfo);
		~CTaskIMToken();

		void Init(const MSG& msg, const Json::Value& vUserInfo);

	protected:
		virtual void Run();

	private:
		Json::Value m_vUserInfo;
	};

	class CTaskIMUpdateInfo
		:public ITask
	{
	public:
		CTaskIMUpdateInfo(const string& strJsonInfo);
		void Init(const string& strJsonInfo);

	protected:
		virtual void Run();

	private:
		string m_strJsonInfo;
	};

	class CTaskIMCheckMsg
		:public ITask
	{
	public:
		CTaskIMCheckMsg(const MSG& msg, int nUid, const string& strToken, const string& strMsg);
		void Init(const MSG& msg, int nUid, const string& strToken, const string& strMsg);

	protected:
		virtual void Run();

	private:
		int m_nUid;
		string m_strToken;
		string m_strMsg;
	};
}
