#pragma once
class CLock
{
public:
	CLock(void)		{ ::InitializeCriticalSection(&m_cs);	}
	~CLock(void)	{ ::DeleteCriticalSection(&m_cs);		}
	void Lock()		{ ::EnterCriticalSection(&m_cs);		}
	void UnLock()	{ ::LeaveCriticalSection(&m_cs);		}
private:
	CRITICAL_SECTION	m_cs;
};

class CScopeLock
{
public:
	CScopeLock(CLock& lock)
		:m_lock(lock)
	{
		m_lock.Lock();
	}
	~CScopeLock()
	{
		m_lock.UnLock();
	}
private:
	CLock& m_lock;
};
