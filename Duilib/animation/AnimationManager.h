#ifndef __ANIMATIONMANAGER_H__
#define __ANIMATIONMANAGER_H__

#include "LinearAnimation.h"
#include "AnimInterface.h"
#include "effect.h"
#define INVALID_ANIMATOR_ID -1
#include <list>
#include <vector>
#include <map>

enum AnimatorType
{
    PROPERTYANIMATOR = 0,
    BKIMAGEANIMATOR,
	WNDEFFECT_ANIMATOR
};

#define EventStoryboardStarted _T("EVENT_STORYBOARD_STATED")
#define EventStoryboardProgressed _T("EVENT_STORYBOARD_PROGRESSED")
#define EventStoryboardEnd     _T("EVENT_STORYBOARD_END")

class CStoryBoard;
class CAnimator;
struct sAnimatorFactor;

class CAnimationManager
{
public:
	typedef std::map<QUI_STRING, CStoryBoard *> StoryboardMap;
public:
	CAnimationManager();
	virtual ~CAnimationManager();

    void         ClearAllAnimators();
	CStoryBoard* AddStoryBoard(QUI_STRING strStoryBoardName, int iFp = 20, bool bTimeSensive = true, bool bThreadTimer = false );
	CStoryBoard* GetStoryBoard(QUI_STRING strStoryBoardName);
	void         RemoveStoryBoard(QUI_STRING strStoryBoardName);

	void         RemoveAnimatorFromStoryBoard(QUI_STRING strStoryBoardName, int iAnimId);
	void         ClearAnimatorFromStoryBoard(QUI_STRING strStoryBoardName);

	void         StartStoryBoard(QUI_STRING strStoryBoardName);
	void         StopStoryBoard(QUI_STRING strStoryBoardName);
    void         StopAllStoryBoards();

protected:
	StoryboardMap::iterator  GetStoryBoardEx(QUI_STRING strStoryBoardName);
	StoryboardMap m_mapStoryBoard;
};

class CAnimator;
class UILIB_API CStoryBoard : public IQuiTimerHandler, public IAnimatorListener, public DuiLib::CEventSets
{
public:
	struct sAnimatorInfo
	{
		CAnimator* pAnimator;
		int id;
		int iParentId;
		int iStartTime;
		std::vector<sAnimatorInfo> arrAfterAnimators;
		bool isAllOver;

		sAnimatorInfo()
		{
			pAnimator = NULL;
			id = 0;
			iParentId = -1;
			iStartTime = 0;
			isAllOver = false;
		}
	};
	typedef std::list< sAnimatorInfo > AnimatorInfoList;
	typedef void (* EnumFunc)(sAnimatorInfo* pInfo);
	friend class CAnimationManager;
public:
	CStoryBoard(QUI_STRING strName, int iFp = 20, bool bTimeSensive = true, bool bThreadTimer = false);
	~CStoryBoard();
public:
	void Start();
	void Stop();
	void Pause();
	void Resume();
	bool IsPaused();
	bool IsStarted();
	//void AddAnimator(int id, IAnimObject *pAnimInterface, CAnimation* pAnim, sAnimatorFactor* pFactor, int iStartTime = 0);
	void AddAnimator(int id, CAnimator* pAnimator, int iStartTime);
	void AddAnimator(CAnimator* pAnimator, int iStartTime);
	void RemoveAnimator(int id);
	void ChangeAnimator(int id);
	void ClearAllAnimator();
	void AddAnimatorFactor(int id, sAnimatorFactor* pFactor);
	void ClearAnimatorFactors(int id);
	void SetAnimatorDuration(int id, int nDuration);

	void SetAnimatorAfterAB(int iBeforeID, int iAfterID);
	void SetAnimatorBeginTime(int id, int iMilisecond);

	void OnTimer(CQuiTimer* pTimer);
	void SetListener(IStoryBoardListener* pListener);
protected://IAnimatorListener
	void OnAnimatorStart(CAnimator* pAnimator);
	void OnAnimatorStoped(CAnimator* pAnimator);
	void OnAnimatorProgressed(CAnimator* pAnimator);

private:	
	sAnimatorInfo* FindAnimatorInfo(int id);
	sAnimatorInfo* FindAnimatorInfo(sAnimatorInfo* pRoot, int  id );

	AnimatorInfoList::iterator FindAnimatorInfoInList(int id);
	bool StepAnimator(sAnimatorInfo* pInfo, CQuiTimer* pTimer);
	void EnumAnimatorInfoes(EnumFunc func);
	void _EnumAnimatorInfoes(sAnimatorInfo* pInfo, EnumFunc func);
private:
	CQuiTimer* m_pTimer;
	AnimatorInfoList m_arrAnimatorInfo;
	int m_iFP;
	int m_iTotalAnimatorCount;
	int m_iEndedAnimatorCount;
	IStoryBoardListener* m_pListener;
	bool m_bPaused;
	bool m_bStarted;
};

//class CCtrlEffect : public IAnimObject
//{
//public:
//	CCtrlEffect();
//	~CCtrlEffect();
//
//	void                      AddCtrlEffectAnimator(DuiLib::CControlUI* pCtrl, int id, CAnimation* pAnim, CStoryBoard* pStoryBoard, sAnimatorFactor* pFactor, int iStartTime = 0);
//
//	//attribute
//	DuiLib::CControlUI*       GetOwnerCtrl() const;
//
//	//IAnimInterface
//	virtual DuiLib::QRect GetPos();
//	virtual void  SetPosition(int x, int y);
//	virtual void  SetSize(int iWidth, int iHeight);
//	virtual void  SetKeyWithValue(QUI_STRING strKeyName, int iValue);
//	virtual DuiLib::CPaintManagerUI* GetManager() const;
//	virtual bool FireAnimationProgressedEvent(void* pAnimation);
//	virtual bool FireAnimationStartEvent(void *pAnimation);
//	virtual bool FireAnimationEndEvent(void *pAnimation);
//
//	virtual void SetBkBitmap(HBITMAP hBmp);
//	virtual void SetBkImage(LPCTSTR pStrImage);
//
//private:
//	DuiLib::CControlUI*                   m_pOwnerCtrl;
//};

#endif
