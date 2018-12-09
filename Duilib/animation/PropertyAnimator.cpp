#include "StdAfx.h"
#include "PropertyAnimatior.h"
#include "AnimationFactor.h"

CPropertyAnimatior::CPropertyAnimatior(int nID, IAnimObject *pAnimInterface, CAnimation* pAnimation)
: CAnimator(nID, pAnimInterface, pAnimation)
{
}

CPropertyAnimatior::CPropertyAnimatior( IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration, QUI_STRING strKey, int iFrom, int iTo, bool bIsToAsRelativeValue/* = false*/,bool bReCalculateCurrentValue/* = false*/,bool bTimeSensive /*= false*/ , bool bUserThreadTimer/* = false*/) : m_iFrom(iFrom), m_iTo(iTo),
	m_bIsToAsRelativeValue(bIsToAsRelativeValue), CAnimator(pAnimObject, tweenType, iDuration, bTimeSensive, bUserThreadTimer), m_strKey(strKey),m_bReCalculateCurrentValue(bReCalculateCurrentValue)
{
	m_iUserFrom = iFrom;
}

CPropertyAnimatior::~CPropertyAnimatior()
{

}

void CPropertyAnimatior::OnAnimationProgressed(CAnimation* pAnimation)
{
    if (m_pAnimObject == NULL)
    {
        Stop();
        return;
    }
#if 1
	int iTo = m_iTo;
	if(m_bIsToAsRelativeValue)
		iTo = m_iFrom + m_iTo;
	int iValue = m_pAnimation->CurrentValueBetween(m_iFrom, iTo);
	m_pAnimObject->SetKeyWithValue(m_strKey, iValue);
	CAnimator::OnAnimationProgressed(pAnimation);
#else
	RECT rcPos = m_pAnimObject->GetPos();
	m_pAnimObject->FireAnimationProgressedEvent((void *)pAnimation);
    for (PropertyAnimationFactorArray::iterator iFactor = m_arrFactor.begin();
        iFactor != m_arrFactor.end(); ++iFactor)
    {
		POINT pt = pAnimation->CurrentValueBetween((*iFactor)->ptStart, (*iFactor)->ptEnd);

		if ((*iFactor)->strPropertyName == _T("x"))
		{
			int nWidth = rcPos.right - rcPos.left;
			int nHeight = rcPos.bottom - rcPos.top;
	
			rcPos.left = pt.x;
			rcPos.right = rcPos.left + nWidth;
			if (pt.y != -1)
			{
				rcPos.top = pt.y;
				rcPos.bottom = rcPos.top + nHeight;
			}
		}
		else if ((*iFactor)->strPropertyName == _T("y"))
		{
			int nWidth = rcPos.right - rcPos.left;
			int nHeight = rcPos.bottom - rcPos.top;

			rcPos.top = pt.y;
			rcPos.bottom = rcPos.top + nHeight;
			if (pt.x != -1)
			{
				rcPos.left = pt.x;
				rcPos.right = rcPos.left + nWidth;
			}
		}
		else if ((*iFactor)->strPropertyName == _T("width"))
		{
			rcPos.right = pt.x + rcPos.left;
		}
		else if ((*iFactor)->strPropertyName == _T("height"))
		{
			rcPos.bottom = pt.y + rcPos.top;
		}
    }
	m_pAnimObject->SetPos(rcPos);
#endif
}

CRotationAnimator::CRotationAnimator(IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration
	, int iFrom, int iTo, bool bIsToAsRelativeValue
	, bool bTimeSensive, bool bUserThreadTimer)
	:CPropertyAnimatior(pAnimObject,tweenType,iDuration,_T("rotation"),iFrom,iTo,bIsToAsRelativeValue,bTimeSensive,bUserThreadTimer)
{

}

CRotationAnimator::~CRotationAnimator()
{

}