#pragma once
#include "CefBase/CefAppEx.h"






class CMyJsBindCallback
	: public CJsBindCallback
{
	virtual void OnInitJavascript(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context);
};

//是否使用V1版本
#define APPLESEED_V1

///////////////////////////////////////////////////////////////////////////////////////////
//网页调用获取客户端信息接口

#ifdef APPLESEED_V1
//下载页面
/*
返回值：status
0.未下载
1.下载中
2.安装中
3.已安装
4.待更新
*/
static bool JsGetGameStatus(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval);	//获取游戏状态		window.guoren.getGameStatus(gameID, version)
static bool JsStartGame(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval);		//设置游戏状态		window.guoren.startGame(gameID, gameName, gameVersion)
static bool JsGetUserInfo(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval);		//获取账号信息		window.guoren.getUserInfo()
static bool JsShowMsg(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval);	        //alert弹框提示		window.guoren.showMsg(msg)
static bool JsShowToast(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval);	    //toast提示		    window.guoren.showToast(msg)
static bool JsShowConfirm(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval);	    //确定取消弹窗		window.guoren.showConfirm(msg)
static bool JsModifyUserInfo(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval);	//用户修改了信息    window.guoren.modifyUserInfo()

#else

#endif