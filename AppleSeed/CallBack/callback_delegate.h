////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 The Sun.AC Authors . All Rights Reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DELEGATE_BASE_INCLUDE_H_
#define _DELEGATE_BASE_INCLUDE_H_

#include "cef_weak_ptr.h"
#include "ref_observer_list.h"

template <typename> class ObserverListRefBase;
template <typename> class CallbackBase;

template<typename T, typename ObserverType>
class CallBackDelegate
    : public base::SupportsWeakPtr<T> {
public:
    CallBackDelegate() {}
    virtual ~CallBackDelegate() { observers_.Clear(); }

public:
    typedef CallbackBase<ObserverType> RefObserverType;
    void AddObserver(scoped_refptr<RefObserverType> obs) {observers_.AddObserver(obs);}
    void RemoveObserver(scoped_refptr<RefObserverType> obs) {observers_.RemoveObserver(obs);}

    RefObserverList<ObserverType>* Observers() { return &observers_; };

    virtual bool Init() { return false; };
    virtual void Exit();

private:
    friend T;
    RefObserverList<ObserverType> observers_;
};

template <typename T, typename ObserverType>
void CallBackDelegate<T, ObserverType>::Exit() {
    observers_.Clear();
}

#endif  // !#define (_DELEGATE_BASE_INCLUDE_H_)
