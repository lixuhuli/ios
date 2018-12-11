#pragma once
#include "KeyEditUI.h"

#define  UI_NORMAL   L"Normal"

namespace DuiLib {
    class CKeyEditUI;
}
class CNormalUI :
    public CHorizontalLayoutUI {
public:
    CNormalUI();
    virtual ~CNormalUI();

public:
    virtual void Init() override;
    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    virtual void DoEvent(TEventUI& event) override;
    virtual void SetPos(RECT rc) override;

    UINT GetControlFlags() const;

public:
    virtual CKeyEditUI* edit_key() { return edit_key_; };

protected:
    void    OnLButtonDown(UINT nFlags, QPoint point);
    void	OnLButtonDown_Edit(UINT nFlags, QPoint point);
    void	OnLButtonDown_Browse(UINT nFlags, QPoint point);

    void    OnMouseMove(UINT nFlags, QPoint point);
    void	OnMouseMove_Browse(UINT nFlags, QPoint point);
    void	OnMouseMove_Edit(UINT nFlags, QPoint point);

    bool    OnClickBtnClose(void* param);

    BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK_PAGE(L"btn_hand_close", this, &CNormalUI::OnClickBtnClose)
    END_BIND_CTRL()

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE_PAGE(edit_key_, CKeyEditUI, this, L"edit_key")
    END_INIT_CTRL()

private:

private:
    UINT m_uButtonState;
    POINT ptLastMouse;
    RECT m_rcNewPos;
    CButtonUI* btn_hand_close_;
    CKeyEditUI* edit_key_;
};

