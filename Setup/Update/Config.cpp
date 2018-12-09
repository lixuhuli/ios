#include "stdafx.h"
#include "Config.h"
#include "resource.h"
#include "GlobalData.h"


CConfig::CConfig()
	: m_pXmlDoc(new TiXmlDocument)
	, m_pRoot(NULL)
{

}

CConfig::~CConfig()
{
	ReleaseUpdateList();
}

bool CConfig::LoadFromFile(const string& strPath)
{
	if (!m_pXmlDoc->LoadFile(strPath, TIXML_ENCODING_UTF8))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"加载XML文件失败");
		return false;
	}
	m_pRoot = m_pXmlDoc->RootElement();
	if (m_pRoot == NULL)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"解析XML失败");
		return false;
	}
	ClearLists();
	return true;
}

bool CConfig::LoadFromMem(void* lpData, size_t nSize)
{
	if (!m_pXmlDoc->LoadMem(lpData, nSize, TIXML_ENCODING_UTF8))
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"加载XML文件失败");
		return false;
	}
	m_pRoot = m_pXmlDoc->RootElement();
	if (m_pRoot == NULL)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"解析XML失败");
		return false;
	}
	ClearLists();
	return true;
}

// bool CConfig::ParseUpdateParam()
// {
// 	if (NULL == m_pRoot)
// 	{
// 		return false;
// 	}
// 	TiXmlElement* pElem = m_pRoot->FirstChildElement("UpdateParam");
// 	if (NULL == pElem)
// 	{
// 		OUTPUT_XYLOG(LEVEL_ERROR, L"没有找到UpdateParam节点");
// 		return false;
// 	}
// 	TiXmlElement* pChildElem = pElem->FirstChildElement();
// 	for (; pChildElem != NULL; pChildElem = pChildElem->NextSiblingElement())
// 	{
// 		const char* pName = pChildElem->Attribute("Name");
// 		if (NULL == pName)
// 			continue;
// 		const char* pValue = pChildElem->Attribute("Value");
// 		if (NULL == pValue)
// 			continue;
// // 		if (strcmp(pName, "AllowsSkip") == 0)
// // 		{
// // 			if (strcmp(pValue, "0") == 0)
// // 				m_UpdateParam.m_bForce = true;
// // 			else
// // 				m_UpdateParam.m_bForce = false;
// // 		}
// // 		else if (strcmp(pName, "UpdateDesc") == 0)
// // 			m_UpdateParam.m_strDesc = PublicLib::Utf8ToU(pValue);
// // 		else if (strcmp(pName, "ProductName") == 0)
// // 			m_UpdateParam.m_strName = PublicLib::Utf8ToU(pValue);
// // 		else if (strcmp(pName, "VersionInfo") == 0)
// // 			m_UpdateParam.m_strVersion = PublicLib::Utf8ToU(pValue);
// // 		else if (strcmp(pName, "Date") == 0)
// // 			m_UpdateParam.m_strDate = PublicLib::Utf8ToU(pValue);
// // 		else if (strcmp(pName, "Win64") == 0)
// // 		{
// // 			if (strcmp(pValue, "1") == 0)
// // 				m_UpdateParam.m_bWin64 = true;
// // 			else
// // 				m_UpdateParam.m_bWin64 = false;
// // 		}
// 	}
// 	return true;
// }

bool CConfig::ParseUpdateSelf()
{
	if (NULL == m_pRoot)
	{
		return false;
	}
	TiXmlElement* pElem = m_pRoot->FirstChildElement("UpdateModule");
	if (NULL == pElem)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"没有找到UpdateModule节点");
		return false;
	}
	TiXmlElement* pChildElem = pElem->FirstChildElement("UpdateSelf");
	if (NULL == pChildElem)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"没有找到UpdateSelf节点");
		return false;
	}
	const char* pValue = pChildElem->Attribute("Dir");
	if (pValue)
		m_UpdateSelf.m_strDirName = PublicLib::Utf8ToU(pValue);
	pValue = pChildElem->Attribute("Version");
	if (pValue)
		m_UpdateSelf.m_strVersion = PublicLib::Utf8ToU(pValue);
	pValue = pChildElem->Attribute("Name");
	if (pValue)
		m_UpdateSelf.m_strFileName = PublicLib::Utf8ToU(pValue);
	pValue = pChildElem->Attribute("MD5");
	if (pValue)
		m_UpdateSelf.m_strMd5 = pValue;
	return true;
}

bool CConfig::ParseUpdatePackage()
{
	if (NULL == m_pRoot)
	{
		return false;
	}
	TiXmlElement* pElem = m_pRoot->FirstChildElement("UpdateModule");
	if (NULL == pElem)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"没有找到UpdateModule节点");
		return false;
	}
	TiXmlElement* pChildElem = pElem->FirstChildElement("UpdatePackage");
	if (NULL == pChildElem)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"没有找到UpdatePackage节点");
		return false;
	}
	const char* pValue = pChildElem->Attribute("Dir");
	if (pValue)
		m_UpdatePackage.m_strDirName = PublicLib::Utf8ToU(pValue);
	pValue = pChildElem->Attribute("Version");
	if (pValue)
		m_UpdatePackage.m_strVersion = PublicLib::Utf8ToU(pValue);
	pValue = pChildElem->Attribute("Name");
	if (pValue)
		m_UpdatePackage.m_strFileName = PublicLib::Utf8ToU(pValue);
	pValue = pChildElem->Attribute("MD5");
	if (pValue)
		m_UpdatePackage.m_strMd5 = pValue;
	return true;
}

bool CConfig::ParseFileList(const BOOL* pExit)
{
	if (NULL == m_pRoot)
	{
		return false;
	}
	TiXmlElement* pElem = m_pRoot->FirstChildElement("UpdateModule");
	if (NULL == pElem)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"没有找到UpdateModule节点");
		return false;
	}
	TiXmlElement* pChildElem = pElem->FirstChildElement("FileList");
	if (NULL == pChildElem)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"没有找到FileList节点");
		return false;
	}
	const char* pDir = pChildElem->Attribute("Dir");
	if (pDir)
	{
		m_strZipListDir = PublicLib::Utf8ToU(pDir);
	}
	wstring strPath = CGlobalData::Instance()->GetOldInstallPath();
	Fi_map localFiles;
	EnumFolderFilesMd5(strPath, localFiles, L"", pExit);
	wstring strShortPath, strName, strDir;
	string strDesMd5;
	LPUPDATE_FILE_ITEM lpItem = NULL;
	pElem = pChildElem->FirstChildElement();
	for (; pElem != NULL && !(*pExit); pElem = pElem->NextSiblingElement())
	{
		strDesMd5 = pElem->Attribute("MD5");
		strName = PublicLib::Utf8ToU(pElem->Attribute("Name"));
		strDir = PublicLib::Utf8ToU(pElem->Attribute("Dir"));
		if (strDir.empty())
			strShortPath = strName;
		else
			strShortPath = strDir + strName;
		Fi_itor itor = localFiles.find(strShortPath);
		bool bFind = (itor != localFiles.end());
		if (!bFind || (_stricmp(itor->second.c_str(), strDesMd5.c_str()) != 0))
		{
			lpItem = new UPDATE_FILE_ITEM;
			lpItem->strZipName = PublicLib::Utf8ToU(pElem->GetText());
			lpItem->strShortPath = strShortPath;
			lpItem->strMD5 = strDesMd5;
			lpItem->strName = strName;
			lpItem->strDir = strDir;
			lpItem->bReplace = bFind;
			m_UpdateList.push_back(lpItem);
		}
	}
	return true;
}

bool CConfig::ParseConflictList()
{
	if (NULL == m_pRoot)
	{
		return false;
	}
	TiXmlElement* pElem = m_pRoot->FirstChildElement("ConflictList");
	if (NULL == pElem)
	{
		OUTPUT_XYLOG(LEVEL_ERROR, L"没有找到ConflictList节点");
		return false;
	}
	ParseList(pElem, m_ConflictList);
	return true;
}

void CConfig::ReleaseUpdateList()
{
	list<LPUPDATE_FILE_ITEM>::iterator itor = m_UpdateList.begin();
	for ( ; itor != m_UpdateList.end(); ++itor )
	{
		LPUPDATE_FILE_ITEM lpItem = *itor;
		delete lpItem;
	}
	m_UpdateList.clear();
}

void CConfig::ClearLists()
{
	m_ConflictList.clear();
}

bool ParseList(TiXmlElement* pElem, vector<wstring>& cmdList)
{
	cmdList.reserve(10);
	TiXmlElement* pChild = pElem->FirstChildElement();
	for (; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		const char* pText = pChild->GetText();
		if (NULL == pText)
			continue;
		cmdList.push_back(PublicLib::Utf8ToU(pText));
	}
	return true;
}

void KillConflictList(const StrList& conflictList)
{
	for ( int i=0; i<5; ++i )
	{
		BOOL bRet = PublicLib::KillProcessFromList(conflictList);
		if ( !bRet )
			break;
		Sleep(500);
	}
}

bool EnumFolderFilesMd5(wstring& strDir, OUT Fi_map& outMap, wstring strFolderName, const BOOL* pExit)
{
	if (*pExit) return false;
	WIN32_FIND_DATA fd;
	if (strDir[strDir.size() - 1] != '\\')
		strDir.append(L"\\");
	HANDLE hFind = FindFirstFile(wstring(strDir + L"*").c_str(), &fd);
	if (INVALID_HANDLE_VALUE == hFind)
		return false;
	wchar_t szPath[MAX_PATH];
	while (FindNextFile(hFind, &fd))
	{
		if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0 ||
			wcscmp(fd.cFileName, L"assets") == 0 || wcscmp(fd.cFileName, L"memstick") == 0)
			continue;
		swprintf_s(szPath, L"%s%s", strDir.c_str(), fd.cFileName);
		if (fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			wstring strChildDir(szPath);
			wstring strNewFolderName = strFolderName;
			strNewFolderName.append(fd.cFileName);
			strNewFolderName.append(L"\\");
			EnumFolderFilesMd5(strChildDir, outMap, strNewFolderName, pExit);
			continue;
		}
		if (*pExit) break;
		wstring strShortPath = strFolderName + fd.cFileName;
		outMap.insert(std::make_pair(strShortPath, PublicLib::CalFileMd5(szPath)));
		if (*pExit) break;
	}
	FindClose(hFind);
	return true;
}