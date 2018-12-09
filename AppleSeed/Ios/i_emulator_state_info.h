////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 The Sun.AC Authors . All Rights Reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _IEMULATOR_STATE_INFO_INCLUDE_H_
#define _IEMULATOR_STATE_INFO_INCLUDE_H_

namespace emulator {

    enum EMULATOR_STATE {
        STATE_ENGINE_OFF = 0,
        STATE_ENGINE_ON,
        STATE_UPDATING,
        STATE_INSTALLING,
    };

    class iEmulatorStateInfo
        : public base::SupportsWeakPtr<iEmulatorStateInfo>
        , public base::RefCountedThreadSafe<iEmulatorStateInfo> {
    public:
        virtual ~iEmulatorStateInfo() {}

        virtual const EMULATOR_STATE& state() = 0;
        virtual bool engine_off() = 0;
        virtual bool engine_on() = 0;
        virtual bool updating() = 0;
        virtual bool installing() = 0;
        virtual const UINT_PTR& app_of_installing() = 0;
    };
}


#endif  // !#define (_IEMULATOR_STATE_INFO_INCLUDE_H_)
