#include "StdAfx.h"
#include "Animation.h"

CAnimation::CAnimation( int iFP /*= 20*/, bool bUseOutTimer/* = false*/, bool bTimesensive /*= true*/, bool bUseThreadTimer/* = false*/)
{
	m_pListener = NULL;
	m_pTimer = NULL;
	m_bAnimating = false;
	m_iFP = iFP;
	m_iRepeatCount = 0;
	m_bUseOutTimer = bUseOutTimer;
	m_bUserThreadTimer = bUseThreadTimer;
	m_bTimesensive = bTimesensive;
}

CAnimation::~CAnimation()
{
	Stop();
}

void CAnimation::SetFP( int iFP )
{
	Stop();
	m_iFP = iFP;
	Start();
}

void CAnimation::Start()
{
	ASSERT(m_iFP > 0);
	Reset();
	if(!m_bUseOutTimer)
	{
		m_pTimer = new CQuiTimer(QUITIMER_REPEAT, this, m_bTimesensive, m_bUserThreadTimer);
		m_iStartTime = ::GetTickCount();
		m_pTimer->Start(m_iFP);
	}

	m_bAnimating = true;
	if(m_pListener != NULL && m_bAnimating)
		m_pListener->OnAnimatonStart(this);
}

void CAnimation::Stop()
{
	if(m_pTimer != NULL)
	{
		delete m_pTimer;
		m_pTimer = NULL;
	}
	if(m_pListener != NULL && m_bAnimating)
		m_pListener->OnAnimationStoped(this);
	m_bAnimating = false;
}

// double CAnimation::CurrentValueBetween( double dStart, double dEnd )
// {
// 	return CTween::ValueBetween(GetCurrentValue(), dStart, dEnd);
// }
// 
// int CAnimation::CurrentValueBetween( int iStart, int iEnd )
// {
// 	return CTween::ValueBetween(GetCurrentValue(), iStart, iEnd);
// }

void CAnimation::OnTimer( CQuiTimer* pTimer )
{
	int iElapsed = pTimer->GetElapsedTime();
	Step(iElapsed * 1.0);
}

void CAnimation::SetRepeatCount( int iCount )
{
	m_iRepeatCount = iCount;
}

void CAnimation::Reset()
{
	m_iStartTime = 0;
	m_iRepeatCount = 0;
	m_bAnimating = false;
}

void CAnimation::Restart()
{
	Reset();
	Start();
}

void CAnimation::SetUseOutTimer( bool bUseOutTimer )
{
	m_bUseOutTimer = bUseOutTimer;
}

int CAnimation::GetDuration()
{
	return m_iDuration;
}
