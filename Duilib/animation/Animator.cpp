#include "StdAfx.h"
#include "Animator.h"
#include "../Core/UIManager.h"
#include "../Utils/UIDelegate.h"

using namespace DuiLib;
CAnimator::CAnimator(int nID, IAnimObject *pAnimInterface, CAnimation* pAnimation)
: m_pAnimation(pAnimation),
  m_pAnimObject(pAnimInterface), 
  m_nID(nID),
  m_iDelayStartTime(0),
  m_iDuration(0),
  m_iReverseDuration(0),
  m_iIdleTime(0),
  m_iRepeatCount(0),
  m_bIsEnded(false)
{
	m_state = STATE_NONE;
	m_pParent = NULL;
	m_iStartTime = 0;
	m_pListener = 0;
	m_iHasRepeatedCount = 0;
	m_bInStoryBoard = false;
}

CAnimator::CAnimator( IAnimObject *pAnimObj, CTween::Type eType, int iDuration, bool bTimesensive /*= true*/ , bool bUserThreadTimer/* = false*/) :m_pAnimObject(pAnimObj), 
m_nID(INVALID_ANIMATOR_ID),
m_iDelayStartTime(0),
m_iDuration(0),
m_iReverseDuration(0),
m_iIdleTime(0),
m_iRepeatCount(0),
m_bIsEnded(false)
{
	m_state = STATE_NONE;
	m_pParent = NULL;
	m_iStartTime = 0;
	m_pListener = 0;
	m_iHasRepeatedCount = 0;
	m_bInStoryBoard = false;
	m_iDuration = iDuration;
	m_pAnimation = new CLinearAnimation(m_iDuration, this, false, bTimesensive, bUserThreadTimer);
	((CLinearAnimation *)m_pAnimation)->SetTweenType(eType);
	if(m_pAnimObject)
	{
		m_pAnimObject->AddToAnimator(this);
	}
}

CAnimator::~CAnimator()
{
	if(m_pAnimObject)
	{
		m_pAnimObject->RemoveFromAnimator(this);
	}
	if(m_pAnimation)
		delete m_pAnimation;
}

void CAnimator::Start()
{
    if (m_pAnimation == NULL)
        return;

	if(m_state > STATE_NONE)
		Reset();
	m_iStartTime = GetNeedTimeFromRootParent();
	if(m_pListener != NULL)
		m_pListener->OnAnimatorStart(this);

	//if(m_iDelayStartTime == 0)
	if(!m_bInStoryBoard)
	{
		m_pAnimation->Start();
	}
	else
	{
		OnAnimatonStart(m_pAnimation);
	}
	if(!m_bInStoryBoard)
	{
		m_state = STATE_DURATION_PERIOD;
	}
}

void CAnimator::Stop()
{
    if (m_pAnimation == NULL)
        return;

	if(m_bInStoryBoard && m_state >= STATE_DURATION_PERIOD && m_state <= STATE_REVERSE_PERIOD)
		m_pAnimation->Stop();
	else if(!m_bInStoryBoard)
		m_pAnimation->Stop();
	if(m_state > STATE_NONE && !m_bIsEnded)
	{
		m_bIsEnded = true;
		if(m_pListener)
			m_pListener->OnAnimatorStoped(this);
	}

}

void CAnimator::SetDuration(int nDuration)
{
    m_pAnimation->SetDuration(nDuration);
	m_iDuration = nDuration;
}

bool CAnimator::OnEvent_Impl(void* p)
{
    DuiLib::TNotifyUI* pNotify = (DuiLib::TNotifyUI*)p;

    if (NULL == pNotify || NULL == pNotify->pSender)
        return false;

    Stop();
    Start();

    return true;
}

void CAnimator::StepAnimator( CQuiTimer* pTimer , CAnimator* pParent)
{
	int iElapsedTime = /*pTimer->GetElapsedTime()*/ pTimer->GetConstantElapsedTime() - GetNeedTimeFromRootParent();
	//CString strLog;
	//strLog.Format(_T("ielapsed time is %d"), iElapsedTime);
	//OutputDebugString(strLog);
	int iTimeExclude = iElapsedTime - m_iDelayStartTime;
	bool bFlag = true;
	int iRepeatedTime = m_iDuration + m_iReverseDuration + m_iIdleTime;
	//if(m_pListener)
	//	m_pListener->OnAnimatorProgressed(this);
	while(bFlag)
	{
		switch(m_state)
		{
		case STATE_NONE:
			{
				Start();
				if(m_iDelayStartTime > 0)
					m_state = STATE_START_DELAY_PERIOD;
				else
				{
					m_state = STATE_DURATION_PERIOD;
					//m_pAnimation->Start();
				}
			}
			break;
		case  STATE_START_DELAY_PERIOD:
			{
				if(iElapsedTime >= m_iDelayStartTime)
					m_state = STATE_DURATION_PERIOD;
				else
					bFlag = false;
			}
			break;
		case STATE_DURATION_PERIOD:
			{
				if(iTimeExclude >=  m_iDuration + m_iHasRepeatedCount * iRepeatedTime )
				{
					m_pAnimation->Step((iTimeExclude - m_iHasRepeatedCount * iRepeatedTime) * 1.0);
					m_state = STATE_REVERSE_PERIOD;
					Reverse();
				}
				else
				{
					m_pAnimation->Step((iTimeExclude - m_iHasRepeatedCount * iRepeatedTime) * 1.0);
					bFlag = false;
				}
			}
			break;
		case STATE_REVERSE_PERIOD:
			{
				if(m_iReverseDuration > 0)
				{
					if(iTimeExclude >=  m_iDuration + m_iReverseDuration + m_iHasRepeatedCount * iRepeatedTime)
					{
						m_pAnimation->Step((iTimeExclude - m_iDuration - m_iHasRepeatedCount * iRepeatedTime) * 1.0);
						m_state = STATE_IDLE_PERIOD;
					}
					else
					{
						m_pAnimation->Step((iTimeExclude - m_iDuration - m_iHasRepeatedCount * iRepeatedTime) * 1.0);
						bFlag = false;
					}
				}
				else
					m_state = STATE_IDLE_PERIOD;

			}
			break;
		case STATE_IDLE_PERIOD:
			{
				bool bEnd = false;
				if(m_iIdleTime > 0)
				{
					if(iTimeExclude >= m_iDuration + m_iReverseDuration + m_iIdleTime + m_iHasRepeatedCount * iRepeatedTime)
					{
						if(m_iRepeatCount == 0 || m_iHasRepeatedCount == m_iRepeatCount)
							bEnd = true;
						if(m_iRepeatCount > 0 && m_iHasRepeatedCount < m_iRepeatCount)
						{
							m_iHasRepeatedCount++;
							m_state = STATE_DURATION_PERIOD;
						}
						else
							bFlag = false;
					}
					else
						bFlag = false;
				}
				else
				{
					bFlag = false;
					bEnd = true;
				}

				if(bEnd && m_pListener != NULL)
				{
					m_bIsEnded = true;
					m_pListener->OnAnimatorStoped(this);
				}
				if(bEnd)
				{
					OnAnimationStoped(m_pAnimation);
				}
			}
			break;
		default:
			bFlag = false;
		}
	}
}

int CAnimator::GetNeedTime()
{
	return m_iDelayStartTime + (m_iDuration + m_iReverseDuration + m_iIdleTime) * (m_iRepeatCount + 1);
}

void CAnimator::SetParent( CAnimator* pAnimator )
{
	m_pParent = pAnimator;
}

int CAnimator::GetNeedTimeFromRootParent()
{
	int iElapsedTime = 0;
	if(m_pParent != NULL)
	{
		CAnimator* pParent = m_pParent;
		while(pParent != NULL)
		{
			iElapsedTime += pParent->GetNeedTime();
			pParent = pParent->m_pParent;
			
		}
	}
	else
		iElapsedTime = m_iStartTime;

	return iElapsedTime;
}

void CAnimator::SetStartTime( int iStartTime )
{
	m_iStartTime = iStartTime;
}

void CAnimator::SetAnimatorListener( IAnimatorListener* pListener )
{
	m_pListener = pListener;
}

bool CAnimator::IsOver()
{
	return m_bIsEnded;
}

void CAnimator::Reset()
{
	m_state = STATE_NONE;
	m_bIsEnded = false;
	if(m_pAnimation != NULL)
		m_pAnimation->Reset();
}

void CAnimator::SetParams( int iDelayStartTime, int iDuration, int iReverseTime, int iIdleTime )
{
	m_iDelayStartTime = iDelayStartTime;
	m_iDuration = iDuration;
	if(m_pAnimation)
		m_pAnimation->SetDuration(m_iDuration);
	m_iReverseDuration = iReverseTime;
	m_iIdleTime = iIdleTime;
}

void CAnimator::SetInStoryBoard()
{
	m_bInStoryBoard = true;
	m_pAnimation->SetUseOutTimer(true);
}

void CAnimator::OnAnimatonStart( CAnimation* pAnimation )
{
	//if(!m_bInStoryBoard)
	//{
	//	if(m_pAnimObject)
	//		m_pAnimObject->FireAnimationStartEvent(this);
	//}
	TNotifyUI notify;
	notify.pSender = NULL;
	notify.sType = EventAnimatorStart;
	notify.wParam = (WPARAM)this;
	bool bRet = FireEvent(EventAnimatorStart, &notify);
}

void CAnimator::OnAnimationProgressed( CAnimation* pAnimation )
{
	//if(!m_bInStoryBoard)
	//{
	//	if(m_pAnimObject)
	//		m_pAnimObject->FireAnimationProgressedEvent(this);
	//}
	TNotifyUI notify;
	notify.pSender = NULL;
	notify.sType = EventAnimatorProgressed;
	notify.wParam = (WPARAM)this;
	bool bRet = FireEvent(EventAnimatorProgressed, &notify);
}

void CAnimator::OnAnimationStoped(CAnimation* pAnimation)
{
	if(!m_bInStoryBoard)
	{
		m_bIsEnded = true;
		m_state = STATE_END;
		//if(m_pAnimObject)
		//	m_pAnimObject->FireAnimationEndEvent(this);
	}

	TNotifyUI notify;
	notify.pSender = NULL;
	notify.sType = EventAnimatorEnd;
	notify.wParam = (WPARAM)this;
	bool bRet = FireEvent(EventAnimatorEnd, &notify);
}