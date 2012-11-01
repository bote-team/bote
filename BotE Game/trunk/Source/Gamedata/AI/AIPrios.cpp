#include "stdafx.h"
#include "AIPrios.h"
#include "Botf2Doc.h"
#include "SectorAI.h"
#include "Races\RaceController.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CAIPrios::CAIPrios(CBotf2Doc* pDoc)
{
	ASSERT(pDoc);
	m_pDoc = pDoc;
	Clear();
}

CAIPrios::~CAIPrios(void)
{
}

//////////////////////////////////////////////////////////////////////
// Zugriffsfunktionen
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion berechnet die Prioritäten der einzelnen Majorrassen, wann sie ein Kolonieschiff in Auftrag
/// geben sollen.
void CAIPrios::CalcShipPrios(CSectorAI* sectorAI)
{
	MYTRACE("ai")(MT::LEVEL_INFO, "CAIPrios::CalcShipPrios() begin... \n");

	ASSERT(sectorAI);
	Clear();
	int max = 0;
	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		ASSERT(pMajor);

		BYTE researchLevels[6] =
		{
			pMajor->GetEmpire()->GetResearch()->GetBioTech(),
			pMajor->GetEmpire()->GetResearch()->GetEnergyTech(),
			pMajor->GetEmpire()->GetResearch()->GetCompTech(),
			pMajor->GetEmpire()->GetResearch()->GetPropulsionTech(),
			pMajor->GetEmpire()->GetResearch()->GetConstructionTech(),
			pMajor->GetEmpire()->GetResearch()->GetWeaponTech()
		};

		SHIP_RANGE::Typ range = SHIP_RANGE::SHORT;
		// berechnen welche max. Reichweite das Kolonieschiff der Rasse hat
		for (int j = 0; j < m_pDoc->m_ShipInfoArray.GetSize(); j++)
			if (m_pDoc->m_ShipInfoArray.GetAt(j).GetRace() == pMajor->GetRaceShipNumber())
				if (m_pDoc->m_ShipInfoArray.GetAt(j).GetShipType() == SHIP_TYPE::COLONYSHIP
					&& m_pDoc->m_ShipInfoArray.GetAt(j).GetRange() > range
					&& m_pDoc->m_ShipInfoArray.GetAt(j).IsThisShipBuildableNow(researchLevels))
				{
					range = m_pDoc->m_ShipInfoArray.GetAt(j).GetRange();
					if (range == SHIP_RANGE::LONG)
						break;
				}
		// Jetzt die zum Terraformen ausgesuchten Sektoren durchgehen und schauen das diese innerhalb der Reichweite
		// des Schiffes liegen.
		for (UINT j = 0; j < sectorAI->GetSectorsToTerraform(it->first)->size(); j++)
			if (pMajor->GetStarmap()->GetRange(sectorAI->GetSectorsToTerraform(it->first)->at(j).p) <= range)
				m_mColoShipPrio[it->first] += 1;
		// Bei 0 oder einem Kolonieschiff bleibt die Priorität gleich der Anzahl der zu terraformenden Sektoren.
		// Bei zwei Kolonieschiffen ist die maximale Prioriät noch 7, bei drei 8, bei vier 9 bei fünf 10.
		// Bei mehr als 5 Kolonieschiffen wird die Priorität halbiert.
		// Wenn die Priorität + 2 kleiner der Anzahl der Kolonischiffe ist, so ist die Priorität 0
		if ((m_mColoShipPrio[it->first] + 1) < sectorAI->GetNumberOfColoships(it->first))
			m_mColoShipPrio[it->first] = 0;
		else if (sectorAI->GetNumberOfColoships(it->first) > 1 && sectorAI->GetNumberOfColoships(it->first) < 6 && m_mColoShipPrio[it->first] > sectorAI->GetNumberOfColoships(it->first) + 5)
			m_mColoShipPrio[it->first] = sectorAI->GetNumberOfColoships(it->first) + 5;
		else if (sectorAI->GetNumberOfColoships(it->first) > 12)
			m_mColoShipPrio[it->first] = rand()%2;
		else if (sectorAI->GetNumberOfColoships(it->first) > 5)
			m_mColoShipPrio[it->first] /= 4;
	}

	// Prioritäten für Truppentransporter berechnen
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		ASSERT(pMajor);

		if (sectorAI->GetStationBuildSector(it->first).points > MINBASEPOINTS)
		{
			BOOLEAN buildableStation = FALSE;
			const BYTE researchLevels[6] =
			{
				pMajor->GetEmpire()->GetResearch()->GetBioTech(),
				pMajor->GetEmpire()->GetResearch()->GetEnergyTech(),
				pMajor->GetEmpire()->GetResearch()->GetCompTech(),
				pMajor->GetEmpire()->GetResearch()->GetPropulsionTech(),
				pMajor->GetEmpire()->GetResearch()->GetConstructionTech(),
				pMajor->GetEmpire()->GetResearch()->GetWeaponTech()
			};
			for (int j = 0; j < m_pDoc->m_ShipInfoArray.GetSize(); j++)
				if (m_pDoc->m_ShipInfoArray.GetAt(j).GetRace() == pMajor->GetRaceShipNumber())
					if (m_pDoc->m_ShipInfoArray.GetAt(j).IsStation())
						if (m_pDoc->m_ShipInfoArray.GetAt(j).IsThisShipBuildableNow(researchLevels))
						{
							buildableStation = TRUE;
							break;
						}
			if (buildableStation)
			{
				if (sectorAI->GetNumberOfTransportShips(it->first) == NULL)
					m_mTransportPrio[it->first] = (BYTE)(sectorAI->GetStationBuildSector(it->first).points / (MINBASEPOINTS * 0.05));
				else
					m_mTransportPrio[it->first] = (BYTE)(sectorAI->GetStationBuildSector(it->first).points / (MINBASEPOINTS * 0.15));

				if ((m_mTransportPrio[it->first]) < sectorAI->GetNumberOfTransportShips(it->first) * 4)
					m_mTransportPrio[it->first] = 0;

				// bei einer niedrigen Kolonieschiffpriorität (möglich wenn keine gut zu terraformenden Systeme vorhanden
				// sind) wird die Transportschiffpriorität verdoppelt. Bei einer hohen Kolonieschiffpriorität wird
				// die Truppentransporterbaupriorität verringert
				if (m_mColoShipPrio[it->first] == NULL && sectorAI->GetNumberOfTransportShips(it->first) == NULL)
					m_mTransportPrio[it->first] *= 7;
				else if (m_mColoShipPrio[it->first] < 2)
					m_mTransportPrio[it->first] *= 2;
				else if (m_mColoShipPrio[it->first] > 5)
					m_mTransportPrio[it->first] /= 3;
			}
		}
	}

	// Prioritäten für Kriegsschiffe ermitteln
	map<CString, int> shipPower;
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		shipPower[it->first] += sectorAI->GetCompleteDanger(it->first);
		if (max < shipPower[it->first])
			max = shipPower[it->first];
	}


	MYTRACE("ai")(MT::LEVEL_INFO, "CAIPrios::CalcShipPrios(): max Combatship Priority is: %d\n",max);
	// Maximum der Schiffsstärken ermitteln
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		MYTRACE("ai")(MT::LEVEL_INFO, "Calc Shippowers: Race: %s has a complete shippower of %d - all shippower is %d\n",it->first, shipPower[it->first], sectorAI->GetCompleteDanger(it->first));
		if (shipPower[it->first] > 0)
			m_mCombatShipPrio[it->first] = (BYTE)(ceil)((float)max / (float)shipPower[it->first] * 1.5);
		else
			m_mCombatShipPrio[it->first] = MAXBYTE;
		MYTRACE("ai")(MT::LEVEL_INFO, "Calc Priorities: Race: %s - CombatShipPrio: %d - ColoshipPrio: %d - TransportPrio: %d\n",it->first, m_mCombatShipPrio[it->first], m_mColoShipPrio[it->first], m_mTransportPrio[it->first]);
		MYTRACE("ai")(MT::LEVEL_INFO, "Number of shiptypes: Race: %s - Colonyships: %d - Transporthips: %d\n",it->first, sectorAI->GetNumberOfColoships(it->first), sectorAI->GetNumberOfTransportShips(it->first));
	}
	MYTRACE("ai")(MT::LEVEL_INFO, "CAIPrios::CalcShipPrios() ... ready\n");
}

/// Funktion löscht alle vorher berechneten Prioritäten.
void CAIPrios::Clear(void)
{
	m_mColoShipPrio.clear();
	m_mTransportPrio.clear();
	m_mCombatShipPrio.clear();
	m_IntelAI.Reset();
}
