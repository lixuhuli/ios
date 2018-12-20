////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 The Sun.AC Authors . All Rights Reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_INFO_INCLUDE_H_
#define _SCENE_INFO_INCLUDE_H_
#include "i_scene_info.h"
#include "utils/SerialRapidxmlDoc.hpp"
#include <memory>

#define FUN_VERSION				"xml version='1.0' encoding='UTF-8'"
#define FUN_ROOT				"Root"
#define FUN_SCENCE				"Scene"
#define FUN_ROTATE_ANGLE		"rotateAngle"
#define FUN_GAME_ID				"id"
#define FUN_OPACITY				"opacity"
#define FUN_SCENE_HEIGHT		"height"
#define FUN_SCENE_WIDTH			"width"
#define FUN_HORIZONTAL_FLAG		"horizonFlag"
#define FUN_ITEM				"Item"
#define FUN_ITEM_TYPE			"type"
#define FUN_ITEM_POS			"pos"
#define FUN_ITEM_HEIGHT			"height"
#define FUN_ITEM_WIDTH			"width"
#define FUN_ITEM_SLIDER			"slider"
#define FUN_ITEM_MACRO			"macro"
#define FUN_ITEM_R_M_SROP		"right_move_stop"
#define FUN_ITEM_FINGER_COUNT	"finger_count"
#define FUN_KEY					"Key"
#define FUN_KEY_DESCRIPTION		"description"
#define FUN_KEY_VALUE			"value"
#define FUN_KEY_STRING			"keyString"
#define FUN_POINT				"Point"

namespace emulator {
    typedef std::vector<ItemInfo> LIST_ITEM;
    typedef std::vector<KeyInfo> LIST_KEY;

    class SceneInfo
        : public iSceneInfo {
    public:
        explicit SceneInfo(): m_nRotateAngle(0), m_strID(""), m_nOpacity(50), 
            m_nSceneHeight(720), m_nSceneWidth(1280), m_nHorizonFlag(1) {
        }

        virtual ~SceneInfo() {}

        SceneInfo(SceneInfo& info) {
            m_nRotateAngle = info.m_nRotateAngle;
            m_strID = info.m_strID;
            m_nOpacity = info.m_nOpacity;
            m_nSceneHeight = info.m_nSceneHeight;
            m_nSceneWidth = info.m_nSceneWidth;
            m_nHorizonFlag = info.m_nHorizonFlag;
            m_items = info.m_items;
        }

    public:
        scoped_refptr<SceneInfo> cloner() {
            return internal_cloner();
        }

        scoped_refptr<SceneInfo> internal_cloner() {
            return new SceneInfo(*this);
        }

        virtual int screen_width() override { return m_nSceneWidth; }
        virtual int screen_height() override { return m_nSceneHeight; }

        virtual void set_opacity(int nOpacity) { m_nOpacity = nOpacity; }
        virtual int opacity() override { return m_nOpacity; }

        void loadScene(const char* keyXmlFile);
        void saveScene(const char* keyXmlFile);

        void AddItem(const ItemInfo& item) { m_items.push_back(item); }

        bool set_key(const ItemType& type, int src_value, int org_value, const std::string& key_string) {
            auto it  = GetKeyInfo(type, src_value);
            if (!it._Ptr) return false;

            it->nValue = org_value;
            it->strKeyString = key_string;
            return true;
        }

        bool set_key_pos(int src_value, int pox_x, int pox_y, const QPoint& point) {
            auto it = m_items.begin();
            for (; it != m_items.end(); it++) {
                auto itr = it->keys.begin();
                for (; itr != it->keys.end(); itr++) {
                    if (itr->nValue == src_value) {
                        it->nItemPosX = pox_x;
                        it->nItemPosY = pox_y;

                        itr->nPointX = point.x;
                        itr->nPointY = point.y;
                        return true;
                    }
                }
            }
            return false;
        }

        bool set_key_pos(int src_value, const QPoint& point) {
            auto it = m_items.begin();
            for (; it != m_items.end(); it++) {
                auto itr = it->keys.begin();
                for (; itr != it->keys.end(); itr++) {
                    if (itr->nValue == src_value) {
                        itr->nPointX = point.x;
                        itr->nPointY = point.y;

                        return true;
                    }
                }
            }
            return false;
        }

        bool set_right_mouse_value(int value) {
            auto it = m_items.begin();
            for (; it != m_items.end(); it++) {
                if (it->itemType == RIGHT_MOUSE_MOVE) {
                    it->nItemSlider = value;
                    return true;
                }
            }
            return false;
        }

        bool set_intelligent_value(int src_value, int value) {
            auto it  = GetItemInfo(INTELLIGENT_CASTING_KEY, src_value);
            if (it == m_items.end()) return false;
            it->nItemSlider = value;
            return false;
        }

        bool set_intelligent_switch_on(int src_value, bool open) {
            auto it  = GetItemInfo(INTELLIGENT_CASTING_KEY, src_value);
            if (it == m_items.end()) return false;

            it->nItemRightMoveStop = (open ? 0 : 1);
            return true;
        }

        virtual std::vector<ItemInfo> GetKeyItemGather(const ItemType& type) override {
            std::vector<ItemInfo> items;

            auto it = m_items.begin();
            for (; it != m_items.end(); it++) {
                if (it->itemType == type) {
                    items.push_back(*it);
                }
            }

            return items;
        }

        virtual const std::vector<ItemInfo>& GetKeyItems() override {
            return m_items;
        }

        bool DeleteRightMouse(std::string& strKeyString) {
            auto it = m_items.begin();
            for (; it != m_items.end(); it++) {
                if (it->itemType == RIGHT_MOUSE_MOVE) {
                    if (it->keys.size() >= 2) strKeyString = it->keys[1].strKeyString;
                    m_items.erase(it);
                    return true;
                }
            }

            return false;
        }

        bool DeleteKey(const ItemType& type, int key_value) {
            auto it = m_items.begin();
            for (; it != m_items.end(); it++) {
                if (it->itemType == type) {
                    auto itr = it->keys.begin();
                    for (; itr != it->keys.end(); itr++) {
                        if (itr->nValue == key_value) {
                            m_items.erase(it);
                            return true;
                        }
                    }
                }
            }

            return false;
        }

        bool DeleteKey(int key_value) {
            auto it = m_items.begin();
            for (; it != m_items.end(); it++) {
                auto itr = it->keys.begin();
                for (; itr != it->keys.end(); itr++) {
                    if (itr->nValue == key_value) {
                        m_items.erase(it);
                        return true;
                    }
                }
            }

            return false;
        }

    private:
        std::vector<KeyInfo>::iterator GetKeyInfo(const ItemType& type, int key_value) {
            auto it = m_items.begin();
            for (; it != m_items.end(); it++) {
                if (it->itemType == type) {
                    auto itr = it->keys.begin();
                    for (; itr != it->keys.end(); itr++) {
                        if (itr->nValue == key_value) {
                            return itr;
                        }
                    }
                }
            }
            return LIST_KEY::iterator();
        }

        std::vector<ItemInfo>::iterator GetItemInfo(const ItemType& type, int key_value) {
            auto it = m_items.begin();
            for (; it != m_items.end(); it++) {
                if (it->itemType == type) {
                    auto itr = it->keys.begin();
                    for (; itr != it->keys.end(); itr++) {
                        if (itr->nValue == key_value) {
                            return it;
                        }
                    }
                }
            }
            return it;
        }

        std::vector<ItemInfo>::iterator GetItemInfo(int key_value) {
            auto it = m_items.begin();
            for (; it != m_items.end(); it++) {
                auto itr = it->keys.begin();
                for (; itr != it->keys.end(); itr++) {
                    if (itr->nValue == key_value) {
                        return it;
                    }
                }
            }
            return it;
        }

    private:
        int m_nRotateAngle;
        std::string m_strID;
        int m_nOpacity;
        int m_nSceneHeight;
        int m_nSceneWidth;
        int m_nHorizonFlag;

        std::vector<ItemInfo> m_items;

    };

    void SceneInfo::loadScene(const char* keyXmlFile)
    {
        char* xml_content = nullptr;
        FILE* file = nullptr;
        errno_t err = fopen_s(&file, keyXmlFile, "rb");
        if (file == nullptr)
            return;

        fseek(file, 0, SEEK_END);
        size_t len = ftell(file);
        fseek(file, 0, SEEK_SET);
        xml_content = new char[len + 1];
        memset(xml_content, 0, len + 1);
        fread(xml_content, 1, len, file);
        fclose(file);

        if (xml_content == nullptr)
            return;

        rapidxml::xml_document<>    doc;
        rapidxml::xml_node<>*       node_root	= nullptr;
        rapidxml::xml_node<>*       node_scene	= nullptr;
        rapidxml::xml_node<>*		node_item	= nullptr;
        rapidxml::xml_node<>*		node_key	= nullptr;
        rapidxml::xml_node<>*		node_point	= nullptr;

        try
        {
            doc.parse<0>(xml_content);
            node_root = doc.first_node(FUN_ROOT);
            if (!node_root) throw node_root;

            node_scene = node_root->first_node();
            if (!node_scene) throw node_scene;
            rapidxml::xml_attribute<>*  attrRotateAngle = node_scene->first_attribute(FUN_ROTATE_ANGLE);
            rapidxml::xml_attribute<>*  attrId			= node_scene->first_attribute(FUN_GAME_ID);
            rapidxml::xml_attribute<>*  attrOpacity		= node_scene->first_attribute(FUN_OPACITY);
            rapidxml::xml_attribute<>*  attrSceneHeight	= node_scene->first_attribute(FUN_SCENE_HEIGHT);
            rapidxml::xml_attribute<>*  attrSceneWidth	= node_scene->first_attribute(FUN_SCENE_WIDTH);
            rapidxml::xml_attribute<>*  attrHoriFlag	= node_scene->first_attribute(FUN_HORIZONTAL_FLAG);

            if (attrRotateAngle)
                sscanf_s(attrRotateAngle->value(), "%d", &m_nRotateAngle);
            if (attrId)
                m_strID = attrId->value();
            if (attrOpacity)
                sscanf_s(attrOpacity->value(), "%d", &m_nOpacity);
            if (attrSceneHeight)
                sscanf_s(attrSceneHeight->value(), "%d", &m_nSceneHeight);
            if (attrSceneWidth)
                sscanf_s(attrSceneWidth->value(), "%d", &m_nSceneWidth);
            if (attrHoriFlag)
                sscanf_s(attrHoriFlag->value(), "%d", &m_nHorizonFlag);

            node_item = node_scene->first_node();
            if (!node_item) throw node_item;
            for (; node_item; node_item = node_item->next_sibling())
            {
                rapidxml::xml_attribute<>*  attrItemType	= node_item->first_attribute(FUN_ITEM_TYPE);
                rapidxml::xml_attribute<>*  attrItemPos		= node_item->first_attribute(FUN_ITEM_POS);
                rapidxml::xml_attribute<>*  attrItemHeight	= node_item->first_attribute(FUN_ITEM_HEIGHT);
                rapidxml::xml_attribute<>*  attrItemWidth	= node_item->first_attribute(FUN_ITEM_WIDTH);
                rapidxml::xml_attribute<>*  attrItemSlider	= node_item->first_attribute(FUN_ITEM_SLIDER);
                rapidxml::xml_attribute<>*  attrItemMacro	= node_item->first_attribute(FUN_ITEM_MACRO);
                rapidxml::xml_attribute<>*  attrItemFinCount= node_item->first_attribute(FUN_ITEM_FINGER_COUNT);
                rapidxml::xml_attribute<>*  attrItemRMStop	 = node_item->first_attribute(FUN_ITEM_R_M_SROP);

                ItemInfo item;
                if (attrItemType)
                    sscanf_s(attrItemType->value(), "%d", &item.itemType);
                if (attrItemPos)
                    sscanf_s(attrItemPos->value(), "%d,%d", &item.nItemPosX, &item.nItemPosY);
                if (attrItemHeight)
                    sscanf_s(attrItemHeight->value(), "%d", &item.nItemHeight);
                if (attrItemWidth)
                    sscanf_s(attrItemWidth->value(), "%d", &item.nItemWidth);
                if (attrItemSlider)
                    sscanf_s(attrItemSlider->value(), "%d", &item.nItemSlider);
                if (attrItemMacro)
                    item.strMacro = attrItemMacro->value();
                if (attrItemFinCount)
                    sscanf_s(attrItemFinCount->value(), "%d", &item.nItemFingerCount);
                if (attrItemRMStop)
                    sscanf_s(attrItemRMStop->value(), "%d", &item.nItemRightMoveStop);

                node_key = node_item->first_node();
                if (!node_key) throw node_key;
                for (; node_key; node_key = node_key->next_sibling())
                {
                    rapidxml::xml_attribute<>*  attrKeyDescription = node_key->first_attribute(FUN_KEY_DESCRIPTION);
                    rapidxml::xml_attribute<>*  attrKeyValue = node_key->first_attribute(FUN_KEY_VALUE);
                    rapidxml::xml_attribute<>*  attrKeyString = node_key->first_attribute(FUN_KEY_STRING);

                    node_point = node_key->first_node();
                    if (!node_point) throw node_point;
                    char* value = node_point->value();

                    KeyInfo key;
                    if (attrKeyDescription)
                        key.strDescription = attrKeyDescription->value();
                    if (attrKeyValue)
                        sscanf_s(attrKeyValue->value(), "%d", &key.nValue);
                    if (attrKeyString)
                        key.strKeyString = attrKeyString->value(); 

                    if (value)
                        sscanf_s(value, "%d,%d", &key.nPointX, &key.nPointY);

                    item.keys.push_back(key);
                }

                m_items.push_back(item);

            }

        }
        catch (...)
        {
        }

        delete[] xml_content;
    }


    void SceneInfo::saveScene(const char* keyXmlFile)
    {
        CSerialRapidxmlDoc serialDoc;

        rapidxml::xml_node<>*   version = serialDoc.doc().allocate_node(rapidxml::node_pi, FUN_VERSION, 0);
        serialDoc.doc().append_node(version);

        rapidxml::xml_node<>*   node_root = serialDoc.doc().allocate_node(rapidxml::node_element, FUN_ROOT, 0);
        serialDoc.doc().append_node(node_root);

        rapidxml::xml_node<>*	node_scene = serialDoc.doc().allocate_node(rapidxml::node_element, FUN_SCENCE);
        node_scene->append_attribute(serialDoc.doc().allocate_attribute(FUN_ROTATE_ANGLE, serialDoc.get(m_nRotateAngle)));
        node_scene->append_attribute(serialDoc.doc().allocate_attribute(FUN_GAME_ID, serialDoc.get(m_strID.c_str())));
        node_scene->append_attribute(serialDoc.doc().allocate_attribute(FUN_OPACITY, serialDoc.get(m_nOpacity)));
        node_scene->append_attribute(serialDoc.doc().allocate_attribute(FUN_SCENE_HEIGHT, serialDoc.get(m_nSceneHeight)));
        node_scene->append_attribute(serialDoc.doc().allocate_attribute(FUN_SCENE_WIDTH, serialDoc.get(m_nSceneWidth)));
        node_scene->append_attribute(serialDoc.doc().allocate_attribute(FUN_HORIZONTAL_FLAG, serialDoc.get(m_nHorizonFlag)));

        for (unsigned i = 0; i < m_items.size(); i++)
        {
            ItemInfo item = m_items.at(i);
            rapidxml::xml_node<>*	node_item = serialDoc.doc().allocate_node(rapidxml::node_element, FUN_ITEM);
            node_item->append_attribute(serialDoc.doc().allocate_attribute(FUN_ITEM_TYPE, serialDoc.get((int)item.itemType)));
            char szPos[128];
            sprintf_s(szPos, "%d,%d", item.nItemPosX, item.nItemPosY);
            node_item->append_attribute(serialDoc.doc().allocate_attribute(FUN_ITEM_POS, serialDoc.get(szPos)));
            node_item->append_attribute(serialDoc.doc().allocate_attribute(FUN_ITEM_HEIGHT, serialDoc.get(item.nItemHeight)));
            node_item->append_attribute(serialDoc.doc().allocate_attribute(FUN_ITEM_WIDTH, serialDoc.get(item.nItemWidth)));
            if (item.itemType == RIGHT_MOUSE_MOVE)
                node_item->append_attribute(serialDoc.doc().allocate_attribute(FUN_ITEM_SLIDER, serialDoc.get(item.nItemSlider)));
            if (item.itemType == INTELLIGENT_CASTING_KEY)
            {
                node_item->append_attribute(serialDoc.doc().allocate_attribute(FUN_ITEM_SLIDER, serialDoc.get(item.nItemSlider)));
                node_item->append_attribute(serialDoc.doc().allocate_attribute(FUN_ITEM_R_M_SROP, serialDoc.get(item.nItemRightMoveStop)));
            }
            if (item.itemType == NORMAL_KEY)
                node_item->append_attribute(serialDoc.doc().allocate_attribute(FUN_ITEM_FINGER_COUNT, serialDoc.get(item.nItemFingerCount)));
            if (item.itemType == PROGAMMING_KEY)
                node_item->append_attribute(serialDoc.doc().allocate_attribute(FUN_ITEM_MACRO, serialDoc.get(item.strMacro.c_str())));

            for (unsigned j = 0; j < item.keys.size(); j++)
            {
                KeyInfo key = item.keys.at(j);
                rapidxml::xml_node<>*	node_key = serialDoc.doc().allocate_node(rapidxml::node_element, FUN_KEY);
                node_key->append_attribute(serialDoc.doc().allocate_attribute(FUN_KEY_DESCRIPTION, serialDoc.get(key.strDescription.c_str())));
                node_key->append_attribute(serialDoc.doc().allocate_attribute(FUN_KEY_VALUE, serialDoc.get(key.nValue)));
                node_key->append_attribute(serialDoc.doc().allocate_attribute(FUN_KEY_STRING, serialDoc.get(key.strKeyString.c_str())));

                char szPointXY[128];
                sprintf_s(szPointXY, "%d,%d", key.nPointX, key.nPointY);
                rapidxml::xml_node<>*	node_point = serialDoc.doc().allocate_node(rapidxml::node_element, FUN_POINT, serialDoc.get(szPointXY));
                node_key->append_node(node_point);

                node_item->append_node(node_key);
            }
            node_scene->append_node(node_item);
        }
        node_root->append_node(node_scene);


        //std::map<string, KeyAttriInfo>::iterator item;
        //for (item = m_mapKeyInfo.begin(); item != m_mapKeyInfo.end(); item++)
        //{
        //	char strKeyboardValue[64];
        //	char strVirValue[64];
        //	sprintf_s(strKeyboardValue, "0x%x", item->second.nKeyboardValue);
        //	sprintf_s(strVirValue, "0x%x", item->second.nVirtualValue);


        //	//此处必须要申请堆内存，strKeyboardValue，strEmuValue出了for就释放了
        //	rapidxml::xml_node<>* node_key = serialDoc.doc().allocate_node(rapidxml::node_element, serialDoc.get(item->second.strKeyName));

        //	node_key->append_attribute(serialDoc.doc().allocate_attribute("id", serialDoc.get(strKeyboardValue)));
        //	node_key->append_attribute(serialDoc.doc().allocate_attribute("type", serialDoc.get(item->second.strGuid)));
        //	node_key->append_attribute(serialDoc.doc().allocate_attribute("repeat_key", serialDoc.get(item->second.nRepeatKey)));
        //	node_key->append_attribute(serialDoc.doc().allocate_attribute("vir_key", serialDoc.get(strVirValue)));

        //	node_root->append_node(node_key);
        //}

        FILE*   pFile = nullptr;
        errno_t err = fopen_s(&pFile, keyXmlFile, "wb");
        if (pFile == nullptr)
            return;
        std::string         xml_content;
        rapidxml::print(std::back_inserter(xml_content), serialDoc.doc());

        fwrite(xml_content.c_str(), xml_content.size(), 1, pFile);
        fclose(pFile);
    }
}

#endif  // !#define (_SCENE_INFO_INCLUDE_H_)
