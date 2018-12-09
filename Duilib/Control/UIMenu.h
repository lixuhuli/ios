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
    void AddDivisionLine( LPCTSTR pstrImage = NULL );     // ��ӷָ���

private:
	void CreateWndByOwner();
	void CreateWndByXml();

public:
	HWND m_hParentWnd;    // �˵���Ϣ֪ͨ���ھ��
	CPaintManagerUI* m_pParentManager;
	CPaintManagerUI m_pm;
	POINT m_BasedPoint;
	STRINGorID m_xml;
	CStdString m_sType;
    CMenuElementUI* m_pOwner;
	CMenuUI* m_pLayout;
	CMenuUI* m_pRoot;
	DWORD m_dwAlignMent;      // �˵��Ķ��뷽ʽ	
	int m_nHeight;
	int m_nMinWidth;
	bool m_bDestroyWindow;

    CStdString m_strDivisionImage;   // �˵���֮ǰ�ָ���
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
    bool IsChecked();                           // �˵����Ƿ�ΪChecked״̬
    void SetChecked( bool bChecked );           
    void AddDivisionLine( LPCTSTR pstrImage = NULL );    // ��Ӳ˵��ָ���
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

    static CStdString m_arrowNorImage;  // �����Ӳ˵���ʱ���ұ߼�ͷ
    static CStdString m_arrowHotImage;  // �����Ӳ˵���ʱ���ұ߼�ͷHot״̬
    static CStdString m_ckeckNorImage;  // Checked
    static CStdString m_ckeckHotImage;  // Checked Hot״̬
    CStdString m_strNorIcon;            // IconͼƬ
    CStdString m_strHotIcon;            // Hover״̬�µ�IconͼƬ 
	CStdString m_strDisableIcon;		// Disable״̬�µ�IconͼƬ
    CStdString m_strCheckedNorIcon;    // Selected����ͨ״̬��ͼƬ
    CStdString m_strCheckedHotIcon;    // Selected��Hover״̬�µ�ͼƬ
    bool m_bChecked;
    bool m_bMouseHotSelected;
	DWORD      m_dwID;
	RECT	   m_rcChildTextPadding;	//�Ӳ˵��ı�textpadding����
	RECT	   m_rcDrawTextPadding;		//�Լ�����ʱʹ�õ�textpadding
};

// �˵���֮��ķָ���
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
