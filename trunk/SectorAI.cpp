#include "stdafx.h"
#include "SectorAI.h"
#include "Botf2Doc.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CSectorAI::CSectorAI(CBotf2Doc* pDoc)
{
	ASSERT(pDoc);
	m_pDoc = pDoc;

	for (int i = 0; i < UNKNOWN; i++)
	{
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			{
				m_iDangers[i][x][y] = 0;
				m_iCombatShipDangers[i][x][y] = 0;
			}
		if (i != UNKNOWN)
			m_HighestShipDanger[i] = CPoint(-1,-1);
	}
	memset(m_iCompleteDanger, 0, sizeof(*m_iCompleteDanger) * DOMINION);
	memset(m_iColoShips, 0, sizeof(*m_iColoShips) * DOMINION);
	memset(m_iTransportShips, 0, sizeof(*m_iTransportShips) * DOMINION);
}

CSectorAI::~CSectorAI(void)
{
	for (int i = 0; i < DOMINION; i++)
	{
		m_SectorsToTerraform[i].RemoveAll();
		m_MinorraceSectors[i].RemoveAll();
		m_OffensiveTargets[i].RemoveAll();
		m_BombardTargets[i].RemoveAll();
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Diese Funktion gibt das gesamte Gefahrenpotenzial aller Rassen in einem Sektor <code>sector</code> zurück.
/// Das Gefahrenpotential der eigenen Rasse <code>ownRace</code> wird dabei aber nicht mit eingerechnet.
UINT CSectorAI::GetCompleteDanger(BYTE ownRace, CPoint sector) const
{
	ASSERT(ownRace);
	UINT danger = 0;
	for (int i = 0; i < UNKNOWN; i++)
		if (i != (ownRace-1))
			danger += m_iDangers[i][sector.x][sector.y];
	return danger;
}

/// Diese Funktion trägt die ganzen Gefahrenpotenziale der Schiffe und Stationen in die Variable
/// <code>m_iDangers</code> ein. Außerdem wird hier auch gleich die Anzahl der verschiedenen Schiffe
/// der einzelnen Rassen ermittelt.
void CSectorAI::CalculateDangers()
{
	for (int i = 0; i < m_pDoc->m_ShipArray.GetSize(); i++)
	{
		CShip* ship = &m_pDoc->m_ShipArray.GetAt(i);
		AddDanger(ship);
		// Führt das Schiff eine Flotte an, so muss dies alles auch für die Schiffe in der Flotte getan werden
		if (ship->GetFleet() != 0)
			for (int j = 0; j < ship->GetFleet()->GetFleetSize(); j++)
				AddDanger(ship->GetFleet()->GetPointerOfShipFromFleet(j));
	}
}

/// Diese Funktion berechnet die ganzen Wichtigkeiten für die einzelnen Sektoren. Also wo lohnt es sich zum
/// Terraformen, wo lernt man neue Minorraces kennen usw. Vorher sollte die Funktion <code>CalculateDangers()
/// </code> aufgerufen werden.
void CSectorAI::CalcualteSectorPriorities()
{
	UINT highestCombatShipDanger[DOMINION] = {0};
	for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		{
			// Gibt es ein Sonnensystem im Sektor?
			if (m_pDoc->m_Sector[x][y].GetSunSystem())
			{
				CalculateTerraformSectors(x,y);
				CalculateMinorraceSectors(x,y);
				
			}
			CalculateOffensiveTargets(x,y);
			for (int i = 0; i < DOMINION; i++)
				if (m_iCombatShipDangers[i][x][y] > highestCombatShipDanger[i])
				{
					highestCombatShipDanger[i] = m_iCombatShipDangers[i][x][y];
					m_HighestShipDanger[i] = CPoint(x,y);
				}
		}

	for (int i = HUMAN; i <= DOMINION; i++)
	{
		// Feld der am ehesten zu terraformenden Systeme der Größe nach Sortieren. Der höchste Eintrag steht an erster Stelle.
		c_arraysort<CArray<SectorToTerraform>,SectorToTerraform>(m_SectorsToTerraform[i-1],sort_desc);

		// Sektor für den Bau eines Außenpostens berechnen
		CalculateStationTargets(i);
#ifdef TRACE_AI
		if (m_SectorsToTerraform[i-1].GetSize())
		{
			TRACE("\n---------- sectors to terraform or colinize -------------\n");
			TRACE("RACE: %d\n",i);
			for (int j = 0; j < m_SectorsToTerraform[i-1].GetSize(); j++)
				TRACE("POP: %d - KO: %d,%d\n",m_SectorsToTerraform[i-1].GetAt(j).pop,m_SectorsToTerraform[i-1].GetAt(j).p.x,m_SectorsToTerraform[i-1].GetAt(j).p.y);
		}
		if (m_OffensiveTargets[i-1].GetSize())
		{
			TRACE("---------- sectors to attack -------------\n");
			TRACE("Offensivziel - Rasse %d\n",i);
			for (int j = 0; j < m_OffensiveTargets[i-1].GetSize(); j++)
				TRACE("Offensivziel Sektor %d/%d\n",m_OffensiveTargets[i-1].GetAt(j).x,m_OffensiveTargets[i-1].GetAt(j).y);
		}
		if (m_BombardTargets[i-1].GetSize())
		{
			TRACE("---------- sectors to bombard -------------\n");
			TRACE("Bombardierungsziel - Rasse %d\n",i);
			for (int j = 0; j < m_BombardTargets[i-1].GetSize(); j++)
				TRACE("Bombardierungsziel Sektor %d/%d\n",m_BombardTargets[i-1].GetAt(j).x, m_BombardTargets[i-1].GetAt(j).y);
		}
#endif
	}

}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
/// Diese Funktion addiert die Offensiv- und Defensivstärke eines Schiffes einer Rasse zum jeweiligen
/// Sektor.
void CSectorAI::AddDanger(CShip* ship)
{
	BYTE race = ship->GetOwnerOfShip();
	ASSERT(race);

	UINT offensive = ship->GetCompleteOffensivePower();
	UINT defensive = ship->GetCompleteDefensivePower();
	m_iDangers[race-1][ship->GetKO().x][ship->GetKO().y] += (offensive + defensive);
	
	if (ship->GetShipType() > COLONYSHIP && ship->GetShipType() < OUTPOST)
	{
		if (race != UNKNOWN)
			m_iCompleteDanger[race-1] += (offensive + defensive);
		m_iCombatShipDangers[race-1][ship->GetKO().x][ship->GetKO().y] += (offensive + defensive);
	}
	
	// Hier wird die Anzahl an Kolonieschiffen für die Rassen hochgezählt.
	if (race >= HUMAN && race <= DOMINION)
	{
		if (ship->GetShipType() == COLONYSHIP)
			m_iColoShips[race-1]++;
		else if (ship->GetShipType() == TRANSPORTER)
			m_iTransportShips[race-1]++;
	}
}

/// Diese Funktion ermittelt die Sektoren, welche sich am ehesten zum Terraformen für eine bestimmte Rasse eignen.
/// Die Einträge werden dann im Array <code>m_SectorsToTerraform</code> gemacht.
void CSectorAI::CalculateTerraformSectors(int x, int y)
{
	// Gehört der Sektor aktuell auch keiner Minorrace
	if (m_pDoc->m_Sector[x][y].GetOwnerOfSector() != UNKNOWN)
	{
		BYTE pop = 0;
		// wieviel Bevölkerung kann man noch ins System bringen
		for (int j = 0; j < m_pDoc->m_Sector[x][y].GetNumberOfPlanets(); j++)
			if (m_pDoc->m_Sector[x][y].GetPlanet(j)->GetHabitable() == TRUE
				&& m_pDoc->m_Sector[x][y].GetPlanet(j)->GetColonized() == FALSE)
				pop += (BYTE)m_pDoc->m_Sector[x][y].GetPlanet(j)->GetMaxHabitant();
		if (pop > 5)
			// Eintrag für die jeweilige Rasse machen.
			for (int i = HUMAN; i <= DOMINION; i++)
				if (m_pDoc->starmap[i]->GetRange(CPoint(x,y)) != 3)
					if (m_pDoc->m_Sector[x][y].GetOwnerOfSector() == NOBODY || m_pDoc->m_Sector[x][y].GetOwnerOfSector() == i)
					{
						SectorToTerraform stt(pop,CPoint(x,y));
						m_SectorsToTerraform[i-1].Add(stt);
					}
	}	
}

/// Funktion berechnet die Sektoren, in denen eine einem Imperium unbekannte Minorrace lebt, zu deren Sektor
/// aber theoretisch geflogen werden kann. Das Ergebnis wird im Array <code>m_MinorraceSectors</code> gespeichert.
void CSectorAI::CalculateMinorraceSectors(int x, int y)
{
	// Gehört der Sektor aktuell auch einer Minorrace
		// Wenn die Minorrace einem anderen Imperium beigetreten ist, so tritt folgende Bediengnung nicht ein!.
		// Dann fliegt die KI diesen Sektor nicht bevorzugt an, was so auch realistischer ist.
	if (m_pDoc->m_Sector[x][y].GetOwnerOfSector() == UNKNOWN)
	{
		CMinorRace *minor = m_pDoc->GetMinorRace(m_pDoc->m_Sector[x][y].GetName());
		ASSERT(minor);
		// Eintrag für die jeweilige Rasse machen.
		for (int i = HUMAN; i <= DOMINION; i++)
			if (m_pDoc->starmap[i]->GetRange(CPoint(x,y)) != 3)
				if (minor->GetKnownByMajorRace(i) == FALSE)
					m_MinorraceSectors[i-1].Add(minor->GetRaceKO());					
	}
}

/// Diese Funktion berechnet alle möglichen offensiven Ziele für eine bestimmte Rasse. Das Ergebnis wird im Array
/// <code>m_OffensiveTargets</code> gespeichert.
void CSectorAI::CalculateOffensiveTargets(int x, int y)
{
	for (int i = HUMAN; i <= DOMINION; i++)
		// Wenn unsere Rasse dieses Feld überhaupt erreichen kann.
		if (m_pDoc->starmap[i]->GetRange(CPoint(x,y)) != 3)
		{
			int danger = GetCompleteDanger(i, CPoint(x,y));
			// Die Gefahr wird nur hinzugefügt, wenn diese auf MEINEM Gebiet liegt und wir mit der entsprechenden
			// Rasse Krieg oder unsere Beziehung zu ihr auf 50% gefallen ist. Die andere Möglichkeit
			// wäre, wenn diese nicht in unserem Gebiet liegt und wir Krieg haben oder die Beziehung auf
			// unter 30% gefallen ist.
			if (danger > 0)
			{
				// den stärksten Gegner in diesem Sektor ermitteln
				BYTE enemy = 0;
				UINT max = 0;
				for (int j = HUMAN; j <= DOMINION; j++)
					if (j != i && GetDanger(j, CPoint(x,y)) > 0)
						if (max < GetDanger(j, CPoint(x,y)))
						{
							max = GetDanger(j, CPoint(x,y));
							enemy = j;
						}
				// kennen wir den Gegner?
				if (m_pDoc->m_MajorRace[i].GetKnownMajorRace(enemy))
				{
					// prüfen ob es auf unserem eigenen Gebiet ist
					if (m_pDoc->m_Sector[x][y].GetOwnerOfSector() == i)
					{
						// jetzt wird überprüft, ob obige Bedingungen gelten
						if (m_pDoc->m_MajorRace[i].GetRelationshipToMajorRace(enemy) < 50 || m_pDoc->m_MajorRace[i].GetDiplomacyStatus(enemy) == WAR)
							// Gefahr wird dem Feld hinzugeügt
							m_OffensiveTargets[i-1].Add(CPoint(x,y));
					}
					// wenn es nicht auf unserem Gebiet ist
					else
					{
						// jetzt wird überprüft, ob obige Bedingungen gelten
						if (m_pDoc->m_MajorRace[i].GetRelationshipToMajorRace(enemy) < 30 || m_pDoc->m_MajorRace[i].GetDiplomacyStatus(enemy) == WAR)
							// Gefahr wird dem Feld hinzugeügt
							m_OffensiveTargets[i-1].Add(CPoint(x,y));
					}
				}
			}
			CalculateBombardTargets(i,x,y);
		}
}

/// Diese Funktion berechnet Systeme, welche im Kriegsfall womöglich angegriffen werden können. Das Ergebnis wird
/// im Array <code>m_BombardTargets</code> gespeichert.
void CSectorAI::CalculateBombardTargets(BYTE race, int x, int y)
{
	// gehört das System einer anderen Majorrace, außer uns selbst?
	if (m_pDoc->m_System[x][y].GetOwnerOfSystem() != race && m_pDoc->m_System[x][y].GetOwnerOfSystem() != UNKNOWN &&  m_pDoc->m_System[x][y].GetOwnerOfSystem() != NOBODY)
		// haben wir mit dieser anderen Majorrace Krieg?
		if (m_pDoc->m_MajorRace[race].GetDiplomacyStatus(m_pDoc->m_System[x][y].GetOwnerOfSystem()) == WAR)
			// dann wäre dies ein lohnendes Ziel, welches angegriffen werden könnte
			m_BombardTargets[race-1].Add(CPoint(x,y));
}

/// Diese Funktion berechnet einen Sektor, welcher sich zum Bau eines Außenpostens eignet.
void CSectorAI::CalculateStationTargets(BYTE race)
{
	m_StationBuild[race-1] = m_pDoc->starmap[race]->CalcAIBaseSector(0.0f);
}