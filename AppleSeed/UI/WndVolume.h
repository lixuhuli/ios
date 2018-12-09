#ifndef _WND_VOLUME_INCLUDE_H_
#define _WND_VOLUME_INCLUDE_H_

#pragma once

class CWndVolume
    : public CWndBase {
public:
    CWndVolume();
    virtual ~CWndVolume();

public:

protected:
	virtual LPCWSTR GetWndName() const override { return L"…˘“Ù…Ë÷√"; }
	virtual LPCWSTR GetXmlPath() const override;
    virtual void InitWindow() override;

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
    virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    BEGIN_INIT_CTRL()
    END_INIT_CTRL()

    BEGIN_BIND_CTRL()
    END_BIND_CTRL()

protected:
    

private:

};

#endif  // !#define (_WND_VOLUME_INCLUDE_H_) 