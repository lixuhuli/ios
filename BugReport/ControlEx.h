#pragma once


class ComputerExamineUI : public CContainerUI
{
public:
	ComputerExamineUI()
	{
		CDialogBuilder builder;
		CContainerUI* pComputerExamine = static_cast<CContainerUI*>(builder.Create(_T("ComputerExamine.xml"), (UINT)0));
		if( pComputerExamine ) 
		{
			this->Add(pComputerExamine);
		}
		else 
		{
			this->RemoveAll();
			return;
		}
	}
};

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass) 
	{
		if (_tcscmp(pstrClass, _T("CheckBox")) == 0)
		{
			return new CCheckBoxUI();
		}
		else if (_tcscmp(pstrClass, _T("ButtonGif")) == 0)
		{
			return new CButtonGifUI();
		}

		return NULL;
	}
};