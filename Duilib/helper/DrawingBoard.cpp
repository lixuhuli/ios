
#include "stdafx.h"
#include "DrawingBoard.h"
#include <math.h>

extern void DoOrNormal(DWORD dwKey, LPVOID pBuff, int nLen);
CDrawingBoard::CDrawingBoard(void)
{
    m_pBits = NULL;
	m_hBmp = NULL;
	m_hOldBmp = NULL;
	m_hDC = NULL;
	m_DcSize.cx = m_DcSize.cy = 0;
	m_dwBitLen = 0;
}

CDrawingBoard::~CDrawingBoard(void)
{
   Delete();
}

SIZE CDrawingBoard::GetDcSize()
{
	return m_DcSize;
}

HDC& CDrawingBoard::GetSafeHdc()
{
	return m_hDC;
}

HBITMAP& CDrawingBoard::GetBmpHandle()
{
	return m_hBmp;
}

BYTE* CDrawingBoard::GetBits()
{
	return m_pBits;
}

bool CDrawingBoard::Create(int nWidth, int nHeight, bool bReCreate, bool bNoCreateInit)
{
	bool bIsCreateNew = false;
	if ((nWidth <= 0) || (nHeight<= 0))
		return bIsCreateNew;

	if (bReCreate || (nWidth != m_DcSize.cx) || (nHeight != m_DcSize.cy))
	{
		bIsCreateNew = true;
		Delete();
		
		m_DcSize.cx = nWidth;
		m_DcSize.cy = nHeight;

		BITMAPINFOHEADER bih;
		memset(&bih, 0, sizeof(BITMAPINFOHEADER));
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biHeight = nHeight;
		bih.biWidth = nWidth;
		bih.biPlanes = 1;
		bih.biBitCount = 32;
		bih.biCompression = BI_RGB;

		m_dwBitLen = m_DcSize.cx * m_DcSize.cy * 4;

		m_hDC = ::CreateCompatibleDC(NULL);
		if (m_hDC != NULL)
		{
			m_hBmp = ::CreateDIBSection(GetSafeHdc(), (BITMAPINFO*)&bih,
				DIB_RGB_COLORS, (void**)(&m_pBits), NULL, 0);

			if (m_hBmp != NULL && m_pBits != NULL)
			{
				m_hOldBmp = (HBITMAP)::SelectObject(m_hDC, m_hBmp);
				::SetBkMode(m_hDC, TRANSPARENT);
			}
			else
			{
				Delete();
			}
		}
		else
		{
			Delete();
		}
	}
	else
	{
		if (m_pBits != NULL && bNoCreateInit)
			memset(m_pBits, 0, m_DcSize.cx * m_DcSize.cy * 4);
	}

	return bIsCreateNew;
}

bool CDrawingBoard::Create( HWND hWnd , int iWidth, int iHeight)
{
	m_hDC = ::CreateCompatibleDC(::GetDC(hWnd));
	m_hBmp = ::CreateCompatibleBitmap(m_hDC, iWidth, iHeight);
	m_hOldBmp = (HBITMAP)::SelectObject(m_hDC, m_hBmp);
	m_DcSize.cx = iWidth;
	m_DcSize.cy = iHeight;
	return true;
}

void CDrawingBoard::Delete()
{
	if (m_hDC != NULL && m_hOldBmp != NULL)
		::SelectObject(m_hDC, m_hOldBmp);
	m_hOldBmp = NULL;

	if (m_hBmp != NULL)
	{
		::DeleteObject(m_hBmp);
		m_hBmp = NULL;
	}

	if (m_hDC != NULL)
	{
		::DeleteDC(m_hDC);
		m_hDC = NULL;
	}

	m_pBits = NULL;
	m_DcSize.cx = m_DcSize.cy = 0;
}

// 从另一个内存DC克隆
bool CDrawingBoard::Clone(CDrawingBoard& FromBoard)
{
	if (FromBoard.GetSafeHdc() == NULL || FromBoard.GetDcSize().cx <= 0 || FromBoard.GetDcSize().cy <= 0)
		return false;

	this->Create(FromBoard.GetDcSize().cx, FromBoard.GetDcSize().cy, true, false);
	if (m_hDC == NULL)
		return false;
	
	RECT fromRct = {0, 0, FromBoard.GetDcSize().cx, FromBoard.GetDcSize().cy};

	return FromBoard.AlphaBlendTo(*this, fromRct, fromRct);
}

// 绘制到另外一个内存DC上
bool CDrawingBoard::BitBltTo(CDrawingBoard& ToBoard, RECT ToRct, RECT FromRct)
{
	return this->BitBltTo(ToBoard.GetSafeHdc(), ToRct, FromRct);
}

bool CDrawingBoard::BitBltTo(HDC hDc, RECT ToRct, RECT FromRct)
{
	if (!IS_SAFE_HANDLE(m_hDC) || !IS_SAFE_HANDLE(hDc) || IS_RECT_EMPTY(ToRct) || IS_RECT_EMPTY(FromRct))
		return false;
	return (::BitBlt(hDc, ToRct.left, ToRct.top, RECT_WIDTH(ToRct), RECT_HEIGHT(ToRct),
		m_hDC, FromRct.left, FromRct.top, SRCCOPY) == TRUE);

}

bool CDrawingBoard::BitBltFrom(CDrawingBoard& FromBoard, RECT FromRct, RECT ToRct)
{
	return this->BitBltFrom(FromBoard.GetSafeHdc(), FromRct, ToRct);
}

// 从一个DC进行复制
bool CDrawingBoard::BitBltFrom(HDC hDc, RECT FromRct, RECT ToRct)
{
	if (!IS_SAFE_HANDLE(hDc) || !IS_SAFE_HANDLE(m_hDC) || IS_RECT_EMPTY(ToRct) || IS_RECT_EMPTY(FromRct))
		return false;


	return (::BitBlt(m_hDC, ToRct.left, ToRct.top, RECT_WIDTH(ToRct), RECT_HEIGHT(ToRct),
		hDc, FromRct.left, FromRct.top, SRCCOPY) == TRUE);

}

bool CDrawingBoard::StretchBltFrom(CDrawingBoard& FromBoard, RECT FromRct, RECT ToRct)
{
	return this->StretchBltFrom(FromBoard.GetSafeHdc(), FromRct, ToRct);
}

// 从一个DC进行复制
bool CDrawingBoard::StretchBltFrom(HDC hDc, RECT FromRct, RECT ToRct)
{
	if (!IS_SAFE_HANDLE(hDc) || !IS_SAFE_HANDLE(m_hDC) || IS_RECT_EMPTY(ToRct) || IS_RECT_EMPTY(FromRct))
		return false;

	return (::StretchBlt(m_hDC, ToRct.left, ToRct.top, RECT_WIDTH(ToRct), RECT_HEIGHT(ToRct),
		hDc, FromRct.left, FromRct.top, RECT_WIDTH(FromRct), RECT_HEIGHT(FromRct), SRCCOPY) == TRUE);

}

bool CDrawingBoard::AlphaBlendTo(CDrawingBoard& ToBoard, RECT ToRct, RECT FromRct, int nAlpha, bool bFillAlphaChannel)
{
	return this->AlphaBlendTo(ToBoard.GetSafeHdc(), ToRct, FromRct, nAlpha, bFillAlphaChannel);
}

// 绘制到另外一个内存DC上
bool CDrawingBoard::AlphaBlendTo(HDC hDc, RECT ToRct, RECT FromRct,  int nAlpha, bool bFillAlphaChannel)
{
	if (!IS_SAFE_HANDLE(m_hDC) || !IS_SAFE_HANDLE(hDc)  
		|| IS_RECT_EMPTY(ToRct) || IS_RECT_EMPTY(FromRct))
		return false;

    if (/*bFillAlphaChannel && */GetBits() != NULL)
        DoOrNormal(0xff000000, GetBits(), GetBitLen());
    DuiLib::CRenderEngine::AlphaBlend(hDc, ToRct.left, ToRct.top, RECT_WIDTH(ToRct), RECT_HEIGHT(ToRct),
		m_hDC, FromRct.left, FromRct.top, RECT_WIDTH(FromRct), RECT_HEIGHT(FromRct), nAlpha);
	return true;
}

DWORD CDrawingBoard::GetBitLen()
{
	return m_dwBitLen;
}

void CDrawingBoard::RotateTo( HDC hDc, RECT& dstRect, int iAngle )
{
	PLGBLTPNTS	pnts;
	int x =  dstRect.left + RECT_WIDTH(dstRect)/2;
	int y = dstRect.top + RECT_HEIGHT(dstRect)/2;

	pnts[0].x	=	dstRect.left - x;
	pnts[0].y	=	dstRect.top - y;
	pnts[1].x	=	dstRect.right - x;
	pnts[1].y	=	dstRect.top - y;
	pnts[2].x	=	dstRect.left - x;
	pnts[2].y	=	dstRect.bottom - y;

	CRotator rotator(iAngle);
	rotator.rotate((POINT *)pnts,3);

	pnts[0].x	+=	x;
	pnts[0].y	+=	y;
	pnts[1].x	+=	x;
	pnts[1].y	+=	y;
	pnts[2].x	+=	x;
	pnts[2].y	+=	y;
	::PlgBlt(hDc,
		(const POINT *)&pnts,
		m_hDC,
		0,
		0,
		m_DcSize.cx,
		m_DcSize.cy,
		NULL,
		0,
		0);
}

void CDrawingBoard::Reset()
{
	m_pBits = NULL;
	m_hBmp = NULL;
	m_hOldBmp = NULL;
	m_hDC = NULL;
	m_DcSize.cx = m_DcSize.cy = 0;
	m_dwBitLen = 0;
}

#define _PI 3.14159265358979
CRotator::CRotator( int iAngle )
{
	double dRadians = -iAngle / 180.00 * _PI;
	m_dcos = cos(dRadians);
	m_dsin = sin(dRadians);
}

CRotator::~CRotator()
{

}

int CRotator::Round(double num)
{
	int temp = (int)(num * 10);

	if(temp % 10 < 4)
		return (int)floor(num);
	else
		return (int)ceil(num);

}

int CRotator::rX( int x, int y )
{
	return Round(x*m_dcos + y*m_dsin);
}

int CRotator::rY( int x, int y )
{
	return Round(y*m_dcos - x*m_dsin);
}

void CRotator::rotate( POINT *pPnts, UINT nCnt )
{
	for(;nCnt>0; --nCnt, ++pPnts)
	{
		POINT p			=	*pPnts;
		pPnts->x	=	rX(p.x,p.y);
		pPnts->y	=	rY(p.x,p.y);
	}
}
