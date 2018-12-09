#ifndef __SCALEANIMATOR_H__
#define __SCALEANIMATOR_H__

#include "Animator.h"
#include "../Core/UIBase.h"

enum E_AXIS
{
	AXIS_LEFT_TOP = 0,
	AXIS_RIGHT_TOP,
	AXIS_CENTER,
	AXIS_LEFT_BOTTOM,
	AXIS_RIGHT_BOTTOM,
	AXIS_CENTER_TOP,
	AXIS_CENTER_BOTTOM,
	AXIS_CENTER_LEFT,
	AXIS_CENTER_RIGHT
};

class UILIB_API CScaleAnimator : public CAnimator
{
public:
	CScaleAnimator(IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration, DuiLib::QPoint qFrom, DuiLib::QPoint qTo,bool bTopLeftAxis = true, bool bIsToAsRelativeValue = false,bool bReCalculateCurrentValue = false, bool bTimeSensive = false, bool bUserThreadTimer = false);
	CScaleAnimator(IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration, DuiLib::QPoint qFrom, DuiLib::QPoint qTo,E_AXIS axis = AXIS_LEFT_TOP, bool bIsToAsRelativeValue = false, bool bReCalculateCurrentValue = false,bool bTimeSensive = false, bool bUserThreadTimer = false);
	~CScaleAnimator();

	E_AXIS GetAxis();
protected:
	void OnAnimationProgressed(CAnimation* pAnimation);
	void OnAnimatonStart(CAnimation* pAnimation);
	void RefreshOrigen();
private:
	DuiLib::QPoint m_fromSize;
	DuiLib::QPoint m_toSize;
	bool   m_bIsToAsRelativeValue;
	DuiLib::QPoint m_origen;
	bool   m_bTopLeftAxis;
	E_AXIS m_eAxis;
	bool   m_bReCalculateCurrentValue;
	DuiLib::QPoint m_userFromSize;
};

#endif//end __MOVEANIMATOR_H__