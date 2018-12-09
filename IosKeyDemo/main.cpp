// IosKeyDemo.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "WndMain.h"

bool InitDuilibRes(HINSTANCE hInstance);        // 初始化界面库资源
bool InitModules(HINSTANCE hInstance);          // 初始化其他模块
void ExitModules();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    ::CoInitialize(NULL);
    ::OleInitialize(NULL);
    
    InitModules(hInstance);

    CWndMain* pWnd = new CWndMain();
    if (pWnd) {
        pWnd->Create(NULL);
        pWnd->CenterWindow();
        CWndBase::ShowModalWnd(*pWnd);
    }
  
    //pWnd->ShowModal();
    //::ShowWindow(pWnd->GetHWND(), SW_SHOWNORMAL);

    ExitModules();

    ::OleUninitialize();
    ::CoUninitialize();

    return 0;
}

bool InitModules(HINSTANCE hInstance) {
    return true;
}

void ExitModules() {
    return;
}