// 菜单控件
#include "StdAfx.h"
#include "uimenu.h"
namespace DuiLib {
#define WM_MENU_QUIT  WM_USER + 1000

/////////////////////////////////////////////////////////////////////////////////////
//
ContextMenuObserver s_context_menu_observer;
CMenuUI::CMenuUI()
{
	if ( GetHeader() != NULL )
	{
		GetHeader()->SetVisible( false );
	}
	m_ListInfo.dwHotBkColor = 0;
}

CMenuUI::~CMenuUI()
{

}

LPCTSTR CMenuUI::GetClass() const
{
    return _T("CMenuUI");
}

LPVOID CMenuUI::GetInterface( LPCTSTR pstrName )
{
	if ( _tcscmp(pstrName, _T("Menu")) == 0 ) 
	{
		return static_cast<CMenuUI*>(this);
	}

	return CListUI::GetInterface( pstrName );
}

void CMenuUI::DoEvent( TEventUI& event )
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else CVerticalLayoutUI::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_SETFOCUS ) 
    {
        m_bFocused = true;
        return;
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        m_bFocused = false;
        return;
    }

    switch( event.Type ) {
    case UIEVENT_KEYDOWN:
        switch( event.chKey ) {
        case VK_RIGHT:{
            // 右键时存在子菜单就展开 zuojiayong
            int nCurSel = GetCurSel();
            SelectItem( nCurSel,false ); 
            CMenuElementUI *pElement = dynamic_cast<CMenuElementUI*>(GetItemAt(nCurSel));
            if ( NULL == pElement ) return;         
            if ( !GetItemAt(nCurSel)->IsEnabled() ){
                return;
            }
            // 销毁其他的子菜单
            ContextMenuParam param;
            param.hWnd = m_pManager->GetPaintWindow();
            param.wParam = ContextMenuParam::emRemoveSubMenu;
            s_context_menu_observer.RBroadcast( param );

            bool hasSubMenu = false;
            for( int i = 0; i < pElement->GetCount(); ++i )
            {
                CMenuElementUI* pSubElement = static_cast<CMenuElementUI*>( 
                    pElement->GetItemAt(i)->GetInterface( _T("MenuElement") ) );
                if( pSubElement != NULL )
                {
                    pSubElement->SetVisible( true );
                    pSubElement->SetInternVisible( true );
                    hasSubMenu = true;
                }
            }

            if( hasSubMenu )
            {
                pElement->CreateMenuWnd();
                // 置第一项为选择项;
                CMenuWnd* pWnd = pElement->GetMenuWnd();             
                CMenuUI* pMenu = pWnd->m_pLayout;   
                pMenu->SelectItem( pMenu->FindSelectable( 0, false ), true );
            }
            return;
        }
        case VK_LEFT:
            // 第一级菜单左键不响应,此处无法感知，在RBroadcast中过滤 zuojiayong           
            ContextMenuParam param;
            param.hWnd = m_pManager->GetPaintWindow();
            param.wParam = ContextMenuParam::emRemoveSelf;
            s_context_menu_observer.RBroadcast( param );
            return;
        case VK_UP:
            SelectItem(FindSelectable(m_iCurSel - 1, false), true);
            return;
        case VK_DOWN:
            SelectItem(FindSelectable(m_iCurSel + 1, true), true);
            return;
        case VK_PRIOR:
            PageUp();
            return;
        case VK_NEXT:
            PageDown();
            return;
        case VK_HOME:
            SelectItem(FindSelectable(0, false), true);
            return;
        case VK_END:
            SelectItem(FindSelectable(GetCount() - 1, true), true);
            return;
        case VK_RETURN:
            if( m_iCurSel != -1 ) GetItemAt(m_iCurSel)->Activate();
            return;
        }
        break;
    case UIEVENT_SCROLLWHEEL:
        {
            int nLine=3;
            SystemParametersInfo(SPI_GETWHEELSCROLLLINES,0,&nLine,0);
            switch( LOWORD(event.wParam) ) {
            case SB_LINEUP:
                if( m_bScrollSelect ) SelectItem(FindSelectable(m_iCurSel - 1, false), true);
                else LineUp(nLine);
                return;
            case SB_LINEDOWN:
                if( m_bScrollSelect ) SelectItem(FindSelectable(m_iCurSel + 1, true), true);
                else LineDown(nLine);
                return;
            }
        }
        break;
    }
    CVerticalLayoutUI::DoEvent(event);
	//return __super::DoEvent( event );
}

bool CMenuUI::Add( CControlUI* pControl )
{
	if ( _tcsicmp(pControl->GetClass(), _T("CMenuDivisionUI")) == 0 ) // 分割线
	{
		return CListUI::Add( pControl );
	}

	CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>( pControl->GetInterface( _T("MenuElement") ) );
	if ( NULL == pMenuItem )
	{
		return false;
	}

	for ( int i = 0; i < pMenuItem->GetCount(); ++i )
	{
		if ( pMenuItem->GetItemAt(i)->GetInterface( _T("MenuElement") ) != NULL )
		{
			(static_cast<CMenuElementUI*>(pMenuItem->GetItemAt(i)->GetInterface( _T("MenuElement") )))->SetInternVisible( false );
		}
	}
	return CListUI::Add( pControl );
}

bool CMenuUI::AddAt( CControlUI* pControl, int iIndex )
{
	if ( _tcsicmp(pControl->GetClass(), _T("CMenuDivisionUI")) == 0 ) // 分割线
	{
		return CListUI::AddAt( pControl, iIndex );
	}

	CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface( _T("MenuElement") ) );
	if (pMenuItem == NULL)
		return false;

	for ( int i = 0; i < pMenuItem->GetCount(); ++i )
	{
		if ( pMenuItem->GetItemAt(i)->GetInterface( _T("MenuElement") ) != NULL )
		{
			(static_cast<CMenuElementUI*>(pMenuItem->GetItemAt(i)->GetInterface( _T("MenuElement") )))->SetInternVisible( false );
		}
	}
	return CListUI::AddAt( pControl, iIndex );
}

int CMenuUI::GetItemIndex( CControlUI* pControl ) const
{
	CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>( pControl->GetInterface( _T("MenuElement") ) );
	if ( NULL == pMenuItem )
	{
		return -1;
	}

	return __super::GetItemIndex( pControl );
}

bool CMenuUI::SetItemIndex( CControlUI* pControl, int iIndex )
{
	CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>( pControl->GetInterface( _T("MenuElement") ) );
	if ( NULL == pMenuItem )
	{
		return false;
	}

	return __super::SetItemIndex( pControl, iIndex );
}

bool CMenuUI::Remove( CControlUI* pControl )
{
	CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>( pControl->GetInterface( _T("MenuElement") ) );
	if ( NULL == pMenuItem )
	{
		return false;
	}

	return __super::Remove( pControl );
}

SIZE CMenuUI::EstimateSize( SIZE szAvailable )
{
	SIZE szFixSize;
	szFixSize.cx = 0;
    szFixSize.cy = 0;
    for( int it = 0; it < GetCount(); it++ ) 
	{
        CControlUI* pControl = static_cast<CControlUI*>( GetItemAt( it ) );
        if( !pControl->IsVisible() ) continue;
        SIZE sz = pControl->EstimateSize( szAvailable );	
        szFixSize.cy += sz.cy;
		if( szFixSize.cx < sz.cx ) szFixSize.cx = sz.cx;		
    }
    szFixSize.cx += m_rcInset.left + m_rcInset.right;
    szFixSize.cy += m_rcInset.top + m_rcInset.bottom;

    return szFixSize;
}

void CMenuUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
    // 存在子菜单时的右边箭头
    if ( !_tcscmp( pstrName, _T("submenuarrownormalimage"))){
         CMenuElementUI::SetDefaultArrowNorImage( pstrValue );
     }
     else if ( !_tcscmp( pstrName, _T("submenuarrowhotimage")) ){
         CMenuElementUI::SetDefaultArrowHotImage( pstrValue );
     }
     else if ( !_tcscmp( pstrName, _T("submenuchecknorimage")) ){
         CMenuElementUI::SetDefaultCheckNorImage( pstrValue );
     }
     else if ( !_tcscmp( pstrName, _T("submenucheckhotimage")) ){
         CMenuElementUI::SetDefaultCheckHotImage( pstrValue );
     }
     else if ( !_tcscmp( pstrName, _T("menudivisioniamge")) ){        
         CMenuDivisionUI::SetDefaultDivisionImage( pstrValue );
     }
     else if ( !_tcscmp( pstrName, _T("menudivisioncolor")) ){        
         CMenuDivisionUI::SetDefaultDivisionImage( pstrValue );
     }
     else {
        CListUI::SetAttribute( pstrName, pstrValue );
    }
}

/////////////////////////////////////////////////////////////////////////////////////
//

CMenuWnd::CMenuWnd( STRINGorID xml, LPCTSTR pSkinType, CPaintManagerUI* pParentManager )
: m_hParentWnd( NULL )
, m_pParentManager( pParentManager )
, m_pOwner( NULL ) 
, m_pLayout( NULL ) 
, m_pRoot( NULL )
, m_xml( xml )
, m_sType( pSkinType )
, m_dwAlignMent( eMenuAlignment_Left | eMenuAlignment_Top )
, m_nHeight(1000)
, m_nMinWidth( 0 )
, m_bDestroyWindow(false)
{
    memset( &m_BasedPoint, 0, sizeof(m_BasedPoint));
	s_context_menu_observer.AddReceiver( this );	
}

CMenuWnd::CMenuWnd( CMenuElementUI* pOwner, CPaintManagerUI* pParentManager )
: m_hParentWnd( NULL ) 
, m_pParentManager( pParentManager )
, m_pOwner( pOwner ) 
, m_pLayout( NULL ) 
, m_pRoot( NULL )
, m_xml( _T("") )
, m_sType( _T("") )
, m_dwAlignMent( eMenuAlignment_Left | eMenuAlignment_Top )
, m_nHeight(550)
, m_nMinWidth( 0 )
, m_bDestroyWindow(false)
{
    memset( &m_BasedPoint, 0, sizeof(m_BasedPoint) );
	s_context_menu_observer.AddReceiver( this );
}

CMenuWnd::~CMenuWnd()
{
}

bool CMenuWnd::AddMenuItem( CControlUI* pControl )
{
	if ( !pControl || !m_pRoot )
	{
		return false;
	}

	m_pRoot->Add( pControl );

	return true;
}

void CMenuWnd::TrackPopupMenu( DWORD dwAlignment, POINT point, HWND hParentWnd )
{
	m_dwAlignMent = dwAlignment;
	m_BasedPoint = point;
	m_hParentWnd = hParentWnd;
	Create( m_hParentWnd, NULL, WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, QRect() );
}

void CMenuWnd::DeleteMenuItem( LPCTSTR pstrName )
{
	if ( m_pRoot == NULL )
	{
		return;
	}

	CControlUI* pControl = ( static_cast<CContainerUI*>(m_pRoot) )->FindSubControl( pstrName );
    int hight_remove = 0;
	if ( pControl != NULL )
	{
        hight_remove = pControl->GetFixedHeight();
		m_pRoot->Remove( pControl );
	}
	if (hight_remove == 0) return;
    RECT rcWnd = { 0 };
    HWND hPaintWnd  = nullptr;
    if (m_pOwner != nullptr) {
        hPaintWnd  = m_pOwner->GetManager()->GetPaintWindow();
    }
    else {
        hPaintWnd  = m_pm.GetPaintWindow();
    }
    GetWindowRect(hPaintWnd, &rcWnd);
    rcWnd.bottom -= hight_remove;
    MoveWindow(hPaintWnd, rcWnd.left, rcWnd.top, 
        rcWnd.right - rcWnd.left,
        rcWnd.bottom - rcWnd.top,
        FALSE);    
}

void CMenuWnd::CheckMenuItem( LPCTSTR pstrName, bool bCheck )
{
	if ( m_pRoot == NULL )
	{
		return;
	}

	CControlUI* pControl = ( static_cast<CContainerUI*>(m_pRoot) )->FindSubControl( pstrName );
	if ( pControl != NULL && _tcsicmp( pControl->GetClass(), _T("MenuElementUI") ) == 0 )
	{
		( static_cast<CMenuElementUI*>(pControl) )->CheckMenu( bCheck );
	}
}

void CMenuWnd::EnableMenuItem( LPCTSTR pstrName, bool bEnable )
{
	if ( m_pRoot == NULL )
	{
		return;
	}

	CControlUI* pControl = ( static_cast<CContainerUI*>(m_pRoot) )->FindSubControl( pstrName );
	if ( pControl != NULL &&  _tcsicmp( pControl->GetClass(), _T("MenuElementUI") ) == 0 )
	{
		pControl->SetEnabled( bEnable );
	}
}

void CMenuWnd::ModifyMenu( LPCTSTR pstrName, LPCTSTR pstrText )
{
	if ( NULL == m_pRoot )
	{
		return;
	}

	CControlUI* pControl = ( static_cast<CContainerUI*>(m_pRoot) )->FindSubControl( pstrName );
	if ( pControl != NULL &&  _tcsicmp( pControl->GetClass(), _T("MenuElementUI") ) == 0 )
	{
		pControl->SetText( pstrText );
	}
}

void CMenuWnd::SetItemTextPadding( RECT rc )
{
	m_pRoot->SetItemTextPadding( rc );
}

LPCTSTR CMenuWnd::GetWindowClassName() const
{
    return _T("CMenuWndUI");
}

void CMenuWnd::OnFinalMessage( HWND hWnd )
{
	s_context_menu_observer.RemoveReceiver( this );

	if( m_pOwner != NULL ) 
	{
		m_pOwner->SetManager( m_pOwner->GetManager(), m_pOwner->GetParent(), false );
		m_pOwner->SetPos( m_pOwner->GetPos() );

		for( int i = 0; i < m_pOwner->GetCount(); i++ ) 
		{
			CMenuElementUI* pElement = static_cast<CMenuElementUI*>( m_pOwner->GetItemAt(i)->GetInterface( _T("MenuElement") ) );
			if( pElement != NULL ) 
			{
				pElement->SetOwner( m_pOwner->GetParent() );
				pElement->SetVisible( false );
				pElement->SetInternVisible( false );
			}
		}
		m_pOwner->m_pWindow = NULL;
		m_pOwner->m_uButtonState &= ~ UISTATE_PUSHED;
		m_pOwner->Invalidate();
	}

    delete this;
}

CControlUI* CMenuWnd::CreateControl(LPCTSTR pstrClass) {
    if (_tcsicmp(pstrClass, _T("Menu")) == 0)
        return new CMenuUI;
    else if (_tcsicmp(pstrClass, _T("MenuElement")) == 0)
        return new CMenuElementUI;
    else if (_tcsicmp(pstrClass, _T("MenuDivision")) == 0)
        return new CMenuDivisionUI;
    return NULL;
}

void CMenuWnd::CreateWndByOwner()
{
	LONG styleValue = ::GetWindowLong( *this, GWL_STYLE );
	styleValue &= ~WS_CAPTION;
	::SetWindowLong( *this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN );

	m_pm.Init( m_hWnd );
	// The trick is to add the items to the new container. Their owner gets
	// reassigned by this operation - which is why it is important to reassign
	// the items back to the righfull owner/manager when the window closes.
	if( m_pLayout != NULL )
	{
		delete m_pLayout;
		m_pLayout = NULL;
	}

	m_pLayout = new CMenuUI();
	m_pm.UseParentResource( m_pOwner->GetManager() );
	m_pLayout->SetManager( &m_pm, NULL, true );
	LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList( _T("Menu") );
	if( pDefaultAttributes ) 
	{
		m_pLayout->ApplyAttributeList( pDefaultAttributes );
    }
	m_pLayout->SetAutoDestroy( false );
	m_pLayout->EnableScrollBar();
	for( int i = 0; i < m_pOwner->GetCount(); i++ ) 
	{
		if( m_pOwner->GetItemAt(i)->GetInterface( _T("MenuElement") ) != NULL )
		{
			(static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)))->SetOwner( m_pLayout );
			m_pLayout->Add( static_cast<CControlUI*>( m_pOwner->GetItemAt(i) ) );
		}
	}

	m_pm.AddNotifier( this );
	m_pm.AttachDialog( m_pLayout );
	//m_pm.SetFocusNeeded( true );

	// Position the popup window in absolute space
	RECT rcOwner = m_pOwner->GetPos();
	RECT rc = rcOwner; 
	int cxFixed = 0;
	int cyFixed = 0;

#if defined(WIN32) && !defined(UNDER_CE)
	MONITORINFO oMonitor = {}; 
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo( ::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor );
	QRect rcWork = oMonitor.rcWork;
#else
	QRect rcWork;
	GetWindowRect( m_pOwner->GetManager()->GetPaintWindow(), &rcWork );
#endif
	SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };

	for( int it = 0; it < m_pOwner->GetCount(); it++ ) 
	{
		if(m_pOwner->GetItemAt(it)->GetInterface( _T("MenuElement") ) != NULL )
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_pOwner->GetItemAt(it));
			SIZE sz = pControl->EstimateSize( szAvailable );
			cyFixed += sz.cy;

			if( cxFixed < sz.cx )
				cxFixed = sz.cx;

			if ( cyFixed > m_pOwner->m_pWindow->GetMaxHeight() )
			{
				cyFixed = m_nHeight;
			}
		}
	}

    CControlUI* pRootOwner = m_pOwner;
    while (pRootOwner->GetParent() != NULL) {
        pRootOwner = pRootOwner->GetParent();
    }
    CMenuUI* pMenu =  static_cast<CMenuUI*>(pRootOwner->GetInterface(_T("Menu")));
    if (pMenu != NULL) {  
        // 设置roundcorner
        CPaintManagerUI* root_pm = pMenu->GetManager();
        if (root_pm != NULL) {
            SIZE szCorner = root_pm->GetRoundCorner();
            m_pm.SetRoundCorner(szCorner.cx, szCorner.cy);
        } 
    }   
    RECT rcInset = m_pLayout->GetInset();
	cyFixed += rcInset.top + rcInset.bottom;
	cxFixed += rcInset.left + rcInset.right;

	RECT rcWindow;
	::GetWindowRect( m_pOwner->GetManager()->GetPaintWindow(), &rcWindow );

	rc.top = rcOwner.top - rcInset.top;
	rc.bottom = rc.top + cyFixed;
	::MapWindowRect( m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc );
	rc.left = rcWindow.right;
	rc.right = rc.left + cxFixed;
	rc.right += rcInset.left + rcInset.right;

	bool bReachBottom = false;
	bool bReachRight = false;
	LONG chRightAlgin = 0;
	LONG chBottomAlgin = 0;

	RECT rcPreWindow = {0};
	ContextMenuObserver::Iterator<BOOL, ContextMenuParam> iterator(s_context_menu_observer);
	ReceiverImplBase<BOOL, ContextMenuParam>* pReceiver = iterator.next();
	while( pReceiver != NULL ) 
	{
		CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>( pReceiver );
		if( pContextMenu != NULL ) 
		{
			::GetWindowRect( pContextMenu->GetHWND(), &rcPreWindow );

			bReachRight = rcPreWindow.left >= rcWindow.right;
			bReachBottom = rcPreWindow.top >= rcWindow.bottom;
			if( pContextMenu->GetHWND() == m_pOwner->GetManager()->GetPaintWindow() 
				||  bReachBottom || bReachRight )
			{
				break;
			}
		}
		pReceiver = iterator.next();
	}

	if ( bReachBottom )
	{
		rc.bottom = rcWindow.top;
		rc.top = rc.bottom - cyFixed;
	}

	if ( bReachRight )
	{
		rc.right = rcWindow.left;
		rc.left = rc.right - cxFixed;
	}

	if( rc.bottom > rcWork.bottom )
	{
		rc.bottom = rc.top + rcInset.top + m_pOwner->GetFixedHeight();
        rc.bottom += rcInset.bottom;
		rc.top = rc.bottom - cyFixed;
	}

	if ( rc.right > rcWork.right )
	{
		rc.right = rcWindow.left;
		rc.left = rc.right - cxFixed;
	}

	if( rc.top < rcWork.top )
	{
		rc.top = rcOwner.top;
		rc.bottom = rc.top + cyFixed;
	}

	if (rc.left < rcWork.left)
	{
		rc.left = rcWindow.right;
		rc.right = rc.left + cxFixed;
	}

    // 这里向右移2px是参照现有PPS效果
	::MoveWindow( m_hWnd, rc.left + 2, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE );

    HWND hWndParent = m_hWnd;
    while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent( hWndParent );
    ::ShowWindow( m_hWnd, SW_SHOW );
#if defined(WIN32) && !defined(UNDER_CE)
    ::SendMessage( hWndParent, WM_NCACTIVATE, TRUE, 0L );
#endif	
}


void CMenuWnd::CreateWndByXml()
{
	m_pm.Init( m_hWnd );

    // 创建要提前，不然控件找不到
    CDialogBuilder builder;

    CControlUI* pRoot = builder.Create( m_xml, m_sType.GetData(), this, &m_pm );
    if ( pRoot->GetInterface( _T("Menu") ) )
    {
        m_pRoot = static_cast<CMenuUI*>( pRoot );
    }
	m_pm.AddNotifier( this );
	m_pm.AttachDialog( m_pRoot );
	//m_pm.SetFocusNeeded( true );
    
#if defined(WIN32) && !defined(UNDER_CE)
    MONITORINFO oMonitor = {};
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromPoint(m_BasedPoint, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
    QRect rcWork = oMonitor.rcMonitor;
#else
	QRect rcWork;
	GetWindowRect( m_pm.GetPaintWindow(), &rcWork );
#endif
	SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
	szAvailable = m_pRoot->EstimateSize( szAvailable );    

	//m_pm.SetInitSize( szAvailable.cx, szAvailable.cy );
	SIZE szInit = m_pm.GetInitSize();

	if ( 0 < m_nMinWidth && szInit.cx < m_nMinWidth )
	{
		szInit.cx = m_nMinWidth;
		if ( szInit.cy > m_nHeight )
		{
			szInit.cy = m_nHeight;
			m_pm.SetMaxInfo(szInit.cx,szInit.cy);
		}
		m_pm.SetMinInfo(szInit.cx,szInit.cy);
	}
	else if ( szInit.cy > m_nHeight )
	{
		szInit.cy = m_nHeight;
		m_pm.SetMaxInfo(szInit.cx,szInit.cy);
	}

	QRect rc;
	POINT point = m_BasedPoint;
	rc.left = point.x;
	rc.top = point.y;
	rc.right = rc.left + szInit.cx;
	rc.bottom = rc.top + szInit.cy;

	int nWidth = rc.GetWidth();
	int nHeight = rc.GetHeight();

	if ( m_dwAlignMent & eMenuAlignment_Right )
	{
		rc.right = point.x;
		rc.left = rc.right - nWidth;
	}

	if ( m_dwAlignMent & eMenuAlignment_Bottom )
	{
		rc.bottom = point.y;
		rc.top = rc.bottom - nHeight;
	}

	if( rc.bottom > rcWork.bottom )
	{
		rc.bottom = rcWork.bottom;
		rc.top = rc.bottom - nHeight;
	}

	if ( rc.right > rcWork.right )
	{
		rc.right = rcWork.right;
		rc.left = rc.right - nWidth;
	}

	if( rc.top < rcWork.top )
	{
		rc.top = rcWork.top;
		rc.bottom = rc.top + nHeight;
	}

	if (rc.left < rcWork.left)
	{
		rc.left = rcWork.left;
		rc.right = rc.left + nWidth;
	}

	::SetForegroundWindow( m_hWnd );
	::MoveWindow( m_hWnd, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), FALSE );
	SetWindowPos( m_hWnd, HWND_TOPMOST, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), SWP_NOACTIVATE | SWP_SHOWWINDOW );
	m_pRoot->Invalidate();
	if ( m_pLayout )
	{
		m_pLayout->Invalidate();
	}
	
	HWND hWndParent = m_hWnd;
	while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent( hWndParent );
	::ShowWindow( m_hWnd, SW_SHOW );
#if defined(WIN32) && !defined(UNDER_CE)
	::SendMessage( hWndParent, WM_NCACTIVATE, TRUE, 0L );
#endif
}

LRESULT CMenuWnd::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if( m_pOwner != NULL) {
		// 通过父菜单创建子菜单
		CreateWndByOwner();
	}
	else {    
		// 通过XML配置文件创建菜单
		CreateWndByXml();
	}

	bHandled = true;
	return 0;
}

LRESULT CMenuWnd::OnClose( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    ::DestroyWindow(GetHWND());
	bHandled = true;
	return true;
}

LRESULT CMenuWnd::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	HWND hFocusWnd = (HWND)wParam;

	BOOL bInMenuWindowList = FALSE;
	ContextMenuParam param;
	param.hWnd = GetHWND();

	ContextMenuObserver::Iterator<BOOL, ContextMenuParam> iterator(s_context_menu_observer);
	ReceiverImplBase<BOOL, ContextMenuParam>* pReceiver = iterator.next();
	while( pReceiver != NULL ) 
	{
		CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>( pReceiver );
		if( pContextMenu != NULL && pContextMenu->GetHWND() ==  hFocusWnd )
		{
			bInMenuWindowList = TRUE;
			break;
		}
		pReceiver = iterator.next();
	}

	// 销毁整个菜单
	if( !bInMenuWindowList ) 
	{
		param.wParam = ContextMenuParam::emRemoveAll;
		s_context_menu_observer.RBroadcast( param );
	}

	bHandled = true;
	return 0;
}

LRESULT CMenuWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    SIZE szRoundCorner = m_pm.GetRoundCorner();
    if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) 
    {
        QRect rcWnd;
        ::GetWindowRect(*this, &rcWnd);
        rcWnd.Offset(-rcWnd.left, -rcWnd.top);
        rcWnd.right++; rcWnd.bottom++;
        HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
        ::SetWindowRgn(*this, hRgn, TRUE);
        ::DeleteObject(hRgn);
    }

    bHandled = FALSE;

    return 0;  
}

LRESULT CMenuWnd::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg )
	{
	case WM_CREATE: lRes = OnCreate( uMsg, wParam, lParam, bHandled ); break;
	case WM_CLOSE:  lRes = OnClose( uMsg, wParam, lParam, bHandled ); break;
	case WM_KILLFOCUS: lRes = OnKillFocus( uMsg, wParam, lParam, bHandled ); break;
    case WM_SIZE: lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
	case WM_RBUTTONDOWN:
	case WM_CONTEXTMENU:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		{
			return 0;
		}

	default: bHandled = FALSE;
	}
	
	if( bHandled ) 
	{
		return lRes;
	}

	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_ESCAPE )
		{
			Close();
		}
	}

    if ( WM_PAINT == uMsg) {
        int i = 0;
    }
    if( m_pm.MessageHandler( uMsg, wParam, lParam, lRes ) ) 
	{
		return lRes;
	}
    return CWindowWnd::HandleMessage( uMsg, wParam, lParam );
}

void CMenuWnd::Notify( TNotifyUI& msg )
{
	// 选中菜单的消息要发给父窗口
	if ( _tcsicmp( msg.sType, NOTIFY_ITEM_CLICK ) == 0 && m_pParentManager != NULL ) 
	{
		//ShowWindow( false );
		CMenuElementUI* pElementUI = static_cast<CMenuElementUI*>(msg.pSender);
		if( pElementUI ){
			msg.lParam = pElementUI->GetID();
		}
		m_pParentManager->SendNotify( msg, false ); // 异步发送，解决弹出模态框崩溃		
	}
	else if ( _tcsicmp( msg.sType, NOTIFY_ITEM_ACTIVATE ) == 0 && m_pParentManager != NULL ) 
	{
		//ShowWindow( false );
		msg.sType = NOTIFY_ITEM_CLICK;	// 回车消息转成菜单点击消息
		m_pParentManager->SendNotify( msg, false ); // 异步发送，解决弹出模态框崩溃
	}            
 	else if ( _tcsicmp( msg.sType, NOTIFY_KILLFOCUS ) == 0 && m_pParentManager != NULL ) 
    {
        m_pParentManager->SendNotify( msg, false ); 
    }
}

BOOL CMenuWnd::Receive( ContextMenuParam param )
{
	switch ( param.wParam )
	{
	case ContextMenuParam::emRemoveAll:
		Close();
		break;
    case ContextMenuParam::emRemoveSubMenu:
		{
			HWND hParent = GetParent( m_hWnd );
			while ( hParent != NULL )
			{
				if ( hParent == param.hWnd ) 
				{
					Close();
					break;
				}
				hParent = GetParent( hParent );
			}
		}
        break; 
    case ContextMenuParam::emRemoveSelf:
        // RemoveSelf为解决展开子菜单后按VK_LEFT关闭子菜单
        if ( param.hWnd == this->GetHWND() )
        {        
            // 如果已经是顶层菜单了VK_LEFT不响应
            if ( m_pOwner != NULL ){
                Close();
            }
        }
        break;
    default:
        break;
	}

	return TRUE;
}

CMenuUI* CMenuWnd::GetMenuRoot()
{
	return m_pRoot;
}

void CMenuWnd::SetMaxHeight( int nHeight )
{
	m_nHeight = nHeight;
}

int CMenuWnd::GetMaxHeight()
{
	return m_nHeight;
}

void CMenuWnd::SetMinWidth( int nWidth )
{
	m_nMinWidth = nWidth;
}

int CMenuWnd::GetMinWidth()
{
	return m_nMinWidth;
}

void CMenuWnd::SetDivisionLineImage( LPCTSTR pstrImage )
{
    m_strDivisionImage = pstrImage;
}

void CMenuWnd::AddDivisionLine( LPCTSTR pstrImage /*= NULL */ )
{
    CMenuDivisionUI* pLine = new CMenuDivisionUI; 
    if ( NULL != pstrImage ){
        pLine->SetDivisionImage( pstrImage );
    }
    else{
        pLine->SetDivisionImage( CMenuDivisionUI::m_strDefaultDivImg );
    }
    pLine->SetFixedHeight( 1 );
    this->AddMenuItem( pLine );
}

CMenuElementUI*  CMenuWnd::GetElementUIByID(DWORD dwID)
{
	for( int i = 0 ; i < m_pRoot->GetCount(); i++ )
	{
		CMenuElementUI* pMenuEle = static_cast<CMenuElementUI*>(m_pRoot->GetItemAt(i));
		if( pMenuEle && pMenuEle->GetID() == dwID)
		{
			return pMenuEle;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////////////
//
CStdString CMenuElementUI::m_arrowNorImage = _T("");
CStdString CMenuElementUI::m_arrowHotImage = _T("");
CStdString CMenuElementUI::m_ckeckNorImage = _T("");
CStdString CMenuElementUI::m_ckeckHotImage = _T("");
CStdString CMenuDivisionUI::m_strDefaultDivImg = _T("");

CMenuElementUI::CMenuElementUI() 
: m_pWindow( NULL )
, m_pCheck( NULL )
, m_bChecked( false )
, m_bMouseHotSelected(true)
{
	::ZeroMemory(&m_rcChildTextPadding, sizeof(RECT));
	::ZeroMemory(&m_rcDrawTextPadding, sizeof(RECT));
	m_cxyFixed.cy = 25;
	m_bMouseChildEnabled = true;
	m_dwID = 0;
	SetMouseChildEnabled( false );
}

CMenuElementUI::~CMenuElementUI()
{
}

LPCTSTR CMenuElementUI::GetClass() const
{
	return _T("MenuElementUI");
}

LPVOID CMenuElementUI::GetInterface( LPCTSTR pstrName )
{
    if( _tcsicmp(pstrName,  _T("MenuElement") ) == 0 ) 
	{
		return static_cast<CMenuElementUI*>(this);    
	}
    return CListContainerElementUI::GetInterface( pstrName );
}

void CMenuElementUI::DoPaint( HDC hDC, const RECT& rcPaint )
{  
    if( !::IntersectRect( &m_rcPaint, &rcPaint, &m_rcItem ) ) 
	{
		return;
	}

	CListContainerElementUI::DrawItemBk( hDC, m_rcItem );
	DrawItemText( hDC, m_rcItem );
	for ( int i = 0; i < GetCount(); ++i )
	{
		if ( GetItemAt(i)->GetInterface( _T("MenuElement") ) == NULL )
		{
			GetItemAt(i)->DoPaint( hDC, rcPaint );
		}
	}
 
    // 绘制Icon
    DrawMenuIcon(hDC);
    // 如果存在子菜单，绘制角标 zuo.JY
    if ( GetCount() > 0  )
    {
        DrawParentArrow( hDC );
    }
}

void CMenuElementUI::DrawParentArrow( HDC hDc )
{
    QRect rcPaint = m_rcItem;
    rcPaint.left = rcPaint.right - 30;
    if ( IsSelected() ){
        if ( m_arrowHotImage.IsEmpty() ) return;
        bool bDraw = CRenderEngine::DrawImageString( hDc, m_pManager, m_rcItem, 
            rcPaint, m_arrowHotImage );
        if ( !bDraw ) m_arrowHotImage.Empty();   
    }
    else{
        if ( m_arrowNorImage.IsEmpty() ) return;
        bool bDraw = CRenderEngine::DrawImageString( hDc, m_pManager, m_rcItem, 
            rcPaint, m_arrowNorImage );
        if ( !bDraw ) m_arrowNorImage.Empty();       
    }
}

void CMenuElementUI::DrawMenuIcon( HDC hDc )
{
	if (!IsEnabled())
	{
		if (!m_strDisableIcon.IsEmpty())
		{
			bool bDraw = CRenderEngine::DrawImageString( hDc, m_pManager, m_rcItem, m_rcPaint, m_strDisableIcon );
			if ( !bDraw ) m_strDisableIcon.Empty(); 
		}
	}
    else if ( IsChecked() ){
        if ( IsSelected() ){
            if ( m_strCheckedHotIcon.IsEmpty() ) {
                m_strCheckedHotIcon = m_ckeckHotImage;
                if (m_strCheckedHotIcon.IsEmpty()) return;
            }
            bool bDraw = CRenderEngine::DrawImageString( hDc, m_pManager, m_rcItem, m_rcPaint, m_strCheckedHotIcon );
            if ( !bDraw ) m_strCheckedHotIcon.Empty();     
        }
        else{
            if ( m_strCheckedNorIcon.IsEmpty() ) {
                m_strCheckedNorIcon = m_ckeckNorImage;
                if (m_strCheckedNorIcon.IsEmpty()) return;
            }
            bool bDraw = CRenderEngine::DrawImageString( hDc, m_pManager, m_rcItem, m_rcPaint, m_strCheckedNorIcon );
            if ( !bDraw ) m_strCheckedNorIcon.Empty();
        }       
    }
    else{
        if ( IsSelected() ){          
            if ( m_strHotIcon.IsEmpty() ) return;
            bool bDraw = CRenderEngine::DrawImageString( hDc, m_pManager, m_rcItem, m_rcPaint, m_strHotIcon );
            if ( !bDraw ) m_strHotIcon.Empty();
          
        }
        else{
            if ( m_strNorIcon.IsEmpty() ) return;
            bool bDraw = CRenderEngine::DrawImageString( hDc, m_pManager, m_rcItem, m_rcPaint, m_strNorIcon );
            if ( !bDraw ) m_strNorIcon.Empty();            
        }
    }    
}

void CMenuElementUI::DrawMenuLine( HDC hDc )
{
    
}


void CMenuElementUI::DrawItemText( HDC hDC, const RECT& rcItem )
{
    if( m_sText.IsEmpty() ) 
	{
		return;
	}

    if( NULL == m_pOwner ) 
	{
		return;
	}

    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    DWORD iTextColor = pInfo->dwTextColor;
    if( (m_uButtonState & UISTATE_HOT) != 0 ) 
	{
        iTextColor = pInfo->dwHotTextColor;
    }
    if( IsSelected() ) 
	{
        iTextColor = pInfo->dwSelectedTextColor;
    }
	if( IsChecked() && pInfo->dwCheckedTextColor )
	{
		iTextColor = pInfo->dwCheckedTextColor;
	}
    if( !IsEnabled() ) 
	{
        iTextColor = pInfo->dwDisabledTextColor;
    }

    int nLinks = 0;
    RECT rcText = rcItem;
	if (m_rcDrawTextPadding.left == 0 && m_rcDrawTextPadding.top == 0
		&& m_rcDrawTextPadding.right == 0 && m_rcDrawTextPadding.bottom == 0)
	{
		rcText.left += pInfo->rcTextPadding.left;
		rcText.right -= pInfo->rcTextPadding.right;
		rcText.top += pInfo->rcTextPadding.top;
		rcText.bottom -= pInfo->rcTextPadding.bottom;
	}
	else
	{
		rcText.left += m_rcDrawTextPadding.left;
		rcText.right -= m_rcDrawTextPadding.right;
		rcText.top += m_rcDrawTextPadding.top;
		rcText.bottom -= m_rcDrawTextPadding.bottom;
	}

    if( pInfo->bShowHtml )
	{
        CRenderEngine::DrawHtmlText( hDC, m_pManager, rcText, m_sText, iTextColor 
                                  , NULL, NULL, nLinks, DT_SINGLELINE | pInfo->uTextStyle );
	}
    else
	{
        CRenderEngine::DrawText( hDC, m_pManager, rcText, m_sText, iTextColor 
                              , pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle );
	}
}


SIZE CMenuElementUI::EstimateSize( SIZE szAvailable )
{
	SIZE cXY = {0};
	for( int it = 0; it < GetCount(); it++ ) 
	{
		CControlUI* pControl = static_cast<CControlUI*>( GetItemAt(it) );
        CMenuElementUI* pElem = static_cast<CMenuElementUI*>(pControl->GetInterface(_T("MenuElement")));
		if (pElem != NULL && pElem->GetOwner() == NULL) {
            pElem->SetOwner(this);
		}
        if( !pControl->IsVisible() ) continue;
		if( pControl == m_pCheck ) continue;
		SIZE sz = pControl->EstimateSize( szAvailable );
		cXY.cy += sz.cy;
		if( cXY.cx < sz.cx )
			cXY.cx = sz.cx;
	}
	if( cXY.cy == 0 && m_pOwner != NULL) 
	{
		TListInfoUI* pInfo = m_pOwner->GetListInfo();

		DWORD iTextColor = pInfo->dwTextColor;
		if( (m_uButtonState & UISTATE_HOT) != 0 ) 
		{
			iTextColor = pInfo->dwHotTextColor;
		}
		if( IsSelected() ) 
		{
			iTextColor = pInfo->dwSelectedTextColor;
		}
		if( !IsEnabled() ) 
		{
			iTextColor = pInfo->dwDisabledTextColor;
		}

		RECT rcText = { 0, 0, MAX(szAvailable.cx, m_cxyFixed.cx), 9999 };
		rcText.left += pInfo->rcTextPadding.left;
		rcText.right -= pInfo->rcTextPadding.right;

		cXY.cx = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right + 5;
		cXY.cy = rcText.bottom - rcText.top + pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
	}

	if( m_cxyFixed.cy != 0 ) 
	{
		cXY.cy = m_cxyFixed.cy;
	}

	if( m_cxyFixed.cx != 0 ){
		cXY.cx = m_cxyFixed.cx;
	}
	return cXY;
}

void CMenuElementUI::DoEvent( TEventUI& event )
{
    if ( UISTATE_HOT & m_uButtonState){
        Invalidate();
    }
	if( UIEVENT_MOUSEENTER == event.Type )
	{
		CListContainerElementUI::DoEvent( event );
		if( m_pWindow ) 
		{
			return;
		}

		if( m_pOwner != NULL && m_bMouseHotSelected)
		{
			m_pOwner->SelectItem( GetIndex(), false );
		}

		if ( !IsEnabled() )
		{
			return;
		}

        StartFocusInFadeEffect();
        Invalidate();

		// 销毁其他的子菜单
		ContextMenuParam param;
		param.hWnd = m_pManager->GetPaintWindow();
		param.wParam = ContextMenuParam::emRemoveSubMenu;
		s_context_menu_observer.RBroadcast( param );

		bool hasSubMenu = false;
		for( int i = 0; i < GetCount(); ++i )
		{
			CMenuElementUI* pElement = static_cast<CMenuElementUI*>( GetItemAt(i)->GetInterface( _T("MenuElement") ) );
			if( pElement != NULL )
			{
				pElement->SetVisible( true );
				pElement->SetInternVisible( true );
				pElement->SetDrawTextPadding(m_rcChildTextPadding);
				hasSubMenu = true;
			}
		}

		if( hasSubMenu )
		{
			CreateMenuWnd();
		}

		return;
	}
    
	if( UIEVENT_BUTTONUP == event.Type ||UIEVENT_RBUTTONUP == event.Type )
	{
		if( IsEnabled() )
		{
			CListContainerElementUI::DoEvent( event );
			// 说明有子菜单，不处理
			if( m_pWindow ) {
				return;
			}
			else{
                m_pManager->SendNotify(this, _T("itemclick"), (WPARAM)&event.ptMouse);
				// 销毁整个菜单
				ContextMenuParam param;
				param.hWnd = m_pManager->GetPaintWindow();
				param.wParam = ContextMenuParam::emRemoveAll;
				s_context_menu_observer.RBroadcast( param );
				CControlUI* pParent = GetParent();
				while ( pParent != NULL )
				{
					param.hWnd = pParent->GetManager()->GetPaintWindow();
					param.wParam = ContextMenuParam::emRemoveAll;
					s_context_menu_observer.RBroadcast( param );
					pParent = pParent->GetParent();
				}
			}
        }
        return;
    }
    if (event.Type == UIEVENT_MOUSELEAVE)
    {
        if (IsEnabled()) {
            StartFocusOutFadeEffect();
            Invalidate();
        }
        // return;
    }

    // 解决点击有子菜单项的菜单时会销毁菜单的问题 zuojiayong
    bool hasSubMenu = false;
    for( int i = 0; i < GetCount(); ++i )
    {
        CMenuElementUI* pSubElement = static_cast<CMenuElementUI*>( 
            GetItemAt(i)->GetInterface( _T("MenuElement") ) );
        if( pSubElement != NULL )
        {
            pSubElement->SetVisible( true );
            pSubElement->SetInternVisible( true );
            hasSubMenu = true;
        }
    }
    if ( hasSubMenu )
    {
        if( event.Type == UIEVENT_KEYDOWN && IsEnabled() )
        {
            if( event.chKey == VK_RETURN ) {
                //Activate();
                // 销毁其他的子菜单
                ContextMenuParam param;
                param.hWnd = m_pManager->GetPaintWindow();
                param.wParam = ContextMenuParam::emRemoveSubMenu;
                s_context_menu_observer.RBroadcast( param ); 
              
                CreateMenuWnd();
                // 置第一项为选择项;              
                CMenuUI* pMenu = m_pWindow->m_pLayout; 
                pMenu->SelectItem( pMenu->FindSelectable( 0, false ), true );
                Invalidate();
                return;
            }
        }
        if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
        {
            if( IsEnabled() ){
                Invalidate();
            }
            return;
        }       
    }

    // 改为在ButtonUp中而非ButtonDown中响应
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
    {
        if( IsEnabled() ){
            Select();
            Invalidate();
        }
        return;
    }
    CListContainerElementUI::DoEvent( event );
}

bool CMenuElementUI::Activate()
{
	if ( CListContainerElementUI::Activate() && m_bSelected )
	{
		if( m_pWindow ) {
			return true;
		}
		else{
			// 销毁整个菜单
			ContextMenuParam param;
			param.hWnd = m_pManager->GetPaintWindow();
			param.wParam = ContextMenuParam::emRemoveAll;
			s_context_menu_observer.RBroadcast( param );
		}

		return true;
	}
	return false;
}

CMenuWnd* CMenuElementUI::GetMenuWnd()
{
	return m_pWindow;
}

void CMenuElementUI::CreateMenuWnd()
{
	if( m_pWindow ) 
	{
		return;
	}

	m_pWindow = new CMenuWnd( static_cast<CMenuElementUI*>(this), m_pManager );
	ASSERT( m_pWindow );
	m_pWindow->Create( m_pManager->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, QRect() );
}

void CMenuElementUI::CheckMenu( bool bCheck )
{
	if ( m_pCheck == NULL )
	{
		m_pCheck = new CControlUI;
		Add( m_pCheck );
//		m_pCheck->SetBkImage( m_ckImage );

		// 设置位置初始值
		m_pCheck->SetFloat( true );
		int nTop = ( m_cxyFixed.cy - 25 ) / 2;
		SIZE szXY = { 5, nTop };
		m_pCheck->SetFixedXY( szXY );
		m_pCheck->SetFixedWidth( 25 );
		m_pCheck->SetFixedHeight( 25 );
	}

	m_pCheck->SetVisible( bCheck );
}

void CMenuElementUI::SetCheckPos( RECT rc )
{
	if ( m_pCheck == NULL )
	{
		m_pCheck = new CControlUI;
		Add( m_pCheck );
	}

	m_pCheck->SetFloat( true );
	SIZE szXY = { rc.left, rc.top };
	m_pCheck->SetFixedXY( szXY );
	m_pCheck->SetFixedWidth( rc.right - rc.left );
	m_pCheck->SetFixedHeight( rc.bottom - rc.top );
}

bool CMenuElementUI::SetSelect( bool bSelect )
{
    CListContainerElementUI::Select( bSelect );
	return true;
}

void CMenuElementUI::SetDefaultArrowNorImage( LPCTSTR pstrImage )
{
    m_arrowNorImage = pstrImage;
}

void CMenuElementUI::SetDefaultArrowHotImage( LPCTSTR pstrImage )
{
    m_arrowHotImage = pstrImage;
}

void CMenuElementUI::SetDefaultCheckNorImage( LPCTSTR pstrImage )
{
    m_ckeckNorImage = pstrImage;
}

void CMenuElementUI::SetDefaultCheckHotImage( LPCTSTR pstrImage )
{
    m_ckeckHotImage = pstrImage;
}

void CMenuElementUI::SetNormalIconImage( LPCTSTR pstrImage )
{
    m_strNorIcon = pstrImage;            // Icon图片
}

void CMenuElementUI::SetHotIconImage( LPCTSTR pstrImage )
{
    m_strHotIcon = pstrImage;            // Hover状态下的Icon图片 
}

void CMenuElementUI::SetDisableIconImage( LPCTSTR pstrImage )
{
	m_strDisableIcon = pstrImage;
}

void CMenuElementUI::SetSelectedNormalIconImage( LPCTSTR pstrImage )
{
    m_strCheckedNorIcon = pstrImage;    // Selected后普通状态的图片
}

void CMenuElementUI::SetSelectedHotIconImage( LPCTSTR pstrImage )
{
    m_strCheckedHotIcon = pstrImage;    // Selected后Hover状态下的图片
}

bool CMenuElementUI::IsChecked()
{
    return m_bChecked;
}

void CMenuElementUI::SetChecked( bool bChecked )
{
    m_bChecked = bChecked;
}

void CMenuElementUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{    
    if ( !_tcscmp( pstrName, _T("iconnormalimage"))){
        m_strNorIcon = pstrValue;
    }
    else if ( !_tcscmp( pstrName, _T("iconhotimage"))) {
        m_strHotIcon = pstrValue;
    } 
	else if ( !_tcscmp( pstrName, _T("icondisabledimage"))) {
		m_strDisableIcon = pstrValue;
	}
    else if ( !_tcscmp( pstrName, _T("iconcheckednormalimage"))){
        m_strCheckedNorIcon = pstrValue;
    }
    else if ( !_tcscmp(pstrName, _T("iconcheckedhotimage"))){
        m_strCheckedHotIcon = pstrValue;
    }
	else if( !_tcscmp( pstrName, _T("id")) )
	{
		m_dwID = _ttoi(pstrValue);
	}
	else if( _tcscmp(pstrName, _T("childtextpadding")) == 0 ) {
		RECT rcTextPadding = { 0 };
		LPTSTR pstr = NULL;
		rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetChildTextPadding(rcTextPadding);
	}
    else if (_tcscmp(pstrName, _T("mousehotselected")) == 0) SetMouseHotSelected(_tcscmp(pstrValue, _T("true")) == 0);
    else __super::SetAttribute(pstrName, pstrValue);
}

// 使用此接口前，请设置CMenuDivisionUI的默认图片
void CMenuElementUI::AddDivisionLine( LPCTSTR pstrImage /*= NULL*/)
{
    CMenuDivisionUI* pLine = new CMenuDivisionUI;   
    if ( NULL != pstrImage ){
        pLine->SetDivisionImage( pstrImage );
    }
    else {       
        pLine->SetDivisionImage( CMenuDivisionUI::m_strDefaultDivImg );      
    }
    pLine->SetFixedHeight( 1 ); 
    this->Add(pLine);
}

CMenuElementUI*  CMenuElementUI::GetSubElementByID(DWORD dwID)
{
	CMenuElementUI* pSubEle = NULL;
	for( int i = 0 ; i < GetCount(); i++ )
	{
		pSubEle = static_cast<CMenuElementUI*>(GetItemAt(i));
		if( pSubEle && pSubEle->GetID() == dwID )
		{
			return pSubEle;
		}
	}
	return NULL;
}

void CMenuElementUI::SetChildTextPadding(RECT rc)
{
	m_rcChildTextPadding = rc;
}

void CMenuElementUI::SetDrawTextPadding(RECT rc)
{
	m_rcDrawTextPadding = rc;
}

bool CMenuElementUI::RemoveAt( int iIndex )
{
    CControlUI* pItem = GetItemAt(iIndex);
    if (pItem == nullptr) return false;
    CPaintManagerUI* pm = pItem->GetManager();
    RECT rcWnd = { 0 };
    HWND hPaintWnd  = pm->GetPaintWindow();
    GetWindowRect(hPaintWnd, &rcWnd);
    rcWnd.bottom -= pItem->GetFixedHeight();
    MoveWindow(hPaintWnd, rcWnd.left, rcWnd.top, 
        rcWnd.right - rcWnd.left,
        rcWnd.bottom - rcWnd.top,
        FALSE);
    return __super::RemoveAt(iIndex);
}

LPCTSTR CMenuDivisionUI::GetClass() const
{
    return _T("CMenuDivisionUI");
}

void CMenuDivisionUI::DoEvent( TEventUI& event )
{
   __super::DoEvent(event);
}

void CMenuDivisionUI::SetDivisionImage( LPCTSTR pStrImage )
{
    __super::SetBkImage( pStrImage );
}

void CMenuDivisionUI::SetDivisionColor( DWORD dwColor )
{
    __super::SetBkColor( dwColor );
}

void CMenuDivisionUI::SetDefaultDivisionImage( LPCTSTR pstrImage )
{
    m_strDefaultDivImg = pstrImage;
}

// 此处为了一般情况下只设置一个static的线图,而又兼顾灵活性，可以设置个性线图
void CMenuDivisionUI::DoPaint( HDC hDC, const RECT& rcPaint )
{
    ASSERT(m_pOwner);
    if( m_pOwner == NULL ) return;
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    DWORD iBackColor = 0;
    if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) iBackColor = pInfo->dwBkColor;
    if (m_dwBackColor != 0) iBackColor = m_dwBackColor;
  
    if ( iBackColor != 0 ) {
        CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(iBackColor));
    }  

    if ( m_sBkImage.IsEmpty() ){
        if ( m_strDefaultDivImg.IsEmpty() ) return;
        bool bDraw = CRenderEngine::DrawImageString( hDC, m_pManager, m_rcItem, 
            rcPaint, m_strDefaultDivImg );
        if( !bDraw ) m_strDefaultDivImg.Empty();
    }
    else{          
        bool bDraw = CRenderEngine::DrawImageString( hDC, m_pManager, m_rcItem, 
            rcPaint, m_sBkImage );
        if( !bDraw ) m_sBkImage.Empty();        
    }    
}

CMenuDivisionUI::CMenuDivisionUI()
{
    // VK_UP和VK_DOWN的时候略过Division
    SetEnabled( false );
}

LPVOID CMenuDivisionUI::GetInterface( LPCTSTR pstrName )
{
    if ( _tcscmp(pstrName, _T("MenuDivision")) == 0 ) 
    {
        return static_cast<CMenuDivisionUI*>(this);
    }
    return __super::GetInterface( pstrName );
}


} // namespace DirectUICore
