#ifndef __ANIMATIONMANAGERPROXY_H__
#define __ANIMATIONMANAGERPROXY_H__

class CAnimationManager;
class CAnimator;
class CStoryBoard;
class UILIB_API CAnimationManagerProxy
{
public:
	CAnimationManagerProxy();
	~CAnimationManagerProxy();
	virtual QUI_STRING GetDefaultStoryboardName() = 0;
	void AddAnimation(int id, CAnimator* pAnimator, int iStartTime = 0);
	void StartAnimations();
	void StopAnimations();
	CStoryBoard* AddStoryBoard(QUI_STRING strStoryBoardName, int iFp = 20,bool bTimeSensive = true, bool bThreadTimer = false );
	CStoryBoard* GetStoryBoard(QUI_STRING strStoryBoardName); 
	CStoryBoard* GetDefaultStoryBoard();
	void         RemoveStoryBoard(QUI_STRING strSoryBoardName);
	void         ClearStoryBoardAnimators(QUI_STRING strStoryBoardName);
	void         RemoveAnimatorFromStoryBoard(QUI_STRING strStoryBoardName, int iAnimId);

	void         StartStoryBoard(QUI_STRING strStoryBoardName);
	void         StopStoryBoard(QUI_STRING strStoryBoardName);
    void         StopAllStoryBoards();
	//void         StartAnimator(int iAnimatorID);
protected:
	CAnimationManager* m_pAnimationManager;
};

#endif//end __ANIMATIONMANAGERPROXY_H__