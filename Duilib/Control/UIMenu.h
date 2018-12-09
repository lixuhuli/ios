#ifndef __DUI_MENU_H__
#define __DUI_MENU_H__
#include <comdef.h>
#include "observer_impl_base.h"

namespace DuiLib 
{
#define NOTIFY_ITEM_CLICK       _T("itemclick")
#define NOTIFY_ITEM_ACTIVATE    _T("itemactivate")
#define NOTIFY_KILLFOCUS        _T("killfocus")
/////////////////////////////////////////////////////////////////////////////////////
//
struct ContextMenuParam
{
    // 1: remove all
    // 2: remove the sub menu
    enum 
    {
        emRemoveAll     = 1,
        emRemoveSubMenu = 2,
        emRemoveSelf    = 3,
    };
	
	WPARAM wParam;
	HWND hWnd;
};

enum MenuAlignment
{
	eMenuAlignment_Left = 1 << 1,
	eMenuAlignment_Top = 1 << 2,
	eMenuAlignment_Right = 1 << 3,
	eMenuAlignment_Bottom = 1 << 4,
};

typedef class ObserverImpl<BOOL, ContextMenuParam> ContextMenuObserver;
typedef class ReceiverImpl<BOOL, ContextMenuParam> ContextMenuReceiver;
extern ContextMenuObserver s_context_menu_observer;

class UILIB_API CMenuUI : public CListUI
{
public:
	CMenuUI();
	~CMenuUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface( LPCTSTR pstrName );

	virtual void DoEvent( TEventUI& event );

    virtual bool Add( CControlUI* pControl );
    virtual bool AddAt( CControlUI* pControl, int iIndex );
    virtual bool Remove( CControlUI* pControl );
	virtual int GetItemIndex( CControlUI* pControl ) const;
	virtual bool SetItemIndex( CControlUI* pControl, int iIndex );

	SIZE EstimateSize( SIZE szAvailable );
	void SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue );
};

/////////////////////////////////////////////////////////////////////////////////////
//

class CMenuElementUI;
class UILIB_API CMenuWnd 
    : public CWindowWnd
    , public ContextMenuReceiver
    , public INotifyUI
    , public IDialogBuilderCallback
{
public:
	CMenuWnd( STRINGorID xml, LPCTSTR pSkinType, CPaintManagerUI* pParentManager );
	CMenuWnd( CMenuElementUI* pOwner, CPaintManagerUI* pParentManager );
	virtual ~CMenuWnd();

    LPCTSTR GetWindowClassName() const;
    void OnFinalMessage( HWND hWnd );
    
    virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	virtual LRESULT OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	virtual LRESULT OnClose( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	virtual LRESULT OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Notify( TNotifyUI& msg );

	BOOL Receive( ContextMenuParam param );

	void TrackPopupMenu( DWORD dwAlignment, POINT point, HWND hParentWnd );

	void SetItemTextPadding( RECT rc );
	bool AddMenuItem( CControlUI* pControl );
	void DeleteMenuItem( LPCTSTR pstrName );
	void CheckMenuItem( LPCTSTR pstrName, bool bCheck );
	void EnableMenuItem( LPCTSTR pstrName, bool bEnable );
	void ModifyMenu( LPCTSTR pstrName, LPCTSTR pstrText );
	CMenuElementUI*  GetElementUIByID(DWORD dwID);
	CMenuUI* GetMenuRoot();
	void SetMaxHeight(int nHeight );
	int GetMaxHeight();
	void SetMinWidth( int nWidth );
	int GetMinWidth();
    void SetDivisionLineImage( LPCTSTR pstrImage );
    void AddDivisionLine( LPCTSTR pstrImage = NULL );     // 添加分隔线

private:
	void CreateWndByOwner();
	void CreateWndByXml();

public:
	HWND m_hParentWnd;    // 菜单消息通知窗口句柄
	CPaintManagerUI* m_pParentManager;
	CPaintManagerUI m_pm;
	POINT m_BasedPoint;
	STRINGorID m_xml;
	CStdString m_sType;
    CMenuElementUI* m_pOwner;
	CMenuUI* m_pLayout;
	CMenuUI* m_pRoot;
	DWORD m_dwAlignMent;      // 菜单的对齐方式	
	int m_nHeight;
	int m_nMinWidth;
	bool m_bDestroyWindow;

    CStdString m_strDivisionImage;   // 菜单项之前分隔线
};


class UILIB_API CMenuElementUI : public CListContainerElementUI
{
	friend CMenuWnd;
public:
    CMenuElementUI();
	~CMenuElementUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface( LPCTSTR pstrName );

    virtual void SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue );
     
    void DoPaint( HDC hDC, const RECT& rcPaint );

	void DrawItemText( HDC hDC, const RECT& rcItem );
	SIZE EstimateSize( SIZE szAvailable );
    void DrawParentArrow( HDC hDc );

	bool Activate();
	void DoEvent( TEventUI& event );

	CMenuWnd* GetMenuWnd();
	virtual void CreateMenuWnd();

	void CheckMenu( bool bCheck );
	void SetCheckPos( RECT rc );
	bool SetSelect(bool bSelect);

    static void SetDefaultArrowNorImage( LPCTSTR pstrImage );
    static void SetDefaultArrowHotImage( LPCTSTR pstrImage );

    static void SetDefaultCheckNorImage( LPCTSTR pstrImage );
    static void SetDefaultCheckHotImage( LPCTSTR pstrImage );

    void SetNormalIconImage( LPCTSTR pstrImage );
    void SetHotIconImage( LPCTSTR pstrImage );
	void SetDisableIconImage( LPCTSTR pstrImage );
    void SetSelectedNormalIconImage( LPCTSTR pstrImage );
    void SetSelectedHotIconImage( LPCTSTR pstrImage );
    void DrawMenuIcon( HDC hDc );
    void DrawMenuLine( HDC hDc );
    bool IsChecked();                           // 菜单项是否为Checked状态
    void SetChecked( bool bChecked );           
    void AddDivisionLine( LPCTSTR pstrImage = NULL );    // 添加菜单分隔线
	DWORD GetID(){ return m_dwID; }
	void  SetID(DWORD dwID){ m_dwID = dwID; }
	CMenuElementUI*  GetSubElementByID(DWORD dwID);
	void SetChildTextPadding(RECT rc);
	void SetDrawTextPadding(RECT rc);
    bool RemoveAt( int iIndex );

    void SetMouseHotSelected(bool bMouseHotSelected = true) { m_bMouseHotSelected = bMouseHotSelected; };
    bool GetMouseHotSelected() { return m_bMouseHotSelected; }
protected:
	CMenuWnd* m_pWindow;
	CControlUI* m_pCheck;

    static CStdString m_arrowNorImage;  // 存在子菜单项时的右边箭头
    static CStdString m_arrowHotImage;  // 存在子菜单项时的右边箭头Hot状态
    static CStdString m_ckeckNorImage;  // Checked
    static CStdString m_ckeckHotImage;  // Checked Hot状态
    CStdString m_strNorIcon;            // Icon图片
    CStdString m_strHotIcon;            // Hover状态下的Icon图片 
	CStdString m_strDisableIcon;		// Disable状态下的Icon图片
    CStdString m_strCheckedNorIcon;    // Selected后普通状态的图片
    CStdString m_strCheckedHotIcon;    // Selected后Hover状态下的图片
    bool m_bChecked;
    bool m_bMouseHotSelected;
	DWORD      m_dwID;
	RECT	   m_rcChildTextPadding;	//子菜单文本textpadding属性
	RECT	   m_rcDrawTextPadding;		//自己绘制时使用的textpadding
};

// 菜单项之间的分隔线
class UILIB_API CMenuDivisionUI : public CMenuElementUI
{
public:
    CMenuDivisionUI();
    virtual ~CMenuDivisionUI(){}
    LPVOID GetInterface( LPCTSTR pstrName );
    virtual LPCTSTR GetClass() const;
    virtual void DoEvent( TEventUI& event );
    virtual void DoPaint( HDC hDC, const RECT& rcPaint );
    void SetDivisionImage( LPCTSTR pStrImage );
    void SetDivisionColor( DWORD dwColor );
    static void SetDefaultDivisionImage( LPCTSTR pstrImage );
    static CStdString m_strDefaultDivImg;

private:    
};

} // namespace DirectUICore

#endif // __DUI_MENU_H__
