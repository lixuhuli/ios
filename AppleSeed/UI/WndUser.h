#ifndef _WND_USER_INCLUDE_H_
#define _WND_USER_INCLUDE_H_

#pragma once

class CWndUser
    : public CWndBase {
public:
    CWndUser();
    virtual ~CWndUser();

public:

protected:
	virtual LPCWSTR GetWndName() const override { return L"用户信息"; }
	virtual LPCWSTR GetXmlPath() const override;
    virtual void InitWindow() override;

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnGetVerificationCode(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnGetVerificationCode2(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnUserCheckCode(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnUserRegister(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnUserLogin(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnTimer(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnUserFindPassword(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnUserCodeLogin(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnUserCheckMobileCode(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnUserModifyPassword(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnUserKeyDown(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    BEGIN_INIT_CTRL()
        DECLARE_CTRL_TYPE(layout_user_, CTabLayoutUI, L"layout_user")
        DECLARE_CTRL_TYPE(btn_back_, CButtonUI, L"btn_back")
        DECLARE_CTRL_TYPE(btn_forget_password_, CButtonUI, L"btn_forget_password")
        DECLARE_CTRL_TYPE(edit_phone_, CEditUI, L"edit_phone")
        DECLARE_CTRL_TYPE(btn_verification_code_, CButtonUI, L"btn_verification_code")
        DECLARE_CTRL_TYPE(btn_next_, CButtonUI, L"btn_next")
        DECLARE_CTRL_TYPE(btn_next2_, CButtonUI, L"btn_next2")
        DECLARE_CTRL_TYPE(edit_verification_code_, CEditUI, L"edit_verification_code")
        DECLARE_CTRL_TYPE(edit_active_code_, CEditUI, L"edit_active_code")
        DECLARE_CTRL_TYPE(edit_password_, CEditUI, L"edit_password")
        DECLARE_CTRL_TYPE(check_show_password_, COptionUI, L"check_show_password")
        DECLARE_CTRL_TYPE(btn_login_, CButtonUI, L"btn_login")
        DECLARE_CTRL_TYPE(btn_login2_, CButtonUI, L"btn_login2")
        DECLARE_CTRL_TYPE(btn_login3_, CButtonUI, L"btn_login3")
        DECLARE_CTRL_TYPE(btn_confirm_, CButtonUI, L"btn_confirm")
        DECLARE_CTRL_TYPE(edit_input_user_, CEditUI, L"edit_input_user")
        DECLARE_CTRL_TYPE(edit_input_password_, CEditUI, L"edit_input_password")
        DECLARE_CTRL_TYPE(btn_verification_code2_, CButtonUI, L"btn_verification_code2")
        DECLARE_CTRL_TYPE(edit_phone2_, CEditUI, L"edit_phone2")
        DECLARE_CTRL_TYPE(edit_verification_code2_, CEditUI, L"edit_verification_code2")
        DECLARE_CTRL_TYPE(lbl_password_prefix_, CLabelUI, L"lbl_password_prefix")
        DECLARE_CTRL_TYPE(check_remember_password_, COptionUI, L"check_remember_password")
        DECLARE_CTRL_TYPE(check_auto_login_, COptionUI, L"check_auto_login")
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
        BIND_CTRL_CLICK(L"btn_back", &CWndUser::OnBtnClickBack);
        BIND_CTRL_CLICK(L"btn_forget_password", &CWndUser::OnBtnClickForgetPassword);
        BIND_CTRL_CLICK(L"btn_confirm", &CWndUser::OnBtnClickConfirm);
        BIND_CTRL_CLICK(L"btn_close", &CWndUser::OnBtnClickClose);
        BIND_CTRL_CLICK(L"btn_register", &CWndUser::OnBtnClickRegister);
        BIND_CTRL_CLICK(L"btn_next", &CWndUser::OnBtnClickNext);
        BIND_CTRL_CLICK(L"btn_next2", &CWndUser::OnBtnClickNext2);
        BIND_CTRL_CLICK(L"btn_verification_code", &CWndUser::OnBtnClickVerificationCode);
        BIND_CTRL_CLICK(L"btn_verification_code2", &CWndUser::OnBtnClickVerificationCode2);
        BIND_CTRL_EVENT(L"edit_phone", DUI_MSGTYPE_TEXTCHANGED, &CWndUser::OnEditPhoneTextChanged);
        BIND_CTRL_EVENT(L"edit_phone2", DUI_MSGTYPE_TEXTCHANGED, &CWndUser::OnEditPhoneTextChanged2);
        BIND_CTRL_EVENT(L"edit_verification_code", DUI_MSGTYPE_TEXTCHANGED, &CWndUser::OnEditVerificationCodeTextChanged);
        BIND_CTRL_EVENT(L"edit_verification_code2", DUI_MSGTYPE_TEXTCHANGED, &CWndUser::OnEditVerificationCodeTextChanged2);
        BIND_CTRL_EVENT(L"edit_active_code", DUI_MSGTYPE_TEXTCHANGED, &CWndUser::OnEditPhoneActiveCodeChanged);
        BIND_CTRL_CLICK(L"btn_active_code", &CWndUser::OnBtnClickActiveCode);
        BIND_CTRL_CLICK(L"btn_back2", &CWndUser::OnBtnClickBack2);
        BIND_CTRL_CLICK(L"btn_back3", &CWndUser::OnBtnClickBack3);
        BIND_CTRL_CLICK(L"check_show_password", &CWndUser::OnCheckShowPassword);
        BIND_CTRL_CLICK(L"btn_login3", &CWndUser::OnBtnClickLogin3);
        BIND_CTRL_CLICK(L"btn_login2", &CWndUser::OnBtnClickLogin2);
        BIND_CTRL_CLICK(L"btn_login", &CWndUser::OnBtnClickLogin);
        BIND_CTRL_CLICK(L"btn_used_item", &CWndUser::OnBtnUsedItem);
        BIND_CTRL_CLICK(L"btn_private_item", &CWndUser::OnBtnPrivateItem);
        BIND_CTRL_EVENT(L"edit_password", DUI_MSGTYPE_TEXTCHANGED, &CWndUser::OnEditPasswordChanged);
        BIND_CTRL_EVENT(L"edit_input_user", DUI_MSGTYPE_TEXTCHANGED, &CWndUser::OnEditInputUserChanged);
        BIND_CTRL_EVENT(L"edit_input_password", DUI_MSGTYPE_TEXTCHANGED, &CWndUser::OnEditInputPasswordTextChanged);
        BIND_CTRL_EVENT(L"edit_input_user", DUI_MSGTYPE_RETURN, &CWndUser::OnEditReturn);
        BIND_CTRL_EVENT(L"edit_input_password", DUI_MSGTYPE_RETURN, &CWndUser::OnEditReturn);
    END_BIND_CTRL()

    /* --------------------------------------------------------- */
    bool OnBtnClickClose(void* param);

    /* --------------------------------------------------------- */
    bool OnBtnClickForgetPassword(void* param);
    bool OnBtnClickLogin(void* param);
    bool OnBtnClickRegister(void* param);
    bool OnEditInputUserChanged(void* param);
    bool OnEditInputPasswordTextChanged(void* param);
    bool OnEditReturn(void* param);


    /* --------------------------------------------------------- */
    bool OnBtnClickBack(void* param);
    bool OnBtnClickNext(void* param);
    bool OnBtnClickVerificationCode(void* param);

    bool OnEditPhoneTextChanged(void* param);
    bool OnEditVerificationCodeTextChanged(void* param);
    bool OnEditPhoneActiveCodeChanged(void* param);

    bool OnBtnClickActiveCode(void* param);

    bool OnBtnUsedItem(void* param);
    bool OnBtnPrivateItem(void* param);


    /* --------------------------------------------------------- */
    bool OnBtnClickBack2(void* param);
    bool OnCheckShowPassword(void* param);
    bool OnBtnClickLogin2(void* param);
    bool OnBtnClickConfirm(void* param);
    bool OnEditPasswordChanged(void* param);


    /* --------------------------------------------------------- */
    bool OnBtnClickBack3(void* param);
    bool OnBtnClickNext2(void* param);
    bool OnBtnClickVerificationCode2(void* param);
    bool OnBtnClickLogin3(void* param);

    bool OnEditPhoneTextChanged2(void* param);
    bool OnEditVerificationCodeTextChanged2(void* param);


protected:
    void UpdateWindowPos(bool min_size);
    void UpdateNextBtnStatus();
    void UpdateBtnVerificationCode();
    void UpdateBtnLoginStatus();
    void UpdateNextBtnStatus2();
    void UpdateBtnLoginStatus3();
    void UpdateBtnVerificationCode2();

    void IntEditEvent();

private:
    CTabLayoutUI* layout_user_;

    /* --------------------------------------------------------- */
    CButtonUI* btn_forget_password_;
    CButtonUI* btn_login_;
    CEditUI* edit_input_user_;
    CEditUI* edit_input_password_;
    COptionUI* check_remember_password_;
    COptionUI* check_auto_login_;


    /* --------------------------------------------------------- */
    CButtonUI* btn_verification_code_;
    CButtonUI* btn_next_;
    CButtonUI* btn_back_;

    CEditUI* edit_phone_;
    CEditUI* edit_verification_code_;
    CEditUI* edit_active_code_;

    int verification_count_down_;

    /* --------------------------------------------------------- */
    CEditUI* edit_password_;
    COptionUI* check_show_password_;
    CButtonUI* btn_login2_;
    CButtonUI* btn_confirm_;
    CLabelUI* lbl_password_prefix_;

    /* --------------------------------------------------------- */



    /* --------------------------------------------------------- */
    wstring login_password_temp;
    CButtonUI* btn_verification_code2_;
    CButtonUI* btn_next2_;
    CButtonUI* btn_login3_;

    CEditUI* edit_phone2_;
    CEditUI* edit_verification_code2_;

    int verification_count_down2_;
};

#endif  // !#define (_WND_USER_INCLUDE_H_) 