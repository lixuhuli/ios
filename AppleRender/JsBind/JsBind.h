#pragma once
#include "CefBase/CefAppEx.h"






class CMyJsBindCallback
	: public CJsBindCallback
{
	virtual void OnInitJavascript(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context);
};

//�Ƿ�ʹ��V1�汾
#define APPLESEED_V1

///////////////////////////////////////////////////////////////////////////////////////////
//��ҳ���û�ȡ�ͻ�����Ϣ�ӿ�

#ifdef APPLESEED_V1
//����ҳ��
/*
����ֵ��status
0.δ����
1.������
2.��װ��
3.�Ѱ�װ
4.������
*/
static bool JsGetGameStatus(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval);	//��ȡ��Ϸ״̬		window.guoren.getGameStatus(gameID, version)
static bool JsStartGame(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval);		//������Ϸ״̬		window.guoren.startGame(gameID, gameName, gameVersion)
static bool JsGetUserInfo(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval);		//��ȡ�˺���Ϣ		window.guoren.getUserInfo()
static bool JsShowMsg(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval);	        //alert������ʾ		window.guoren.showMsg(msg)
static bool JsShowToast(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval);	    //toast��ʾ		    window.guoren.showToast(msg)
static bool JsShowConfirm(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval);	    //ȷ��ȡ������		window.guoren.showConfirm(msg)
static bool JsModifyUserInfo(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval);	//�û��޸�����Ϣ    window.guoren.modifyUserInfo()

#else

#endif