#include "StdAfx.h"
#include "TaskIM.h"
#include "HttpInterface.h"
/***************************************************************************************************************************************************************
token��������ͨ��ID������
����������ͨ��IDʱ���������APP�Լ�ָ��������Ϊ���ܺ�����룬Ҳ����Ϊ������Ϊ�ʺ����ɵ�һ�����ܵ���Կ��
��token�ڴ���������ͨ��IDʱָ������appֱ��������ͨ��imserverʱ���ϼ��ɡ�
��token�ڵ��������޸�ʱ����ͬ������������ͨ��webserver�����޸ģ����������û���½ʧ��;
����������ͨ��IDʱ�����ָ����token����������ͨ�Ż�Ϊ��ID����һ�����ܵ�token����������Ҫ�����token��
ͬ���ģ�app�ڵ�¼ʱ��Ҫ������֤����ͨ��������ͨ��imserver��������ʱ����Ҫ���ϴ�token��
ͬ����token�ڵ��������޸�ʱ����ͬ������������ͨ��webserver�����޸ģ����������û���½ʧ�ܣ���tokenԭ����û�й���ʱ�䣬�������token��ʧ����ʹ�ýӿ�ˢ��token����
*/
namespace TaskCenter{
	CTaskIMToken::CTaskIMToken(const MSG& msg, const Json::Value& vUserInfo)
		: ITask(msg)
		, m_vUserInfo(vUserInfo)
	{
		m_type = TaskIMCreateToken;
		m_bUsed = TRUE;
	}


	CTaskIMToken::~CTaskIMToken()
	{
	}

	void CTaskIMToken::Init(const MSG& msg, const Json::Value& vUserInfo)
	{
		m_msg = msg;
		m_vUserInfo = vUserInfo;
		m_bUsed = TRUE;
	}

	void CTaskIMToken::Run()
	{
		PublicLib::CHttpClient http(PublicLib::ContentJson);
		Json::FastWriter fw;
		string strPost = fw.write(m_vUserInfo);
		m_strResult = http.Request(URL_IM_CREATE_TOKEN, PublicLib::Post, strPost.c_str(), strPost.size());
		wstring str = PublicLib::Utf8ToU(m_strResult);
		if (m_strResult.empty())
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"�ӿ�����ʧ�ܣ�������Ϣ��%s", http.GetError());
			return;
		}
		//{"desc":"already register","code":414}
		if (m_strResult.find("414") != string::npos)
		{
			strPost = m_vUserInfo["accid"].asString();
			m_strResult = http.Request(URL_IM_UPDATE_TOKEN, PublicLib::Post, strPost.c_str(), strPost.size());
		}
		else
		{
			PublicLib::CHttpClient http(PublicLib::ContentJson);
			string strRet = http.Request(URL_IM_UPDATE_USERINFO, PublicLib::Post, strPost.c_str(), strPost.size());
			if (strRet.empty())
			{
				OUTPUT_XYLOG(LEVEL_ERROR, L"�ӿ�����ʧ�ܣ�������Ϣ��%s", http.GetError());
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CTaskIMUpdateInfo::CTaskIMUpdateInfo(const string& strJsonInfo)
		: m_strJsonInfo(strJsonInfo)
	{
		m_type = TaskIMUpdateInfo;
		m_bUsed = TRUE;
	}

	void CTaskIMUpdateInfo::Init(const string& strJsonInfo)
	{
		m_strJsonInfo = strJsonInfo;
		m_bUsed = TRUE;
	}

	void CTaskIMUpdateInfo::Run()
	{
		PublicLib::CHttpClient http(PublicLib::ContentJson);
		m_strResult = http.Request(URL_IM_UPDATE_USERINFO, PublicLib::Post, m_strJsonInfo.c_str(), m_strJsonInfo.size());
		wstring str = PublicLib::Utf8ToU(m_strResult);
		if (m_strResult.empty())
		{
			OUTPUT_XYLOG(LEVEL_ERROR, L"�ӿ�����ʧ�ܣ�������Ϣ��%s", http.GetError());
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CTaskIMCheckMsg::CTaskIMCheckMsg(const MSG& msg, int nUid, const string& strToken, const string& strMsg)
		: ITask(msg)
		, m_nUid(nUid)
		, m_strToken(strToken)
		, m_strMsg(strMsg)
	{
		m_type = TaskIMCheckMsg;
		m_bUsed = TRUE;
	}

	void CTaskIMCheckMsg::Init(const MSG& msg, int nUid, const string& strToken, const string& strMsg)
	{
		m_msg = msg;
		m_nUid = nUid;
		m_strToken = strToken;
		m_strMsg = strMsg;
		m_bUsed = TRUE;
	}

	void CTaskIMCheckMsg::Run()
	{
		PublicLib::CHttpClient http(PublicLib::ContentJson);
		Json::Value vData;
		vData["uid"] = m_nUid;
		vData["token"] = m_strToken;
		vData["message"] = m_strMsg;
		Json::FastWriter fw;
		string strJson = fw.write(vData);
		m_strResult = http.Request(URL_IM_GET_CHECKMSG, PublicLib::Post, strJson.c_str(), strJson.size());
	}

}