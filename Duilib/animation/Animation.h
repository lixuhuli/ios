#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "QuiTimer.h"
#include "Tween.h"
#include "AnimInterface.h"

#define INFINITE_REPEAT_COUNT (-1)


class UILIB_API CAnimation : public IQuiTimerHandler
{
public:
	CAnimation(int iFP = 20, bool bUseOutTimer = false, bool bTimesensive = true, bool bUseThreadTimer = false);//50 frame per second
	virtual ~CAnimation();

public:
    virtual void SetDuration(int nDuration) = 0;

public:
	void SetFP(int iFP);
	void SetRepeatCount(int iCount);
	int GetFP(){return m_iFP;};
	void Start();
	void Stop();
	void SetListener(IAnimationListener* pListener){ m_pListener = pListener; }
	virtual double CurrentValueBetween(double dStart, double dEnd) = 0;
	virtual int  CurrentValueBetween(int iStart, int iEnd) = 0;
	virtual POINT CurrentValueBetween(POINT beginP, POINT endP) = 0;
	void OnTimer(CQuiTimer* pTimer);
	void Reset();
	void Restart();
	void SetUseOutTimer(bool bUseOutTimer);
	int GetDuration();
public:
	virtual double GetCurrentValue() = 0;
public:
	virtual void Step(double dTimeElapsed) = 0;

protected:
	IAnimationListener* m_pListener;
	CQuiTimer* m_pTimer;
	int m_iFP;
	int m_iStartTime;//GetTickCount();
	bool m_bAnimating;

	int m_iRepeatCount;
	bool m_bUseOutTimer;
	bool m_bTimesensive;
	bool m_bUserThreadTimer;
	int m_iDuration;
};


#endif//end__ANIMATION_H__