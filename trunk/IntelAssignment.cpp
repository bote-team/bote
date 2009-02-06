#include "stdafx.h"
#include "IntelAssignment.h"
#include "MajorRace.h"

IMPLEMENT_SERIAL (CIntelAssignment, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CIntelAssignment::CIntelAssignment(void)
{
	Reset();
}

CIntelAssignment::~CIntelAssignment(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CIntelAssignment::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		for (int i = 0; i < DOMINION; i++)
		{
			for (int j = 0; j < 2; j++)
				ar << m_byPercentage[i][j];
			for (int j = 0; j < 4; j++)
			{
				ar << m_bySpyPercentage[i][j];
				ar << m_bySabPercentage[i][j];
			}
		}
	}
	else if (ar.IsLoading())
	{
		for (int i = 0; i < DOMINION; i++)
		{
			for (int j = 0; j < 2; j++)
				ar >> m_byPercentage[i][j];
			for (int j = 0; j < 4; j++)
			{
				ar >> m_bySpyPercentage[i][j];
				ar >> m_bySabPercentage[i][j];
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion gibt die globale prozentuale Zuteilung auf die innere Sicherheit zurück.
BYTE CIntelAssignment::GetInnerSecurityPercentage() const
{
	int perc = 100;
	for (int i = 0; i < DOMINION; i++)
		perc -= (m_byPercentage[i][0] + m_byPercentage[i][1]);
	return perc;
}

/// Funktion gibt die einzelnen Spionagezuteilungen für eine bestimmte Rasse zurück.
BYTE CIntelAssignment::GetSpyPercentages(BYTE race, BYTE type) const
{
	ASSERT(race);

	if (type != 4)
		return m_bySpyPercentage[race-1][type];
	else
		return 100 - (m_bySpyPercentage[race-1][0] + m_bySpyPercentage[race-1][1] + m_bySpyPercentage[race-1][2] +
			m_bySpyPercentage[race-1][3]);
}

/// Funktion gibt die einzelnen Sabotagezuteilungen für eine bestimmte Rasse zurück.
BYTE CIntelAssignment::GetSabotagePercentages(BYTE race, BYTE type) const
{
	ASSERT(race);

	if (type != 4)
		return m_bySabPercentage[race-1][type];
	else
		return 100 - (m_bySabPercentage[race-1][0] + m_bySabPercentage[race-1][1] + m_bySabPercentage[race-1][2] +
			m_bySabPercentage[race-1][3]);
}

/// Funktion ändert die globale prozentuale Zuteilung der einzelnen Geheimdienstressorts. Dabei wird wenn
/// nötig die Zuteilung der anderen Ressorts verändert.
void CIntelAssignment::SetGlobalPercentage(BYTE type, BYTE perc, CMajorRace* major, BYTE race, BYTE ownRace)
{
	BYTE known = 0;
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		if (i != ownRace && major->GetKnownMajorRace(i))
			known++;
		// kennen wir die Rasse nicht, haben aber irgendwelche Prozentpunkte in diesem Bereich verteilt, so müssen
		// diese zuvor entfernt werden. Dies kann vorkommen, wenn wir die Rasse vergeheimdiensteln, sie danach
		// aber komplett aus dem Spiel ausscheidet
		else if (m_byPercentage[i-1][0] > 0)
			m_byPercentage[i-1][0] = 0;
		else  if (m_byPercentage[i-1][1] > 0)
			m_byPercentage[i-1][1] = 0;
	}
	
	// wenn die innere Sicherheitszuweisung verändert wird
	if (type == 2 && known > 0)
	{
		int percentages[DOMINION][2];
		for (int i = 0; i < DOMINION; i++)
		{
			percentages[i][0] = m_byPercentage[i][0];
			percentages[i][1] = m_byPercentage[i][1];
		}
		
		// solange durchgehen, bis der richtige Prozentwert erreicht wurde
		do
		{
			int diff = GetInnerSecurityPercentage() - perc;
			int t = diff / (known * 2);
			int rest = diff - t * (known * 2);
			// gleichen Anteil auf alle Gebiete von bekannten Rassen verteilen
			for (int i = HUMAN; i <= DOMINION; i++)
				if (i != ownRace && major->GetKnownMajorRace(i))
					for (int j = 0; j < 2; j++)
						percentages[i-1][j] += t;				
			// bleibt noch ein Rest übrig (z.B. durch ungerade Anzahl), diesen zufällig auf ein Gebiet verteilen
			if (rest != 0)
			{
				int random = ownRace;
				while (random == ownRace || major->GetKnownMajorRace(random) == FALSE)
					random = rand()%DOMINION+1;
				percentages[random-1][rand()%2] += rest;
			}
		
			// überprüfen das nirgends mehr als 100% oder weniger als 0% vorhanden sind
			for (int i = 0; i < DOMINION; i++)
				for (int j = 0; j < 2; j++)
				{
					if (percentages[i][j] < 0)
						percentages[i][j] = 0;
					else if (percentages[i][j] > 100)
						percentages[i][j] = 100;
				}

			for (int i = 0; i < DOMINION; i++)
			{
				m_byPercentage[i][0] = percentages[i][0];
				m_byPercentage[i][1] = percentages[i][1];
			}
		} while (GetInnerSecurityPercentage() != perc);
	}

	// wenn irgendein Spionagebalken verändert wird
	else if (type != 2 && major->GetKnownMajorRace(race) && race != ownRace)
	{
		BYTE innerSecurity = GetInnerSecurityPercentage();
		if (perc - m_byPercentage[race-1][type] > innerSecurity)
			perc = m_byPercentage[race-1][type] + innerSecurity;
		m_byPercentage[race-1][type] = perc;
	}
}

/// Funktion ändert die genaueen Zuteilungen bei der Spionage. Also wieviel Prozent der Spionageabteilung
/// gehen z.B. in die Wirtschaftsspionage. Nicht vergebene Prozentpunkte befinden sich automatisch im
/// Spionagedepot für die jeweilige Rasse.
void CIntelAssignment::SetSpyPercentage(BYTE type, BYTE perc, BYTE race)
{
	ASSERT(race);
	if (type == 4)
	{
		int percentages[4];
		for (int i = 0; i < 4; i++)
			percentages[i] = m_bySpyPercentage[race-1][i];
		
		// solange durchgehen, bis der richtige Prozentwert erreicht wurde
		do
		{
			int diff = this->GetSpyPercentages(race, 4);
			diff -= perc;
			int t = diff / 4;
			int rest = diff - t * 4;
			// gleichen Anteil auf alle Gebiete von bekannten Rassen verteilen
			for (int i = 0; i < 4; i++)
				percentages[i] += t;				
			// bleibt noch ein Rest übrig (z.B. durch ungerade Anzahl), diesen zufällig auf ein Gebiet verteilen
			if (rest != 0)
			{
				int random = rand()%4;
				percentages[random] += rest;
			}
		
			// überprüfen das nirgends mehr als 100% oder weniger als 0% vorhanden sind
			for (int i = 0; i < 4; i++)
			{
				if (percentages[i] < 0)
					percentages[i] = 0;
				else if (percentages[i] > 100)
					percentages[i] = 100;
			}
			for (int i = 0; i < 4; i++)
				m_bySpyPercentage[race-1][i] = percentages[i];

		} while (this->GetSpyPercentages(race, 4) != perc);
	}

	// wenn irgendein spezieller Spionagebalken (außer Spionagedepot) verändert wird
	else if (type != 4)
	{
		BYTE spyDepot = this->GetSpyPercentages(race, 4);
		if (perc - m_bySpyPercentage[race-1][type] > spyDepot)
			perc = m_bySpyPercentage[race-1][type] + spyDepot;
		m_bySpyPercentage[race-1][type] = perc;
	}
}

/// Funktion ändert die genauen Zuteilungen bei der Sabotage. Also wieviel Prozent der Sabotageabteilung
/// gehen z.B. in die Wirtschaftssabotage. Nicht vergebene Prozentpunkte befinden sich automatisch im
/// Sabotagedepot für die jeweilige Rasse.
void CIntelAssignment::SetSabotagePercentage(BYTE type, BYTE perc, BYTE race)
{
	ASSERT(race);
	if (type == 4)
	{
		int percentages[4];
		for (int i = 0; i < 4; i++)
			percentages[i] = m_bySabPercentage[race-1][i];
		
		// solange durchgehen, bis der richtige Prozentwert erreicht wurde
		do
		{
			int diff = this->GetSabotagePercentages(race, 4);
			diff -= perc;
			int t = diff / 4;
			int rest = diff - t * 4;
			// gleichen Anteil auf alle Gebiete von bekannten Rassen verteilen
			for (int i = 0; i < 4; i++)
				percentages[i] += t;				
			// bleibt noch ein Rest übrig (z.B. durch ungerade Anzahl), diesen zufällig auf ein Gebiet verteilen
			if (rest != 0)
			{
				int random = rand()%4;
				percentages[random] += rest;
			}
		
			// überprüfen das nirgends mehr als 100% oder weniger als 0% vorhanden sind
			for (int i = 0; i < 4; i++)
			{
				if (percentages[i] < 0)
					percentages[i] = 0;
				else if (percentages[i] > 100)
					percentages[i] = 100;
			}
			for (int i = 0; i < 4; i++)
				m_bySabPercentage[race-1][i] = percentages[i];

		} while (this->GetSabotagePercentages(race,4) != perc);
	}

	// wenn irgendein spezieller Spionagebalken (außer Spionagedepot) verändert wird
	else if (type != 4)
	{
		BYTE sabDepot = this->GetSabotagePercentages(race,4);
		if (perc - m_bySabPercentage[race-1][type] > sabDepot)
			perc = m_bySabPercentage[race-1][type] + sabDepot;
		m_bySabPercentage[race-1][type] = perc;
	}
}

/// Resetfunktion für das CIntelligence-Objekt.
void CIntelAssignment::Reset()
{
	for (int i = 0; i < DOMINION; i++)
	{
		memset(m_byPercentage[i], 0, sizeof(*m_byPercentage[i])*2);
		memset(m_bySpyPercentage[i], 0, sizeof(*m_bySpyPercentage[i])*4);
		memset(m_bySabPercentage[i], 0, sizeof(*m_bySabPercentage[i])*4);
	}
}