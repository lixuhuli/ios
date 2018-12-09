#include "stdafx.h"
#include "UIButtonGif.h"

namespace DuiLib
{
	CButtonGifUI::CButtonGifUI()
		:m_pGif(NULL)
		,m_nPreUpdateDelay(0)
		,m_isUpdateTime(false)
	{
	}

	CButtonGifUI::~CButtonGifUI()
	{
		m_pManager->KillTimer(this, GIF_TIMER_ID);
		if (m_pGif)
		{
			delete m_pGif;
			m_pGif = NULL;
		}
	}

	LPCTSTR CButtonGifUI::GetClass() const
	{
		return _T("ButtonGifUI");
	}

	void CButtonGifUI::SetVisible(bool bVisible /*= true*/)
	{
		if(bVisible == false)
			m_pManager->KillTimer(this, GIF_TIMER_ID);
		CButtonUI::SetVisible(bVisible);
	}

	void CButtonGifUI::PaintStatusImage( HDC hDC )
	{
		if (m_pGif == NULL)
			return;
		TImageInfo* pImageInfo = NULL;
		if (m_isUpdateTime)
		{
			m_isUpdateTime = false;
			pImageInfo = m_pGif->GetNextFrameInfo();
		}
		else
		{
			pImageInfo = m_pGif->GetCurrentFrameInfo();
		}
		if (pImageInfo)
		{
			RECT rcBmpPart = { 0 };
			RECT rcCorners = { 0 };
			rcBmpPart.right = pImageInfo->nX;
			rcBmpPart.bottom = pImageInfo->nY;
			CRenderEngine::DrawImage(hDC, pImageInfo->hBitmap, m_rcItem, m_rcPaint, rcBmpPart, rcCorners, true, 255);
			if (m_nPreUpdateDelay != pImageInfo->delay)
			{
				m_pManager->KillTimer(this, GIF_TIMER_ID);
				m_pManager->SetTimer(this, GIF_TIMER_ID, pImageInfo->delay);
				m_nPreUpdateDelay = pImageInfo->delay;
			}
		}

        __super::PaintStatusImage(hDC);
	}

	void CButtonGifUI::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_TIMER && event.wParam == GIF_TIMER_ID )
		{
			m_isUpdateTime = true;
			Invalidate();
			return;
		}
        __super::DoEvent(event);
	}

	void CButtonGifUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if( _tcscmp(pstrName, _T("GifFile")) == 0 )
		{
			SetGifFile(pstrValue);
			return;
		}
        __super::SetAttribute(pstrName, pstrValue);
	}

	LPVOID CButtonGifUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, _T("ButtonGif")) == 0)
			return static_cast<CButtonGifUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	LPCTSTR CButtonGifUI::GetGifFile()
	{
		return m_strGifPath.GetData();
	}

	void CButtonGifUI::SetGifFile( LPCTSTR pstrName )
	{
		if(pstrName == NULL) return;
		m_strGifPath = pstrName;
		if (m_pGif)
		{
			m_pManager->KillTimer(this, GIF_TIMER_ID);
			m_nPreUpdateDelay = 0;
			delete m_pGif;
			m_pGif = NULL;
		}
		m_pGif = CRenderEngine::LoadGif(STRINGorID(pstrName),0, 0);
		Invalidate();
	}
}