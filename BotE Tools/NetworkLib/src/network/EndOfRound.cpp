#include "StdAfx.h"
#include "EndOfRound.h"

#pragma warning (disable: 4244)

#include <cassert>

namespace network
{
	CEndOfRound::CEndOfRound() : CMessage(MSGID_ENDOFROUND), m_nSize(0), m_pDeletableData(NULL),
		m_pElseData(NULL)
	{
	}

	CEndOfRound::CEndOfRound(CPeerData *pDoc) : CMessage(MSGID_ENDOFROUND), m_nSize(0), m_pDeletableData(NULL),
		m_pElseData(NULL)
	{
		ASSERT(pDoc);

		// Dokument serialisieren
		CMemFile memFile;
		CArchive ar(&memFile, CArchive::store);
		pDoc->SerializeEndOfRoundData(ar, RACE_NONE);
		ar.Flush();
		m_nSize = memFile.GetLength();
		m_pElseData = memFile.Detach();
	}

	CEndOfRound::~CEndOfRound()
	{
		assert(!m_pDeletableData || !m_pElseData);
		if (m_pDeletableData)
		{
			delete[] m_pDeletableData;
			m_pDeletableData = NULL;
		}
		if(m_pElseData)
		{
			free(m_pElseData);
			m_pElseData = NULL;
		}
	}

	void CEndOfRound::Serialize(CArchive &ar)
	{
		if (ar.IsStoring())
		{
			ar << m_nSize;
			assert(m_pElseData && !m_pDeletableData);
			ar.Write(m_pElseData, m_nSize);
		}
		else
		{
			ar >> m_nSize;
			if (m_pDeletableData)
			{
				delete[] m_pDeletableData;
				m_pDeletableData = NULL;
			}
			m_pDeletableData = new BYTE[m_nSize];
			ar.Read(m_pDeletableData, m_nSize);
		}
	}

	BYTE *CEndOfRound::Detach()
	{
		assert(!m_pElseData && m_pDeletableData);
		m_nSize = 0;
		BYTE *result = m_pDeletableData;
		m_pDeletableData = NULL;
		return result;
	}

};
