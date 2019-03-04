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
    void Executecpuid(DWORD eax); // 用来实现cpuid

    DWORD m_eax;   // 存储返回的eax
    DWORD m_ebx;   // 存储返回的ebx
    DWORD m_ecx;   // 存储返回的ecx
    DWORD m_edx;   // 存储返回的edx
};

#endif // __CPUID_INCLUDE_H__
