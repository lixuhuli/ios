/*
@brief
@author longyong@qiyi.com
@2012/12/18
*/
#ifndef __QUITIMER_H__
#define __QUITIMER_H__

#include <map>
#include "../../include/AutoLock.h"

enum E_QUITIMER_TYPE
{
	QUITIMER_ONCE = 0,
	QUITIMER_REPEAT
};



class CQuiTimer;
class UILIB_API IQuiTimerHandler
{
public:
	virtual void OnTimer(CQuiTimer* pTimer) = 0;
};

class UILIB_API CQuiTimer
{
public:
	typedef std::map<UINT_PTR, CQuiTimer*>  TimerMap;
	CQuiTimer(E_QUITIMER_TYPE eType, IQuiTimerHandler* pTimerHandler = NULL, bool bTimeSensive = true, bool bThreadTimer = false);
	~CQuiTimer();
	void Start(UINT iTime, IQuiTimerHandler* pTimerHanler = NULL);
	void Stop();
	static void CALLBACK TimerProc(HWND hwnd,
		UINT uMsg,
		UINT_PTR idEvent,
		DWORD dwTime
		);

	static void CALLBACK ThreadTimerProc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
	int GetElapsedTime();
	int GetStartTime();
	int GetConstantElapsedTime();//return value do not change in the same time interval
	void Pause();
	void Resume();
public:
	void SetTimerHandler(IQuiTimerHandler* pTimerHandler = NULL){ m_pHandler = pTimerHandler; } ////added by yueli 201406
protected:
	E_QUITIMER_TYPE m_eType;
	UINT_PTR m_uID;
	static UINT s_uAccuracy;
	static CLock g_LockAccuracy;
	IQuiTimerHandler* m_pHandler;
protected:
	static TimerMap s_timerMap;
	static CLock g_LockTimerMap;
	bool m_bThreadTimer;
	bool m_bTimeSensive;
	int  m_iStartTime;
	int  m_iOntimerCount;
	int  m_iTime;
	DWORD m_dwPausedTime;
	DWORD m_dwResumeTime;
	DWORD m_dwOntimerTime;
	bool m_bPaused;
};


#endif//end __QUITIMER_H__