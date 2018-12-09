#ifndef __PROPERTYANIMATIOR_H__
#define __PROPERTYANIMATIOR_H__

#include "Animator.h"
#include "../core/UIBase.h"

struct sPropertyAnimationFactor;
class UILIB_API CPropertyAnimatior : public CAnimator
{
public:
    CPropertyAnimatior(int nID, IAnimObject *pAnimObject, CAnimation* pAnimation);//deprecated compatible with old version
	CPropertyAnimatior(IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration, QUI_STRING strKey, int iFrom, int iTo, bool bIsToAsRelativeValue = false,bool bReCalculateCurrentValue = false, bool bTimeSensive = false, bool bUserThreadTimer = false);
    virtual ~CPropertyAnimatior();
protected:
    void OnAnimationProgressed(CAnimation* pAnimation);


protected:
	QUI_STRING m_strKey;
	int m_iFrom;
	int m_iTo;
	bool m_bIsToAsRelativeValue;
	bool m_bReCalculateCurrentValue;
	int  m_iUserFrom;
};

class UILIB_API CRotationAnimator : public CPropertyAnimatior
{
public:
	CRotationAnimator(IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration,int iFrom, int iTo, bool bIsToAsRelativeValue = false, bool bTimeSensive = false, bool bUserThreadTimer = false);
	~CRotationAnimator();
};

#endif