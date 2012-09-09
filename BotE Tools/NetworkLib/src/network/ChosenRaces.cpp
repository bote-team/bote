#include "StdAfx.h"
#include "ChosenRaces.h"

namespace network
{
	CChosenRaces::CChosenRaces() : CMessage(MSGID_CHOSENRACES), m_nClientRace(RACE_NONE)
	{
		memset(m_pPlayers, 0, RACE_COUNT * sizeof(*m_pPlayers));
	}

	CChosenRaces::CChosenRaces(RACE nClientRace)
		: CMessage(MSGID_CHOSENRACES), m_nClientRace(nClientRace)
	{
		ASSERT(nClientRace < RACE_LAST);
		memset(m_pPlayers, 0, RACE_COUNT * sizeof(*m_pPlayers));
	}

	CChosenRaces::~CChosenRaces()
	{
	}

	void CChosenRaces::Serialize(CArchive &ar)
	{
		if (ar.IsStoring())
		{
			ar << (BYTE)m_nClientRace;
			for (int i = 0; i < RACE_COUNT; i++)
				ar << (BYTE)m_pPlayers[i];
			for (int i = 0; i < RACE_COUNT; i++)
				ar << m_pstrUserNames[i];
		}
		else
		{
			BYTE nClientRace;
			ar >> nClientRace;
			if (RACE_FIRST <= nClientRace && nClientRace < RACE_LAST)
				m_nClientRace = (RACE)nClientRace;
			else
				m_nClientRace = RACE_NONE;

			memset(m_pPlayers, 0, RACE_COUNT * sizeof(*m_pPlayers));
			for (int i = 0; i < RACE_COUNT; i++)
			{
				BYTE player;
				ar >> player;
				if (player == PLAYER_NONE || player == PLAYER_SERVER || player == PLAYER_CLIENT)
					m_pPlayers[i] = (PLAYER)player;
			}

			for (int i = 0; i < RACE_COUNT; i++)
				ar >> m_pstrUserNames[i];
		}
	}

	void CChosenRaces::SetPlayedByClient(RACE nRace, const CString &strUserName)
	{
		if (nRace >= RACE_FIRST && nRace < RACE_LAST)
		{
			m_pPlayers[nRace - RACE_FIRST] = PLAYER_CLIENT;
			m_pstrUserNames[nRace - RACE_FIRST] = strUserName;
		}
	}

	void CChosenRaces::SetPlayedByServer(RACE nRace)
	{
		if (nRace >= RACE_FIRST && nRace < RACE_LAST)
		{
			m_pPlayers[nRace - RACE_FIRST] = PLAYER_SERVER;
			m_pstrUserNames[nRace - RACE_FIRST] = "";
		}
	}

	BOOL CChosenRaces::IsPlayer(RACE nRace)
	{
		if (nRace >= RACE_FIRST && nRace < RACE_LAST)
			return (m_pPlayers[nRace - RACE_FIRST] != PLAYER_NONE);
		else
			return FALSE;
	}

	BOOL CChosenRaces::IsPlayedByClient(RACE nRace)
	{
		if (nRace >= RACE_FIRST && nRace < RACE_LAST)
			return (m_pPlayers[nRace - RACE_FIRST] == PLAYER_CLIENT);
		else
			return FALSE;
	}

	BOOL CChosenRaces::IsPlayedByServer(RACE nRace)
	{
		if (nRace >= RACE_FIRST && nRace < RACE_LAST)
			return (m_pPlayers[nRace - RACE_FIRST] == PLAYER_SERVER);
		else
			return FALSE;
	}

	CString CChosenRaces::GetUserName(RACE nRace)
	{
		if (nRace >= RACE_FIRST && nRace < RACE_LAST)
			return m_pstrUserNames[nRace - RACE_FIRST];
		else
			return "";
	}

};
