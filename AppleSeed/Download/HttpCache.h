#pragma once
#include <list>
using std::list;
#include <PublicLib/HttpLib.h>
using PublicLib::CHttpDownload;



#define MAX_CACHE_COUNT			10
class CHttpCache
{
public:
	CHttpCache()
	{

	}
	~CHttpCache()
	{
		Release();
	}
	void Add(CHttpDownload* pHttp)
	{
		if ( NULL == pHttp )
			return;
		if (pHttp->IsHasExited())
		{
			delete pHttp;
			return ;
		}
		if (m_cacheList.size()>MAX_CACHE_COUNT)
		{
			list<CHttpDownload*>::iterator itor = m_cacheList.begin();
			while (itor != m_cacheList.end())
			{
				CHttpDownload* pHttp = *itor;
				if (pHttp->IsHasExited())
				{
					delete pHttp;
					itor = m_cacheList.erase(itor);
					continue;
				}
				itor++;
			}
		}
		m_cacheList.push_back(pHttp);
	}

protected:
	void Release()
	{
		list<CHttpDownload*>::iterator itor = m_cacheList.begin();
		while (itor != m_cacheList.end())
		{
			CHttpDownload* pHttp = *itor;
			delete pHttp;
			itor++;
		}
		m_cacheList.clear();
		OutputDebugString(L"CHttpDeleteListÎö¹¹Íê³É++++++++++++++++++++++\n");
	}

private:
	list<CHttpDownload*> m_cacheList;
};



