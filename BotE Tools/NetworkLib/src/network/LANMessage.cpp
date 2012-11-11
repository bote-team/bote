#include "StdAfx.h"
#include "LANMessage.h"

namespace network
{
	CLANMessage::CLANMessage() : m_nID(LANID_NONE), m_dwSenderIP(0), m_nSenderPort(0),
		m_dwReceiverIP(0), m_nReceiverPort(0)
	{
	}

	CLANMessage::CLANMessage(BYTE nID) : m_nID(nID), m_dwSenderIP(0), m_nSenderPort(0),
		m_dwReceiverIP(0), m_nReceiverPort(0)
	{
	}

	CLANMessage::~CLANMessage()
	{
	}

	void CLANMessage::SetValue(const CString &param, const CString &value)
	{
		if (!param.IsEmpty()) m_params[param] = value;
	}

	void CLANMessage::SetValue(const CString &param, UINT value)
	{
		if (!param.IsEmpty())
		{
			CString v;
			v.Format("%u", value);
			m_params[param] = v;
		}
	}

	CString CLANMessage::GetStringValue(const CString &param) const
	{
		PARAM_MAP::const_iterator it = m_params.find(param);
		if (it != m_params.end())
			return (*it).second;
		else
			return "";
	}

	UINT CLANMessage::GetUINTValue(const CString &param) const
	{
		PARAM_MAP::const_iterator it = m_params.find(param);
		if (it != m_params.end())
			return strtoul((*it).second, NULL, 10);
		else
			return 0;
	}

	void CLANMessage::Serialize(CArchive &ar)
	{
		if (ar.IsStoring())
		{
			ar << m_nID;
			CString params = "";
			for (PARAM_MAP::const_iterator it = m_params.begin(); it != m_params.end(); ++it)
			{
				if (!params.IsEmpty()) params += "&";
				params += Encode((*it).first) + "=" + Encode((*it).second);
			}
			ar << params;
		}
		else
		{
			ar >> m_nID;
			CString params;
			ar >> params;
			m_params.clear();
			int nPos = 0;
			CString token = params.Tokenize("&", nPos);
			while (nPos > -1)
			{
				int nSplit = token.Find('=');
				if (nSplit > -1)
				{
					CString key = Decode(token.Mid(0, nSplit));
					CString value = Decode(token.Mid(nSplit + 1));
					if (!key.IsEmpty()) m_params[key] = value;
				}
				token = params.Tokenize("&", nPos);
			}
		}
	}

	CString CLANMessage::Encode(const CString &str)
	{
		CString result = str;
		result.Replace("%", "%0");
		result.Replace("=", "%1");
		result.Replace("&", "%2");
		return result;
	}

	CString CLANMessage::Decode(const CString &str)
	{
		CString result = str;
		result.Replace("%2", "&");
		result.Replace("%1", "=");
		result.Replace("%0", "%");
		return result;
	}

	CString CLANMessage::GetParamName(int idx) const
	{
		int i = 0;
		for (PARAM_MAP::const_iterator it = m_params.begin(); it != m_params.end(); it++, i++)
		{
			if (i == idx) return (*it).first;
		}
		return "";
	}

};
