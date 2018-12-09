#pragma once
/*****************************************
*���������ļ�������
*2015��7��10��
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
// 	wstring		m_strDesc;			//��������
// 	wstring		m_strName;			//��Ʒ����
// 	wstring		m_strVersion;		//�µİ汾��
// 	wstring		m_strDate;			//����ʱ��
// 	bool		m_bForce;			//�Ƿ�ǿ������
// 	bool		m_bWin64;
// 	UPDATE_PARAM()
// 		: m_bForce(false)
// 		, m_bWin64(false)
// 	{
// 
// 	}
// };

//�����ļ��б�ڵ���Ϣ
typedef struct _UPDATE_FILE_ITEM
{
	wstring strZipName;		//ѹ���ļ���
	wstring strShortPath;	//�ļ��ڰ�װ·���µ����·��
	wstring strName;		//��ѹ����ļ���
	string	strMD5;			//��ѹ����ļ�MD5
	wstring strDir;			//�ڰ�װ·���µ����·��
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
	void	ReleaseUpdateList();//�ͷŸ����б�

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
