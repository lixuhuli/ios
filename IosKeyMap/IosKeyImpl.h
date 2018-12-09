
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 The Sun.AC Authors . All Rights Reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef IOS_KEY_IMPL_INCLUDE_H_
#define IOS_KEY_IMPL_INCLUDE_H_

#pragma once
#include "IosKeySdk.h"

class CKeyWnd;
namespace ioskey {
    class IosKeyImpl 
        : public iIosKey {
    public:
        IosKeyImpl();
        virtual ~IosKeyImpl();

        virtual bool CreateWnd(const HWND& parent, const RECT& pos);
        virtual void SetPos(const RECT& pos);
        virtual void Show(const bool& show);
        virtual bool IsVisible();

    private:
        CKeyWnd* key_wnd_;
    };

}
#endif  // !#define (IOS_KEY_IMPL_INCLUDE_H_)
