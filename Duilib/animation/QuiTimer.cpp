#include "StdAfx.h"
#include "QuiTimer.h"
#include "Mmsystem.h"

using namespace std;

CQuiTimer::TimerMap CQuiTimer::s_timerMap;
UINT CQuiTimer::s_uAccuracy = 0;
CLock CQuiTimer::g_LockAccuracy;
CLock CQuiTimer::g_LockTimerMap;

CQuiTimer::CQuiTimer( E_QUITIMER_TYPE eType, IQuiTimerHandler* pTimerHandler /*= NULL*/ ,bool bTimeSensive, bool bThreadTimer/* = false*/)
{
	m_eType = /*QUITIMER_ONCE*/eType;
	m_uID = 0;
	m_pHandler = pTimerHandler;
	m_bThreadTimer = bThreadTimer;
	m_bTimeSensive = bTimeSensive;
	m_iStartTime= 0;
	m_iOntimerCount = 0;
	m_iTime = 20;
	m_dwPausedTime = 0;
	m_dwResumeTime = 0;
	m_dwOntimerTime = 0;
	m_bPaused = false;
	if(m_bThreadTimer)
	{
        CScopeLock lock(g_LockAccuracy);
		if(s_uAccuracy == 0)
		{
			TIMECAPS tc;
			if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR) 
			{ 
				s_uAccuracy = min(max(tc.wPeriodMin, 1), tc.wPeriodMax); 
				timeBeginPeriod(s_uAccuracy); 
			 }
		}
 
	}
}

CQuiTimer::~CQuiTimer()
{
	Stop();
}

void CQuiTimer::Start( UINT iTime, IQuiTimerHandler* pTimerHanler /*= NULL*/ )
{
	if(m_bThreadTimer)
	{
		UINT fEvent = TIME_CALLBACK_FUNCTION;
		if(m_eType == QUITIMER_ONCE)
			fEvent |= TIME_ONESHOT;
		else
			fEvent |= TIME_PERIODIC;

        CScopeLock lock(g_LockAccuracy);
		m_uID = timeSetEvent(iTime, s_uAccuracy, ThreadTimerProc,  (DWORD_PTR)this, fEvent);
	}
	else
		m_uID = ::SetTimer(NULL, 0, iTime, TimerProc);
	{
        CScopeLock lock(g_LockTimerMap);
		s_timerMap[m_uID] = this;
	}
	m_iStartTime = GetTickCount();
	m_dwOntimerTime = m_iStartTime;
	m_iOntimerCount = 0;
	m_iTime = iTime;
}

void CALLBACK CQuiTimer::TimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
	CQuiTimer* pTimer = NULL;
	{
        CScopeLock lock(g_LockTimerMap);
		TimerMap::iterator it = s_timerMap.find(idEvent);
		if(it != s_timerMap.end())
			pTimer = it->second;
	}

	if (pTimer == NULL)
		return;
	{        
        E_QUITIMER_TYPE aType = pTimer->m_eType;
		if(pTimer->m_bPaused && aType != QUITIMER_ONCE)
			return;
		pTimer->m_iOntimerCount++;
		//for sometime OnTimer will delete  pTimer then refind it 
        if (aType == QUITIMER_ONCE)
        {
            CScopeLock lock(g_LockTimerMap);
            TimerMap::iterator it = s_timerMap.find(idEvent);
            if (it != s_timerMap.end())
                it->second->Stop();
        }
		pTimer->m_dwOntimerTime = ::GetTickCount();
        if(pTimer->m_pHandler != NULL)
            pTimer->m_pHandler->OnTimer(pTimer);
	}
}

void CQuiTimer::Stop()
{
	m_bPaused = false;
	if(m_uID != 0)
	{
		if(m_bThreadTimer)
			timeKillEvent(m_uID);
		else
			::KillTimer(NULL, m_uID);
        CScopeLock lock(g_LockTimerMap);
		s_timerMap.erase(m_uID);
	}
	//m_pHandler = NULL;
}

int CQuiTimer::GetElapsedTime()
{
	int iElapsedTime = 0;
	if(m_bTimeSensive)
	{
		if(m_dwResumeTime > m_dwPausedTime)
			iElapsedTime = GetTickCount() - m_dwResumeTime - m_dwPausedTime - m_iStartTime;
		else
			iElapsedTime = GetTickCount() - m_iStartTime;
	}
	else 
		iElapsedTime = m_iOntimerCount * m_iTime;
	return iElapsedTime;
}

int CQuiTimer::GetStartTime()
{
	return m_iStartTime;
}

void CALLBACK CQuiTimer::ThreadTimerProc( UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2 )
{
	CQuiTimer* pTimer = (CQuiTimer*)dwUser;
	if(pTimer->m_bPaused)
		return;
	if(pTimer)
	{
		pTimer->m_iOntimerCount++;
		pTimer->m_dwOntimerTime = ::GetTickCount();
	}

	if(pTimer->m_pHandler != NULL)
		pTimer->m_pHandler->OnTimer(pTimer);
}

void CQuiTimer::Pause()
{
	m_bPaused = true;
	m_dwPausedTime = GetTickCount();
}

void CQuiTimer::Resume()
{
	m_bPaused = false;
	m_dwResumeTime = GetTickCount();
}

int CQuiTimer::GetConstantElapsedTime()
{
	int iElapsedTime = 0;
	if(m_bTimeSensive)
	{
		if(m_dwResumeTime > m_dwPausedTime)
			iElapsedTime = m_dwOntimerTime - m_dwResumeTime - m_dwPausedTime - m_iStartTime;
		else
			iElapsedTime = m_dwOntimerTime - m_iStartTime;
	}
	else 
		iElapsedTime = m_iOntimerCount * m_iTime;
	return iElapsedTime;
}
