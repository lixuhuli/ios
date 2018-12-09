#pragma once


namespace PublicLib
{
	//删除文件夹
	void RemoveDir(const wchar_t* pDirPath);

	//计算文件的MD5值
	string CalFileMd5(const wstring& strFilePath, const int nBuffSize = 1024 * 1024);

	//获取文件大小
	__int64 GetFileSizeW(const wstring& strFile);
	__int64 GetFileSizeA(const string& strFile);

	//计算文件夹大小
	__int64 GetFolderSize(const wstring& strDir);
}