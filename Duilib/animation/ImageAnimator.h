#ifndef __IMAGEANIMATOR_H__
#define __IMAGEANIMATOR_H__

#include "Animator.h"

namespace DuiLib
{
	class CControlUI;
};

class CImageAnimationAlgorithm
{
public:
	CImageAnimationAlgorithm(DuiLib::CControlUI* pControl, bool bSingleBitmapAlgorithm);
	CImageAnimationAlgorithm(IAnimObject* pAnimInterface, bool bSingleBitmapAlgorithm);
	virtual ~CImageAnimationAlgorithm();

public:
	virtual HBITMAP GetCurrentBitmap(CAnimation* pAnimation) { return NULL; }

public:
	void RemoveBitmaps();
	void SetSourceBitmap(HBITMAP hBitmap);

protected:
	HBITMAP GenerateEmptyBitmap();
	HBITMAP GenerateBitmap(DuiLib::CStdString& strImagePath);
	bool GenerateSourceBitmap(DuiLib::CStdString& strImagePath);
	bool GenerateDestBitmap(DuiLib::CStdString& strImagePath);

protected:
	DuiLib::CControlUI* m_pControl;
	IAnimObject*     m_pAnimObject;

	HBITMAP m_hSourceImageBitmap;
	HBITMAP m_hDestImageBitmap;

	// 用于标识目标位图, 此块内存由绘制层管理
	HBITMAP m_hDestBitmap;

	bool m_bSingleBitmapAlgorithm;
};

struct sRollingImageAnimationFactor;
class CRollingImageAnimation : public CImageAnimationAlgorithm
{
public:
	CRollingImageAnimation(sRollingImageAnimationFactor* pFactor, DuiLib::CControlUI* pControl);
	CRollingImageAnimation(sRollingImageAnimationFactor *pFactor, IAnimObject *pAnimInterface);
	virtual ~CRollingImageAnimation();

public:
	virtual HBITMAP GetCurrentBitmap(CAnimation* pAnimation);

protected:
	RECT GetDestBounding(CAnimation* pAnimation);

private:
	sRollingImageAnimationFactor* m_pFactor;
};

struct sTransitionImageAnimationFactor;
class CAnimation;
class CTransitionImageAnimation : public CImageAnimationAlgorithm
{
public:
	CTransitionImageAnimation(sTransitionImageAnimationFactor* pFactor, DuiLib::CControlUI* pControl);
	CTransitionImageAnimation(sTransitionImageAnimationFactor* pFactor, IAnimObject* pAnimInterface);
	virtual ~CTransitionImageAnimation();

public:
	virtual HBITMAP GetCurrentBitmap(CAnimation* pAnimation);

protected:
	// 函数功能: 返回相应图像区域
	// 参数:
	//      rcSource1: 源图像贴图时的源区域
	//      rcSource2: 源图像贴图时的目的区域
	//      rcDest1: 目的图像贴图时的源区域
	//      rcDest2: 目的图像贴图时的目的区域
	bool GetImageBounding(CAnimation* pAnimation, RECT& rcSource1, RECT& rcSource2, RECT& rcDest1, RECT& rcDest2);

private:
	sTransitionImageAnimationFactor* m_pFactor;
};

struct sImageAnimationFactor;
class UILIB_API CImageAnimator : public CAnimator
{
private:
    typedef std::vector< CImageAnimationAlgorithm* > AlgorithmArray;

public:
    //CImageAnimator(int nID, IAnimObject *pAnimInterface, CAnimation* pAnimation);
	CImageAnimator(IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration, sAnimatorFactor* pFactor, bool bUseOutFactor = true, bool bTimeSensive = false, bool bUserThreadTimer = false);
    virtual ~CImageAnimator();

public:
    virtual void SetBitamp(HBITMAP hBmp) { }
    virtual void SetImagePath(DuiLib::CStdString strImagePath) { }
protected:
	virtual void AddFactor(sAnimatorFactor* pFactor);
	virtual void ClearFactors();
    virtual void OnAnimationProgressed(CAnimation* pAnimation);
    virtual void OnAnimationStoped(CAnimation* pAnimation);

private:
    AlgorithmArray m_arrAlgorithm;
    HBITMAP m_hDestBitmap;
    sImageAnimationFactor* m_pDestFactor;
	bool m_bUseOutFactor;
};

class UILIB_API CBKImageAnimator : public CImageAnimator
{ 
public:
	CBKImageAnimator(IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration, sAnimatorFactor* pFactor, bool bUseOutFactor = true, bool bTimeSensive = false, bool bUserThreadTimer = false);    
    virtual ~CBKImageAnimator();

public:
    virtual void SetBitamp(HBITMAP hBmp);
    virtual void SetImagePath(DuiLib::CStdString strImagePath);
};

//图片逼近动画，从源图片向目标图片逼近,要求源与目录图片大小相同且32位图。
class UILIB_API CImageApproachAnimator : public CAnimator
{
public:
	CImageApproachAnimator(IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration
		,bool bTimeSensive = false, bool bUserThreadTimer = false);
	~CImageApproachAnimator();

	BOOL SetImage(LPCTSTR pstrSrc, LPCTSTR pstrDst);
protected:
	virtual void OnAnimatonStart(CAnimation* pAnimation);
	virtual void OnAnimationProgressed(CAnimation* pAnimation);
	virtual void OnAnimationStoped(CAnimation* pAnimation);
	void ClearImage();
private:
    DuiLib::CStdString		m_strSrcImg;//原图片绝对地址
    DuiLib::CStdString		m_strDstImg;//目标图片绝对地址
    DuiLib::TImageInfo*		m_pSrcImg;//原图片信息
    DuiLib::TImageInfo*		m_pDstImg;//目录图片信息
	HBITMAP			m_hDrawImg;//绘图过程中实际绘制的图片

};

#endif