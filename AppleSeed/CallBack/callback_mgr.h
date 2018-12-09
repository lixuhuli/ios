#ifndef __CALL_BACK_MGR_H__
#define __CALL_BACK_MGR_H__

#pragma once
#include "cef_weak_ptr.h"
#include "callback_delegate.h"
#include "appleseed_callback.h"

class CCallBackMgr 
 : public CallBackDelegate<CCallBackMgr, AppleSeedCallback>{
public:
    static CCallBackMgr* Instance();

public:
    CCallBackMgr();
	virtual ~CCallBackMgr();

    virtual bool Init() override;
    virtual void Exit() override;

    void GoTo(void(AppleSeedCallback::*func)(void)) {
        observers_.Go(func);
    }

private:

};

#endif // __CALL_BACK_MGR_H__