#ifndef __EFFECT_ANIMATOR_H__
#define __EFFECT_ANIMATOR_H__

#include "Animator.h"

struct  sWndEffectFactor;
class CWndEffectAnimator : public CAnimator
{
public:
};

class CUIEffect;
class UILIB_API CWndReginAnimator : public CAnimator
{
public:
	~CWndReginAnimator();

	CWndReginAnimator(CUIEffect* pEffect, CTween::Type tweenType, int iDuration,  int iPointCount, Gdiplus::Point* pBeginPoints, 
	Gdiplus::Point* pEndPoints,  bool bTimeSensive = false, bool bUserThreadTimer = false);
	
	void OnAnimationProgressed(CAnimation* pAnimation);
private:
	BLENDFUNCTION m_Blend;
	int m_iPointCount;
	Gdiplus::Point* m_pBeginPoints;
	Gdiplus::Point* m_pEndPoints;
	CUIEffect* m_pUiEffect;
};

class UILIB_API CWndFadeAnimator : public CAnimator//deprecated
{
public:
	CWndFadeAnimator(CUIEffect* pEffect, CTween::Type tweenType, int iDuration,  int iAlphaFrom, int iAlphaTo);
	~CWndFadeAnimator();
	void OnAnimatonStart(CAnimation* pAnimation);
	void OnAnimationProgressed(CAnimation* pAnimation);

protected:
	int m_iAlphaFrom;
	int m_iAlphaTo;
	
	BLENDFUNCTION m_Blend;
	CUIEffect* m_pUiEffect;
};


#endif//end __EFFECT_ANIMATOR_H__