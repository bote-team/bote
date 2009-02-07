#include "stdafx.h"
#include "AIPrios.h"
#include "Botf2Doc.h"
#include "SectorAI.h"

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
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion berechnet die Prioritäten der einzelnen Majorrassen, wann sie ein Kolonieschiff in Auftrag
/// geben sollen.
void CAIPrios::CalcShipPrios(CSectorAI* sectorAI)
{
	ASSERT(sectorAI);
	int shipPower[DOMINION] = {0};
	int max = 0;
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		short range = -1;
		// berechnen welche max. Reichweite das Kolonieschiff der Rasse hat
		for (int j = 0; j < m_pDoc->m_ShipInfoArray.GetSize(); j++)
			if (m_pDoc->m_ShipInfoArray.GetAt(j).GetRace() == i)
				if (m_pDoc->m_ShipInfoArray.GetAt(j).GetShipType() == COLONYSHIP
					&& m_pDoc->m_ShipInfoArray.GetAt(j).GetRange() > range
					&& m_pDoc->m_ShipInfoArray.GetAt(j).IsThisShipBuildableNow(m_pDoc->m_Empire[i].GetResearch()))
				{
					range = m_pDoc->m_ShipInfoArray.GetAt(j).GetRange();
					if (range == RANGE_LONG)
						break;
				}
		// Jetzt die zum Terraformen ausgesuchten Sektoren durchgehen und schauen das diese innerhalb der Reichweite
		// des Schiffes liegen.
		for (int j = 0; j < sectorAI->GetSectorsToTerraform(i)->GetSize(); j++)
			if (m_pDoc->starmap[i]->GetRange(sectorAI->GetSectorsToTerraform(i)->GetAt(j).p) <= range)
				m_byColoShipPrio[i-1]++;
		// Bei 0 oder einem Kolonieschiff bleibt die Priorität gleich der Anzahl der zu terraformenden Sektoren.
		// Bei zwei Kolonieschiffen ist die maximale Prioriät noch 7, bei drei 8, bei vier 9 bei fünf 10.
		// Bei mehr als 5 Kolonieschiffen wird die Priorität halbiert.
		// Wenn die Priorität + 2 kleiner der Anzahl der Kolonischiffe ist, so ist die Priorität 0
		if ((m_byColoShipPrio[i-1]+1) < sectorAI->GetNumberOfColoships(i))
			m_byColoShipPrio[i-1] = 0;
		else if (sectorAI->GetNumberOfColoships(i) > 1 && sectorAI->GetNumberOfColoships(i) < 6 && m_byColoShipPrio[i-1] > sectorAI->GetNumberOfColoships(i) + 5)
			m_byColoShipPrio[i-1] = sectorAI->GetNumberOfColoships(i) + 5;
		else if (sectorAI->GetNumberOfColoships(i) > 12)
			m_byColoShipPrio[i-1] = rand()%2;
		else if (sectorAI->GetNumberOfColoships(i) > 5)
			m_byColoShipPrio[i-1] /= 4;
	}

	// Prioritäten für Truppentransporter berechnen
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		if (sectorAI->GetStationBuildSector(i).points > MINBASEPOINTS)
		{
			BOOLEAN buildableStation = FALSE;
			for (int j = 0; j < m_pDoc->m_ShipInfoArray.GetSize(); j++)
				if (m_pDoc->m_ShipInfoArray.GetAt(j).GetRace() == i)
					if (m_pDoc->m_ShipInfoArray.GetAt(j).GetShipType() == OUTPOST || m_pDoc->m_ShipInfoArray.GetAt(j).GetShipType() == STARBASE)
						if (m_pDoc->m_ShipInfoArray.GetAt(j).IsThisShipBuildableNow(m_pDoc->m_Empire[i].GetResearch()))
						{
							buildableStation = TRUE;
							break;
						}
			if (buildableStation)
			{
				if (sectorAI->GetNumberOfTransportShips(i) == NULL)
					m_byTransportPrio[i-1] = (BYTE)(sectorAI->GetStationBuildSector(i).points / (MINBASEPOINTS * 0.05));
				else
					m_byTransportPrio[i-1] = (BYTE)(sectorAI->GetStationBuildSector(i).points / (MINBASEPOINTS * 0.15));

				if ((m_byTransportPrio[i-1]) < sectorAI->GetNumberOfTransportShips(i) * 4)
					m_byTransportPrio[i-1] = 0;

				// bei einer niedrigen Kolonieschiffpriorität (möglich wenn keine gut zu terraformenden Systeme vorhanden
				// sind) wird die Transportschiffpriorität verdoppelt. Bei einer hohen Kolonieschiffpriorität wird
				// die Truppentransporterbaupriorität verringert
				if (m_byColoShipPrio[i-1] == NULL && sectorAI->GetNumberOfTransportShips(i) == NULL)
					m_byTransportPrio[i-1] *= 7;
				else if (m_byColoShipPrio[i-1] < 2)
					m_byTransportPrio[i-1] *= 2;
				else if (m_byColoShipPrio[i-1] > 5)
					m_byTransportPrio[i-1] /= 3;
			}
		}
	}

	// Prioritäten für Kriegsschiffe ermitteln
/*	for (int i = 0; i < m_pDoc->m_ShipArray.GetSize(); i++)
		if (m_pDoc->m_ShipArray.GetAt(i).GetShipType() > COLONYSHIP && m_pDoc->m_ShipArray.GetAt(i).GetShipType() < OUTPOST)
		{
			BYTE race = m_pDoc->m_ShipArray.GetAt(i).GetOwnerOfShip();
			if (race != NOBODY && race != UNKNOWN)
			{
				shipPower[race-1] += m_pDoc->m_ShipArray.GetAt(i).GetCompleteOffensivePower()
					+ m_pDoc->m_ShipArray.GetAt(i).GetCompleteDefensivePower();
				if (max < shipPower[race-1])
					max = shipPower[race-1];
			}
		}
*/
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		shipPower[i-1] += sectorAI->GetCompleteDanger(i);
		if (max < shipPower[i-1])
			max = shipPower[i-1];
	}
	

#ifdef TRACE_AI
		TRACE("max CombatShipPrio: %d\n",max);
#endif
	// Maximum der Schiffsstärken ermitteln
	for (int i = 0; i < DOMINION; i++)
	{
#ifdef TRACE_AI
		TRACE("Race: %d - ShipPower: %d - complete: %d\n",i+1, shipPower[i], sectorAI->GetCompleteDanger(i+1));
#endif
		if (shipPower[i] > 0)
			m_byCombatShipPrio[i] = (BYTE)(ceil)((float)max / (float)shipPower[i] * 1.5);
		else
			m_byCombatShipPrio[i] = MAXBYTE;
#ifdef TRACE_AI
		TRACE("Race: %d - CombatShipPrio: %d - ColoshipPrio: %d - TransportPrio: %d\n",i+1, m_byCombatShipPrio[i], m_byColoShipPrio[i], m_byTransportPrio[i]);
		TRACE("Race: %d - ColonyShipNumber: %d - TransportshipNumber: %d\n",i+1, sectorAI->GetNumberOfColoships(i+1), sectorAI->GetNumberOfTransportShips(i+1));
#endif
	}
#ifdef TRACE_AI
	TRACE("CAIPrios::CalcShipPrios ... ready\n");
#endif
}

/// Funktion löscht alle vorher berechneten Prioritäten.
void CAIPrios::Clear(void)
{
	memset(m_byColoShipPrio,   0, sizeof(*m_byColoShipPrio)   * DOMINION);
	memset(m_byTransportPrio,  0, sizeof(*m_byTransportPrio)  * DOMINION);
	memset(m_byCombatShipPrio, 0, sizeof(*m_byCombatShipPrio) * DOMINION);
	m_IntelAI.Reset();
}