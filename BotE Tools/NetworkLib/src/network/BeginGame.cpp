#include "StdAfx.h"
#include "BeginGame.h"

#pragma warning (disable: 4244)

#include <cassert>

namespace network
{
	CBeginGame::CBeginGame() : CMessage(MSGID_BEGINGAME), m_nSize(0), m_pDeletableData(NULL), m_pElseData(NULL)
	{
	}

	CBeginGame::CBeginGame(CPeerData *pDoc) : CMessage(MSGID_BEGINGAME), m_nSize(0),
		m_pDeletableData(NULL), m_pElseData(NULL)
	{
		ASSERT(pDoc);

		// Dokument serialisieren
		CMemFile memFile;
		CArchive ar(&memFile, CArchive::store);
		pDoc->SerializeBeginGameData(ar);
		ar.Flush();
		m_nSize = memFile.GetLength();
		m_pElseData = memFile.Detach();
	}

	CBeginGame::~CBeginGame()
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

	void CBeginGame::Serialize(CArchive &ar)
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

	void CBeginGame::DeserializeToDoc(CPeerData *pDoc)
	{
		assert(!m_pElseData && m_pDeletableData);
		CMemFile memFile(m_pDeletableData, m_nSize);
		CArchive ar(&memFile, CArchive::load);
		pDoc->SerializeBeginGameData(ar);
		ar.Flush();
		memFile.Detach();
	}

};
