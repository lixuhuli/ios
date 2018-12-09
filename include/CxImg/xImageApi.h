#pragma once
#include <string>
#include "ximadef.h"

//Ñ¹ËõÍ¼Æ¬´íÎóÂë
#define				LC_ERROR_UNDEFINE_CODE							-1
#define				LC_ERROR_SUCCESS								0
#define				LC_ERROR_READ_FILE								600
#define				LC_ERROR_WRITE_FILE								601
#define             LC_ERROR_DECODE_IMAGE							602
#define             LC_ERROR_ENCODE_IMAGE							603
#define             LC_ERROR_GET_FOLDER								604
#define             LC_ERROR_CREATE_FOLDER							605
#define             LC_ERROR_CUT_OUT_IMAGE							606



#ifdef __cplusplus
extern "C"
{
#endif

	DLL_EXP int CompressImageFileA(const char* src_file, int width, int height, int quality, const char* dst_file);
	DLL_EXP int CompressImageFileW(const wchar_t* src_file, int width, int height, int quality, const wchar_t* dst_file);

	DLL_EXP int CompressImageDataA(unsigned char* lpData, int nSize, int width, int height, int quality, const char* dst_file);
	DLL_EXP int CompressImageDataW(unsigned char* lpData, int nSize, int width, int height, int quality, const wchar_t* dst_file);

#ifdef __cplusplus
};
#endif