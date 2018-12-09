#include "StdAfx.h"
#include "LinearAnimation.h"


CLinearAnimation::CLinearAnimation( int iDuration , IAnimationListener* pListener, bool bUseOutTimer/* = false*/, bool bTimesensive /*= true*/, bool bUseThreadTimer/* = false*/): CAnimation(20, bUseOutTimer, bTimesensive, bUseThreadTimer)
{
	m_iDuration = iDuration;
	m_dFactor = 0.0;
	m_eType = CTween::LINEAR;
	SetListener(pListener);
}

CLinearAnimation::~CLinearAnimation()
{
}

void CLinearAnimation::SetDuration( int iDuration )
{
    m_iDuration = iDuration;
}

void CLinearAnimation::Step( double dElapsed )
{
	m_dFactor = dElapsed/m_iDuration;
	//CString strLog;
	//strLog.Format(_T("\nstep %f:%f\n"), (float)dElapsed, (float)m_dFactor);
	//OutputDebugString(strLog);
	if(m_dFactor >= 1.0)
	{
		m_dFactor = 1.0;
	}
	if(m_dFactor >= 1.0)
	{
		if(m_pListener != NULL)
			m_pListener->OnAnimationProgressed(this);
		if(m_iRepeatCount > 0 )
		{
			m_iRepeatCount--;
			Restart();
		}
		else if(m_iRepeatCount == INFINITE_REPEAT_COUNT)
		{
			Restart();
		}
		else
			Stop();
	}
	else
	{
		AnimateToState(m_dFactor);
		if(m_pListener != NULL)
			m_pListener->OnAnimationProgressed(this);
	}
}

void CLinearAnimation::AnimateToState( double dValue )
{
	m_dFactor = CTween::CalculateValue(m_eType, dValue);
}

double CLinearAnimation::GetCurrentValue()
{
	return m_dFactor;
}

double CLinearAnimation::CurrentValueBetween( double dStart, double dEnd )
{
	return CLinearAnimation::ValueBetween(GetCurrentValue(), dStart, dEnd);
}

int CLinearAnimation::CurrentValueBetween( int iStart, int iEnd )
{
	return CLinearAnimation::ValueBetween(GetCurrentValue(), iStart, iEnd);
}

POINT CLinearAnimation::CurrentValueBetween(POINT beginP, POINT endP)
{
	POINT ptValue = {0, 0};
	double iStart = 0;
	double iEnd = 0;


	iStart = (double)beginP.x;
	iEnd = (double)endP.x;
	ptValue.x = CLinearAnimation::ValueBetween(GetCurrentValue(), iStart, iEnd);


	iStart = (double)beginP.y;
	iEnd = (double)endP.y;

	ptValue.y = CLinearAnimation::ValueBetween(GetCurrentValue(), iStart, iEnd);


	return ptValue;
}

double CLinearAnimation::ValueBetween(double value, double start, double target)
{
	return start + (target - start) * value;
}

// static
int CLinearAnimation::ValueBetween(double value, int start, int target)
{
	if(start == target)
	{
		return start;
	}
	double delta = static_cast<double>(target - start);
	if(delta < 0)
	{
		delta--;
	}
	else
	{
		delta++;
	}
	return start + static_cast<int>(value * _nextafter(delta, 0));
}