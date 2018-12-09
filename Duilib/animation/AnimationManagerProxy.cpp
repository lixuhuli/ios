#include "StdAfx.h"
#include "AnimationManagerProxy.h"
#include "Animator.h"
#include "AnimationManager.h"


void CAnimationManagerProxy::AddAnimation(int id, CAnimator* pAnimator, int iStartTime/* = 0*/)
{
	CStoryBoard* pStoryBoard = m_pAnimationManager->GetStoryBoard(GetDefaultStoryboardName());
	if(pStoryBoard == NULL)
	{
		pStoryBoard = m_pAnimationManager->AddStoryBoard(GetDefaultStoryboardName());
	}
	if(pStoryBoard)
	{
		pStoryBoard->AddAnimator(id, pAnimator, iStartTime);
	}

}
void CAnimationManagerProxy::StartAnimations()
{
	m_pAnimationManager->StartStoryBoard(GetDefaultStoryboardName());
}

void CAnimationManagerProxy::StopAnimations()
{
	m_pAnimationManager->StopStoryBoard(GetDefaultStoryboardName());
}

CStoryBoard* CAnimationManagerProxy::AddStoryBoard( QUI_STRING strStoryBoardName, int iFp /*= 20*/,bool bTimeSensive /*= true*/, bool bThreadTimer /*= false */ )
{
	return m_pAnimationManager->AddStoryBoard(strStoryBoardName, iFp, bTimeSensive, bThreadTimer);
}

CStoryBoard* CAnimationManagerProxy::GetStoryBoard( QUI_STRING strStoryBoardName )
{
	return m_pAnimationManager->GetStoryBoard(strStoryBoardName);
}

void CAnimationManagerProxy::RemoveStoryBoard(QUI_STRING strSoryBoardName)
{
	m_pAnimationManager->RemoveStoryBoard(strSoryBoardName);
}

void CAnimationManagerProxy::ClearStoryBoardAnimators(QUI_STRING strStoryBoardName)
{
	m_pAnimationManager->ClearAnimatorFromStoryBoard(strStoryBoardName);
}

CAnimationManagerProxy::CAnimationManagerProxy()
{
	m_pAnimationManager = new CAnimationManager;
}

CAnimationManagerProxy::~CAnimationManagerProxy()
{
	if(m_pAnimationManager)
		delete m_pAnimationManager;
}

CStoryBoard* CAnimationManagerProxy::GetDefaultStoryBoard()
{
	return GetStoryBoard(GetDefaultStoryboardName());
}

void CAnimationManagerProxy::StartStoryBoard( QUI_STRING strStoryBoardName )
{
	CStoryBoard* pBoard = GetStoryBoard(strStoryBoardName);
	if(pBoard)
		pBoard->Start();
}

void CAnimationManagerProxy::StopStoryBoard( QUI_STRING strStoryBoardName )
{
	CStoryBoard* pBoard = GetStoryBoard(strStoryBoardName);
	if(pBoard)
		pBoard->Stop();
}

void CAnimationManagerProxy::StopAllStoryBoards()
{
    m_pAnimationManager->StopAllStoryBoards();
}
