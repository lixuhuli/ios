#pragma once


//Content-Type: application/json
/*
md5('{appID}_{clientId}_{privateKey}');
484781
*/

//http://192.168.70.72:8080/swagger-ui.html#/23458251433147119978202562596825454


static const wchar_t* g_pStrAppID = L"wufan_pc_event";
static const wchar_t* g_pStrPriKey = L"484781";

//#define TEST
#ifdef TEST

//获取token get
#define	URL_GET_TOKEN	L"http://192.168.70.72:8080/api/auth"

//投递数据
#define	URL_POST		L"http://192.168.70.72:8080/api"

#else

#define	URL_GET_TOKEN	L"http://mqsservice.5fun.com/api/auth"

#define	URL_POST		L"http://mqsservice.5fun.com/api"
#endif