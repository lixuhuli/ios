#ifndef __UICONTROL_H__
#define __UICONTROL_H__

#pragma once

#include "animation/AnimInterface.h"
#include "animation/AnimationManager.h"
#include "animation/FadeAnimator.h"
#include "animation/Animator.h"
#include "animation/ScaleAnimator.h"
#include "animation/MoveAnimator.h"

#define DEFAULT_CONTROL_STORYBOARD _T("default_control_storyboard")

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//

typedef CControlUI* (CALLBACK* FINDCONTROLPROC)(CControlUI*, LPVOID);

class UILIB_API CControlUI
    : public CEventSets
    , public IAnimObject
    , public CAnimationManagerProxy
{
public:
    static QUI_STRING Item_Hot;
    static QUI_STRING Item_Not_Hot;

public:
    class CControlPropertyAnimOjbect : public IAnimObject
    {
    public:
        CControlPropertyAnimOjbect(CControlUI* pControl);
        ~CControlPropertyAnimOjbect();
        friend class CControlUI;
    public:
        bool IsAttachedToAnimator();
    public:
        virtual DuiLib::QRect GetPos();
        void  SetPosition(int x, int y, bool bRelative = false);
        void  SetSize(int iWidth, int iHeight);
        void  SetKeyWithValue(QUI_STRING strKeyName, int iValue);
        DuiLib::CPaintManagerUI* GetManager() const;
        void  SetBkImage(LPCTSTR pStrImage);

    protected:
        void RefreshItemRect();
    private:
        RECT m_rcItem;//相对坐标，相对于control左上角
        CControlUI* m_pControl;
        int  m_iRotation;
        int  m_iAlpha;
    };

private:
    enum EffectProperty
    {
        NONE = 0,
        BKCOLOR = 1,
        BKIMAGE = 2,
        STATUSIMAGE = 4,
        TEXT = 8,
        BORDER = 16,
        ALL = 31,
    };

public:
    CControlUI();
    virtual ~CControlUI();

public:
    virtual CDuiString GetName() const;
    virtual void SetName(LPCTSTR pstrName);
    virtual LPCTSTR GetClass() const;
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual UINT GetControlFlags() const;

    virtual bool Activate();
    virtual CPaintManagerUI* GetManager() const;
    virtual void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);
    virtual CControlUI* GetParent() const;

    // 文本相关
    virtual CDuiString GetText() const;
    virtual void SetText(LPCTSTR pstrText);

    // 图形相关
    DWORD GetBkColor() const;
    void SetBkColor(DWORD dwBackColor);
    DWORD GetBkColor2() const;
    void SetBkColor2(DWORD dwBackColor);
    DWORD GetBkColor3() const;
    void SetBkColor3(DWORD dwBackColor);
    LPCTSTR GetBkImage();
    void SetBkImage(LPCTSTR pStrImage);
	DWORD GetFocusBorderColor() const;
	void SetFocusBorderColor(DWORD dwBorderColor);
    bool IsColorHSL() const;
    void SetColorHSL(bool bColorHSL);
    SIZE GetBorderRound() const;
    void SetBorderRound(SIZE cxyRound);
    bool DrawImage(HDC hDC, LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);

	//边框相关
	int GetBorderSize() const;
	void SetBorderSize(int nSize);
	DWORD GetBorderColor() const;
	void SetBorderColor(DWORD dwBorderColor);

	void SetBorderSize(RECT rc);
	int GetLeftBorderSize() const;
	void SetLeftBorderSize(int nSize);
	int GetTopBorderSize() const;
	void SetTopBorderSize(int nSize);
	int GetRightBorderSize() const;
	void SetRightBorderSize(int nSize);
	int GetBottomBorderSize() const;
	void SetBottomBorderSize(int nSize);
	int GetBorderStyle() const;
	void SetBorderStyle(int nStyle);

    // 位置相关
    virtual QRect GetPos();
    virtual void SetPos(RECT rc);
    virtual int GetWidth() const;
    virtual int GetHeight() const;
    virtual int GetX() const;
    virtual int GetY() const;
    virtual RECT GetPadding() const;
    virtual void SetPadding(RECT rcPadding); // 设置外边距，由上层窗口绘制
    virtual SIZE GetFixedXY() const;         // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedXY(SIZE szXY);      // 仅float为true时有效
    virtual int GetFixedWidth() const;       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedWidth(int cx);      // 预设的参考值
    virtual int GetFixedHeight() const;      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedHeight(int cy);     // 预设的参考值
    virtual int GetMinWidth() const;
    virtual void SetMinWidth(int cx);
    virtual int GetMaxWidth() const;
    virtual void SetMaxWidth(int cx);
    virtual int GetMinHeight() const;
    virtual void SetMinHeight(int cy);
    virtual int GetMaxHeight() const;
    virtual void SetMaxHeight(int cy);
    virtual void SetRelativePos(SIZE szMove,SIZE szZoom);
    virtual void SetRelativeParentSize(SIZE sz);
    virtual TRelativePosUI GetRelativePos() const;
    virtual bool IsRelativePos() const;

    // 鼠标提示
    virtual CDuiString GetToolTip() const;
    virtual void SetToolTip(LPCTSTR pstrText);
	virtual void SetToolTipWidth(int nWidth);
	virtual int	  GetToolTipWidth(void);	// 多行ToolTip单行最长宽度

    // 快捷键
    virtual TCHAR GetShortcut() const;
    virtual void SetShortcut(TCHAR ch);

    // 菜单
    virtual bool IsContextMenuUsed() const;
    virtual void SetContextMenuUsed(bool bMenuUsed);

    // 用户属性
    virtual const CDuiString& GetUserData(); // 辅助函数，供用户使用
    virtual void SetUserData(LPCTSTR pstrText); // 辅助函数，供用户使用
    virtual UINT_PTR GetTag() const; // 辅助函数，供用户使用
    virtual void SetTag(UINT_PTR pTag); // 辅助函数，供用户使用
	virtual void SetInheritableUserData(LPCTSTR szUserData);
	virtual const CDuiString& GetInheritableUserData();
	virtual void SetInheritableTag(UINT_PTR pTag);
	virtual UINT_PTR GetInheritableTag();

    // 一些重要的属性
	virtual bool GetVisible() const;
    virtual bool IsVisible() const;
    virtual void SetVisible(bool bVisible = true);
    virtual void SetInternVisible(bool bVisible = true); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
    virtual bool IsEnabled() const;
    virtual void SetEnabled(bool bEnable = true);
    virtual bool IsMouseEnabled() const;
    virtual void SetMouseEnabled(bool bEnable = true);
    virtual bool IsKeyboardEnabled() const;
    virtual void SetKeyboardEnabled(bool bEnable = true);
    virtual bool IsFocused() const;
    virtual void SetFocus();
    virtual bool IsFloat() const;
    virtual void SetFloat(bool bFloat = true);

    virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

    void Invalidate();
    bool IsUpdateNeeded() const;
    void NeedUpdate();
    void NeedParentUpdate();
    DWORD GetAdjustColor(DWORD dwColor);

    virtual void Init();
    virtual void DoInit();

    virtual void Event(TEventUI& event);
    virtual void DoEvent(TEventUI& event);

    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    CControlUI* ApplyAttributeList(LPCTSTR pstrList);

    virtual SIZE EstimateSize(SIZE szAvailable);

    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
    virtual void PaintBkColor(HDC hDC);
    virtual void PaintBkImage(HDC hDC);
    virtual void PaintStatusImage(HDC hDC);
    virtual void PaintText(HDC hDC);
    virtual void PaintBorder(HDC hDC);

    virtual void DoPostPaint(HDC hDC, const RECT& rcPaint);

	//虚拟窗口参数
	void SetVirtualWnd(LPCTSTR pstrValue);
	CDuiString GetVirtualWnd() const;

    virtual void SetTransparent(BYTE btAlpha) { m_btAlpha = btAlpha; }
    BYTE GetTransparent() const { return m_btAlpha; }

    void SetRotation(int iValue) { m_iRotation = iValue; }
    int  GetRotation() { return m_iRotation; }

    bool OnEvent_Impl(void* pEvent);
    bool OnNotify_Impl(void* pEvent);

    IAnimObject* BkImageAnimObject();
    IAnimObject* StatusImageAnimObject();

    //IAnimObject
    virtual void SetBKBitmap(HBITMAP hBmp);
    //bool FireAnimationProgressedEvent(void* pAnimation);
    //bool FireAnimationEndEvent(void* pAnimation);
    //bool FireAnimationStartEvent(void *pAnimation);
    void  SetPosition(int x, int y, bool bRelative = false);
    void  SetSize(int iWidth, int iHeight);
    virtual void  SetKeyWithValue(QUI_STRING strKeyName, int iValue);

    QUI_STRING GetDefaultStoryboardName();

    virtual bool OnFocusInAnimationEnd();
    virtual bool OnFocusOutAnimationEnd();
    bool OnFocusInFadeEffectEnd(void* p);
    bool OnFocusOutFadeEffectEnd(void* p);

    void SetFadeEffectProperty(int nProperty);
    void SetFadeEffectProperty(LPCTSTR pProperty);
    int  GetFadeEffectProperty() const;

    void PaintToBoard(CDrawingBoard& aBoard, const RECT& rcPaint);
    bool IsParentHasAlpha();//判断父控件的alpha值是否在0和255之间

    void SetCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    LPCTSTR GetCustomAttribute(LPCTSTR pstrName);

protected:
    void EffectPaint(EffectProperty aProperty, HDC hDC);
    void EffectPaint(EffectProperty aProperty, HDC hDC, CDrawingBoard& aBoard);
    void DoAlphaPaint(HDC hDC, const RECT& rcPaint);
    EffectProperty GetEffectProperty(LPCTSTR pProperty);
    void StartFocusInFadeEffect();
    void StartFocusOutFadeEffect();
    bool IsNeedUseEffect();
    bool DefaultAnimEventHandler(void* pNotify);
    void DoDefaultAnimEventHandlerInit();

public:
    CEventSource OnInit;
    CEventSource OnDestroy;
    CEventSource OnSize;
    CEventSource OnEvent;
    CEventSource OnNotify;

protected:
    CPaintManagerUI* m_pManager;
    CControlUI* m_pParent;
	CDuiString m_sVirtualWnd;
    CDuiString m_sName;
    bool m_bUpdateNeeded;
    bool m_bMenuUsed;
    RECT m_rcItem;
    RECT m_rcPadding;
    SIZE m_cXY;
    SIZE m_cxyFixed;
    SIZE m_cxyMin;
    SIZE m_cxyMax;
    bool m_bVisible;
    bool m_bInternVisible;
    bool m_bEnabled;
    bool m_bMouseEnabled;
	bool m_bKeyboardEnabled ;
    bool m_bFocused;
    bool m_bFloat;
    bool m_bSetPos; // 防止SetPos循环调用
    TRelativePosUI m_tRelativePos;

    CDuiString m_sText;
    CDuiString m_sToolTip;
    TCHAR m_chShortcut;
    CDuiString m_sUserData;
    UINT_PTR m_pTag;
	CDuiString m_sInheritableUserData;
	UINT_PTR m_pInheritableTag;

    DWORD m_dwBackColor;
    DWORD m_dwBackColor2;
    DWORD m_dwBackColor3;
    CDuiString m_sBkImage;
	CDuiString m_sForeImage;
    DWORD m_dwBorderColor;
	DWORD m_dwFocusBorderColor;
    bool m_bColorHSL;
    int m_nBorderSize;
	int m_nBorderStyle;
	int m_nTooltipWidth;
    SIZE m_cxyBorderRound;
    RECT m_rcPaint;
	RECT m_rcBorderSize;

    bool m_bFadeEffectEnd;
    int m_aFadeEffectProperty;

    BYTE m_btAlpha;

    int m_nFadeTime;

    int m_iRotation;

    CControlPropertyAnimOjbect m_bkImageObject;
    CControlPropertyAnimOjbect m_stausImageObject;

    typedef std::map<CDuiString, CDuiString> CustomAttributeMap;
    typedef std::map<CDuiString, CStoryBoard*> DefaultEventSotryBoardMap;
    CustomAttributeMap m_customAttributeMap;
    DefaultEventSotryBoardMap m_defaultEventStoryboardMap;
};

} // namespace DuiLib

#endif // __UICONTROL_H__
