#include "StdAfx.h"
#include "UIAnimatTabLayout.h"

namespace DuiLib {

	CAnimatTabLayoutUI::CAnimatTabLayoutUI() : 
		CUIAnimation(this), 
		m_bVertDirect(false), 
		m_nPosDirect(1),
		m_pCurrentControl(NULL),
		m_bVisibleFlag(false)
	{
		
	}


	CAnimatTabLayoutUI::~CAnimatTabLayoutUI()
	{
	}

	LPCTSTR CAnimatTabLayoutUI::GetClass() const
	{
		return _T("AnimatTabLayoutUI");
	}

	LPVOID CAnimatTabLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, _T("AnimatTabLayout")) == 0 ) 
			return static_cast<CAnimatTabLayoutUI*>(this);
		return CTabLayoutUI::GetInterface(pstrName);
	}

	bool CAnimatTabLayoutUI::SelectItem(int iIndex, bool bSetFocus /*= true*/)
	{
		if( iIndex < 0 || iIndex >= m_items.GetSize() ) return false;
		if( iIndex == m_iCurSel ) return true;
		if( iIndex > m_iCurSel ) m_nPosDirect = -1;
		if( iIndex < m_iCurSel ) m_nPosDirect = 1;

		int iOldSel = m_iCurSel;
		m_iCurSel = iIndex;
		for( int it = 0; it < m_items.GetSize(); it++ ) 
		{
			if( it == iIndex ) 
			{
				m_pCurrentControl = static_cast<CControlUI*>(m_items[it]);
				if (m_pCurrentControl)
				{
					m_pCurrentControl->SetVisible(true);
					if (bSetFocus)
						m_pCurrentControl->SetFocus();
				}
				m_bVisibleFlag = false;
			}
			else 
				GetItemAt(it)->SetVisible(false);
		}

		NeedParentUpdate();
		if( NULL != m_pCurrentControl ) 
			m_pCurrentControl->SetVisible( false );
		AnimationSwitch();

		if( m_pManager != NULL ) 
		{
			m_pManager->SetNextTabControl();
			m_pManager->SendNotify(this, _T("tabselect"), m_iCurSel, iOldSel);
		}
		return true;
	}

	void CAnimatTabLayoutUI::AnimationSwitch()
	{
		m_rcItemOld = m_rcItem;
		if( !m_bVertDirect )
		{
			m_rcCurPos.top = m_rcItem.top;
			m_rcCurPos.bottom = m_rcItem.bottom;
			m_rcCurPos.left = m_rcItem.left - ( m_rcItem.right - m_rcItem.left ) * m_nPosDirect + 52 * m_nPosDirect;
			m_rcCurPos.right = m_rcItem.right - ( m_rcItem.right - m_rcItem.left ) * m_nPosDirect+ 52 * m_nPosDirect;		
		}
		else
		{
			m_rcCurPos.left = m_rcItem.left;
			m_rcCurPos.right = m_rcItem.right;
			m_rcCurPos.top = m_rcItem.top - ( m_rcItem.bottom - m_rcItem.top ) * m_nPosDirect;
			m_rcCurPos.bottom = m_rcItem.bottom - ( m_rcItem.bottom - m_rcItem.top ) * m_nPosDirect;		
		}

		StopAnimation(TAB_ANIMATION_ID);
		StartAnimation(TAB_ANIMATION_ELLAPSE, TAB_ANIMATION_FRAME_COUNT, TAB_ANIMATION_ID);
	}

	void CAnimatTabLayoutUI::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_TIMER ) 
		{
			OnTimer(  event.wParam );
		}
		__super::DoEvent( event );
	}

	void CAnimatTabLayoutUI::OnTimer( int nTimerID )
	{
		OnAnimationElapse(nTimerID);
	}

	void CAnimatTabLayoutUI::OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID)
	{
		if( !m_bVisibleFlag ) {
			m_bVisibleFlag = true;
			m_pCurrentControl->SetVisible( true );
		}

		int iStepLen = 0;
		if( !m_bVertDirect )
		{
			iStepLen = ( m_rcItemOld.right - m_rcItemOld.left ) * m_nPosDirect / nTotalFrame;
			if( nCurFrame != nTotalFrame )
			{
				m_rcCurPos.left = m_rcCurPos.left + iStepLen;
				m_rcCurPos.right = m_rcCurPos.right +iStepLen;			
			}
			else
			{
				m_rcItem = m_rcCurPos = m_rcItemOld;
			}
		}
		else
		{
			iStepLen = ( m_rcItemOld.bottom - m_rcItemOld.top ) * m_nPosDirect / nTotalFrame;
			if( nCurFrame != nTotalFrame )
			{
				m_rcCurPos.top = m_rcCurPos.top + iStepLen;
				m_rcCurPos.bottom = m_rcCurPos.bottom +iStepLen;			
			}
			else
			{
				m_rcItem = m_rcCurPos = m_rcItemOld;	
			}	
		}
		SetPos(m_rcCurPos);
	}

	void CAnimatTabLayoutUI::OnAnimationStop(INT nAnimationID) 
	{
		SetPos(m_rcItemOld);
		NeedParentUpdate();
	}

	void CAnimatTabLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("animati_direct")) == 0 && _tcsicmp( pstrValue, _T("vertical")) == 0 ) 
			m_bVertDirect = true; // pstrValue = "vertical" or "horizontal"
		return CTabLayoutUI::SetAttribute(pstrName, pstrValue);
	}
} // namespace DuiLib