#include "StdAfx.h"
#include "RequestRace.h"

namespace network
{
	CRequestRace::CRequestRace() : CMessage(MSGID_REQUESTRACE), m_nRace(RACE_NONE)
	{
	}

	CRequestRace::CRequestRace(RACE nRace) : CMessage(MSGID_REQUESTRACE), m_nRace(nRace)
	{
	}

	CRequestRace::~CRequestRace()
	{
	}

	void CRequestRace::Serialize(CArchive &ar)
	{
		if (ar.IsStoring())
			ar << (BYTE)m_nRace;
		else
		{
			BYTE nRace;
			ar >> nRace;
			m_nRace = (RACE)nRace;
		}
	}

};
