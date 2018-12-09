#ifndef __MOVEANIMATOR_H__
#define __MOVEANIMATOR_H__

#include "Animator.h"
#include "ScaleAnimator.h"
#include "../core/UIBase.h"

class UILIB_API CMoveAnimator : public CAnimator
{
public:
	CMoveAnimator(IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration, DuiLib::QPoint qFrom, DuiLib::QPoint qTo, bool bIsToAsRelativeValue = false, bool bReCalculateCurrentValue = false, bool bRelativePos = true, bool bTimeSensive = false, bool bUserThreadTimer = false);
	~CMoveAnimator();

	CScaleAnimator *AxisPartner(CScaleAnimator *pPartner, BOOL bSetOrGet=TRUE);
protected:
	void OnAnimationProgressed(CAnimation* pAnimation);
	void OnAnimatonStart(CAnimation* pAnimation);

	DuiLib::QPoint ConvertPointToLeftTop(DuiLib::QPoint srcPoint, SIZE sz, E_AXIS eAxis);
private:
	DuiLib::QPoint m_fromPos;
	DuiLib::QPoint m_toPos;
	bool   m_bIsToAsRelativeValue;
	DuiLib::QPoint m_userFromPos;
	bool   m_bReCalculateCurrentValue;
	bool m_bReletivePos;
};

#endif//end __MOVEANIMATOR_H__