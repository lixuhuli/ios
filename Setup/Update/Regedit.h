#pragma once
#include <string>
using std::wstring;
#include <vector>
using std::vector;

//注册表安装信息
struct INSTALL_INFO
{
	wstring	strKeyName;			//写注册表主键名称
	wstring	strExePath;			//安装主程序完整路径
	wstring strDisplayName;		//软件名称
	wstring strVersion;			//版本号
	wstring strPublisher;		//公司名	
	wstring strUninstPath;		//卸载程序路径
	wstring strUrl;				//官网URL
	wstring strUpExeVersion;	//更新程序版本
	wstring strChannel;			//渠道信息
	DWORD dwAutoRun;
};

/*********************************************************************************
注册表操作
*/

//写入软件卸载信息注册表项
bool	RegWriteUninstInfo(const INSTALL_INFO& info);
//删除软件卸载信息注册表项
bool	RegDelUninstInfo(const wstring& strKeyName);

bool	RegGetInstallInfo(const wstring& strKeyName, wstring& strInstallPath, wstring& strVersion, wstring& strUpExeVersion, wstring& strChannelName);

//更新自更新程序版本
bool	RegWriteUpdateexeVersion(const wstring& strKeyName, const wstring& strVersion);

enum RegListType
{
	RegComList,
	RegConflictList,
	RegUnsetupCmdList,
};

bool	RegWriteList(const wstring& strKeyName, const vector<wstring>& comList, RegListType type);

bool	RegReadList(const wstring& strKeyName, vector<wstring>& comList, RegListType type);

bool	RegWriteDefChannel(const wstring& strKeyName, const wstring& strChannelName);

//获取渠道信息
//bool	RegGetChannelInfo(const wstring& strKeyName, wstring& strChannelName, bool bIs64Key);

//更新渠道信息
//bool	RegUpdateChannelInfo(const wstring& strKeyName, const wstring& strNewVersion, bool bIs64Key);

//删除开机启动项
bool	RegDelBootRun(const wstring& strKeyName);

bool	RegGetUpdateExeVersion(const wstring& strKeyName, wstring& strVersion);

bool	RegSetUpdateExeVersion(const wstring& strKeyName, const wstring& strVersion);

bool	RegWritePca();


