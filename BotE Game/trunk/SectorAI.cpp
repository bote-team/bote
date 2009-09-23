#include "stdafx.h"
#include "SectorAI.h"
#include "Botf2Doc.h"
#include "RaceController.h"
#include "Fleet.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CSectorAI::CSectorAI(CBotf2Doc* pDoc)
{
	ASSERT(pDoc);
	m_pDoc = pDoc;
	
	Clear();
}

CSectorAI::~CSectorAI(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Diese Funktion gibt das gesamte Gefahrenpotenzial aller Rassen in einem Sektor <code>sector</code> zurück.
/// Das Gefahrenpotential der eigenen Rasse <code>sOwnRaceID</code> wird dabei aber nicht mit eingerechnet.
UINT CSectorAI::GetCompleteDanger(const CString& sOwnRaceID, const CPoint& sector)
{
	UINT danger = 0;
	
	for (map<CString, map<pair<int, int>, UINT> >::const_iterator it = m_iDangers.begin(); it != m_iDangers.end(); it++)
		if (it->first != sOwnRaceID)
			for (map<pair<int, int>, UINT>::const_iterator itt = it->second.begin(); itt != it->second.end(); itt++)
				if (CPoint(itt->first.first, itt->first.second) == sector)
					danger += itt->second;
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
	map<CString, CRace*>* mRaces = m_pDoc->GetRaceCtrl()->GetRaces();
	map<CString, UINT> highestCombatShipDanger;

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
			for (map<CString, CRace*>::const_iterator it = mRaces->begin(); it != mRaces->end(); it++)
				if (GetDangerOnlyFromCombatShips(it->first, CPoint(x,y)) > highestCombatShipDanger[it->first])
				{
					highestCombatShipDanger[it->first] = GetDangerOnlyFromCombatShips(it->first, CPoint(x,y));
					m_HighestShipDanger[it->first] = CPoint(x,y);
				}
		}

	for (map<CString, CRace*>::const_iterator it = mRaces->begin(); it != mRaces->end(); it++)
		if (it->second->GetType() == MAJOR)
		{
			// Feld der am ehesten zu terraformenden Systeme der Größe nach Sortieren. Der höchste Eintrag steht an erster Stelle.
			std::sort(m_vSectorsToTerraform[it->first].begin(), m_vSectorsToTerraform[it->first].end());
			std::reverse(m_vSectorsToTerraform[it->first].begin(), m_vSectorsToTerraform[it->first].end());			

			// Sektor für den Bau eines Außenpostens berechnen
			CalculateStationTargets(it->first);
#ifdef TRACE_AI
			if (m_vSectorsToTerraform[it->first].size())
			{
				MYTRACE(MT::LEVEL_INFO, "\n---------- sectors to terraform or colinize -------------\n");
				MYTRACE(MT::LEVEL_INFO, "Race-ID: %s\n",it->first);
				for (UINT j = 0; j < m_vSectorsToTerraform[it->first].size(); j++)
					MYTRACE(MT::LEVEL_INFO, "POP: %d - KO: %d,%d\n",m_vSectorsToTerraform[it->first][j].pop, m_vSectorsToTerraform[it->first][j].p.x, m_vSectorsToTerraform[it->first][j].p.y);
			}
			if (m_vOffensiveTargets[it->first].size())
			{
				MYTRACE(MT::LEVEL_INFO, "---------- sectors to attack -------------\n");
				MYTRACE(MT::LEVEL_INFO, "offensive targets for race %s\n",it->first);
				for (UINT j = 0; j < m_vOffensiveTargets[it->first].size(); j++)
					MYTRACE(MT::LEVEL_INFO, "offensive target in sector %d/%d\n", m_vOffensiveTargets[it->first][j].x, m_vOffensiveTargets[it->first][j].y);
			}
			if (m_vBombardTargets[it->first].size())
			{
				MYTRACE(MT::LEVEL_INFO, "---------- sectors to bombard -------------\n");
				MYTRACE(MT::LEVEL_INFO, "bombard target for race %s\n",it->first);
				for (UINT j = 0; j < m_vBombardTargets[it->first].size(); j++)
					MYTRACE(MT::LEVEL_INFO, "bombard target in sector %d/%d\n",m_vBombardTargets[it->first][j].x, m_vBombardTargets[it->first][j].y);
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
	CString race = ship->GetOwnerOfShip();
	
	UINT offensive = ship->GetCompleteOffensivePower();
	UINT defensive = ship->GetCompleteDefensivePower();
	m_iDangers[race][pair<int, int>(ship->GetKO().x, ship->GetKO().y)] += (offensive + defensive);
	
	if (ship->GetShipType() > COLONYSHIP && ship->GetShipType() < OUTPOST)
	{
		m_iCompleteDanger[race] += (offensive + defensive);
		m_iCombatShipDangers[race][pair<int, int>(ship->GetKO().x, ship->GetKO().y)] += (offensive + defensive);
	}
	
	// Hier wird die Anzahl an Kolonieschiffen für die Rassen hochgezählt.
	if (ship->GetShipType() == COLONYSHIP)
		m_iColoShips[race] += 1;
	else if (ship->GetShipType() == TRANSPORTER)
		m_iTransportShips[race] += 1;	
}

/// Diese Funktion ermittelt die Sektoren, welche sich am ehesten zum Terraformen für eine bestimmte Rasse eignen.
/// Die Einträge werden dann im Array <code>m_vSectorsToTerraform</code> gemacht.
void CSectorAI::CalculateTerraformSectors(int x, int y)
{
	BYTE pop = 0;
	// wieviel Bevölkerung kann man noch ins System bringen
	for (int j = 0; j < m_pDoc->m_Sector[x][y].GetNumberOfPlanets(); j++)
		if (m_pDoc->m_Sector[x][y].GetPlanet(j)->GetHabitable() == TRUE
			&& m_pDoc->m_Sector[x][y].GetPlanet(j)->GetColonized() == FALSE)
			pop += (BYTE)m_pDoc->m_Sector[x][y].GetPlanet(j)->GetMaxHabitant();
	
	if (pop > 5)
	{
		// Eintrag für die jeweilige Rasse machen.
		map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();	
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
			if (it->second->GetStarmap()->GetRange(CPoint(x,y)) != 3)
				if (m_pDoc->m_Sector[x][y].GetOwnerOfSector().IsEmpty() || m_pDoc->m_Sector[x][y].GetOwnerOfSector() == it->first)
				{
					SectorToTerraform stt(pop,CPoint(x,y));
					m_vSectorsToTerraform[it->first].push_back(stt);
				}
	}	
}

/// Funktion berechnet die Sektoren, in denen eine einem Imperium unbekannte Minorrace lebt, zu deren Sektor
/// aber theoretisch geflogen werden kann. Das Ergebnis wird im Array <code>m_vMinorraceSectors</code> gespeichert.
void CSectorAI::CalculateMinorraceSectors(int x, int y)
{
	// Gehört der Sektor aktuell auch einer Minorrace
		// Wenn die Minorrace einem anderen Imperium beigetreten ist, so tritt folgende Bediengnung nicht ein!.
		// Dann fliegt die KI diesen Sektor nicht bevorzugt an, was so auch realistischer ist.
	CString sOwner	= m_pDoc->m_Sector[x][y].GetOwnerOfSector();
	if (sOwner.IsEmpty())
		return;

	CRace* pOwner	= m_pDoc->GetRaceCtrl()->GetRace(sOwner);
	if (!pOwner || pOwner->GetType() != MINOR)
		return;

	CMinor* pMinor = dynamic_cast<CMinor*>(pOwner);
	ASSERT(pMinor);

	// Eintrag für die jeweilige Rasse machen.
	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();	
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
			if (it->second->GetStarmap()->GetRange(CPoint(x,y)) != 3)
				if (it->second->IsRaceContacted(sOwner) == false)
					m_vMinorraceSectors[it->first].push_back(pMinor->GetRaceKO());
}

/// Diese Funktion berechnet alle möglichen offensiven Ziele für eine bestimmte Rasse. Das Ergebnis wird im Array
/// <code>m_vOffensiveTargets</code> gespeichert.
void CSectorAI::CalculateOffensiveTargets(int x, int y)
{
	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();	
	
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
		// Wenn unsere Rasse dieses Feld überhaupt erreichen kann.
		if (it->second->GetStarmap()->GetRange(CPoint(x,y)) != 3)
		{
			int danger = GetCompleteDanger(it->first, CPoint(x,y));
			// Die Gefahr wird nur hinzugefügt, wenn diese auf MEINEM Gebiet liegt und wir mit der entsprechenden
			// Rasse Krieg oder unsere Beziehung zu ihr auf 50% gefallen ist. Die andere Möglichkeit
			// wäre, wenn diese nicht in unserem Gebiet liegt und wir Krieg haben oder die Beziehung auf
			// unter 30% gefallen ist.
			if (danger > 0)
			{
				// den stärksten Gegner in diesem Sektor ermitteln
				CString sEnemy = 0;
				UINT max = 0;
				for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); itt++)
					if (it->first != itt->first && GetDanger(itt->first, CPoint(x,y)) > 0)
						if (max < GetDanger(itt->first, CPoint(x,y)))
						{
							max = GetDanger(itt->first, CPoint(x,y));
							sEnemy = itt->first;
						}
				// kennen wir den Gegner?
				if (it->second->IsRaceContacted(sEnemy))
				{
					// prüfen ob es auf unserem eigenen Gebiet ist
					if (m_pDoc->m_Sector[x][y].GetOwnerOfSector() == it->first)
					{
						// jetzt wird überprüft, ob obige Bedingungen gelten
						if (it->second->GetRelation(sEnemy) < 50 || it->second->GetAgreement(sEnemy) == WAR)
							// Gefahr wird dem Feld hinzugeügt
							m_vOffensiveTargets[it->first].push_back(CPoint(x,y));
					}
					// wenn es nicht auf unserem Gebiet ist
					else
					{
						// jetzt wird überprüft, ob obige Bedingungen gelten
						if (it->second->GetRelation(sEnemy) < 30 || it->second->GetAgreement(sEnemy) == WAR)
							// Gefahr wird dem Feld hinzugeügt
							m_vOffensiveTargets[it->first].push_back(CPoint(x,y));
					}
				}
			}
			CalculateBombardTargets(it->first,x,y);
		}
}

/// Diese Funktion berechnet Systeme, welche im Kriegsfall womöglich angegriffen werden können. Das Ergebnis wird
/// im Array <code>m_vBombardTargets</code> gespeichert.
void CSectorAI::CalculateBombardTargets(const CString& sRaceID, int x, int y)
{
	CString sOwner	= m_pDoc->m_System[x][y].GetOwnerOfSystem();
	if (sOwner.IsEmpty())
		return;
	CRace* pOwner	= m_pDoc->GetRaceCtrl()->GetRace(sOwner);
	// wenn das System nicht einem anderen Major gehört
	if (!pOwner || pOwner->GetType() != MAJOR)
		return;

	// gehört das System einer anderen Majorrace, außer uns selbst?
	if (m_pDoc->m_System[x][y].GetOwnerOfSystem() != sRaceID)
	{
		CRace* pOurRace = m_pDoc->GetRaceCtrl()->GetRace(sRaceID);
		if (!pOurRace)
			return;
		// haben wir mit dieser anderen Majorrace Krieg?
		if (pOurRace->GetAgreement(sOwner) == WAR)
			// dann wäre dies ein lohnendes Ziel, welches angegriffen werden könnte
			m_vBombardTargets[sRaceID].push_back(CPoint(x,y));
	}
}

/// Diese Funktion berechnet einen Sektor, welcher sich zum Bau eines Außenpostens eignet.
void CSectorAI::CalculateStationTargets(const CString& sRaceID)
{
	CMajor* pMajor = dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(sRaceID));
	if (pMajor)
		m_mStationBuild[sRaceID] = pMajor->GetStarmap()->CalcAIBaseSector(0.0f);
}

/// Funktion löscht alle vorher berechneten Prioritäten.
void CSectorAI::Clear(void)
{
	map<CString, CRace*>* mRaces = m_pDoc->GetRaceCtrl()->GetRaces();
	
	for (map<CString, CRace*>::const_iterator it = mRaces->begin(); it != mRaces->end(); it++)
		m_HighestShipDanger[it->first] = CPoint(-1,-1);

	m_iDangers.clear();
	m_iCombatShipDangers.clear();
	m_iCompleteDanger.clear();
	m_mStationBuild.clear();
	m_vSectorsToTerraform.clear();
	m_vMinorraceSectors.clear();
	m_vOffensiveTargets.clear();
	m_vBombardTargets.clear();
	m_iColoShips.clear();
	m_iTransportShips.clear();		
}