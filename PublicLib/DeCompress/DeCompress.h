#pragma once
#include <string>
using std::string;
using std::wstring;




typedef void(*UncompressCallback)(int nCurrentPercent, void* pParam);


typedef struct _DeCompressParams
{
	UncompressCallback lpProgressCallback;
	void* lpParam;
	BOOL* lpNeedExit;
	wstring strError;
	int nSpeed;// KB/S
	_DeCompressParams()
		: lpProgressCallback(NULL)
		, lpParam(NULL)
		, lpNeedExit(NULL)
		, strError(L"")
		, nSpeed(0)
	{

	}
}DeCompressParams, *LPDeCompressParams;

namespace PublicLib{

	//��ѹ7z��ʽ
	bool DeCompress7zFile(const wstring& str7zPath, const wstring& strDesPath, IN OUT DeCompressParams& params);

	bool DeCompress7zMem(void* pMem, long nMemLength, const wstring& strDesPath, IN OUT DeCompressParams& params);

	//��ѹzip��ʽ
	bool DeCompressZipFile(const wstring& strZipPath, const wstring& strDesPath, IN OUT DeCompressParams& params);

	bool DeCompressZipMem(BYTE* lpData, DWORD dwSize, const wstring& strDesPath, IN OUT DeCompressParams& params);

	//�Զ�ʶ���ʽ��ѹ
	bool DeCompressFile(const wstring& strFilePath, const wstring& strDesPath, IN OUT DeCompressParams& params);

	bool DeCompressMem(void* lpData, DWORD dwSize, const wstring& strDesPath, IN OUT DeCompressParams& params);
}