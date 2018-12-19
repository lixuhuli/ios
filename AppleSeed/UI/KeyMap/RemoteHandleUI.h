#pragma once
#include "IKey.h"

#define  UI_REMOTE_HANDLE   L"RemoteHandle"

class CRemoteHandleUI 
    : public Ikey
    , public CVerticalLayoutUI {
public:
    CRemoteHandleUI();
    virtual ~CRemoteHandleUI();

public:
    virtual void Init() override;
    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    virtual void DoEvent(TEventUI& event) override;
    virtual void SetPos(RECT rc) override;

    UINT GetControlFlags() const;
    void SetSepWidth(int iWidth);
    int GetSepWidth() const;

    void UpdateBrowserMode(bool browser_mode, int opacity = 100) override;

public:
    virtual void PaintStatusImage(HDC hDC) override;

protected:
    void DrawBorderRect(HDC hDC);

protected:
    void    OnLButtonDown(UINT nFlags, QPoint point);
    void	OnLButtonDown_Edit(UINT nFlags, QPoint point);
    void	OnLButtonDown_Browse(UINT nFlags, QPoint point);

    void    OnMouseMove(UINT nFlags, QPoint point);
    void	OnMouseMove_Browse(UINT nFlags, QPoint point);
    void	OnMouseMove_Edit(UINT nFlags, QPoint point);

    bool    OnClickBtnClose(void* param);

    BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK_PAGE(L"btn_hand_close", this, &CRemoteHandleUI::OnClickBtnClose)
    END_BIND_CTRL()

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE_PAGE(btn_hand_circle_, CButtonUI, this, L"btn_hand_circle")
        DECLARE_CTRL_TYPE_PAGE(edit_key_1_, CEditUI, this, L"edit_key_1")
        DECLARE_CTRL_TYPE_PAGE(edit_key_2_, CEditUI, this, L"edit_key_2")
        DECLARE_CTRL_TYPE_PAGE(edit_key_3_, CEditUI, this, L"edit_key_3")
        DECLARE_CTRL_TYPE_PAGE(edit_key_4_, CEditUI, this, L"edit_key_4")
        DECLARE_CTRL_TYPE_PAGE(btn_hand_close_, CButtonUI, this, L"btn_hand_close")
    END_INIT_CTRL()

private:
    int GetRegionStatus(POINT ptMouse); // 0-left  1-top  2-right  3-bottom  4-left-top  5-right-top  6-right-bottom  7-left-bottom  8-center 

private:
    int m_iSepWidth;
    UINT m_uButtonState;
    POINT ptLastMouse;
    RECT m_rcNewPos;
    int direct_;
    bool drag_;

    CButtonUI* btn_hand_circle_;
    CEditUI* edit_key_1_;
    CEditUI* edit_key_2_;
    CEditUI* edit_key_3_;
    CEditUI* edit_key_4_;
    CButtonUI* btn_hand_close_;

};

