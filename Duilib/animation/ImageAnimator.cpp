#include "StdAfx.h"
#include "ImageAnimator.h"
#include "AnimationFactor.h"
#include "../Core/UIControl.h"

using namespace DuiLib;
CImageAnimationAlgorithm::CImageAnimationAlgorithm(DuiLib::CControlUI* pControl, bool bSingleBitmapAlgorithm)
	: m_pControl(pControl), 
	m_bSingleBitmapAlgorithm(bSingleBitmapAlgorithm), 
	m_hSourceImageBitmap(NULL),
	m_hDestImageBitmap(NULL),
	m_hDestBitmap(NULL)
{
}

CImageAnimationAlgorithm::CImageAnimationAlgorithm(IAnimObject* pAnimInterface, bool bSingleBitmapAlgorithm)
	: m_pAnimObject(pAnimInterface), 
	m_bSingleBitmapAlgorithm(bSingleBitmapAlgorithm), 
	m_hSourceImageBitmap(NULL),
	m_hDestImageBitmap(NULL),
	m_hDestBitmap(NULL)
{

}

CImageAnimationAlgorithm::~CImageAnimationAlgorithm()
{
	RemoveBitmaps();
}

void CImageAnimationAlgorithm::RemoveBitmaps()
{
	if (m_hSourceImageBitmap != NULL)
	{
		::DeleteObject(m_hSourceImageBitmap);
		m_hSourceImageBitmap = NULL;
	}

	if (m_hDestImageBitmap != NULL)
	{
		::DeleteObject(m_hDestImageBitmap);
		m_hDestImageBitmap = NULL;
	}

	m_hDestBitmap = NULL;
}

void CImageAnimationAlgorithm::SetSourceBitmap(HBITMAP hBitmap)
{
	if (m_bSingleBitmapAlgorithm || hBitmap == m_hSourceImageBitmap || hBitmap == NULL)
		return;

	if (m_hSourceImageBitmap != NULL)
		::DeleteObject(m_hSourceImageBitmap);
	m_hSourceImageBitmap = hBitmap;
}

HBITMAP CImageAnimationAlgorithm::GenerateEmptyBitmap()
{
	QRect rectClient = m_pAnimObject->GetPos();
	int nWidth = rectClient.GetWidth();
	int nHeight = rectClient.GetHeight();

	BITMAPINFO bmi;
	::ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = nWidth;
	bmi.bmiHeader.biHeight = -nHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = nWidth * nHeight * 4;

	LPBYTE pDest = NULL;
	HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);

	return hBitmap;
}

HBITMAP CImageAnimationAlgorithm::GenerateBitmap(DuiLib::CStdString& strImagePath)
{
	//    RECT rcItem = {0, 0, m_pControl->GetFixedWidth(), m_pControl->GetFixedHeight()};
	QRect rectClient = m_pAnimObject->GetPos();
	RECT rcItem = {0, 0, rectClient.right - rectClient.left, rectClient.bottom - rectClient.top};
	HBITMAP hBitmap = GenerateEmptyBitmap();
	if (hBitmap == NULL)
		return NULL;

	HDC hDC = ::GetDC(NULL);
	HDC hDCClone = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDCClone, hBitmap);

	DuiLib::CRenderEngine::DrawImageString(hDCClone, /*m_pControl->GetManager()*/m_pAnimObject->GetManager(), rcItem, rcItem, strImagePath);

	::SelectObject(hDCClone, hOldBitmap);
	::DeleteDC(hDCClone);
	::ReleaseDC(NULL, hDC);

	return hBitmap;
}

bool CImageAnimationAlgorithm::GenerateSourceBitmap(DuiLib::CStdString& strImagePath)
{
	if (m_hSourceImageBitmap != NULL)
	{
		DeleteObject(m_hSourceImageBitmap);
		m_hSourceImageBitmap = NULL;
	}

	m_hSourceImageBitmap = GenerateBitmap(strImagePath);
	if (m_hSourceImageBitmap == NULL)
		return false;

	return true;
}

bool CImageAnimationAlgorithm::GenerateDestBitmap(DuiLib::CStdString& strImagePath)
{
	if (m_hDestImageBitmap != NULL)
	{
		DeleteObject(m_hDestImageBitmap);
		m_hDestImageBitmap = NULL;
	}

	m_hDestImageBitmap = GenerateBitmap(strImagePath);
	if (m_hDestImageBitmap == NULL)
		return false;

	return true;
}

CRollingImageAnimation::CRollingImageAnimation(sRollingImageAnimationFactor* pFactor, DuiLib::CControlUI* pControl)
	: m_pFactor(pFactor), CImageAnimationAlgorithm(pControl, false)
{
}

CRollingImageAnimation::CRollingImageAnimation(sRollingImageAnimationFactor *pFactor, IAnimObject *pAnimInterface)
	: m_pFactor(pFactor), CImageAnimationAlgorithm(pAnimInterface, false)
{

}

CRollingImageAnimation::~CRollingImageAnimation()
{
	delete m_pFactor;
}

HBITMAP CRollingImageAnimation::GetCurrentBitmap(CAnimation* pAnimation)
{
	if (pAnimation == NULL)
		return NULL;

	if (m_hDestImageBitmap == NULL && !GenerateDestBitmap(m_pFactor->strDestImage))
		return NULL;

	RECT rcDestBounding = GetDestBounding(pAnimation);
	if ((rcDestBounding.bottom - rcDestBounding.top) == 0
		|| (rcDestBounding.right - rcDestBounding.left) == 0)
		return NULL;

	HDC hDC = ::GetDC(NULL);
	if (m_hDestBitmap == NULL)
		m_hDestBitmap = GenerateEmptyBitmap();
	HDC hDCClone = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDCClone, m_hDestBitmap);

	QRect rect = m_pAnimObject->GetPos();
	RECT rcItem = { 0, 0, rect.GetWidth(), rect.GetHeight() };
	RECT rcCorner = { 0 };
	DuiLib::CRenderEngine::DrawImage(
		hDCClone, m_hDestImageBitmap, rcDestBounding, rcItem, rcItem, rcCorner, true);

	::SelectObject(hDCClone, hOldBitmap);
	::DeleteDC(hDCClone);
	::ReleaseDC(NULL, hDC);

	return m_hDestBitmap;
}

RECT CRollingImageAnimation::GetDestBounding(CAnimation* pAnimation)
{
	int nWidth = 0;
	int nHeight = 0;

	QRect rectItem = m_pAnimObject->GetPos();
	int nControlWidth = rectItem.GetWidth();
	int nControlHeight = rectItem.GetHeight();

	switch (m_pFactor->aDirection)
	{
	case IMAGEANIMATION_XROLLING:
		nWidth = pAnimation->CurrentValueBetween(nControlWidth / 10, nControlWidth);
		nHeight = nControlHeight;
		break;
	case IMAGEANIMATION_YROLLING:
		nWidth = nControlWidth;
		nHeight = pAnimation->CurrentValueBetween(nControlHeight / 10, nControlHeight);
		break;
	case IMAGEANIMATION_XYROLLING:
		nWidth = pAnimation->CurrentValueBetween(nControlWidth / 10, nControlWidth);
		nHeight = pAnimation->CurrentValueBetween(nControlHeight / 10, nControlHeight);
		break;
	default:
		return DuiLib::QRect(0, 0, 0, 0);
	}

	if (nWidth == 0 || nHeight == 0)
		return DuiLib::QRect(0, 0, 0, 0);

	int nTop = (nControlHeight - nHeight) / 2;
	int nLeft = (nControlWidth - nWidth) / 2;
	return DuiLib::QRect(nLeft, nTop, nLeft + nWidth, nTop + nHeight);
}

CTransitionImageAnimation::CTransitionImageAnimation(sTransitionImageAnimationFactor* pFactor, DuiLib::CControlUI* pControl)
	: m_pFactor(pFactor), CImageAnimationAlgorithm(pControl, true)
{

}

CTransitionImageAnimation::CTransitionImageAnimation(sTransitionImageAnimationFactor* pFactor, IAnimObject* pAnimInterface)
	:m_pFactor(pFactor), CImageAnimationAlgorithm(pAnimInterface, true)
{

}

CTransitionImageAnimation::~CTransitionImageAnimation()
{
	delete m_pFactor;
}

HBITMAP CTransitionImageAnimation::GetCurrentBitmap(CAnimation* pAnimation)
{
	if (pAnimation == NULL)
		return NULL;

	if (m_hDestImageBitmap == NULL && !GenerateDestBitmap(m_pFactor->strDestImage))
		return NULL;
	if (m_hSourceImageBitmap == NULL && !GenerateSourceBitmap(m_pFactor->strSourceImage))
		return NULL;

	RECT rcSourceImage_SourceBounding;
	RECT rcSourceImage_DestBounding;
	RECT rcDestImage_SourceBounding;
	RECT rcDestImage_DestBounding;
	if (!GetImageBounding(
		pAnimation, rcSourceImage_SourceBounding, 
		rcSourceImage_DestBounding, rcDestImage_SourceBounding, rcDestImage_DestBounding))
		return NULL;

	HDC hDC = ::GetDC(NULL);
	if (m_hDestBitmap == NULL)
		m_hDestBitmap = GenerateEmptyBitmap();
	HDC hDCClone = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDCClone, m_hDestBitmap);

	QRect rect = m_pAnimObject->GetPos();
	RECT rcItem = {0, 0, rect.GetWidth(), rect.GetHeight()};
	RECT rcCorner = { 0 };
	DuiLib::CRenderEngine::DrawImage(
		hDCClone, m_hSourceImageBitmap, rcSourceImage_DestBounding, rcItem, rcSourceImage_SourceBounding, rcCorner, true);
	DuiLib::CRenderEngine::DrawImage(
		hDCClone, m_hDestImageBitmap, rcDestImage_DestBounding, rcItem, rcDestImage_SourceBounding, rcCorner, true);

	::SelectObject(hDCClone, hOldBitmap);
	::DeleteDC(hDCClone);
	::ReleaseDC(NULL, hDC);

	return m_hDestBitmap;
}

bool CTransitionImageAnimation::GetImageBounding(CAnimation* pAnimation, RECT& rcSource1, RECT& rcSource2, RECT& rcDest1, RECT& rcDest2)
{
	int nWidth = 0;
	int nHeight = 0;

	QRect rectItem = m_pAnimObject->GetPos();
	int nControlWidth = rectItem.GetWidth();
	int nControlHeight = rectItem.GetHeight();

	switch (m_pFactor->aDirection)
	{
	case IMAGEANIMATION_TOP2BOTTOM:
		{
			nHeight = pAnimation->CurrentValueBetween(0, nControlHeight);
			rcSource1 = DuiLib::QRect(0, 0, nControlWidth, nControlHeight - nHeight);
			rcSource2 = DuiLib::QRect(0, nHeight, nControlWidth, nControlHeight);
			rcDest1 = DuiLib::QRect(0, nControlHeight - nHeight, nControlWidth, nControlHeight);
			rcDest2 = DuiLib::QRect(0, 0, nControlWidth, nHeight);
		}
		break;
	case IMAGEANIMATION_BOTTOM2TOP:
		{
			nHeight = pAnimation->CurrentValueBetween(0, nControlHeight);
			rcSource1 = DuiLib::QRect(0, nHeight, nControlWidth, nControlHeight);
			rcSource2 = DuiLib::QRect(0, 0, nControlWidth, nControlHeight - nHeight);
			rcDest1 = DuiLib::QRect(0, 0, nControlWidth, nHeight);
			rcDest2 = DuiLib::QRect(0, nControlHeight - nHeight, nControlWidth, nControlHeight);
		}
		break;
	case IMAGEANIMATION_LEFT2RIGHT:
		{
			nWidth = pAnimation->CurrentValueBetween(0, nControlWidth);
			rcSource1 = DuiLib::QRect(0, 0, nControlWidth - nWidth, nControlHeight);
			rcSource2 = DuiLib::QRect(nWidth, 0, nControlWidth, nControlHeight);
			rcDest1 = DuiLib::QRect(nControlWidth - nWidth, 0, nControlWidth, nControlHeight);
			rcDest2 = DuiLib::QRect(0, 0, nWidth, nControlHeight);
		}
		break;
	case IMAGEANIMATION_RIGHT2LEFT:
		{
			nWidth = pAnimation->CurrentValueBetween(0, nControlWidth);
			rcSource1 = DuiLib::QRect(nWidth, 0, nControlWidth, nControlHeight);
			rcSource2 = DuiLib::QRect(0, 0, nControlWidth - nWidth, nControlHeight);
			rcDest1 = DuiLib::QRect(0, 0, nWidth, nControlHeight);
			rcDest2 = DuiLib::QRect(nControlWidth - nWidth, 0, nControlWidth, nControlHeight);
		}
		break;
	default:
		return false;
	}

	return true;
}

//CImageAnimator::CImageAnimator(int nID, IAnimObject *pAnimInterface, CAnimation* pAnimation)
//: CAnimator(nID, pAnimInterface, pAnimation), m_hDestBitmap(NULL), m_pDestFactor(NULL)
//{
//
//}

CImageAnimator::CImageAnimator( IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration, sAnimatorFactor* pFactor, bool bUseOutFactor /*= true*/, bool bTimeSensive /*= false*/, bool bUserThreadTimer /*= false*/ ):
CAnimator(pAnimObject, tweenType, iDuration, bTimeSensive, bUserThreadTimer)
{
	m_bUseOutFactor = bUseOutFactor;
	AddFactor(pFactor);
}

CImageAnimator::~CImageAnimator()
{
    for (AlgorithmArray::iterator iAlgorithm = m_arrAlgorithm.begin();
        iAlgorithm != m_arrAlgorithm.end(); ++iAlgorithm)
    {
        delete *iAlgorithm;
    }
	if(m_bUseOutFactor)
		SAFE_DELETE(m_pDestFactor);
}

void CImageAnimator::AddFactor(sAnimatorFactor* pFactor)
{
    sImageAnimationFactor* pImageFactor = static_cast< sImageAnimationFactor* >(pFactor);
    if (pImageFactor == NULL)
        return;

    m_pDestFactor = pImageFactor;
	CImageAnimationAlgorithm* pAlgorithm = pImageFactor->CreateAlgorithmObject(m_pAnimObject);
    if (pAlgorithm == NULL)
        return;

    m_arrAlgorithm.push_back(pAlgorithm);
}

void CImageAnimator::ClearFactors()
{
    for (AlgorithmArray::iterator iAlgorithm = m_arrAlgorithm.begin();
        iAlgorithm != m_arrAlgorithm.end(); ++iAlgorithm)
    {
        delete *iAlgorithm;
    }

    m_arrAlgorithm.clear();
}

void CImageAnimator::OnAnimationProgressed(CAnimation* pAnimation)
{
    if (m_pAnimObject == NULL || m_arrAlgorithm.empty())
    {
        Stop();
        return;
    }

//    m_pControl->FireAnimProgressedEvent(pAnimation);
	//m_pAnimObject->FireAnimationProgressedEvent(pAnimation);

    m_hDestBitmap = NULL;
    for (AlgorithmArray::iterator iAlgorithm = m_arrAlgorithm.begin();
        iAlgorithm != m_arrAlgorithm.end(); ++iAlgorithm)
    {
        if (m_hDestBitmap != NULL)
            (*iAlgorithm)->SetSourceBitmap(m_hDestBitmap);

        m_hDestBitmap = (*iAlgorithm)->GetCurrentBitmap(pAnimation);
        if (m_hDestBitmap == NULL)
        {
            Stop();
            return;
        }
    }
	m_pAnimObject->SetBkBitmap(m_hDestBitmap);
	CAnimator::OnAnimationProgressed(pAnimation);
}

void CImageAnimator::OnAnimationStoped(CAnimation* pAnimation)
{
    m_hDestBitmap = NULL;
    SetBitamp(NULL);
    
    DuiLib::CStdString strDestImagePath;
    if (m_pDestFactor != NULL)
        strDestImagePath = m_pDestFactor->strDestImage;
    if (strDestImagePath.GetLength() > 0)
        SetImagePath(strDestImagePath);

    for (AlgorithmArray::iterator iAlgorithm = m_arrAlgorithm.begin();
        iAlgorithm != m_arrAlgorithm.end(); ++iAlgorithm)
    {
        (*iAlgorithm)->RemoveBitmaps();
    }

    CAnimator::OnAnimationStoped(pAnimation);
}

CBKImageAnimator::CBKImageAnimator( IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration, sAnimatorFactor* pFactor, bool bUseOutFactor /*= true*/, bool bTimeSensive /*= false*/, bool bUserThreadTimer /*= false*/ ) :
CImageAnimator(pAnimObject, tweenType, iDuration,pFactor, bUseOutFactor, bTimeSensive, bUserThreadTimer)
{
}

CBKImageAnimator::~CBKImageAnimator()
{
}

void CBKImageAnimator::SetBitamp(HBITMAP hBmp)
{
	if (m_pAnimObject != NULL)
	{
		m_pAnimObject->SetBkBitmap(hBmp);
	}
}

void CBKImageAnimator::SetImagePath(DuiLib::CStdString strImagePath)
{
	if (m_pAnimObject != NULL)
	{
		m_pAnimObject->SetBkImage(strImagePath);
	}
}

//------------------CImageApproachAnimator----------------------
CImageApproachAnimator::CImageApproachAnimator(IAnimObject* pAnimObject,CTween::Type tweenType
	, int iDuration,bool bTimeSensive, bool bUserThreadTimer)
	:CAnimator(pAnimObject,tweenType,iDuration,bTimeSensive,bUserThreadTimer)
{
	m_pSrcImg = NULL;
	m_pDstImg = NULL;
	m_hDrawImg = NULL;
}

CImageApproachAnimator::~CImageApproachAnimator()
{
	ClearImage();
}

void CImageApproachAnimator::OnAnimatonStart(CAnimation* pAnimation)
{
	if (m_pAnimObject && m_pAnimObject->GetManager())
	{
		m_hDrawImg = CreateCompatibleBitmap(m_pAnimObject->GetManager()->GetPaintDC(),m_pSrcImg->nX,m_pSrcImg->nY);
		m_pAnimObject->SetBkBitmap(m_hDrawImg);
	}
}

void CImageApproachAnimator::OnAnimationProgressed(CAnimation* pAnimation)
{
	int alpha = pAnimation->CurrentValueBetween(0,255);
	HDC srcDC = CreateCompatibleDC(::GetDC(m_pAnimObject->GetManager()->GetPaintWindow()));
	HDC dstDC = CreateCompatibleDC(::GetDC(m_pAnimObject->GetManager()->GetPaintWindow()));
	HDC drawDC = CreateCompatibleDC(::GetDC(m_pAnimObject->GetManager()->GetPaintWindow()));

	HBITMAP old1 = (HBITMAP)SelectObject(srcDC,m_pSrcImg->hBitmap);
	HBITMAP old2 = (HBITMAP)SelectObject(dstDC,m_pDstImg->hBitmap);
	HBITMAP old3 = (HBITMAP)SelectObject(drawDC,m_hDrawImg);

	::BitBlt(drawDC,0,0,m_pSrcImg->nX,m_pSrcImg->nY,srcDC,0,0,SRCCOPY);

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
	BOOL ok = AlphaBlend(drawDC,0,0,m_pSrcImg->nX,m_pSrcImg->nY,dstDC,0,0,m_pDstImg->nX,m_pDstImg->nY,bf);

	SelectObject(srcDC,old1);
	SelectObject(dstDC,old2);
	SelectObject(drawDC,old3);

	DeleteDC(srcDC);
	DeleteDC(dstDC);
	DeleteDC(drawDC);
	RECT rc = m_pAnimObject->GetPos();
	m_pAnimObject->SetPosition(rc.left,rc.top);
	CAnimator::OnAnimationProgressed(pAnimation);
}

void CImageApproachAnimator::OnAnimationStoped(CAnimation* pAnimation)
{
	m_pAnimObject->SetBkBitmap(NULL);
	CAnimator::OnAnimationStoped(pAnimation);
}

BOOL CImageApproachAnimator::SetImage(LPCTSTR pstrSrc, LPCTSTR pstrDst)
{
	m_strSrcImg = pstrSrc;
	m_strDstImg = pstrDst;

	ClearImage();
	m_pSrcImg = CRenderEngine::LoadImage(pstrSrc);
	m_pDstImg = CRenderEngine::LoadImage(pstrDst);
	if (NULL== m_pSrcImg || NULL == m_pDstImg || m_pSrcImg->nX != m_pDstImg->nX || m_pSrcImg->nY != m_pDstImg->nY)
	{
		ClearImage();
		return FALSE;
	}
	
	return TRUE;
}

void CImageApproachAnimator::ClearImage()
{
	if (m_pSrcImg != NULL)
	{
		DeleteObject(m_pSrcImg->hBitmap);
		SAFE_DELETE(m_pSrcImg);
	}
	if (m_pDstImg != NULL)
	{
		DeleteObject(m_pDstImg->hBitmap);
		SAFE_DELETE(m_pDstImg);
	}
	//DeleteObject(m_hDrawImg);//交由m_pAnimObject来释放
	m_hDrawImg = NULL;
}