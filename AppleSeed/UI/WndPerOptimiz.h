#pragma once

class CWndPerOptimiz : public CWndBase {
public:
    CWndPerOptimiz();
	virtual ~CWndPerOptimiz();

protected:
	virtual LPCWSTR GetWndName() const { return L"性能优化"; }
	virtual LPCWSTR GetXmlPath() const;
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	bool OnClickBtnClose(void* lpParam);
    bool OnClickBtnOpenVT(void* param);
    bool OnClickBtnSet360(void* param);
    bool OnClickCheckShowPer(void* param);

	BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE(btn_open_vt_, CButtonUI, L"btn_open_vt")
        DECLARE_CTRL_TYPE(btn_set_360_, CButtonUI, L"btn_set_360")
        DECLARE_CTRL_TYPE(cpu_vt_logo_, CControlUI, L"cpu_vt_logo")
        DECLARE_CTRL_TYPE(check_show_per_, COptionUI, L"check_show_per")
	END_INIT_CTRL()

	BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK(L"btn_close", &CWndPerOptimiz::OnClickBtnClose)
        BIND_CTRL_CLICK(L"btn_open_vt", &CWndPerOptimiz::OnClickBtnOpenVT)
        BIND_CTRL_CLICK(L"btn_set_360", &CWndPerOptimiz::OnClickBtnSet360)
        BIND_CTRL_CLICK(L"check_show_per", &CWndPerOptimiz::OnClickCheckShowPer)
	END_BIND_CTRL()

private:
    void UpdateVTBtnStatus(bool open);

private:
    CButtonUI* btn_open_vt_;
    CButtonUI* btn_set_360_;
    CControlUI* cpu_vt_logo_;
    COptionUI* check_show_per_;
};

