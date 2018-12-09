#pragma once

//////////////////////////////////////////////////
//初始化控件
#define BEGIN_INIT_CTRL()			\
protected:							\
	virtual void InitControls()		\
{

#define DECLARE_CTRL_TYPE(ptr, cls, name)					\
	(ptr = dynamic_cast<cls*>(m_pm.FindControl(name)));	\
	ASSERT(ptr);

#define DECLARE_CTRL_TYPE_PAGE(ptr, cls, root, name)			\
if (root)                                                       \
    (ptr = dynamic_cast<cls*>(root->FindSubControl(name))); 	\
	ASSERT(ptr);

#define DECLARE_CTRL_BIND(ptr,cls,name,func)			\
	(ptr = dynamic_cast<cls*>(m_pm.FindControl(name)));	\
	ASSERT(ptr);										\
	ptr->OnNotify += MakeDelegate(this, func);

#define DECLARE_CTRL_BIND_PAGE(ptr,cls,name,func)		\
	(ptr = dynamic_cast<cls*>(FindSubControl(name)));	\
	ASSERT(ptr);										\
	ptr->OnNotify += MakeDelegate(this, func);

#define DECLARE_CTRL(ptr,name)			\
	(ptr = m_pm.FindControl(name));		\
	ASSERT(ptr);

#define DECLARE_CTRL_PAGE(ptr,name)		\
	(ptr = FindSubControl(name));		\
	ASSERT(ptr);


#define END_INIT_CTRL()				\
}
//////////////////////////////////////////////////
//绑定事件
#define BEGIN_BIND_CTRL()			\
protected:							\
	virtual void BindControls()		\
{									\
	CControlUI* p = NULL;

#define BIND_CTRL(name, func)		\
	p = m_pm.FindControl(name);		\
if (p)								\
	p->OnNotify += MakeDelegate(this, func);

#define BIND_CTRL_PAGE(name, root, func)	\
if (root)                           \
    p = root->FindSubControl(name);		\
if (p)								\
	p->OnNotify += MakeDelegate(this, func);


#define BIND_CTRL_EVENT(name, event_name, func)		\
    p = m_pm.FindControl(name);		\
if (p)								\
    p->Subscribe(event_name, MakeDelegate(this, func));

#define BIND_CTRL_EVENT_PAGE(name, root, event_name, func)		\
if (root)                               \
    p = root->FindSubControl(name);		\
if (p)								\
    p->Subscribe(event_name, MakeDelegate(this, func));

#define BIND_CTRL_PTR_EVENT(ptr, event_name, func)		\
if (ptr)								                \
    ptr->Subscribe(event_name, MakeDelegate(this, func));


#define BIND_CTRL_CLICK(name, func)                      BIND_CTRL_EVENT(name, DUI_MSGTYPE_CLICK, func)
#define BIND_CTRL_CLICK_PAGE(name, root, func)           BIND_CTRL_EVENT_PAGE(name, root, DUI_MSGTYPE_CLICK, func)
#define BIND_CTRL_PTR_CLICK(ptr, func)                   BIND_CTRL_PTR_EVENT(ptr, DUI_MSGTYPE_CLICK, func)

#define BIND_CTRL_SELECT_CHANGED(name, func)             BIND_CTRL_EVENT(name, DUI_MSGTYPE_SELECTCHANGED, func)
#define BIND_CTRL_SELECT_CHANGED_PAGE(name, root, func)  BIND_CTRL_EVENT_PAGE(name, root, DUI_MSGTYPE_SELECTCHANGED, func)

#define BIND_CTRL_TIMER(name, func)                      BIND_CTRL_EVENT(name, DUI_MSGTYPE_TIMER, func)
#define BIND_CTRL_TIMER_PAGE(name, root, func)           BIND_CTRL_EVENT_PAGE(name, root, DUI_MSGTYPE_TIMER, func)
#define BIND_CTRL_PTR_TIMER(ptr, func)                   BIND_CTRL_PTR_EVENT(ptr, DUI_MSGTYPE_TIMER, func)


#define END_BIND_CTRL()				\
}

#define BIND_SUB_PAGE(ptr, cls, name) {     \
    auto p = m_pm.FindControl(name);		\
if (p)	ptr.reset(new cls(p));			    \
if (ptr) ptr->Init(); }

#define BIND_SUB_PAGE_ON_PAGE(ptr, cls, root, name) {     \
    auto p = root->FindSubControl(name);		          \
if (p)	ptr.reset(new cls(p));			                  \
if (ptr) ptr->Init(); }

#define RELEASE_SUB_PAGE(ptr)               \
if (ptr) { ptr->Clean(); ptr.reset(nullptr); }


class CWndBase : 
	public CWindowWnd,
	public IDialogBuilderCallback,
	public INotifyUI
{
public:
	CWndBase();
	virtual ~CWndBase(void);
	//子类重写窗口的属性
	virtual LPCWSTR GetWndName() const = 0;
	virtual LPCWSTR GetXmlPath() const = 0;
	virtual LPCTSTR GetWindowClassName()const = 0;
	
	virtual UINT GetClassStyle() const { return UI_CLASSSTYLE_DIALOG; }
	virtual void OnFinalMessage(HWND hWnd);
	virtual HWND Create(HWND hParentWnd, bool child = false, int nPosX = 0, int nPosY = 0);
	virtual HWND CreateModalWnd(HWND hParentWnd, bool bUseTopModal = true, int nPosX = 0, int nPosY = 0);

protected:
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	
	virtual void InitWindow();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual bool QuitOnSysClose();
	virtual void SetWindowStyle(DWORD dwStyle);
	virtual void SetWindowExStyle(DWORD dwExStyle);
	virtual void Notify(TNotifyUI& msg);
	virtual bool OnEscape();

	BEGIN_INIT_CTRL();
	END_INIT_CTRL();
	BEGIN_BIND_CTRL();
	END_BIND_CTRL();

protected:
	CPaintManagerUI m_pm;
	DWORD m_dwStyle;
	DWORD m_dwExStyle;
	bool m_bAddToMgr;
	bool m_bShowOnTaskbar;
	bool m_bShowShadow;
	bool m_bEscape;
	bool m_bRoundRect;

private:
	CWndShadow	*m_pShadowWnd;
};

