#pragma once



#define BEGIN_TRASACTION     "begin transaction;"
#define COMMIT_TRANSACTION   "commit transaction;"

#define TB_VERSION	"tb_version"
#define SQL_CREATE_DB_VERSION "create table tb_version(id INTEGER PRIMARY KEY AUTOINCREMENT, database_version text, software_version text);"
#define SQL_READ_DB_VERSION "select * from tb_version"
#define SQL_UPDATE_DB_VERSION "update tb_version set database_version = '%s';"


#define TB_SETTING "tb_setting"
#define SQL_CREATE_DB_SETTING "create table tb_setting(id INTEGER PRIMARY KEY AUTOINCREMENT, exit INTEGER, max_load_speed INTEGER, load_path text);"
#define SQL_READ_DB_SETTING "select * from tb_setting"
#define SQL_UPDATE_DB_SETTING "update tb_setting set exit = %d, max_load_speed = %d, load_path = '%s';"


#define TB_LOADING "tb_loading"
#define SQL_CREATE_DB_LOADING "create table tb_loading(id INTEGER PRIMARY KEY AUTOINCREMENT, url text, title text, version text, total_size INT64, load_size INT64,\
save_path text, load_state INTEGER, game_id INT64, game_info text, tag_list text, game_type int, pkg_name text, r1 text, r2 text, r3 text, file_type INTEGER, ver text, source_ver text, config_ver text, icon_url text);"
#define SQL_INSERT_DB_LOADING "insert into tb_loading(url, title, version, total_size, load_size, save_path, load_state, game_id, \
game_info, tag_list, game_type, pkg_name, r1, r2, r3, file_type, ver, source_ver, config_ver, icon_url) \
values('%s', '%s', '%s', %I64d, %I64d, '%s', %d, '%I64d', '%s', '%s', %d, '%s', '%s', '', '',%d, '%s', '%s', '%s', '%s');"
#define SQL_READ_DB_LOADING "select * from tb_loading"
#define SQL_UPDATE_DB_LOADING  "update tb_loading set total_size=?, load_size=?, load_state=? , ver=?, source_ver=?, config_ver=?, icon_url=? where game_id=?;"
#define SQL_DELETE_DB_LOADING  "delete from tb_loading where game_id = ?;"

#define TB_LOADED "tb_loaded"
#define SQL_CREATE_DB_LOADED "create table tb_loaded(id INTEGER PRIMARY KEY AUTOINCREMENT, title text, version text, total_size INT64, \
save_path text, game_id INT64, date INT64, game_type int, r1 text, r2 text, r3 text, file_type INTEGER, info text, pkg_name text);"
#define SQL_INSERT_DB_LOADED "insert into tb_loaded(title, version, total_size, save_path, game_id, date, game_type, r1, r2, r3, file_type, info, pkg_name) \
values('%s', '%s', %I64d, '%s', '%I64d', %I64d, %d, '', '', '', %d, '%s', '%s');"
#define SQL_READ_DB_LOADED "select * from tb_loaded"
#define SQL_DELETE_DB_LOADED  "delete from tb_loaded where game_id = ?;"


#define TB_GAMEINFO "tb_game_info"
#define SQL_CREATE_DB_GAMEINFO "create table tb_game_info(game_id INT64 PRIMARY KEY, name text, version text, size INT64, info text, \
tag_list text, save_path text, game_type int, pkg_name text, r1 text, r2 text, r3 text, ver text, source_ver text, config_ver text, file_type INTEGER, icon_url text, favorite INTEGER);"
#define SQL_INSERT_DB_GAMEINFO "insert into tb_game_info(game_id, name, version, size, info, tag_list, save_path, game_type, pkg_name, r1, r2, r3, ver, source_ver, config_ver, file_type, icon_url, favorite) \
values('%I64d', '%s', '%s', %I64d, '%s', '%s', '%s', %d, '%s', '%s', '%s', '', '%s', '%s', '%s', %d, '%s', %d);"
#define SQL_UPDATE_DB_GAMEINFO "update tb_game_info set version=?, size=?, info=?, tag_list=?, save_path=?, r1=?, r2=?, ver=?, source_ver=?, config_ver=?, icon_url=?, file_type=? where game_id=?;"
#define SQL_DB_ADD_FAVORITE "update tb_game_info set favorite = %d where game_id =  '%I64d'; "
#define SQL_READ_DB_GAMEINFO "select * from tb_game_info"
#define SQL_DELETE_DB_GAMEINFO  "delete from tb_game_info where game_id = ?;"


struct AppSetting
{
	int nExit;//退出时，0：隐藏到托盘，1：直接退出
	int nMaxLoadSpeed;//最大下载速度，0：不限速
	string strLoadPath;//下载保存路径
};

//重载比较操作符
inline bool operator == (const AppSetting& l, const AppSetting& r)
{
	if (l.nExit != r.nExit)
		return false;
	if (l.nMaxLoadSpeed != r.nMaxLoadSpeed)
		return false;
	if (l.strLoadPath != r.strLoadPath)
		return false;
	return true;
}

inline bool operator !=(const AppSetting& l, const AppSetting& r)
{
	return !(l == r);
}