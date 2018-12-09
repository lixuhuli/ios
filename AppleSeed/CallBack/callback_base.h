#include "CefBase\main_message_loop.h"

////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 The Sun.AC Authors . All Rights Reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _CALLBACK_BASE_INCLUDE_H_
#define _CALLBACK_BASE_INCLUDE_H_

#include "cef_scoped_ptr.h"
#include "CefBase/main_message_loop.h"

template <typename T>
class CallbackBase : public base::RefCountedThreadSafe<CallbackBase<T>> {
public:
    CallbackBase(const base::WeakPtr<T>& weak) : from_message_loop_(MainMessageLoop::Get()), weak_(weak) {}
    virtual ~CallbackBase() {}

    MainMessageLoop*  from_message_loop() { return from_message_loop_; }
    base::WeakPtr<T> GetWeakPtr() { return weak_; }

private:
    MainMessageLoop* from_message_loop_;
    base::WeakPtr<T> weak_;
};

namespace appleseed {
template <typename T>
class Callback {
public:
    explicit Callback(T* ptr) : weak_factory_(ptr) { ref_ = new CallbackBase<T>(weak_factory_.GetWeakPtr());}
    virtual ~Callback() {}

    scoped_refptr<CallbackBase<T>> BasePtr() { return ref_; }
    base::WeakPtr<T> GetWeakPtr() { return weak_factory_.GetWeakPtr(); }

private:
    scoped_refptr<CallbackBase<T>> ref_;
    base::WeakPtrFactory<T> weak_factory_;
};

}


#endif  // !#define (_CALLBACK_BASE_INCLUDE_H_)
