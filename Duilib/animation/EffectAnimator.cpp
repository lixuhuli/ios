#include "StdAfx.h"
#include "EffectAnimator.h"
#include "../helper/DrawingBoard.h"
#include "AnimationFactor.h"

#include "math/matrix4.h"
#include "math/vector3d.h"
#include "effect.h"

using namespace irr;
using namespace irr::core;
using namespace Gdiplus;

CWndReginAnimator::CWndReginAnimator( CUIEffect* pEffect, CTween::Type tweenType, int iDuration, int iPointCount, Gdiplus::Point* pBeginPoints, Gdiplus::Point* pEndPoints, bool bTimeSensive /*= false*/, bool bUserThreadTimer /*= false*/ ) :
CAnimator(pEffect->GetAnimObject(), tweenType,iDuration, bTimeSensive, bUserThreadTimer),
m_iPointCount(iPointCount),
m_pUiEffect(pEffect)
{
	if(m_iPointCount > 0)
	{
		m_pBeginPoints = new Gdiplus::Point[m_iPointCount];
		m_pEndPoints = new Gdiplus::Point[m_iPointCount];
		for(int i = 0; i < m_iPointCount; i++)
		{
			m_pBeginPoints[i] = pBeginPoints[i];
			m_pEndPoints[i] = pEndPoints[i];
		}
		m_pUiEffect->SetHasPainted(true);
	}
}

CWndReginAnimator::~CWndReginAnimator()
{
	if(m_pBeginPoints)
		delete[] m_pBeginPoints;
	if(m_pEndPoints)
		delete[] m_pEndPoints;
}

RECT GetBoudsOfPoints(Point* pPoints, int iPointCount)
{
	RECT rect ={pPoints[0].X,pPoints[0].Y,pPoints[0].X,pPoints[0].Y};
	for(int i = 0; i < iPointCount; i++)
	{
		if(pPoints[i].X < rect.left)
		{
			rect.left = pPoints[i].X;
		}
		else if(pPoints[i].X > rect.right)
		{
			rect.right = pPoints[i].X;
		}
		if(pPoints[i].Y < rect.top)
		{
			rect.top = pPoints[i].Y;
		}
		else if(pPoints[i].Y > rect.bottom)
		{
			rect.bottom = pPoints[i].Y;
		}
	}
	return rect;
}

void CWndReginAnimator::OnAnimationProgressed( CAnimation* pAnimation )
{
 	if(m_iPointCount > 0)
 	{
  		Gdiplus::Point* currentPoints = new Gdiplus::Point[m_iPointCount];
  		for(int i = 0; i < m_iPointCount; i++)
  		{
  			currentPoints[i].X = m_pAnimation->CurrentValueBetween(m_pBeginPoints[i].X, m_pEndPoints[i].X);
  			currentPoints[i].Y = m_pAnimation->CurrentValueBetween(m_pBeginPoints[i].Y, m_pEndPoints[i].Y);
  		}

		m_pBeginPoints[0].X;
		m_pBeginPoints[0].Y;
		m_pBeginPoints[1].X;
		m_pBeginPoints[1].Y;
		m_pBeginPoints[2].X;
		m_pBeginPoints[2].Y;
		m_pBeginPoints[3].X;
		m_pBeginPoints[3].Y;

		m_pEndPoints[0].X;
		m_pEndPoints[0].Y;
		m_pEndPoints[1].X;
		m_pEndPoints[1].Y;
		m_pEndPoints[2].X;
		m_pEndPoints[2].Y;
		m_pEndPoints[3].X;
		m_pEndPoints[3].Y;
  
  		currentPoints[0].X;
  		currentPoints[0].Y;
  		currentPoints[1].X;
  		currentPoints[1].Y;
  		currentPoints[2].X;
  		currentPoints[2].Y;
  		currentPoints[3].X;
  		currentPoints[3].Y;
  
  
  		RECT windowRect = GetBoudsOfPoints(currentPoints, m_iPointCount);
  
  		for(int i = 0; i < m_iPointCount; i++)
  		{
  			currentPoints[i].X -= windowRect.left;
  			currentPoints[i].Y -= windowRect.top;
  		}
  
  		POINT pointDest = {windowRect.left, windowRect.top};
  		POINT pointSrc = {0, 0};
  		SIZE  sizeDest = {RECT_WIDTH(windowRect), /*400*/RECT_HEIGHT(windowRect)};
  		SIZE sizeSrc = sizeDest;
		CDrawingBoard* pBoard = m_pUiEffect->GetDrawingBoard();
  		SIZE dcSize = pBoard->GetDcSize();
  
  		Gdiplus::GraphicsPath path(FillModeWinding);
  		path.AddPolygon(currentPoints, m_iPointCount);
  		//Gdiplus::Region region(&path);
  		Gdiplus::Bitmap bitmap(pBoard->GetBmpHandle(), NULL);
  		//Blur blur;
  		//int iRadius = m_pAnimation->CurrentValueBetween(0, 10);
  		//BlurParams param = {iRadius, false};
  		//blur.SetParameters(&param);
  
  		//RECT bitmapRect = {0, 0, dcSize.cx, dcSize.cy};
  		//Status sA = bitmap.ApplyEffect(&blur, &bitmapRect);
  		////Gdiplus::TextureBrush brush(&bitmap, WrapModeTileFlipXY);
  		CDrawingBoard board;
  		board.Create(sizeDest.cx, sizeDest.cy, false, true);
  
  		Gdiplus::Graphics* graph = Gdiplus::Graphics::FromHDC(/*pBoard->GetSafeHdc()*/board.GetSafeHdc());
  		//Gdiplus::Graphics* graph = Gdiplus::Graphics::FromHDC(GetDC(NULL));
  		//Gdiplus::Graphics* graph = Gdiplus::Graphics::FromHWND(m_pFactor->hWndEffect);
  		if(graph == NULL)
  			return;
  		//graph->FillRegion(&brush, &region);
  		//graph->FillPath(&brush, &path);
  
  		graph->SetClip(&path);
  		Rect srcRect(0,0, dcSize.cx, dcSize.cy);
  		Rect dstRect(0,0,sizeDest.cx, sizeDest.cy);
  		graph->DrawImage(&bitmap, dstRect, 0,0,dcSize.cx, dcSize.cy, UnitPixel);
  	
  
  		Gdiplus::Graphics* graphDesktop = Gdiplus::Graphics::FromHDC(GetDC(NULL));
		//RECT toRect_ ={0, 0, 400, 400};
		//RECT srcRect_ = toRect_;
        //board.BitBltTo(GetDC(NULL), toRect_, srcRect_);
  		//Gdiplus::Status s = graph->DrawImage(&bitmap, currentPoints, m_pFactor->iPointCount);
  
  		//Gdiplus::Status s = graph->DrawImage(&bitmap, currentPoints, m_pFactor->iPointCount, 0, 0, dcSize.cx, dcSize.cy, Unit::UnitPixel);
  		const RectF rf(0, 0, 400, 400);
  		//const  RectF rf1(0, 0, 400, 400);
  		//graphDesktop->DrawImage(&bitmap, rf, 0,0,400,400, UnitPixel);
  
  		m_Blend.BlendOp = AC_SRC_OVER; 
  		m_Blend.BlendFlags = 0; 
  		m_Blend.AlphaFormat = AC_SRC_ALPHA;
  		m_Blend.SourceConstantAlpha = 255;
  		//::SetWindowPos(m_pFactor->hWndEffect, NULL, windowRect.left, windowRect.top, sizeDest.cx, sizeDest.cy, SWP_NOACTIVATE);
 
 		RECT rtPos = {0};
 		rtPos.left = windowRect.left;
 		rtPos.top = windowRect.top;
 		rtPos.right = rtPos.left + sizeDest.cx;
 		rtPos.bottom = rtPos.top + sizeDest.cy;
 		m_pAnimObject->SetPosition(rtPos.left, rtPos.top);
		m_pAnimObject->SetSize(rtPos.right - rtPos.left, rtPos.bottom - rtPos.top);
		RECT effectRect;
		::GetWindowRect(m_pUiEffect->GetEffectHWnd(), &effectRect);
  		BOOL bResult = ::UpdateLayeredWindow(m_pUiEffect->GetEffectHWnd(), NULL, &pointDest, &sizeDest, /*pBoard->GetSafeHdc()*/board.GetSafeHdc(), &pointSrc, 0, &m_Blend, 2);
  		DWORD dwResult = ::GetLastError();
  		int aa = 0;

        CAnimator::OnAnimationProgressed(pAnimation);
 	}
}

CWndFadeAnimator::CWndFadeAnimator( CUIEffect* pEffect,  CTween::Type tweenType, int iDuration,  int iAlphaFrom, int iAlphaTo ) :
CAnimator(pEffect->GetAnimObject(), tweenType, iDuration)
{
	m_iAlphaFrom = iAlphaFrom;
	m_iAlphaTo = iAlphaTo;
	m_pUiEffect = pEffect;
}

CWndFadeAnimator::~CWndFadeAnimator()
{
}

void CWndFadeAnimator::OnAnimationProgressed( CAnimation* pAnimation )
{
	int iWidth = m_pUiEffect->GetDrawingBoard()->GetDcSize().cx;
	int iHeight = m_pUiEffect->GetDrawingBoard()->GetDcSize().cy;

	//BitBlt(GetDC(NULL),0,0,iRWidth,iRHeight, board.GetSafeHdc(), 0, 0, SRCCOPY);

	m_Blend.BlendOp = AC_SRC_OVER; 
	m_Blend.BlendFlags = 0; 
	m_Blend.AlphaFormat = AC_SRC_ALPHA;
	m_Blend.SourceConstantAlpha = 255;

	RECT ownedRect;
	GetWindowRect(m_pUiEffect->GetEffectHWnd(), &ownedRect);

	POINT pointDest = {ownedRect.left, ownedRect.top};
	POINT pointSrc = {0, 0};
	SIZE  sizeDest = {iWidth, iHeight};
	SIZE sizeSrc = sizeDest;

	RECT effectRect;
	::GetWindowRect(m_pUiEffect->GetEffectHWnd(), &effectRect);
	BOOL bResult = ::UpdateLayeredWindow(m_pUiEffect->GetEffectHWnd(), NULL,/*&pointSrc*/ &pointDest, /*&dcSize*/&sizeDest, m_pUiEffect->GetDrawingBoard()->GetSafeHdc(), &pointSrc, 0, &m_Blend, 2);
	DWORD dwResult = ::GetLastError();
	CAnimator::OnAnimationProgressed(pAnimation);
}

void CWndFadeAnimator::OnAnimatonStart( CAnimation* pAnimation )
{
	CAnimator::OnAnimatonStart(pAnimation);
}


