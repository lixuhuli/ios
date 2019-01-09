#include "StdAfx.h"
#include "CPUID.h"

CPUID::CPUID() 
 : m_eax(0) 
 , m_ebx(0)
 , m_ecx(0)
 , m_edx(0) {
}


CPUID::~CPUID() {
}

void CPUID::Executecpuid(DWORD veax) {
    // 因为嵌入式的汇编代码不能识别 类成员变量
    // 所以定义四个临时变量作为过渡
    DWORD deax;
    DWORD debx;
    DWORD decx;
    DWORD dedx;

    __asm
    {
        mov eax,veax
            cpuid
            mov deax,eax
            mov debx,ebx
            mov decx,ecx
            mov dedx,edx
    }

    m_eax = deax; // 把临时变量中的内容放入类成员变量
    m_ebx = debx;
    m_ecx = decx;
    m_edx = dedx;
}

string CPUID::GetBrand() {
    const DWORD id = 0x80000002;             // 从0x80000002开始，到0x80000004结束
    char szBrand[49];                        // 用来存储商标字符串，48个字符
    memset(szBrand, 0, 49);                  // 初始化为0

    for (DWORD i = 0; i < 3; i++) {          // 依次执行3个指令
        Executecpuid(id + i);   

        // 每次循环结束,保存信息到数组
        memcpy(szBrand+16 * i + 0, &m_eax, 4);
        memcpy(szBrand+16 * i + 4, &m_ebx, 4);
        memcpy(szBrand+16 * i + 8, &m_ecx, 4);
        memcpy(szBrand+16 * i + 12, &m_edx, 4);
    } 

    return string(szBrand);  // 以string的形式返回
}

