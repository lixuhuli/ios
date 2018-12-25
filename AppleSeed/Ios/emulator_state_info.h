////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 The Sun.AC Authors . All Rights Reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _EMULATOR_STATE_INFO_INCLUDE_H_
#define _EMULATOR_STATE_INFO_INCLUDE_H_
#include "i_emulator_state_info.h"

namespace emulator {
    class EmulatorStateInfo
        : public iEmulatorStateInfo {
    public:
        explicit EmulatorStateInfo()
         : state_(STATE_ENGINE_OFF)
         , app_of_installing_(0) {

        }
        virtual ~EmulatorStateInfo() {}

        EmulatorStateInfo(EmulatorStateInfo& info) {
            state_ = info.state_;
            app_of_installing_ = info.app_of_installing_;
        }

    public:
        scoped_refptr<EmulatorStateInfo> cloner() {
            return internal_cloner();
        }

        scoped_refptr<EmulatorStateInfo> internal_cloner() {
            return new EmulatorStateInfo(*this);
        }

        virtual const EMULATOR_STATE& state() override { return state_; }
        void set_state(const EMULATOR_STATE& state) { state_ = state; }

        void clear_emulatro_info() {
        }

        virtual bool engine_off() override { return state_ == STATE_ENGINE_OFF; };
        virtual bool engine_on() override { return state_ == STATE_ENGINE_ON; };
        virtual bool updating() override { return state_ == STATE_UPDATING; };
        virtual bool installing() override { return state_ == STATE_INSTALLING; };

        virtual const UINT_PTR& app_of_installing() override { return app_of_installing_; };
        void set_app_of_installing(const UINT_PTR& app_of_installing) { app_of_installing_ = app_of_installing; }

        virtual const std::string& running_app_id() override { return running_app_id_; };
        void set_running_app_id(const std::string& id) { running_app_id_ = id; }

    private:
        EMULATOR_STATE state_;
        UINT_PTR app_of_installing_;
        std::string running_app_id_;
    };
}

#endif  // !#define (_EMULATOR_STATE_INFO_INCLUDE_H_)
