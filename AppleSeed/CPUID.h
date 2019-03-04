#ifndef __CPUID_INCLUDE_H__
#define __CPUID_INCLUDE_H__

#pragma once

class CPUID {
public:
    CPUID();
    virtual ~CPUID();

public:
    string GetBrand();
    string GetVID();
    string GetSerialNumber();

    bool IsEST();
    bool IsMMX();
    bool IsSupportVT();

private:
    void Executecpuid(DWORD eax); // ����ʵ��cpuid

    DWORD m_eax;   // �洢���ص�eax
    DWORD m_ebx;   // �洢���ص�ebx
    DWORD m_ecx;   // �洢���ص�ecx
    DWORD m_edx;   // �洢���ص�edx
};

#endif // __CPUID_INCLUDE_H__
