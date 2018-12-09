#include "JsBind.h"
#include <PublicLib/json/json-forwards.h>
#include <PublicLib/json/json.h>
#include <PublicLib/StrHelper.h>
#include <SoftDefine.h>
#include <AppHelper.h>
#include <PublicLib/StrHelper.h>
#include <PublicLib/System.h>
#include <atlfile.h>
#include "../../AppleSeed/MsgDefine.h"

#define MAX_USER_INFO_SIZE		1024
#define MAX_CLIENT_INFO_SIZE	256
typedef struct CEF_SHARE_DATA_ {
    HWND hMainWnd;
    UINT nLoadCount;
    char szUserInfo[MAX_USER_INFO_SIZE + 1];
    char szClientInfo[MAX_CLIENT_INFO_SIZE + 1];
}CEF_SHARE_DATA, *LPCEF_SHARE_DATA;

static LPCWSTR g_pShareGuid = L"{B6364C8A-47E4-4FD1-9E0D-658DB9CFC5A9}";
CAtlFileMapping<CEF_SHARE_DATA> g_shareData;

static const struct FuncList{
	wstring szName;
	JS_CALLBACK_FUN func;
}

g_funcList[] = {
#ifdef APPLESEED_V1
	{ L"getGameStatus", JsGetGameStatus },
	{ L"startGame", JsStartGame },
    { L"getUserInfo", JsGetUserInfo },
    { L"showMsg", JsShowMsg },
    { L"showToast", JsShowToast },
    { L"showConfirm", JsShowConfirm },
    { L"modifyUserInfo", JsModifyUserInfo },
#else

#endif
};

inline void AddFunction(CefRefPtr<CefV8Value> pObj, LPCWSTR lpFunName, JS_CALLBACK_FUN func)
{
	CefRefPtr<CefV8Handler> myV8handle = new CCefV8Handler();
	((CCefV8Handler*)myV8handle.get())->AddFun(lpFunName, func);
	CefRefPtr<CefV8Value> myFun = CefV8Value::CreateFunction(lpFunName, myV8handle);
	pObj->SetValue(lpFunName, myFun, V8_PROPERTY_ATTRIBUTE_NONE);
}

void CMyJsBindCallback::OnInitJavascript(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	CefRefPtr<CefV8Value> window = context->GetGlobal();
	CefRefPtr<CefV8Accessor> myV8Acc = new CCefV8Accessor;
	CefRefPtr<CefV8Value> val = CefV8Value::CreateString(L"guoren");
	CefString cefException;
	myV8Acc->Set(L"name", window, val, cefException);
	CefRefPtr<CefV8Value> pObjApp = CefV8Value::CreateObject(myV8Acc);
	window->SetValue(L"guoren", pObjApp, V8_PROPERTY_ATTRIBUTE_NONE);
	for (int i = 0; i < _countof(g_funcList);++i)
		AddFunction(pObjApp, g_funcList[i].szName.c_str(), g_funcList[i].func);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef APPLESEED_V1
//get
bool JsGetGameStatus(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval)
{
	string strJson = PublicLib::UtfToA(arguments[0]->GetStringValue());
	COPYDATASTRUCT cs;
	ZeroMemory(&cs, sizeof(COPYDATASTRUCT));
	cs.lpData = (LPVOID)strJson.c_str();
	cs.cbData = strJson.size() * 2;
	cs.dwData = ID_MW_COPYDATA_JS_GAME_STATUS;
	g_shareData.OpenMapping(g_pShareGuid, sizeof(CEF_SHARE_DATA));
	HWND hMainWnd = ((LPCEF_SHARE_DATA)g_shareData)->hMainWnd;
	g_shareData.Unmap();
	int nStatus = ::SendMessage(hMainWnd, WM_COPYDATA, 0, (LPARAM)&cs);
	retval = CefV8Value::CreateInt(nStatus);
	return true;
}

bool JsStartGame(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval)
{
	string strJson = arguments[0]->GetStringValue();
	COPYDATASTRUCT cs;
	ZeroMemory(&cs, sizeof(COPYDATASTRUCT));
	cs.lpData = (LPVOID)strJson.c_str();
	cs.cbData = strJson.size() * 2;
	cs.dwData = ID_MW_COPYDATA_JS_STARTGAME;
	g_shareData.OpenMapping(g_pShareGuid, sizeof(CEF_SHARE_DATA));
	HWND hMainWnd = ((LPCEF_SHARE_DATA)g_shareData)->hMainWnd;
	g_shareData.Unmap();
	int nStatus = ::SendMessage(hMainWnd, WM_COPYDATA, 0, (LPARAM)&cs);
    retval = CefV8Value::CreateInt(nStatus);
	return true;
}

bool JsGetUserInfo(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval) {
    g_shareData.OpenMapping(g_pShareGuid, sizeof(CEF_SHARE_DATA));
    retval = CefV8Value::CreateString(((LPCEF_SHARE_DATA)g_shareData)->szUserInfo);
    g_shareData.Unmap();
    return true;
}

bool JsShowMsg(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval) {
    string prefix = arguments[0]->GetStringValue();
    if (prefix.empty()) return true;

    Json::Value vRoot;
    vRoot["prefix"] = prefix;
    Json::FastWriter fw;
    string strJson = fw.write(vRoot);

    COPYDATASTRUCT cs;
    ZeroMemory(&cs, sizeof(COPYDATASTRUCT));
    cs.lpData = (LPVOID)strJson.c_str();
    cs.cbData = strJson.size() * 2;
    cs.dwData = ID_MW_COPYDATA_JS_SHOWMSG;
    g_shareData.OpenMapping(g_pShareGuid, sizeof(CEF_SHARE_DATA));

    HWND hMainWnd = ((LPCEF_SHARE_DATA)g_shareData)->hMainWnd;
    g_shareData.Unmap();

    ::SendMessage(hMainWnd, WM_COPYDATA, 0, (LPARAM)&cs);
    return true;
}

bool JsShowToast(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval) {
    string prefix = arguments[0]->GetStringValue();
    if (prefix.empty()) return true;

    Json::Value vRoot;
    vRoot["prefix"] = prefix;
    Json::FastWriter fw;
    string strJson = fw.write(vRoot);

    COPYDATASTRUCT cs;
    ZeroMemory(&cs, sizeof(COPYDATASTRUCT));
    cs.lpData = (LPVOID)strJson.c_str();
    cs.cbData = strJson.size() * 2;
    cs.dwData = ID_MW_COPYDATA_JS_SHOWTOAST;
    g_shareData.OpenMapping(g_pShareGuid, sizeof(CEF_SHARE_DATA));

    HWND hMainWnd = ((LPCEF_SHARE_DATA)g_shareData)->hMainWnd;
    g_shareData.Unmap();

    ::SendMessage(hMainWnd, WM_COPYDATA, 0, (LPARAM)&cs);
    return true;
}

bool JsShowConfirm(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval) {
    string prefix = arguments[0]->GetStringValue();
    if (prefix.empty()) return true;

    Json::Value vRoot;
    vRoot["prefix"] = prefix;
    Json::FastWriter fw;
    string strJson = fw.write(vRoot);

    COPYDATASTRUCT cs;
    ZeroMemory(&cs, sizeof(COPYDATASTRUCT));
    cs.lpData = (LPVOID)strJson.c_str();
    cs.cbData = strJson.size() * 2;
    cs.dwData = ID_MW_COPYDATA_JS_SHOWCONFIRM;
    g_shareData.OpenMapping(g_pShareGuid, sizeof(CEF_SHARE_DATA));

    HWND hMainWnd = ((LPCEF_SHARE_DATA)g_shareData)->hMainWnd;
    g_shareData.Unmap();

    int nStatus = ::SendMessage(hMainWnd, WM_COPYDATA, 0, (LPARAM)&cs);
    retval = CefV8Value::CreateInt(nStatus);

    return true;
}

bool JsModifyUserInfo(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval) {
    g_shareData.OpenMapping(g_pShareGuid, sizeof(CEF_SHARE_DATA));
    HWND hMainWnd = ((LPCEF_SHARE_DATA)g_shareData)->hMainWnd;
    g_shareData.Unmap();
    ::PostMessage(hMainWnd, WM_MAINWND_MSG_COMMON, WpCommonModifyUserInfo, 0);
    return true;
}

#else

#endif

