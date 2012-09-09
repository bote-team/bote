#include "StdAfx.h"
#include "BeginGame.h"

#pragma warning (disable: 4244)

namespace network
{
	CBeginGame::CBeginGame() : CMessage(MSGID_BEGINGAME), m_nSize(0), m_pData(NULL)
	{
	}

	CBeginGame::CBeginGame(CPeerData *pDoc) : CMessage(MSGID_BEGINGAME), m_nSize(0), m_pData(NULL)
	{
		ASSERT(pDoc);

		// Dokument serialisieren
		CMemFile memFile;
		CArchive ar(&memFile, CArchive::store);
		pDoc->SerializeBeginGameData(ar);
		ar.Flush();
		m_nSize = memFile.GetLength();
		m_pData = memFile.Detach();
	}

	CBeginGame::~CBeginGame()
	{
		if (m_pData) delete[] m_pData;
	}

	void CBeginGame::Serialize(CArchive &ar)
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

	void CBeginGame::DeserializeToDoc(CPeerData *pDoc)
	{
		CMemFile memFile(m_pData, m_nSize);
		CArchive ar(&memFile, CArchive::load);
		pDoc->SerializeBeginGameData(ar);
		ar.Flush();
		memFile.Detach();
	}

};
