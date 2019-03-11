#include "StdAfx.h"
#include "WndPerOptimiz.h"
#include "Ios/IosMgr.h"
#include "UrlDefine.h"
#include "GlobalData.h"
#include "MsgDefine.h"

CWndPerOptimiz::CWndPerOptimiz()
 : btn_open_vt_(nullptr)
 , btn_set_360_(nullptr)
 , cpu_vt_logo_(nullptr)
 , check_show_per_(nullptr) {
	m_bEscape = true;
}

CWndPerOptimiz::~CWndPerOptimiz() {
}

bool CWndPerOptimiz::OnClickBtnClose(void* lpParam) {
    Close(IDCLOSE);
	return true;
}

bool CWndPerOptimiz::OnClickBtnOpenVT(void* param) {
    if (!btn_open_vt_) return true;

    bool support = false;
    auto IsVTOpened = CIosMgr::Instance()->IsCPUVTOpened(support);

    if (IsVTOpened && btn_open_vt_->GetTag() == 0) {
        UpdateVTBtnStatus(true);
    }
    else if (!IsVTOpened && btn_open_vt_->GetTag() == 1) {
        UpdateVTBtnStatus(false);
    }

    ShellExecute(NULL, L"open", URL_VT_PREFIX, NULL, NULL, SW_SHOW);

    return true;
}

bool CWndPerOptimiz::OnClickBtnSet360(void* param) {
    ShellExecute(NULL, L"open", URL_SET_360, NULL, NULL, SW_SHOW);
    return true;
}

bool CWndPerOptimiz::OnClickCheckShowPer(void* param) {
    if (!check_show_per_) return true;

    bool show = (check_show_per_->IsSelected() == true);
    CGlobalData::Instance()->SetCheckShowPerOptimiz(show);

    ::PostMessage(CGlobalData::Instance()->GetMainWnd(), WM_MAINWND_MSG_SHOW_PEROPTIMIZATION_WARNING, show ? 1 : 0, (LPARAM)0);

    return true;
}

void CWndPerOptimiz::InitWindow() {
	__super::InitWindow();

    if (btn_open_vt_) {
        bool support = false;
        auto IsVTOpened = CIosMgr::Instance()->IsCPUVTOpened(support);

        if (!support) btn_open_vt_->SetVisible(false);
        else {
            if (IsVTOpened) UpdateVTBtnStatus(true);
        }
    }

    if (check_show_per_) {
        check_show_per_->Selected(CGlobalData::Instance()->IsCheckShowPerOptimiz() == false);
    }
}

void CWndPerOptimiz::UpdateVTBtnStatus(bool open) {
    if (open) {
        btn_open_vt_->SetTag(1);
        btn_open_vt_->SetNormalImage(L"file='peroptimiz/btn_open_prefix_nor.png'");
        btn_open_vt_->SetHotImage(L"file='peroptimiz/btn_open_prefix_hot.png'");
        btn_open_vt_->SetPushedImage(L"file='peroptimiz/btn_open_prefix_nor.png'");
        cpu_vt_logo_->SetBkImage(L"file='peroptimiz/cpu_vt_opened.png' file='update/check.png'");
    }
    else {
        btn_open_vt_->SetTag(0);
        btn_open_vt_->SetNormalImage(L"file='peroptimiz/btn_open_vt_nor.png'");
        btn_open_vt_->SetHotImage(L"file='peroptimiz/btn_open_vt_hot.png'");
        btn_open_vt_->SetPushedImage(L"file='peroptimiz/btn_open_vt_nor.png'");
        cpu_vt_logo_->SetBkImage(L"file='peroptimiz/cpu_vt_closed.png'");
    }
}


LRESULT CWndPerOptimiz::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;

    //switch (uMsg) {
    //default: 
    //    bHandled = FALSE;
    //    break;
    //}

    //if (bHandled) {
    //    return lRes;
    //}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

CControlUI* CWndPerOptimiz::CreateControl(LPCTSTR pstrClass) {
    std::wstring control_name = pstrClass;
    //if (control_name == L"CefWebUI") return new CCefWebkitUI();

    return nullptr;
}

LPCWSTR CWndPerOptimiz::GetXmlPath() const {
	return L"WndPerOptimiz.xml";
}
