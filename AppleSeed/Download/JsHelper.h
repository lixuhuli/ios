#pragma once
#include "GlobalData.h"
#include "WndMain.h"



inline void SetWebGameStatus(__int64 nGameID, int nStatus)
{
    CWndMain* pWnd =  dynamic_cast<CWndMain*>(CGlobalData::Instance()->GetMainWndPtr());
    if (NULL == pWnd)
    {
        return;
    }
    pWnd->SetGameStatus(nGameID, nStatus);
}