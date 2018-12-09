#include "StdAfx.h"

namespace DuiLib {

QUI_STRING CControlUI::Item_Hot = _T("itemhot");
QUI_STRING CControlUI::Item_Not_Hot = _T("itemnothot");

CControlUI::CControlUI() : 
m_pManager(NULL), 
m_pParent(NULL), 
m_bUpdateNeeded(true),
m_bMenuUsed(false),
m_bVisible(true), 
m_bInternVisible(true),
m_bFocused(false),
m_bEnabled(true),
m_bMouseEnabled(true),
m_bKeyboardEnabled(true),
m_bFloat(false),
m_bSetPos(false),
m_chShortcut('\0'),
m_pTag(NULL),
m_pInheritableTag(NULL),
m_dwBackColor(0),
m_dwBackColor2(0),
m_dwBackColor3(0),
m_dwBorderColor(0),
m_dwFocusBorderColor(0),
m_bColorHSL(false),
m_nBorderSize(0),
m_nBorderStyle(PS_SOLID),
m_btAlpha(255),
m_nFadeTime(200),
m_bkImageObject(this),
m_stausImageObject(this),
m_iRotation(0),
m_bFadeEffectEnd(true),
m_aFadeEffectProperty(NONE),
m_nTooltipWidth(300)
{
    m_cXY.cx = m_cXY.cy = 0;
    m_cxyFixed.cx = m_cxyFixed.cy = 0;
    m_cxyMin.cx = m_cxyMin.cy = 0;
    m_cxyMax.cx = m_cxyMax.cy = 999999;
    m_cxyBorderRound.cx = m_cxyBorderRound.cy = 0;

    ::ZeroMemory(&m_rcPadding, sizeof(RECT));
    ::ZeroMemory(&m_rcItem, sizeof(RECT));
    ::ZeroMemory(&m_rcPaint, sizeof(RECT));
	::ZeroMemory(&m_rcBorderSize,sizeof(RECT));
    ::ZeroMemory(&m_tRelativePos, sizeof(TRelativePosUI));

    OnEvent += MakeDelegate(this, &CControlUI::OnEvent_Impl);
    OnNotify += MakeDelegate(this, &CControlUI::OnNotify_Impl);
}

CControlUI::~CControlUI()
{
    if( OnDestroy ) OnDestroy(this);
    if( m_pManager != NULL ) m_pManager->ReapObjects(this);

    DefaultEventSotryBoardMap::iterator itStoryboard = m_defaultEventStoryboardMap.begin();
    while (itStoryboard != m_defaultEventStoryboardMap.end())
    {
        if (itStoryboard->second)
            delete itStoryboard->second;
        itStoryboard++;
    }
    m_defaultEventStoryboardMap.clear();
}

CDuiString CControlUI::GetName() const
{
    return m_sName;
}

void CControlUI::SetName(LPCTSTR pstrName)
{
    m_sName = pstrName;
}

LPVOID CControlUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_CONTROL) == 0 ) return this;
    return NULL;
}

LPCTSTR CControlUI::GetClass() const
{
    return _T("ControlUI");
}

UINT CControlUI::GetControlFlags() const
{
    return 0;
}

bool CControlUI::Activate()
{
    if( !IsVisible() ) return false;
    if( !IsEnabled() ) return false;
    return true;
}

CPaintManagerUI* CControlUI::GetManager() const
{
    return m_pManager;
}

void CControlUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
{
    m_pManager = pManager;
    m_pParent = pParent;
    if( bInit && m_pParent )
	{
		if(m_pParent->GetInheritableTag()) SetInheritableTag(m_pParent->GetInheritableTag());
		if(!m_pParent->GetInheritableUserData().IsEmpty()) SetInheritableUserData(m_pParent->GetInheritableUserData());
		Init();
	}
}

CControlUI* CControlUI::GetParent() const
{
    return m_pParent;
}

CDuiString CControlUI::GetText() const
{
    return m_sText;
}

void CControlUI::SetText(LPCTSTR pstrText)
{
    if( m_sText == pstrText ) return;

    m_sText = pstrText;
    Invalidate();
}

DWORD CControlUI::GetBkColor() const
{
    return m_dwBackColor;
}

void CControlUI::SetBkColor(DWORD dwBackColor)
{
    if( m_dwBackColor == dwBackColor ) return;

    m_dwBackColor = dwBackColor;
    Invalidate();
}

DWORD CControlUI::GetBkColor2() const
{
    return m_dwBackColor2;
}

void CControlUI::SetBkColor2(DWORD dwBackColor)
{
    if( m_dwBackColor2 == dwBackColor ) return;

    m_dwBackColor2 = dwBackColor;
    Invalidate();
}

DWORD CControlUI::GetBkColor3() const
{
    return m_dwBackColor3;
}

void CControlUI::SetBkColor3(DWORD dwBackColor)
{
    if( m_dwBackColor3 == dwBackColor ) return;

    m_dwBackColor3 = dwBackColor;
    Invalidate();
}

LPCTSTR CControlUI::GetBkImage()
{
    return m_sBkImage;
}

void CControlUI::SetBkImage(LPCTSTR pStrImage)
{
    if( m_sBkImage == pStrImage ) return;

    m_sBkImage = pStrImage;
    Invalidate();
}

DWORD CControlUI::GetBorderColor() const
{
    return m_dwBorderColor;
}

void CControlUI::SetBorderColor(DWORD dwBorderColor)
{
    if( m_dwBorderColor == dwBorderColor ) return;

    m_dwBorderColor = dwBorderColor;
    Invalidate();
}

DWORD CControlUI::GetFocusBorderColor() const
{
    return m_dwFocusBorderColor;
}

void CControlUI::SetFocusBorderColor(DWORD dwBorderColor)
{
    if( m_dwFocusBorderColor == dwBorderColor ) return;

    m_dwFocusBorderColor = dwBorderColor;
    Invalidate();
}

bool CControlUI::IsColorHSL() const
{
    return m_bColorHSL;
}

void CControlUI::SetColorHSL(bool bColorHSL)
{
    if( m_bColorHSL == bColorHSL ) return;

    m_bColorHSL = bColorHSL;
    Invalidate();
}

int CControlUI::GetBorderSize() const
{
    return m_nBorderSize;
}

void CControlUI::SetBorderSize(int nSize)
{
    if( m_nBorderSize == nSize ) return;

    m_nBorderSize = nSize;
    Invalidate();
}

//************************************
// 函数名称: SetBorderSize
// 返回类型: void
// 参数信息: RECT rc
// 函数说明: 
//************************************
void CControlUI::SetBorderSize( RECT rc )
{
	m_rcBorderSize = rc;
	Invalidate();
}

SIZE CControlUI::GetBorderRound() const
{
    return m_cxyBorderRound;
}

void CControlUI::SetBorderRound(SIZE cxyRound)
{
    m_cxyBorderRound = cxyRound;
    Invalidate();
}

bool CControlUI::DrawImage(HDC hDC, LPCTSTR pStrImage, LPCTSTR pStrModify)
{
    return CRenderEngine::DrawImageString(hDC, m_pManager, m_rcItem, m_rcPaint, pStrImage, pStrModify);
}

QRect CControlUI::GetPos()
{
    return m_rcItem;
}

void CControlUI::SetPos(RECT rc)
{
    if( rc.right < rc.left ) rc.right = rc.left;
    if( rc.bottom < rc.top ) rc.bottom = rc.top;

    CDuiRect invalidateRc = m_rcItem;
    if( ::IsRectEmpty(&invalidateRc) ) invalidateRc = rc;

    m_rcItem = rc;
    if( m_pManager == NULL ) return;

    if( !m_bSetPos ) {
        m_bSetPos = true;
        if( OnSize ) OnSize(this);
        m_bSetPos = false;
    }
    
    if( m_bFloat ) {
        CControlUI* pParent = GetParent();
        if( pParent != NULL ) {
            RECT rcParentPos = pParent->GetPos();
            if( m_cXY.cx >= 0 ) m_cXY.cx = m_rcItem.left - rcParentPos.left;
            else m_cXY.cx = m_rcItem.right - rcParentPos.right;
            if( m_cXY.cy >= 0 ) m_cXY.cy = m_rcItem.top - rcParentPos.top;
            else m_cXY.cy = m_rcItem.bottom - rcParentPos.bottom;
            m_cxyFixed.cx = m_rcItem.right - m_rcItem.left;
            m_cxyFixed.cy = m_rcItem.bottom - m_rcItem.top;
        }
    }

    m_bUpdateNeeded = false;
    invalidateRc.Join(m_rcItem);

    CControlUI* pParent = this;
    RECT rcTemp;
    RECT rcParent;
    while( pParent = pParent->GetParent() )
    {
        rcTemp = invalidateRc;
        rcParent = pParent->GetPos();
        if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
        {
            return;
        }
    }
    m_pManager->Invalidate(invalidateRc);
}

int CControlUI::GetWidth() const
{
    return m_rcItem.right - m_rcItem.left;
}

int CControlUI::GetHeight() const
{
    return m_rcItem.bottom - m_rcItem.top;
}

int CControlUI::GetX() const
{
    return m_rcItem.left;
}

int CControlUI::GetY() const
{
    return m_rcItem.top;
}

RECT CControlUI::GetPadding() const
{
    return m_rcPadding;
}

void CControlUI::SetPadding(RECT rcPadding)
{
    m_rcPadding = rcPadding;
    NeedParentUpdate();
}

SIZE CControlUI::GetFixedXY() const
{
    return m_cXY;
}

void CControlUI::SetFixedXY(SIZE szXY)
{
    m_cXY.cx = szXY.cx;
    m_cXY.cy = szXY.cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetFixedWidth() const
{
    return m_cxyFixed.cx;
}

void CControlUI::SetFixedWidth(int cx)
{
    if( cx < 0 ) return; 
    m_cxyFixed.cx = cx;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetFixedHeight() const
{
    return m_cxyFixed.cy;
}

void CControlUI::SetFixedHeight(int cy)
{
    if( cy < 0 ) return; 
    m_cxyFixed.cy = cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMinWidth() const
{
    return m_cxyMin.cx;
}

void CControlUI::SetMinWidth(int cx)
{
    if( m_cxyMin.cx == cx ) return;

    if( cx < 0 ) return; 
    m_cxyMin.cx = cx;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMaxWidth() const
{
    return m_cxyMax.cx;
}

void CControlUI::SetMaxWidth(int cx)
{
    if( m_cxyMax.cx == cx ) return;

    if( cx < 0 ) return; 
    m_cxyMax.cx = cx;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMinHeight() const
{
    return m_cxyMin.cy;
}

void CControlUI::SetMinHeight(int cy)
{
    if( m_cxyMin.cy == cy ) return;

    if( cy < 0 ) return; 
    m_cxyMin.cy = cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMaxHeight() const
{
    return m_cxyMax.cy;
}

void CControlUI::SetMaxHeight(int cy)
{
    if( m_cxyMax.cy == cy ) return;

    if( cy < 0 ) return; 
    m_cxyMax.cy = cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

void CControlUI::SetRelativePos(SIZE szMove,SIZE szZoom)
{
    m_tRelativePos.bRelative = TRUE;
    m_tRelativePos.nMoveXPercent = szMove.cx;
    m_tRelativePos.nMoveYPercent = szMove.cy;
    m_tRelativePos.nZoomXPercent = szZoom.cx;
    m_tRelativePos.nZoomYPercent = szZoom.cy;
}

void CControlUI::SetRelativeParentSize(SIZE sz)
{
    m_tRelativePos.szParent = sz;
}

TRelativePosUI CControlUI::GetRelativePos() const
{
    return m_tRelativePos;
}

bool CControlUI::IsRelativePos() const
{
    return m_tRelativePos.bRelative;
}

CDuiString CControlUI::GetToolTip() const
{
    return m_sToolTip;
}

void CControlUI::SetToolTip(LPCTSTR pstrText)
{
	CDuiString strTemp(pstrText);
	strTemp.Replace(_T("<n>"),_T("\r\n"));
	m_sToolTip=strTemp;
}

void CControlUI::SetToolTipWidth( int nWidth )
{
	m_nTooltipWidth=nWidth;
}

int CControlUI::GetToolTipWidth( void )
{
	return m_nTooltipWidth;
}

TCHAR CControlUI::GetShortcut() const
{
    return m_chShortcut;
}

void CControlUI::SetShortcut(TCHAR ch)
{
    m_chShortcut = ch;
}

bool CControlUI::IsContextMenuUsed() const
{
    return m_bMenuUsed;
}

void CControlUI::SetContextMenuUsed(bool bMenuUsed)
{
    m_bMenuUsed = bMenuUsed;
}

const CDuiString& CControlUI::GetUserData()
{
    return m_sUserData;
}

void CControlUI::SetUserData(LPCTSTR pstrText)
{
    m_sUserData = pstrText;
}

UINT_PTR CControlUI::GetTag() const
{
    return m_pTag;
}

void CControlUI::SetTag(UINT_PTR pTag)
{
    m_pTag = pTag;
}

void CControlUI::SetInheritableUserData(LPCTSTR szUserData)
{
	m_sInheritableUserData = szUserData;
}

const CDuiString& CControlUI::GetInheritableUserData()
{
	return m_sInheritableUserData;
}

void CControlUI::SetInheritableTag(UINT_PTR pTag)
{
	m_pInheritableTag = pTag;
}

UINT_PTR CControlUI::GetInheritableTag()
{
	return m_pInheritableTag;
}

bool CControlUI::GetVisible() const
{
	return m_bVisible;
}

bool CControlUI::IsVisible() const
{

    return m_bVisible && m_bInternVisible;
}

void CControlUI::SetVisible(bool bVisible)
{
    if( m_bVisible == bVisible ) return;

    bool v = IsVisible();
    m_bVisible = bVisible;
    if( m_bFocused ) m_bFocused = false;
	if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
		m_pManager->SetFocus(NULL, false) ;
	}
    if( IsVisible() != v ) {
        NeedParentUpdate();
    }
}

void CControlUI::SetInternVisible(bool bVisible)
{
    m_bInternVisible = bVisible;
	if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
		m_pManager->SetFocus(NULL, false) ;
	}
}

bool CControlUI::IsEnabled() const
{
    return m_bEnabled;
}

void CControlUI::SetEnabled(bool bEnabled)
{
    if( m_bEnabled == bEnabled ) return;

    m_bEnabled = bEnabled;
    Invalidate();
}

bool CControlUI::IsMouseEnabled() const
{
    return m_bMouseEnabled;
}

void CControlUI::SetMouseEnabled(bool bEnabled)
{
    m_bMouseEnabled = bEnabled;
}

bool CControlUI::IsKeyboardEnabled() const
{
	return m_bKeyboardEnabled ;
}
void CControlUI::SetKeyboardEnabled(bool bEnabled)
{
	m_bKeyboardEnabled = bEnabled ; 
}

bool CControlUI::IsFocused() const
{
    return m_bFocused;
}

void CControlUI::SetFocus()
{
    if( m_pManager != NULL ) m_pManager->SetFocus(this);
}

bool CControlUI::IsFloat() const
{
    return m_bFloat;
}

void CControlUI::SetFloat(bool bFloat)
{
    if( m_bFloat == bFloat ) return;

    m_bFloat = bFloat;
    NeedParentUpdate();
}

CControlUI* CControlUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
    if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
    if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
	if( (uFlags & UIFIND_HITTEST) != 0 && (!m_bMouseEnabled || !::PtInRect(&m_rcItem, * static_cast<LPPOINT>(pData))) ) return NULL;
    return Proc(this, pData);
}

void CControlUI::Invalidate()
{
    if( !IsVisible() ) return;

    RECT invalidateRc = m_rcItem;

    CControlUI* pParent = this;
    RECT rcTemp;
    RECT rcParent;
    while( pParent = pParent->GetParent() )
    {
        rcTemp = invalidateRc;
        rcParent = pParent->GetPos();
        if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
        {
            return;
        }
    }

    if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
}

bool CControlUI::IsUpdateNeeded() const
{
    return m_bUpdateNeeded;
}

void CControlUI::NeedUpdate()
{
    if( !IsVisible() ) return;
    m_bUpdateNeeded = true;
    Invalidate();

    if( m_pManager != NULL ) m_pManager->NeedUpdate();
}

void CControlUI::NeedParentUpdate()
{
    if( GetParent() ) {
        GetParent()->NeedUpdate();
        GetParent()->Invalidate();
    }
    else {
        NeedUpdate();
    }

    if( m_pManager != NULL ) m_pManager->NeedUpdate();
}

DWORD CControlUI::GetAdjustColor(DWORD dwColor)
{
    if( !m_bColorHSL ) return dwColor;
    short H, S, L;
    CPaintManagerUI::GetHSL(&H, &S, &L);
    return CRenderEngine::AdjustColor(dwColor, H, S, L);
}

void CControlUI::Init()
{
    DoInit();
    if( OnInit ) OnInit(this);
}

void CControlUI::DoInit()
{
    DoDefaultAnimEventHandlerInit();
}

void CControlUI::Event(TEventUI& event)
{
	if (OnEvent(&event))
		DoEvent(event);
}

void CControlUI::DoEvent(TEventUI& event)
{
    if( event.Type == UIEVENT_SETCURSOR )
    {
        ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
        return;
    }
    if( event.Type == UIEVENT_SETFOCUS ) 
    {
        m_bFocused = true;
        Invalidate();
        return;
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        m_bFocused = false;
        Invalidate();
        return;
    }
    if( event.Type == UIEVENT_TIMER )
    {
        m_pManager->SendNotify(this, DUI_MSGTYPE_TIMER, event.wParam, event.lParam);
        return;
    }
    if( event.Type == UIEVENT_CONTEXTMENU )
    {
        if( IsContextMenuUsed() ) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
            return;
        }
    }
    if( m_pParent != NULL ) m_pParent->DoEvent(event);
}


void CControlUI::SetVirtualWnd(LPCTSTR pstrValue)
{
	m_sVirtualWnd = pstrValue;
	m_pManager->UsedVirtualWnd(true);
}

CDuiString CControlUI::GetVirtualWnd() const
{
	CDuiString str;
	if( !m_sVirtualWnd.IsEmpty() ){
		str = m_sVirtualWnd;
	}
	else{
		CControlUI* pParent = GetParent();
		if( pParent != NULL){
			str = pParent->GetVirtualWnd();
		}
		else{
			str = _T("");
		}
	}
	return str;
}

class CXmlAnimInfo
{
public:
    CXmlAnimInfo(QUI_STRING strValue)
    {
        m_iDuration = 0;
        m_iTweentype = 0;
        m_bRelative = false;
        m_iAxis = 0;
        m_bRecalculate = false;
        Parse(strValue);
    }
    ~CXmlAnimInfo() {}
    CAnimator* CreateAnimator(IAnimObject* pControl)
    {
        CAnimator* pAnimator = NULL;
        if (m_strAnimType == _T("scale"))
        {
            pAnimator = new CScaleAnimator(pControl, (CTween::Type)m_iTweentype, m_iDuration, m_from, m_to, (E_AXIS)m_iAxis, m_bRelative, m_bRecalculate);
        }
        else if (m_strAnimType == _T("fade"))
        {
            pAnimator = new CFadeAnimator(pControl, (CTween::Type)m_iTweentype, m_iDuration, m_from.x, m_to.x, m_bRelative);
        }
        else if (m_strAnimType == _T("move"))
        {
            pAnimator = new CMoveAnimator(pControl, (CTween::Type)m_iTweentype, m_iDuration, m_from, m_to, m_bRelative, m_bRecalculate);
        }
        else if (m_strAnimType == _T("rotation"))
        {
            pAnimator = new CRotationAnimator(pControl, (CTween::Type)m_iTweentype, m_iDuration, m_from.x, m_to.x, m_bRelative);
        }
        else if (m_strAnimType == _T("property"))
        {
            pAnimator = new CPropertyAnimatior(pControl, (CTween::Type)m_iTweentype, m_iDuration, m_strPropertyName.GetData(), m_from.x, m_to.x, m_bRelative);
        }
        return pAnimator;
    }
    void Parse(QUI_STRING str)
    {
        std::vector<QUI_STRING> arr;
        SplitStringAlongWhitespace(str, &arr);
        for (int i = 0; i < arr.size(); i++)
        {
            std::vector<QUI_STRING> arrValues;
            SplitString(arr[i], L'=', arrValues);
            if (arrValues.size() > 0)
            {
                QUI_STRING strKey = arrValues[0];
                int iBegin = arrValues[1].find_first_of(L'\'');
                int iEnd = arrValues[1].find_last_of(L'\'');
                if (iBegin == QUI_STRING::npos || iEnd == QUI_STRING::npos)
                    continue;
                QUI_STRING strValue = arrValues[1].substr(iBegin + 1, iEnd - iBegin - 1);
                if (strKey == _T("type"))
                {
                    m_strAnimType = strValue.c_str();
                }
                else if (strKey == _T("duration"))
                {
                    m_iDuration = _ttoi(strValue.c_str());
                }
                else if (strKey == _T("tweentype"))
                {
                    m_iTweentype = _ttoi(strValue.c_str());
                }
                else if (strKey == _T("from"))
                {
                    std::vector<QUI_STRING> arrPos;
                    SplitString(strValue, L',', arrPos);
                    if (arrPos.size() > 0)
                    {
                        if (arrPos[0].find(_T("0x")) == QUI_STRING::npos)
                            m_from.x = _ttoi(arrPos[0].c_str());
                        else
                            m_from.x = ANIM_CURRENT_VALUE;
                    }
                    if (arrPos.size() > 1)
                    {
                        if (arrPos[1].find(_T("0x")) == QUI_STRING::npos)
                            m_from.y = _ttoi(arrPos[1].c_str());
                        else
                            m_from.y = ANIM_CURRENT_VALUE;
                    }
                }
                else if (strKey == _T("to"))
                {
                    std::vector<QUI_STRING> arrPos;
                    SplitString(strValue, L',', arrPos);
                    if (arrPos.size() > 0)
                    {
                        if (arrPos[0].find(_T("0x")) == QUI_STRING::npos)
                            m_to.x = _ttoi(arrPos[0].c_str());
                        else
                            m_to.x = ANIM_CURRENT_VALUE;
                    }
                    if (arrPos.size() > 1)
                    {
                        if (arrPos[1].find(_T("0x")) == QUI_STRING::npos)
                            m_to.y = _ttoi(arrPos[1].c_str());
                        else
                            m_to.y = ANIM_CURRENT_VALUE;
                    }
                }
                else if (strKey == _T("relative"))
                {
                    m_bRelative = (_ttoi(strValue.c_str()) == 1);
                }
                else if (strKey == _T("axis"))
                {
                    m_iAxis = _ttoi(strValue.c_str());
                }
                else if (strKey == _T("recalculate"))
                {
                    m_bRecalculate = _ttoi(strValue.c_str()) == 1;
                }
                else if (strKey == _T("propertyname"))
                {
                    m_strPropertyName = strValue.c_str();
                }
            }
        }
    }
    friend class CControlUI;
private:
    CStdString m_strValue;
    CStdString m_strAnimType;
    int        m_iDuration;
    int        m_iTweentype;
    QPoint     m_from;
    QPoint     m_to;
    bool       m_bRelative;
    CStdString m_strPropertyName;
    int        m_iAxis;
    bool       m_bRecalculate;
};

bool IsDescendant(CControlUI* pAsc, CControlUI* pDesc) {
    while (pDesc != NULL  && pDesc->GetParent() != NULL && pDesc->GetParent() != pAsc)
        pDesc = pDesc->GetParent();
    
    if (pDesc == pAsc || (pDesc != NULL && pDesc->GetParent() == pAsc))
        return true;
    return false;
}

bool CControlUI::OnEvent_Impl(void* pEvent) {
    TEventUI* p = (TEventUI*)pEvent;
    if (!pEvent || !p) return true;

    CControlUI* pSender = p->pSender;
    if (!pSender) return true;

    if (p->Type == UIEVENT_MOUSEENTER) {
        p->pSender = this;
        TNotifyUI notify;
        notify.pSender = this;
        notify.sType = DUI_MSGTYPE_FOCUS_IN;
        return FireEvent(DUI_MSGTYPE_FOCUS_IN, &notify);
    }
    else if (p->Type == UIEVENT_MOUSELEAVE) {
        p->pSender = this;
        if (pSender == this)
        {
            TNotifyUI notify;
            notify.pSender = this;
            notify.sType = DUI_MSGTYPE_FOCUS_OUT;
            return FireEvent(DUI_MSGTYPE_FOCUS_OUT, &notify);
        }
        else if (pSender->GetParent() == this) {
            CControlUI* pNewHover = (CControlUI*)p->wParam;
            if (!::PtInRect(&m_rcItem, p->ptMouse) || (::PtInRect(&m_rcItem, p->ptMouse) && !IsDescendant(this, pNewHover))) {
                TNotifyUI notify;
                notify.pSender = this;
                notify.sType = DUI_MSGTYPE_FOCUS_OUT;
                bool bRet = FireEvent(DUI_MSGTYPE_FOCUS_OUT, &notify);
                return bRet;
            }
            else {
                if (::PtInRect(&m_rcItem, p->ptMouse)) {
                    return false;
                }
            }
        }
        else {
            return true;
        }
    }
    return true;
}

bool CControlUI::OnNotify_Impl(void* pEvent) {
    TNotifyUI* p = (TNotifyUI*)pEvent;
    if (!pEvent || !p) return true;
    return FireEvent(p->sType.GetData(), pEvent);
}

void CControlUI::SetBKBitmap(HBITMAP hBmp)
{
    SetBkBitmap(hBmp);
}

void CControlUI::SetPosition(int x, int y, bool bRelative/* = false*/)
{
    if (m_bFloat)//not support relative pos
    {
        QRect rectParent;
        if (GetParent() != NULL && bRelative)
        {
            CControlUI* pParent = GetParent();
            rectParent = pParent->GetPos();
        }
        SIZE size = { x, y };
        SetFixedXY(size);
        RECT rectItem = GetPos();
        QRect rect = rectItem;

        rectItem.left = rectParent.left + x;
        rectItem.top = rectParent.top + y;
        rectItem.right = rectItem.left + rect.GetWidth();
        rectItem.bottom = rectItem.top + rect.GetHeight();
        SetPos(rectItem);
    }
    else
    {
        RECT rectItem = GetPos();
        QRect rect = rectItem;
        rectItem.left = x;
        rectItem.top = y;
        rectItem.right = x + rect.GetWidth();
        rectItem.bottom = y + rect.GetHeight();
        SetPos(rectItem);
    }
}

void CControlUI::SetSize(int iWidth, int iHeight)
{
    RECT rectItem = GetPos();
    //if(!m_bFloat)
    {
        m_cxyFixed.cx = iWidth >= 0 ? iWidth : 0;
        m_cxyFixed.cy = iHeight >= 0 ? iHeight : 0;
    }
    if (iWidth >= 0)
    {
        rectItem.right = rectItem.left + iWidth;
    }
    else
    {
        rectItem.right = rectItem.left;
    }
    if (iHeight >= 0)
    {
        rectItem.bottom = rectItem.top + iHeight;
    }
    else
    {
        rectItem.bottom = rectItem.top;
    }
    SetPos(rectItem);
}

void CControlUI::SetKeyWithValue(QUI_STRING strKeyName, int iValue)
{
    RECT rcPos = GetPos();
    if (strKeyName == _T("x"))
    {
        SetPosition(iValue, rcPos.top);
    }
    else if (strKeyName == _T("y"))
    {
        SetPosition(rcPos.left, iValue);
    }
    else if (strKeyName == _T("width"))
    {
        SetSize(iValue, rcPos.bottom - rcPos.top);
    }
    else if (strKeyName == _T("height"))
    {
        SetSize(rcPos.right - rcPos.left, iValue);
    }
    else if (strKeyName == _T("alpha"))
    {
        SetTransparent(iValue);
        Invalidate();
    }
    else if (strKeyName == _T("rotation"))
    {
        SetRotation(iValue);
        Invalidate();
    }
}

IAnimObject* CControlUI::BkImageAnimObject()
{
    return (IAnimObject*)&m_bkImageObject;
}

IAnimObject* CControlUI::StatusImageAnimObject()
{
    return (IAnimObject*)&m_stausImageObject;
}

bool CControlUI::DefaultAnimEventHandler(void* p)
{
    TNotifyUI *pNotify = (TNotifyUI *)p;
    if (!pNotify) return true;
    DefaultEventSotryBoardMap::iterator it = m_defaultEventStoryboardMap.begin();
    while (it != m_defaultEventStoryboardMap.end())
    {
        if (it->first != pNotify->sType)
        {
            it->second->Stop();
        }
        it++;
    }
    if (m_defaultEventStoryboardMap[pNotify->sType])
        m_defaultEventStoryboardMap[pNotify->sType]->Start();
    return true;
}

void CControlUI::DoDefaultAnimEventHandlerInit()
{
    CustomAttributeMap::iterator it = m_customAttributeMap.begin();
    while (it != m_customAttributeMap.end())
    {
        if (it->first == Item_Hot.c_str() ||
            it->first == Item_Not_Hot.c_str() ||
            it->first == _T("focusin") ||
            it->first == _T("focusout"))
        {
            QUI_STRING strAnimValue = it->second.GetData();
            std::vector<QUI_STRING> arrValues;
            SplitString(strAnimValue, L',', arrValues);
            int iCnt = arrValues.size();
            if (iCnt > 0)
            {
                bool bNeedSubscribe = false;
                for (int i = 0; i < arrValues.size(); i++)
                {
                    QUI_STRING strName = arrValues[i];
                    QUI_STRING strAnimNameValue = m_customAttributeMap[strName.c_str()];
                    if (!strAnimNameValue.empty())
                    {
                        CXmlAnimInfo info(strAnimNameValue);
                        CAnimator* pAnimator = info.CreateAnimator(this);
                        if (pAnimator)
                        {
                            bNeedSubscribe = true;
                            if (m_defaultEventStoryboardMap[it->first])
                            {
                                m_defaultEventStoryboardMap[it->first]->AddAnimator(i, pAnimator, 0);
                            }
                            else
                            {
                                m_defaultEventStoryboardMap[it->first] = new CStoryBoard(((CStdString)it->first).GetData());
                                m_defaultEventStoryboardMap[it->first]->AddAnimator(i, pAnimator, 0);
                            }
                        }
                    }
                }
                if (bNeedSubscribe)
                {
                    this->Subscribe(((CStdString)it->first).GetData(), MakeDelegate(this, &CControlUI::DefaultAnimEventHandler));
                }
            }
        }

        it++;
    }
}

QUI_STRING CControlUI::GetDefaultStoryboardName()
{
    return DEFAULT_CONTROL_STORYBOARD;
}

void CControlUI::SetFadeEffectProperty(int nProperty)
{
    m_aFadeEffectProperty = nProperty;
}

void CControlUI::SetFadeEffectProperty(LPCTSTR pProperty)
{
    CStdString strProperty = pProperty;
    if (strProperty == _T("false"))
    {
        m_aFadeEffectProperty = NONE;
        return;
    }
    else if (strProperty == _T("true"))
    {
        m_aFadeEffectProperty |= STATUSIMAGE;
        return;
    }
    int nPos = strProperty.Find(_T(";"));
    CStdString strFadeProperty = strProperty;
    if (nPos != -1)
    {
        strFadeProperty = strProperty.Mid(0, nPos);
        CStdString strFadeTime = strProperty.Mid(nPos + 1, strProperty.GetLength() - nPos - 1);
        m_nFadeTime = _wtoi(strFadeTime);
    }
    nPos = strFadeProperty.Find(_T(","));
    while (nPos != -1)
    {
        CStdString strValue = strFadeProperty.Mid(0, nPos);
        m_aFadeEffectProperty |= GetEffectProperty(strValue);
        strFadeProperty = strFadeProperty.Mid(nPos + 1, strFadeProperty.GetLength() - nPos - 1);
        nPos = strFadeProperty.Find(_T(","));
    }
    if (nPos == -1)
    {
        CStdString strValue = strFadeProperty.Mid(0, strFadeProperty.GetLength());
        m_aFadeEffectProperty |= GetEffectProperty(strValue);
    }
}

bool CControlUI::OnFocusInAnimationEnd()
{
    m_bFadeEffectEnd = true;
    return true;
}

bool CControlUI::OnFocusOutAnimationEnd()
{
    return true;
}

bool CControlUI::OnFocusInFadeEffectEnd(void* p)
{
    return OnFocusInAnimationEnd();
}

bool CControlUI::OnFocusOutFadeEffectEnd(void* p)
{
    return OnFocusOutAnimationEnd();
}

int CControlUI::GetFadeEffectProperty() const
{
    return m_aFadeEffectProperty;
}

CControlUI::EffectProperty CControlUI::GetEffectProperty(LPCTSTR pProperty)
{
    if (_tcscmp(pProperty, _T("bkcolor")) == 0) return BKCOLOR;
    else if (_tcscmp(pProperty, _T("bkimage")) == 0) return BKIMAGE;
    else if (_tcscmp(pProperty, _T("statusimage")) == 0) return STATUSIMAGE;
    else if (_tcscmp(pProperty, _T("text")) == 0) return TEXT;
    else if (_tcscmp(pProperty, _T("border")) == 0) return BORDER;
    else if (_tcscmp(pProperty, _T("all")) == 0) return ALL;

    return NONE;
}

void CControlUI::EffectPaint(EffectProperty aProperty, HDC hDC)
{
    switch (aProperty)
    {
    case BKCOLOR:
        PaintBkColor(hDC);
        break;
    case BKIMAGE:
        PaintBkImage(hDC);
        break;
    case STATUSIMAGE:
        PaintStatusImage(hDC);
        break;
    case TEXT:
        PaintText(hDC);
        break;
    case BORDER:
        PaintBorder(hDC);
        break;
    }
}

void CControlUI::EffectPaint(EffectProperty aProperty, HDC hDC, CDrawingBoard& aBoard)
{
    if ((m_aFadeEffectProperty & aProperty) != 0)
    {
        HDC hBoardDC = aBoard.GetSafeHdc();
        RECT myPaintRect = { 0, 0, RECT_WIDTH(m_rcPaint), RECT_HEIGHT(m_rcPaint) };
        RECT myItemRect = { 0, 0, RECT_WIDTH(m_rcItem), RECT_HEIGHT(m_rcItem) };
        RECT oldItemRect = m_rcItem;
        m_rcItem = myItemRect;
        RECT oldPaintRect = m_rcPaint;
        m_rcPaint = myPaintRect;

        if (m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0)
        {
            CRenderClip roundClip;
            CRenderClip::GenerateRoundClip(hBoardDC, m_rcPaint, m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
            EffectPaint(aProperty, hBoardDC);
        }
        else
        {
            EffectPaint(aProperty, hBoardDC);
        }
        aBoard.AlphaBlendTo(hDC, oldItemRect, m_rcItem, (int)m_btAlpha);

        m_rcItem = oldItemRect;
        m_rcPaint = oldPaintRect;
    }
    else
    {
        if (m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0)
        {
            CRenderClip roundClip;
            CRenderClip::GenerateRoundClip(hDC, m_rcPaint, m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
            EffectPaint(aProperty, hDC);
        }
        else
            EffectPaint(aProperty, hDC);
    }
}

//#define ROTATION_EFFECT 1
//#define ALPHA_EFFECT    2
//void CControlUI::ApplyEffect(CDrawingBoard& board, HDC hDc, int iType, int iValue)
//{
//
//}

void CControlUI::DoAlphaPaint(HDC hDC, const RECT& rcPaint)
{
    CDrawingBoard board;
#if 0
    CRect rcClient;
    ::GetClientRect(m_pManager->GetPaintWindow(), &rcClient);
    board.Create(RECT_WIDTH(rcClient), RECT_HEIGHT(rcClient));
#else
    int iWidth = RECT_WIDTH(m_rcItem);
    int iHeight = RECT_HEIGHT(m_rcItem);
    board.Create(iWidth, iHeight);
#endif

    //等加入bkcolor、text、border object之后m_bFadeEffectEnd就可以去掉了
    if (!m_bFadeEffectEnd)
    {
        if (m_cxyBorderRound.cx > 0 && m_cxyBorderRound.cy > 0)
        {
            HDC hBoardDC = board.GetSafeHdc();
            RECT myPaintRect = { 0, 0, RECT_WIDTH(m_rcPaint), RECT_HEIGHT(m_rcPaint) };
            RECT myItemRect = { 0, 0, RECT_WIDTH(m_rcItem), RECT_HEIGHT(m_rcItem) };
            RECT oldItemRect = m_rcItem;
            RECT oldPaintRect = m_rcPaint;
            m_rcItem = myItemRect;
            m_rcPaint = myPaintRect;

            EffectPaint(BKCOLOR, hBoardDC);
            EffectPaint(BKIMAGE, hBoardDC);
            EffectPaint(STATUSIMAGE, hBoardDC);
            EffectPaint(TEXT, hBoardDC);
            //EffectPaint(BORDER, hBoardDC);

            DWORD dwBorderColor = (IsFocused() && m_dwFocusBorderColor != 0) ? GetAdjustColor(m_dwFocusBorderColor) : GetAdjustColor(m_dwBorderColor);

            HBITMAP hbmp = board.GetBmpHandle();
            CRenderEngine::DrawRoundRectAntiAlias(hbmp, hDC, oldItemRect.left, oldItemRect.top, m_cxyBorderRound.cx, m_cxyBorderRound.cy, m_nBorderSize, dwBorderColor, m_btAlpha);

            m_rcItem = oldItemRect;
            m_rcPaint = oldPaintRect;
        }
        else
        {
            EffectPaint(BKCOLOR, hDC, board);
            EffectPaint(BKIMAGE, hDC, board);
            EffectPaint(STATUSIMAGE, hDC, board);
            EffectPaint(TEXT, hDC, board);
            EffectPaint(BORDER, hDC, board);
        }

    }
    else
    {
        RECT myPaintRect = { 0, 0, RECT_WIDTH(m_rcPaint), RECT_HEIGHT(m_rcPaint) };
        RECT myItemRect = { 0, 0, RECT_WIDTH(m_rcItem), RECT_HEIGHT(m_rcItem) };

        RECT oldItemRect = m_rcItem;
        RECT oldPaintRect = m_rcPaint;

        bool bNeedAllPainted = false;
        HDC hDcToDraw = hDC;

        if (m_iRotation != 0 || (m_btAlpha > 0 && m_btAlpha < 255))
        {
            bNeedAllPainted = true;
            hDcToDraw = board.GetSafeHdc();
            m_rcItem = myItemRect;
            m_rcPaint = myPaintRect;
        }
        PaintBkColor(hDcToDraw);
        if (m_bkImageObject.IsAttachedToAnimator())
        {
            if (!bNeedAllPainted)
            {
                m_rcItem = myItemRect;
                m_rcPaint = myPaintRect;
            }

            CDrawingBoard boardTemp;
            boardTemp.Create(iWidth, iHeight);

            CDrawingBoard boardToRotate, boardToDraw;

            RECT dstRect = { 0,0, iWidth, iHeight };
            PaintBkImage(boardTemp.GetSafeHdc());
            if (m_bkImageObject.m_iRotation > 0)
            {
                boardToRotate.Create(iWidth, iHeight);
                boardTemp.RotateTo(boardToRotate.GetSafeHdc(), dstRect, m_bkImageObject.m_iRotation);
                boardToDraw = boardToRotate;
            }
            else
            {
                boardToDraw = boardTemp;
            }

            RECT rectToDraw;
            if (bNeedAllPainted)
                rectToDraw = dstRect;
            else
                rectToDraw = oldItemRect;
            ::OffsetRect(&rectToDraw, m_stausImageObject.m_rcItem.left, m_stausImageObject.m_rcItem.top);
            int iItemWidth = m_stausImageObject.GetPos().GetWidth();
            int iItemHeight = m_stausImageObject.GetPos().GetHeight();
            rectToDraw.right = rectToDraw.left + iItemWidth;
            rectToDraw.bottom = rectToDraw.top + iItemHeight;
            if (m_stausImageObject.m_iAlpha > 0 && m_stausImageObject.m_iAlpha < 255)
                boardToDraw.AlphaBlendTo(hDcToDraw, rectToDraw, dstRect, m_stausImageObject.m_iAlpha);
            else
                boardToDraw.AlphaBlendTo(hDcToDraw, rectToDraw, dstRect, 254, true);
            boardToDraw.Reset();
            if (!bNeedAllPainted)
            {
                m_rcItem = oldItemRect;
                m_rcPaint = oldPaintRect;
            }
        }
        else
        {
            PaintBkImage(hDcToDraw);
        }
        if (m_stausImageObject.IsAttachedToAnimator())
        {
            if (!bNeedAllPainted)
            {
                m_rcItem = myItemRect;
                m_rcPaint = myPaintRect;
            }

            CDrawingBoard boardTemp;
            boardTemp.Create(iWidth, iHeight);

            CDrawingBoard boardToRotate, boardToDraw;

            RECT dstRect = { 0,0, iWidth, iHeight };
            PaintStatusImage(boardTemp.GetSafeHdc());
            if (m_stausImageObject.m_iRotation > 0)
            {
                boardToRotate.Create(iWidth, iHeight);
                boardTemp.RotateTo(boardToRotate.GetSafeHdc(), dstRect, m_stausImageObject.m_iRotation);
                boardToDraw = boardToRotate;
            }
            else
            {
                boardToDraw = boardTemp;
            }
            RECT rectToDraw;
            if (bNeedAllPainted)
                rectToDraw = dstRect;
            else
                rectToDraw = oldItemRect;

            ::OffsetRect(&rectToDraw, m_stausImageObject.m_rcItem.left, m_stausImageObject.m_rcItem.top);
            int iItemWidth = m_stausImageObject.GetPos().GetWidth();
            int iItemHeight = m_stausImageObject.GetPos().GetHeight();
            rectToDraw.right = rectToDraw.left + iItemWidth;
            rectToDraw.bottom = rectToDraw.top + iItemHeight;

            if (m_stausImageObject.m_iAlpha > 0 && m_stausImageObject.m_iAlpha < 255)
                boardToDraw.AlphaBlendTo(hDcToDraw, rectToDraw, dstRect, m_stausImageObject.m_iAlpha);
            else
                boardToDraw.AlphaBlendTo(hDcToDraw, rectToDraw, dstRect, 254);
            boardToDraw.Reset();
            if (!bNeedAllPainted)
            {
                m_rcItem = oldItemRect;
                m_rcPaint = oldPaintRect;
            }
        }
        else
        {
            PaintStatusImage(hDcToDraw);
        }
        PaintText(hDcToDraw);


        if (m_cxyBorderRound.cx > 0 && m_cxyBorderRound.cy > 0)
        {
            DWORD dwBorderColor = (IsFocused() && m_dwFocusBorderColor != 0) ? GetAdjustColor(m_dwFocusBorderColor) : GetAdjustColor(m_dwBorderColor);
            if (m_iRotation != 0)
            {
                CDrawingBoard boardTemp;
                boardTemp.Create(iWidth, iHeight);
                RECT dstRect = { 0,0, iWidth, iHeight };
                board.RotateTo(boardTemp.GetSafeHdc(), dstRect, m_iRotation);

                HBITMAP hbmp = boardTemp.GetBmpHandle();
                CRenderEngine::DrawRoundRectAntiAlias(hbmp, hDC, oldItemRect.left, oldItemRect.top, m_cxyBorderRound.cx, m_cxyBorderRound.cy, m_nBorderSize, dwBorderColor, m_btAlpha);
            }
            else if (m_btAlpha > 0 && m_btAlpha < 255)
            {
                HBITMAP hbmp = board.GetBmpHandle();
                CRenderEngine::DrawRoundRectAntiAlias(hbmp, hDC, oldItemRect.left, oldItemRect.top, m_cxyBorderRound.cx, m_cxyBorderRound.cy, m_nBorderSize, dwBorderColor, m_btAlpha);
            }
        }
        else
        {
            PaintBorder(hDcToDraw);
            if (m_iRotation != 0)
            {
                CDrawingBoard boardTemp;
                boardTemp.Create(iWidth, iHeight);
                RECT dstRect = { 0,0, iWidth, iHeight };
                board.RotateTo(boardTemp.GetSafeHdc(), dstRect, m_iRotation);
                boardTemp.AlphaBlendTo(hDC, oldItemRect, m_rcItem, (int)m_btAlpha);
            }
            else if (m_btAlpha > 0 && m_btAlpha < 255)
            {
                board.AlphaBlendTo(hDC, oldItemRect, m_rcItem, (int)m_btAlpha);
            }
        }
        if (bNeedAllPainted)
        {
            m_rcItem = oldItemRect;
            m_rcPaint = oldPaintRect;
        }
    }

}

void CControlUI::StartFocusInFadeEffect()
{
    if (m_aFadeEffectProperty <= NONE)
        return;

    StopAllStoryBoards();
    SetTransparent(0);
    CStoryBoard *pStoryBoard = GetStoryBoard(_T("FadeInEffect_Internal"));
    if (pStoryBoard == NULL)
    {
        pStoryBoard = AddStoryBoard(_T("FadeInEffect_Internal"));
        CFadeAnimator* pAnimator = new CFadeAnimator(this, CTween::LINEAR, m_nFadeTime, 0, 255);

        pStoryBoard->AddAnimator(0, pAnimator, 0);
        pAnimator->Subscribe(EventAnimatorEnd, MakeDelegate(this, &CControlUI::OnFocusInFadeEffectEnd));
    }
    m_bFadeEffectEnd = false;
    pStoryBoard->Start();
}

void CControlUI::StartFocusOutFadeEffect()
{
    if (m_aFadeEffectProperty <= NONE)
        return;
    StopAllStoryBoards();
    SetTransparent(255);
    CStoryBoard *pStoryBoard = GetStoryBoard(_T("FadeOutEffect_Internal"));
    if (pStoryBoard == NULL)
    {
        pStoryBoard = AddStoryBoard(_T("FadeOutEffect_Internal"));
        CFadeAnimator* pAnimator = new CFadeAnimator(this, CTween::LINEAR, m_nFadeTime, 255, 0);
        pStoryBoard->AddAnimator(0, pAnimator, 0);
        pAnimator->Subscribe(EventAnimatorEnd, MakeDelegate(this, &CControlUI::OnFocusOutFadeEffectEnd));
    }
    m_bFadeEffectEnd = false;
    pStoryBoard->Start();
}

bool CControlUI::IsNeedUseEffect()
{
    if ((m_btAlpha < 255 && m_btAlpha >= 0) || (m_iRotation != 0) || m_bkImageObject.IsAttachedToAnimator() || m_stausImageObject.IsAttachedToAnimator())
        return true;
    return false;
}

void CControlUI::PaintToBoard(CDrawingBoard& aBoard, const RECT& rcPaint)
{
    RECT oldPaintRect = m_rcPaint;
    RECT oldItemRect = m_rcItem;

    RECT myPaintRect = { 0, 0, RECT_WIDTH(rcPaint), RECT_HEIGHT(rcPaint) };
    RECT myItemRect = { 0, 0, RECT_WIDTH(m_rcItem), RECT_HEIGHT(m_rcItem) };
    if (!::IntersectRect(&m_rcPaint, &myPaintRect, &myItemRect)) return;
    m_rcItem = myItemRect;
    m_rcPaint = myPaintRect;

    HDC hBoardDC = aBoard.GetSafeHdc();
    if (IsNeedUseEffect())
    {
        DoAlphaPaint(hBoardDC, rcPaint);
    }
    else
    {
        if (m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0)
        {
            CRenderClip roundClip;
            CRenderClip::GenerateRoundClip(hBoardDC, m_rcPaint, m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
            EffectPaint(BKCOLOR, hBoardDC);
            EffectPaint(BKIMAGE, hBoardDC);
            EffectPaint(STATUSIMAGE, hBoardDC);
            EffectPaint(TEXT, hBoardDC);
            EffectPaint(BORDER, hBoardDC);
        }
        else
        {
            EffectPaint(BKCOLOR, hBoardDC);
            EffectPaint(BKIMAGE, hBoardDC);
            EffectPaint(STATUSIMAGE, hBoardDC);
            EffectPaint(TEXT, hBoardDC);
            EffectPaint(BORDER, hBoardDC);
        }
    }


    m_rcItem = oldItemRect;
    m_rcPaint = oldPaintRect;
}

bool CControlUI::IsParentHasAlpha()
{
    CContainerUI *pTmp = (CContainerUI *)m_pParent;
    while (pTmp)
    {
        if (pTmp->GetTransparent() < 255 && pTmp->GetTransparent() > 0)
        {
            return true;
        }
        pTmp = (CContainerUI *)pTmp->GetParent();
    }
    return false;
}

void CControlUI::SetCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    m_customAttributeMap[pstrName] = pstrValue;
}

LPCTSTR CControlUI::GetCustomAttribute(LPCTSTR pstrName)
{
    CustomAttributeMap::iterator iter = m_customAttributeMap.find(pstrName);
    if (iter != m_customAttributeMap.end())
        return iter->second;
    else
        return NULL;
}

void CControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("pos")) == 0 ) {
        RECT rcPos = { 0 };
        LPTSTR pstr = NULL;
        rcPos.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcPos.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcPos.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SIZE szXY = {rcPos.left >= 0 ? rcPos.left : rcPos.right, rcPos.top >= 0 ? rcPos.top : rcPos.bottom};

        SetFixedXY(szXY);
		SetFixedWidth(rcPos.right - rcPos.left);
		SetFixedHeight(rcPos.bottom - rcPos.top);
    }
    else if( _tcscmp(pstrName, _T("relativepos")) == 0 ) {
        SIZE szMove,szZoom;
        LPTSTR pstr = NULL;
        szMove.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        szMove.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        szZoom.cx = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        szZoom.cy = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr); 
        SetRelativePos(szMove,szZoom);
    }
    else if( _tcscmp(pstrName, _T("padding")) == 0 ) {
        RECT rcPadding = { 0 };
        LPTSTR pstr = NULL;
        rcPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetPadding(rcPadding);
    }
    else if( _tcscmp(pstrName, _T("bkcolor")) == 0 || _tcscmp(pstrName, _T("bkcolor1")) == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("bkcolor2")) == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBkColor2(clrColor);
    }
    else if( _tcscmp(pstrName, _T("bkcolor3")) == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBkColor3(clrColor);
    }
    else if( _tcscmp(pstrName, _T("bordercolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBorderColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("focusbordercolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetFocusBorderColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("colorhsl")) == 0 ) SetColorHSL(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("bordersize")) == 0 ) {
		CDuiString nValue = pstrValue;
		if(nValue.Find(',') < 0)
		{
			SetBorderSize(_ttoi(pstrValue));
			RECT rcPadding = {0};
			SetBorderSize(rcPadding);
		}
		else
		{
			RECT rcPadding = { 0 };
			LPTSTR pstr = NULL;
			rcPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			SetBorderSize(rcPadding);
		}
	}
    else if (_tcscmp(pstrName, _T("fadeeffect")) == 0 || _tcscmp(pstrName, _T("animation")) == 0) SetFadeEffectProperty(pstrValue);
	else if( _tcscmp(pstrName, _T("leftbordersize")) == 0 ) SetLeftBorderSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("topbordersize")) == 0 ) SetTopBorderSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("rightbordersize")) == 0 ) SetRightBorderSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("bottombordersize")) == 0 ) SetBottomBorderSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("borderstyle")) == 0 ) SetBorderStyle(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("borderround")) == 0 ) {
        SIZE cxyRound = { 0 };
        LPTSTR pstr = NULL;
        cxyRound.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        cxyRound.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);     
        SetBorderRound(cxyRound);
    }
    else if( _tcscmp(pstrName, _T("bkimage")) == 0 ) SetBkImage(pstrValue);
	else if (_tcscmp(pstrName, _T("width")) == 0) SetFixedWidth(_ttoi(pstrValue));
	else if (_tcscmp(pstrName, _T("height")) == 0) SetFixedHeight(_ttoi(pstrValue));
	else if (_tcscmp(pstrName, _T("minwidth")) == 0) SetMinWidth(_ttoi(pstrValue));
	else if (_tcscmp(pstrName, _T("minheight")) == 0) SetMinHeight(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("maxwidth")) == 0 ) SetMaxWidth(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("maxheight")) == 0 ) SetMaxHeight(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("name")) == 0 ) SetName(pstrValue);
    else if( _tcscmp(pstrName, _T("text")) == 0 ) SetText(pstrValue);
    else if (_tcscmp(pstrName, _T("tag")) == 0) SetTag(_ttoi(pstrValue));
	else if (_tcscmp(pstrName, _T("xmltext")) == 0)
	{
		CDuiString strValue = pstrValue;
		int nPos = strValue.Find(_T('/'));
		if(nPos != -1)
		{
			CDuiString strParent = strValue.Left(nPos);
			CDuiString strID = strValue.Mid(nPos+1, strValue.GetLength() - nPos - 1);
			LPCTSTR pstrText = CXmlParser::Instance()->GetXmlText(strParent.GetData(), strID.GetData());
			SetText(pstrText);
		}
	}
	else if (_tcscmp(pstrName, _T("xmltooltip")) == 0)
	{


		CDuiString strValue = pstrValue;
		int nPos = strValue.Find(_T("/"));
		if(nPos != -1)
		{
			CDuiString strParent = strValue.Left(nPos);
			CDuiString strID = strValue.Mid(nPos + 1);
			LPCTSTR pstrText = CXmlParser::Instance()->GetXmlText(strParent, strID);
			SetToolTip(pstrText);
		}
	}
    else if( _tcscmp(pstrName, _T("tooltip")) == 0 ) SetToolTip(pstrValue);
    else if( _tcscmp(pstrName, _T("userdata")) == 0 ) SetUserData(pstrValue);
    else if( _tcscmp(pstrName, _T("enabled")) == 0 ) SetEnabled(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("mouse")) == 0 ) SetMouseEnabled(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("keyboard")) == 0 ) SetKeyboardEnabled(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("visible")) == 0 ) SetVisible(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("float")) == 0 ) SetFloat(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("shortcut")) == 0 ) SetShortcut(pstrValue[0]);
    else if( _tcscmp(pstrName, _T("menu")) == 0 ) SetContextMenuUsed(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("virtualwnd")) == 0 ) SetVirtualWnd(pstrValue);
    else {
        CDuiString str_name = pstrName;
        if (str_name == Item_Hot.c_str() ||
            str_name == Item_Not_Hot.c_str() ||
            str_name == _T("focusin") ||
            str_name == _T("focusout") ||
            str_name == _T("focus_out") ||
            str_name == _T("focus_in")) {
            m_customAttributeMap[pstrName] = pstrValue;
        }
    }
}

CControlUI* CControlUI::ApplyAttributeList(LPCTSTR pstrList)
{
    CDuiString sItem;
    CDuiString sValue;
    while( *pstrList != _T('\0') ) {
        sItem.Empty();
        sValue.Empty();
        while( *pstrList != _T('\0') && *pstrList != _T('=') ) {
            LPTSTR pstrTemp = ::CharNext(pstrList);
            while( pstrList < pstrTemp) {
                sItem += *pstrList++;
            }
        }
        ASSERT( *pstrList == _T('=') );
        if( *pstrList++ != _T('=') ) return this;
        ASSERT( *pstrList == _T('\"') );
        if( *pstrList++ != _T('\"') ) return this;
        while( *pstrList != _T('\0') && *pstrList != _T('\"') ) {
            LPTSTR pstrTemp = ::CharNext(pstrList);
            while( pstrList < pstrTemp) {
                sValue += *pstrList++;
            }
        }
        ASSERT( *pstrList == _T('\"') );
        if( *pstrList++ != _T('\"') ) return this;
        SetAttribute(sItem, sValue);
        if( *pstrList++ != _T(' ') ) return this;
    }
    return this;
}

SIZE CControlUI::EstimateSize(SIZE szAvailable)
{
    return m_cxyFixed;
}

void CControlUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

    if (IsNeedUseEffect())
    {
        DoAlphaPaint(hDC, rcPaint);
    }
    else {
        // 绘制循序：背景颜色->背景图->状态图->文本->边框
        if (m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0) {
            CRenderClip roundClip;
            CRenderClip::GenerateRoundClip(hDC, m_rcPaint, m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
            PaintBkColor(hDC);
            PaintBkImage(hDC);
            PaintStatusImage(hDC);
            PaintText(hDC);
            PaintBorder(hDC);
        }
        else {
            PaintBkColor(hDC);
            PaintBkImage(hDC);
            PaintStatusImage(hDC);
            PaintText(hDC);
            PaintBorder(hDC);
        }
    }
}

void CControlUI::PaintBkColor(HDC hDC)
{
    if( m_dwBackColor != 0 ) {
        if( m_dwBackColor2 != 0 ) {
            if( m_dwBackColor3 != 0 ) {
                RECT rc = m_rcItem;
                rc.bottom = (rc.bottom + rc.top) / 2;
                CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2), true, 8);
                rc.top = rc.bottom;
                rc.bottom = m_rcItem.bottom;
                CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(m_dwBackColor2), GetAdjustColor(m_dwBackColor3), true, 8);
            }
            else 
                CRenderEngine::DrawGradient(hDC, m_rcItem, GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2), true, 16);
        }
        else if (m_dwBackColor >= 0xFF000000) CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwBackColor), IsParentHasAlpha() || (m_btAlpha > 0 && m_btAlpha < 255));
        else CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(m_dwBackColor), IsParentHasAlpha() || (m_btAlpha > 0 && m_btAlpha < 255));
    }
}

void CControlUI::PaintBkImage(HDC hDC)
{
    if( m_sBkImage.IsEmpty() ) return;
    if( !DrawImage(hDC, (LPCTSTR)m_sBkImage) ) m_sBkImage.Empty();
}

void CControlUI::PaintStatusImage(HDC hDC)
{
    return;
}

void CControlUI::PaintText(HDC hDC)
{
    return;
}

void CControlUI::PaintBorder(HDC hDC)
{
	if(m_dwBorderColor != 0 || m_dwFocusBorderColor != 0)
	{
		if(m_nBorderSize > 0 && ( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ))//画圆角边框
		{
			if (IsFocused() && m_dwFocusBorderColor != 0)
				CRenderEngine::DrawRoundRect(hDC, m_rcItem, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy, GetAdjustColor(m_dwFocusBorderColor));
			else
				CRenderEngine::DrawRoundRect(hDC, m_rcItem, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy, GetAdjustColor(m_dwBorderColor));
		}
		else
		{
			if (IsFocused() && m_dwFocusBorderColor != 0 && m_nBorderSize > 0)
				CRenderEngine::DrawRect(hDC, m_rcItem, m_nBorderSize, GetAdjustColor(m_dwFocusBorderColor));
			else if(m_rcBorderSize.left > 0 || m_rcBorderSize.top > 0 || m_rcBorderSize.right > 0 || m_rcBorderSize.bottom > 0)
			{
				RECT rcBorder;

				if(m_rcBorderSize.left > 0){
					rcBorder		= m_rcItem;
					rcBorder.right	= m_rcItem.left;
					CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.left,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
				if(m_rcBorderSize.top > 0){
					rcBorder		= m_rcItem;
					rcBorder.bottom	= m_rcItem.top;
					CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.top,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
				if(m_rcBorderSize.right > 0){
					rcBorder		= m_rcItem;
					rcBorder.left	= m_rcItem.right;
					CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.right,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
				if(m_rcBorderSize.bottom > 0){
					rcBorder		= m_rcItem;
					rcBorder.top	= m_rcItem.bottom;
					CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.bottom,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
			}
			else if(m_nBorderSize > 0)
				CRenderEngine::DrawRect(hDC, m_rcItem, m_nBorderSize, GetAdjustColor(m_dwBorderColor));
		}
	}
}

void CControlUI::DoPostPaint(HDC hDC, const RECT& rcPaint)
{
    return;
}

//************************************
// 函数名称: GetLeftBorderSize
// 返回类型: int
// 函数说明: 
//************************************
int CControlUI::GetLeftBorderSize() const
{
	return m_rcBorderSize.left;
}

//************************************
// 函数名称: SetLeftBorderSize
// 返回类型: void
// 参数信息: int nSize
// 函数说明: 
//************************************
void CControlUI::SetLeftBorderSize( int nSize )
{
	m_rcBorderSize.left = nSize;
	Invalidate();
}

//************************************
// 函数名称: GetTopBorderSize
// 返回类型: int
// 函数说明: 
//************************************
int CControlUI::GetTopBorderSize() const
{
	return m_rcBorderSize.top;
}

//************************************
// 函数名称: SetTopBorderSize
// 返回类型: void
// 参数信息: int nSize
// 函数说明: 
//************************************
void CControlUI::SetTopBorderSize( int nSize )
{
	m_rcBorderSize.top = nSize;
	Invalidate();
}

//************************************
// 函数名称: GetRightBorderSize
// 返回类型: int
// 函数说明: 
//************************************
int CControlUI::GetRightBorderSize() const
{
	return m_rcBorderSize.right;
}

//************************************
// 函数名称: SetRightBorderSize
// 返回类型: void
// 参数信息: int nSize
// 函数说明: 
//************************************
void CControlUI::SetRightBorderSize( int nSize )
{
	m_rcBorderSize.right = nSize;
	Invalidate();
}

//************************************
// 函数名称: GetBottomBorderSize
// 返回类型: int
// 函数说明: 
//************************************
int CControlUI::GetBottomBorderSize() const
{
	return m_rcBorderSize.bottom;
}

//************************************
// 函数名称: SetBottomBorderSize
// 返回类型: void
// 参数信息: int nSize
// 函数说明: 
//************************************
void CControlUI::SetBottomBorderSize( int nSize )
{
	m_rcBorderSize.bottom = nSize;
	Invalidate();
}

//************************************
// 函数名称: GetBorderStyle
// 返回类型: int
// 函数说明: 
//************************************
int CControlUI::GetBorderStyle() const
{
	return m_nBorderStyle;
}

//************************************
// 函数名称: SetBorderStyle
// 返回类型: void
// 参数信息: int nStyle
// 函数说明: 
//************************************
void CControlUI::SetBorderStyle( int nStyle )
{
	m_nBorderStyle = nStyle;
	Invalidate();
}


CControlUI::CControlPropertyAnimOjbect::CControlPropertyAnimOjbect(CControlUI* pControl)
{
    m_pControl = pControl;
    memset(&m_rcItem, 0, sizeof(RECT));
    RefreshItemRect();
    m_iRotation = 0;
    m_iAlpha = 255;
}

CControlUI::CControlPropertyAnimOjbect::~CControlPropertyAnimOjbect()
{
}

bool CControlUI::CControlPropertyAnimOjbect::IsAttachedToAnimator()
{
    if (m_rcItem.left != 0 || m_rcItem.top != 0 || /*RECT_WIDTH(m_rcItem) != m_pControl->GetPos().GetWidth() ||
                                                   RECT_HEIGHT(m_rcItem) != m_pControl->GetPos().GetHeight() ||*/
        (m_iRotation != 0 && (m_iRotation % 360) != 0) || m_iAlpha != 255)
        return true;
    return false;
}

DuiLib::QRect CControlUI::CControlPropertyAnimOjbect::GetPos()
{
    RefreshItemRect();
    return m_rcItem;
}

void CControlUI::CControlPropertyAnimOjbect::SetPosition(int x, int y, bool bRelative/* = false*/)
{
    RefreshItemRect();
    QRect rect = m_rcItem;
    m_rcItem.left = x;
    m_rcItem.top = y;
    m_rcItem.right = x + rect.GetWidth();
    m_rcItem.bottom = y + rect.GetHeight();
    m_pControl->Invalidate();
}

void CControlUI::CControlPropertyAnimOjbect::SetSize(int iWidth, int iHeight)
{
    m_rcItem.right = m_rcItem.left + iWidth;
    m_rcItem.bottom = m_rcItem.top + iHeight;
    m_pControl->Invalidate();
}

void CControlUI::CControlPropertyAnimOjbect::SetKeyWithValue(QUI_STRING strKeyName, int iValue)
{
    RECT rcPos = GetPos();
    if (strKeyName == _T("x"))
    {
        SetPosition(iValue, rcPos.top);
    }
    else if (strKeyName == _T("y"))
    {
        SetPosition(rcPos.left, iValue);
    }
    else if (strKeyName == _T("width"))
    {
        SetSize(iValue, rcPos.bottom - rcPos.top);
    }
    else if (strKeyName == _T("height"))
    {
        SetSize(rcPos.right - rcPos.left, iValue);
    }
    else if (strKeyName == _T("alpha"))
    {
        m_iAlpha = iValue;
    }
    else if (strKeyName == _T("rotation"))
    {
        m_iRotation = iValue;
    }
    if (m_pControl)
        m_pControl->Invalidate();
}

DuiLib::CPaintManagerUI* CControlUI::CControlPropertyAnimOjbect::GetManager() const
{
    if (m_pControl)
        return m_pControl->GetManager();
    return NULL;
}

void CControlUI::CControlPropertyAnimOjbect::SetBkImage(LPCTSTR pStrImage)
{
}

void CControlUI::CControlPropertyAnimOjbect::RefreshItemRect()
{
    if (RECT_WIDTH(m_rcItem) == 0)
    {
        m_rcItem.right = m_rcItem.left + m_pControl->GetPos().GetWidth();
    }
    if (RECT_HEIGHT(m_rcItem) == 0)
    {
        m_rcItem.bottom = m_rcItem.top + m_pControl->GetPos().GetHeight();
    }
}

} // namespace DuiLib
