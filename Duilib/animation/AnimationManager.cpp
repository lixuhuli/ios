#include "stdafx.h"
#include "AnimationManager.h"
#include "Animator.h"
#include "PropertyAnimatior.h"
#include "AnimationFactor.h"
#include "ImageAnimator.h"
#include "EffectAnimator.h"

using namespace DuiLib;
//////////////////////////////////////////////////////////////////////////
//CommonMgr
CAnimationManager::CAnimationManager()
{

}

CAnimationManager::~CAnimationManager()
{
	ClearAllAnimators();
}

void CAnimationManager::ClearAllAnimators()
{
    for (std::map<QUI_STRING, CStoryBoard *>::iterator iStoryBoard = m_mapStoryBoard.begin(); iStoryBoard != m_mapStoryBoard.end(); ++iStoryBoard)
    {
        if (iStoryBoard->second != NULL)
        {
            //iStoryBoard->second->ClearAllAnimator();
            delete iStoryBoard->second;
            iStoryBoard->second = NULL;
        }
    }

    m_mapStoryBoard.clear();
}

CStoryBoard* CAnimationManager::AddStoryBoard(QUI_STRING strStoryBoardName, int iFp /* = 20 */,bool bTimeSensive /* = true */, bool bThreadTimer /* = false */ )
{
	if(m_mapStoryBoard.find(strStoryBoardName) != m_mapStoryBoard.end())
	{
		return m_mapStoryBoard[strStoryBoardName];
	}
	CStoryBoard* pStoryBoard = new CStoryBoard(strStoryBoardName, iFp, bTimeSensive, bThreadTimer);
	m_mapStoryBoard[strStoryBoardName] = pStoryBoard;
	return pStoryBoard;
}

CStoryBoard* CAnimationManager::GetStoryBoard(QUI_STRING strStoryBoardName)
{
//	return m_mapStoryBoard[strStoryBoardName];
	std::map<QUI_STRING, CStoryBoard *>::iterator it = m_mapStoryBoard.find(strStoryBoardName);
	if (it != m_mapStoryBoard.end())
	{
		return it->second;
	}

	return NULL;
}

void CAnimationManager::RemoveStoryBoard(QUI_STRING strStoryBoardName)
{
	CStoryBoard *pStoryBoard = GetStoryBoard(strStoryBoardName);
	if (pStoryBoard != NULL)
	{
		pStoryBoard->ClearAllAnimator();
		delete pStoryBoard;
	}

	std::map<QUI_STRING, CStoryBoard *>::iterator it = GetStoryBoardEx(strStoryBoardName);
	if (it != m_mapStoryBoard.end())
	{
		m_mapStoryBoard.erase(it);
	}
}

void CAnimationManager::StartStoryBoard(QUI_STRING strStoryBoardName)
{
	CStoryBoard *pStoryBoard = GetStoryBoard(strStoryBoardName);
	if (pStoryBoard != NULL)
	{
		pStoryBoard->Stop();
		pStoryBoard->Start();
	}
}

void CAnimationManager::StopStoryBoard(QUI_STRING strStoryBoardName)
{
	CStoryBoard *pStoryBoard = GetStoryBoard(strStoryBoardName);
	if (pStoryBoard != NULL)
	{
		pStoryBoard->Stop();
	}
}

void CAnimationManager::StopAllStoryBoards()
{
    for (StoryboardMap::iterator iBoard = m_mapStoryBoard.begin();
        iBoard != m_mapStoryBoard.end(); ++iBoard)
    {
        CStoryBoard* pBoard = iBoard->second;
        if (pBoard == NULL)
            continue;
        pBoard->Stop();
    }
}

void CAnimationManager::RemoveAnimatorFromStoryBoard(QUI_STRING strStoryBoardName, int iAnimId)
{
	CStoryBoard *pStoryBoard = GetStoryBoard(strStoryBoardName);
	if (pStoryBoard != NULL)
	{
		pStoryBoard->RemoveAnimator(iAnimId);
	}
}

void CAnimationManager::ClearAnimatorFromStoryBoard(QUI_STRING strStoryBoardName)
{
	CStoryBoard *pStoryBoard = GetStoryBoard(strStoryBoardName);
	if (pStoryBoard != NULL)
	{
		pStoryBoard->ClearAllAnimator();
	}
}

std::map<QUI_STRING, CStoryBoard *>::iterator CAnimationManager::GetStoryBoardEx(QUI_STRING strStoryBoardName)
{
	std::map<QUI_STRING, CStoryBoard *>::iterator it = m_mapStoryBoard.find(strStoryBoardName);
	if (it != m_mapStoryBoard.end())
	{
		return it;
	}

	return m_mapStoryBoard.end();
}


//////////////////////////////////////////////////////////////////////////
//

CStoryBoard::CStoryBoard( QUI_STRING strName, int iFp/* = 20*/,  bool bTimeSensive/* = true*/, bool bThreadTimer /*= false*/ )
{
	m_pTimer = new CQuiTimer(QUITIMER_REPEAT, this, bTimeSensive, bThreadTimer);
	m_iFP = iFp;
	m_iTotalAnimatorCount = 0;
//	m_pControl = pControl;
	m_iEndedAnimatorCount = 0;
	m_pListener = NULL;
	m_bPaused = false;
	m_bStarted = false;
}

CStoryBoard::~CStoryBoard()
{
	ClearAllAnimator();
}

void CStoryBoard::AddAnimatorFactor( int id, sAnimatorFactor* pFactor )
{
	sAnimatorInfo* pInfo = FindAnimatorInfo(id);
	if(pInfo != NULL)
		pInfo->pAnimator->AddFactor(pFactor);
}

void CStoryBoard::ClearAnimatorFactors( int id )
{
	sAnimatorInfo* pInfo = FindAnimatorInfo(id);
	if(pInfo != NULL)
		pInfo->pAnimator->ClearFactors();
}

void CStoryBoard::SetAnimatorDuration( int id, int nDuration )
{
	sAnimatorInfo* pInfo = FindAnimatorInfo(id);
	if(pInfo != NULL)
		pInfo->pAnimator->SetDuration(nDuration);
}

void CStoryBoard::SetAnimatorAfterAB( int iBeforeID, int iAfterID )
{
	AnimatorInfoList::iterator it = FindAnimatorInfoInList(iAfterID);

	sAnimatorInfo* pInfo = FindAnimatorInfo(iBeforeID);
	if(pInfo != NULL && it != m_arrAnimatorInfo.end())
	{
		it->iParentId = pInfo->id;
		pInfo->arrAfterAnimators.push_back(*it);
		it->pAnimator->SetParent(pInfo->pAnimator);
		m_arrAnimatorInfo.erase(it);
	}
}

void CStoryBoard::SetAnimatorBeginTime( int id, int iMilisecond )
{
	sAnimatorInfo* pInfo = FindAnimatorInfo(id);
	if(pInfo != NULL)
	{
		pInfo->iStartTime = iMilisecond;
		pInfo->pAnimator->SetStartTime(iMilisecond);
	}
}

void CStoryBoard::OnTimer( CQuiTimer* pTimer )
{
	//OutputDebugString(_T("\nOnTimer begin\n"));
	int iElapsedTime = pTimer->GetElapsedTime();
	AnimatorInfoList::iterator it = m_arrAnimatorInfo.begin();
	while(it != m_arrAnimatorInfo.end())
	{
		if(it->iStartTime < iElapsedTime)
		{
			if(!it->isAllOver)
			{
				TNotifyUI notify;
				notify.pSender = NULL;
				notify.sType = EventStoryboardProgressed;
				notify.wParam = (WPARAM)this;
				bool bRet = FireEvent(EventStoryboardProgressed, &notify);
				StepAnimator(&(*it), pTimer);
			}
		}
		it++;
	}
	//OutputDebugString(_T("\nOnTimer end\n"));
}

void StartFunc(CStoryBoard::sAnimatorInfo* pInfo)
{
	pInfo->pAnimator->Reset();
	pInfo->isAllOver = false;
}

void CStoryBoard::Start()
{
	Stop();
    m_bStarted = true;
	EnumAnimatorInfoes(StartFunc);
	m_iEndedAnimatorCount = 0;
	if(m_iTotalAnimatorCount > 0)
		m_pTimer->Start(m_iFP);
}

//void CStoryBoard::AddAnimator( int id, IAnimObject *pAnimInterface, CAnimation* pAnim, sAnimatorFactor* pFactor, int iStartTime /*= 0*/ )
//{
//	CAnimator* pAnimator = NULL;
//	int nID = id;
//	if(FindAnimatorInfo(id))
//		return;
//
//	pAnimator = pFactor->CreateAnimator(id, pAnimInterface, pAnim);
//	if (pAnimator == NULL)
//		return;
//	pAnim->SetUseOutTimer(true);
//	pAnim->SetListener(pAnimator);
//
//	pAnimator->AddFactor(pFactor);
//	pAnimator->SetAnimatorListener(this);
//	pAnimator->SetInStoryBoard();
//	pAnimator->SetDuration(pAnim->GetDuration());
//	pAnimator->SetStartTime(iStartTime);
//	sAnimatorInfo info;
//	info.pAnimator = pAnimator;
//	info.id = nID;
//	info.iStartTime = iStartTime;
//	m_iTotalAnimatorCount++;
//	m_arrAnimatorInfo.push_back(info);
//}

void CStoryBoard::AddAnimator( int id, CAnimator* pAnimator, int iStartTime )
{
	int nID = id;
	if(FindAnimatorInfo(id))
		return;
	if (pAnimator == NULL)
		return;
	pAnimator->SetAnimatorListener(this);
	pAnimator->SetInStoryBoard();
	pAnimator->SetStartTime(iStartTime);
	pAnimator->SetID(nID);
	sAnimatorInfo info;
	info.pAnimator = pAnimator;
	info.id = nID;
	info.iStartTime = iStartTime;
	m_iTotalAnimatorCount++;
	m_arrAnimatorInfo.push_back(info);
}

void CStoryBoard::AddAnimator(CAnimator* pAnimator, int iStartTime)
{
	//自动生成ID
	int nID = m_iTotalAnimatorCount+1;
	for ( ; FindAnimatorInfo(nID) != NULL; ++nID);
	
	if (pAnimator == NULL)
		return;
	pAnimator->SetAnimatorListener(this);
	pAnimator->SetInStoryBoard();
	pAnimator->SetStartTime(iStartTime);
	pAnimator->SetID(nID);
	sAnimatorInfo info;
	info.pAnimator = pAnimator;
	info.id = nID;
	info.iStartTime = iStartTime;
	m_iTotalAnimatorCount++;
	m_arrAnimatorInfo.push_back(info);
}

void RemoveFunc(CStoryBoard::sAnimatorInfo* pInfo)
{
	pInfo->pAnimator->ClearFactors();
	delete pInfo->pAnimator;
	pInfo->pAnimator = NULL;
}

void CStoryBoard::RemoveAnimator(int id)
{
	//Stop();
	sAnimatorInfo *pInfo = FindAnimatorInfo(id);
	if (pInfo != NULL)
	{
		_EnumAnimatorInfoes(pInfo, &RemoveFunc);
	
		//delete top item
		if (pInfo->iParentId == -1)
		{
			AnimatorInfoList::iterator iter = FindAnimatorInfoInList(pInfo->id);
			if (iter != m_arrAnimatorInfo.end())
			{
				m_arrAnimatorInfo.erase(iter);
				m_iTotalAnimatorCount--;
			}
		}
		else //delete not top item
		{
			sAnimatorInfo *pParentInfo = FindAnimatorInfo(pInfo->iParentId);
			if (pParentInfo != NULL)
			{
				pParentInfo->arrAfterAnimators.clear();
				m_iTotalAnimatorCount--;
			}
		}
	}
}

void CStoryBoard::ClearAllAnimator()
{
	Stop();
	EnumAnimatorInfoes(&RemoveFunc);
	m_arrAnimatorInfo.clear();
	m_iTotalAnimatorCount = 0;
}

CStoryBoard::sAnimatorInfo* CStoryBoard::FindAnimatorInfo( int id )
{
	AnimatorInfoList::iterator it = m_arrAnimatorInfo.begin();
	while (it != m_arrAnimatorInfo.end())
	{
		CStoryBoard::sAnimatorInfo* pInfo = FindAnimatorInfo(&(*it), id);
		if(pInfo != NULL)
			return pInfo;
		it++;
	}
	return NULL;
}

CStoryBoard::sAnimatorInfo* CStoryBoard::FindAnimatorInfo( sAnimatorInfo* pRoot, int id)
{
	if(pRoot->id == id)
		return pRoot;
	std::vector<sAnimatorInfo>::iterator it = pRoot->arrAfterAnimators.begin();
	while(it != pRoot->arrAfterAnimators.end())
	{
		CStoryBoard::sAnimatorInfo* pInfo = FindAnimatorInfo(&(*it), id);
		if(pInfo != NULL)
			return pInfo;
		it++;
	}
	return NULL;
}

bool CStoryBoard::StepAnimator(sAnimatorInfo* pInfo, CQuiTimer* pTimer)//true:所有animator over，false：还有子animation需要运行
{
	bool bAllOver = true;
	if(pInfo->pAnimator->IsOver() && !pInfo->isAllOver)
	{
		std::vector<sAnimatorInfo>::iterator it = pInfo->arrAfterAnimators.begin();
		while(it != pInfo->arrAfterAnimators.end())
		{
			if(it->pAnimator->IsOver() )
			{
				bAllOver &= StepAnimator(&(*it), pTimer);
			}
			else
			{
				it->pAnimator->StepAnimator(pTimer, pInfo->pAnimator);
				bAllOver &= false;
			}
			it ++;
		}
	}
	else if(!pInfo->pAnimator->IsOver())
	{
		pInfo->pAnimator->StepAnimator(pTimer, NULL);
		bAllOver = false;
	}
	pInfo->isAllOver = bAllOver;
	return bAllOver;
}

CStoryBoard::AnimatorInfoList::iterator CStoryBoard::FindAnimatorInfoInList( int id )
{
	AnimatorInfoList::iterator it = m_arrAnimatorInfo.begin();
	while (it != m_arrAnimatorInfo.end())
	{
		if(it->id == id)
			return it;
		it++;
	}
	return m_arrAnimatorInfo.end();
}

void CStoryBoard::EnumAnimatorInfoes( EnumFunc func )
{
	AnimatorInfoList::iterator it = m_arrAnimatorInfo.begin();
	while(it != m_arrAnimatorInfo.end())
	{
		_EnumAnimatorInfoes(&(*it), func);
		it++;
	}
}

void CStoryBoard::_EnumAnimatorInfoes( sAnimatorInfo* pInfo, EnumFunc func)
{	
	std::vector<sAnimatorInfo>::iterator it = pInfo->arrAfterAnimators.begin();
	while(it != pInfo->arrAfterAnimators.end())
	{
		_EnumAnimatorInfoes(&(*it), func);
		it++;
	}

	if(func)
	{
		func(pInfo);
	}
}

void StopFunc(CStoryBoard::sAnimatorInfo* pInfo)
{
	pInfo->pAnimator->Stop();
	pInfo->isAllOver = true;
}

void CStoryBoard::Stop()
{
    if (!IsStarted())
        return;
	m_bStarted = false;
	m_bPaused = false;
	m_pTimer->Stop();
	if(m_pListener)
		m_pListener->OnStoryBoardStoped(this);
	TNotifyUI notify;
	notify.pSender = NULL;
	notify.sType = EventStoryboardEnd;
	notify.wParam = (WPARAM)this;
	bool bRet = FireEvent(EventStoryboardEnd, &notify);
	EnumAnimatorInfoes(&StopFunc);
}

void CStoryBoard::OnAnimatorStart( CAnimator* pAnimator )
{
	if (m_iEndedAnimatorCount == 0)
	{
		if (m_pListener != NULL)
		{
			m_pListener->OnStoryBoardStart(this);
		}
		TNotifyUI notify;
		notify.pSender = NULL;
		notify.sType = EventStoryboardStarted;
		notify.wParam = (WPARAM)this;
		bool bRet = FireEvent(EventStoryboardStarted, &notify);
	}
}

void CStoryBoard::OnAnimatorStoped( CAnimator* pAnimator )
{
	m_iEndedAnimatorCount++;
	if(m_iEndedAnimatorCount == m_iTotalAnimatorCount)
	{
		Stop();
	}
}

void CStoryBoard::SetListener( IStoryBoardListener* pListener )
{
	m_pListener = pListener;
}

void CStoryBoard::Pause()
{
	m_bPaused = true;
	m_pTimer->Pause();
}

void CStoryBoard::Resume()
{
	m_bPaused = false;
	m_pTimer->Resume();
}

bool CStoryBoard::IsPaused()
{
	return m_bPaused && m_bStarted;
}

bool CStoryBoard::IsStarted()
{
	return m_bStarted;
}


////////////////////////////////////////////////////////////////////////////
////
//CCtrlEffect::CCtrlEffect() : m_pOwnerCtrl(NULL)
//{
//
//}
//
//CCtrlEffect::~CCtrlEffect()
//{
//
//}
//
//void CCtrlEffect::AddCtrlEffectAnimator(DuiLib::CControlUI* pCtrl, int id, CAnimation* pAnim, CStoryBoard* pStoryBoard, sAnimatorFactor* pFactor, int iStartTime /* = 0 */)
//{
//	if (m_pOwnerCtrl == pCtrl)
//	{
//		pStoryBoard->AddAnimator(id, (IAnimObject *)this, pAnim, pFactor, iStartTime);
////		pStoryBoard->AddAnimatorFactor(id, pFactor);
//	}
//	else
//	{
//		m_pOwnerCtrl = pCtrl;
//		pStoryBoard->AddAnimator(id, (IAnimObject *)this, pAnim, pFactor, iStartTime);
////		pStoryBoard->AddAnimatorFactor(id, pFactor);
//	}
//}
//
//DuiLib::CControlUI* CCtrlEffect::GetOwnerCtrl() const
//{
//	return m_pOwnerCtrl;
//}
//
//
////IANIMINTERFACE
//DuiLib::QRect CCtrlEffect::GetPos()
//{
//	return m_pOwnerCtrl->GetPos();
//}
//
//DuiLib::CPaintManagerUI* CCtrlEffect::GetManager() const
//{
//	if (m_pOwnerCtrl != NULL)
//	{
//		return m_pOwnerCtrl->GetManager();
//	}
//
//	return NULL;
//}
//
//bool CCtrlEffect::FireAnimationProgressedEvent(void* pAnimation)
//{
//	if (m_pOwnerCtrl != NULL)
//	{
//		m_pOwnerCtrl->FireAnimationProgressedEvent(pAnimation);
//	}
//	return true;
//}
//
//bool CCtrlEffect::FireAnimationStartEvent(void *pAnimation)
//{
//	if (m_pOwnerCtrl != NULL)
//	{
//		m_pOwnerCtrl->FireAnimationStartEvent(pAnimation);
//	}
//	return true;
//}
//
//bool CCtrlEffect::FireAnimationEndEvent(void *pAnimation)
//{
//	if (m_pOwnerCtrl != NULL)
//	{
//		m_pOwnerCtrl->FireAnimationEndEvent(pAnimation);
//	}
//	return true;
//}
//
//void CCtrlEffect::SetBkBitmap(HBITMAP hBmp)
//{
//	if (m_pOwnerCtrl != NULL)
//	{
//		m_pOwnerCtrl->SetBKBitmap(hBmp);
//	}
//}
//
//void CCtrlEffect::SetBkImage(LPCTSTR pStrImage)
//{
//	if (m_pOwnerCtrl != NULL)
//	{
//		m_pOwnerCtrl->SetBkImage(pStrImage);
//	}
//}
//
//void CCtrlEffect::SetPosition( int x, int y )
//{
//
//}
//
//void CCtrlEffect::SetSize( int iWidth, int iHeight )
//{
//
//}
//
//void CCtrlEffect::SetKeyWithValue( QUI_STRING strKeyName, int iValue )
//{
//
//}
