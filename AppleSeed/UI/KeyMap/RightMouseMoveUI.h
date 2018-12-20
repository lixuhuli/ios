#pragma once
#include "KeyEditUI.h"
#include "IKey.h"

#define  UI_RIGHT_MOUSEMOVE   L"RightMouseMove"

class CRightMouseMoveUI 
    : public Ikey
    , public CHorizontalLayoutUI {
public:
    CRightMouseMoveUI();
    virtual ~CRightMouseMoveUI();

public:
    virtual void Init() override;
    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    virtual void DoEvent(TEventUI& event) override;
    virtual void SetPos(RECT rc) override;

    UINT GetControlFlags() const;

    void UpdateBrowserMode(bool browser_mode, int opacity = 100) override;

public:
    virtual CKeyEditUI* edit_key() { return edit_key_; };
    virtual CSliderUI* slider_mouse() { return slider_mouse_; };

protected:
    void    OnLButtonDown(UINT nFlags, QPoint point);
    void    OnMouseMove(UINT nFlags, QPoint point);

    BEGIN_BIND_CTRL()
    END_BIND_CTRL()

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE_PAGE(edit_key_, CKeyEditUI, this, L"edit_key")
        DECLARE_CTRL_TYPE_PAGE(slider_mouse_, CSliderUI, this, L"slider_mouse")
        DECLARE_CTRL_TYPE_PAGE(mouse_copy_, CControlUI, this, L"mouse_copy")
    END_INIT_CTRL()

private:

private:
    UINT m_uButtonState;
    POINT ptLastMouse;
    RECT m_rcNewPos;
    CSliderUI* slider_mouse_;
    CKeyEditUI* edit_key_;
    CControlUI* mouse_copy_;

};

