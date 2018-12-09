#include "stdafx.h"
#include "UICheckBox.h"

namespace DuiLib
{
	LPCTSTR CCheckBoxUI::GetClass() const
	{
		return _T("CheckBoxUI");
	}

	void CCheckBoxUI::SetCheck(bool bCheck, bool bNotify /*= true*/)
	{
		Selected(bCheck, bNotify);
	}

	bool  CCheckBoxUI::GetCheck() const
	{
		return IsSelected();
	}
}
