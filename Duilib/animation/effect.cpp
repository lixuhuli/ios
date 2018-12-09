#include "StdAfx.h"
#include "effect.h"
#include "AnimationFactor.h"
#include "../helper/DrawingBoard.h"
#include "../Core/UIManager.h"
#include "EffectAnimator.h"

using namespace Gdiplus;
using namespace DuiLib;
#define EffectWndName _T("quilib_effect_wnd")

CUIEffect::CUIEffect( HWND hWnd, bool bShowWhenAnimOver, bool bAutoDelete/* = true*/)
{
	m_pControl = NULL;
	m_hWnd = hWnd;
	m_hEffectWnd = NULL;
	m_hShowWnd = NULL;
	m_pDrawingBoard = NULL;
	m_iStartAnimatorCount = 0;
	m_iStopAnimatorCount = 0;
	m_iProgressedCount = 0;
	m_iAlpha = 255;
	m_iRotation = 0;
	m_bNeedShowWndWhenAnimOver = bShowWhenAnimOver;
	m_bAutoDelete = bAutoDelete;
	m_bHasPainted = false;
	m_Blend.BlendOp = AC_SRC_OVER; 
	m_Blend.BlendFlags = 0; 
	m_Blend.AlphaFormat = AC_SRC_ALPHA;
	m_Blend.SourceConstantAlpha = 255;
	CreateEffectWnd();
}

CUIEffect::CUIEffect( HWND hWnd, DuiLib::CControlUI* pControl, bool bAutoDelete/* = true*/ )
{
	m_pControl = pControl;
	m_hWnd = hWnd;
	m_hEffectWnd = NULL;
	m_hShowWnd = NULL;
	m_pDrawingBoard = NULL;
	m_iStartAnimatorCount = 0;
	m_iStopAnimatorCount = 0;
	m_iProgressedCount = 0;
	m_iAlpha = 255;
	m_bHasPainted = false;
	m_iRotation = 0;

	m_bNeedShowWndWhenAnimOver = true;
	m_bAutoDelete = bAutoDelete;
	m_Blend.BlendOp = AC_SRC_OVER; 
	m_Blend.BlendFlags = 0; 
	m_Blend.AlphaFormat = AC_SRC_ALPHA;
	m_Blend.SourceConstantAlpha = 255;
	CreateEffectWnd();

}

CUIEffect::CUIEffect( HWND hWnd, HWND hWndToShow , bool bAutoDelete/* = true*/)
{
	m_pControl = NULL;
	m_hWnd = hWnd;
	m_hEffectWnd = NULL;
	m_hShowWnd = hWndToShow;
	m_pDrawingBoard = NULL;
	m_iStartAnimatorCount = 0;
	m_iStopAnimatorCount = 0;
	m_iProgressedCount = 0;
	m_iAlpha = 255;
	m_bHasPainted = false;
	m_iRotation = 0;

	m_bNeedShowWndWhenAnimOver = true;
	m_bAutoDelete = bAutoDelete;
	m_Blend.BlendOp = AC_SRC_OVER; 
	m_Blend.BlendFlags = 0; 
	m_Blend.AlphaFormat = AC_SRC_ALPHA;
	m_Blend.SourceConstantAlpha = 255;
	CreateEffectWnd();

}

CUIEffect::~CUIEffect()
{
	if(m_hEffectWnd != NULL)
		::DestroyWindow(m_hEffectWnd);
	if(m_pDrawingBoard)
		delete m_pDrawingBoard;
	m_hEffectWnd = m_hWnd = NULL;
	m_pFactor = NULL;
	m_pControl = NULL;
	AnimatorArray::iterator it = m_arrAnimators.begin();
	while(it != m_arrAnimators.end())
	{
		(*it)->UnSubscribe(EventAnimatorStart, MakeDelegate(this, &CUIEffect::OnAnimatorStart));
		(*it)->UnSubscribe(EventAnimatorEnd, MakeDelegate(this, &CUIEffect::OnAnimatorStoped));
		(*it)->UnSubscribe(EventAnimatorProgressed, MakeDelegate(this, &CUIEffect::OnAnimatorProgressed));
		it++;
	}
	m_arrAnimators.clear();
}

void DoOrNormal(DWORD dwKey, LPVOID pBuff, int nLen)
{
	if(nLen <= 0 || nLen % 4)
	{
		return;
	}
	int alpha = (dwKey & 0xFF000000)>>24;
	BYTE* DataPtr = (BYTE*)pBuff;
	BYTE *pR,*pG,*pB,*pA;
	for(; nLen > 0; nLen -= 4)
	{
		pB = DataPtr++;
		pG = DataPtr++;
		pR = DataPtr++;
		pA = DataPtr++;
		if (*pA == 0)
		{
			*pA = ((*pR == 0) && (*pG == 0) && (*pB == 0)) ? 0: alpha;// Set Alpha
		}
	}
}
void DoOrNormalWhole(DWORD dwKey, LPVOID pBuff, int nLen)
{
	if(nLen <= 0 || nLen % 4)
	{
		return;
	}
	for(DWORD * pSrc = (DWORD *)pBuff; nLen > 0; nLen -= 4, pSrc++)
	{
		*pSrc |= dwKey;
	}
}

CDrawingBoard* CUIEffect::SnapShot()
{
	if(m_pDrawingBoard)
	{
		delete m_pDrawingBoard;
		m_pDrawingBoard = NULL;
	}
	if(m_pDrawingBoard == NULL)
		m_pDrawingBoard = new CDrawingBoard;
	RECT rt;
	GetWindowRect(m_hWnd, &rt);
	m_pDrawingBoard->Create(RECT_WIDTH(rt), RECT_HEIGHT(rt));

	//SendMessage(m_hWnd, WM_PRINT, (WPARAM)m_pDrawingBoard->GetSafeHdc(), PRF_NONCLIENT | PRF_CLIENT | PRF_ERASEBKGND | PRF_CHILDREN);
	RECT rectFrom = {0,0, RECT_WIDTH(rt), RECT_HEIGHT(rt)};
	RECT rectTo = rectFrom;
	m_pDrawingBoard->BitBltFrom(GetDC(m_hWnd), rectFrom, rectTo);
	if(m_pDrawingBoard->GetBits() != NULL)
	{
		DoOrNormalWhole(0xff000000, m_pDrawingBoard->GetBits(), m_pDrawingBoard->GetBitLen());
	}
	return m_pDrawingBoard;
}

CDrawingBoard* CUIEffect::SnapShotEx()
{
	if(m_pDrawingBoard)
	{
		delete m_pDrawingBoard;
		m_pDrawingBoard = NULL;
	}
	if(m_pDrawingBoard == NULL)
		m_pDrawingBoard = new CDrawingBoard;
	RECT rt;
	GetWindowRect(m_hEffectWnd, &rt);
	m_pDrawingBoard->Create(RECT_WIDTH(rt), RECT_HEIGHT(rt));

	m_pControl->NeedUpdate();
	m_pControl->PaintToBoard(*m_pDrawingBoard, rt);

	return m_pDrawingBoard;
}

HWND CUIEffect::CreateEffectWnd()
{
	if(m_hEffectWnd)
		return m_hEffectWnd;
	if(RegisterWindowClass())
	{
		RECT rect;
		GetWindowRect(m_hWnd, &rect);
		m_hEffectWnd = ::CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_LAYERED , EffectWndName, _T("effect"),WS_POPUPWINDOW & (~WS_VISIBLE)/*UI_WNDSTYLE_FRAME*/, rect.left, rect.top, RECT_WIDTH(rect), RECT_HEIGHT(rect),NULL,
			NULL, DuiLib::CPaintManagerUI::GetInstance(), this);
	}
	return m_hEffectWnd;
}

HWND CUIEffect::CreateEffectWnd(DuiLib::QRect rt)
{
	if(RegisterWindowClass())
	{
		m_hEffectWnd = ::CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_LAYERED , EffectWndName, _T("effect"),WS_POPUPWINDOW/*UI_WNDSTYLE_FRAME*/, rt.left, rt.top, RECT_WIDTH(rt), RECT_HEIGHT(rt),NULL,
			NULL, DuiLib::CPaintManagerUI::GetInstance(), this);
	}
	return m_hEffectWnd;
}

bool CUIEffect::RegisterWindowClass()
{
	WNDCLASS wc = { 0 };
	wc.style = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.lpfnWndProc = CUIEffect::__WndProc;
	wc.hInstance =  DuiLib::CPaintManagerUI::GetInstance();
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = EffectWndName;
	ATOM ret = ::RegisterClass(&wc);
	ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LRESULT CALLBACK CUIEffect::__WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(uMsg == WM_CREATE)
	{
		//LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		//CUIEffect* pThis = static_cast<CUIEffect*>(lpcs->lpCreateParams);
		//RECT rect;
		//GetWindowRect(hWnd, &rect);
		//BLENDFUNCTION m_Blend;
		//m_Blend.BlendOp = AC_SRC_OVER; 
		//m_Blend.BlendFlags = 0; 
		//m_Blend.AlphaFormat = AC_SRC_ALPHA;
		//m_Blend.SourceConstantAlpha = 255;

		//POINT ptDst = {rect.left, rect.top};
		//POINT ptSrc = {0, 0};
		//SIZE sizeSrc = {RECT_WIDTH(rect), RECT_HEIGHT(rect)};
		//StretchBlt(GetDC(NULL), 0, 0, sizeSrc.cx, sizeSrc.cy, pThis->m_pDrawingBoard->GetSafeHdc(), 0, 0, pThis->m_pDrawingBoard->GetDcSize().cx, pThis->m_pDrawingBoard->GetDcSize().cy, SRCCOPY);
		//BOOL bResult = ::UpdateLayeredWindow(hWnd,NULL, &ptDst, &sizeSrc, pThis->m_pDrawingBoard->GetSafeHdc(), &ptSrc, 0, &m_Blend, 2);
		//DWORD dwResult = ::GetLastError();
		//int a = 0;
		return 1L;
	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool  CUIEffect::OnAnimatorStart( void* pAnimator )
{
	if(m_iStartAnimatorCount == 0)
	{
		//::ShowWindow(m_hWnd, SW_HIDE);
		InitDrawingBoard();
		::ShowWindow(m_hEffectWnd, SW_SHOW);
		::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE);
		m_iStopAnimatorCount = 0;
	}
	m_iStartAnimatorCount++;
	return true;
}

bool CUIEffect::OnAnimatorStoped( void* pAnimator )
{
	m_iStopAnimatorCount++;
	if(m_iStopAnimatorCount == m_arrAnimators.size())
	{
		::SetWindowPos(m_hEffectWnd, NULL, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
		RECT rtEffect;
		GetWindowRect(m_hEffectWnd, &rtEffect);
		if(m_bNeedShowWndWhenAnimOver)
		{
			HWND hWndToShow = m_hShowWnd ? m_hShowWnd : m_hWnd;
			::SetWindowPos(hWndToShow, NULL, rtEffect.left, rtEffect.top, rtEffect.right - rtEffect.left, rtEffect.bottom - rtEffect.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
		}
		::DestroyWindow(m_hEffectWnd);
		m_hEffectWnd = NULL;
		m_iStartAnimatorCount = 0;
	}
	return true;
}

HWND CUIEffect::GetOwnerHWnd() const
{
	return m_hWnd;
}

HWND CUIEffect::GetEffectHWnd()
{
	return m_hEffectWnd;
}

DuiLib::CControlUI* CUIEffect::GetEffectControl() const
{
	return m_pControl;
}

DuiLib::QRect CUIEffect::GetPos()
{
	RECT rt;
	::GetWindowRect(m_hEffectWnd, &rt);

	return rt;
}

DuiLib::CPaintManagerUI* CUIEffect::GetManager() const
{
	if(m_pControl != NULL)
		return m_pControl->GetManager();
	return NULL;
}

void CUIEffect::SetBkBitmap(HBITMAP hBmp)
{	
}

void CUIEffect::SetBkImage(LPCTSTR pStrImage)
{	
}

void CUIEffect::SetPosition( int x, int y , bool bRelative /*= false*/ )
{
	::SetWindowPos(m_hEffectWnd, NULL, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);	
	RECT effectRect;
	::GetWindowRect(m_hEffectWnd, &effectRect);
	int a = 0;
}

void CUIEffect::SetSize( int iWidth, int iHeight )
{
	::SetWindowPos(m_hEffectWnd, NULL, 0, 0, iWidth, iHeight, SWP_NOACTIVATE | SWP_NOMOVE | SWP_SHOWWINDOW);	
	RECT effectRect;
	::GetWindowRect(m_hEffectWnd, &effectRect);
	if (m_pControl)
	{
		m_pControl->SetSize(iWidth,iHeight);
		SnapShotEx();
	}
	int a = 0;
}

void CUIEffect::SetKeyWithValue( QUI_STRING strKeyName, int iValue)
{
	RECT rcPos = GetPos();
	if (strKeyName == _T("x"))
	{
		SetPosition(iValue, rcPos.top);
	}
	else if(strKeyName == _T("y"))
	{
		SetPosition(rcPos.left, iValue);
	}
	else if(strKeyName == _T("width"))
	{
		SetSize(iValue, rcPos.bottom - rcPos.top);
	}
	else if(strKeyName == _T("height"))
	{
		SetSize(rcPos.right - rcPos.left, iValue);
	}
	else if (strKeyName == _T("alpha"))
	{
		SetTransparent(iValue);
	}
	else if(strKeyName == _T("rotation"))
	{
		SetRotation(iValue);
	}
}

CDrawingBoard* CUIEffect::GetDrawingBoard()
{
	return m_pDrawingBoard;
}

void CUIEffect::AddToAnimator( CAnimator* pAnimator )
{
	if(pAnimator)
	{
		pAnimator->Subscribe(EventAnimatorStart, MakeDelegate(this, &CUIEffect::OnAnimatorStart));
		pAnimator->Subscribe(EventAnimatorEnd, MakeDelegate(this, &CUIEffect::OnAnimatorStoped));
		pAnimator->Subscribe(EventAnimatorProgressed, MakeDelegate(this, &CUIEffect::OnAnimatorProgressed));
	}
	m_arrAnimators.push_back(pAnimator);
}

void CUIEffect::RemoveFromAnimator( CAnimator* pAnimator )
{
	AnimatorArray::iterator it = m_arrAnimators.begin();
	while(it != m_arrAnimators.end())
	{
		if(*it == pAnimator)
		{
			m_arrAnimators.erase(it);
			pAnimator->UnSubscribe(EventAnimatorStart, MakeDelegate(this, &CUIEffect::OnAnimatorStart));
			pAnimator->UnSubscribe(EventAnimatorEnd, MakeDelegate(this, &CUIEffect::OnAnimatorStoped));
			break;
		}
		it++;
	}
	if(m_arrAnimators.size() == 0)
	{
		if(m_bAutoDelete)
			delete this;
	}
}

IAnimObject* CUIEffect::GetAnimObject()
{
	return (IAnimObject*)this;
}

bool CUIEffect::OnAnimatorProgressed( void* pAnimator )
{
	m_iProgressedCount++;
	if(m_iProgressedCount == (int)m_arrAnimators.size())
	{
		m_iProgressedCount = 0;
		DoPaint();
	}
	return false;
}

void CUIEffect::DoPaint()
{
	if(!m_pDrawingBoard)
		return;
	int iWidth = m_pDrawingBoard->GetDcSize().cx;
	int iHeight = m_pDrawingBoard->GetDcSize().cy;

	m_Blend.BlendOp = AC_SRC_OVER; 
	m_Blend.BlendFlags = 0; 
	m_Blend.AlphaFormat = AC_SRC_ALPHA;
	m_Blend.SourceConstantAlpha = m_iAlpha;

	RECT ownedRect;
	GetWindowRect(this->GetEffectHWnd(), &ownedRect);

	POINT pointDest = {ownedRect.left, ownedRect.top};
	POINT pointSrc = {0, 0};
	SIZE  sizeDest = {RECT_WIDTH(ownedRect), RECT_HEIGHT(ownedRect)};
	SIZE sizeSrc = {iWidth, iHeight};

	RECT effectRect;
	::GetWindowRect(this->GetEffectHWnd(), &effectRect);
	BOOL bResult = ::UpdateLayeredWindow(this->GetEffectHWnd(), NULL,&pointDest, &sizeDest, this->GetDrawingBoard()->GetSafeHdc(), &pointSrc, 0, &m_Blend, 2);
	DWORD dwResult = ::GetLastError();

}

void CUIEffect::SetTransparent( int iAlpha )
{
	m_iAlpha = iAlpha;
}

void CUIEffect::SetRotation(int iValue)
{
	m_iRotation = iValue;
	if (m_pControl != NULL)
	{
		m_pControl->SetRotation(m_iRotation);
		SnapShotEx();
	}
}

void CUIEffect::InitDrawingBoard()
{
	if(!m_pDrawingBoard)
	{
		if(m_pControl == NULL)
			SnapShot();
		else
			SnapShotEx();
	}
	CreateEffectWnd();
}

void CUIEffect::Shutter()
{
	if(m_pControl == NULL)
		SnapShot();
	else
		SnapShotEx();
	if(m_hEffectWnd == NULL)
		CreateEffectWnd();
	else
	{
		RECT rect;
		GetWindowRect(m_hWnd, &rect);
		::SetWindowPos(m_hEffectWnd, NULL, rect.left, rect.top, RECT_WIDTH(rect), RECT_HEIGHT(rect), SWP_HIDEWINDOW);
	}
}

void CUIEffect::SetHasPainted( bool bPainted )
{
	m_bHasPainted = bPainted;
}

