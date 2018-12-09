#include "StdAfx.h"
#include "PageLayout.h"

CPageLayoutUI::CPageLayoutUI()
    : column_(10)
    , dis_hor_(2)
    , dis_ver_(2)
    , row_(4)
    , item_height_(0)
    , item_width_(0)
    , align_edge_(false)
    , auto_calc_height_(false) {
}

CPageLayoutUI::~CPageLayoutUI() {
}

LPCTSTR CPageLayoutUI::GetClassName() {
    return L"PageLayout";
}

LPCTSTR CPageLayoutUI::GetClass() const {
    return L"PageLayoutUI";
}

void CPageLayoutUI::DoEvent(TEventUI& event) {
    CContainerUI::DoEvent(event);
}

void CPageLayoutUI::Init() {
    CContainerUI::Init();
    InitVScrollBar();
}

void CPageLayoutUI::InitVScrollBar() {
    if (!m_pVerticalScrollBar) return;
    m_pVerticalScrollBar->SetFixedWidth(0);
}

void CPageLayoutUI::SetPos(RECT rc) {
    CControlUI::SetPos(rc);
    rc = m_rcItem;

    // Adjust for inset
    rc.left += m_rcInset.left;
    rc.top += m_rcInset.top;
    rc.right -= m_rcInset.right;
    rc.bottom -= m_rcInset.bottom;

    if (m_items.GetSize() <= 0) {
        ProcessScrollBar(rc, 0, 0);
        return;
    }

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

    if ((row_ <= 0 && item_height_ <= 0) || (column_ <= 0 && item_width_ <= 0)) return;

    int h_range = 0;
    int real_row = 0;

    auto dis_hor_temp = dis_hor_;
    if (item_width_ <= 0) item_width_ = ((rc.right - rc.left) - (column_ - 1) * dis_hor_) / column_;
    else {
        column_ = (rc.right - rc.left + dis_hor_) / (item_width_ + dis_hor_);
        if (align_edge_ && column_ > 1) {
            dis_hor_temp = (rc.right - rc.left - item_width_ * column_) / (column_ - 1);
        }

        if (column_ <= 0) return;
    }

    if (item_height_ <= 0) item_height_ = ((rc.bottom - rc.top) - (row_ - 1) * dis_ver_) / row_;
    else row_ = (rc.bottom - rc.top + dis_ver_) / (item_height_ + dis_ver_);

    int cyNeeded = 0;
    int top_index = 0;
    int left_index = 0;

    int not_show_count = 0;
    int scroll_pos = m_pVerticalScrollBar ? m_pVerticalScrollBar->GetScrollPos() : 0;
    for (int it = 0; it < m_items.GetSize(); it++) {
        CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
        if (!pControl->IsVisible()) {
            not_show_count++;
            continue;
        }

        top_index = (it - not_show_count) / column_;
        left_index = it - not_show_count - top_index * column_;

        RECT rc_item;
        rc_item.left = rc.left + left_index * dis_hor_temp + left_index * item_width_;
        rc_item.top = rc.top + top_index * dis_ver_ + top_index * item_height_ - scroll_pos;
        rc_item.right = rc_item.left + item_width_;
        rc_item.bottom = rc_item.top + item_height_;
        pControl->SetPos(rc_item);
    }

    if (top_index + 1 > row_) cyNeeded = (top_index + 1) * (dis_ver_ + item_height_);

    if (auto_calc_height_) {
        auto need_height = (top_index + 1) * (dis_ver_ + item_height_);
        if (need_height != m_cxyFixed.cy) SetFixedHeight(need_height);
    }

    ProcessScrollBar(rc, 0, cyNeeded);
}

void CPageLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
    if (_tcscmp(pstrName, _T("column")) == 0) SetColumn(_ttoi(pstrValue));
    else if (_tcscmp(pstrName, _T("row")) == 0) SetRow(_ttoi(pstrValue));
    else if (_tcscmp(pstrName, _T("dis_hor")) == 0) SetDisHor(_ttoi(pstrValue));
    else if (_tcscmp(pstrName, _T("dis_ver")) == 0) SetDisVer(_ttoi(pstrValue));
    else if (_tcscmp(pstrName, _T("item_width")) == 0) SetItemWidth(_ttoi(pstrValue));
    else if (_tcscmp(pstrName, _T("item_height")) == 0) SetItemHeight(_ttoi(pstrValue));
    else if (_tcscmp(pstrName, _T("align_edge")) == 0) SetAlignEdge(_tcscmp(pstrValue, _T("true")) == 0);
    else if (_tcscmp(pstrName, _T("autocalcheight")) == 0) SetAutoCalcHeight(_tcscmp(pstrValue, _T("true")) == 0);
    else CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CPageLayoutUI::SetAlignEdge(bool align_edge /*= false*/) {
    align_edge_ = align_edge;
}

void CPageLayoutUI::SetAutoCalcHeight(bool auto_calc_height /*= false*/) {
    auto_calc_height_ = auto_calc_height;
}

bool CPageLayoutUI::HasGameData() {
    return m_items.GetSize() > 0;
}
