#pragma once

#include "resource.h"




bool	RunNewUpdate(const Json::Value& vPid, const Json::Value& vCmd, const Json::Value& vVersion);

//�����Ը�������
bool	RunTempUpdate(const wstring& strTempPath);

/***************************
*	0	����ʧ��
*	1	�Ը���
*	2	��ǰ�������°汾
*	3	�ɸ���
*/
UpdateStatus CheckUpdate(wstring& strError);

//��ȡ���¸�Ŀ¼
bool GetUpdateRootPath(const wstring& strChannelName);
