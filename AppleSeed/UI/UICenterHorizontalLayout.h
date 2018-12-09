#ifndef __UICENTER_HORIZONTAL_LAYOUT_H__
#define __UICENTER_HORIZONTAL_LAYOUT_H__

#pragma once

#define DUI_CTR_CENTER_HORIZONTALLAYOUT   (_T("CenterHorizontalLayout"))

class CCenterHorizontalLayoutUI :
    public CHorizontalLayoutUI {
public:
    CCenterHorizontalLayoutUI();
    virtual ~CCenterHorizontalLayoutUI();

public:
    virtual void Init() override;
    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    virtual void SetPos(RECT rc) override;

private:
    CControlUI* left_contrl_;
    CControlUI* right_contrl_;
    CControlUI* need_center_contrl_;
};

#endif // __UICENTER_HORIZONTAL_LAYOUT_H__