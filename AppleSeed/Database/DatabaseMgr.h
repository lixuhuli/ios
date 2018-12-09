/*******************************
*��װsqlite���ݿ������
*���룺utf-8
*/
#pragma once
#include "sqlite3/CppSQLite3.h"
#include "DatabaseDefine.h"
#include "Download/Task.h"
#include <list>
using std::list;


class CanPlayMgr;
class CDatabaseMgr
{
public:
	static CDatabaseMgr* Instance(){
		static CDatabaseMgr obj;
		return &obj;
	}

	bool Init(); // ��ʼ��
	void Exit(); // �˳�
    
	const list<ITask*>& GetInstallList()const { return m_listInstall; }
	bool Open(const string& strDbFile);
	void GetSetting(OUT AppSetting& setting);
	void GetDefSetting(OUT AppSetting& setting);
	void SetSetting(const AppSetting& setting);

	// ��ȡ���ر���·������ת�룩
	wstring GetLoadPath();

	// ���ر�
	bool GetLoadTasks(OUT vector<ITask*> &loadList);
	bool InsertLoadTask(ITask* pTask);
	bool UpdateLoadTask(ITask* pTask);
	bool DeleteLoadTask(ITask* pTask);
	bool DeleteLoadTaskList(const std::vector<__int64>& gameIDList);

	// ��ɱ�
	bool GetFinishTasks(OUT vector<ITask*> &finishList);
	bool InsertFinishTask(ITask* pTask);
	bool DeleteFinishTask(ITask* pTask);

	// ��װ�б�
	bool ReadGameInfos();
	bool InsertGameInfo(ITask* pTask);
	bool DeleteGameInfo(__int64 nGameID);
	bool DeleteGameInfoList(const list<__int64>& listGames, OUT list<wstring>& listDirs, OUT list<wstring>& listFiles);
	ITask* GetGameInfo(__int64 nGameID);
    ITask* GetGameInfo(string package_name);
    void AddFvoriteGameInfo(__int64 nGameID, bool favorite);
	void ReleaseGameInfo();

    // ��ȡ���ݿ�汾��
    const string& GetDbVersion() { return startup_db_version_; };

	bool CheckFreeSpace();

protected:
	CDatabaseMgr();
	~CDatabaseMgr();
	void ReadSettingData();
	void UpdateSettingData();
	int AddTableField(const char* table_name, const char* field_name, const char* field_type);
	bool UpdateGameInfo(ITask* pTask);

private:
	string m_strDbPath;
	AppSetting	m_setting;
	CppSQLite3DB m_sqliteDB;
	CLock m_lockGameInfo;
	list<ITask*> m_listInstall;
    string startup_db_version_;
};

