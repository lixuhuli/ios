#pragma once
#include "Task.h"
#include "GlobalData.h"
#include "HttpInterface.h"

namespace TaskCenter{

	//联想关键字搜索类
	class CTaskSearch
		:public ITask
	{
	public:
		CTaskSearch(const MSG& msg, const wstring& strWord, bool bAuto, int nPage)
			: ITask(msg)
			, m_strWord(strWord)
			, m_bAuto(bAuto)
			, m_nPage(nPage)
		{
			m_type = TaskSearchAuto;
			m_bUsed = TRUE;
		}
		void Init(const MSG& msg, const wstring& strWord, bool bAuto, int nPage)
		{
			m_msg = msg;
			m_strWord = strWord;
			m_bAuto = bAuto;
			m_nPage = nPage;
			m_bUsed = TRUE;
		}
		const wstring& GetSearchWord()const
		{
			return m_strWord;
		}

	protected:
		virtual void Run()
		{
			Json::Value vParam, vMsg;
			CGlobalData::Instance()->InitPhpParams(vParam);
			vMsg["args"]["keyword"] = PublicLib::UToUtf8(m_strWord);
			vMsg["args"]["pc"] = 6;//一页总数
			vMsg["args"]["pn"] = m_nPage;//翻页数
			vParam["messages"] = vMsg;
			Json::FastWriter wr;
			string strParam = wr.write(vParam);
			PublicLib::CHttpClient http;
			m_strResult = http.Request(m_bAuto ? URL_SEARCH_AUTO : URL_SEARCH_WORD, PublicLib::Post, strParam.c_str(), strParam.size());
			if (m_strResult.empty())
			{
				OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
			}
		}

	private:
		int m_nPage;
		bool m_bAuto;//是否联想搜索
		wstring m_strWord;
	};

	class CTaskRecommend
		:public ITask
	{
	public:
		CTaskRecommend(const MSG& msg, int nPage)
			: ITask(msg)
			, m_nPage(nPage)
		{
			m_type = TaskRecommend;
			m_bUsed = TRUE;
		}
		void Init(const MSG& msg, int nPage)
		{
			m_msg = msg; 
			m_nPage = nPage;
			m_bUsed = TRUE;
		}

	protected:
		virtual void Run()
		{
			Json::Value vParam, vMsg;
			CGlobalData::Instance()->InitPhpParams(vParam);
			vMsg["args"]["pn"] = m_nPage;//翻页数
			vParam["messages"] = vMsg;
			Json::FastWriter wr;
			string strParam = wr.write(vParam);
			PublicLib::CHttpClient http;
			m_strResult = http.Request(URL_RECOMMEND, PublicLib::Post, strParam.c_str(), strParam.size());
			if (m_strResult.empty())
			{
				OUTPUT_XYLOG(LEVEL_ERROR, L"接口请求失败，错误信息：%s", http.GetError());
			}
		}

	private:
		int m_nPage;
	};
}