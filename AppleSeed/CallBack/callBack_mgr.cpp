#include "stdafx.h"
#include "callback_mgr.h"

CCallBackMgr::CCallBackMgr() {
}

CCallBackMgr::~CCallBackMgr() {
}

bool CCallBackMgr::Init() {
    __super::Init();

    return true;
}

void CCallBackMgr::Exit() {
    __super::Exit();
}

CCallBackMgr* CCallBackMgr::Instance() {
    static CCallBackMgr obj;
    return &obj;
}