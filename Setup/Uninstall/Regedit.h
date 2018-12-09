#pragma once
#include <string>
using std::wstring;
#include <vector>
using std::vector;

//ע���װ��Ϣ
struct INSTALL_INFO
{
	wstring	strKeyName;			//дע�����������
	wstring	strExePath;			//��װ����������·��
	wstring strDisplayName;		//�������
	wstring strVersion;			//�汾��
	wstring strPublisher;		//��˾��	
	wstring strUninstPath;		//ж�س���·��
	wstring strUrl;				//����URL
	wstring strUpExeVersion;	//���³���汾
	wstring strChannel;			//������Ϣ
	DWORD dwAutoRun;
};

/*********************************************************************************
ע������
*/

//д�����ж����Ϣע�����
bool	RegWriteUninstInfo(const INSTALL_INFO& info);
//ɾ�����ж����Ϣע�����
bool	RegDelUninstInfo(const wstring& strKeyName);

bool	RegGetInstallInfo(const wstring& strKeyName, wstring& strInstallPath, wstring& strVersion, wstring& strUpExeVersion, wstring& strChannelName);

//�����Ը��³���汾
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

//��ȡ������Ϣ
//bool	RegGetChannelInfo(const wstring& strKeyName, wstring& strChannelName, bool bIs64Key);

//����������Ϣ
//bool	RegUpdateChannelInfo(const wstring& strKeyName, const wstring& strNewVersion, bool bIs64Key);

//ɾ������������
bool	RegDelBootRun(const wstring& strKeyName);

bool	RegGetUpdateExeVersion(const wstring& strKeyName, wstring& strVersion);

bool	RegSetUpdateExeVersion(const wstring& strKeyName, const wstring& strVersion);

bool	RegWritePca();


