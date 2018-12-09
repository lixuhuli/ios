#include "StdAfx.h"
#include "UIGif.h"

namespace DuiLib
{
	CGifUI::CGifUI(void) :
	m_nTimerElapse(50),
	m_nIndex(0),
	m_sImagePostfix(_T("png")),
	m_nStopAt(-1),
	m_nRound(0),
	m_bStopped(false)
{
}

CGifUI::~CGifUI(void)
{
	Stop();
}

void CGifUI::DoEvent( TEventUI& event )
{
	if( event.Type == UIEVENT_TIMER && event.wParam == GIF_TIMER_ID )
	{
		if(GetCount() > 0)
		{
			if(m_nRound != 0 && m_nStopAt != -1)
			{
				Stop(m_nStopAt);
				m_bStopped = true;
				CContainerUI::DoEvent(event);
				return;
			}

			long nNextIndex = (m_nIndex + 1) % GetCount();

			CControlUI* pCurrentItem = GetItemAt(m_nIndex);
			CControlUI* pNextItem = GetItemAt(nNextIndex);
			if(pCurrentItem) pCurrentItem->SetVisible(false);
			if(pNextItem) pNextItem->SetVisible(true);

			m_nIndex = nNextIndex;
			if(m_nIndex == m_nCount - 1) m_nRound++;
		}
	}

	CContainerUI::DoEvent(event);
}

void CGifUI::SetVisible( bool bVisible /*= true*/ )
{
	bool bOldVisible = IsVisible();
	CContainerUI::SetVisible(bVisible);
	bool bNewVisible = IsVisible();
	if(m_pManager && m_nTimerElapse > 0 && GetCount() > 0)
	{
		if(!bOldVisible && bNewVisible && !m_bStopped)
		{
			Start();
		}
		else if(bOldVisible && !bNewVisible)
		{
			Stop();
		}
	}
}

void CGifUI::SetInternVisible(bool bVisible /*= true*/)
{
	bool bOldVisible = IsVisible();
	CContainerUI::SetInternVisible(bVisible);
	bool bNewVisible = IsVisible();
	if(m_pManager && m_nTimerElapse > 0 && GetCount() > 0)
	{
		if(!bOldVisible && bNewVisible && !m_bStopped)
		{
			Start();
		}
		else if(bOldVisible && !bNewVisible)
		{
			Stop();
		}
	}
}

void CGifUI::Init()
{
	int nWidth = GetFixedWidth();
	int nHeight = GetFixedHeight();
	CDuiString sPos;
	sPos.SmallFormat(_T("%d,%d,%d,%d"),0,0,nWidth,nHeight);

	CDuiString sGifTitle = m_sImageFolder;
	int nIndex = sGifTitle.ReverseFind(_T('/'));
	if(nIndex != -1) sGifTitle = sGifTitle.Mid(nIndex + 1,sGifTitle.GetLength());
	nIndex = sGifTitle.ReverseFind(_T('\\'));
	if(nIndex != -1) sGifTitle = sGifTitle.Mid(nIndex + 1,sGifTitle.GetLength());

	for(long i = 0 ; i < m_nCount ; i++)
	{
		CDuiString sImagePath;
		sImagePath.Format(_T("%s/%s%d.%s"),m_sImageFolder.GetData(),sGifTitle.GetData(),i+1,m_sImagePostfix.GetData());

		CControlUI* pItem = new CControlUI;
		pItem->SetAttribute(_T("bkimage"),sImagePath);
		pItem->SetAttribute(_T("pos"),sPos);
		pItem->SetAttribute(_T("float"),_T("true"));
		if(i == 0) pItem->SetAttribute(_T("visible"),_T("true"));
		else pItem->SetAttribute(_T("visible"),_T("false"));

		this->Add(pItem);
	}

	//m_bVisible属性和IsVisible()方法不同，IsVisible返回的是m_bVisible&m_bInternVisible
	if(IsVisible() && m_pManager && m_nTimerElapse > 0  && GetCount() > 0 && !m_bStopped)
	{
		Start();
	}

	CContainerUI::Init();
}

void CGifUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	if( _tcscmp(pstrName, _T("timeelapse")) == 0 ) m_nTimerElapse = _ttoi(pstrValue);
	else if( _tcscmp(pstrName, _T("imagefolder")) == 0 ) m_sImageFolder = pstrValue;
	else if( _tcscmp(pstrName, _T("imagecount")) == 0 ) m_nCount = _ttoi(pstrValue);
	else if( _tcscmp(pstrName, _T("imagepostfix")) == 0 ) m_sImagePostfix = pstrValue;
	else if( _tcscmp(pstrName, _T("stopat")) == 0 )
	{
		int nStopAt = _ttoi(pstrValue) - 1;
		m_nStopAt = nStopAt < 0 ? -1 : nStopAt;
	}

	CContainerUI::SetAttribute(pstrName,pstrValue);
}

void CGifUI::Start(int nStartIndex /*= 0*/)
{
	CControlUI* pInitItem = GetItemAt(nStartIndex);
	if(pInitItem) pInitItem->SetVisible(true);
	m_nIndex = nStartIndex;
	m_pManager->SetTimer(this,GIF_TIMER_ID,m_nTimerElapse);
}

void CGifUI::Stop(int nStopIndex /*= -1*/)
{
	m_pManager->KillTimer(this,GIF_TIMER_ID);

	for(long i = 0 ; i < GetCount() ; i++)
	{
		CControlUI* pItem = GetItemAt(i);
		if(pItem) pItem->SetVisible(false);
	}

	m_nIndex = nStopIndex;

	if(m_nIndex != -1)
	{
		CControlUI* pItem = GetItemAt(m_nIndex);
		if(pItem) pItem->SetVisible(true);
	}
}

}
