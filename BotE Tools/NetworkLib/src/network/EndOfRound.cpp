#include "StdAfx.h"
#include "EndOfRound.h"

#pragma warning (disable: 4244)

namespace network
{
	CEndOfRound::CEndOfRound() : CMessage(MSGID_ENDOFROUND), m_nSize(0), m_pData(NULL)
	{
	}

	CEndOfRound::CEndOfRound(CPeerData *pDoc) : CMessage(MSGID_ENDOFROUND), m_nSize(0), m_pData(NULL)
	{
		ASSERT(pDoc);

		// Dokument serialisieren
		CMemFile memFile;
		CArchive ar(&memFile, CArchive::store);
		pDoc->SerializeEndOfRoundData(ar, RACE_NONE);
		ar.Flush();
		m_nSize = memFile.GetLength();
		m_pData = memFile.Detach();
	}

	CEndOfRound::~CEndOfRound()
	{
		if (m_pData) delete[] m_pData;
	}

	void CEndOfRound::Serialize(CArchive &ar)
	{
		if (ar.IsStoring())
		{
			ar << m_nSize;
			ar.Write(m_pData, m_nSize);
		}
		else
		{
			ar >> m_nSize;
			if (m_pData) delete[] m_pData;
			m_pData = new BYTE[m_nSize];
			ar.Read(m_pData, m_nSize);
		}
	}

	BYTE *CEndOfRound::Detach()
	{
		m_nSize = 0;
		BYTE *result = m_pData;
		m_pData = NULL;
		return result;
	}

};
