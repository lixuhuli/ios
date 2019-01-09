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
    // ��ΪǶ��ʽ�Ļ����벻��ʶ�� ���Ա����
    // ���Զ����ĸ���ʱ������Ϊ����
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

    m_eax = deax; // ����ʱ�����е����ݷ������Ա����
    m_ebx = debx;
    m_ecx = decx;
    m_edx = dedx;
}

string CPUID::GetBrand() {
    const DWORD id = 0x80000002;             // ��0x80000002��ʼ����0x80000004����
    char szBrand[49];                        // �����洢�̱��ַ�����48���ַ�
    memset(szBrand, 0, 49);                  // ��ʼ��Ϊ0

    for (DWORD i = 0; i < 3; i++) {          // ����ִ��3��ָ��
        Executecpuid(id + i);   

        // ÿ��ѭ������,������Ϣ������
        memcpy(szBrand+16 * i + 0, &m_eax, 4);
        memcpy(szBrand+16 * i + 4, &m_ebx, 4);
        memcpy(szBrand+16 * i + 8, &m_ecx, 4);
        memcpy(szBrand+16 * i + 12, &m_edx, 4);
    } 

    return string(szBrand);  // ��string����ʽ����
}

