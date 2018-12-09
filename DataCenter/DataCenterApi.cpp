#include "stdafx.h"
#include "DataCenterApi.h"
#include "DataCenter.h"
using namespace DataCenter;




DATA_CENTER_API void InitDataCenter(const char* pToken)
{
	CDataCenter::Instance()->Init(pToken);
}

DATA_CENTER_API void ExitDataCenter()
{
	CDataCenter::Instance()->Exit();
}

DATA_CENTER_API void CreateDataTask(const char* pEventName, const Json::Value& vParams/*=Json::Value(Json::objectValue)*/, int nUid/*=0*/)
{
	CDataCenter::Instance()->CreatePostTask(pEventName, vParams, nUid);
}

DATA_CENTER_API void GetDataCenterToken(OUT wstring& strToken)
{
	strToken = CDataCenter::Instance()->GetToken();
}

DATA_CENTER_API void CreateDataTaskC(char* pEventName, char* pJsonParam, int nUid)
{
	if(NULL == pJsonParam || strlen(pJsonParam) == 0)
	{
		CDataCenter::Instance()->CreatePostTask(pEventName, Json::Value(Json::objectValue), nUid);
		return;
	}
	try
	{
		Json::Value vParam;
		Json::Reader read;
		if (read.parse(pJsonParam, vParam))
		{
			CDataCenter::Instance()->CreatePostTask(pEventName, vParam, nUid);
		}
	}
	catch(...)
	{

	}
}

DATA_CENTER_API void SetDataCenterID(int nAdID)
{
	CDataCenter::Instance()->SetAdID(nAdID);
}

DATA_CENTER_API void SetDataCenterVersion(const char* pVersion)
{
	if (pVersion)
		CDataCenter::Instance()->SetPCVersion(string(pVersion));
}