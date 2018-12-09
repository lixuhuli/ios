#pragma once
//////////////////////////////////////////////////
//初始化控件
#define BEGIN_INIT_CTRL()			\
protected:							\
	virtual void InitControls()		\
	{

#define DECLARE_CTRL_TYPE(ptr,cls,name)					\
	(ptr = dynamic_cast<cls>(m_pm.FindControl(name)));	\
	ASSERT(ptr);

#define DECLARE_CTRL_BIND(ptr,cls,name,func)			\
	(ptr = dynamic_cast<cls>(m_pm.FindControl(name)));	\
	ASSERT(ptr);										\
	ptr->OnNotify += MakeDelegate(this, func);

#define DECLARE_CTRL(ptr,name)			\
	(ptr = m_pm.FindControl(name));		\
	ASSERT(ptr);


#define END_INIT_CTRL()				\
	}
//////////////////////////////////////////////////
//绑定事件
#define BEGIN_BIND_CTRL()			\
protected:							\
	virtual void BindControls()		\
	{								\
	CControlUI* p = NULL;

#define BIND_CTRL(name,func)		\
	p = m_pm.FindControl(name);	\
if (p)						\
	p->OnNotify += MakeDelegate(this, func);

#define BIND_CTRL_PAGE(name,func)		\
	p = FindSubControl(name);	\
if (p)						\
	p->OnNotify += MakeDelegate(this, func);

#define END_BIND_CTRL()				\
	}

class CWndBase : 
	public CWindowWnd,
	public IDialogBuilderCallback
{
public:
	CWndBase();
	virtual ~CWndBase(void);
	//子类重写窗口的属性
	virtual LPCWSTR GetWndName()const = 0;
	virtual LPCWSTR GetXmlPath()const = 0;
	virtual LPCTSTR GetWindowClassName()const;
	
	virtual UINT GetClassStyle() const { return UI_CLASSSTYLE_DIALOG; }
	virtual void OnFinalMessage(HWND hWnd);
	virtual HWND Create(HWND hParentWnd, int nPosX = 0, int nPosY = 0);
	virtual HWND CreateModalWnd(HWND hParentWnd, int nPosX = 0, int nPosY = 0);

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
	virtual LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	virtual void InitWindow();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual bool QuitOnSysClose();
	virtual void SetWindowStyle(DWORD dwStyle);
	virtual void SetWindowExStyle(DWORD dwExStyle);

	BEGIN_INIT_CTRL()
	END_INIT_CTRL()
	BEGIN_BIND_CTRL()
	END_BIND_CTRL()

protected:
	CPaintManagerUI m_pm;
	DWORD m_dwStyle;
	DWORD m_dwExStyle;
	bool m_bShowOnTaskbar;
	bool m_bShowShadow;
	bool m_bEscapExit;

private:
	CWndShadow	*m_pShadowWnd;
};

