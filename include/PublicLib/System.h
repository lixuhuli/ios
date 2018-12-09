#pragma once

namespace PublicLib{

	//��ȡ����ϵͳ�汾��Ϣ
	bool GetOSVersion(OUT wstring& strOsName, OUT wstring& strVersion);

	//��ȡ����ϵͳ��Ϣ
	void SafeGetNativeSystemInfo(LPSYSTEM_INFO lpSysInfo);

	//�Ƿ���64λϵͳ
	bool Is64bitSystem();

	bool CreateGuid(OUT wstring& strGuid);
	//��ȡ����IP��ַ
	bool GetMacAddress(std::wstring& strMac);

	bool IsOsOverVista();

	//����������
	bool PinToWin7TaskBar(const wchar_t* pDirPath, const wchar_t* pExeName);
	bool UnpinToWin7TaskBar(const wchar_t* pDirPath, const wchar_t* pExeName);

	//ˢ��ϵͳͼ��
	void RefreshIcons();

	//�ѵ�ǰȨ������
	DWORD ShellExecuteRunas(const wchar_t* lpExe, const wchar_t* lpParam, const wchar_t* lpWorkDir);

	//��ֹ�߳�
	void SafeKillThread(HANDLE* hThread, DWORD dwWaitTime);

	BOOL IsRunasAdmin(HANDLE hProcess);

	//���ÿ�������
	bool SetBootRun(const wstring& strKey, const wstring& strValue);

	bool CopyToClipboard(const wstring& strData);

	BOOL EnableDebugPrivilege();
}