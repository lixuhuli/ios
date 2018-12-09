#ifndef __UIRENDER_H__
#define __UIRENDER_H__

#pragma once

#include "utils/GifHandler.h"

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CRenderClip
{
public:
    ~CRenderClip();
    RECT rcItem;
    HDC hDC;
    HRGN hRgn;
    HRGN hOldRgn;

    static void GenerateClip(HDC hDC, RECT rc, CRenderClip& clip);
    static void GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CRenderClip& clip);
    static void UseOldClipBegin(HDC hDC, CRenderClip& clip);
    static void UseOldClipEnd(HDC hDC, CRenderClip& clip);
};

/////////////////////////////////////////////////////////////////////////////////////
//
#define  MAX_ALPHABUF	1<<16

typedef struct tagALPHAINFO
{
    LPBYTE lpBuf;
    RECT	rc;
    tagALPHAINFO()
    {
        lpBuf = NULL;
        rc.left = rc.top = rc.right = rc.bottom = 0;
    }
} ALPHAINFO, *LPALPHAINFO;

class UILIB_API CGdiAlpha
{
public:
    static BOOL AlphaBackup(HDC hdc, LPCRECT pRect, ALPHAINFO &alphaInfo);
    static void AlphaRestore(HDC hdc, const ALPHAINFO &alphaInfo);
    static HBITMAP CreateBitmap32(HDC hdc, int nWid, int nHei, LPVOID * ppBits = NULL, BYTE byAlpha = 0);

private:
    static BYTE s_byAlphaBack[MAX_ALPHABUF];
    //����λͼ��Alphaͨ��
    static LPBYTE ALPHABACKUP(BITMAP *pBitmap, int x, int y, int cx, int cy);
    //�ָ�λͼ��Alphaͨ��
    static void ALPHARESTORE(BITMAP *pBitmap, int x, int y, int cx, int cy, LPBYTE lpAlpha);
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CRenderEngine
{
public:
    static DWORD AdjustColor(DWORD dwColor, short H, short S, short L);
    static TImageInfo* LoadImage(STRINGorID bitmap, LPCTSTR type = NULL, DWORD mask = 0);
	static TImageInfo* LoadImage(LPBYTE pData, DWORD dwSize, DWORD mask = 0);//�Զ���
	static CGifHandler* LoadGif(STRINGorID bitmap, LPCTSTR type = NULL, DWORD mask = 0);
    static void FreeImage(const TImageInfo* bitmap);
    static void DrawImage(HDC hDC, HBITMAP hBitmap, const RECT& rc, const RECT& rcPaint, \
        const RECT& rcBmpPart, const RECT& rcCorners, bool alphaChannel, BYTE uFade = 255, 
        bool hole = false, bool xtiled = false, bool ytiled = false, bool Rxtiled = false);
    static bool DrawImageString(HDC hDC, CPaintManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint, 
        LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);
    static void DrawColor(HDC hDC, const RECT& rc, DWORD color, BOOL bByBmp = FALSE);
    static void DrawGradient(HDC hDC, const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps);

    // ���º����е���ɫ����alphaֵ��Ч
    static void DrawLine(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor,int nStyle = PS_SOLID);
    static void DrawRect(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor);
    static void DrawRoundRect(HDC hDC, const RECT& rc, int width, int height, int nSize, DWORD dwPenColor);
    static void DrawText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, int iFont, UINT uStyle, bool bUseGDIPlus = true);
    static HBITMAP CreateAlphaTextBitmap(LPCTSTR inText, HFONT inFont, COLORREF inColour, RECT rcBmp, UINT uStyle, POINT &pt, BOOL bUseTextout = FALSE);
    static void DrawHtmlText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, 
        DWORD dwTextColor, RECT* pLinks, CDuiString* sLinks, int& nLinkRects, UINT uStyle);
    static HBITMAP GenerateBitmap(CPaintManagerUI* pManager, CControlUI* pControl, RECT rc);
    static void AlphaBlend(HDC hDCDst, int ix, int iy, int cx, int cy,
        HDC hDCSrc, int iSrcX, int iSrcY, int icxSrc, int icySrc, int iAlpha);
	static SIZE GetTextSize(HDC hDC, CPaintManagerUI* pManager , LPCTSTR pstrText, int iFont, UINT uStyle);
    static void DoAnimateWindow(HWND hWnd, UINT uStyle, DWORD dwTime = 200);

	static bool DrawBufferImage(HDC hDC, CPaintManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint, LPCTSTR strImgName,
		LPCSTR strBuffer, DWORD dwSize, LPCTSTR pStrModify = NULL);

    static bool GdiplusStretchBlt(HDC hdcDst, int nXOriginDst, int nYOriginDst, int nWidthDst, int nHeightDst
        , HBITMAP hBitmap, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc);
    //��alpha͸���Ƚ� hbmp ��Բ�Ǿ�����ʽ���Ƶ�dstDc��(x,y)λ��. 
    static BOOL DrawRoundRectAntiAlias(HBITMAP hbmp, HDC dstDc, int x, int y, int RoundCornerX, int RoundCornerY, int bordersize, DWORD dwborderColor, int alpha = 255);
    //ͬ�ϣ�ֻ�ǽ�hbmp���srcRc������Ƶ�dstDc��(x,y)λ��
    static BOOL DrawRoundRectAntiAlias(HDC srcDC, RECT srcRc, HDC dstDc, int x, int y, int RoundCornerX, int RoundCornerY, int bordersize, DWORD dwborderColor, int alpha = 255);

private:
	static bool LoadImageData(STRINGorID bitmap, LPCTSTR type, OUT void** ppImgData, OUT DWORD& dwSize);
	static bool LoadFileData(LPCTSTR lpFile, OUT void** ppImgData, OUT DWORD& dwSize);
	static bool LoadResourceData(LPCTSTR lpResource, LPCTSTR type, OUT void** ppImgData, OUT DWORD& dwSize);
	static bool LoadZipData(LPCTSTR lpFile, OUT void** ppImgData, OUT DWORD& dwSize);
};

class CGraphicsRoundRectPath : public Gdiplus::GraphicsPath
{
public:
    CGraphicsRoundRectPath();
    CGraphicsRoundRectPath(INT x, INT y, INT w, INT h, INT cornerX, INT cornerY);
    ~CGraphicsRoundRectPath();

    BOOL AddRoundRect(INT x, INT y, INT w, INT h, INT cornerX, INT cornerY);
};

} // namespace DuiLib

#endif // __UIRENDER_H__
