#pragma once


namespace PublicLib
{
	//ɾ���ļ���
	void RemoveDir(const wchar_t* pDirPath);

	//�����ļ���MD5ֵ
	string CalFileMd5(const wstring& strFilePath, const int nBuffSize = 1024 * 1024);

	//��ȡ�ļ���С
	__int64 GetFileSizeW(const wstring& strFile);
	__int64 GetFileSizeA(const string& strFile);

	//�����ļ��д�С
	__int64 GetFolderSize(const wstring& strDir);
}