
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

// ��ʼ����
#define INIT_POINT(pt)								((pt).x=(pt).y=0)
// �õ����ο��
#define RECT_WIDTH(GetRct)							((GetRct).right-(GetRct).left)
// �õ����ο��
#define RECT_HEIGHT(GetRct)							((GetRct).bottom-(GetRct).top)
// ��ʼ������Ϊ��
#define INIT_RECT(SetEmRect)						((SetEmRect).left=(SetEmRect).right=(SetEmRect).top=(SetEmRect).bottom=0)
// ���þ���
#define SET_RECT(SetRct,nL,nT,nR,nB)				((SetRct).left=(nL),(SetRct).top=(nT),(SetRct).right=(nR),(SetRct).bottom=(nB))
#define SET_RECT_WH(SetRct,nL,nT,nWidth,nHeight)	((SetRct).left=(nL),(SetRct).top=(nT),(SetRct).right=(nL)+(nWidth),(SetRct).bottom=(nT)+(nHeight))
// �жϾ����Ƿ�Ϊ��
#define IS_RECT_NOT_EMPTY(CheckRect)				((RECT_WIDTH(CheckRect)>0)&&((RECT_HEIGHT(CheckRect)>0)))
// �жϾ����Ƿ�Ϊ��
#define IS_RECT_EMPTY(CheckRect)					((RECT_WIDTH(CheckRect)<=0)||(RECT_HEIGHT(CheckRect)<=0))

// ���GDI+��Alpha�ں���Alpha�ںϽ�λ�������⣬����Ĭ�ϵ�Alpha�����ֵΪ254������255
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

	// ������
	// nWidth�����
	// nHeight���߶�
	// nDefaultColor��Ĭ�������ɫֵ
	// bReCreate��ÿ�ζ����´��������ܿ�Ⱥ͸߶��Ƿ����仯
	// bNoCreateInit��û�����´�����������Ƿ�ʹ��Ĭ����ɫֵ��ʼ���������ݿռ�
	// ����ֵ���壺�Ƿ񴴽���һ���µ��ڴ�DC
	bool Create(int nWidth, int nHeight, bool bReCreate = false, bool bNoCreateInit = false);
	bool Create(HWND hWnd, int iWidth, int iHeight);
	void Delete();
	HBITMAP& GetBmpHandle();
	HDC& GetSafeHdc();
	// ���� BGRA ˳���Ų���32λλͼ��Ϣ
	BYTE* GetBits();
	SIZE GetDcSize();

	// ����һ���ڴ�DC��¡
	bool Clone(CDrawingBoard& FromBoard);

	// ���Ƶ�����һ���ڴ�DC��
	bool AlphaBlendTo(CDrawingBoard& ToBoard, RECT ToRct, RECT FromRct,  int nAlpha = 255, bool bFillAlphaChannel = false);
	bool AlphaBlendTo(HDC hDc, RECT ToRct, RECT FromRct, int nAlpha = 255, bool bFillAlphaChannel = false);

	// ��һ��DC���и��ƣ���� pUiKernel ����ΪNULL����ʹ��API���в���������ʹ���Ż���ĺ���ִ��
	bool StretchBltFrom(CDrawingBoard& FromBoard, RECT FromRct, RECT ToRct);
	bool StretchBltFrom(HDC hDc, RECT FromRct, RECT ToRct);

	// ���Ƶ�����һ���ڴ�DC�ϣ���� pUiKernel ����ΪNULL����ʹ��API���в���������ʹ���Ż���ĺ���ִ��
	bool BitBltTo(CDrawingBoard& ToBoard, RECT ToRct, RECT FromRct);
	bool BitBltTo(HDC hDc, RECT ToRct, RECT FromRct);

	// ��һ��DC���и��ƣ���� pUiKernel ����ΪNULL����ʹ��API���в���������ʹ���Ż���ĺ���ִ��
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
