#include "stdafx.h"
#include "FileHelper.h"
#include "MD5/md5.h"


namespace PublicLib
{
	void RemoveDir(const wchar_t* pDirPath)
	{
		WIN32_FIND_DATA	wfd;
		wchar_t szFindPath[MAX_PATH] = { 0 };
		swprintf_s(szFindPath, L"%s\\*", pDirPath);
		HANDLE hFile = FindFirstFile(szFindPath, &wfd);
		if (INVALID_HANDLE_VALUE == hFile)
			return;
		wchar_t szDirPath[MAX_PATH] = { 0 };
		while (FindNextFile(hFile, &wfd))
		{
			if (wcscmp(wfd.cFileName, L".") == 0 || wcscmp(wfd.cFileName, L"..") == 0)
				continue;
			memset(szDirPath, 0, MAX_PATH);
			swprintf_s(szDirPath, L"%s\\%s", pDirPath, wfd.cFileName);
			if (wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				RemoveDir(szDirPath);
				continue;
			}
			DeleteFile(szDirPath);
		}
		FindClose(hFile);
		RemoveDirectory(pDirPath);
	}

	string CalFileMd5(const wstring& strFilePath, const int nBuffSize /*= 1024 * 1024*/)
	{
		std::string strMd5;
		FILE* fp = NULL;
		_wfopen_s(&fp, strFilePath.c_str(), L"rb");
		if (NULL == fp)
			return strMd5;
		MD5 md5;
		BYTE* lpData = (BYTE*)malloc(nBuffSize);
		while (!feof(fp))
		{
			size_t nRead = fread(lpData, 1, nBuffSize, fp);
			if (nRead <= 0)
				break;
			md5.update(lpData, nRead);
		}
		fclose(fp);
		free(lpData);
		md5.finalize();
		strMd5 = md5.md5();
		return strMd5;
	}

	__int64 GetFileSizeW(const wstring& strFile)
	{
		__int64 nSize = 0;
		HANDLE hFile = CreateFileW(strFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
			return nSize;
		LARGE_INTEGER size;
		if (::GetFileSizeEx(hFile, &size))
		{
			nSize += size.QuadPart;
		}
		CloseHandle(hFile);
		return nSize;
	}

	__int64 GetFileSizeA(const string& strFile)
	{
		__int64 nSize = 0;
		HANDLE hFile = CreateFileA(strFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
			return nSize;
		LARGE_INTEGER size;
		if (::GetFileSizeEx(hFile, &size))
		{
			nSize += size.QuadPart;
		}
		CloseHandle(hFile);
		return nSize;
	}

	__int64 GetFolderSize(const wstring& strDir)
	{
		__int64 nSize = 0;
		wstring strRootPath = strDir + L"\\";
		wstring strRoot = strRootPath + L"*.*";
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(strRoot.c_str(), &fd);
		if (INVALID_HANDLE_VALUE == hFind)
			return nSize;
		while (FindNextFile(hFind, &fd))
		{
			if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0)
				continue;
			wstring strPath = strRootPath + fd.cFileName;
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				nSize += GetFolderSize(strPath);
			else
			{
				HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (INVALID_HANDLE_VALUE == hFile)
					continue;
				LARGE_INTEGER size;
				if (::GetFileSizeEx(hFile, &size))
				{
					nSize += size.QuadPart;
				}
				CloseHandle(hFile);
			}
		}
		FindClose(hFind);
		return nSize;
	}

}