#include "StdAfx.h"
#include "TaskIM.h"
#include "HttpInterface.h"
/***************************************************************************************************************************************************************
token：网易云通信ID的密码
创建网易云通信ID时如果第三方APP自己指定，可以为加密后的密码，也可以为第三方为帐号生成的一个加密的密钥。
此token在创建网易云通信ID时指定，在app直连网易云通信imserver时带上即可。
此token在第三方被修改时，请同步调用网易云通信webserver进行修改，以免引起用户登陆失败;
创建网易云通信ID时如果不指定此token，则网易云通信会为该ID生成一个加密的token，第三方需要保存此token。
同样的，app在登录时需要进行认证，在通过网易云通信imserver进行连接时，需要带上此token。
同理，此token在第三方被修改时，请同步调用网易云通信webserver进行修改，以免引起用户登陆失败（此token原则上没有过期时间，如果发现token丢失可以使用接口刷新token）；
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
			OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
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
				OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
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
			OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
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