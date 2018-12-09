////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 The Sun.AC Authors . All Rights Reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _APPLESEED_CALLBACK_INCLUDE_H_
#define _APPLESEED_CALLBACK_INCLUDE_H_
#include "callback_base.h"

class AppleSeedCallback 
    : public appleseed::Callback<AppleSeedCallback> {
public:
    AppleSeedCallback() : Callback(this) {}
    virtual ~AppleSeedCallback() {}

public:
    virtual void UpdateGameIcon(const __int64& nGameID, const wstring& strName, const wstring& strVersion) {}
    virtual void ApplicationRemoved(const __int64& nGameID) {}
};

#endif  // !#define (_APPLESEED_CALLBACK_INCLUDE_H_)
