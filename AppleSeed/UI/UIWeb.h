#ifndef __UIWEB_H__
#define __UIWEB_H__

#pragma once

#define DUI_UI_WEB   (_T("Web"))

class CWndWeb;
class CWebUI :
    public CControlUI {
public:
    CWebUI();
    virtual ~CWebUI();

public:
    virtual void Init() override;
    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    virtual void SetPos(RECT rc) override;
    virtual void SetFocus() override;
    virtual void UpdatePos(const QRect& rc);

public:
    void Navigate(LPCTSTR lpUrl);

private:
    CWndWeb* wnd_web_;
};

#endif // __UIWEB_H__