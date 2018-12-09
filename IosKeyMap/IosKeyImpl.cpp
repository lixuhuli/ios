#include "stdafx.h"
#include "KeyWnd.h"
#include "IosKeyImpl.h"

namespace ioskey {
    IosKeyImpl::IosKeyImpl() 
     : key_wnd_(nullptr) {
    }


    IosKeyImpl::~IosKeyImpl() {
        if (key_wnd_ && IsWindow(*key_wnd_))
            key_wnd_->Close();
        key_wnd_ = nullptr;
    }

    bool IosKeyImpl::CreateWnd(const HWND& parent, const RECT& pos) {
        if (key_wnd_ && IsWindow(*key_wnd_)) return true;

        key_wnd_ = new CKeyWnd;
        if (!key_wnd_) return false;

        key_wnd_->Create(parent);
        key_wnd_->ShowWindow(true);

        if (pos.right - pos.left == 0 || pos.bottom - pos.top == 0) return false;

        MoveWindow(*key_wnd_, pos.left, pos.top, pos.right - pos.left, pos.bottom - pos.top, TRUE);

        return true;
    }

    void IosKeyImpl::SetPos(const RECT& pos) {
        if (!key_wnd_ || !IsWindow(*key_wnd_)) return;
        MoveWindow(*key_wnd_, pos.left, pos.top, pos.right - pos.left, pos.bottom - pos.top, TRUE);
    }

    void IosKeyImpl::Show(const bool& show) {
        if (key_wnd_) key_wnd_->ShowWindow(show);
    }

    bool IosKeyImpl::IsVisible() {
        if (!key_wnd_ || !IsWindow(*key_wnd_)) return false;
        return IsWindowVisible(*key_wnd_) == TRUE;
    }

}