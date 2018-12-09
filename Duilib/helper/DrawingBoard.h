
#pragma once
#include <Windows.h>
#include <WinDef.h>

#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(hHandle)					{if((hHandle)!=NULL)::CloseHandle((hHandle));(hHandle)=NULL;};
#endif

#ifndef SAFE_FREE_LIBRARY
#define SAFE_FREE_LIBRARY(hHandle)					{if((hHandle)!=NULL)::FreeLibrary((hHandle));(hHandle)=NULL;};
#endif

#ifndef SAFE_DELETE_OBJECT
#define SAFE_DELETE_OBJECT(hHandle)					{if((hHandle)!=NULL)::DeleteObject((hHandle));(hHandle)=NULL;};
#endif

#ifndef IS_SAFE_HANDLE
#define IS_SAFE_HANDLE(hWnd)						(((hWnd)!=NULL)&&((hWnd)!=INVALID_HANDLE_VALUE))
#endif

#ifndef IS_INVALID_HANDLE
#define IS_INVALID_HANDLE(hWnd)						(((hWnd)==NULL)||((hWnd)==INVALID_HANDLE_VALUE))
#endif

// 初始化点
#define INIT_POINT(pt)								((pt).x=(pt).y=0)
// 得到矩形宽度
#define RECT_WIDTH(GetRct)							((GetRct).right-(GetRct).left)
// 得到矩形宽度
#define RECT_HEIGHT(GetRct)							((GetRct).bottom-(GetRct).top)
// 初始化矩形为空
#define INIT_RECT(SetEmRect)						((SetEmRect).left=(SetEmRect).right=(SetEmRect).top=(SetEmRect).bottom=0)
// 设置矩形
#define SET_RECT(SetRct,nL,nT,nR,nB)				((SetRct).left=(nL),(SetRct).top=(nT),(SetRct).right=(nR),(SetRct).bottom=(nB))
#define SET_RECT_WH(SetRct,nL,nT,nWidth,nHeight)	((SetRct).left=(nL),(SetRct).top=(nT),(SetRct).right=(nL)+(nWidth),(SetRct).bottom=(nT)+(nHeight))
// 判断矩形是否不为空
#define IS_RECT_NOT_EMPTY(CheckRect)				((RECT_WIDTH(CheckRect)>0)&&((RECT_HEIGHT(CheckRect)>0)))
// 判断矩形是否为空
#define IS_RECT_EMPTY(CheckRect)					((RECT_WIDTH(CheckRect)<=0)||(RECT_HEIGHT(CheckRect)<=0))

// 解决GDI+的Alpha融合中Alpha融合进位误差的问题，程序默认的Alpha的最大值为254，而非255
#define MAX_ALPHA									(254)

class CRotator
{
public:
	CRotator(int iAngle);
	~CRotator();
	int rX(int x, int y);
	int rY(int x, int y);
	void rotate(POINT *pPnts, UINT nCnt);
private:
	int CRotator::Round(double num);
	double m_dcos, m_dsin;
};

class CDrawingBoard
{
public:
    CDrawingBoard();
    virtual ~CDrawingBoard();

	// 参数：
	// nWidth：宽度
	// nHeight：高度
	// nDefaultColor：默认填充颜色值
	// bReCreate：每次都重新创建，不管宽度和高度是否发生变化
	// bNoCreateInit：没有重新创建的情况下是否使用默认颜色值初始化整个数据空间
	// 返回值含义：是否创建了一个新的内存DC
	bool Create(int nWidth, int nHeight, bool bReCreate = false, bool bNoCreateInit = false);
	bool Create(HWND hWnd, int iWidth, int iHeight);
	void Delete();
	HBITMAP& GetBmpHandle();
	HDC& GetSafeHdc();
	// 按照 BGRA 顺序排布的32位位图信息
	BYTE* GetBits();
	SIZE GetDcSize();

	// 从另一个内存DC克隆
	bool Clone(CDrawingBoard& FromBoard);

	// 绘制到另外一个内存DC上
	bool AlphaBlendTo(CDrawingBoard& ToBoard, RECT ToRct, RECT FromRct,  int nAlpha = 255, bool bFillAlphaChannel = false);
	bool AlphaBlendTo(HDC hDc, RECT ToRct, RECT FromRct, int nAlpha = 255, bool bFillAlphaChannel = false);

	// 从一个DC进行复制，如果 pUiKernel 参数为NULL，将使用API进行操作，否则使用优化后的函数执行
	bool StretchBltFrom(CDrawingBoard& FromBoard, RECT FromRct, RECT ToRct);
	bool StretchBltFrom(HDC hDc, RECT FromRct, RECT ToRct);

	// 绘制到另外一个内存DC上，如果 pUiKernel 参数为NULL，将使用API进行操作，否则使用优化后的函数执行
	bool BitBltTo(CDrawingBoard& ToBoard, RECT ToRct, RECT FromRct);
	bool BitBltTo(HDC hDc, RECT ToRct, RECT FromRct);

	// 从一个DC进行复制，如果 pUiKernel 参数为NULL，将使用API进行操作，否则使用优化后的函数执行
	bool BitBltFrom(CDrawingBoard& FromBoard, RECT FromRct, RECT ToRct);
	bool BitBltFrom(HDC hDc, RECT FromRct, RECT ToRct);

	void RotateTo(HDC hDc, RECT& dstRect, int iAngle);
	DWORD GetBitLen();
	void  Reset();
protected:
	HBITMAP m_hBmp;
	HBITMAP m_hOldBmp;
	HDC m_hDC;
	BYTE* m_pBits;
	SIZE m_DcSize;
	DWORD m_dwBitLen;

	typedef POINT	PLGBLTPNTS[3];
};
