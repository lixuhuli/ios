#ifndef __ANIMATIONFACTOR_H__
#define __ANIMATIONFACTOR_H__

#include "PropertyAnimatior.h"
#include "ImageAnimator.h"
#include "EffectAnimator.h"
#include <GdiPlus.h>

class CImageAnimationAlgorithm;
class CDrawingBoard;

enum ImageAnimationDirection
{
    IMAGEANIMATION_TOP2BOTTOM = 0,
    IMAGEANIMATION_BOTTOM2TOP,
    IMAGEANIMATION_LEFT2RIGHT,
    IMAGEANIMATION_RIGHT2LEFT,
    IMAGEANIMATION_XROLLING,
    IMAGEANIMATION_YROLLING,
    IMAGEANIMATION_XYROLLING
};

struct UILIB_API sAnimatorFactor
{
    sAnimatorFactor() { }
    virtual ~sAnimatorFactor() { }
	//virtual CAnimator* CreateAnimator(int id, IAnimObject *pAnimInterface, CAnimation* pAnim) = 0;
	//virtual AnimatorType GetType() = 0;
};

//struct UILIB_API sPropertyAnimationFactor : public sAnimatorFactor
//{
//	POINT ptStart; //一定不要初始化，已经在构造函数中初始化完毕
//	POINT ptEnd; //一定不要初始化，已经在构造函数中初始化完毕
//
////     int iFrom;
////     int iTo;
//    DuiLib::CStdString strPropertyName;
//
//	sPropertyAnimationFactor()
//	{
//		ptStart.x = ptStart.y = -1;
//		ptEnd.x = ptEnd.y = -1;
//	}
//
//	virtual CAnimator* CreateAnimator(int id, IAnimObject *pAnimInterface, CAnimation* pAnim)
//	{
//		return new CPropertyAnimatior(id, pAnimInterface, pAnim);
//	}
//
//	virtual AnimatorType GetType()
//	{
//		return PROPERTYANIMATOR;
//	}
//};

struct UILIB_API sImageAnimationFactor : public sAnimatorFactor
{
    DuiLib::CStdString strSourceImage;
    DuiLib::CStdString strDestImage;

	//virtual CAnimator* CreateAnimator(int id, IAnimObject *pAnimInterface, CAnimation* pAnim)
	//{
	//	return new CBKImageAnimator(id, pAnimInterface, pAnim);
	//}

	//virtual AnimatorType GetType()
	//{
	//	return BKIMAGEANIMATOR;
	//}

    virtual CImageAnimationAlgorithm* CreateAlgorithmObject(DuiLib::CControlUI* pControl) { return NULL; }
	virtual CImageAnimationAlgorithm* CreateAlgorithmObject(IAnimObject *pAnimInterface) {return NULL;}
};

struct UILIB_API sTransitionImageAnimationFactor : public sImageAnimationFactor
{
    virtual CImageAnimationAlgorithm* CreateAlgorithmObject(DuiLib::CControlUI* pControl) 
    { 
        return new CTransitionImageAnimation(this, pControl);
    }

	virtual CImageAnimationAlgorithm* CreateAlgorithmObject(IAnimObject *pAnimInterface)
	{
		return new CTransitionImageAnimation(this, pAnimInterface);
	}

    ImageAnimationDirection aDirection;
};

struct UILIB_API sRollingImageAnimationFactor : public sImageAnimationFactor
{
    virtual CImageAnimationAlgorithm* CreateAlgorithmObject(DuiLib::CControlUI* pControl) 
    { 
        return new CRollingImageAnimation(this, pControl); 
    }

	virtual CImageAnimationAlgorithm* CreateAlgorithmObject(IAnimObject *pAnimInterface)
	{
		return new CRollingImageAnimation(this, pAnimInterface);
	}

    ImageAnimationDirection aDirection;
};

//struct UILIB_API sWndEffectFactor : public sAnimatorFactor
//{
//	HWND hWndEffect;
//	CDrawingBoard* pBoard;
//
//	int iPointCount;
//	Gdiplus::Point* pBeginPoints;
//	Gdiplus::Point* pEndPoints;
//	int*   pTweenType;
//
//	virtual CAnimator* CreateAnimator(int id, IAnimObject *pAnimInterface, CAnimation* pAnim)
//	{
//		//return new CWndReginAnimator(id, pAnimInterface, pAnim);
//		return NULL;
//	}
//
//	virtual AnimatorType GetType()
//	{
//		return WNDEFFECT_ANIMATOR;
//	}
//};

#endif