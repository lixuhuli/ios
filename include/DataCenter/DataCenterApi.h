#pragma once
#include "TaskEnum.h"



#ifdef USE_DLL
#define DATA_CENTER_API __declspec(dllimport)
#else
#ifdef  USE_LIB
#define DATA_CENTER_API
#else
#define DATA_CENTER_API __declspec(dllexport)
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

//初始化数据中心
DATA_CENTER_API void InitDataCenter(const char* pToken);
//退出数据中心
DATA_CENTER_API void ExitDataCenter();
//创建打点任务C API
DATA_CENTER_API void CreateDataTaskC(char* pEventName, char* pJsonParam, int nUid);
//设置广告ID
DATA_CENTER_API void SetDataCenterID(int nAdID);
//设置版本号
DATA_CENTER_API void SetDataCenterVersion(const char* pVersion);

#ifdef __cplusplus
}
#endif

//创建打点任务 C++ API
DATA_CENTER_API void CreateDataTask(const char* pEventName, const Json::Value& vParams = Json::Value(Json::objectValue), int nUid=0);