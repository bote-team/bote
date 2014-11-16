#include "StdAfx.h"
#include "NextRound.h"

#pragma warning (disable: 4244)

#include <cassert>

namespace network
{
	CNextRound::CNextRound() : CMessage(MSGID_NEXTROUND), m_nSize(0), m_pDeletableData(NULL), m_pElseData(NULL)
	{
		memset(m_pPlayers, 0, RACE_COUNT * sizeof(*m_pPlayers));
	}

	CNextRound::CNextRound(CPeerData *pDoc)
		: CMessage(MSGID_NEXTROUND), m_nSize(0), m_pDeletableData(NULL), m_pElseData(NULL)
	{
		ASSERT(pDoc);
		memset(m_pPlayers, 0, RACE_COUNT * sizeof(*m_pPlayers));

		// Dokument serialisieren
		// TODO später zielspezifisch machen, senden erstmal allen Clients die kompletten Daten
		CMemFile memFile;
		CArchive ar(&memFile, CArchive::store);
		pDoc->SerializeNextRoundData(ar);
		ar.Flush();
		m_nSize = memFile.GetLength();
		m_pElseData = memFile.Detach();
	}

	CNextRound::~CNextRound()
	{
		assert(!m_pDeletableData || !m_pElseData);
		if(m_pDeletableData)
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

	void CNextRound::Serialize(CArchive &ar)
	{
		if (ar.IsStoring())
		{
			for (int i = 0; i < RACE_COUNT; i++)
				ar << (BYTE)m_pPlayers[i];
			ar << m_nSize;
			assert(m_pElseData && !m_pDeletableData);
			ar.Write(m_pElseData, m_nSize);
		}
		else
		{
			memset(m_pPlayers, 0, RACE_COUNT * sizeof(*m_pPlayers));
			for (int i = 0; i < RACE_COUNT; i++)
			{
				BYTE player;
				ar >> player;
				if (player == PLAYER_NONE || player == PLAYER_CLIENT || player == PLAYER_SERVER)
					m_pPlayers[i] = (PLAYER)player;
			}
			ar >> m_nSize;
			if(m_pDeletableData)
			{
				delete[] m_pDeletableData;
				m_pDeletableData = NULL;
			}
			m_pDeletableData = new BYTE[m_nSize];
			ar.Read(m_pDeletableData, m_nSize);
		}
	}

	BOOL CNextRound::IsPlayer(RACE race) const
	{
		if (race < RACE_FIRST || race >= RACE_LAST) return FALSE;
		return (m_pPlayers[race - RACE_FIRST] != PLAYER_NONE);
	}

	BOOL CNextRound::IsPlayedByClient(RACE race) const
	{
		if (race < RACE_FIRST || race >= RACE_LAST) return FALSE;
		return (m_pPlayers[race - RACE_FIRST] == PLAYER_CLIENT);
	}

	BOOL CNextRound::IsPlayedByServer(RACE race) const
	{
		if (race < RACE_FIRST || race >= RACE_LAST) return FALSE;
		return (m_pPlayers[race - RACE_FIRST] == PLAYER_SERVER);
	}

	void CNextRound::SetPlayedByClient(RACE race)
	{
		if (RACE_FIRST <= race && race < RACE_LAST)
			m_pPlayers[race - RACE_FIRST] = PLAYER_CLIENT;
	}

	void CNextRound::SetPlayedByServer(RACE race)
	{
		if (RACE_FIRST <= race && race < RACE_LAST)
			m_pPlayers[race - RACE_FIRST] = PLAYER_SERVER;
	}

	void CNextRound::DeserializeToDoc(CPeerData *pDoc)
	{
		assert(!m_pElseData && m_pDeletableData);
		CMemFile memFile(m_pDeletableData, m_nSize);
		CArchive ar(&memFile, CArchive::load);
		pDoc->SerializeNextRoundData(ar);
		ar.Flush();
		memFile.Detach();
	}

};
