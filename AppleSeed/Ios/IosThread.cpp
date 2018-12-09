#include "stdafx.h"
#include "IosThread.h"

CIosThread::CIosThread()
    : PublicLib::CThread() {
}

CIosThread::~CIosThread() {
}

bool CIosThread::Create(char* thread_name, void* argument, const CALL_BACK_FUNCTION& call_back) {
    m_bHasExited = FALSE;

    if (!CreateThread(thread_name, argument)) return false;

    call_back_func_ = call_back;

    ResumeThread();

    return true;
}

int CIosThread::Run(void * argument) {
    return call_back_func_ ? call_back_func_(argument) : 0;
}