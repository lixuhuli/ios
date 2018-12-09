#include "stdafx.h"
#include "DeCompress.h"
#include "7zRes/Types.h"
#include "7zRes/7zAlloc.h"
#include "7zRes/7zAlloc.c"
#include "7zRes/7zBuf.h"
#include "7zRes/7zFile.h"
#include "7zRes/7z.h"
#include "7zRes/7zCrc.h"
#include "Unzip/unzip.h"
#include <ShlObj.h>
#include <time.h>
using namespace MiniZip;


namespace PublicLib{

	bool DeCompress7zFile(const wstring& str7zPath, const wstring& strDesPath, IN OUT DeCompressParams& params)
	{
		ISzAlloc allocImp;
		allocImp.Alloc = SzAlloc;
		allocImp.Free = SzFree;

		ISzAlloc allocTempImp;
		allocTempImp.Alloc = SzAllocTemp;
		allocTempImp.Free = SzFreeTemp;

		CFileInStream archiveStream;
		if (InFile_OpenW(&archiveStream.file, str7zPath.c_str()))
		{
			params.strError = L"打开文件：" + str7zPath + L"失败";
			return false;
		}
		FileInStream_CreateVTable(&archiveStream);

		CLookToRead lookStream;
		LookToRead_CreateVTable(&lookStream, False);
		lookStream.realStream = &archiveStream.s;
		LookToRead_Init(&lookStream);

		CrcGenerateTable();

		CSzArEx db;
		SzArEx_Init(&db);
		//OutputDebugString(L"SzArEx_Open\n");
		SRes nRet = SzArEx_Open(&db, &lookStream.s, &allocImp, &allocTempImp);
		if (nRet != SZ_OK)
		{
			SzArEx_Free(&db, &allocImp);
			File_Close(&archiveStream.file);
			params.strError = L"SzArEx_Open打开压缩资源失败！";
			return false;
		}

		UInt32 blockIndex = 0xFFFFFFFF; /* it can have any value before first call (if outBuffer = 0) */
		Byte *outBuffer = 0; /* it must be 0 before first call for each new archive. */
		size_t outBufferSize = 0;  /* it can have any value before first call (if outBuffer = 0) */
		for (UInt32 i = 0; i < db.db.NumFiles; i++)
		{
			if (params.lpNeedExit && (*params.lpNeedExit == TRUE))
				break;//外面控制需要退出
			if (params.lpProgressCallback)
			{
				int nPercent = int(i / (float)db.db.NumFiles * 100);
				params.lpProgressCallback(nPercent, params.lpParam);
			}
			const CSzFileItem *f = db.db.Files + i;

			size_t nNameLength = SzArEx_GetFileNameUtf16(&db, i, NULL);
			if (nNameLength <= 0)
			{
				nRet = SZ_ERROR_READ;
				params.strError = L"SzArEx_GetFileNameUtf16失败";
				break;
			}

			UInt16* pNameBuf = (UInt16 *)SzAlloc(NULL, (nNameLength + 1) * sizeof(UInt16));
			if (!pNameBuf)
			{
				nRet = SZ_ERROR_MEM;
				params.strError = L"SzAlloc失败";
				break;
			}
			memset(pNameBuf, 0, (nNameLength + 1) * sizeof(UInt16));

			SzArEx_GetFileNameUtf16(&db, i, pNameBuf);
			wstring wstrFileName((wchar_t*)pNameBuf);
			for (size_t m = 0; m < wstrFileName.size(); ++m)
			{
				if (wstrFileName[m] == '/')
					wstrFileName[m] = '\\';
			}
			SzFree(NULL, pNameBuf);
			wstring wstrFilePath = strDesPath;
			if (wstrFilePath.at(wstrFilePath.length() - 1) != L'\\')
				wstrFilePath += L"\\";
			wstrFilePath += wstrFileName;

			size_t offset = 0;
			size_t outSizeProcessed = 0;
			nRet = SzArEx_Extract(&db, &lookStream.s, i,
				&blockIndex, &outBuffer, &outBufferSize,
				&offset, &outSizeProcessed,
				&allocImp, &allocTempImp);
			if (nRet != SZ_OK)
			{
				params.strError = L"SzArEx_Extract失败";
				break;
			}
			//makepath
			if (f->IsDir)
			{
				int ErrCode = SHCreateDirectoryEx(NULL, wstrFilePath.c_str(), NULL);
				if (ErrCode == ERROR_BAD_PATHNAME || ErrCode == ERROR_FILENAME_EXCED_RANGE || ErrCode == ERROR_PATH_NOT_FOUND)
				{
					OutputDebugString(L"can not create output folder\n");
					nRet = SZ_ERROR_WRITE;
					params.strError = L"创建文件夹：" + wstrFileName + L"失败";
					break;
				}
				continue;
			}
			else
			{
				int nPos = wstrFilePath.find_last_of('\\');
				if (nPos != wstring::npos)
				{
					wstring strDir = wstrFilePath.substr(0, nPos);
					SHCreateDirectoryEx(NULL, strDir.c_str(), NULL);
				}
			}
			bool bFlag = false, bIgnore = false;
			CSzFile outFile;
			for (int i = 0; i < 3; ++i)
			{
				if (0 == OutFile_OpenW(&outFile, wstrFilePath.c_str()))
				{
					bFlag = true;
					break;
				}
				//重试机制
				Sleep(500);
			}
			if (bIgnore)
				continue;
			if (!bFlag)
			{
				nRet = SZ_ERROR_WRITE;
				params.strError = L"释放文件：" + wstrFileName + L"失败";
				break;//尝试3次后，依然失败了
			}
			size_t processedSize = outSizeProcessed;
			if (File_Write(&outFile, outBuffer + offset, &processedSize) != 0 || processedSize != outSizeProcessed)
			{
				OutputDebugString(L"can not write output file\n");
				File_Close(&outFile);
				nRet = SZ_ERROR_WRITE;
				params.strError = L"写入文件：" + wstrFileName + L"失败";
				break;
			}
			File_Close(&outFile);

			if (f->AttribDefined) SetFileAttributes(wstrFilePath.c_str(), f->Attrib);
		}

		IAlloc_Free(&allocImp, outBuffer);
		SzArEx_Free(&db, &allocImp);
		File_Close(&archiveStream.file);
		return nRet == SZ_OK;
	}

	typedef struct
	{
		ISeekInStream s;
		void* pMem;
		__int64 nMemPointer;
		__int64 nMemLength;
	} CMemInStream;

	WRes Mem_Read(CMemInStream* pMemStream, void *data, size_t *size)
	{
		if (*size == 0) return 0;
		size_t nRestSize = size_t(pMemStream->nMemLength - pMemStream->nMemPointer);
		if (nRestSize <= 0)
		{
			*size = 0;
			return 0;
		}
		if (*size > nRestSize) *size = nRestSize;

		memcpy(data, (BYTE*)(pMemStream->pMem) + (pMemStream->nMemPointer), *size);
		pMemStream->nMemPointer += *size;

		return 0;
	}

	WRes Mem_Seek(CMemInStream* pMemStream, Int64 *pos, ESzSeek origin)
	{
		switch (origin)
		{
			case SZ_SEEK_SET:
			{
				if (*pos > pMemStream->nMemLength) pMemStream->nMemPointer = pMemStream->nMemLength;
				else pMemStream->nMemPointer = *pos;
			}break;
			case SZ_SEEK_CUR:
			{
				__int64 nRestSize = pMemStream->nMemLength - pMemStream->nMemPointer;
				if (*pos > nRestSize) pMemStream->nMemPointer = pMemStream->nMemLength;
				else pMemStream->nMemPointer += *pos;
			}break;
			case SZ_SEEK_END:
			{
				pMemStream->nMemPointer = pMemStream->nMemLength;
			}break;
			default: 
				return ERROR_INVALID_PARAMETER;
		}

		*pos = pMemStream->nMemPointer;
		return 0;
	}

	static SRes MemInStream_Read(void *pp, void *buf, size_t *size)
	{
		CMemInStream *p = (CMemInStream *)pp;
		return (Mem_Read(p, buf, size) == 0) ? SZ_OK : SZ_ERROR_READ;
	}

	static SRes MemInStream_Seek(void *pp, Int64 *pos, ESzSeek origin)
	{
		CMemInStream *p = (CMemInStream *)pp;
		return Mem_Seek(p, pos, origin);
	}

	void MemInStream_CreateVTable(CMemInStream *p)
	{
		p->s.Read = MemInStream_Read;
		p->s.Seek = MemInStream_Seek;
	}

	bool DeCompress7zMem(void* pMem, DWORD dwSize, const wstring& strDesPath, IN OUT DeCompressParams& params)
	{
		ISzAlloc allocImp;
		allocImp.Alloc = SzAlloc;
		allocImp.Free = SzFree;

		ISzAlloc allocTempImp;
		allocTempImp.Alloc = SzAllocTemp;
		allocTempImp.Free = SzFreeTemp;

		CMemInStream memStream;
		memStream.pMem = pMem;
		memStream.nMemLength = dwSize;
		memStream.nMemPointer = 0;
		MemInStream_CreateVTable(&memStream);

		CLookToRead lookStream;
		LookToRead_CreateVTable(&lookStream, False);
		lookStream.realStream = &memStream.s;
		LookToRead_Init(&lookStream);

		CrcGenerateTable();

		CSzArEx db;
		SzArEx_Init(&db);

		SRes nRet = SzArEx_Open(&db, &lookStream.s, &allocImp, &allocTempImp);
		if (nRet != SZ_OK)
		{
			SzArEx_Free(&db, &allocImp);
			params.strError = L"打开7Z资源失败";
			return false;
		}

		UInt32 blockIndex = 0xFFFFFFFF; /* it can have any value before first call (if outBuffer = 0) */
		Byte *outBuffer = 0; /* it must be 0 before first call for each new archive. */
		size_t outBufferSize = 0;  /* it can have any value before first call (if outBuffer = 0) */
		for (UInt32 i = 0; i < db.db.NumFiles; i++)
		{
			if (params.lpNeedExit && (*params.lpNeedExit == TRUE))
				break;//外面控制需要退出
			if (params.lpProgressCallback)
			{
				int nPercent = int(i / (float)db.db.NumFiles * 100);
				params.lpProgressCallback(nPercent, params.lpParam);
			}
			const CSzFileItem *f = db.db.Files + i;

			size_t nNameLength = SzArEx_GetFileNameUtf16(&db, i, NULL);
			if (nNameLength <= 0)
			{
				nRet = SZ_ERROR_READ;
				params.strError = L"SzArEx_GetFileNameUtf16失败";
				break;
			}

			UInt16* pNameBuf = (UInt16 *)SzAlloc(NULL, (nNameLength + 1) * sizeof(UInt16));
			if (!pNameBuf)
			{
				nRet = SZ_ERROR_MEM;
				params.strError = L"SzAlloc失败";
				break;
			}
			memset(pNameBuf, 0, (nNameLength + 1) * sizeof(UInt16));

			SzArEx_GetFileNameUtf16(&db, i, pNameBuf);
			wstring wstrFileName((wchar_t*)pNameBuf);
			for (size_t m = 0; m < wstrFileName.size(); ++m)
			{
				if (wstrFileName[m] == '/')
					wstrFileName[m] = '\\';
			}
			SzFree(NULL, pNameBuf);

			wstring wstrFilePath(strDesPath);
			if (wstrFilePath.at(wstrFilePath.length() - 1) != L'\\') wstrFilePath += L"\\";
			wstrFilePath += wstrFileName;
			size_t offset = 0;
			size_t outSizeProcessed = 0;
			nRet = SzArEx_Extract(&db, &lookStream.s, i,
				&blockIndex, &outBuffer, &outBufferSize,
				&offset, &outSizeProcessed,
				&allocImp, &allocTempImp);
			if (nRet != SZ_OK)
			{
				params.strError = L"SzArEx_Extract失败";
				break;
			}

			//makepath
			if (f->IsDir)
			{
				int ErrCode = SHCreateDirectoryEx(NULL, wstrFilePath.c_str(), NULL);
				if (ErrCode == ERROR_BAD_PATHNAME || ErrCode == ERROR_FILENAME_EXCED_RANGE || ErrCode == ERROR_PATH_NOT_FOUND)
				{
					nRet = SZ_ERROR_WRITE;
					params.strError = L"创建文件夹：" + wstrFileName + L"失败";
					break;
				}
				continue;
			}
			else
			{
				int nPos = wstrFilePath.find_last_of('\\');
				if (nPos != wstring::npos)
				{
					wstring strDir = wstrFilePath.substr(0, nPos);
					SHCreateDirectoryEx(NULL, strDir.c_str(), NULL);
				}
			}
			CSzFile outFile;
			if (OutFile_OpenW(&outFile, wstrFilePath.c_str()))
			{
				nRet = SZ_ERROR_WRITE;
				params.strError = L"释放文件：" + wstrFileName + L"失败";
				break;
			}

			size_t processedSize = outSizeProcessed;
			if (File_Write(&outFile, outBuffer + offset, &processedSize) != 0 || processedSize != outSizeProcessed)
			{
				File_Close(&outFile);
				nRet = SZ_ERROR_WRITE;
				params.strError = L"写入文件：" + wstrFileName + L"失败";
				break;
			}
			File_Close(&outFile);

			if (f->AttribDefined) SetFileAttributes(wstrFilePath.c_str(), f->Attrib);
		}

		IAlloc_Free(&allocImp, outBuffer);
		SzArEx_Free(&db, &allocImp);

		return nRet == SZ_OK;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	enum UnzipError
	{
		Err_FileUnExist = 0,
		Err_CreateDir,
		Err_OpenZip,
		Err_Unzip,
	};


	bool DeCompressZipFile(const wstring& strZipPath, const wstring& strDesPath, IN OUT DeCompressParams& params)
	{
		bool bResult = false;
		HZIP hUnzip = NULL;
		try
		{
			if (!PathFileExists(strZipPath.c_str()))
				throw Err_FileUnExist;
			if (!PathFileExists(strDesPath.c_str()) && !SHCreateDirectory(NULL, strDesPath.c_str()))
				throw Err_CreateDir;
			hUnzip = OpenZip(strZipPath.c_str(), NULL);
			if (NULL == hUnzip)
				throw Err_OpenZip;
			ZIPENTRY ze;
			ZRESULT zr;
			zr = GetZipItem(hUnzip, -1, &ze);
			if (zr != ZR_OK)
				throw Err_Unzip;
			SetUnzipBaseDir(hUnzip, strZipPath.c_str());
			int nCount = ze.index;
			wchar_t szPath[MAX_PATH + 1] = { 0 };
			__int64 nBeginTime = time(NULL);
			__int64 nTotalSize = 0;
			for (int i = 0; i < nCount; ++i)
			{
				if (params.lpNeedExit && (*params.lpNeedExit == TRUE))
					break;//外面控制需要退出
				zr = GetZipItem(hUnzip, i, &ze);
				if (zr != ZR_OK)
					continue;
				nTotalSize += ze.unc_size;
				int nLen = _tcslen(ze.name);
				for (int j = 0; j < nLen; ++j)
				{
					if (ze.name[j] == '/')
						ze.name[j] = '\\';
				}
				memset(szPath, 0, sizeof(TCHAR)*(MAX_PATH + 1));
				swprintf_s(szPath, _T("%s\\%s"), strDesPath.c_str(), ze.name);
				if (ze.attr & FILE_ATTRIBUTE_DIRECTORY)
					SHCreateDirectory(NULL, szPath);
				else
					UnzipItem(hUnzip, i, szPath);
				if (params.lpProgressCallback)
				{
					int nPercent = int(100 * ((i + 1) / float(nCount)));
					params.lpProgressCallback(nPercent, params.lpParam);
				}
			}
			__int64 nMilSeconds = time(NULL) - nBeginTime;
			params.nSpeed = (nMilSeconds>0)? int((nTotalSize/1024.0)/float(nMilSeconds)):(int)(nTotalSize/1024);
			bResult = true;
		}
		catch (UnzipError error)
		{
			wchar_t szMsg[MAX_PATH * 2 + 2] = { 0 };
			swprintf_s(szMsg, L"zip=%s, des=%s, err=%d", strZipPath.c_str(), strDesPath.c_str(), error);
			params.strError = szMsg;
		}
		if (hUnzip)
			CloseZip(hUnzip);
		return bResult;
	}

	bool DeCompressZipMem(void* lpData, DWORD dwSize, const wstring& strDesPath, IN OUT DeCompressParams& params)
	{
		bool bResult = false;
		HZIP hUnzip = NULL;
		try
		{
			if (!PathFileExists(strDesPath.c_str()) && !SHCreateDirectory(NULL, strDesPath.c_str()))
				throw Err_CreateDir;
			hUnzip = OpenZip(lpData, dwSize, NULL);
			if (NULL == hUnzip)
				throw Err_OpenZip;
			ZIPENTRY ze;
			ZRESULT zr;
			zr = GetZipItem(hUnzip, -1, &ze);
			if (zr != ZR_OK)
				throw Err_Unzip;
			SetUnzipBaseDir(hUnzip, strDesPath.c_str());
			int nCount = ze.index;
			wchar_t szPath[MAX_PATH + 1] = { 0 };
			for (int i = 0; i < nCount; ++i)
			{
				if (params.lpNeedExit && (*params.lpNeedExit == TRUE))
					break;//外面控制需要退出
				zr = GetZipItem(hUnzip, i, &ze);
				if (zr != ZR_OK)
					continue;
				int nLen = (int)_tcslen(ze.name);
				for (int j = 0; j < nLen; ++j)
				{
					if (ze.name[j] == '/')
						ze.name[j] = '\\';
				}
				memset(szPath, 0, sizeof(TCHAR)*(MAX_PATH + 1));
				swprintf_s(szPath, L"%s\\%s", strDesPath.c_str(), ze.name);
				if (ze.attr & FILE_ATTRIBUTE_DIRECTORY)
					SHCreateDirectory(NULL, szPath);
				else
					UnzipItem(hUnzip, i, szPath);
				if (params.lpProgressCallback)
				{
					int nPercent = int(100 * ((i + 1) / float(nCount)));
					params.lpProgressCallback(nPercent, params.lpParam);
				}
			}
			bResult = true;
		}
		catch (UnzipError error)
		{
			wchar_t szMsg[MAX_PATH * 2 + 2] = { 0 };
			swprintf_s(szMsg, L"des=%s, err=%d", strDesPath.c_str(), error);
			params.strError = szMsg;
			bResult = false;
		}
		if (hUnzip)
			CloseZip(hUnzip);
		return bResult;
	}

	enum ZipType
	{
		zt_Lzma,
		zt_Zip,
		zt_UnKnow,
	};

	inline ZipType GetZipType(const wstring& strFilePath)
	{
		FILE* fp = NULL;
		_wfopen_s(&fp, strFilePath.c_str(), L"rb");
		if (NULL == fp)
		{
			return zt_UnKnow;
		}
		byte bFlag[2];
		fread(bFlag, 1, 2, fp);
		fclose(fp);
		if ((bFlag[0] == 0x37) && (bFlag[1] == 0x7A))
			return zt_Lzma;
		if ((bFlag[0] == 0x50) && (bFlag[1] == 0x4B))
			return zt_Zip;
		return zt_UnKnow;
	}

	bool DeCompressFile(const wstring& strFilePath, const wstring& strDesPath, IN OUT DeCompressParams& params)
	{
		ZipType zt = GetZipType(strFilePath);
		if (zt == zt_Lzma)
			return DeCompress7zFile(strFilePath, strDesPath, params);
		if (zt == zt_Zip)
			return DeCompressZipFile(strFilePath, strDesPath, params);
		params.strError = L"不可识别的压缩格式";
		return false;
	}

	bool DeCompressMem(void* lpData, DWORD dwSize, const wstring& strDesPath, IN OUT DeCompressParams& params)
	{
		BYTE* lpFlag = (BYTE*)lpData;
		if ((lpFlag[0] == 0x37) && (lpFlag[1] == 0x7A))
			return DeCompress7zMem(lpData, dwSize, strDesPath, params);
		if ((lpFlag[0] == 0x50) && (lpFlag[1] == 0x4B))
			return DeCompressZipMem(lpData, dwSize, strDesPath, params);
		params.strError = L"不可识别的压缩格式";
		return false;
	}

}