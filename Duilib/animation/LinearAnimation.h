#ifndef __LINEARANIMATION_H__
#define __LINEARANIMATION_H__

#include "Animation.h"

class UILIB_API CLinearAnimation : public CAnimation
{
public:
	CLinearAnimation(int iDuration, IAnimationListener* pListener = NULL, bool bUseOutTimer = false, bool bTimesensive = true, bool bUseThreadTimer = false);
	~CLinearAnimation();
	virtual void SetDuration(int iDuration);
	void Step(double dElapsed);
	void SetTweenType(CTween::Type eType){m_eType = eType;};
	double CurrentValueBetween(double dStart, double dEnd);
	int    CurrentValueBetween(int iStart, int iEnd);
	POINT  CurrentValueBetween(POINT beginP, POINT endP);

protected:
	virtual void AnimateToState(double dValue);
	double GetCurrentValue();

	static double ValueBetween(double value, double start, double target);
	static int    ValueBetween(double value, int start, int target);

protected:
	//int m_iDuration;
	double m_dFactor;
	CTween::Type m_eType;
};

#endif //end __LINEARANIMATION_H__