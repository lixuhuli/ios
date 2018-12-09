#include "StdAfx.h"
#include "ScaleAnimator.h"

using namespace DuiLib;
CScaleAnimator::CScaleAnimator( IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration, QPoint qFrom, QPoint qTo, bool bTopLeftAxis/* = false*/, bool bIsToAsRelativeValue /*= false*/,bool bReCalculateCurrentValue/* = false*/, bool bTimeSensive /*= false*/, bool bUserThreadTimer /*= false*/ ) : m_fromSize(qFrom),
	m_toSize(qTo), m_bIsToAsRelativeValue(bIsToAsRelativeValue),m_bReCalculateCurrentValue(bReCalculateCurrentValue),
	CAnimator(pAnimObject,  tweenType, iDuration, bTimeSensive, bUserThreadTimer)
{
	m_bTopLeftAxis = bTopLeftAxis;
	if(m_bTopLeftAxis)
	{
		m_eAxis = AXIS_LEFT_TOP;
	}
	else
	{
		m_eAxis = AXIS_CENTER;
	}
	RefreshOrigen();	
	QRect rect = m_pAnimObject->GetPos();
	if(m_fromSize.x == ANIM_CURRENT_VALUE)
	{
		m_fromSize.x = rect.GetWidth();
	}
	if(m_fromSize.y == ANIM_CURRENT_VALUE)
	{
		m_fromSize.y = rect.GetHeight();
	}
	m_userFromSize = qFrom;
}

CScaleAnimator::CScaleAnimator( IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration, DuiLib::QPoint qFrom, DuiLib::QPoint qTo,E_AXIS axis /*= AXIS_LEFT_TOP*/, bool bIsToAsRelativeValue /*= false*/,bool bReCalculateCurrentValue/* = false*/, bool bTimeSensive /*= false*/, bool bUserThreadTimer /*= false*/ ) : m_fromSize(qFrom),
	m_toSize(qTo), m_bIsToAsRelativeValue(bIsToAsRelativeValue),m_bReCalculateCurrentValue(bReCalculateCurrentValue),
	CAnimator(pAnimObject,  tweenType, iDuration, bTimeSensive, bUserThreadTimer)
{
	m_eAxis = axis;
	if(m_eAxis == AXIS_LEFT_TOP)
	{
		m_bTopLeftAxis = true;
	}
	else
	{
		m_bTopLeftAxis = false;
	}
	RefreshOrigen();
	QRect rect = m_pAnimObject->GetPos();
	if(m_fromSize.x == ANIM_CURRENT_VALUE)
	{
		m_fromSize.x = rect.GetWidth();
	}
	if(m_fromSize.y == ANIM_CURRENT_VALUE)
	{
		m_fromSize.y = rect.GetHeight();
	}
	m_userFromSize = qFrom;
}

CScaleAnimator::~CScaleAnimator()
{

}

E_AXIS CScaleAnimator::GetAxis()
{
	return m_eAxis;
}

void CScaleAnimator::OnAnimationProgressed( CAnimation* pAnimation )
{
	RefreshOrigen();
	QPoint current;
	QPoint toSize = m_toSize;
	if(m_bIsToAsRelativeValue)
	{
		toSize.x = m_fromSize.x + m_toSize.x;
		toSize.y = m_fromSize.y + m_toSize.y;
	}
	current = pAnimation->CurrentValueBetween(m_fromSize, toSize);
	if(m_pAnimObject)
	{
			m_pAnimObject->SetSize(current.x, current.y);
			if(m_eAxis == AXIS_RIGHT_TOP)
			{
				m_pAnimObject->SetPosition(m_origen.x - current.x, m_origen.y);
			}
			else if(m_eAxis == AXIS_LEFT_BOTTOM)
			{
				m_pAnimObject->SetPosition(m_origen.x, m_origen.y - current.y);
			}
			else if(m_eAxis == AXIS_RIGHT_BOTTOM)
			{
				m_pAnimObject->SetPosition(m_origen.x - current.x, m_origen.y - current.y);
			}
			else if(m_eAxis == AXIS_CENTER)
			{
				m_pAnimObject->SetPosition(m_origen.x - current.x/2, m_origen.y - current.y/2);
			}
			else if (m_eAxis == AXIS_CENTER_TOP)
			{
				m_pAnimObject->SetPosition(m_origen.x - current.x/2, m_origen.y);
			}
			else if (m_eAxis == AXIS_CENTER_BOTTOM)
			{
				m_pAnimObject->SetPosition(m_origen.x - current.x/2, m_origen.y-current.y);
			}
			else if (m_eAxis == AXIS_CENTER_LEFT)
			{
				m_pAnimObject->SetPosition(m_origen.x, m_origen.y-current.y/2);
			}
			else if (m_eAxis == AXIS_CENTER_RIGHT)
			{
				m_pAnimObject->SetPosition(m_origen.x-current.x, m_origen.y-current.y/2);
			}
	}
	CAnimator::OnAnimationProgressed(pAnimation);
}

void CScaleAnimator::RefreshOrigen()
{
	if(!m_pAnimObject)
		return;
	QRect rect = m_pAnimObject->GetPos();
	if(m_eAxis == AXIS_CENTER)
		m_origen = QPoint(rect.left + rect.GetWidth()/2, rect.top + rect.GetHeight()/2) ;
	else if (m_eAxis == AXIS_CENTER_TOP)
	{
		m_origen = QPoint(rect.left+rect.GetWidth()/2, rect.top);
	}
	else if (m_eAxis == AXIS_CENTER_BOTTOM)
	{
		m_origen = QPoint(rect.left+rect.GetWidth()/2, rect.bottom);
	}
	else if (m_eAxis == AXIS_CENTER_LEFT)
	{
		m_origen = QPoint(rect.left, rect.top+rect.GetHeight()/2);
	}
	else if (m_eAxis == AXIS_CENTER_RIGHT)
	{
		m_origen = QPoint(rect.right, rect.top+rect.GetHeight()/2);
	}
	else if(m_eAxis ==  AXIS_RIGHT_TOP)
	{
		m_origen = QPoint(rect.right, rect.top);
	}
	else if(m_eAxis == AXIS_LEFT_BOTTOM)
	{
		m_origen = QPoint(rect.left, rect.bottom);
	}
	else if(m_eAxis == AXIS_RIGHT_BOTTOM)
	{
		m_origen = QPoint(rect.right, rect.bottom);
	}
	else
	{
		m_origen = QPoint(rect.left, rect.top);
	}
}

void CScaleAnimator::OnAnimatonStart( CAnimation* pAnimation )
{
	if(m_bReCalculateCurrentValue)
	{
		QRect rect = m_pAnimObject->GetPos();
		if(m_userFromSize.x == ANIM_CURRENT_VALUE)
		{
			m_fromSize.x = rect.GetWidth();
		}
		if(m_userFromSize.y == ANIM_CURRENT_VALUE)
		{
			m_fromSize.y = rect.GetHeight();
		}
	}
    CAnimator::OnAnimatonStart(pAnimation);
}

