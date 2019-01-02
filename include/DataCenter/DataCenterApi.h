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

//��ʼ����������
DATA_CENTER_API void InitDataCenter(const char* pToken);
//�˳���������
DATA_CENTER_API void ExitDataCenter();
//�����������C API
DATA_CENTER_API void CreateDataTaskC(char* pEventName, char* pJsonParam, int nUid);
//���ù��ID
DATA_CENTER_API void SetDataCenterID(int nAdID);
//���ð汾��
DATA_CENTER_API void SetDataCenterVersion(const char* pVersion);

#ifdef __cplusplus
}
#endif

//����������� C++ API
DATA_CENTER_API void CreateDataTask(const char* pEventName, const Json::Value& vParams = Json::Value(Json::objectValue), int nUid=0);