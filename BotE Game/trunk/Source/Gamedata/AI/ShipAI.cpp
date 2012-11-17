#include "stdafx.h"
#include "ShipAI.h"
#include "Botf2Doc.h"
#include "SectorAI.h"
#include "Races\RaceController.h"
#include "Ships/Ships.h"

#include <cassert>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CShipAI::CShipAI(CBotf2Doc* pDoc) : m_pSectorAI()
{
	ASSERT(pDoc);
	m_pDoc = pDoc;

	map<CString, CRace*>* mRaces = m_pDoc->GetRaceCtrl()->GetRaces();
	ASSERT(mRaces);

	// Durchschnittsmoral berechnen
	map<CString, int> moralAll;
	map<CString, int> systems;
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (m_pDoc->GetSystem(x, y).GetOwnerOfSystem() != "")
			{
				moralAll[m_pDoc->GetSystem(x, y).GetOwnerOfSystem()] += m_pDoc->GetSystem(x, y).GetMoral();
				systems[m_pDoc->GetSystem(x, y).GetOwnerOfSystem()] += 1;
			}

	// alles initial initialisieren
	for (map<CString, CRace*>::const_iterator it = mRaces->begin(); it != mRaces->end(); ++it)
	{
		m_AttackSector[it->first] = CPoint(-1,-1);
		m_BombardSector[it->first] = CPoint(-1,-1);

		if (systems[it->first] > 0)
			m_iAverageMoral[it->first] = moralAll[it->first] / systems[it->first];
		else
			m_iAverageMoral[it->first] = 0;
	}
}

CShipAI::~CShipAI(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Diese Funktion erteilt allen Schiffen aller computergesteuerten Rassen Befehle.
void CShipAI::CalculateShipOrders(CSectorAI* SectorAI)
{
	ASSERT(SectorAI);
	m_pSectorAI = SectorAI;

	// einen möglichen Angriffssektor berechnen lassen
	CalcAttackSector();
	// danach einen möglichen Bombardierungssektor finden
	CalcBombardSector();

	for(CShipMap::iterator i = m_pDoc->m_ShipArray.begin(); i != m_pDoc->m_ShipArray.end(); ++i)
	{
		CShips* pShip	= &i->second;
		const CString& sOwner	= pShip->GetOwnerOfShip();
		CMajor* pOwner	= dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(sOwner));

		// gilt erstmal nur für Majors
		if (!pOwner || !pOwner->IsMajor())
			continue;

		// gilt nicht für menschliche Spieler
		if (pOwner->IsHumanPlayer())
			continue;

		// Flotte versuchen zu erstellen
		DoMakeFleet(pShip, m_pDoc->m_ShipArray.index_of(i));

		// Vielleicht haben unsere Schiffe ein Ziel, welches sie angreifen müssen/können
		if (DoAttackMove(pShip, pOwner))
		{
			DoCamouflage(pShip);
			continue;
		}

		// haben Kolonieschiffe einen Sektor zum Terraformen als Ziel, welcher kurz zuvor aber von einer
		// anderen Rasse weggeschnappt wurde, so wird ihr Kurs gelöscht
		if (pShip->GetShipType() == SHIP_TYPE::COLONYSHIP)
		{
			CPoint ptKO = pShip->GetKO();
			// hat das Kolonieschiff den Befehl zum Terraformen, so wird dieser rückgängig gemacht, wenn der Sektor
			// schon einer anderen Rasse gehört
			if (pShip->GetCurrentOrder() == SHIP_ORDER::TERRAFORM)
			{
				if (m_pDoc->GetSector(ptKO.x, ptKO.y).GetOwnerOfSector() != "" && m_pDoc->GetSector(ptKO.x, ptKO.y).GetOwnerOfSector() != sOwner)
				{
					// Terraforming abbrechen
					pShip->SetTerraformingPlanet(-1);
					pShip->SetCurrentOrder(SHIP_ORDER::AVOID);
				}
			}

			CPoint ptTarget = pShip->GetTargetKO();
			// nur wenn der Sektor noch niemandem gehört bzw. uns selbst ist, sollen Planeten terraformt werden
			if (ptTarget != CPoint(-1,-1) && m_pDoc->GetSector(ptTarget.x, ptTarget.y).GetOwnerOfSector() != "" && m_pDoc->GetSector(ptTarget.x, ptTarget.y).GetOwnerOfSector() != sOwner)
			{
				// nicht weiter fliegen und Kurs löschen
				pShip->SetTargetKO(CPoint(-1, -1),0);
				pShip->GetPath()->RemoveAll();
			}
		}

		// exisitiert kein aktueller Kurs, so wird dieser hier versucht dem Schiff zu erteilen
		if (pShip->GetPath()->GetSize() == 0)
		{
			// Scouts und Kriegsschiffe fliegen zuerst einmal zu den Minorracesystemen
			if (pShip->GetShipType() > SHIP_TYPE::COLONYSHIP)
			{
				// Zeiger auf Vektor mit Minorracessektoren holen
				vector<CPoint>* vMinorraceSectors = m_pSectorAI->GetMinorraceSectors(sOwner);

				bool bSet = false;
				int nCount = vMinorraceSectors->size() * 2;
				while (vMinorraceSectors->size() && nCount--)
				{
					int j = rand()%vMinorraceSectors->size();
					CPoint ko = vMinorraceSectors->at(j);
					// Wenn Gefahr der anderen Rassen kleiner als die der meinen ist
					if (m_pSectorAI->GetCompleteDanger(sOwner, ko) == NULL ||
						(m_pSectorAI->GetCompleteDanger(sOwner, ko) <= m_pSectorAI->GetDangerOnlyFromCombatShips(sOwner, pShip->GetKO())))
						if (pOwner->GetStarmap()->GetRange(ko) <= pShip->GetRange())
						{
							// Zielkoordinate für das Schiff setzen
							pShip->SetTargetKO(ko,0);
							MYTRACE("shipai")(MT::LEVEL_INFO, "Race %s: Ship to Minor: %s (%s) - Target: %d,%d\n",sOwner, pShip->GetShipName(), pShip->GetShipTypeAsString(), ko.x,ko.y);
							vMinorraceSectors->erase(vMinorraceSectors->begin() + j--);
							bSet = true;
							break;
						}
				}

				if (bSet)
				{
					DoCamouflage(pShip);
					continue;
				}
			}

			// Kolonieschiffe zum Terraformen schicken. Andere Schiffe fliegen manchmal auch dort hin, wenn
			// sie gerade keinen anderen Flugauftrag haben.
			if (pShip->GetShipType() >= SHIP_TYPE::COLONYSHIP && pShip->GetCurrentOrder() != SHIP_ORDER::TERRAFORM)
			{
				// Zeiger auf Vektor mit Terraformsektoren holen
				vector<CSectorAI::SectorToTerraform>* vSectorsToTerrform = m_pSectorAI->GetSectorsToTerraform(sOwner);

				for (UINT j = 0; j < vSectorsToTerrform->size(); j++)
				{
					CPoint ko = vSectorsToTerrform->at(j).p;
					// Wenn das Kolonieschiff schon auf einem Sektor für unser Terraforming steht, so fliegt es nicht weiter
					if (pShip->GetShipType() == SHIP_TYPE::COLONYSHIP && pShip->GetKO() == ko)
						break;

					// Wenn Gefahr der anderen Rassen kleiner als die der meinen ist
					if (m_pSectorAI->GetCompleteDanger(sOwner, ko) == NULL || (m_pSectorAI->GetCompleteDanger(sOwner, ko) < m_pSectorAI->GetDanger(sOwner, pShip->GetKO())))
					{
						if (pOwner->GetStarmap()->GetRange(ko) <= pShip->GetRange())
						{
							// Zielkoordinate für das Schiff setzen
							pShip->SetTargetKO(ko == pShip->GetKO() ? CPoint(-1, -1) : ko,0);
							MYTRACE("shipai")(MT::LEVEL_INFO, "Race %s: Ship %s (%s) has terraforming target: %d,%d\n",sOwner, pShip->GetShipName(), pShip->GetShipTypeAsString(), ko.x,ko.y);
							break;
						}
					}
				}
			}
			// Truppentransporter zu einem möglichen Sektor fliegen lassen um dort einen Außenposten bauen zu können
			if (m_pSectorAI->GetStationBuildSector(sOwner).points > MINBASEPOINTS && pShip->GetCurrentOrder() != SHIP_ORDER::BUILD_OUTPOST)
			{
				// nur Truppentransporter oder andere Schiffe ohne Ziel fliegen zu diesem Punkt, niemals aber
				// Kolonieschiffe
				if (pShip->GetShipType() == SHIP_TYPE::TRANSPORTER	|| (pShip->GetShipType() != SHIP_TYPE::COLONYSHIP && pShip->GetTargetKO() == pShip->GetKO()))
				{
					CPoint ko(m_pSectorAI->GetStationBuildSector(sOwner).position.x, m_pSectorAI->GetStationBuildSector(sOwner).position.y);
					// Wenn Gefahr der anderen Rassen kleiner als die der meinen ist
					if (m_pSectorAI->GetCompleteDanger(sOwner, ko) == 0 || (m_pSectorAI->GetCompleteDanger(sOwner, ko) < m_pSectorAI->GetDanger(sOwner, pShip->GetKO())))
					{
						if (pOwner->GetStarmap()->GetRange(ko) <= pShip->GetRange())
						{
							// Zielkoordinate für das Schiff setzen
							pShip->SetTargetKO(ko == pShip->GetKO() ? CPoint(-1, -1) : ko,0);
							MYTRACE("shipai")(MT::LEVEL_INFO, "Race %s: Ship %s (%s) has stationbuild target: %d,%d\n",sOwner, pShip->GetShipName(), pShip->GetShipTypeAsString(), ko.x,ko.y);
						}
					}
				}
			}

			DoCamouflage(pShip);
			if (m_pDoc->GetSector(pShip->GetKO().x, pShip->GetKO().y).GetSunSystem())
			{
				if (!DoTerraform(pShip))
					DoColonize(pShip);
			}

			DoStationBuild(pShip);
		}
		else
		{
			DoCamouflage(pShip);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion erteilt einen Terraformbefehl, sofern dies auch möglich ist.
/// @param pShip Zeiger des Schiffes
/// @return <code>true</code> wenn ein Terraformbefehl gegeben werden könnte
bool CShipAI::DoTerraform(CShips* pShip)
{
	if (!pShip)
	{
		ASSERT(pShip);
		return false;
	}

	// Es wird jeder Planet sofort genommen, welcher weniger als 8 Runden zum Terraformen
	// benötigt. Planeten welche mehr Runden zum Terraformen benötigen werden nur ausgewählt,
	// wenn in dem Sektor kein Planet zum sofortigen Kolonisieren zur Verfügung steht

	int nTerraPoints = pShip->GetColonizePoints();
	if (nTerraPoints <= 0)
		return false;

	CSector* pSector = &m_pDoc->GetSector(pShip->GetKO().x, pShip->GetKO().y);
	// nur wenn der Sektor noch niemandem gehört bzw. uns selbst ist, sollen Planeten terraformt werden
	if (pSector->GetOwnerOfSector() != "" && pSector->GetOwnerOfSector() != pShip->GetOwnerOfShip())
		return false;

	int nMinTerraPoints = INT_MAX;
	short nPlanet = -1;

	bool bColonizable = false;
	for (int j = 0; j < pSector->GetNumberOfPlanets(); j++)
	{
		const CPlanet* pPlanet = pSector->GetPlanet(j);
		// Planet überhaupt bewohnbar?
		if (!pPlanet->GetHabitable())
			continue;

		// Planet schon terraformt?
		if (pPlanet->GetTerraformed())
		{
			// und noch nicht kolonisiert?
			if (!pPlanet->GetColonized())
				bColonizable = true;
		}
		else if (pPlanet->GetNeededTerraformPoints() < nMinTerraPoints)
		{
			nMinTerraPoints = pPlanet->GetNeededTerraformPoints();
			nPlanet = j;
		}
	}

	// Wurde ein zu terraformender Planet gefunden und würden weniger als 6 Runden
	// zum Terraformen benötigt werden oder es gibt keinen Planeten, der
	// sofort kolonisiert werden könnte, dann den gefundenen Planeten terraformen
	if (nPlanet != -1 && (!bColonizable || nMinTerraPoints / nTerraPoints < 8))
	{
		// Hier muss als erstes ein möglicher neuer Kurs gelöscht werden
		pShip->SetTargetKO(CPoint(-1, -1), 0);
		pShip->SetTerraformingPlanet(nPlanet);
		pShip->SetCurrentOrder(SHIP_ORDER::TERRAFORM);
		return true;
	}

	return false;
}

/// Funktion erteilt einen Kolonisierungsbefehl, sofern dies auch möglich ist.
/// @param pShip Zeiger des Schiffes
/// @return <code>true</code> wenn ein Kolonisierungsbefehl gegeben werden könnte
bool CShipAI::DoColonize(CShips* pShip)
{
	if (!pShip)
	{
		ASSERT(pShip);
		return false;
	}

	// Kolonieschiffe eine Kolonisierung vorschlagen
	if (pShip->GetShipType() != SHIP_TYPE::COLONYSHIP)
		return false;

	CSector* pSector = &m_pDoc->GetSector(pShip->GetKO().x, pShip->GetKO().y);
	// Gehört der Sektor aktuell auch keiner Minorrace (also niemanden oder uns selbst)
	if (pSector->GetOwnerOfSector() != "" && pSector->GetOwnerOfSector() != pShip->GetOwnerOfShip())
		return false;

	// Kolonisierungsbefehl geben
	for (int i = 0; i < pSector->GetNumberOfPlanets(); i++)
	{
		if (pSector->GetPlanet(i)->GetTerraformed() && !pSector->GetPlanet(i)->GetColonized())
		{
			// Hier muss als erstes ein möglicher neuer Kurs gelöscht werden
			pShip->SetTargetKO(CPoint(-1, -1), 0);
			pShip->SetCurrentOrder(SHIP_ORDER::COLONIZE);
			return true;
		}
	}

	return false;
}

/// Funktion schickt Kriegsschiffe zu einem möglichen Offensivziel. Unter Umständen wird auch der Befehl zum
/// Angriff automatisch erteilt. Auch kann es passieren, das diese Funktion die Kriegsschiffe zu einem
/// gemeinsamen Sammelpunkt schickt.
/// @param pShip Zeiger des Schiffes
/// @param pMajor Zeiger auf den Besitzer des Schiffes
/// @return <code>true</code> wenn ein Bewegungsbefehl gegeben werden könnte
bool CShipAI::DoAttackMove(CShips* pShip, const CMajor* pMajor)
{
	if (!pShip)
	{
		ASSERT(pShip);
		return false;
	}

	if (!pMajor)
	{
		ASSERT(pMajor);
		return false;
	}

	if (pShip->GetShipType() <= SHIP_TYPE::COLONYSHIP || pShip->GetShipType() >= SHIP_TYPE::OUTPOST)
		return false;

	if (DoBombardSystem(pShip))
		return true;

	CString sRace = pMajor->GetRaceID();
	UINT nOurDanger = m_pSectorAI->GetDangerOnlyFromCombatShips(sRace, pShip->GetKO());

	// Gibt es ein Bombardierungsziel, so wird dieses mit der höchsten Priorität angeflogen
	if (m_BombardSector[sRace] != CPoint(-1,-1))
	{
		// prüfen ob wir im Bombardierungssektor stärker wären
		if (nOurDanger > m_pSectorAI->GetCompleteDanger(sRace, m_BombardSector[sRace]))
		{
			// hier überprüfen, ob der Sektor erreicht werden kann
			if (pMajor->GetStarmap()->GetRange(m_BombardSector[sRace]) <= pShip->GetRange())
			{
				pShip->SetTargetKO(m_BombardSector[sRace] == pShip->GetKO() ? CPoint(-1, -1) : m_BombardSector[sRace], 0);
				MYTRACE("shipai")(MT::LEVEL_INFO, "Race %s: BOMBARDTARGET in sector: %d/%d\n",sRace,m_BombardSector[sRace].x,m_BombardSector[sRace].y);
				return true;
			}
		}
	}

	// wenn es kein Ziel in der Nähe gibt, oder wenn das Ziel in der Nähe nicht für den Angriff geeignet war, dann
	// wenn möglich die Schiffe zu dem Punkt schicken, welcher als generelles Angriffsziel auserkoren wurde
	if (m_AttackSector[sRace] != CPoint(-1,-1))
	{
		// prüfen ob wir stärker sind
		UINT nEnemyDanger = m_pSectorAI->GetCompleteDanger(sRace, m_AttackSector[sRace]);
		bool bAttack = nOurDanger > nEnemyDanger;
		// falls nicht, prüfen ob schon 75% bis 90% unserer Flotte hier stationiert haben
		if (!bAttack)
		{
			// nur wenn wir mindestens halb so stark sind wir der Gegner (sonst macht ein Angriff keinen Sinn)
			if (nOurDanger > nEnemyDanger / 2)
				bAttack = nOurDanger > (m_pSectorAI->GetCompleteDanger(sRace) * (rand()%16+75) / 100);
		}

		if (bAttack)
		{
			// hier noch überprüfen, ob der Sektor erreicht werden kann
			if (pMajor->GetStarmap()->GetRange(m_AttackSector[sRace]) <= pShip->GetRange())
			{
				pShip->SetTargetKO(m_AttackSector[sRace] == pShip->GetKO() ? CPoint(-1, -1) : m_AttackSector[sRace], 0);
				if(MT::CMyTrace::IsLoggingEnabledFor("shipai"))
				{
					MYTRACE_CHECKED("shipai")(MT::LEVEL_INFO, "Race %s: GLOBAL ATTACK in sector: %d/%d\n",sRace,m_AttackSector[sRace].x,m_AttackSector[sRace].y);
					MYTRACE_CHECKED("shipai")(MT::LEVEL_INFO, "Ship: %s\n",pShip->GetShipName());
					MYTRACE_CHECKED("shipai")(MT::LEVEL_INFO, "OnlyDangerFromShips: %d in Sector: %d/%d\n",nOurDanger,pShip->GetKO().x,pShip->GetKO().y);
					MYTRACE_CHECKED("shipai")(MT::LEVEL_INFO, "CompleteDangerInTargetSector: %d\n",nEnemyDanger);
					MYTRACE_CHECKED("shipai")(MT::LEVEL_INFO, "Our Complete Danger overall: %d\n",(m_pSectorAI->GetCompleteDanger(sRace) * 100 / (rand()%16+75)));
				}
				return true;
			}
		}
		// falls wir nicht stärker sind, so fliegen wir zu dem Feld, wo unsere stärkste Schiffsansammlung ist
		else if (m_pSectorAI->GetHighestShipDanger(sRace) != CPoint(-1,-1))
		{
			CPoint p = m_pSectorAI->GetHighestShipDanger(sRace);
			// hier noch überprüfen, ob der Sektor erreicht werden kann
			if (pMajor->GetStarmap()->GetRange(p) <= pShip->GetRange())
			{
				pShip->SetTargetKO(p == pShip->GetKO() ? CPoint(-1, -1) : p, 0);
				MYTRACE("shipai")(MT::LEVEL_INFO, "Race %s: COLLECT ships in sector: %d/%d\n", sRace,p.x,p.y);
				return true;
			}
		}
	}
	return false;
}

/// Funktion schickt Kriegsschiffe zu einem möglichen System, welches Bombardiert werden könnte.
/// @param pShip Zeiger des Schiffes
/// @return <code>true</code> wenn ein Bewegungsbefehl gegeben werden konnte, eine Bombardierung befohlen wurde oder das Schiff auf Verstärkung zur Bombardierung im System wartet
bool CShipAI::DoBombardSystem(CShips* pShip)
{
	if (!pShip)
	{
		ASSERT(pShip);
		return false;
	}

	if (pShip->GetKO() == m_BombardSector[pShip->GetOwnerOfShip()])
	{
		// Hier muss als erstes ein möglicher neuer Kurs gelöscht werden und ein alter Systemangriffsbefehl aufgehoben werden
		pShip->SetTargetKO(CPoint(-1, -1), 0);

		// Wenn das Schiff bzw. Schiffe aus der Flotte getarnt sind, dann müssen diese erst enttarnt werden
		if (pShip->GetCloak() || (pShip->HasFleet() && pShip->CheckOrder(SHIP_ORDER::ATTACK_SYSTEM) == FALSE))
		{
			// Schiff enttarnen
			pShip->SetCurrentOrder(SHIP_ORDER::CLOAK);
			return true;
		}

		// Wenn die defensive Schiffsstärke im System des Angreifers ungefähr doppelt so groß als der Systemverteidigungswert
		// des Verteidigers ist, wird bombardiert!
		int nShipDefend = m_pDoc->GetSystem(pShip->GetKO().x, pShip->GetKO().y).GetProduction()->GetShipDefend();
		nShipDefend *= 1.25;

		int nShipValue = 0;

		for(CShipMap::iterator i = m_pDoc->m_ShipArray.begin(); i != m_pDoc->m_ShipArray.end(); ++i)
		{
			if (i->second.GetOwnerOfShip() != pShip->GetOwnerOfShip())
				continue;

			if (i->second.GetKO() != pShip->GetKO())
				continue;

			nShipValue += i->second.GetHull()->GetCurrentHull();
			for (CShips::iterator m = i->second.begin(); m != i->second.end(); ++m)
				nShipValue += m->second.GetHull()->GetCurrentHull();

			if (nShipValue > nShipDefend)
				break;
		}

		//CString s;
		//s.Format("shipValue = %d\nshipDefend = %d\nSektor = %s", shipValue, shipDefend, m_pDoc->GetSector(pShip->GetKO().x, pShip->GetKO().y).GetName(true));
		//AfxMessageBox(s);
		if (nShipValue > nShipDefend)
		{
			MYTRACE("shipai")(MT::LEVEL_INFO, "Race %s: Ship %s (%s) is bombarding system: %d,%d\n",pShip->GetOwnerOfShip(), pShip->GetShipName(), pShip->GetShipTypeAsString(), pShip->GetKO().x,pShip->GetKO().y);
			pShip->SetCurrentOrder(SHIP_ORDER::ATTACK_SYSTEM);
			return true;
		}
	}

	return false;
}

/// Funktion erteilt einen Tarnbefehl oder Enttarnbefehl.
/// @param pShip Zeiger des Schiffes
/// @param bCamouflage <code>true</code> für Tarnen, <code>false</code> für Enttarnen
/// @return <code>true</code> wenn der Befehl gegeben wurde
bool CShipAI::DoCamouflage(CShips* pShip, bool bCamouflage/* = true*/)
{
	if (!pShip)
	{
		ASSERT(pShip);
		return false;
	}

	// Schiff ist schon getarnt/enttarnt
	if (bCamouflage == pShip->GetCloak())
		return false;

	// Nur wenn das Schiff sich tarnen kann und nicht gerade dabei ist ein System zu bombardieren soll es sich tarnen
	if (pShip->GetStealthPower() > 3 && pShip->GetCurrentOrder() != SHIP_ORDER::ATTACK_SYSTEM)
	{
		pShip->SetCurrentOrder(SHIP_ORDER::CLOAK);
		return true;
	}

	return false;
}

/// Funktion erstellt eine Flotte. Schiffe werden der Flotte nur hinzugefügt, wenn diese bestimmte Voraussetzungen erfüllen.
/// So muss der ungefähre Schiffstyp übereinstimmen (Combat <-> NonCombat) sowie die Reichweite des Schiffes.
void CShipAI::DoMakeFleet(CShips* pShip, int nIndex)
{
	if (!pShip)
	{
		ASSERT(pShip);
		return;
	}

	if (pShip->IsStation())
		return;

	assert(pShip == &m_pDoc->m_ShipArray.GetAt(nIndex));
	bool increment = false;
	for(CShipMap::iterator i = m_pDoc->m_ShipArray.iterator_at(nIndex + 1);;)
	{
		if(increment)
			++i;
		increment = true;
		if(i == m_pDoc->m_ShipArray.end())
			break;

		// Schiffe müssen von der selben Rasse sein
		if (pShip->GetOwnerOfShip() != i->second.GetOwnerOfShip())
			continue;

		// Schiffe müssen sich im selben Sektor befinden
		if (pShip->GetKO() != i->second.GetKO())
			continue;

		// beide Schiffe müssen die selbe Reichweite haben
		if (pShip->GetRange() != i->second.GetRange())
			continue;

		// das hinzuzufügende Schiff darf kein Außenposten oder Sternbasis sein
		if (i->second.IsStation())
			continue;

		// das hinzuzufügende Schiff darf kein individuelles Ziel haben
		if (i->second.GetPath()->GetSize() > 0)
			continue;

		// der Tarnstatus muss gleich sein (also keine getarnten und ungetarnte Schiffe in eine Flotte)
		if (pShip->GetCloak() != i->second.GetCloak())
			continue;

		// wenn sich das Führungsschiff tarnen kann, dann muss das hinzuzufügende Schiff sich auch tarnen können
		if ((pShip->GetStealthPower() > 3 && i->second.GetStealthPower() <= 3) || (pShip->GetStealthPower() <= 3 && i->second.GetStealthPower() > 3))
			continue;

		// es muss sich bei beiden Schiffen um Kriegsschiffe handeln oder bei beiden Schiffen um Transporter oder bei beiden Schiffen um Kolonieschiffe
		if ((!pShip->IsNonCombat() && !i->second.IsNonCombat())
			||(pShip->GetShipType() == SHIP_TYPE::TRANSPORTER && i->second.GetShipType() == SHIP_TYPE::TRANSPORTER && i->second.GetCurrentOrder() < SHIP_ORDER::BUILD_OUTPOST)
			||(pShip->GetShipType() == SHIP_TYPE::COLONYSHIP && i->second.GetShipType() == SHIP_TYPE::COLONYSHIP && i->second.GetCurrentOrder() < SHIP_ORDER::COLONIZE))
		{
			pShip->AddShipToFleet(i->second);
			m_pDoc->m_ShipArray.RemoveAt(i);
			increment = false;
		}
	}
}

/// Funktion erteilt einen Außenpostenbaubefehl, sofern dies auch möglich ist.
/// @param pShip Zeiger des Schiffes
/// @return <code>true</code> wenn ein Außenpostenbaubefehl gegeben werden könnte
bool CShipAI::DoStationBuild(CShips* pShip)
{
	if (!pShip)
	{
		ASSERT(pShip);
		return false;
	}

	if (pShip->GetStationBuildPoints() <= 0 || pShip->GetCurrentOrder() == SHIP_ORDER::BUILD_OUTPOST)
		return false;

	const CString& sRace = pShip->GetOwnerOfShip();
	if (m_pSectorAI->GetStationBuildSector(sRace).points <= MINBASEPOINTS)
		return false;

	CPoint ptKO(m_pSectorAI->GetStationBuildSector(sRace).position.x, m_pSectorAI->GetStationBuildSector(sRace).position.y);
	if (pShip->GetKO() != ptKO)
		return false;

	// Nur wenn der Sektor uns bzw. niemanden gehört
	if (m_pDoc->GetSector(ptKO.x, ptKO.y).GetOwnerOfSector() == "" || m_pDoc->GetSector(ptKO.x, ptKO.y).GetOwnerOfSector() == sRace)
	{
		pShip->SetTargetKO(CPoint(-1, -1), 0);
		pShip->SetCurrentOrder(SHIP_ORDER::BUILD_OUTPOST);
		return true;
	}

	return false;
}

/// Funkion berechnet einen möglichen Angriffssektor, welcher später gesammelt angegriffen werden kann.
void CShipAI::CalcAttackSector(void)
{
	// für alle Majors den Angriffssektor berechnen
	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		int nearestSector = MAXSHORT;
		// beinhaltet die Sektorkoordinate mit unserem Angriffsziel
		CPoint sector = CPoint(-1,-1);
		for (UINT j = 0; j < m_pSectorAI->GetOffensiveTargets(it->first)->size(); j++)
		{
			int space = max(abs(m_pSectorAI->GetHighestShipDanger(it->first).x - m_pSectorAI->GetOffensiveTargets(it->first)->at(j).x),
				abs(m_pSectorAI->GetHighestShipDanger(it->first).y - m_pSectorAI->GetOffensiveTargets(it->first)->at(j).y));
			if (space < nearestSector)
			{
				// Wenn dieses Ziel eine kleinere Gefahr darstellt als unsere größte Flottenansammlung oder wir
				// in schon mindst 50% unserer Flotte hier versammelt haben, so wird es als Ziel aufgenommen
				if ((m_pSectorAI->GetDangerOnlyFromCombatShips(it->first, m_pSectorAI->GetHighestShipDanger(it->first)) > m_pSectorAI->GetCompleteDanger(it->first, m_pSectorAI->GetOffensiveTargets(it->first)->at(j)))
					|| (m_pSectorAI->GetDangerOnlyFromCombatShips(it->first, m_pSectorAI->GetHighestShipDanger(it->first)) > (m_pSectorAI->GetCompleteDanger(it->first) * 50 / 100)))
				{
					nearestSector = space;
					sector = m_pSectorAI->GetOffensiveTargets(it->first)->at(j);
				}
			}
		}
		m_AttackSector[it->first] = sector;
		MYTRACE("shipai")(MT::LEVEL_INFO, "Race %s: global attack sector is %d/%d\n",it->first, m_AttackSector[it->first].x, m_AttackSector[it->first].y);
	}
}

/// Funktion berechnet einen möglich besten Sektor für eine Bombardierung. Wurde solch ein Sektor ermittelt hat dieser
/// die allerhöchste Priorität.
void CShipAI::CalcBombardSector(void)
{
	// für alle Majors den Angriffssektor berechnen
	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		// pazifistische Rassen bombardieren nie
		if (it->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
			continue;

		// die Hauptrassen bombardieren nicht alle gleichoft und gleich lang. Die Föderation z.B. kann wegen den
		// Moralabzuügen nicht so lang bombardieren.
		// Moralwert für Bombardierungen holen
		short nMoralValue = CMoralObserver::GetMoralValue(it->second->GetRaceMoralNumber(), 19);
		if (nMoralValue < 0)
		{
			nMoralValue = abs(nMoralValue);
			if (m_iAverageMoral[it->first] < 80 + nMoralValue * 5)
				continue;
		}

		// ist der globale Angriffssektor auch bei den Bombardierungzielen vorhanden, so wird dieser benutzt und kein
		// am nächsten befindlicher Bombardierungssektor
		for (UINT j = 0; j < m_pSectorAI->GetBombardTargets(it->first)->size(); j++)
		{
			if (m_pSectorAI->GetBombardTargets(it->first)->at(j) == m_AttackSector[it->first])
			{
				m_BombardSector[it->first] = m_pSectorAI->GetBombardTargets(it->first)->at(j);
				break;
			}
		}

		// Wurde kein Bombardierungziel mit dem gleichen globalen Attackziel gefunden, so wird versucht ein nächstes
		// zu unserer größten Flottenansammlung zu finden
		if (m_BombardSector[it->first] == CPoint(-1,-1))
		{
			int nearestSector = INT_MAX;
			// beinhaltet die Sektorkoordinate mit unserem Bombardierungsziel
			CPoint sector = CPoint(-1,-1);
			for (UINT j = 0; j < m_pSectorAI->GetBombardTargets(it->first)->size(); j++)
			{
				// Es wird das Ziel ausgewählt, welches am nächsten von unserer größten Flottensammlung entfernt ist
				int space = max(abs(m_pSectorAI->GetHighestShipDanger(it->first).x - m_pSectorAI->GetBombardTargets(it->first)->at(j).x),
					abs(m_pSectorAI->GetHighestShipDanger(it->first).y - m_pSectorAI->GetBombardTargets(it->first)->at(j).y));
				if (space < nearestSector)
				{
					// Wenn dieses Ziel eine kleinere Gefahr darstellt als unsere größte Flottenansammlung, so wird
					// es als Ziel aufgenommen
					if (m_pSectorAI->GetDangerOnlyFromCombatShips(it->first, m_pSectorAI->GetHighestShipDanger(it->first)) > m_pSectorAI->GetCompleteDanger(it->first, m_pSectorAI->GetBombardTargets(it->first)->at(j)))
					{
						nearestSector = space;
						sector = m_pSectorAI->GetBombardTargets(it->first)->at(j);
					}
				}
			}
			m_BombardSector[it->first] = sector;
		}
		MYTRACE("shipai")(MT::LEVEL_INFO, "Race %s: global bombard sector is %d/%d\n",it->first, m_BombardSector[it->first].x, m_BombardSector[it->first].y);
	}
}
