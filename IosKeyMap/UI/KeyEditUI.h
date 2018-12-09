#ifndef __UIKEYEDIT_H__
#define __UIKEYEDIT_H__

#pragma once

#define  UI_KEY_EDIT   L"KeyEdit"

namespace DuiLib {
    class CKeyEditWnd;
    class CKeyEditUI;
    class CKeyEditRbtnupCallback
    {
    public:
        virtual LRESULT OnEditRbtnup(CEditUI* pEditCtl, BOOL& bHandled) = 0;
    };

    class CKeyEditUI : public CLabelUI
    {
        friend class CKeyEditWnd;
    public:
        CKeyEditUI();
        virtual ~CKeyEditUI();// 析构函数

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);
        UINT GetControlFlags() const;

        void SetEnabled(bool bEnable = true);
        void SetText(LPCTSTR pstrText);
        void SetMaxChar(UINT uMax);
        UINT GetMaxChar();
        void SetReadOnly(bool bReadOnly);
        bool IsReadOnly() const;
        void SetPasswordMode(bool bPasswordMode);
        bool IsPasswordMode() const;
        void SetPasswordChar(TCHAR cPasswordChar);
        TCHAR GetPasswordChar() const;
        void SetNumberOnly(bool bNumberOnly);
        bool IsNumberOnly() const;
        int GetWindowStyls() const;

        LPCTSTR GetNormalImage();
        void SetNormalImage(LPCTSTR pStrImage);
        LPCTSTR GetHotImage();
        void SetHotImage(LPCTSTR pStrImage);
        LPCTSTR GetFocusedImage();
        void SetFocusedImage(LPCTSTR pStrImage);
        LPCTSTR GetDisabledImage();
        void SetDisabledImage(LPCTSTR pStrImage);
        void SetNativeEditBkColor(DWORD dwBkColor);
        DWORD GetNativeEditBkColor() const;

        void SetSel(long nStartChar, long nEndChar);
        void SetSelAll();
        void SetReplaceSel(LPCTSTR lpszReplace);

        void SetPos(RECT rc);
        RECT CalEditPos();
        void SetVisible(bool bVisible = true);
        void SetInternVisible(bool bVisible = true);
        SIZE EstimateSize(SIZE szAvailable);
        void DoEvent(TEventUI& event);
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

        void PaintStatusImage(HDC hDC);
        void PaintText(HDC hDC);

        void	SetDefText(LPCTSTR pstrValue);
        void	SetDefTextColor(DWORD dwColor);

        CDuiImage*	GetEditBkImage() { return m_pEditBkImage; }
        void	SetRbtnupMenu(const bool bVal) { m_bRbtnupMenu = bVal; }
        bool	IsRbtnupMenu()const { return m_bRbtnupMenu; }
        //创建一个空位图
        BOOL	CreateEmptyImage(CDuiImage* pImg, int nWidth, int nHeight, int nBitsCount = 32);
        //拷贝一个位图
        BOOL	CopyImage(CDuiImage* pdestImage, CDuiImage* psrcImage, int nBitsCount = 32, int nx = 0, int ny = 0, int nWidth = 0, int nHeight = 0);
        void	ReleaseEditImage();//释放背景位图对象
        HWND	GetEditWnd();

    protected:
        CKeyEditWnd*	m_pWindow;
        CDuiImage*	m_pEditBkImage;//用于edit窗口 背景
        DWORD		m_dwCaretColor;//edit光标颜色
        int			m_nCaretWidth;//光标宽度
        UINT m_uMaxChar;
        bool m_bReadOnly;
        bool m_bPasswordMode;
        TCHAR m_cPasswordChar;
        UINT m_uButtonState;
        CDuiString m_sNormalImage;
        CDuiString m_sHotImage;
        CDuiString m_sFocusedImage;
        CDuiString m_sDisabledImage;
        DWORD	m_dwEditbkColor;
        int		m_iWindowStyls;
        bool	m_bRbtnupMenu;
        bool	m_bShowDefMenu;
        CDuiString m_szDefText;
        DWORD	m_dwDefTextColor;
    };

    class CKeyEditWnd : public CWndBase, public IMessageFilterUI
    {
    public:
        CKeyEditWnd();
        virtual ~CKeyEditWnd();//增加一个析构函数 安全释放对象

        HWND Init(HWND hParentWnd, CKeyEditUI* pOwner);
        RECT CalPos();

        virtual LPCWSTR GetWndName() const override { return L"KeyEditWnd"; }
        virtual LPCWSTR GetXmlPath() const override;

        LPCTSTR GetWindowClassName() const;
        LPCTSTR GetSuperClassName() const;
        void OnFinalMessage(HWND hWnd);
        virtual void InitWindow();

        virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;

        virtual CControlUI* CreateControl(LPCTSTR pstrClass);
        virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
        virtual LRESULT OnSetCursor(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    protected:
        CKeyEditUI* m_pOwner;
        HBRUSH	m_hBkBrush;
        bool	m_bInit;
        HWND	m_hParentWnd;

    public:
        CDuiImage* m_pMemBkImage;//绘制背景内存位图对象//
    };
}
#endif // __UIKEYEDIT_H__

