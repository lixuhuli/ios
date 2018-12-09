#ifndef __FADEANIMATOR_H__
#define __FADEANIMATOR_H__

#include "Animator.h"
#include "../Core/UIBase.h"
#include "PropertyAnimatior.h"

class UILIB_API CFadeAnimator : public CPropertyAnimatior
{
public:
	CFadeAnimator(IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration,int iFrom, int iTo, bool bIsToAsRelativeValue = false, bool bTimeSensive = false, bool bUserThreadTimer = false);
	~CFadeAnimator();
};

#endif //__FADEANIMATOR_H__



