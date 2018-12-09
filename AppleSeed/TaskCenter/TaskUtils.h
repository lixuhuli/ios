#pragma once
#include "Task.h"
#include "RegBoot.h"
#include "SoftDefine.h"
#include "CxImg/xImageApi.h"


namespace TaskCenter{
	class CTaskBootRun
			:public ITask
		{
		public:
			CTaskBootRun(bool bBootRun)
				: m_bBootRun(bBootRun)
			{
				m_type = TaskBootRun;
				m_bUsed = TRUE;
			}

			void Init(bool bBootRun)
			{
				m_bBootRun = bBootRun;
				m_bUsed = TRUE;
			}

		protected:
			virtual void Run()
			{
				if (m_bBootRun)
				{
					wchar_t szPath[MAX_PATH] = { 0 };
					GetModuleFileName(NULL, szPath, MAX_PATH);
					wstring strCmd(L"\"");
					strCmd.append(szPath);
					strCmd.append(L"\"");
					strCmd.append(L" /bootrun");
					AddBootRun(REG_ROOT_NAME, strCmd);
				}
				else
				{
					DelBootRun(REG_ROOT_NAME);
				}
			}

		private:
			bool m_bBootRun;
		};

	class CTaskDelDir
			:public ITask
		{
		public:
			CTaskDelDir(const list<wstring>& listDirs, const list<wstring>& listFiles)
				: m_listDirs(listDirs)
				, m_listFiles(listFiles)
			{
				m_type = TaskDelDir;
				m_bUsed = TRUE;
			}

			void Init(const list<wstring>& listDirs, const list<wstring>& listFiles)
			{
				m_listDirs = listDirs;
				m_listFiles = listFiles;
				m_bUsed = TRUE;
			}

		protected:
			virtual void Run()
			{
				for (auto itor = m_listFiles.begin(); itor != m_listFiles.end(); ++itor)
				{
					::DeleteFile(itor->c_str());
				}
				for (auto itor = m_listDirs.begin(); itor != m_listDirs.end(); ++itor)
				{
					PublicLib::RemoveDir(itor->c_str());
				}
			}

		private:
			list<wstring> m_listFiles;
			list<wstring> m_listDirs;
		};

	class CTaskPlayerIco
			: public ITask
		{
		public:
			CTaskPlayerIco(const MSG& msg)
				: ITask(msg)
				, m_hEvent(NULL)
			{
				m_type = TaskPlayerIco;
				m_bUsed = TRUE;
				m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			}

			~CTaskPlayerIco()
			{
				if (m_hEvent)
					CloseHandle(m_hEvent);
				Clear();
			}

			void Init(const MSG& msg)
			{
				m_msg = msg;
				ResetEvent(m_hEvent);
				m_bUsed = TRUE;
			}

			bool AddPlayerIco(LPlayerIcoParam lpParam)
			{
				if (lpParam->nUid != 0)
				{
					if (std::find(m_uidList.begin(), m_uidList.end(), lpParam->nUid) != m_uidList.end())
					{
						return false;
					}
				}
				CScopeLock lock(m_lock);
				m_paramList.push_back(lpParam);
				m_uidList.push_back(lpParam->nUid);
				return true;
			}

			void Stop()
			{
				memset(&m_msg, 0, sizeof(MSG));
				SetEvent(m_hEvent);
			}

		protected:
			virtual void Clear()
			{
				CScopeLock lock(m_lock);
				for (auto itor = m_paramList.begin(); itor != m_paramList.end(); ++itor)
				{
					delete *itor;
				}
				m_paramList.clear();
				m_uidList.clear();
			}

			virtual void Run()
			{
				LPlayerIcoParam lpPlayerParam = NULL;
				DWORD dwWaitTime = 500;
				PublicLib::CHttpClient http;
				while (WaitForSingleObject(m_hEvent, dwWaitTime) == WAIT_TIMEOUT)
				{
					{
						CScopeLock lock(m_lock);
						if (m_paramList.empty())
						{
							dwWaitTime = 500;
							continue;
						}
						lpPlayerParam = *m_paramList.begin();
						m_paramList.pop_front();
					}
					if (WaitForSingleObject(m_hEvent, 10) != WAIT_TIMEOUT)
						break;
					wstring strUrl = PublicLib::AToU(lpPlayerParam->strUrl);
					byte* pImgData = NULL;
					DWORD dwImgSize = 0;
					//strUrl = L"http://ctimg.5fun.com/upload/images//headportraitv3/20180124/7e3fb3359350159414a3d88c42701db4_1516782268.jpg";
					bool bSuccess = http.DownLoadMem(strUrl.c_str(), (void**)&pImgData, dwImgSize);
					if (bSuccess)
					{
						CompressImageDataW(pImgData, dwImgSize, 49, 49, 100, lpPlayerParam->strSavePath.c_str());
						::SendMessage(m_msg.hwnd, m_msg.message, 0, (LPARAM)lpPlayerParam);
					}
					if (pImgData)
						free(pImgData);
					delete lpPlayerParam;
					lpPlayerParam = NULL;
					dwWaitTime = 1;
				}
			}

		private:
			CLock m_lock;
			HANDLE m_hEvent;
			list<LPlayerIcoParam> m_paramList;
			list<int> m_uidList;
		};

	class CTaskGameTabList
		:public ITask
	{
	public: 
		CTaskGameTabList(const MSG& msg)
			: ITask(msg)
		{
			m_type = TaskGameTabList;
			m_bUsed = TRUE;
		}

		void Init(const MSG& msg)
		{
			m_msg = msg;
			m_bUsed = TRUE;
		}

	protected:
		virtual void Run()
		{
			PublicLib::CHttpClient http;
			m_strResult = http.Request(URL_GAME_TAB, PublicLib::Get);
		}

	private:

	};
	class CTaskGameRecommand
		:public ITask
	{
	public:
		CTaskGameRecommand(const MSG& msg)
			: ITask(msg)
		{
			m_type = TaskGameRecommand;
			m_bUsed = TRUE;
		}

		void Init(const MSG& msg)
		{
			m_msg = msg;
			m_bUsed = TRUE;
		}

	protected:
		virtual void Run()
		{
			PublicLib::CHttpClient http;
			m_strResult = http.Request(L"http://anv3frapi.papa91.com/battle_pc/game_start_recommend/index", PublicLib::Get);
		}

	private:

	};

	class CTaskGameRecommandIco
		:public ITask
	{
	public:
		CTaskGameRecommandIco(const MSG& msg, const wstring& strIcoUrl, const wstring& strSavePath)
			: ITask(msg)
			, m_strIcoUrl(strIcoUrl)
			, m_strSavePath(strSavePath)
		{
			m_type = TaskGameRecommandIco;
			m_bUsed = TRUE;
		}

		void Init(const MSG& msg, const wstring& strIcoUrl, const wstring& strSavePath)
		{
			m_msg = msg;
			m_strIcoUrl = strIcoUrl;
			m_strSavePath = strSavePath;
			m_bUsed = TRUE;
		}

	protected:
		virtual void Run()
		{
			PublicLib::CHttpClient http;
			byte* pImgData = NULL;
			DWORD dwImgSize = 0;
			if (http.DownLoadMem(m_strIcoUrl.c_str(), (void**)&pImgData, dwImgSize))
			{
				CompressImageDataW(pImgData, dwImgSize, 60, 60, 100, m_strSavePath.c_str());
				if (pImgData)
					free(pImgData);
				m_strResult = "1";
			}
			else
			{
				OUTPUT_XYLOG(LEVEL_ERROR, L"”Œœ∑Õº±Íœ¬‘ÿ ß∞‹£¨URL = %s", m_strIcoUrl.c_str());
				m_strResult = "0";
			}
		}

	private:
		wstring m_strIcoUrl;
		wstring m_strSavePath;
	};

	class CTaskDownloadFile
	:public ITask
{
public:
	CTaskDownloadFile(const MSG& msg, const string& strUrl, const wstring& strSavePath)
		: ITask(msg)
		, m_strSavePath(strSavePath)
	{
		m_strUrl = PublicLib::AToU(strUrl);
		m_bUsed = TRUE;
		m_type = TaskDownloadFile;
	}

	void Init(const MSG& msg, const string& strUrl, const wstring& strSavePath)
	{
		m_msg = msg;
		m_strUrl = PublicLib::AToU(strUrl);
		m_strSavePath = strSavePath;
		m_bUsed = TRUE;
	}
	const wstring& GetSavePath()
	{
		return m_strSavePath;
	}

protected:
	virtual void Run()
	{
		PublicLib::CHttpClient http;
		m_strResult = http.DownLoadFile(m_strUrl.c_str(), m_strSavePath.c_str()) ? "1" : "0";
	}

private:
	wstring m_strUrl;
	wstring m_strSavePath;
};

	class CTaskUrl
		: public ITask
	{
	public:
		CTaskUrl(const MSG& msg, const wstring& strUrl)
			: ITask(msg)
			, m_strUrl(strUrl)
		{
			m_bUsed = TRUE;
			m_type = TaskUrl;
		}

		void Init(const MSG& msg, const wstring& strUrl)
		{
			m_bUsed = TRUE;
			m_msg = msg;
			m_strUrl = strUrl;
		}

	protected:
		virtual void Run()
		{
			PublicLib::CHttpClient http;
			m_strResult = http.Request(m_strUrl.c_str(), PublicLib::Get);
		}

	private:
		wstring m_strUrl;
	};

	class CTaskCheckGameUpdate
		: public ITask
	{
	public:
		CTaskCheckGameUpdate(const MSG& msg, const std::map<string, string>& gameList)
			: ITask(msg)
			, m_gameList(gameList)
		{
			m_bUsed = TRUE;
			m_type = TaskCheckGameUpdate;
		}

		void Init(const MSG& msg, const std::map<string, string>& gameList)
		{
			m_bUsed = TRUE;
			m_msg = msg;
			m_gameList = gameList;
		}

	protected:
		virtual void Run()
		{
			PublicLib::CHttpClient http(PublicLib::ContentJson);
			Json::Value vRoot;
			Json::Value vArray(Json::arrayValue);
			for (auto itor = m_gameList.begin(); itor != m_gameList.end(); ++itor)
			{
				Json::Value vItem;
				vItem["game_id"] = itor->first;
				vItem["version"] = itor->second;
				vArray.append(vItem);
			}
			vRoot["game_list"] = vArray;
			Json::FastWriter fw;
			string strJson = fw.write(vRoot);
			m_strResult = http.Request(URL_CHECK_GAME_UPDATE, PublicLib::Post, strJson.c_str(), strJson.size());
			m_gameList.clear();
		}

	private:
		std::map<string, string> m_gameList;
	};
}