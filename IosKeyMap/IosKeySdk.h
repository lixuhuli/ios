
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 The Sun.AC Authors . All Rights Reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef IOS_KEY_INCLUDE_H_
#define IOS_KEY_INCLUDE_H_

#ifdef IOSKEYMAP_EXPORTS
#define IOSKEY_MAP_API extern "C" __declspec(dllexport)
#else
#define IOSKEY_MAP_API extern "C" __declspec(dllimport)
#endif

namespace ioskey {
    class iIosKey {
    public:
        virtual ~iIosKey() {};

        virtual bool CreateWnd(const HWND& parent, const RECT& pos) = 0;
        virtual void SetPos(const RECT& pos) = 0;
        virtual void Show(const bool& show) = 0;
        virtual bool IsVisible() = 0;
    };
}

IOSKEY_MAP_API ioskey::iIosKey* ios_create_key();

IOSKEY_MAP_API void ios_destroy_key();

#endif  // !#define (IOS_KEY_INCLUDE_H_)