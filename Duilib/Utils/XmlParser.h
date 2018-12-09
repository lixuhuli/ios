#pragma once
#include "..\Core\UIMarkup.h"

namespace DuiLib
{
	class  CXmlParser
	{
	public:
		static CXmlParser* Instance();
		bool LoadStaticXmlFile(LPCTSTR pStrXml);
		LPCTSTR GetXmlText(LPCTSTR pstrRoot, LPCTSTR pStrId);
	private:
		CXmlParser();
		bool m_bLoaded;
	private:
		CMarkup m_xmlText;
	};
}