#include "StdAfx.h"
#include "GlobalData.h"
#include <algorithm>
#include "CPUID.h"

CGlobalData::CGlobalData()
 : m_hWndMain(nullptr)
 , m_hWndIos(nullptr)
 , m_pWndMain(nullptr)
 , need_reboot_(false)
 , programme_mode_(false)
 , cpu_type_(0) {
}


CGlobalData::~CGlobalData() {
}

void CGlobalData::Init(){
    m_strRunPath = GetRunPathW();
    m_strAppVersion = GetApplicationVersion();
    m_strDocPath = GetDocumentPath();
    m_strDataPath = m_strDocPath + L"\\Data.dat";
    m_strIosPath = m_strDocPath + L"\\ios";
    PublicLib::GetOSVersion(m_strOSName, m_strOSVersion);
    m_strOSType = PublicLib::Is64bitSystem() ? L"64" : L"32";
    PublicLib::GetMacAddress(m_strMacAddr);
    GetDeviceGuid(m_strGuid);
    GetCpuInfo();

    InitKeyboardMapping();
}

void CGlobalData::Exit() {
    keyboard_mapping_.clear();
}

CWndBase* CGlobalData::GetMainWndPtr()
{
    if (IsWindow(m_hWndMain))
        return m_pWndMain;
    return NULL;
}

const wstring& CGlobalData::GetDocPath() const
{
    if (!PathFileExists(m_strDocPath.c_str()))
    {
        SHCreateDirectory(NULL, m_strDocPath.c_str());
    }
    return m_strDocPath;
}

const wstring& CGlobalData::GetIosPath() const {
    if (!PathFileExists(m_strIosPath.c_str()))
        SHCreateDirectory(NULL, m_strIosPath.c_str());
    return m_strIosPath;
}

wstring CGlobalData::GetIosVmPath() {
    wstring strPath = m_strIosPath + L"\\vm";
    if (!PathFileExists(strPath.c_str()))
        SHCreateDirectory(NULL, strPath.c_str());
    return strPath;
}

wstring CGlobalData::GetDefLoadPath()
{
    wstring strPath = m_strDocPath + L"\\Download";
    if (!PathFileExists(strPath.c_str()))
        SHCreateDirectory(NULL, strPath.c_str());
    return strPath;
}

std::wstring CGlobalData::GetKeyboardStr(int key_code) {
    auto it = keyboard_mapping_.find(key_code);
    if (it != keyboard_mapping_.end()) return it->second;
    return L"";
}

void CGlobalData::GetCpuInfo() {
    CPUID cpuid;
    auto info = cpuid.GetBrand();

    if (info.find("Intel") != string::npos) cpu_type_ = 0;
    else if (info.find("AMD") != string::npos) cpu_type_ = 1;
    else cpu_type_ = -1;

    OUTPUT_XYLOG(LEVEL_INFO, L"获取处理器类型：%s, 类型值：%d", PublicLib::AToU(info).c_str(), cpu_type_);
}

void CGlobalData::InitKeyboardMapping() {
    // 以下是以获取的键盘值为key
    keyboard_mapping_[VK_ESCAPE] = L"ESC";//保留作为快捷键
    keyboard_mapping_[VK_F1] = L"F1 ";
    keyboard_mapping_[VK_F2] = L"F2 ";
    keyboard_mapping_[VK_F3] = L"F3 ";
    keyboard_mapping_[VK_F4] = L"F4 ";
    keyboard_mapping_[VK_F5] = L"F5 ";
    keyboard_mapping_[VK_F6] = L"F6 ";
    keyboard_mapping_[VK_F7] = L"F7 ";
    keyboard_mapping_[VK_F8] = L"F8 ";
    keyboard_mapping_[VK_F9] = L"F9 ";
    keyboard_mapping_[VK_F10] = L"F10";
    keyboard_mapping_[VK_F11] = L"F11";
    keyboard_mapping_[VK_F12] = L"F12";

    keyboard_mapping_[VK_OEM_3] = L"、~";// '`~' for US
    keyboard_mapping_[G_VK_1] = L"1";
    keyboard_mapping_[G_VK_2] = L"2";
    keyboard_mapping_[G_VK_3] = L"3";
    keyboard_mapping_[G_VK_4] = L"4";
    keyboard_mapping_[G_VK_5] = L"5";
    keyboard_mapping_[G_VK_6] = L"6";
    keyboard_mapping_[G_VK_7] = L"7";
    keyboard_mapping_[G_VK_8] = L"8";
    keyboard_mapping_[G_VK_9] = L"9";
    keyboard_mapping_[G_VK_0] = L"0";
    keyboard_mapping_[VK_OEM_MINUS] = L"-_";/* - on main keyboard */
    keyboard_mapping_[VK_OEM_PLUS] = L"=+";// '+' or '='
    keyboard_mapping_[VK_BACK] = L"B-S";/* backspace */

    keyboard_mapping_[VK_TAB] = L"Tab";
    keyboard_mapping_[G_VK_Q] = L"Q";
    keyboard_mapping_[G_VK_W] = L"W";
    keyboard_mapping_[G_VK_E] = L"E";
    keyboard_mapping_[G_VK_R] = L"R";
    keyboard_mapping_[G_VK_T] = L"T";
    keyboard_mapping_[G_VK_Y] = L"Y";
    keyboard_mapping_[G_VK_U] = L"U";
    keyboard_mapping_[G_VK_I] = L"I";
    keyboard_mapping_[G_VK_O] = L"O";
    keyboard_mapping_[G_VK_P] = L"P";
    keyboard_mapping_[VK_OEM_4] = L"[{";//  '[{' for US
    keyboard_mapping_[VK_OEM_6] = L"]}";//  ']}' for US
    keyboard_mapping_[VK_OEM_5] = L"\|";//  '\|' for US

    keyboard_mapping_[VK_CAPITAL] = L"CLk";
    keyboard_mapping_[G_VK_A] = L"A";
    keyboard_mapping_[G_VK_S] = L"S";
    keyboard_mapping_[G_VK_D] = L"D";
    keyboard_mapping_[G_VK_F] = L"F";
    keyboard_mapping_[G_VK_G] = L"G";
    keyboard_mapping_[G_VK_H] = L"H";
    keyboard_mapping_[G_VK_J] = L"J";
    keyboard_mapping_[G_VK_K] = L"K";
    keyboard_mapping_[G_VK_L] = L"L";
    keyboard_mapping_[VK_OEM_1] = L";:";// ';:' for US
    keyboard_mapping_[VK_OEM_7] = L"'";//  ''"' for US
    keyboard_mapping_[VK_RETURN] = L"Enter";/* Enter on main keyboard */

    keyboard_mapping_[VK_SHIFT] = L"Shift";
    keyboard_mapping_[G_VK_Z] = L"Z";
    keyboard_mapping_[G_VK_X] = L"X";
    keyboard_mapping_[G_VK_C] = L"C";
    keyboard_mapping_[G_VK_V] = L"V";
    keyboard_mapping_[G_VK_B] = L"B";
    keyboard_mapping_[G_VK_N] = L"N";
    keyboard_mapping_[G_VK_M] = L"M";
    keyboard_mapping_[VK_OEM_COMMA] = L", <";// ',' or '<'
    keyboard_mapping_[VK_OEM_PERIOD] = L". >";/* . on main keyboard */
    keyboard_mapping_[VK_OEM_2] = L"/ ?";/* / on main keyboard */				// '/?' for US

    keyboard_mapping_[VK_CONTROL] = L"Ctrl";
    keyboard_mapping_[VK_LWIN] = L"L_W";
    keyboard_mapping_[VK_MENU] = L"Alt";/* left Alt */
    keyboard_mapping_[VK_SPACE] = L"空格";
    keyboard_mapping_[VK_RWIN] = L"R_W";
    keyboard_mapping_[VK_APPS] = L"Aps";

    keyboard_mapping_[VK_PRINT] = L"PSn";//PrtScn SysRq
    keyboard_mapping_[VK_PAUSE] = L"Pau";//PrtScn SysRq
    keyboard_mapping_[VK_SCROLL] = L"Scr";/* Scroll Lock */
    //Pause Break
    keyboard_mapping_[VK_INSERT] = L"Ins";	/* Insert on arrow keypad */
    keyboard_mapping_[VK_HOME] = L"Hom";		/* Home on arrow keypad */
    keyboard_mapping_[VK_PRIOR] = L"PaU";		/* PgUp on arrow keypad */
    keyboard_mapping_[VK_DELETE] = L"Del";	/* Delete on arrow keypad */
    keyboard_mapping_[VK_END] = L"End";			/* End on arrow keypad */
    keyboard_mapping_[VK_NEXT] = L"PDn";		/* PgDn on arrow keypad */
    keyboard_mapping_[VK_UP] = L"Up";			/* UpArrow on arrow keypad */
    keyboard_mapping_[VK_DOWN] = L"Dwn";		 /* DownArrow on arrow keypad */
    keyboard_mapping_[VK_LEFT] = L"Lef";		/* LeftArrow on arrow keypad */
    keyboard_mapping_[VK_RIGHT] = L"Rig";		/* RightArrow on arrow keypad */

    keyboard_mapping_[VK_NUMLOCK] = L"NLk";
    keyboard_mapping_[VK_DIVIDE] = L"N /";/* / on numeric keypad */
    keyboard_mapping_[VK_MULTIPLY] = L"N *";/* * on numeric keypad */
    keyboard_mapping_[VK_SUBTRACT] = L"N -";/* - on numeric keypad */
    keyboard_mapping_[VK_NUMPAD7] = L"N-7";
    keyboard_mapping_[VK_NUMPAD8] = L"N-8";
    keyboard_mapping_[VK_NUMPAD9] = L"N-9";
    keyboard_mapping_[VK_NUMPAD4] = L"N-4";
    keyboard_mapping_[VK_NUMPAD5] = L"N-5";
    keyboard_mapping_[VK_NUMPAD6] = L"N-6";
    keyboard_mapping_[VK_ADD] = L"N-+";/* + on numeric keypad */
    keyboard_mapping_[VK_NUMPAD1] = L"N-1";
    keyboard_mapping_[VK_NUMPAD2] = L"N-2";
    keyboard_mapping_[VK_NUMPAD3] = L"N-3";
    keyboard_mapping_[VK_NUMPAD0] = L"N-0";
    keyboard_mapping_[VK_DECIMAL] = L"N-D";/* . on numeric keypad */
    //keyboard_mapping_[DIK_NUMPADENTER] = L"N-E";/* Enter on numeric keypad */
}

void CGlobalData::InitPhpParams(OUT Json::Value& vRoot)
{
    vRoot["version"] = PublicLib::UToA(m_strAppVersion);
    string strGuid = PublicLib::UToA(m_strGuid);
    vRoot["deviceid"] = strGuid;
    vRoot["Unique-Code"] = "";
    vRoot["Access-Token"] = "A2838C35DAAAE9058E521930B8D27F9A0538C7F8D7CD248A263820E6A6B27FB12058141FDA3CEF5D98BDC24C53F07B2B";
    vRoot["ChaoHuiWan-App-Interal-Tags"] = "Salem501200";
}