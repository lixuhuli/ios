#ifndef __UIBUTTONGIF_H__
#define __UIBUTTONGIF_H__

#pragma once

#include "UIButton.h"

namespace DuiLib
{
	class UILIB_API CButtonGifUI 
		: public COptionUI
	{
		enum
		{
			GIF_TIMER_ID = 15
		};
	public:	
		CButtonGifUI();
		~CButtonGifUI();

		LPCTSTR GetGifFile();
		void SetGifFile(LPCTSTR pstrName);
		void SetVisible(bool bVisible = true);//设置控件可视性

	protected:
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void DoEvent(TEventUI& event);
		virtual void PaintStatusImage(HDC hDC);

	private:
		bool m_isUpdateTime;
		CGifHandler* m_pGif;
		int m_nPreUpdateDelay;
		CDuiString    m_strGifPath;
	};
}// namespace DuiLib

#endif // __UIBUTTONGIF_H__