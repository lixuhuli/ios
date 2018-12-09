#ifndef __EFFECT_H__
#define __EFFECT_H__

#include "AnimationManager.h"
#include <GdiPlus.h>
#include <WinDef.h>
#include <vector>

class CUIEffect;
class CDrawingBoard;
struct sWndEffectFactor;
namespace DuiLib
{
	class CControlUI;
};
class CAnimator;

class UILIB_API CUIEffect : public IAnimObject
{
public:
	struct sWndEffectInfo
	{
		HWND hWndEffect;
		HWND hWnd;
		
	};
	typedef std::vector<CAnimator*> AnimatorArray;
	friend class CWndReginAnimator;
public:
	CUIEffect(HWND hWnd, bool bShowWhenAnimOver = true, bool bAutoDelete = true);
	CUIEffect(HWND hWnd, DuiLib::CControlUI* pControl, bool bAutoDelete = true);
	CUIEffect(HWND hWnd, HWND hWndToShow, bool bAutoDelete = true);
	~CUIEffect();

	IAnimObject* GetAnimObject();
	//call on effect animator
	void  AddToAnimator(CAnimator* pAnimator);
	void  RemoveFromAnimator(CAnimator* pAnimator);

	bool OnAnimatorStart(void* pAnimator);
	bool OnAnimatorStoped(void* pAnimator);
	bool OnAnimatorProgressed(void* pAnimator);

	//attribute
	HWND                 GetOwnerHWnd() const;
	HWND                 GetEffectHWnd();
	DuiLib::CControlUI*  GetEffectControl() const;
	CDrawingBoard*       GetDrawingBoard();
	void                 SetHasPainted(bool bPainted);

	//IAnimInterface
	virtual DuiLib::QRect GetPos();
	virtual void  SetPosition(int x, int y, bool bRelative = false);
	virtual void  SetSize(int iWidth, int iHeight);
	virtual void  SetKeyWithValue(QUI_STRING strKeyName, int iValue);

	virtual DuiLib::CPaintManagerUI* GetManager() const;

	virtual void SetBkBitmap(HBITMAP hBmp);
	virtual void SetBkImage(LPCTSTR pStrImage);
	void SetTransparent(int iAlpha);
	void SetRotation(int iValue);
	int  GetRotation(){return m_iRotation;}

	void Shutter();
	void SetShowWndWhenAnimOver(bool bShow){m_bNeedShowWndWhenAnimOver=bShow;}
	bool GetShowWndWhenAnimOver(){return m_bNeedShowWndWhenAnimOver;}

protected:
	CDrawingBoard* SnapShot();
	CDrawingBoard* SnapShotEx();
	HWND CreateEffectWnd();
	HWND CreateEffectWnd(DuiLib::QRect rt);
	bool RegisterWindowClass();
	static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void DoPaint();
	void InitDrawingBoard();
protected:
	HWND m_hWnd;
	HWND m_hEffectWnd;
	HWND m_hShowWnd;
	CDrawingBoard* m_pDrawingBoard;
	sWndEffectFactor* m_pFactor;
	DuiLib::CControlUI *m_pControl;
	AnimatorArray m_arrAnimators;
	int  m_iStartAnimatorCount;
	int  m_iStopAnimatorCount;
	bool m_bNeedShowWndWhenAnimOver;
	bool m_bAutoDelete;
	int  m_iProgressedCount;
	BLENDFUNCTION m_Blend;
	int  m_iAlpha;
	bool m_bHasPainted;
	int  m_iRotation;
};



#endif//end __EFFECT_H__