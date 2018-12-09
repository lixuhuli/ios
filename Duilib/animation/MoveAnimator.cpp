#include "StdAfx.h"
#include "MoveAnimator.h"

using namespace DuiLib;
CMoveAnimator::CMoveAnimator( IAnimObject* pAnimObject, CTween::Type tweenType, int iDuration, QPoint qFrom, QPoint qTo, bool bIsToAsRelativeValue /*= false*/,bool bReCalculateCurrentValue/* = false*/, bool bReletivePos, bool bTimeSensive /*= false*/, bool bUserThreadTimer /*= false*/ ) : m_fromPos(qFrom),
	m_toPos(qTo), m_bIsToAsRelativeValue(bIsToAsRelativeValue),m_bReCalculateCurrentValue(bReCalculateCurrentValue), m_bReletivePos(bReletivePos),
	CAnimator(pAnimObject,  tweenType, iDuration, bTimeSensive, bUserThreadTimer)
{
	if(qFrom.x == ANIM_CURRENT_VALUE)
	{
		m_fromPos.x = pAnimObject->GetPos().left;
	}
	if(qFrom.y == ANIM_CURRENT_VALUE)
	{
		m_fromPos.y = pAnimObject->GetPos().top;
	}
	m_userFromPos = qFrom;
}

CMoveAnimator::~CMoveAnimator()
{

}

CScaleAnimator * CMoveAnimator::AxisPartner(CScaleAnimator *pPartner, BOOL bSetOrGet)
{
	static CScaleAnimator *partner = NULL;
	static CMoveAnimator *pSelf = NULL;
	
	if (bSetOrGet)
	{
		partner = pPartner;
		pSelf = this;
		return NULL;
	}
	else
	{
		return (pSelf == this) ? partner : NULL;
	}
}

void CMoveAnimator::OnAnimationProgressed( CAnimation* pAnimation )
{
	QPoint current;
	QPoint toPos = m_toPos;
	if(m_bIsToAsRelativeValue)
	{
		toPos.x = m_fromPos.x + m_toPos.x;
		toPos.y = m_fromPos.y + m_toPos.y;
	}
	current = pAnimation->CurrentValueBetween(m_fromPos, toPos);
	if(m_pAnimObject)
	{
		CScaleAnimator *partner = AxisPartner(NULL,FALSE);
		if (partner != NULL)
		{
			SIZE sz;
			sz.cx = m_pAnimObject->GetPos().GetWidth();
			sz.cy = m_pAnimObject->GetPos().GetHeight();

			QPoint pos = ConvertPointToLeftTop(current,sz,partner->GetAxis());
			m_pAnimObject->SetPosition(pos.x, pos.y, m_bReletivePos);
		}
		else
			m_pAnimObject->SetPosition(current.x, current.y, m_bReletivePos);
	}
	CAnimator::OnAnimationProgressed(pAnimation);
}

void CMoveAnimator::OnAnimatonStart( CAnimation* pAnimation )
{
	if(m_bReCalculateCurrentValue)
	{
		QRect rect = m_pAnimObject->GetPos();
		if(m_userFromPos.x == ANIM_CURRENT_VALUE)
		{
			m_fromPos.x = rect.left;
		}
		if(m_userFromPos.y == ANIM_CURRENT_VALUE)
		{
			m_fromPos.y = rect.top;
		}
	}
    CAnimator::OnAnimatonStart(pAnimation);
}


DuiLib::QPoint CMoveAnimator::ConvertPointToLeftTop(DuiLib::QPoint srcPoint, SIZE sz, E_AXIS eAxis)
{
	DuiLib::QPoint pt;
	switch(eAxis)
	{
	case AXIS_LEFT_TOP:
		pt = srcPoint;
		break;
	case AXIS_RIGHT_TOP:
		pt.x = srcPoint.x - sz.cx;
		pt.y = srcPoint.y;
		break;
	case AXIS_CENTER:
		pt.x = srcPoint.x - sz.cx/2;
		pt.y = srcPoint.y - sz.cy/2;
		break;
	case AXIS_LEFT_BOTTOM:
		pt.x = srcPoint.x;
		pt.y = srcPoint.y - sz.cy;
		break;
	case AXIS_RIGHT_BOTTOM:
		pt.x = srcPoint.x - sz.cx;
		pt.y = srcPoint.y - sz.cy;
		break;
	case AXIS_CENTER_TOP:
		pt.x = srcPoint.x - sz.cx/2;
		pt.y = srcPoint.y;
		break;
	case AXIS_CENTER_BOTTOM:
		pt.x = srcPoint.x - sz.cx/2;
		pt.y = srcPoint.y - sz.cy;
		break;
	case AXIS_CENTER_LEFT:
		pt.x = srcPoint.x;
		pt.y = srcPoint.y - sz.cy/2;
		break;
	case AXIS_CENTER_RIGHT:
		pt.x = srcPoint.x - sz.cx;
		pt.y = srcPoint.y - sz.cy/2;
		break;
	default:
		break;
	}
	return pt;
}

