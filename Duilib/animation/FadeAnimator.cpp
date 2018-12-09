#include "StdAfx.h"
#include "FadeAnimator.h"

CFadeAnimator::CFadeAnimator(IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration
								, int iFrom, int iTo, bool bIsToAsRelativeValue
								, bool bTimeSensive, bool bUserThreadTimer)
	:CPropertyAnimatior(pAnimObject,tweenType,iDuration,_T("alpha"),iFrom,iTo,bIsToAsRelativeValue,bTimeSensive,bUserThreadTimer)
{

}

CFadeAnimator::~CFadeAnimator()
{
}


