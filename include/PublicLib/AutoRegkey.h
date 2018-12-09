#pragma once
#include <Windows.h>
#pragma comment(lib, "Advapi32")



//定义一个简单的自动关闭注册表类
class CAutoRegkey
{
public:
	CAutoRegkey()
		:m_hKey(NULL)
	{

	}
	~CAutoRegkey()
	{
		Close();
	}
	PHKEY operator &()
	{
		return &m_hKey;
	}
	operator HKEY()
	{
		return m_hKey;
	}
	void Close()
	{
		if ( m_hKey )
		{
			RegCloseKey(m_hKey);
			m_hKey = NULL;
		}
	}
protected:
	CAutoRegkey(const CAutoRegkey& key);
	CAutoRegkey& operator = (const CAutoRegkey& key);
private:
	HKEY	m_hKey;
};