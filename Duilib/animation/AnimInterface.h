#ifndef __ANIMINTERFACE_H__
#define __ANIMINTERFACE_H__

#include <WinDef.h>
class CAnimator;
class UILIB_API IAnimatorListener
{
public:
	virtual ~IAnimatorListener(){};
	virtual void OnAnimatorStart(CAnimator* pAnimator) = 0;
	virtual void OnAnimatorStoped(CAnimator* pAnimator) = 0;
};

class CAnimation;
class UILIB_API IAnimationListener
{
public:
	virtual ~IAnimationListener(){};
	virtual void OnAnimatonStart(CAnimation* pAnimation) = 0;
	virtual void OnAnimationProgressed(CAnimation* pAnimation) = 0;
	virtual void OnAnimationStoped(CAnimation* pAnimation) = 0;
};

class CStoryBoard;
class UILIB_API IStoryBoardListener
{
public:
	virtual ~IStoryBoardListener(){};
	virtual void OnStoryBoardStart(CStoryBoard* pAnimation) = 0;
	virtual void OnStoryBoardProgressed(CStoryBoard* pAnimation) = 0;
	virtual void OnStoryBoardStoped(CStoryBoard* pAnimation) = 0;
};

namespace DuiLib
{
	class CPaintManagerUI;
};

class UILIB_API IAnimObject
{
public:
    virtual DuiLib::QRect GetPos() = 0;
    virtual void  SetPosition(int x, int y, bool bRelative = false) = 0;
    virtual void  SetSize(int iWidth, int iHeight) = 0;
	virtual void  SetKeyWithValue(QUI_STRING strKeyName, int iValue) = 0;

	virtual void  AddToAnimator(CAnimator* pAnimator){}
	virtual void  RemoveFromAnimator(CAnimator* pAnimator){}

	//image animator∂¿”√
	virtual DuiLib::CPaintManagerUI* GetManager() const = 0;
	virtual void  SetBkBitmap(HBITMAP hBmp){};//rotate
	virtual void  SetBkImage(LPCTSTR pStrImage) = 0;//transition
};

#endif//end __ANIMINTERFACE_H__