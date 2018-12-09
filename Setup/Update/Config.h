#pragma once
/*****************************************
*新增配置文件解析类
*2015年7月10日
*/
#include <vector>
#include <list>
using std::vector;
using std::list;



typedef vector<wstring>	StrList;

void KillConflictList(const StrList& conflictList);
bool ParseList(TiXmlElement* pElem, vector<wstring>& cmdList);



struct UPDATE_ITEM
{
	wstring m_strDirName;		
	string	m_strMd5;
	wstring	m_strFileName;
	wstring m_strVersion;
};

// struct UPDATE_PARAM
// {
// 	wstring		m_strDesc;			//更新内容
// 	wstring		m_strName;			//产品名称
// 	wstring		m_strVersion;		//新的版本号
// 	wstring		m_strDate;			//更新时间
// 	bool		m_bForce;			//是否强制升级
// 	bool		m_bWin64;
// 	UPDATE_PARAM()
// 		: m_bForce(false)
// 		, m_bWin64(false)
// 	{
// 
// 	}
// };

//下载文件列表节点信息
typedef struct _UPDATE_FILE_ITEM
{
	wstring strZipName;		//压缩文件名
	wstring strShortPath;	//文件在安装路径下的相对路径
	wstring strName;		//解压后的文件名
	string	strMD5;			//解压后的文件MD5
	wstring strDir;			//在安装路径下的相对路径
	bool	bReplace;

}UPDATE_FILE_ITEM, *LPUPDATE_FILE_ITEM;


class CConfig
{
public:
	static CConfig*	Instance()
	{
		static CConfig config;
		return &config;
	}
	StrList&	GetConflictList()		{ return m_ConflictList; }
	//void		SetWin64(bool bIsWin64) { m_UpdateParam.m_bWin64 = bIsWin64; }
	//UPDATE_PARAM&	GetUpdateParam()	{ return m_UpdateParam; }
	UPDATE_ITEM&	GetUpdateSelf()		{ return m_UpdateSelf; }
	UPDATE_ITEM&	GetUpdatePackage()	{ return m_UpdatePackage; }
	const wstring&	GetZipListDir()const{ return m_strZipListDir; }
	//bool			IsWin64Program()const{ return m_UpdateParam.m_bWin64; }
	list<LPUPDATE_FILE_ITEM>&	GetUpdateItemList()	{ return m_UpdateList; }
	void	KillConflict()				{ KillConflictList(m_ConflictList); }

	bool	LoadFromFile(const string& strPath);
	bool	LoadFromMem(void* lpData, size_t nSize);
	bool	ParseUpdateSelf();
	bool	ParseUpdatePackage();
	bool	ParseFileList(const BOOL* pExit);
	bool	ParseConflictList();
	void	ClearLists();

protected:
	CConfig();
	~CConfig();
	void	ReleaseUpdateList();//释放更新列表

private:
	TiXmlDocument*	m_pXmlDoc;
	TiXmlElement*	m_pRoot;
	//UPDATE_PARAM	m_UpdateParam;
	UPDATE_ITEM		m_UpdateSelf;
	UPDATE_ITEM		m_UpdatePackage;
	wstring			m_strZipListDir;
	list<LPUPDATE_FILE_ITEM>	m_UpdateList;
	StrList		m_ConflictList;
};


typedef map<wstring, string>	Fi_map;
typedef map<wstring, string>::iterator	Fi_itor;

bool EnumFolderFilesMd5(wstring& strDir, OUT Fi_map& outMap, wstring strFolderName, const BOOL* pExit);
