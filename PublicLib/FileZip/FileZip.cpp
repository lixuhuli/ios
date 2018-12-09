#include "StdAfx.h"
#include "FileZip.h"
#include "libzip/zip.h"
#include "litezip/LiteZip.h"
#include "litezip/LiteUnzip.h"

#ifdef _WIN64
#ifdef DEBUG
#ifndef _DLL
#pragma comment(lib, "libzip-mt.lib")
#pragma comment(lib, "libzlib-mt.lib")
#else
#pragma comment(lib, "libzip.lib")
#pragma comment(lib, "libzlib.lib")
#endif
#else
#ifndef _DLL
#pragma comment(lib, "libzip-mt.lib")
#pragma comment(lib, "libzlib-mt.lib")
#else
#pragma comment(lib, "libzip.lib")
#pragma comment(lib, "libzlib.lib")
#endif
#endif
#else
#ifdef DEBUG
#ifndef _DLL
#pragma comment(lib, "libzip-mt.lib")
#pragma comment(lib, "libzlib-mt.lib")
#else
#pragma comment(lib, "libzip.lib")
#pragma comment(lib, "libzlib.lib")
#pragma comment(lib, "LiteZip.lib")
#endif
#else
#ifndef _DLL
#pragma comment(lib, "libzip-mt.lib")
#pragma comment(lib, "libzlib-mt.lib")
#else
#pragma comment(lib, "libzip.lib")
#pragma comment(lib, "libzlib.lib")
#pragma comment(lib, "LiteZip.lib")
#endif
#endif
#endif

namespace PublicLib{

	///////////////////////////////////////////////////////////////////////////////////////////////////
	CFileZip_LibZip::CFileZip_LibZip()
	{
		m_pZipFile = NULL;
	}

	CFileZip_LibZip::~CFileZip_LibZip(void)
	{
		if (NULL != m_pZipFile)
		{
			Close();
			m_pZipFile = NULL;
		}
	}

	bool CFileZip_LibZip::Create(LPCWSTR lpszZipFilePath)
	{
		int nLen = wcslen(lpszZipFilePath);
		//zip路径有问题
		if (NULL == lpszZipFilePath || nLen <= 0)
		{
			return false;
		}

		if (PathFileExists(lpszZipFilePath) == TRUE)
		{
			return false;
		}

		//尝试关闭未关闭的zip文件
		Close();
		//打开zip文件
		int nZerr = 0;
		m_strFilePath = lpszZipFilePath;
		m_pZipFile = zip_open(m_strFilePath, ZIP_CREATE, &nZerr);
		if (NULL == m_pZipFile)
		{
			return false;
		}

		return true;
	}

	bool CFileZip_LibZip::Open(LPCWSTR lpszZipFilePath)
	{
		int nLen = wcslen(lpszZipFilePath);
		//zip路径有问题
		if (NULL == lpszZipFilePath || nLen <= 0)
		{
			return false;
		}

		if (PathFileExists(lpszZipFilePath) == false)
		{
			return false;
		}

		// 	FILE *fp = _wfopen(lpszZipFilePath, L"rb");
		// 	if(fp == NULL)
		// 	{
		// 		return false;
		// 	}
		// 	//int i = fseek(fp, 65600, SEEK_END);
		// 	clearerr(fp);
		// 	fclose(fp);

		//尝试关闭未关闭的zip文件
		Close();
		//打开zip文件
		int nZerr = 0;
		m_strFilePath = lpszZipFilePath;//UnicodeToAnsi(lpszZipFilePath);
		m_pZipFile = zip_open(m_strFilePath, ZIP_CREATE, &nZerr);
		if (NULL == m_pZipFile)
		{
			return false;
		}

		return true;
	}

	bool CFileZip_LibZip::Save()
	{
		if (NULL != m_pZipFile)
		{
			zip_close((zip*)m_pZipFile);
			int nZerr = 0;
			m_pZipFile = zip_open(m_strFilePath, ZIP_CREATE, &nZerr);
			return m_pZipFile != NULL;
		}

		return false;
	}

	void CFileZip_LibZip::Close()
	{
		if (NULL != m_pZipFile)
		{
			zip_close((zip*)m_pZipFile);
			m_pZipFile = NULL;
			m_strFilePath.Empty();
		}
	}

	bool CFileZip_LibZip::UnZip(LPCWSTR lpszSaveDir, ZipCallback pCallback)
	{
		if (NULL == m_pZipFile || NULL == lpszSaveDir || wcslen(lpszSaveDir) <= 0)
		{
			return false;
		}
		CString strFilePath = lpszSaveDir;

		ULONGLONG ullFileCount = GetZipFileCount();
		for (ULONGLONG i = 0; i < ullFileCount; i++)
		{
			CString lpszFileName = GetFileNameByIndex(i);
			if (lpszFileName.IsEmpty())
			{
				return false;
			}
			CString strNewFilePath = strFilePath + "\\" + lpszFileName;
			if (!UnZipOneFile(i, strNewFilePath.GetString()))
			{
				return false;
			}
			if (pCallback)
			{
				pCallback((int)i, (int)ullFileCount, NULL);
			}
		}

		return true;
	}

	bool CFileZip_LibZip::UnZipOneFolder(LPCWSTR lpszFolderName, LPCWSTR lpszSaveDir)
	{
		if (NULL == m_pZipFile || NULL == lpszSaveDir || wcslen(lpszSaveDir) <= 0 || NULL == lpszFolderName || wcslen(lpszFolderName) <= 0)
		{
			return false;
		}
		ULONGLONG ullFileCount = GetZipFileCount();
		CString strFilePath = lpszSaveDir;
		CString strName;
		int nLen = wcslen(lpszFolderName);
		for (ULONGLONG i = 0; i < ullFileCount; i++)
		{
			CString lpszFileName = GetFileNameByIndex(i);
			if (lpszFileName.IsEmpty()) return false;

			strName = lpszFileName;
			CString strNewFilePath = strFilePath + "\\" + lpszFileName;
			if (strName.Left(nLen) == lpszFolderName)
			{
				if (!UnZipOneFile(i, strNewFilePath.GetString()))
				{
					return false;
				}
			}
		}
		return true;
	}

	bool CFileZip_LibZip::UnZipOneFile(LPCWSTR lpszFileName, LPCWSTR lpszSaveFilePath)
	{
		if (NULL == m_pZipFile || NULL == lpszSaveFilePath || wcslen(lpszSaveFilePath) <= 0)
		{
			return false;
		}
		ULONGLONG ullFileCount = GetZipFileCount();
		CString strName(lpszFileName);
		strName.Replace('\\', '/');
		for (ULONGLONG i = 0; i < ullFileCount; i++)
		{
			CString strZipFileName = GetFileNameByIndex(i);
			if (strZipFileName.IsEmpty() == FALSE && strName.Compare(strZipFileName) == 0)
			{
				if (!UnZipOneFile(i, lpszSaveFilePath))
				{
					return false;
				}
			}
		}

		return true;
	}

	bool CFileZip_LibZip::UnZipOneFileToMem(LPCWSTR  lpszFileName, BYTE * pBuffer, DWORD dwBufferLength, DWORD * pdwRetLength)
	{
		DWORD dwLength = 0;
		if (NULL == m_pZipFile || NULL == lpszFileName)
		{
			return false;
		}

		ULONGLONG ullFileCount = GetZipFileCount();
		CString strName(lpszFileName);
		strName.Replace('\\', '/');

		ULONGLONG index = GetFileIndexByName(lpszFileName);
		if (pBuffer != NULL)
		{
			if (dwLength > dwBufferLength) return false;
			dwLength = UnZipOneFile(index, pBuffer, dwBufferLength);
			*pdwRetLength = dwLength;
		}
		else
		{
			dwLength = UnZipOneFile(index, (BYTE*)NULL, 0);
			*pdwRetLength = dwLength;
		}
		return dwLength == 0 ? false : true;
	}

	bool CFileZip_LibZip::AppendFolder(LPCWSTR lpszName)
	{
		if (NULL == m_pZipFile || NULL == lpszName || wcslen(lpszName) <= 0)
		{
			return false;
		}
		CStringA utf8Name = UnicodeToUtf8(lpszName);
		utf8Name.Replace('\\', '/');

		return zip_dir_add((zip*)m_pZipFile, utf8Name.GetString(), ZIP_FL_ENC_UTF_8) < 0 ? false : true;
	}

	bool CFileZip_LibZip::AppendFile(LPCWSTR lpszFilePath, LPCWSTR lpszName)
	{
		if (NULL == m_pZipFile || NULL == lpszFilePath || wcslen(lpszFilePath) <= 0 || NULL == lpszName || wcslen(lpszName) <= 0)
		{
			return false;
		}

		if (PathFileExists(lpszFilePath) == FALSE) return false;
		CStringA strZipFileName = UnicodeToUtf8(lpszName);
		strZipFileName.Replace('\\', '/');
		zip_source* pFileSource = zip_source_file((zip*)m_pZipFile, lpszFilePath, 0, -1);
		if (NULL == pFileSource)
		{
			return false;
		}

		return zip_file_add((zip*)m_pZipFile, strZipFileName.GetString(), pFileSource, ZIP_FL_OVERWRITE | ZIP_FL_ENC_GUESS) < 0 ? false : true;
	}

	bool CFileZip_LibZip::AppendBuffer(char* buffer, unsigned int buffer_length, LPCWSTR lpszName)
	{
		if (NULL == buffer || 0 == buffer_length)
		{
			return false;
		}

		CStringA strZipFileName = UnicodeToUtf8(lpszName);
		strZipFileName.Replace('\\', '/');
		zip_source* pFileSource = zip_source_buffer((zip*)m_pZipFile, buffer, buffer_length, 0);
		if (NULL == pFileSource)
		{
			return false;
		}

		zip_int64_t zip_length = zip_file_add((zip*)m_pZipFile, strZipFileName.GetString(), pFileSource, ZIP_FL_OVERWRITE | ZIP_FL_ENC_CP437);
		//zip_source_free(pFileSource);
		return zip_length < 0 ? false : true;
	}

	bool CFileZip_LibZip::AppendDirectory(LPCWSTR lpszDirName, LPCWSTR lpszName)
	{
		if (NULL == lpszDirName)
		{
			return false;
		}

		std::map<CString, CString> mapFiles;

		CString DirName = lpszDirName;
		DirName.Replace(L"/", L"\\");

		EnumerateDirectoryFileToMap(DirName, DirName, mapFiles);

		std::map<CString, CString>::iterator itor;
		for (itor = mapFiles.begin(); itor != mapFiles.end(); itor++)
		{
			CString strDestDir;
			if (lpszName)
			{
				strDestDir = lpszName;
				strDestDir += L"\\";
			}
			strDestDir += itor->first;
			CString strFilePath = itor->second;
			if (AppendFile(strFilePath, strDestDir) == false)
			{
				return false;
			}
		}

		return true;
	}

	bool CFileZip_LibZip::DeleteFile(LPCWSTR lpszName)
	{
		if (NULL == m_pZipFile || NULL == lpszName || wcslen(lpszName) <= 0)
		{
			return false;
		}

		ULONGLONG ullFileCount = GetZipFileCount();
		CString strName(lpszName);
		strName.Replace('\\', '/');
		for (ULONGLONG i = 0; i < ullFileCount; i++)
		{
			CString strZipFileName = GetFileNameByIndex(i);
			CString strZipFileNameTmp = strZipFileName.Left(strName.GetLength());
			if (strZipFileNameTmp.Compare(strName) == 0)
			{
				if (ZipDeleteFile(i) == false) return false;
			}
		}

		return true;
	}

	bool CFileZip_LibZip::GetZipFileList(std::map<CString, STZipInfo>& mapFileList)
	{
		if (NULL == m_pZipFile)
		{
			return false;
		}
		ULONGLONG ullFileCount = GetZipFileCount();
		for (ULONGLONG i = 0; i < ullFileCount; i++)
		{
			CString lpszFileName = GetFileNameByIndex(i);
			if (lpszFileName.IsEmpty())
			{
				return false;
			}
			AddFileToFileListMap(i, lpszFileName, mapFileList);
		}

		return true;
	}

	bool CFileZip_LibZip::GetZipFileList(std::vector<CString>& file_list)
	{
		if (NULL == m_pZipFile)
		{
			return false;
		}
		ULONGLONG ullFileCount = GetZipFileCount();
		for (ULONGLONG i = 0; i < ullFileCount; i++)
		{
			CString lpszFileName = GetFileNameByIndex(i);
			CString temp = lpszFileName.Right(1);
			if (lpszFileName.IsEmpty() || 0 == temp.Compare(L"/"))
			{
				continue;
			}
			file_list.push_back(lpszFileName);
		}

		return true;
	}

	bool CFileZip_LibZip::GetZipFileList(std::vector<std::wstring>& file_list)
	{
		if (NULL == m_pZipFile)
		{
			return false;
		}
		ULONGLONG ullFileCount = GetZipFileCount();
		for (ULONGLONG i = 0; i < ullFileCount; i++)
		{
			CString lpszFileName = GetFileNameByIndex(i);
			CString temp = lpszFileName.Right(1);
			if (lpszFileName.IsEmpty() || 0 == temp.Compare(L"/"))
			{
				continue;
			}
			file_list.push_back(lpszFileName.GetString());
		}

		return true;
	}

	ULONGLONG CFileZip_LibZip::GetZipFileCount()
	{
		if (NULL == m_pZipFile)
		{
			return 0;
		}

		return zip_get_num_entries((zip*)m_pZipFile, 0);
		//return zip_get_num_files(m_pZipFile);
	}

	__int64 CFileZip_LibZip::GetFileIndexByName(LPCWSTR lpszName)
	{
		if (NULL == m_pZipFile || NULL == lpszName || wcslen(lpszName) <= 0)
		{
			return -1;
		}

		CStringW wstrName = lpszName;
		wstrName.Replace(L"\\", L"/");
		CStringA strName = UnicodeToAnsi(wstrName);

		return zip_name_locate((zip*)m_pZipFile, strName.GetString(), ZIP_FL_ENC_GUESS);
	}

	CString CFileZip_LibZip::GetFileNameByIndex(ULONGLONG ullIndex)
	{
		if (NULL == m_pZipFile)
		{
			return CString();
		}

		const char *pName = zip_get_name((zip*)m_pZipFile, ullIndex, ZIP_FL_ENC_GUESS);
		CStringW wstrName = Utf8ToUnicode(pName);
		wstrName.TrimRight(' ');
		wstrName.TrimLeft(' ');
		return wstrName;
	}

	bool CFileZip_LibZip::UnZipOneFile(ULONGLONG ullIndex, LPCWSTR lpszSaveFilePath)
	{
		if (NULL == m_pZipFile || NULL == lpszSaveFilePath || wcslen(lpszSaveFilePath) <= 0)
		{
			return false;
		}
		zip_file* pFile = zip_fopen_index((zip*)m_pZipFile, ullIndex, 0);
		if (NULL == pFile)
		{
			return false;
		}

		CString strTemp = lpszSaveFilePath;
		strTemp.Replace('/', '\\');
		int nPos = strTemp.ReverseFind('\\');
		if (-1 != nPos)
		{
			strTemp = strTemp.Left(nPos + 1);
		}
		if (!strTemp.IsEmpty())
		{
			CreateMultipleDirectory(strTemp);
		}

		HANDLE hFile = CreateFile(lpszSaveFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
		if (NULL == hFile)
		{
			zip_fclose(pFile);
			return false;
		}

		zip_int64_t nReadBytes = 0;
		DWORD dwWrite = 0;
		char szBuf[8192];
		while ((nReadBytes = zip_fread(pFile, szBuf, sizeof(szBuf))) > 0)
		{
			WriteFile(hFile, szBuf, (DWORD)nReadBytes, &dwWrite, NULL);
		}
		CloseHandle(hFile);
		zip_fclose(pFile);

		return true;
	}


	DWORD CFileZip_LibZip::UnZipOneFile(ULONGLONG ullIndex, BYTE * pData, DWORD dwDataLen)
	{
		if (NULL == m_pZipFile)
		{
			return 0;
		}
		zip_file* pFile = zip_fopen_index((zip*)m_pZipFile, ullIndex, 0);
		if (NULL == pFile)
		{
			return 0;
		}



		zip_int64_t nReadBytes = 0;
		DWORD dwWrite = 0;
		char szBuf[8192];
		while ((nReadBytes = zip_fread(pFile, szBuf, sizeof(szBuf))) > 0)
		{
			if (pData != NULL)
			{
				if (dwDataLen >= dwWrite + nReadBytes)
				{
					memcpy(pData + dwWrite, szBuf, size_t(nReadBytes));
				}
				//内存长度不足以存储文件
				else
				{
					dwWrite = 0;
					break;
				}
			}
			dwWrite += DWORD(nReadBytes);
		}
		zip_fclose(pFile);

		return dwWrite;
	}
	bool CFileZip_LibZip::ZipDeleteFile(ULONGLONG ullIndex)
	{
		if (NULL == m_pZipFile)
		{
			return false;
		}

		return zip_delete((zip*)m_pZipFile, ullIndex) == 0 ? true : false;
	}

	CStringW CFileZip_LibZip::AnsyToUnicode(const CStringA &str)
	{
		int u16Len = ::MultiByteToWideChar(CP_ACP, NULL, str, (int)str.GetLength(), NULL, 0);
		wchar_t* wstrBuf = new wchar_t[u16Len + 1];
		::MultiByteToWideChar(CP_ACP, NULL, str, (int)str.GetLength(), wstrBuf, u16Len);
		wstrBuf[u16Len] = L'\0';
		CStringW wStr(wstrBuf, u16Len);
		delete[] wstrBuf;

		return wStr;
	}

	CStringW CFileZip_LibZip::Utf8ToUnicode(const CStringA &str)
	{
		int u16Len = ::MultiByteToWideChar(CP_UTF8, NULL, str, (int)str.GetLength(), NULL, 0);
		wchar_t* wstrBuf = new wchar_t[u16Len + 1];
		::MultiByteToWideChar(CP_UTF8, NULL, str, (int)str.GetLength(), wstrBuf, u16Len);
		wstrBuf[u16Len] = L'\0';
		CStringW wStr(wstrBuf, u16Len);
		delete[] wstrBuf;

		return wStr;
	}

	CStringA CFileZip_LibZip::UnicodeToAnsi(const CStringW& wstr)
	{
		int u8Len = ::WideCharToMultiByte(CP_ACP, NULL, wstr, (int)wstr.GetLength(), NULL, 0, NULL, NULL);
		char* strBuf = new char[u8Len + 1];
		::WideCharToMultiByte(CP_ACP, NULL, wstr, (int)wstr.GetLength(), strBuf, u8Len, NULL, NULL);
		strBuf[u8Len] = '\0';
		CStringA str(strBuf, u8Len);
		delete[] strBuf;

		return str;
	}

	CStringA CFileZip_LibZip::UnicodeToUtf8(const CStringW& wstr)
	{
		int u8Len = ::WideCharToMultiByte(CP_UTF8, NULL, wstr, (int)wstr.GetLength(), NULL, 0, NULL, NULL);
		char* strBuf = new char[u8Len + 1];
		::WideCharToMultiByte(CP_UTF8, NULL, wstr, (int)wstr.GetLength(), strBuf, u8Len, NULL, NULL);
		strBuf[u8Len] = '\0';
		CStringA str(strBuf, u8Len);
		delete[] strBuf;

		return str;
	}



	bool CFileZip_LibZip::CreateMultipleDirectory(const CString & szPath)
	{
		CString strDir(szPath);//存放要创建的目录字符串
		//确保以'\'结尾以创建最后一个目录
		if (strDir.GetAt(strDir.GetLength() - 1) != _T('\\'))
		{
			strDir.AppendChar(_T('\\'));
		}
		std::vector<CString> vPath;//存放每一层目录字符串
		CString strTemp;//一个临时变量,存放目录字符串
		bool bSuccess = false;//成功标志
		//遍历要创建的字符串
		for (int i = 0; i < strDir.GetLength(); ++i)
		{
			if (strDir.GetAt(i) != _T('\\'))
			{//如果当前字符不是'\\'
				strTemp.AppendChar(strDir.GetAt(i));
			}
			else
			{//如果当前字符是'\\'
				vPath.push_back(strTemp);//将当前层的字符串添加到数组中
				strTemp.AppendChar(_T('\\'));
			}
		}

		//遍历存放目录的数组,创建每层目录
		std::vector<CString>::const_iterator vIter;
		for (vIter = vPath.begin(); vIter != vPath.end(); vIter++)
		{
			//如果CreateDirectory执行成功,返回true,否则返回false
			bSuccess = CreateDirectory(*vIter, NULL) ? true : false;
		}

		return bSuccess;
	}

	BOOL CFileZip_LibZip::GetSplitString(CString SourceString, CString SplitFlag, std::vector<CString> &DestStringArray)
	{

		int len = SourceString.GetLength();
		size_t last = 0;
		size_t index = SourceString.Find(SplitFlag, last);
		while (index != -1)
		{
			CString strMidString(SourceString.Mid(last, index - last));

			DestStringArray.push_back(strMidString);
			last = index + SplitFlag.GetLength();
			index = SourceString.Find(SplitFlag, last);
		}
		if (last != len)
		{
			CString strMidString(SourceString.Mid(last, len - last));
			DestStringArray.push_back(strMidString);
		}

		return TRUE;
	}

	bool CFileZip_LibZip::AddFileToFileListMap(ULONGLONG ullIndex, const CString& strFileName, std::map<CString, STZipInfo>& mapFileList)
	{
		CString strName = strFileName;
		strName.Replace('\\', '/');
		strName.TrimRight('/');
		strName.TrimRight(' ');
		strName.TrimLeft('/');
		strName.TrimLeft(' ');

		std::vector<CString>  vecPath;
		GetSplitString(strName, ("/"), vecPath);
		int nSize = vecPath.size();
		std::map<CString, STZipInfo>* pMapFileList = &mapFileList;
		for (int i = 0; i < nSize; i++)
		{
			std::map<CString, STZipInfo>::iterator Iter = pMapFileList->find(vecPath[i]);
			if (Iter != pMapFileList->end())
			{
				if (i == nSize - 1)
				{
					Iter->second.m_strName = strFileName;
					Iter->second.m_ullIndex = ullIndex;
				}
				pMapFileList = &(Iter->second.m_mapSubFileInfo);
			}
			else
			{
				STZipInfo ZipInfo;
				if (i == nSize - 1)
				{
					ZipInfo.m_strName = strFileName;
					ZipInfo.m_ullIndex = ullIndex;
				}
				(*pMapFileList)[vecPath[i]] = ZipInfo;
				pMapFileList = &((*pMapFileList)[vecPath[i]].m_mapSubFileInfo);
			}
		}

		return true;
	}

	bool CFileZip_LibZip::EnumerateDirectoryFileToMap(CString strDirectory, CString strDirectoryBase, std::map<CString, CString> & mapFiles)
	{
		if (strDirectory.IsEmpty()){ return false; }

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		CString strDirSpec;

		DWORD dwError = 0;
		strDirSpec.Format(L"%s\\*", strDirectory);
		hFind = FindFirstFile(strDirSpec, &FindFileData);

		if (INVALID_HANDLE_VALUE == hFind)
		{
			return false;
		}

		bool bFlag = true;
		do
		{
			if (0 != wcscmp(L".", FindFileData.cFileName) && 0 != wcscmp(L"..", FindFileData.cFileName))
			{

				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					CString strNextDir;
					strNextDir.Format(L"%s\\%s", strDirectory, FindFileData.cFileName);
					EnumerateDirectoryFileToMap(strNextDir, strDirectoryBase, mapFiles);
				}
				else
				{
					CString strFileKey = strDirectory;
					CString strFilePath;
					strFilePath.Format(L"%s\\%s", strDirectory, FindFileData.cFileName);
					strFileKey.Replace(strDirectoryBase, L"");
					//strFileKey +="\\";
					if (strFileKey.IsEmpty() == FALSE)
					{
						if (strFileKey.GetAt(strFileKey.GetLength() - 1) != '\\')
						{
							strFileKey += "\\";
						}
					}
					strFileKey += FindFileData.cFileName;

					if (strFileKey.GetAt(0) == '\\' || strFileKey.GetAt(0) == '//')
					{
						strFileKey = strFileKey.Mid(1);
					}
					mapFiles[strFileKey] = strFilePath;
				}
			}
		} while (FindNextFile(hFind, &FindFileData) != 0);
		FindClose(hFind);

		return true;
	}

	int CFileZip_LiteZip::CreateZip(const std::wstring& zip_file)
	{
		HZIP hz = (HZIP)hz_;
		return ZipCreateFile(&hz_, zip_file.c_str(), 0);
	}

	int CFileZip_LiteZip::Close()
	{
		return ZipClose(hz_);
	}

	int CFileZip_LiteZip::AddZipBuffer(std::wstring& name, std::string& data, unsigned int data_length)
	{
		int result = ZipAddBuffer(hz_, name.c_str(), data.c_str(), data_length);
		return result;
	}

	int CFileZip_LiteZip::AddZipFolder(std::wstring& path)
	{
		return ZipAddFolder(hz_, path.c_str());
	}

	CFileZip_LiteZip::CFileZip_LiteZip()
	{
		hz_ = NULL;
	}

	int CFileZip_LiteZip::OpenZip(const std::wstring& zip_file)
	{
		HZIP hz = (HZIP)hz_;
		return UnzipOpenFile(&hz, zip_file.c_str(), 0);
	}

	int CFileZip_LiteUnzip::OpenZip(const std::wstring& zip_file)
	{
		return UnzipOpenFile(&hz_, zip_file.c_str(), 0);
	}

	CFileZip_LiteUnzip::CFileZip_LiteUnzip()
	{
		hz_ = NULL;
	}

	int CFileZip_LiteUnzip::Close()
	{
		int result = 0;
		if (hz_)
		{
			result = UnzipClose(hz_);
			hz_ = NULL;
		}

		return 0;
	}

	int CFileZip_LiteUnzip::UnzipFile(std::vector<std::string>& vectInfoList)
	{
		int result = 0;
		ZIPENTRY ze;
		ze.Index = -1;
		do
		{

			result = UnzipGetItem(hz_, &ze);
			if (0 != result)
			{
				break;
			}

			DWORD count = ze.Index;
			ze.Index = 0;
			std::string plist;
			for (size_t i = 0; i < count; i++)
			{
				ze.Index = i;
				UnzipGetItem(hz_, &ze);
				int nLen = ze.UncompressedSize;
				char* szbuf = (char*)malloc(nLen);
				ZeroMemory(szbuf, nLen);
				result = UnzipItemToBuffer(hz_, szbuf, nLen, &ze);
				if (0 != result)
				{
					break;
				}
				plist.assign(szbuf, nLen);
				free(szbuf);
				vectInfoList.push_back(plist);
			}
		} while (false);

		return result;
	}

	int CFileZip_LiteUnzip::UnZipOneFile(const std::wstring& file_name, std::string& file_data)
	{
		UnzipSetBaseDir(hz_, L"");
		int result = -1;
		ZIPENTRY ze;
		ze.Index = -1;
		//strcpy(ze.Name, UnicodeToUtf8(file_name).c_str());
		lstrcpy(ze.Name, file_name.c_str());
		do
		{
			result = UnzipFindItem(hz_, &ze, FALSE);
			if (0 != result)
			{
				break;
			}

			char* buf = (char*)malloc(ze.UncompressedSize);
			result = UnzipItemToBuffer(hz_, buf, ze.UncompressedSize, &ze);
			if (result != 0)
			{
				free(buf);
				break;
			}

			file_data.assign(buf, ze.UncompressedSize);
			free(buf);
		} while (false);

		return result;
	}

	bool CFileZip_LiteUnzip::ZipFileIsExist(const std::wstring& strFileName)
	{
		UnzipSetBaseDir(hz_, L"");
		ZIPENTRY ze;
		ze.Index = -1;
		lstrcpy(ze.Name, (LPCTSTR)strFileName.c_str());
		if (UnzipFindItem(hz_, &ze, FALSE) != 0)
			return false;

		return true;
	}

	int CFileZip_LiteUnzip::EnumDirFileList(const std::wstring& dir, std::vector<std::wstring>& file_list)
	{
		file_list.clear();
		int result = 0;
		UnzipSetBaseDir(hz_, L"");
		ZIPENTRY ze;
		ze.Index = -1;
		lstrcpy(ze.Name, (LPCTSTR)dir.c_str());
		do
		{
			result = UnzipGetItem(hz_, &ze);
			if (0 != result)
			{
				break;
			}

			size_t nums = ze.Index;
			bool bFind = false;
			for (size_t i = 0; i < nums; i++)
			{
				ze.Index = i;
				UnzipGetItem(hz_, &ze);
				std::wstring name = ze.Name;
				file_list.push_back(name);
			}
		} while (false);

		return result;
	}

	int CIpa_LiteUnzip::GetIpaMainDir(std::wstring& dir)
	{
		int result = 0;
		UnzipSetBaseDir(hz_, L"");
		ZIPENTRY ze;
		ze.Index = -1;

		do
		{
			result = UnzipGetItem(hz_, &ze);
			if (0 != result)
			{
				break;
			}

			size_t nums = ze.Index;
			bool bFind = false;
			for (size_t i = 0; i < nums; i++)
			{
				ze.Index = i;
				UnzipGetItem(hz_, &ze);
				std::wstring name = ze.Name;
				if (0 == name.find_first_of(L"\\Payload\\") &&
					std::wstring::npos != name.find(L".app\\"))
				{
					dir = name;
					if (0 == dir.find_first_of(L"\\"))
					{
						dir = dir.substr(1, dir.length() - 1);
					}
					break;
				}

			}
		} while (false);

		return result;
	}

	bool CIpa_LiteUnzip::HaveDoc()
	{
		UnzipSetBaseDir(hz_, L"");
		ZIPENTRY ze;
		ze.Index = -1;
		lstrcpy(ze.Name, L"Container/");
		return UnzipFindItem(hz_, &ze, TRUE) == 0;

	}

	int CIpa_LiteUnzip::GetIpaChineseDir(std::wstring& dir)
	{
		int result = 0;
		UnzipSetBaseDir(hz_, L"");
		ZIPENTRY ze;
		ze.Index = -1;

		do
		{
			std::wstring ipa_main_dir;
			result = GetIpaMainDir(ipa_main_dir);
			if (!ipa_main_dir.empty())
			{
				dir = ipa_main_dir + L"zh-Hans.lproj\\";
			}

		} while (false);

		return result;
	}
}