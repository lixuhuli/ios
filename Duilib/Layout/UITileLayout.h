#ifndef __UITILELAYOUT_H__
#define __UITILELAYOUT_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CTileLayoutUI : public CContainerUI
	{
	public:
		CTileLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetPos(RECT rc);

		SIZE GetItemSize() const;
		void SetItemSize(SIZE szItem);
		int GetColumns() const;
		void SetColumns(int nCols);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void SetLineSpacing(int nValue);
		int GetLineSpacing();

        int GetDisplayWidth() const;
        void SetDisplayWidth(int cx);

        int GetDisplayHeight() const;
        void SetDisplayHeight(int cy);

        int GetOverWidth() const;
        void SetOverWidth(int width);

        void SetHorNoSpace(bool bHorNoSpace = true);

	protected:
		SIZE m_szItem;
        SIZE m_szDisplay;
		int m_nColumns;
		int m_nLineSpacing;
        int m_nOverWidth;
        bool m_bHorNoSpace;
	};
}
#endif // __UITILELAYOUT_H__
