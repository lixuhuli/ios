#include "StdAfx.h"
#include "Shortcut.h"
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi")
#include <assert.h>

namespace PublicLib{

	CShortcutHelper::CShortcutHelper(void)
	{

	}

	CShortcutHelper::~CShortcutHelper(void)
	{

	}

	bool CShortcutHelper::InitLinkInstance()
	{
		HRESULT hr = m_pIShellLink.CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER);
		if (SUCCEEDED(hr))
			return true;
		SetError(_T("初始化COM快捷方式实例失败"));
		return false;
	}

	bool CShortcutHelper::InitUrlInstance()
	{
		HRESULT hr = m_pIUrl.CoCreateInstance(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER);
		if (SUCCEEDED(hr))
			return true;
		SetError(_T("初始化COM URL快捷方式实例失败"));
		return false;
	}

	bool CShortcutHelper::CreateUrlLink(const wchar_t* pSavePath)
	{
		if (NULL == pSavePath)
			return false;
		::DeleteFile(pSavePath);
		if (m_strResPath.empty() || !PathFileExists(m_strResPath.c_str()))
		{
			SetError(L"快捷方式源文件不存在");
			return false;
		}
		if (NULL == m_pIUrl && !InitUrlInstance())
			return false;
		HRESULT hr = m_pIUrl->SetURL(m_strUrl.c_str(), 0);
		if (FAILED(hr))
		{
			SetError(L"SetURL接口调用失败");
			return false;
		}
		if (!m_strIconPath.empty() && PathFileExists(m_strIconPath.c_str()))
		{
			CComPtr<IShellLink> pLink;
			hr = m_pIUrl->QueryInterface(IID_IShellLink, (void**)&pLink);
			if (SUCCEEDED(hr))
			{
				pLink->SetIconLocation(m_strIconPath.c_str(), 0);
			}
		}
		CComPtr<IPersistFile> pFile;
		hr = m_pIUrl->QueryInterface(IID_IPersistFile, (void **)&pFile);
		if (FAILED(hr))
			return false;
		hr = pFile->Save(pSavePath, TRUE);
		if (FAILED(hr))
		{
			SetError(L"保存RUL快捷方式失败");
			return false;
		}
		return true;
	}

	bool CShortcutHelper::CreateFileLink(const wchar_t* pSavePath)
	{
		::DeleteFile(pSavePath);
		if (m_strResPath.empty() || !PathFileExists(m_strResPath.c_str()))
		{
			SetError(L"快捷方式源文件不存在");
			return false;
		}
		if (NULL == m_pIShellLink && !InitLinkInstance())
			return false;
		m_pIShellLink->SetPath(m_strResPath.c_str());
		if (!m_strArguments.empty())
			m_pIShellLink->SetArguments(m_strArguments.c_str());
		int nPos = m_strResPath.rfind('\\');
		if (nPos != wstring::npos)
		{
			wstring strDir = m_strResPath.substr(0, nPos);
			m_pIShellLink->SetWorkingDirectory(strDir.c_str());
		}
		if (!m_strIconPath.empty() && PathFileExists(m_strIconPath.c_str()))
			m_pIShellLink->SetIconLocation(m_strIconPath.c_str(), 0);
		CComPtr<IPersistFile> pFile;
		HRESULT hr = m_pIShellLink->QueryInterface(IID_IPersistFile, (void **)&pFile);
		if (FAILED(hr))
			return false;
		hr = pFile->Save(pSavePath, TRUE);
		if (FAILED(hr))
		{
			SetError(L"保存文件快捷方式失败");
			return false;
		}
		return true;
	}

	bool CShortcutHelper::CreateFileLinkAtSpecialPath(const wchar_t* pName, const int nCsidl/*=CSIDL_COMMON_DESKTOPDIRECTORY*/)
	{
		wchar_t szPath[MAX_PATH + 1] = { 0 };
		if (!SHGetSpecialFolderPath(NULL, szPath, nCsidl, FALSE))
		{
			SetError(L"获取系统特殊路径失败");
			return false;
		}
		if (CSIDL_APPDATA == nCsidl)
			wcscat_s(szPath, L"\\Microsoft\\Internet Explorer\\Quick Launch");
		wcscat_s(szPath, L"\\");
		wcscat_s(szPath, pName);
		wcscat_s(szPath, L".lnk");
		return CreateFileLink(szPath);
	}

	bool CShortcutHelper::CreateUrlLinkAtSpecialPath(const wchar_t* pName, const int nCsidl/*=CSIDL_COMMON_DESKTOPDIRECTORY*/)
	{
		wchar_t szPath[MAX_PATH + 1] = { 0 };
		if (!SHGetSpecialFolderPath(NULL, szPath, nCsidl, FALSE))
		{
			SetError(L"获取系统特殊路径失败");
			return false;
		}
		if (CSIDL_APPDATA == nCsidl)
			wcscat_s(szPath, L"\\Microsoft\\Internet Explorer\\Quick Launch");
		wcscat_s(szPath, L"\\");
		wcscat_s(szPath, pName);
		wcscat_s(szPath, L".url");
		return CreateUrlLink(szPath);
	}

	bool CShortcutHelper::GetUrlLinkDes(const wchar_t* pFilePath, wchar_t* pBuffer, const int nBufferLen)
	{
		CComPtr<IUniformResourceLocator> pURL;
		HRESULT hr = pURL.CoCreateInstance(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER);
		if (FAILED(hr))
			return false;
		CComPtr<IPersistFile> pFile;
		hr = pURL->QueryInterface(IID_IPersistFile, (void**)&pFile);
		if (FAILED(hr))
			return false;
		LPTSTR lpUrl = NULL;
		hr = pFile->Load(pFilePath, STGM_READ);
		if (FAILED(hr))
			return false;
		hr = pURL->GetURL(&lpUrl);
		if (FAILED(hr))
			return false;
		if (lpUrl)
			wcscpy_s(pBuffer, nBufferLen, lpUrl);
		return true;
	}

	bool CShortcutHelper::GetFileLinkDes(const wchar_t* pFilePath, wchar_t* pBuffer, const int nBufferLen)
	{
		CComPtr<IShellLink> pLink;
		HRESULT hr = pLink.CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER);
		if (FAILED(hr))
			return false;
		CComPtr<IPersistFile> pFile;
		hr = pLink->QueryInterface(IID_IPersistFile, (void**)&pFile);
		if (FAILED(hr))
			return false;
		hr = pFile->Load(pFilePath, STGM_READ);
		if (FAILED(hr))
			return false;
		return SUCCEEDED(pLink->GetPath(pBuffer, nBufferLen, NULL, SLGP_UNCPRIORITY));
	}

	bool CShortcutHelper::RemoveShortcutAtSpecialPath(const wchar_t* pName, const int nCsidl/*=CSIDL_COMMON_DESKTOPDIRECTORY*/, ShortcutType type/*=url_link*/)
	{
		wchar_t szPath[MAX_PATH + 1] = { 0 };
		SHGetSpecialFolderPath(NULL, szPath, nCsidl, FALSE);
		if (CSIDL_APPDATA == nCsidl)
			wcscat_s(szPath, _T("\\Microsoft\\Internet Explorer\\Quick Launch"));
		wcscat_s(szPath, _T("\\"));
		wcscat_s(szPath, pName);
		if (UrlLink == type)
			wcscat_s(szPath, _T(".url"));
		else
			wcscat_s(szPath, _T(".lnk"));
		DeleteFile(szPath);
		return true;
	}
}