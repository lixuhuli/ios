#ifndef __ANIMATOR_H__
#define __ANIMATOR_H__

#include "AnimInterface.h"
#include "Tween.h"
#include "../Utils/UIDelegate.h"

#define INVALID_ANIMATOR_ID -1

#define EventAnimatorProgressed  _T("EventAnimatorProgressed")
#define EventAnimatorEnd _T("EventAnimatorEnd")
#define EventAnimatorStart _T("EventAnimatorStart")

#define ANIM_CURRENT_POINT   DuiLib::QPoint(0xffffee,0xffffee)
#define ANIM_CURRENT_VALUE  0xffffee
#define ANIM_CURRENT_SIZE   ANIM_CURRENT_POINT
#define ANIM_CURRENT_POS    ANIM_CURRENT_POINT

class UILIB_API CAnimator : public IAnimationListener, public DuiLib::CEventSets
{
public:
	enum E_STATE
	{
		STATE_NONE = 0,
		STATE_START_DELAY_PERIOD,
		STATE_DURATION_PERIOD,
		STATE_REVERSE_PERIOD,
		STATE_IDLE_PERIOD,
		STATE_END
	};
public:
    CAnimator(int nID, IAnimObject *pAnimObj, CAnimation* pAnimation);//deprecated compatible with old version
	CAnimator(IAnimObject *pAnimObj, CTween::Type eType, int iDuration, bool bTimesensive = true, bool bUserThreadTimer = false);
    virtual ~CAnimator();

public:
	virtual void OnAnimatonStart(CAnimation* pAnimation);//used alone with animation not in storyboard
	virtual void OnAnimationProgressed(CAnimation* pAnimation);
    virtual void OnAnimationStoped(CAnimation* pAnimation);
	virtual void Reverse(){};

public:
    virtual void AddFactor(sAnimatorFactor* pFactor) { }
    virtual void ClearFactors() { }
    virtual void Start();
    virtual void Stop();
	virtual void Reset();
	void StepAnimator(CQuiTimer* pTimer, CAnimator* pParent);

public:
    CAnimation* GetAnimation() const { return m_pAnimation; }
    int GetID() const { return m_nID; }
	void SetID(int ID){m_nID = ID;};
    void SetDuration(int nDuration);
	void SetParams(int iDelayStartTime, int iDuration, int iReverseTime, int iIdleTime);

	int GetNeedTimeFromRootParent();
	int GetNeedTime();
	bool IsOver();//动画是否结束
	void SetParent(CAnimator* pAnimator);
	void SetStartTime(int iStartTime);
	void SetAnimatorListener(IAnimatorListener* pListener);
	void SetInStoryBoard();
protected:
    bool OnEvent_Impl(void* p);

protected:
    int m_nID;
	IAnimObject*                 m_pAnimObject;
    CAnimation*						m_pAnimation;

	int m_iStartTime;
	int m_iDelayStartTime;
	int m_iDuration;
	int m_iReverseDuration;
	int m_iIdleTime;
	int m_iRepeatCount;
	int m_iHasRepeatedCount;

	bool m_bIsEnded;
	E_STATE m_state;
	CAnimator* m_pParent;

	IAnimatorListener* m_pListener;

	bool m_bInStoryBoard;//兼容独立使用animator时，时间驱动是由animation发起的而不是storyboard发起的

	//animator in storyboard design

	//|---------------------------|-------------------|-------------------------|---------------|
	//start m_iDelayStartTime           duration              reverseDuration        idleTime   end
	//                                                           反向运动时长
	//                            |<----------------------------------------------------------->|
	//                                                         repeat
};

#endif