#pragma once
#include <ShObjIdl.h>
#include <intshcut.h>
#include <atlbase.h>
#include <ShlObj.h>



//枚举两种基本快捷方式
enum ShortcutType
{
	UrlLink,
	FileLink,
};

namespace PublicLib{

	class CShortcutHelper
	{
	public:
		CShortcutHelper(void);
		~CShortcutHelper(void);
		//Sets the path and file name of a Shell link object.
		void SetResPath(const wstring& strResPath) { m_strResPath = strResPath; }
		void SetArguments(const wstring& strArguments) { m_strArguments = strArguments; }
		void SetUrl(const wstring& strUrl) { m_strUrl = strUrl; }
		void SetIconPath(const wstring& strIconPath) { m_strIconPath = strIconPath; }
		const wchar_t* GetErrorInfo()const { return m_strError.c_str(); }
		bool CreateUrlLink(const wchar_t* pSavePath);
		bool CreateFileLink(const wchar_t* pSavePath);
		bool CreateFileLinkAtSpecialPath(const wchar_t* pName, const int nCsidl = CSIDL_DESKTOPDIRECTORY);
		bool CreateUrlLinkAtSpecialPath(const wchar_t* pName, const int nCsidl = CSIDL_DESKTOPDIRECTORY);
		static bool GetUrlLinkDes(const wchar_t* pFilePath, wchar_t* pBuffer, const int nBufferLen);
		static bool GetFileLinkDes(const wchar_t* pFilePath, wchar_t* pBuffer, const int nBufferLen);
		static bool RemoveShortcutAtSpecialPath(const wchar_t* pName, const int nCsidl = CSIDL_DESKTOPDIRECTORY, ShortcutType type = FileLink);

	protected:
		bool InitLinkInstance();
		bool InitUrlInstance();
		void SetError(const wchar_t* pError){ m_strError = pError; }

	private:
		wstring m_strResPath;
		wstring m_strArguments;
		wstring m_strUrl;		//对于超链接快捷方式对应的URL
		wstring m_strIconPath;
		wstring m_strError;
		CComPtr<IShellLink>	m_pIShellLink;
		CComPtr<IUniformResourceLocator> m_pIUrl;
	};
}