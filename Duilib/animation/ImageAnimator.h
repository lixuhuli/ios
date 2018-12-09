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

	// ���ڱ�ʶĿ��λͼ, �˿��ڴ��ɻ��Ʋ����
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
	// ��������: ������Ӧͼ������
	// ����:
	//      rcSource1: Դͼ����ͼʱ��Դ����
	//      rcSource2: Դͼ����ͼʱ��Ŀ������
	//      rcDest1: Ŀ��ͼ����ͼʱ��Դ����
	//      rcDest2: Ŀ��ͼ����ͼʱ��Ŀ������
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

//ͼƬ�ƽ���������ԴͼƬ��Ŀ��ͼƬ�ƽ�,Ҫ��Դ��Ŀ¼ͼƬ��С��ͬ��32λͼ��
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
    DuiLib::CStdString		m_strSrcImg;//ԭͼƬ���Ե�ַ
    DuiLib::CStdString		m_strDstImg;//Ŀ��ͼƬ���Ե�ַ
    DuiLib::TImageInfo*		m_pSrcImg;//ԭͼƬ��Ϣ
    DuiLib::TImageInfo*		m_pDstImg;//Ŀ¼ͼƬ��Ϣ
	HBITMAP			m_hDrawImg;//��ͼ������ʵ�ʻ��Ƶ�ͼƬ

};

#endif