#pragma once

/*
* VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
* 0x40 : unassigned
* VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
*/
enum
{
    G_VK_0 = 0x30,
    G_VK_1 = 0x31,
    G_VK_2,
    G_VK_3,
    G_VK_4,
    G_VK_5,
    G_VK_6,
    G_VK_7,
    G_VK_8,
    G_VK_9 = 0x39,
};

enum
{
    G_VK_A = 0x41,
    G_VK_B,
    G_VK_C,
    G_VK_D,
    G_VK_E,
    G_VK_F,
    G_VK_G,
    G_VK_H,
    G_VK_I,
    G_VK_J,
    G_VK_K,
    G_VK_L,
    G_VK_M,
    G_VK_N,
    G_VK_O,
    G_VK_P,
    G_VK_Q,
    G_VK_R,
    G_VK_S,
    G_VK_T,
    G_VK_U,
    G_VK_V,
    G_VK_W,
    G_VK_X,
    G_VK_Y,
    G_VK_Z = 0x5A,
};

class CGlobalData {
public:
	static CGlobalData* Instance() {
		static CGlobalData obj;
		return &obj;
	}
	

public:
	CGlobalData();
	virtual ~CGlobalData();

public:

public:
    void Init();
    void Exit();

    std::wstring GetKeyboardStr(int key_code);

protected:
    void InitKeyboardMapping();

private:
    map<int, std::wstring> keyboard_mapping_;
};
