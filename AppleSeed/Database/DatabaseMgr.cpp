#include "StdAfx.h"
#include "DatabaseMgr.h"
#include "GlobalData.h"
#include <atlstr.h>
#include <algorithm>

#define DATABASE_VERSION	"1.0"
#define SOFT_VERSION		"1.0"
#define ANDROID_EMU_VERSION	"1.0.1"

CDatabaseMgr::CDatabaseMgr() {
}

CDatabaseMgr::~CDatabaseMgr() {
}

bool CDatabaseMgr::Init()
{
	bool bResult = false;
	std::wstring strDbPath = CGlobalData::Instance()->GetDocPath() + L"\\AppleSeed.db";
	try
	{
		m_strDbPath = PublicLib::UToUtf8(strDbPath);
		bResult = Open(m_strDbPath);
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s", strDbPath.c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
		return false;
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"打开数据库失败，未知错误");
		return false;
	}
	return bResult;
}

void CDatabaseMgr::Exit()
{
	m_sqliteDB.close();
	ReleaseGameInfo();
	OUTPUT_XYLOG(LEVEL_INFO, L"数据库模块退出完毕");
}

bool CDatabaseMgr::Open(const string& strDbFile)
{
	try
	{
		m_sqliteDB.open(strDbFile.c_str());
		if (!m_sqliteDB.tableExists(TB_VERSION))
		{
			m_sqliteDB.execDML(SQL_CREATE_DB_VERSION);
			CStringA strSql;
			strSql.Format("insert into %s(database_version, software_version) values('%s','%s')", TB_VERSION, DATABASE_VERSION, SOFT_VERSION);
			m_sqliteDB.execDML(strSql);
		}
		else
		{//表已经存在，读取版本信息
 			CppSQLite3Query query = m_sqliteDB.execQuery(SQL_READ_DB_VERSION);
            startup_db_version_ = query.getStringField("database_version");
		}

		GetDefSetting(m_setting);

		if (!m_sqliteDB.tableExists(TB_SETTING))
		{
			m_sqliteDB.execDML(SQL_CREATE_DB_SETTING);
			CStringA strSql;
			strSql.Format("insert into %s(exit, max_load_speed, load_path) values('%d','%d', '%s')", TB_SETTING, m_setting.nExit,
				m_setting.nMaxLoadSpeed, m_setting.strLoadPath.c_str());
			m_sqliteDB.execDML(strSql);
		}
		else
		{
			ReadSettingData();
		}
		if (!m_sqliteDB.tableExists(TB_LOADING))
		{
			m_sqliteDB.execDML(SQL_CREATE_DB_LOADING);
		}
		if (!m_sqliteDB.tableExists(TB_LOADED))
		{
			m_sqliteDB.execDML(SQL_CREATE_DB_LOADED);
		}
		if (!m_sqliteDB.tableExists(TB_GAMEINFO))
		{
			m_sqliteDB.execDML(SQL_CREATE_DB_GAMEINFO);
		}
		else
		{
			ReadGameInfos();
		}
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s", PublicLib::Utf8ToU(m_strDbPath).c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
		return false;
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"打开数据库失败，未知错误");
		return false;
	}
	return true;
}


void CDatabaseMgr::GetSetting(OUT AppSetting& setting)
{
	setting = m_setting;
}

void CDatabaseMgr::GetDefSetting(OUT AppSetting& setting)
{
	setting.nExit = 0;
	setting.nMaxLoadSpeed = 0;
	setting.strLoadPath = PublicLib::UToUtf8(CGlobalData::Instance()->GetDefLoadPath());
}

void CDatabaseMgr::SetSetting(const AppSetting& setting)
{
	m_setting = setting;
	UpdateSettingData();
}

wstring CDatabaseMgr::GetLoadPath()
{
	wstring strPath = PublicLib::Utf8ToU(m_setting.strLoadPath);
	if (!PathFileExists(strPath.c_str()))
	{
		SHCreateDirectory(NULL, strPath.c_str());
	}
	return strPath;
}

int CDatabaseMgr::AddTableField(const char* table_name, const char* field_name, const char* field_type)
{
	try
	{
		if (m_sqliteDB.tableExists(table_name))
		{
			bool field_exist = false;
			CStringA strSql;
			strSql.Format("select * from %s", table_name);
			CppSQLite3Query& q = m_sqliteDB.execQuery(strSql);
			for (int i = 0; i < q.numFields(); i++)
			{
				const char* pName = q.fieldName(i);
				if (pName && stricmp(pName, field_name) == 0)
				{
					field_exist = true;
					break;
				}
			}
			if (!field_exist)
			{
				CStringA strSql;
				strSql.Format("alter table %s add column %s %s", table_name, field_name, field_type);
				m_sqliteDB.execDML(BEGIN_TRASACTION);
				m_sqliteDB.execDML(strSql);
				m_sqliteDB.execDML(COMMIT_TRANSACTION);
			}
		}
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s",
			PublicLib::Utf8ToU(m_strDbPath).c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
	}
	return 0;
}

bool CDatabaseMgr::GetLoadTasks(OUT vector<ITask*> &loadList)
{
	loadList.clear();
	try
	{
		CppSQLite3DB db;
		db.open(m_strDbPath.c_str());
		CppSQLite3Query q = db.execQuery(SQL_READ_DB_LOADING);
		loadList.reserve(q.numFields());
		while (!q.eof())
		{
			ITask* pTask = new ITask;
			pTask->strUrl = q.getStringField("url");
			pTask->strName = PublicLib::Utf8ToU(q.getStringField("title"));
            pTask->strVersion = PublicLib::Utf8ToU(q.getStringField("version"));
			pTask->nTotalSize = q.getInt64Field("total_size");
			pTask->nLoadSize = q.getInt64Field("load_size");
			pTask->nGameID = q.getInt64Field("game_id");
			pTask->strSavePath = PublicLib::Utf8ToU(q.getStringField("save_path"));
			int nState = q.getIntField("load_state");
			if (nState < 0 || nState >= (int)Ts_End)
			{
				nState = 0;
			}
			else if (nState == Ts_Loading || nState == Ts_Wait || nState == Ts_Install || nState == Ts_CheckEnd || nState == Ts_Error)
			{
				nState = (int)Ts_Pause;
			}
			pTask->state = (TaskState)nState;
			pTask->strGameInfo = q.getStringField("game_info");
			pTask->strGameTags = q.getStringField("tag_list");
			pTask->gameType = (GameType)q.getIntField("game_type");
			pTask->strPkgName = q.getStringField("pkg_name");
			pTask->strSyncMem = q.getStringField("r1");
			pTask->type = (FileType)q.getIntField("file_type");
            pTask->strVer = PublicLib::Utf8ToU(q.getStringField("ver"));
            pTask->strSourceVer = PublicLib::Utf8ToU(q.getStringField("source_ver"));
            pTask->strConfigVer = PublicLib::Utf8ToU(q.getStringField("config_ver"));
            pTask->strIconUrl = q.fieldIsNull("icon_url") ? "" : q.getStringField("icon_url");

			loadList.push_back(pTask);
			q.nextRow();
		}
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s", 
			PublicLib::Utf8ToU(m_strDbPath).c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
		return false;
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"未知错误");
		return false;
	}
	return true;
}

bool CDatabaseMgr::InsertLoadTask(ITask* pTask)
{
	try
	{
		CppSQLite3DB db;
		db.open(m_strDbPath.c_str());
		CStringA strSql;
		strSql.Format(SQL_INSERT_DB_LOADING, pTask->strUrl.c_str(), PublicLib::UToUtf8(pTask->strName).c_str(), PublicLib::UToUtf8(pTask->strVersion).c_str(), 
			pTask->nTotalSize, pTask->nLoadSize, PublicLib::UToUtf8(pTask->strSavePath).c_str(),
			(int)pTask->state, pTask->nGameID, pTask->strGameInfo.c_str(), pTask->strGameTags.c_str(), (int)pTask->gameType, pTask->strPkgName.c_str(),
            pTask->strSyncMem.c_str(), (int)pTask->type, PublicLib::UToUtf8(pTask->strVer).c_str(), PublicLib::UToUtf8(pTask->strSourceVer).c_str(), PublicLib::UToUtf8(pTask->strConfigVer).c_str(), pTask->strIconUrl.c_str());
		int nResult = db.execDML(strSql);
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s", PublicLib::Utf8ToU(m_strDbPath).c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
		return false;
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"未知错误");
		return false;
	}
	return true;
}

bool CDatabaseMgr::UpdateLoadTask(ITask* pTask)
{
	try
	{
		CppSQLite3DB db;
		db.open(m_strDbPath.c_str());
		db.execDML(BEGIN_TRASACTION);
		CppSQLite3Statement stat = db.compileStatement(SQL_UPDATE_DB_LOADING);
		int index = 0;
		stat.bind(++index, pTask->nTotalSize);
		stat.bind(++index, pTask->nLoadSize);
		stat.bind(++index, (int)pTask->state);
        stat.bind(++index, PublicLib::UToUtf8(pTask->strVer).c_str());
        stat.bind(++index, PublicLib::UToUtf8(pTask->strSourceVer).c_str());
        stat.bind(++index, PublicLib::UToUtf8(pTask->strConfigVer).c_str());
        stat.bind(++index, pTask->strIconUrl.c_str());
		stat.bind(++index, pTask->nGameID);
		stat.execDML();
		stat.reset();
		db.execDML(COMMIT_TRANSACTION);
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s", PublicLib::Utf8ToU(m_strDbPath).c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
		return false;
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"未知错误");
		return false;
	}
	return true;
}

bool CDatabaseMgr::DeleteLoadTask(ITask* pTask)
{
	try
	{
		CppSQLite3DB db;
		db.open(m_strDbPath.c_str());
		db.execDML(BEGIN_TRASACTION);
		CppSQLite3Statement stmt = db.compileStatement(SQL_DELETE_DB_LOADING);
		stmt.bind(1, pTask->nGameID);
		stmt.execDML();
		stmt.reset();
		db.execDML(COMMIT_TRANSACTION);
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s", PublicLib::Utf8ToU(m_strDbPath).c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
		return false;
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"未知错误");
		return false;
	}
	return true;
}

bool CDatabaseMgr::DeleteLoadTaskList(const std::vector<__int64>& gameIDList)
{
	try
	{
		CppSQLite3DB db;
		db.open(m_strDbPath.c_str());
		db.execDML(BEGIN_TRASACTION);
		CppSQLite3Statement stmt = db.compileStatement(SQL_DELETE_DB_LOADING);
		for (auto itor = gameIDList.begin(); itor != gameIDList.end(); ++itor)
		{
			stmt.bind(1, *itor);
			stmt.execDML();
			stmt.reset();
		}
		db.execDML(COMMIT_TRANSACTION);
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s", PublicLib::Utf8ToU(m_strDbPath).c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
		return false;
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"未知错误");
		return false;
	}
	return true;
}

bool CDatabaseMgr::GetFinishTasks(OUT vector<ITask*> &finishList)
{
	finishList.clear();
	try
	{
		CppSQLite3DB db;
		db.open(m_strDbPath.c_str());
		CppSQLite3Query q = db.execQuery(SQL_READ_DB_LOADED);
		finishList.reserve(q.numFields());
		while (!q.eof())
		{
			ITask* pTask = new ITask;
			pTask->strGameInfo = q.getStringField("info");
			pTask->strName = PublicLib::Utf8ToU(q.getStringField("title"));
            pTask->strVersion = PublicLib::Utf8ToU(q.getStringField("version"));
			pTask->nTotalSize = q.getInt64Field("total_size");
			pTask->strSavePath = PublicLib::Utf8ToU(q.getStringField("save_path"));
			pTask->nGameID = q.getInt64Field("game_id");
			pTask->nDate = q.getInt64Field("date");
			pTask->gameType = (GameType)q.getIntField("game_type");
			int nType = q.getIntField("file_type");
			pTask->type = (nType == 0) ? Ft_Game : (FileType)nType;
			finishList.push_back(pTask);
			q.nextRow();
		}
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s", PublicLib::Utf8ToU(m_strDbPath).c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
		return false;
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"未知错误");
		return false;
	}
	return true;
}

bool CDatabaseMgr::InsertFinishTask(ITask* pTask)
{
	try
	{
		CppSQLite3DB db;
		db.open(m_strDbPath.c_str());
		CStringA strSql;
		strSql.Format(SQL_INSERT_DB_LOADED, PublicLib::UToUtf8(pTask->strName).c_str(), PublicLib::UToUtf8(pTask->strVersion).c_str(), pTask->nTotalSize,
            PublicLib::UToUtf8(pTask->strSavePath).c_str(), pTask->nGameID, pTask->nDate, (int)pTask->gameType, (int)pTask->type, pTask->strGameInfo.c_str());
		db.execDML(strSql);
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s", PublicLib::Utf8ToU(m_strDbPath).c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
		return false;
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"未知错误");
		return false;
	}
	return true;
}

bool CDatabaseMgr::DeleteFinishTask(ITask* pTask)
{
	try
	{
		CppSQLite3DB db;
		db.open(m_strDbPath.c_str());
		db.execDML(BEGIN_TRASACTION);
		CppSQLite3Statement stmt = db.compileStatement(SQL_DELETE_DB_LOADED);
		stmt.bind(1, pTask->nGameID);
		stmt.execDML();
		stmt.reset();
		db.execDML(COMMIT_TRANSACTION);
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s", PublicLib::Utf8ToU(m_strDbPath).c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
		return false;
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"未知错误");
		return false;
	}
	return true;
}

bool CDatabaseMgr::ReadGameInfos()
{
	try
	{
		CScopeLock lock(m_lockGameInfo);
		CppSQLite3DB db;
		db.open(m_strDbPath.c_str());
		CppSQLite3Query q = db.execQuery(SQL_READ_DB_GAMEINFO);
		while (!q.eof())
		{
			ITask* pInfo = new ITask;
			pInfo->nGameID = q.getInt64Field("game_id");
			pInfo->strName = PublicLib::Utf8ToU(q.getStringField("name"));
            pInfo->strVer = PublicLib::Utf8ToU(q.getStringField("ver"));
			pInfo->strVersion = PublicLib::Utf8ToU(q.getStringField("version"));
            pInfo->strSourceVer = PublicLib::Utf8ToU(q.getStringField("source_ver"));
            pInfo->strConfigVer = PublicLib::Utf8ToU(q.getStringField("config_ver"));
			pInfo->nTotalSize = q.getInt64Field("size");
			pInfo->strGameInfo = q.getStringField("info");
			pInfo->strGameTags = q.getStringField("tag_list");
			pInfo->strSavePath = PublicLib::Utf8ToU(q.getStringField("save_path"));
			pInfo->gameType = (GameType)q.getIntField("game_type");
			pInfo->strPkgName = q.getStringField("pkg_name");
			pInfo->strSyncMem = q.getStringField("r1");
			pInfo->strRomPath = PublicLib::Utf8ToU(q.getStringField("r2"));
            pInfo->type = (FileType)q.getIntField("file_type");
            pInfo->strIconUrl = q.fieldIsNull("icon_url") ? "" : q.getStringField("icon_url");
            pInfo->favorite = q.fieldIsNull("favorite") ? false : (q.getIntField("favorite") == 1);
	
			m_listInstall.push_back(pInfo);
			q.nextRow();
		}
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s", PublicLib::Utf8ToU(m_strDbPath).c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
		return false;
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"未知错误");
		return false;
	}
	return true;
}

bool CDatabaseMgr::InsertGameInfo(ITask* pTask)
{
	try
	{
		CScopeLock lock(m_lockGameInfo);
		ITask* pOldTask = NULL;
		for (auto itor = m_listInstall.begin(); itor != m_listInstall.end(); ++itor)
		{
			if ((*itor)->nGameID == pTask->nGameID)
			{
				pOldTask = *itor;
				break;
			}
		}
		if (pOldTask)//已经存在这个游戏，更新部分信息即可
		{
            pOldTask->strVer = pTask->strVer;
			pOldTask->strVersion = pTask->strVersion;
            pOldTask->strSourceVer = pTask->strSourceVer;
            pOldTask->strConfigVer = pTask->strConfigVer;
			pOldTask->strGameInfo = pTask->strGameInfo;
			pOldTask->strGameTags = pTask->strGameTags;
			pOldTask->strSyncMem = pTask->strSyncMem;
			pOldTask->strRomPath = pTask->strRomPath;
			pOldTask->strSavePath = pTask->strSavePath;
			pOldTask->nTotalSize = pTask->nTotalSize;
            pOldTask->strIconUrl = pTask->strIconUrl;
            pOldTask->type = pTask->type;
			UpdateGameInfo(pOldTask);
			return true;
		}
		pOldTask = new ITask;
		pOldTask->nGameID = pTask->nGameID;
		pOldTask->strName = pTask->strName;
        pOldTask->strVer = pTask->strVer;
		pOldTask->strVersion = pTask->strVersion;
        pOldTask->strSourceVer = pTask->strSourceVer;
        pOldTask->strConfigVer = pTask->strConfigVer;
		pOldTask->nTotalSize = pTask->nTotalSize;
		pOldTask->strGameInfo = pTask->strGameInfo;
		pOldTask->strGameTags = pTask->strGameTags;
		pOldTask->strSavePath = pTask->strSavePath;
		pOldTask->gameType = pTask->gameType;
		pOldTask->strPkgName = pTask->strPkgName;
		pOldTask->strSyncMem = pTask->strSyncMem;
		pOldTask->strRomPath = pTask->strRomPath;
        pOldTask->type = pTask->type;
        pOldTask->strIconUrl = pTask->strIconUrl;
        pOldTask->favorite = pTask->favorite;
		m_listInstall.push_back(pOldTask);
		CppSQLite3DB db;
		db.open(m_strDbPath.c_str());
		CStringA strSql;
		strSql.Format(SQL_INSERT_DB_GAMEINFO, pTask->nGameID, PublicLib::UToUtf8(pTask->strName).c_str(), PublicLib::UToUtf8(pTask->strVersion).c_str(), pTask->nTotalSize,
			pTask->strGameInfo.c_str(), pTask->strGameTags.c_str(), PublicLib::UToUtf8(pTask->strSavePath).c_str(), (int)pTask->gameType, pTask->strPkgName.c_str(),
			pTask->strSyncMem.c_str(), PublicLib::UToUtf8(pTask->strRomPath).c_str(), PublicLib::UToUtf8(pTask->strVer).c_str(), PublicLib::UToUtf8(pTask->strSourceVer).c_str(), PublicLib::UToUtf8(pTask->strConfigVer).c_str(), (int)pTask->type, pTask->strIconUrl.c_str(), pTask->favorite ? 1 : 0);
		int nResult = db.execDML(strSql);
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s", PublicLib::Utf8ToU(m_strDbPath).c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
		return false;
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"未知错误");
		return false;
	}
	return true;
}

bool CDatabaseMgr::UpdateGameInfo(ITask* pTask)
{
	try
	{
		CppSQLite3DB db;
		db.open(m_strDbPath.c_str());
		db.execDML(BEGIN_TRASACTION);
		CppSQLite3Statement stat = db.compileStatement(SQL_UPDATE_DB_GAMEINFO);
		int index = 0;
		stat.bind(++index, PublicLib::UToUtf8(pTask->strVersion).c_str());
		stat.bind(++index, pTask->nTotalSize);
		stat.bind(++index, pTask->strGameInfo.c_str());
		stat.bind(++index, pTask->strGameTags.c_str());
		stat.bind(++index, PublicLib::UToUtf8(pTask->strSavePath).c_str());
		stat.bind(++index, pTask->strSyncMem.c_str());
		stat.bind(++index, PublicLib::UToUtf8(pTask->strRomPath).c_str());
        stat.bind(++index, PublicLib::UToUtf8(pTask->strVer).c_str());
        stat.bind(++index, PublicLib::UToUtf8(pTask->strSourceVer).c_str());
        stat.bind(++index, PublicLib::UToUtf8(pTask->strConfigVer).c_str());
        stat.bind(++index, pTask->strIconUrl.c_str());
        stat.bind(++index, pTask->type);
        stat.bind(++index, pTask->nGameID);
		stat.execDML();
		stat.reset();
		db.execDML(COMMIT_TRANSACTION);
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s", PublicLib::Utf8ToU(m_strDbPath).c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
		return false;
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"未知错误");
		return false;
	}
	return true;
}

bool CDatabaseMgr::DeleteGameInfo(__int64 nGameID)
{
	try
	{
		{
			CScopeLock lock(m_lockGameInfo);
			bool bFind = false;
			for (auto itor = m_listInstall.begin(); itor != m_listInstall.end(); ++itor)
			{
				if ((*itor)->nGameID == nGameID)
				{
					bFind = true;
					ITask* pTask = *itor;
					//删除文件
					if (!pTask->strSavePath.empty())
					{
						::DeleteFile(pTask->strSavePath.c_str());
						if (!pTask->strPkgName.empty())
						{
							wstring strDir = pTask->strRomPath + L"\\" + PublicLib::AToU(pTask->strPkgName);
							PublicLib::RemoveDir(strDir.c_str());
						}
					}
					delete pTask;
					m_listInstall.erase(itor);
					break;
				}
			}
			if (!bFind)
			{
				OUTPUT_XYLOG(LEVEL_ERROR, L"安装游戏ID不存在");
				return false;
			}
		}
		CppSQLite3DB db;
		db.open(m_strDbPath.c_str());
		db.execDML(BEGIN_TRASACTION);
		CppSQLite3Statement stmt = db.compileStatement(SQL_DELETE_DB_GAMEINFO);
		stmt.bind(1, nGameID);
		stmt.execDML();
		stmt.reset();
		db.execDML(COMMIT_TRANSACTION);
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s", PublicLib::Utf8ToU(m_strDbPath).c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
		return false;
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"未知错误");
		return false;
	}
	return true;
}

bool CDatabaseMgr::DeleteGameInfoList(const list<__int64>& listGames, OUT list<wstring>& listDirs, OUT list<wstring>& listFiles)
{
	try
	{
		CScopeLock lock(m_lockGameInfo);
		CppSQLite3DB db;
		db.open(m_strDbPath.c_str());
		db.execDML(BEGIN_TRASACTION);
		for (auto itorList = listGames.begin(); itorList != listGames.end(); ++itorList)
		{
			for (auto itor = m_listInstall.begin(); itor != m_listInstall.end(); ++itor)
			{
				if ((*itor)->nGameID == *itorList)
				{
					ITask* pTask = *itor;
					//删除文件
					if (PathFileExists(pTask->strSavePath.c_str()))
					{
						listFiles.push_back(pTask->strSavePath);
					}
					if (!pTask->strPkgName.empty())
					{
						wstring strDir = pTask->strRomPath + L"\\" + PublicLib::AToU(pTask->strPkgName);
						if (PathFileExists(strDir.c_str()))
							listDirs.push_back(strDir);
					}
					delete pTask;
					m_listInstall.erase(itor);
					break;
				}
			}
			CppSQLite3Statement stmt = db.compileStatement(SQL_DELETE_DB_GAMEINFO);
			stmt.bind(1, *itorList);
			stmt.execDML();
			stmt.reset();
		}
		db.execDML(COMMIT_TRANSACTION);
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"open database %s failed error code:%d, error msg:%s", PublicLib::Utf8ToU(m_strDbPath).c_str(), e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
		return false;
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"未知错误");
		return false;
	}
	return true;
}

ITask* CDatabaseMgr::GetGameInfo(__int64 nGameID)
{
	CScopeLock lock(m_lockGameInfo);
	for (auto itor = m_listInstall.begin(); itor != m_listInstall.end(); ++itor) {
		if ((*itor)->nGameID == nGameID)
			return *itor;
	}

	return nullptr;
}

ITask* CDatabaseMgr::GetGameInfo(string package_name) {
    CScopeLock lock(m_lockGameInfo);
    for (auto itor = m_listInstall.begin(); itor != m_listInstall.end(); ++itor) {
        if ((*itor)->strPkgName == package_name)
            return *itor;
    }

    return nullptr;
}

void CDatabaseMgr::AddFvoriteGameInfo(__int64 nGameID, bool favorite) {
    CScopeLock lock(m_lockGameInfo);

    auto it = std::find_if(m_listInstall.begin(), m_listInstall.end(),
        [&](const ITask* node) {
        return node && node->nGameID == nGameID;
    });

    if (it == m_listInstall.end()) return;

    (*it)->favorite = favorite;

    try {
        CStringA strSql;
        strSql.Format(SQL_DB_ADD_FAVORITE, favorite == true, nGameID);
        m_sqliteDB.execDML(strSql);
    }
    catch (CppSQLite3Exception& e) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"failed error code:%d, error msg:%s", e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
    }
    catch (...) {
        OUTPUT_XYLOG(LEVEL_ERROR, L"更新数据库版本失败");
    }
}

void CDatabaseMgr::ReleaseGameInfo()
{
	CScopeLock lock(m_lockGameInfo);
	for (auto itor = m_listInstall.begin(); itor != m_listInstall.end(); ++itor)
	{
		delete *itor;
	}
	m_listInstall.clear();
}

void CDatabaseMgr::ReadSettingData()
{
	int nExit = 0, nMaxLoadSpeed = 0;
	string strLoadPath;
	try
	{
		CppSQLite3Query query = m_sqliteDB.execQuery(SQL_READ_DB_SETTING);
		nExit = query.getIntField("exit");
		if (nExit < 0)
			nExit = 0;
		nMaxLoadSpeed = query.getIntField("max_load_speed");
		if (nMaxLoadSpeed < 0)
			nMaxLoadSpeed = 0;
		else if (nMaxLoadSpeed > g_nMaxSpeed)
			nMaxLoadSpeed = g_nMaxSpeed;
		strLoadPath = query.getStringField("load_path");
		wstring strPath = PublicLib::Utf8ToU(strLoadPath);
		SHCreateDirectory(NULL, strPath.c_str());
		if (!PathFileExists(strPath.c_str()))
		{
			strLoadPath = PublicLib::UToUtf8(CGlobalData::Instance()->GetDefLoadPath());
		}
		SHCreateDirectory(NULL, strPath.c_str());
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"failed error code:%d, error msg:%s", e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"获取设置数据失败");
	}
	m_setting.nExit = nExit;
	m_setting.nMaxLoadSpeed = nMaxLoadSpeed;
	m_setting.strLoadPath = strLoadPath;
}

void CDatabaseMgr::UpdateSettingData()
{
	try
	{
		CStringA strSql;
		strSql.Format(SQL_UPDATE_DB_SETTING, m_setting.nExit, m_setting.nMaxLoadSpeed, m_setting.strLoadPath.c_str());
		m_sqliteDB.execDML(strSql);
	}
	catch (CppSQLite3Exception& e)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"failed error code:%d, error msg:%s", e.errorCode(), PublicLib::Utf8ToU(e.errorMessage()).c_str());
	}
	catch (...)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"更新设置数据失败");
	}
}

bool CDatabaseMgr::CheckFreeSpace()
{
	wstring strPath = PublicLib::Utf8ToU(m_setting.strLoadPath);
	if (!PathFileExists(strPath.c_str()))
	{
		SHCreateDirectory(NULL, strPath.c_str());
	}
	if (!strPath.empty() && PathFileExists(strPath.c_str()))
	{
		wstring strDisk = strPath.substr(0, 2);
		ULARGE_INTEGER uiAvalaible, uiTotal, uiFree;
		if (GetDiskFreeSpaceEx(strDisk.c_str(), &uiAvalaible, &uiTotal, &uiFree))
		{//小于3G
			int nGSize = (int)(uiFree.QuadPart >> 30);
			if (nGSize < 3)
				return false;
		}
	}
	return true;
}
