#pragma once
#include <string>
#include <vector>
using std::wstring;
using std::vector;

namespace PublicLib {

	class CVersionHelper
	{
	public:
		CVersionHelper()
			:m_nMajorNumber(0)
			, m_nMinorNumber(0)
			, m_nRevisionNumber(0)
			, m_nBuildNumber(0)
		{

		}
		CVersionHelper(const wstring& strVersion)
			:m_nMajorNumber(0)
			, m_nMinorNumber(0)
			, m_nRevisionNumber(0)
			, m_nBuildNumber(0)
		{
			GetVersion(strVersion);
		}
		virtual ~CVersionHelper()
		{
		}
		bool IsDigitString(const wstring& strValue)
		{
			if (strValue.empty())
				return false;
			for (size_t i = 0; i < strValue.size(); ++i)
			{
				if (!iswdigit(strValue[i]))
					return false;
			}
			return true;
		}
		bool GetVersion(const wstring& strVersion)
		{
			size_t nBegin = 0, nPos = 0;
			vector<wstring>	numList;
			wstring strSub;
			while ((nPos = strVersion.find(L".", nBegin)) != wstring::npos)
			{
				strSub = strVersion.substr(nBegin, nPos - nBegin);
				if (strSub.empty())
					return false;
				if (!IsDigitString(strSub))
					return false;
				numList.push_back(strSub);
				nBegin = nPos + 1;
			}
			if (nBegin < strVersion.size())
			{
				strSub = strVersion.substr(nBegin, strVersion.size() - nBegin);
				if (IsDigitString(strSub))
					numList.push_back(strSub);
			}
			if (numList.size()<2)
				return false;
			m_nMajorNumber = _wtoi(numList[0].c_str());
			m_nMinorNumber = _wtoi(numList[1].c_str());
			if (numList.size()>2)
				m_nRevisionNumber = _wtoi(numList[2].c_str());
			if (numList.size() > 3)
				m_nBuildNumber = _wtoi(numList[3].c_str());
			return true;
		}
		int	 Compare(const CVersionHelper& ver)
		{
			if (m_nMajorNumber > ver.m_nMajorNumber)
				return 1;
			if (m_nMajorNumber < ver.m_nMajorNumber)
				return -1;
			if (m_nMinorNumber > ver.m_nMinorNumber)
				return 1;
			if (m_nMinorNumber < ver.m_nMinorNumber)
				return -1;
			if (m_nRevisionNumber > ver.m_nRevisionNumber)
				return 1;
			if (m_nRevisionNumber < ver.m_nRevisionNumber)
				return -1;
			if (m_nBuildNumber > ver.m_nBuildNumber)
				return 1;
			if (m_nBuildNumber < ver.m_nBuildNumber)
				return -1;
			return 0;
		}
		bool operator ==(const CVersionHelper& ver)
		{
			return Compare(ver) == 0;
		}
		bool operator !=(const CVersionHelper& ver)
		{
			return Compare(ver) != 0;
		}
		bool operator <(const CVersionHelper& ver)
		{
			return Compare(ver) < 0;
		}
		bool operator >(const CVersionHelper& ver)
		{
			return Compare(ver) > 0;
		}
		bool operator >=(const CVersionHelper& ver)
		{
			return Compare(ver) >= 0;
		}
		bool operator <=(const CVersionHelper& ver)
		{
			return Compare(ver) <= 0;
		}

	public:
		int	m_nMajorNumber;
		int m_nMinorNumber;
		int	m_nRevisionNumber;
		int m_nBuildNumber;
	};

}