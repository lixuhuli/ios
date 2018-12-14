////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 The Sun.AC Authors . All Rights Reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _ISCENE_INFO_INCLUDE_H_
#define _ISCENE_INFO_INCLUDE_H_

namespace emulator {
    enum ItemType {
        HANDLE_KEY = 0,
        NORMAL_KEY = 1,
        INTELLIGENT_CASTING_KEY = 8,
        PROGAMMING_KEY = 9,
        RIGHT_MOUSE_MOVE = 12,
    };

#ifndef _KEY_INFO_
#define _KEY_INFO_
    typedef struct tagKeyInfo {
        std::string strDescription;
        std::string strKeyString;
        int nValue;
        int nPointX;
        int nPointY;
    }KeyInfo;
#endif // !_KEY_INFO_

#ifndef _ITEM_INFO_
#define _ITEM_INFO_
    typedef struct tagItemInfo {
        ItemType itemType;
        int nItemPosX;
        int nItemPosY;
        int nItemHeight;
        int nItemWidth;
        int nItemSlider;
        int nItemFingerCount;
        int nItemRightMoveStop;
        std::string strMacro;
        std::vector<KeyInfo> keys;
    }ItemInfo;
#endif // !_ITEM_INFO_

    class iSceneInfo
        : public base::SupportsWeakPtr<iSceneInfo>
        , public base::RefCountedThreadSafe<iSceneInfo> {
    public:
        virtual ~iSceneInfo() {}

        virtual int screen_width() = 0;
        virtual int screen_height() = 0;

        virtual std::vector<ItemInfo> GetKeyItemGather(const ItemType& type) = 0;
        virtual const std::vector<ItemInfo>& GetKeyItems() = 0;
    };
}


#endif  // !#define (_ISCENE_INFO_INCLUDE_H_)
