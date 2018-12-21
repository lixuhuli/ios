#ifndef _IKEY_INCLUDE_H_
#define _IKEY_INCLUDE_H_

#pragma once
#include "Ios/i_scene_info.h"

#define DUI_MSGTYPE_POS_CHANGED    (_T("poschanged"))

class Ikey {
public:
    Ikey() : screen_pos_x_(0), screen_pos_y_(0), key_type_(emulator::UNKNOWN_KEY), has_map_memory_(false) {};
    virtual ~Ikey() {};

public:
    virtual void UpdateBrowserMode(bool browser_mode, int opacity = 100) = 0;

    virtual void SetScreenPosX(int pos) { screen_pos_x_ = pos; }
    virtual void SetScreenPosY(int pos) { screen_pos_y_ = pos; }

    virtual int ScreenPosX() { return screen_pos_x_; }
    virtual int ScreenPosY() { return screen_pos_y_; }

    virtual void SetKeyType(const emulator::ItemType& type) { key_type_ = type; }
    virtual const emulator::ItemType& KeyType() { return key_type_; }

    virtual void SetHasMapMemory(const bool& value) { has_map_memory_ = value; }
    virtual const bool& HasMapMemory() { return has_map_memory_; }

    virtual int GetTransparent(int opacity) { return int(((double)opacity) * 255.0 / 100.0); }

private:
    int screen_pos_x_;
    int screen_pos_y_;
    bool has_map_memory_;

    emulator::ItemType key_type_;
};

#endif  // !#define (_IKEY_INCLUDE_H_)