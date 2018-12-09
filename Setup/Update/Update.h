#pragma once

#include "resource.h"




bool	RunNewUpdate(const Json::Value& vPid, const Json::Value& vCmd, const Json::Value& vVersion);

//进入自更新流程
bool	RunTempUpdate(const wstring& strTempPath);

/***************************
*	0	更新失败
*	1	自更新
*	2	当前已是最新版本
*	3	可更新
*/
UpdateStatus CheckUpdate(wstring& strError);

//获取更新根目录
bool GetUpdateRootPath(const wstring& strChannelName);
