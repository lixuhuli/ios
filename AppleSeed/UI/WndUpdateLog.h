#pragma once

class CWndUpdateLog : public CWndBase {
public:
    CWndUpdateLog();
	virtual ~CWndUpdateLog();

protected:
	virtual LPCWSTR GetWndName() const { return L"更新日志"; }
	virtual LPCWSTR GetXmlPath() const;
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnMsgGetUpdateLog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	bool OnClickBtnClose(void* lpParam);

	BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE(label_prifix_, CRichEditUI, L"label_prifix")
        DECLARE_CTRL_TYPE(label_version_, CLabelUI, L"label_version")
        DECLARE_CTRL_TYPE(label_icon_, CControlUI, L"label_icon")
	END_INIT_CTRL()

	BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK(L"btn_close", &CWndUpdateLog::OnClickBtnClose)
	END_BIND_CTRL()

private:
    CRichEditUI* label_prifix_;
    CLabelUI* label_version_;
    CControlUI* label_icon_;
};

