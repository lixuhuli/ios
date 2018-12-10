#include "stdafx.h"
#include "KeyEditUI.h"
#include "GlobalData.h"
#pragma comment(lib, "imm32.lib")

namespace DuiLib {
    CKeyEditWnd::CKeyEditWnd()
        : m_pOwner(NULL)
        , m_hBkBrush(NULL)
        , m_bInit(false)
        , m_hParentWnd(nullptr) {
    }

    CKeyEditWnd::~CKeyEditWnd() {
        // 理论上 是不需要释放 为了安全还是加上
        if (m_pMemBkImage) {
            if (!m_pMemBkImage->IsNull()) m_pMemBkImage->Destroy();
            delete m_pMemBkImage;
            m_pMemBkImage = NULL;
        }
    }

    HWND CKeyEditWnd::Init(HWND hParentWnd, CKeyEditUI* pOwner)
    {
        m_hParentWnd = hParentWnd;
        m_pMemBkImage = NULL;
        m_pOwner = pOwner;
        RECT rcPos = CalPos();
        //UINT uStyle = WS_CHILD | ES_AUTOHSCROLL;
        UINT uStyle = WS_POPUP | ES_AUTOHSCROLL | WS_VISIBLE;
        RECT rcWnd = { 0 };
        ::GetWindowRect(hParentWnd, &rcWnd);
        rcPos.left += rcWnd.left;
        rcPos.right += rcWnd.left;
        rcPos.top += rcWnd.top;
        rcPos.bottom += rcWnd.top;

        if (m_pOwner->IsPasswordMode()) uStyle |= ES_PASSWORD;
        m_dwStyle = uStyle;
        CWindowWnd::Create(m_hParentWnd, NULL, uStyle, 0, rcPos);
        HFONT hFont = NULL;
        int iFontIndex = m_pOwner->GetFont();
        if (iFontIndex != -1)
            hFont = m_pOwner->GetManager()->GetFont(iFontIndex);
        if (hFont == NULL)
            hFont = m_pOwner->GetManager()->GetDefaultFontInfo()->hFont;
        SetWindowFont(m_hWnd, hFont, TRUE);
        Edit_LimitText(m_hWnd, m_pOwner->GetMaxChar());
        if (m_pOwner->IsPasswordMode()) Edit_SetPasswordChar(m_hWnd, m_pOwner->GetPasswordChar());
        Edit_SetText(m_hWnd, m_pOwner->GetText());
        Edit_SetModify(m_hWnd, FALSE);
        SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
        Edit_Enable(m_hWnd, m_pOwner->IsEnabled() == true);
        Edit_SetReadOnly(m_hWnd, m_pOwner->IsReadOnly() == true);
        //Styls
        LONG styleValue = ::GetWindowLong(m_hWnd, GWL_STYLE);
        styleValue |= pOwner->GetWindowStyls();
        ::SetWindowLong(GetHWND(), GWL_STYLE, styleValue);
        ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
        ::SetFocus(m_hWnd);
        m_bInit = true;
        return m_hWnd;
    }

    LPCWSTR CKeyEditWnd::GetXmlPath() const {
        return L"KeyMap/WndEdit.xml";
    }

    LPCTSTR CKeyEditWnd::GetWindowClassName() const {
        return L"KeyEditWnd";
    }

    void CKeyEditWnd::InitWindow() {
        __super::InitWindow();
    }

    CControlUI* CKeyEditWnd::CreateControl(LPCTSTR pstrClass) {
        if (!pstrClass) return nullptr;

        std::wstring control_name = pstrClass;

        return nullptr;
    }

    LRESULT CKeyEditWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
        auto lr = __super::OnCreate(uMsg, wParam, lParam, bHandled);
        //if (lr != -1) m_pm.SetLayeredWithPicture(true);
        m_pm.AddPreMessageFilter(this);
        return lr;
    }

    LRESULT CKeyEditWnd::OnSetCursor(WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
        return 0;
    }

    RECT CKeyEditWnd::CalPos()
    {
        CDuiRect rcPos = m_pOwner->GetPos();
        wstring text = m_pOwner->GetText().GetData();
        if (text.empty()) {
            rcPos.left = (rcPos.left + rcPos.right) / 2;
            rcPos.right = rcPos.left + 1;
        }
        else {
            SIZE sz = GetPaintTextSize(m_pOwner->GetManager(), text.c_str(), 1);
            rcPos.left = (rcPos.left + rcPos.right + sz.cx) / 2;
            rcPos.right = rcPos.left + 1;
        }

        LONG lEditHeight = m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->tm.tmHeight;
        if (lEditHeight < rcPos.GetHeight()) {
            rcPos.top += (rcPos.GetHeight() - lEditHeight) / 2;
            rcPos.bottom = rcPos.top + lEditHeight;
        }
        return rcPos;
    }

    LPCTSTR CKeyEditWnd::GetSuperClassName() const
    {
        return WC_EDIT;
    }

    void CKeyEditWnd::OnFinalMessage(HWND /*hWnd*/)
    {
        m_pOwner->Invalidate();
        // Clear reference and die
        if (m_hBkBrush != NULL)
            ::DeleteObject(m_hBkBrush);
        m_pOwner->m_pWindow = NULL;
        delete this;
    }

    LRESULT CKeyEditWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) {
        LRESULT lRes = 0;

        switch (uMsg) {
        case WM_KEYDOWN: {
            if (wParam == VK_PROCESSKEY) {
                wParam = ImmGetVirtualKey(m_hWnd);
            }

            auto keyboard = CGlobalData::Instance()->GetKeyboardStr(wParam);
            m_pOwner->SetText(keyboard.c_str());

            bHandled = true;
            break;
        }
        case WM_SYSKEYDOWN: {
            auto keyboard = CGlobalData::Instance()->GetKeyboardStr(wParam);
            m_pOwner->SetText(keyboard.c_str());

            bHandled = true;
            break;
        }
        default:
            break;
        }

        return lRes;
    }

    LRESULT CKeyEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT lRes = 0;
        BOOL bHandled = TRUE;
        switch (uMsg)
        {
        case WM_CHAR:
            if (wParam == VK_TAB || wParam == VK_RETURN)
                return 0;
            bHandled = FALSE;
            break;
        case WM_KILLFOCUS:
            lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
            break;
        case WM_SETFOCUS:
            lRes = OnSetFocus(uMsg, wParam, lParam, bHandled);
            break;
        case OCM_COMMAND://OCM_COMMAND
        {
            if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE)
                lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
            else if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE)
            {
                RECT rcClient;
                ::GetClientRect(m_hWnd, &rcClient);
                ::InvalidateRect(m_hWnd, &rcClient, FALSE);
            }
            break;
        }
        case WM_RBUTTONUP:
        {
            if (!m_pOwner->m_bShowDefMenu)
            {
                m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_EDIT_RBTNUP);
            }
            else
                bHandled = FALSE;
            break;
        }
        case WM_KEYDOWN: {
            if (wParam == VK_PROCESSKEY) {
                wParam = ImmGetVirtualKey(m_hWnd);
            }

            auto keyboard = CGlobalData::Instance()->GetKeyboardStr(wParam);
            m_pOwner->SetText(keyboard.c_str());

            bHandled = true;
            break;
        }
        case WM_SYSKEYDOWN: {
            auto keyboard = CGlobalData::Instance()->GetKeyboardStr(wParam);
            m_pOwner->SetText(keyboard.c_str());

            bHandled = true;
            break;
        }
        //case WM_KEYDOWN:
        //{
        //    switch (wParam)
        //    {
        //    case VK_RETURN:
        //        m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_RETURN);
        //        break;
        //    case VK_TAB:
        //        m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TABCHAR);
        //        break;
        //    case VK_DELETE:
        //        ::SendMessage(m_hWnd, WM_CLEAR, 0, 0L);
        //        break;
        //    default:
        //        m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_CHAR);
        //        bHandled = FALSE;
        //    }
        //    break;
        //}
        case OCM__BASE + WM_CTLCOLOREDIT:
        case OCM__BASE + WM_CTLCOLORSTATIC:
        {
            if (m_pOwner->GetNativeEditBkColor() == 0)
            {
                //如果浏览器背景是黑色 代表是透明
                CDuiImage* pImg = m_pOwner->GetEditBkImage();
                if (!pImg->IsNull())
                {
                    if (m_pMemBkImage == NULL)
                        m_pMemBkImage = new CDuiImage();
                    else if (m_pMemBkImage && !m_pMemBkImage->IsNull())
                        m_pMemBkImage->Destroy();
                    m_pOwner->CopyImage(m_pMemBkImage, pImg);
                    HBITMAP bitmap = m_pMemBkImage->Detach();
                    HBRUSH  BkBrush = CreatePatternBrush(bitmap);
                    ::SelectObject((HDC)wParam, BkBrush);
                    ::SetTextColor((HDC)wParam, RGB(GetBValue(m_pOwner->GetTextColor()), GetGValue(m_pOwner->GetTextColor()), GetRValue(m_pOwner->GetTextColor())));
                    ::SetBkMode((HDC)wParam, TRANSPARENT);
                    return (LRESULT)BkBrush; //临时位图对象不用删除返回画刷后 系统会自动释放位图对象
                }
            }
            else
            {
                HBRUSH  BkBrush = NULL;
                ::SetBkMode((HDC)wParam, TRANSPARENT);
                ::SetTextColor((HDC)wParam, RGB(GetBValue(m_pOwner->GetTextColor()), GetGValue(m_pOwner->GetTextColor()), GetRValue(m_pOwner->GetTextColor())));
                if (BkBrush == NULL)
                {
                    DWORD clrColor = m_pOwner->GetNativeEditBkColor();
                    BkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                }
                return (LRESULT)BkBrush;
            }
            break;
        }
        case WM_SETCURSOR: lRes = OnSetCursor(wParam, lParam, bHandled);
        default:
            bHandled = FALSE;
        }
        if (!bHandled) return __super::HandleMessage(uMsg, wParam, lParam);
        return lRes;
    }

    LRESULT CKeyEditWnd::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (m_pOwner)
        {
            CPaintManagerUI* pManager = m_pOwner->GetManager();
            if (pManager)
                pManager->SendNotify(m_pOwner, DUI_MSGTYPE_WND_SETFOCUS, 0, 0, true);
        }
        bHandled = FALSE;
        return 0;
    }

    LRESULT CKeyEditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        if (m_pOwner->IsRbtnupMenu())
            return lRes;

        POINT pt = { 0 };
        ::GetCursorPos(&pt);
        ::ScreenToClient(m_hParentWnd, &pt);

        QRect rc = m_pOwner->GetPos();
        if (::PtInRect(&rc, pt)) {
            SetFocus(m_hWnd);
            return lRes;
        }

        PostMessage(WM_CLOSE);
        return lRes;
    }

    LRESULT CKeyEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if (!m_bInit) return 0;
        if (m_pOwner == NULL) return 0;

        // Copy text back
        int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
        LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
        ASSERT(pstr);
        if (pstr == NULL) return 0;
        ::GetWindowText(m_hWnd, pstr, cchLen);
        m_pOwner->m_sText = pstr;
        m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
        return 0;
    }


    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CKeyEditUI::CKeyEditUI()
        : m_pWindow(NULL)
        , m_uMaxChar(255)
        , m_bReadOnly(false)
        , m_bPasswordMode(false)
        , m_cPasswordChar(_T('*'))
        , m_uButtonState(0)
        , m_dwEditbkColor(0xFFFFFFFF)
        , m_iWindowStyls(0)
        , m_nCaretWidth(1)
        , m_dwCaretColor(0)
        , m_bShowDefMenu(true)
        , m_bRbtnupMenu(false)
        , m_pEditBkImage(new CDuiImage())
    {
        SetTextPadding(CDuiRect(4, 3, 4, 3));
        SetDefText(_T(""));
    }

    CKeyEditUI::~CKeyEditUI()
    {
        if (m_pEditBkImage)
        {
            if (!m_pEditBkImage->IsNull())
                m_pEditBkImage->Destroy();
            delete m_pEditBkImage;
            m_pEditBkImage = NULL;
        }
    }

    void CKeyEditUI::ReleaseEditImage()
    {//释放图片 
        if (m_pEditBkImage)
        {
            if (!m_pEditBkImage->IsNull())
                m_pEditBkImage->Destroy();
        }
    }

    LPCTSTR CKeyEditUI::GetClass() const
    {
        return _T("KeyEditUI");
    }

    LPVOID CKeyEditUI::GetInterface(LPCTSTR pstrName)
    {
        if (_tcscmp(pstrName, UI_KEY_EDIT) == 0) return static_cast<CKeyEditUI*>(this);
        return CLabelUI::GetInterface(pstrName);
    }

    UINT CKeyEditUI::GetControlFlags() const
    {
        if (!IsEnabled()) return CControlUI::GetControlFlags();

        return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
    }

    void CKeyEditUI::DoEvent(TEventUI& event)
    {
        if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
            if (m_pParent != NULL) m_pParent->DoEvent(event);
            else CLabelUI::DoEvent(event);
            return;
        }

        if (event.Type == UIEVENT_SETCURSOR && IsEnabled())
        {
            ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
            return;
        }
        if (event.Type == UIEVENT_WINDOWSIZE)
        {
            if (m_pWindow != NULL) m_pManager->SetFocusNeeded(this);
        }
        if (event.Type == UIEVENT_SCROLLWHEEL)
        {
            if (m_pWindow != NULL) return;
        }
        if (event.Type == UIEVENT_SETFOCUS && IsEnabled())
        {
            if (m_pWindow) return;
            m_pWindow = new CKeyEditWnd();
            ASSERT(m_pWindow);
            m_pWindow->Init(m_pManager->GetPaintWindow(), this);
            Invalidate();
        }
        if (event.Type == UIEVENT_KILLFOCUS && IsEnabled())
        {
            Invalidate();
        }
        if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN)
        {
            if (IsEnabled())
            {
                m_pManager->SendNotify(this, DUI_MSGTYPE_SETFOCUS, 0, 0, true);
                GetManager()->ReleaseCapture();
                if (IsFocused() && m_pWindow == NULL)
                {
                    m_pWindow = new CKeyEditWnd();
                    ASSERT(m_pWindow);
                    m_pWindow->Init(m_pManager->GetPaintWindow(), this);
                    if (PtInRect(&m_rcItem, event.ptMouse))
                    {
                        int nSize = GetWindowTextLength(*m_pWindow);
                        if (nSize == 0)
                            nSize = 1;

                        Edit_SetSel(*m_pWindow, 0, 0);
                    }
                }
                else if (m_pWindow != NULL)
                {
#if 1
                    int nSize = GetWindowTextLength(*m_pWindow);
                    if (nSize == 0)
                        nSize = 1;

                    Edit_SetSel(*m_pWindow, 0, 0);
#else
                    POINT pt = event.ptMouse;
                    pt.x -= m_rcItem.left + m_rcTextPadding.left;
                    pt.y -= m_rcItem.top + m_rcTextPadding.top;
                    ::SendMessage(*m_pWindow, WM_LBUTTONDOWN, event.wParam, MAKELPARAM(pt.x, pt.y));
#endif
                }
            }
            return;
        }
        if (event.Type == UIEVENT_MOUSEMOVE)
        {
            return;
        }
        if (event.Type == UIEVENT_BUTTONUP)
        {
            return;
        }
        if (event.Type == UIEVENT_CONTEXTMENU)
        {
            return;
        }
        if (event.Type == UIEVENT_MOUSEENTER)
        {
            if (IsEnabled())
            {
                m_pManager->SendNotify(this, DUI_MSGTYPE_MOUSEENTER);
                m_uButtonState |= UISTATE_HOT;
                Invalidate();
            }
            return;
        }
        if (event.Type == UIEVENT_MOUSELEAVE)
        {
            if (IsEnabled())
            {
                m_pManager->SendNotify(this, DUI_MSGTYPE_MOUSELEAVE);
                m_uButtonState &= ~UISTATE_HOT;
                Invalidate();
            }
            return;
        }
        CLabelUI::DoEvent(event);
    }

    void CKeyEditUI::SetEnabled(bool bEnable)
    {
        CControlUI::SetEnabled(bEnable);
        if (!IsEnabled()) {
            m_uButtonState = 0;
        }
    }

    void CKeyEditUI::SetText(LPCTSTR pstrText)
    {
        auto src_pos = m_pWindow->CalPos();

        m_sText = pstrText;

        Invalidate();

        if (!m_pWindow) return;

        RECT rcPos = m_pWindow->CalPos();

        if (rcPos.left == src_pos.left) return;

        POINT pt;
        pt.x = rcPos.left;
        pt.y = rcPos.top;

        if (m_pManager) {
            ClientToScreen(m_pManager->GetPaintWindow(), &pt);
            ::SetWindowPos(m_pWindow->GetHWND(), NULL, pt.x, pt.y, 1,
                rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        
    }

    void CKeyEditUI::SetMaxChar(UINT uMax)
    {
        m_uMaxChar = uMax;
        if (m_pWindow != NULL) Edit_LimitText(*m_pWindow, m_uMaxChar);
    }

    UINT CKeyEditUI::GetMaxChar()
    {
        return m_uMaxChar;
    }

    void CKeyEditUI::SetReadOnly(bool bReadOnly)
    {
        if (m_bReadOnly == bReadOnly) return;

        m_bReadOnly = bReadOnly;
        if (m_pWindow != NULL) Edit_SetReadOnly(*m_pWindow, m_bReadOnly);
        Invalidate();
    }

    bool CKeyEditUI::IsReadOnly() const
    {
        return m_bReadOnly;
    }

    void CKeyEditUI::SetNumberOnly(bool bNumberOnly)
    {
        if (bNumberOnly)
        {
            m_iWindowStyls |= ES_NUMBER;
        }
        else
        {
            m_iWindowStyls |= ~ES_NUMBER;
        }
    }

    bool CKeyEditUI::IsNumberOnly() const
    {
        return m_iWindowStyls&ES_NUMBER ? true : false;
    }

    int CKeyEditUI::GetWindowStyls() const
    {
        return m_iWindowStyls;
    }

    void CKeyEditUI::SetPasswordMode(bool bPasswordMode)
    {
        if (m_bPasswordMode == bPasswordMode) return;
        m_bPasswordMode = bPasswordMode;
        Invalidate();
    }

    bool CKeyEditUI::IsPasswordMode() const
    {
        return m_bPasswordMode;
    }

    void CKeyEditUI::SetPasswordChar(TCHAR cPasswordChar)
    {
        if (m_cPasswordChar == cPasswordChar) return;
        m_cPasswordChar = cPasswordChar;
        if (m_pWindow != NULL) Edit_SetPasswordChar(*m_pWindow, m_cPasswordChar);
        Invalidate();
    }

    TCHAR CKeyEditUI::GetPasswordChar() const
    {
        return m_cPasswordChar;
    }

    LPCTSTR CKeyEditUI::GetNormalImage()
    {
        return m_sNormalImage;
    }

    void CKeyEditUI::SetNormalImage(LPCTSTR pStrImage)
    {
        m_sNormalImage = pStrImage;
        Invalidate();
    }

    LPCTSTR CKeyEditUI::GetHotImage()
    {
        return m_sHotImage;
    }

    void CKeyEditUI::SetHotImage(LPCTSTR pStrImage)
    {
        m_sHotImage = pStrImage;
        Invalidate();
    }

    LPCTSTR CKeyEditUI::GetFocusedImage()
    {
        return m_sFocusedImage;
    }

    void CKeyEditUI::SetFocusedImage(LPCTSTR pStrImage)
    {
        m_sFocusedImage = pStrImage;
        Invalidate();
    }

    LPCTSTR CKeyEditUI::GetDisabledImage()
    {
        return m_sDisabledImage;
    }

    void CKeyEditUI::SetDisabledImage(LPCTSTR pStrImage)
    {
        m_sDisabledImage = pStrImage;
        Invalidate();
    }

    void CKeyEditUI::SetNativeEditBkColor(DWORD dwBkColor)
    {
        m_dwEditbkColor = dwBkColor;
    }

    DWORD CKeyEditUI::GetNativeEditBkColor() const
    {
        return m_dwEditbkColor;
    }

    void CKeyEditUI::SetSel(long nStartChar, long nEndChar)
    {
        if (m_pWindow != NULL) Edit_SetSel(*m_pWindow, nStartChar, nEndChar);
    }

    void CKeyEditUI::SetSelAll()
    {
        SetSel(0, -1);
    }

    void CKeyEditUI::SetReplaceSel(LPCTSTR lpszReplace)
    {
        if (m_pWindow != NULL) Edit_ReplaceSel(*m_pWindow, lpszReplace);
    }

    RECT CKeyEditUI::CalEditPos()
    {
        CDuiRect rcPos = m_rcItem;
        RECT rcInset = GetTextPadding();
        rcPos.left += rcInset.left;
        rcPos.top += rcInset.top;
        rcPos.right -= rcInset.right;
        rcPos.bottom -= rcInset.bottom;
        return rcPos;
    }

    BOOL CKeyEditUI::CreateEmptyImage(CDuiImage* pImg, int nWidth, int nHeight, int nBitsCount)
    {
        int nMode = 0;
        if (pImg == NULL) return FALSE;
        if (nBitsCount == 32) nMode = 1;
        if (!pImg->IsNull()) pImg->Destroy();
        return pImg->Create(nWidth, nHeight, nBitsCount, nMode); //不考虑调色版了
    }

    BOOL CKeyEditUI::CopyImage(CDuiImage* pdestImage, CDuiImage* psrcImage, int nBitsCount /*= 32*/, int nx /*= 0*/, int ny /*= 0*/, int nWidth /*= 0*/, int nHeight /*= 0*/)
    {
        if (psrcImage == NULL) return FALSE;

        if (psrcImage->IsNull()) return FALSE;

        if (nWidth == 0) nWidth = psrcImage->GetWidth() - nx;

        if (nHeight == 0) nHeight = psrcImage->GetHeight() - ny;

        if (CreateEmptyImage(pdestImage, nWidth, nHeight, nBitsCount) == FALSE) return FALSE;

        ::BitBlt(pdestImage->GetDC(), 0, 0, nWidth, nHeight, psrcImage->GetDC(), nx, ny, SRCCOPY);

        psrcImage->ReleaseDC();
        pdestImage->ReleaseDC();

        GdiFlush();
        return TRUE;
    }

    void CKeyEditUI::SetPos(RECT rc)
    {
        RECT imgrc = CalEditPos();
        if (imgrc.right - imgrc.left > 0)
        {//如果尺寸改变 新建位图
            if (m_pEditBkImage) {
                if (!m_pEditBkImage->IsNull()) {
                    if (m_pEditBkImage->GetHeight() != (imgrc.right - imgrc.left) || m_pEditBkImage->GetWidth() != (imgrc.bottom - imgrc.top)) {
                        ReleaseEditImage();
                        CreateEmptyImage(m_pEditBkImage, imgrc.right - imgrc.left, imgrc.bottom - imgrc.top);
                    }
                }
                else
                {
                    CreateEmptyImage(m_pEditBkImage, imgrc.right - imgrc.left, imgrc.bottom - imgrc.top);
                }
            }
        }
        CControlUI::SetPos(rc);
        if (m_pWindow != NULL) {
            RECT rcPos = m_pWindow->CalPos();
            POINT pt;
            pt.x = rcPos.left;
            pt.y = rcPos.top;

            if (m_pManager) {
                ClientToScreen(m_pManager->GetPaintWindow(), &pt);
                ::SetWindowPos(m_pWindow->GetHWND(), NULL, pt.x, pt.y, 1,
                    rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }
    }

    void CKeyEditUI::SetVisible(bool bVisible)
    {
        CControlUI::SetVisible(bVisible);
        if (!IsVisible() && m_pWindow != NULL) m_pManager->SetFocus(NULL);
    }

    void CKeyEditUI::SetInternVisible(bool bVisible)
    {
        if (!IsVisible() && m_pWindow != NULL) m_pManager->SetFocus(NULL);
    }

    SIZE CKeyEditUI::EstimateSize(SIZE szAvailable)
    {
        if (m_cxyFixed.cy == 0)
        {
            SIZE sz = { m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 6 };
            return sz;
        }
        return CControlUI::EstimateSize(szAvailable);
    }

    void CKeyEditUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        if (_tcscmp(pstrName, _T("readonly")) == 0) SetReadOnly(_tcscmp(pstrValue, _T("true")) == 0);
        else if (_tcscmp(pstrName, _T("numberonly")) == 0) SetNumberOnly(_tcscmp(pstrValue, _T("true")) == 0);
        else if (_tcscmp(pstrName, _T("password")) == 0) SetPasswordMode(_tcscmp(pstrValue, _T("true")) == 0);
        else if (_tcscmp(pstrName, _T("maxchar")) == 0) SetMaxChar(_ttoi(pstrValue));
        else if (_tcscmp(pstrName, _T("normalimage")) == 0) SetNormalImage(pstrValue);
        else if (_tcscmp(pstrName, _T("hotimage")) == 0) SetHotImage(pstrValue);
        else if (_tcscmp(pstrName, _T("focusedimage")) == 0) SetFocusedImage(pstrValue);
        else if (_tcscmp(pstrName, _T("disabledimage")) == 0) SetDisabledImage(pstrValue);
        else if (_tcscmp(pstrName, _T("nativebkcolor")) == 0) {
            if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetNativeEditBkColor(clrColor);
        }
        else if (_tcscmp(pstrName, _T("showdefmenu")) == 0)
        {
            if (_tcscmp(pstrValue, _T("false")) == 0)
                m_bShowDefMenu = false;
        }
        else if (_tcscmp(pstrName, _T("deftextcolor")) == 0) {
            if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetDefTextColor(clrColor);
        }
        else if (_tcscmp(pstrName, _T("deftext")) == 0)
            SetDefText(pstrValue);
        else
            CLabelUI::SetAttribute(pstrName, pstrValue);
    }

    void CKeyEditUI::PaintStatusImage(HDC hDC)
    {
        if (IsFocused()) m_uButtonState |= UISTATE_FOCUSED;
        else m_uButtonState &= ~UISTATE_FOCUSED;
        if (!IsEnabled()) m_uButtonState |= UISTATE_DISABLED;
        else m_uButtonState &= ~UISTATE_DISABLED;

        if ((m_uButtonState & UISTATE_DISABLED) != 0) {
            if (!m_sDisabledImage.IsEmpty()) {
                if (!DrawImage(hDC, (LPCTSTR)m_sDisabledImage)) m_sDisabledImage.Empty();
                else goto Label_ForeImage;
            }
        }
        else if ((m_uButtonState & UISTATE_FOCUSED) != 0) {
            if (!m_sFocusedImage.IsEmpty()) {
                if (!DrawImage(hDC, (LPCTSTR)m_sFocusedImage)) m_sFocusedImage.Empty();
                else goto Label_ForeImage;
            }
        }
        else if ((m_uButtonState & UISTATE_HOT) != 0) {
            if (!m_sHotImage.IsEmpty()) {
                if (!DrawImage(hDC, (LPCTSTR)m_sHotImage)) m_sHotImage.Empty();
                else goto Label_ForeImage;
            }
        }

        if (!m_sNormalImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sNormalImage)) m_sNormalImage.Empty();
            else goto Label_ForeImage;
        }

    Label_ForeImage:
        if (m_pEditBkImage) {
            if (!m_pEditBkImage->IsNull()) {
                RECT editrc = CalEditPos();
                //m_pEditBkImage->CopyImageFromDC(hDC,editrc.left,editrc.top,m_pEditBkImage->GetWidth(),m_pEditBkImage->GetHeight(),32);
                ::BitBlt(m_pEditBkImage->GetDC(), 0, 0, m_pEditBkImage->GetWidth(), m_pEditBkImage->GetHeight(), hDC, editrc.left, editrc.top, SRCCOPY);
                //m_pEditBkImage->Save(_T("D:\\11.png"),Gdiplus::ImageFormatPNG);
                m_pEditBkImage->ReleaseDC();
                //绘制背景到背景位图供edit窗口使用
            }
            else
            {//位图为空了 创建位图
                RECT editrc = CalEditPos();
                CreateEmptyImage(m_pEditBkImage, editrc.right - editrc.left, editrc.bottom - editrc.top);
                if (!m_pEditBkImage->IsNull()) {
                    RECT editrc = CalEditPos();
                    ::BitBlt(m_pEditBkImage->GetDC(), 0, 0, m_pEditBkImage->GetWidth(), m_pEditBkImage->GetHeight(), hDC, editrc.left, editrc.top, SRCCOPY);
                    m_pEditBkImage->ReleaseDC();
                }
            }
        }
    }

    void CKeyEditUI::PaintText(HDC hDC)
    {
        if (m_dwTextColor == 0) m_dwTextColor = m_pManager->GetDefaultFontColor();
        if (m_dwDisabledTextColor == 0) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

        if (m_sText.IsEmpty())
        {//绘制默认文字
            if (m_szDefText.IsEmpty())
                return;
            RECT rc = m_rcItem;
            rc.left += m_rcTextPadding.left;
            rc.right -= m_rcTextPadding.right;
            rc.top += m_rcTextPadding.top;
            rc.bottom -= m_rcTextPadding.bottom;
            CRenderEngine::DrawText(hDC, m_pManager, rc, m_szDefText, m_dwDefTextColor, m_iFont, m_uTextStyle, m_bDrawTextPlus);
            return;
        }
        //绘制文字
        CDuiString sText;
        if (m_bPasswordMode)
        {
            for (int i = 0; i < m_sText.GetLength(); ++i)
                sText += m_cPasswordChar;
        }
        else
        {
            sText = m_sText;
        }
        RECT rc = m_rcItem;
        rc.left += m_rcTextPadding.left;
        rc.right -= m_rcTextPadding.right;
        rc.top += m_rcTextPadding.top;
        rc.bottom -= m_rcTextPadding.bottom;
        if (IsEnabled())
            CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwTextColor, m_iFont, m_uTextStyle, m_bDrawTextPlus);
        else
            CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, m_iFont, m_uTextStyle, m_bDrawTextPlus);
    }

    void CKeyEditUI::SetDefText(LPCTSTR pstrValue)
    {
        m_szDefText = pstrValue;
    }

    void CKeyEditUI::SetDefTextColor(DWORD dwColor)
    {
        m_dwDefTextColor = dwColor;
    }

    HWND CKeyEditUI::GetEditWnd()
    {
        if (NULL == m_pWindow)
            return NULL;
        return m_pWindow->GetHWND();
    }

}
