#ifndef __UILABEL_H__
#define __UILABEL_H__

#pragma once
#include <string>

namespace DuiLib
{
	class UILIB_API CLabelUI : public CControlUI
	{
	public:
		CLabelUI();
		~CLabelUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetTextStyle(UINT uStyle);
		UINT GetTextStyle() const;
		void SetTextColor(DWORD dwTextColor);
		DWORD GetTextColor() const;
		void SetDisabledTextColor(DWORD dwTextColor);
		DWORD GetDisabledTextColor() const;
		void SetFont(int index);
		int GetFont() const;
		RECT GetTextPadding() const;
		void SetTextPadding(RECT rc);
		bool IsShowHtml();
		void SetShowHtml(bool bShowHtml = true);

		SIZE EstimateSize(SIZE szAvailable);
		SIZE GetTextSize(LPCTSTR szText = NULL);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintText(HDC hDC);
		//��չ����
		void PaintBkImage(HDC hDC);
		void SetBufferImage(CStdString sImageName, LPCSTR lpsz, int nLen = -1);
		bool DrawBufferImage(HDC hDC, LPCTSTR pStrModify=NULL);
		virtual bool GetAutoCalcWidth() const;
		virtual void SetAutoCalcWidth(bool bAutoCalcWidth);
		
	protected:
		DWORD m_dwTextColor;
		DWORD m_dwDisabledTextColor;
		int	m_iFont;
		UINT m_uTextStyle;
		RECT m_rcTextPadding;
		bool m_bShowHtml;
		bool m_bMemoryImg;
		bool m_bDrawTextPlus;
		LPCSTR m_pImageData;
		int m_nLen;
		bool m_bAutoCalcWidth;
	};
}

#endif // __UILABEL_H__