////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 The Sun.AC Authors . All Rights Reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _PAGELAYOUT_INCLUDE_H_
#define _PAGELAYOUT_INCLUDE_H_

class CPageLayoutUI
    : public CContainerUI {
public:
    CPageLayoutUI();
    virtual ~CPageLayoutUI();

    static LPCTSTR GetClassName();
    virtual void DoEvent(TEventUI& event) override;
    virtual void Init() override;

    virtual LPCTSTR GetClass() const override;
    virtual void SetPos(RECT rc) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    
    void    SetColumn(int column) { column_ = column; };
    int     GetColumn() { return column_; };

    void    SetRow(int row) { row_= row; };
    int     GetRow() { return row_; };

    void    SetDisHor(int hor) { dis_hor_ = hor; };
    int     GetDisHor() { return dis_hor_; };

    void    SetDisVer(int ver) { dis_ver_ = ver; };
    int     GetDisVer() { return dis_ver_; };

    void    SetItemWidth(int width) { item_width_ = width; };
    int     GetItemWidth() { return item_width_; };

    void    SetItemHeight(int height) { item_height_ = height; };
    int     GetItemHeight() { return item_height_; };

    void    SetAlignEdge(bool align_edge = false);

    void    SetAutoCalcHeight(bool auto_calc_height = false);
public:
    bool    HasGameData();
    
private:
    void    InitVScrollBar();

private:
    int column_;
    int row_;
    int dis_hor_;
    int dis_ver_;
    int item_width_;
    int item_height_;
    bool align_edge_;
    bool auto_calc_height_;
};

#endif  // !#define (_PAGELAYOUT_INCLUDE_H_)  
