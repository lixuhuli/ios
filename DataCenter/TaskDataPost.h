#pragma once
#include "Task.h"
#include "DataPostDefine.h"



namespace DataCenter{

	bool GetPostAuth();

	class CTaskDataPost
		:public ITask
	{
	public:
		CTaskDataPost(int nUid, const char* pEventName, const Json::Value& vParams);
		~CTaskDataPost();
		void Init(int nUid, const char* pEventName, const Json::Value& vParams);

	protected:
		virtual int Run();

	private:
		int m_nUid;
		Json::Value m_vParams;
		string m_strEventName;
	};

}