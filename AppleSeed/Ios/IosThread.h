#ifndef _IOS_THREAD_INCLUDE_H_
#define _IOS_THREAD_INCLUDE_H_

#pragma once
#include "cef_weak_ptr.h"
#include "PublicLib/HttpLib.h"
#include "cef_bind.h"
#include <functional>

class CIosThread
    : public base::SupportsWeakPtr<CIosThread>
    , public PublicLib::CThread {
public:
    typedef std::function<int(void* argument)> CALL_BACK_FUNCTION;
public:
    CIosThread();
    virtual ~CIosThread();

public:
    bool Create(char* thread_name, void* argument, const CALL_BACK_FUNCTION& call_back);

protected:
    virtual int Run(void * argument); // 线程执行函数

private:
    CALL_BACK_FUNCTION call_back_func_;
};

#endif  // !#define (_IOS_THREAD_INCLUDE_H_)