#include "stdafx.h"
#include "XmlParser.h"

using namespace DuiLib;

CXmlParser::CXmlParser() :
m_bLoaded(false)
{

}

CXmlParser* CXmlParser::Instance()
{
	static CXmlParser parser;
	return &parser;
}

bool CXmlParser::LoadStaticXmlFile(LPCTSTR pStrXml)
{
	m_bLoaded = m_xmlText.LoadFromFile(pStrXml);
	return m_bLoaded;
}

LPCTSTR CXmlParser::GetXmlText(LPCTSTR pstrRoot, LPCTSTR pStrId)
{
	if(!m_bLoaded) return _T("");

	CMarkupNode root = m_xmlText.GetRoot();
	CMarkupNode node = root.GetChild();
	for( ; node.IsValid(); node = node.GetSibling())
	{
		if(_tcscmp(node.GetName(), pstrRoot) != 0)
			continue;
		break;
	}
	for (CMarkupNode childNode=node.GetChild(); childNode.IsValid(); childNode=childNode.GetSibling())
	{
		LPCTSTR pStrValue = childNode.GetAttributeValue(_T("id"));
		if(_tcscmp(pStrValue, pStrId) == 0)
		{
			return childNode.GetValue();
		}
	}

	return _T("");
}