#ifndef __UIGIF_H__
#define __UIGIF_H__

#pragma once

namespace DuiLib
{
	class CGifUI : public CContainerUI
	{
	public:
		CGifUI(void);
		~CGifUI(void);

		LPCTSTR GetClass() const { return _T("GifUI"); }
		LPVOID GetInterface( LPCTSTR pstrName )
		{
			if( _tcscmp(pstrName, _T("Gif")) == 0 )
				return static_cast<CGifUI*>(this);

			return CControlUI::GetInterface(pstrName);
		}

		virtual void DoEvent(TEventUI& event);
		virtual void SetVisible(bool bVisible = true);
		virtual void SetInternVisible(bool bVisible  = true);
		virtual void Init();
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	protected:
		enum { GIF_TIMER_ID = 15 };
		int m_nTimerElapse;

		int m_nIndex;
		int m_nCount;
		int m_nStopAt;
		int m_nRound;
		bool m_bStopped;
		CDuiString m_sImageFolder;
		CDuiString m_sImagePostfix;

		void Start(int nStartIndex = 0);
		void Stop(int nStopIndex = -1);
	};
}// namespace DuiLib

#endif // __UIGIF_H__

