#include "stdafx.h"
#include "SystemAI.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"
#include "AIPrios.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CSystemAI::CSystemAI(CBotf2Doc* pDoc)
{
	ASSERT(pDoc);
	m_pDoc = pDoc;
	
	memset(m_iPriorities, 0, sizeof(m_iPriorities));
	m_bBuildWhat = 0;
	m_bCalcedPrio = FALSE;
	m_KO = CPoint(0,0);
}

CSystemAI::~CSystemAI(void)
{

}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
// Diese Funktion führt die Berechnungen für die künstliche Intelligent in einem System aus und macht
/// alle möglichen Einträge und Veränderungen. Als Paramter wird dafür die Koordinate <code>ko</code>
/// des Systems übergeben.
void CSystemAI::ExecuteSystemAI(CPoint ko)
{
	CString sRace = m_pDoc->m_System[ko.x][ko.y].GetOwnerOfSystem();
	if (sRace.IsEmpty())
	{
		CString s;
		s.Format("Error in CSystemAI::ExecuteSystemAI(): no race controls system %d,%d!", ko.x, ko.y);
		AfxMessageBox(s);
		return;
	}

	// Besitzer des Systems holen
	m_pMajor = dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(sRace));
	ASSERT(m_pMajor);
	if (!m_pMajor)
		return;

	m_KO = ko;
	PerhapsBuy();
	CalcPriorities();
	AssignWorkers();
	ScrapBuildings();
	ApplyTradeRoutes();
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
/// Diese Funktion kauft unter Umständen den aktuellen Bauauftrag. Somit kommt der Ausbaue schneller voran.
void CSystemAI::PerhapsBuy()
{
	CPoint p = m_KO;
	
	// Wenn kein Bauauftrag in der Liste steht, so kann die Funktion sofort verlassen werden.
	int id = m_pDoc->m_System[p.x][p.y].GetAssemblyList()->GetAssemblyListEntry(0);
	if (id == NULL)
		return;
	int roundToBuild = 0;
	if (m_pDoc->m_System[p.x][p.y].GetProduction()->GetIndustryProd() > 0)
	{		
		// Never-Ready Auftrag
		if (id > 0 && id < 10000 && m_pDoc->GetBuildingInfo(id).GetNeverReady())
			roundToBuild = 0;
		// Bei Upgrades
		else if (id < 0)
		{
			roundToBuild = (int)ceil((float)(m_pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededIndustryInAssemblyList(0))
				/((float)m_pDoc->m_System[p.x][p.y].GetProduction()->GetIndustryProd()
					* (100+m_pDoc->m_System[p.x][p.y].GetProduction()->GetUpdateBuildSpeed())/100));
		}
		// Bei Gebäuden
		else if (id < 10000)
		{
			roundToBuild = (int)ceil((float)(m_pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededIndustryInAssemblyList(0))
				/((float)m_pDoc->m_System[p.x][p.y].GetProduction()->GetIndustryProd()
					* (100+m_pDoc->m_System[p.x][p.y].GetProduction()->GetBuildingBuildSpeed())/100));
		}
		// Bei Schiffen Wertfeffiziens mitbeachten
		else if (id < 20000 && m_pDoc->m_System[p.x][p.y].GetProduction()->GetShipYardEfficiency() > 0)
		{					
			roundToBuild = (int)ceil((float)(m_pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededIndustryInAssemblyList(0))
				/((float)m_pDoc->m_System[p.x][p.y].GetProduction()->GetIndustryProd() * m_pDoc->m_System[p.x][p.y].GetProduction()->GetShipYardEfficiency() / 100
					* (100+m_pDoc->m_System[p.x][p.y].GetProduction()->GetShipBuildSpeed())/100));
		}
		// Bei Truppen die Kaserneneffiziens beachten
		else if (m_pDoc->m_System[p.x][p.y].GetProduction()->GetBarrackEfficiency() > 0)
		{
			roundToBuild = (int)ceil((float)(m_pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededIndustryInAssemblyList(0))
				/((float)m_pDoc->m_System[p.x][p.y].GetProduction()->GetIndustryProd() * m_pDoc->m_System[p.x][p.y].GetProduction()->GetBarrackEfficiency() / 100
					* (100+m_pDoc->m_System[p.x][p.y].GetProduction()->GetTroopBuildSpeed())/100));
		}
	}
	// Sobald eine Rundendauer vorhanden ist, kann über den Kauf des Auftrages nachgedacht werden.
	if (roundToBuild > 1)
	{
		m_pDoc->m_System[p.x][p.y].GetAssemblyList()->CalculateBuildCosts(m_pMajor->GetTrade()->GetRessourcePriceAtRoundStart());
		int costs = m_pDoc->m_System[p.x][p.y].GetAssemblyList()->GetBuildCosts();
		int value = (m_pMajor->GetEmpire()->GetCredits() / costs) * 5;
		// Umso mehr Credits das Imperium besitzt, desto eher wird gekauft. Außerdem wird bei einer niedrigen Moral
		// eher versucht den Kauf zu tätigen, um nächstes Mal einen Polizeistaat oder ähnliches schneller starten zu können
		if (rand()%100 < value || (value > 0 && m_pDoc->m_System[p.x][p.y].GetMoral() < (rand()%21 + 60)))
		{
			costs = m_pDoc->m_System[p.x][p.y].GetAssemblyList()->BuyBuilding(m_pMajor->GetEmpire()->GetCredits());
			if (costs != 0)
			{
				m_pDoc->m_System[p.x][p.y].GetAssemblyList()->SetWasBuildingBought(TRUE);
				m_pMajor->GetEmpire()->SetCredits(-costs);
				// Die Preise an der Börse anpassen, da wir ja bestimmte Mengen Ressourcen gekauft haben
				// Achtung, hier flag == 1 setzen bei Aufruf der Funktion BuyRessource!!!!
				m_pMajor->GetTrade()->BuyRessource(TITAN,	 m_pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededTitanInAssemblyList(0),p,m_pMajor->GetEmpire()->GetCredits(),1);
				m_pMajor->GetTrade()->BuyRessource(DEUTERIUM,m_pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededDeuteriumInAssemblyList(0),p,m_pMajor->GetEmpire()->GetCredits(),1);
				m_pMajor->GetTrade()->BuyRessource(DURANIUM, m_pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededDuraniumInAssemblyList(0),p,m_pMajor->GetEmpire()->GetCredits(),1);
				m_pMajor->GetTrade()->BuyRessource(CRYSTAL,  m_pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededCrystalInAssemblyList(0),p,m_pMajor->GetEmpire()->GetCredits(),1);
				m_pMajor->GetTrade()->BuyRessource(IRIDIUM,  m_pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededIridiumInAssemblyList(0),p,m_pMajor->GetEmpire()->GetCredits(),1);
			}
		}
	}
}

/// Diese Funktion legt die Prioritäten an, mit welcher ein bestimmtes Arbeitergebäude gebaut werden soll.
void CSystemAI::CalcPriorities()
{
	CPoint ko = m_KO;	
	
	// Cecken ob ein Schiff in der Bauliste ist, aber keine Werft im System online geschaltet ist, dann abbrechen
	// Ebenfalls wenn eine Truppe in der Bauliste ist und keine Kaserne online ist
	if ((m_pDoc->m_System[ko.x][ko.y].GetAssemblyList()->GetAssemblyListEntry(0) >= 10000
		&& m_pDoc->m_System[ko.x][ko.y].GetAssemblyList()->GetAssemblyListEntry(0) <= 20000
		&& m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetShipYard() == FALSE)
		|| (m_pDoc->m_System[ko.x][ko.y].GetAssemblyList()->GetAssemblyListEntry(0) >= 20000
		&& m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetBarrack() == FALSE))
	{
		// Bauauftrag entfernen
		// CHECK WW: KI sollte hier anteilige Ressourcen zurückbekommen
		m_pDoc->m_System[ko.x][ko.y].GetAssemblyList()->ClearAssemblyList(ko, m_pDoc->m_System);
		m_pDoc->m_System[ko.x][ko.y].CalculateVariables(&m_pDoc->BuildingInfo, m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), m_pDoc->m_Sector[ko.x][ko.y].GetPlanets(), m_pMajor, CTrade::GetMonopolOwner());
	}

	// Wenn die Moral in dem System sehr niedrig ist, dann wird versucht ein Moralgebäude bzw. Polizeistaat oder ähnliches
	// laufen zu lassen. Dadurch wird versucht zu verhindern, dass sich Systeme lossagen
	if (m_pDoc->GetSystem(ko).GetMoral() < (rand()%16 + 70))
	{
		// ist Kriegsrecht, Polizeitstaat oder ähnliches nicht in der Bauliste?
		short nEntry = m_pDoc->GetSystem(ko).GetAssemblyList()->GetAssemblyListEntry(0);
		if (nEntry > 0 && nEntry < 10000)
			if (m_pDoc->GetBuildingInfo(nEntry).GetMoralProd() > 0 && m_pDoc->GetBuildingInfo(nEntry).GetNeverReady())
				// dann ist schon das richige Gebäude in der Liste und es braucht kein neues gesucht zu werden
				return;

		// wenn das Gebäude nicht mehr lang zum fertigbauen braucht (weniger als 3 - 6 Runden), dann wird es auch nicht entfernt
		int nIP = m_pDoc->GetSystem(ko).GetProduction()->GetIndustryProd();
		if (nEntry != 0 && nIP > 0)
		{
			int nRoundToBuild = 0;
			// Updates
			if (nEntry < 0)
			{
				nRoundToBuild = (int)ceil((float)(m_pDoc->GetSystem(ko).GetAssemblyList()->GetNeededIndustryInAssemblyList(0)) / ((float)nIP * (100+m_pDoc->GetSystem(ko).GetProduction()->GetUpdateBuildSpeed())/100));
			}
			// normales Gebäude
			else if (nEntry < 10000)
			{
				if (m_pDoc->GetBuildingInfo(nEntry).GetNeverReady() == false)
				{
					nRoundToBuild = (int)ceil((float)(m_pDoc->GetSystem(ko).GetAssemblyList()->GetNeededIndustryInAssemblyList(0)) / ((float)nIP * (100+m_pDoc->GetSystem(ko).GetProduction()->GetBuildingBuildSpeed())/100));
				}
			}
			// Schiffe
			else if (nEntry < 20000)
			{
				if (m_pDoc->GetSystem(ko).GetProduction()->GetShipYardEfficiency() > 0)
				{
					nRoundToBuild = (int)ceil((float)(m_pDoc->GetSystem(ko).GetAssemblyList()->GetNeededIndustryInAssemblyList(0)) / ((float)nIP * m_pDoc->GetSystem(ko).GetProduction()->GetShipYardEfficiency() / 100 * (100+m_pDoc->GetSystem(ko).GetProduction()->GetShipBuildSpeed())/100));
				}
			}
			else
			{
				if (m_pDoc->GetSystem(ko).GetProduction()->GetBarrackEfficiency() > 0)
				{
					nRoundToBuild = (int)ceil((float)(m_pDoc->GetSystem(ko).GetAssemblyList()->GetNeededIndustryInAssemblyList(0)) / ((float)nIP * m_pDoc->GetSystem(ko).GetProduction()->GetBarrackEfficiency() / 100	* (100+m_pDoc->GetSystem(ko).GetProduction()->GetTroopBuildSpeed())/100));
				}
			}
			// braucht der Auftrag noch länger als 3 bis 6 Runden oder die Moral ist auf unter 50 gefallen
			if (nRoundToBuild > rand()%4 + 3 || m_pDoc->GetSystem(ko).GetMoral() < 50)
			{
				// Bau abbrechen
				// CHECK WW: KI sollte hier anteilige Ressourcen zurückbekommen
				m_pDoc->m_System[ko.x][ko.y].GetAssemblyList()->ClearAssemblyList(ko, m_pDoc->m_System);
				m_pDoc->m_System[ko.x][ko.y].CalculateVariables(&m_pDoc->BuildingInfo, m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), m_pDoc->m_Sector[ko.x][ko.y].GetPlanets(), m_pMajor, CTrade::GetMonopolOwner());
#ifdef TRACE_AI
				MYTRACE(MT::LEVEL_INFO, "CSystemAI::CalcPriorities(): Removed current buildorder because of low moral in System '%s'\n", m_pDoc->m_Sector[ko.x][ko.y].GetName());
#endif
			}
		}

		// Kriegsrecht, Polizeistaat oder ähnliches suchen, welches in die Bauliste gesetzt werden kann um die Moral zu erhöhen
		CArray<short> buildings;
		for (int i = 0; i < m_pDoc->GetSystem(ko).GetBuildableBuildings()->GetSize(); i++)
		{
			int id = m_pDoc->GetSystem(ko).GetBuildableBuildings()->GetAt(i);
			if (m_pDoc->GetBuildingInfo(id).GetMoralProd() > 0 && m_pDoc->GetBuildingInfo(id).GetNeverReady())
				buildings.Add(id);
		}

		while (buildings.GetSize() > 0)
		{
			int nRandom = rand()%buildings.GetSize();
			if (MakeEntryInAssemblyList(buildings.GetAt(nRandom)))
			{
#ifdef TRACE_AI
				MYTRACE(MT::LEVEL_INFO, "CSystemAI::CalcPriorities(): Found building to increase moral in System '%s'\n", m_pDoc->m_Sector[ko.x][ko.y].GetName());
#endif
				// Moralverbesserungsgebäude gefunden -> aus Funktion springen
				return;	
			}
			else
				buildings.RemoveAt(nRandom);
		}
	}

	// Checken ob schon ein Eintrag in der Bauliste ist, wenn ja dann brauchen wir hier überhaupt nichts zu machen
	if (m_pDoc->GetSystem(ko).GetAssemblyList()->GetAssemblyListEntry(0) != 0)
		return;	

	// vorhandene Ressourcen durch die Planeten holen. Wenn eine Ressource nicht vorhanden ist, wird die entsprechende
	// Priorität auf NULL gesetzt. Denn dafür haben wir dann auch keine Gebäude in der Bauliste.
	BOOLEAN resExist[DERITIUM + 1] = {0};
	m_pDoc->m_Sector[ko.x][ko.y].GetAvailableResources(resExist, true);

	double dMaxHab = 0.0;
	for (int i = 0; i < m_pDoc->GetSector(ko).GetPlanets()->GetSize(); i++)
		if (m_pDoc->GetSector(ko).GetPlanet(i)->GetCurrentHabitant() > 0.0)
			dMaxHab += m_pDoc->GetSector(ko).GetPlanet(i)->GetMaxHabitant();
	// wenn die Maximale Anzahl an Einwohnern 1.25 mal größer als die aktuelle Anzahl der Einwoher ist, dann
	// werden Gebäudebauprioritäten verdoppelt
	dMaxHab			= max(1.0, dMaxHab);
	double dCurHab	= max(1.0, m_pDoc->GetSystem(ko).GetHabitants());
	double dHabMod	= max(1.0, dMaxHab / dCurHab);
	// Restliche Arbeiter berechnen (diesen Wert aber durch 1.5 teilen, damit er nicht so stark eingeht)
	int nRestWorkers = floor(dMaxHab - dCurHab) / 1.5;
	
	// zuallererst werden erstmal alle Prioritäten berechnet
	for (int i = FOOD_WORKER; i <= IRIDIUM_WORKER; i++)
	{
		switch (i)
		{
		case FOOD_WORKER:
			{
				if (m_pDoc->m_System[ko.x][ko.y].GetWorker(10) + nRestWorkers < m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i, 0, NULL))
				{
					m_iPriorities[i] = 0;
					break;
				}

				/// existieren Gebäude welche die jeweilige Priorität ermöglichen
				if (!this->CheckBuilding(i))
				{
					m_iPriorities[i] = 0;
					break;
				}
				// Hier wird die aktuelle Nahrungsmittelproduktion mit dem Inhalt des Lagers verglichen.
				// Haben wir eine positive Nahrungsproduktion, so brauchen wir keine neuen Nahrungsgebäude
				// und können daher eine geringe Priorität ansetzen.
				int foodProd = m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetFoodProd();
				if (foodProd >= 0)
					m_iPriorities[i] = 0;
				else
				{
					// Hier müssen wir die negative Nahrungsprduktion gegen den aktuellen Lagerinhalt rechnen
					foodProd *= (-5);
					long div = m_pDoc->m_System[ko.x][ko.y].GetFoodStore() + 1;
					if (div > 0)
						m_iPriorities[i] = (int)(foodProd * 100 / div);
					else 
						m_iPriorities[i] = (int)(foodProd * 100);
					m_iPriorities[i] *= 4;
				}
				if (m_iPriorities[i] > 255)
					m_iPriorities[i] = 255;
				break;
			}
		case INDUSTRY_WORKER:
			{
				if (m_pDoc->m_System[ko.x][ko.y].GetWorker(10) + nRestWorkers < 
					(m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i, 0, NULL) + m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(FOOD_WORKER, 0, NULL)))
				{
					m_iPriorities[i] = 0;
					break;
				}
				/// existieren Gebäude welche die jeweilige Priorität ermöglichen
				if (!this->CheckBuilding(i))
				{
					m_iPriorities[i] = 0;
					break;
				}
				// Hier wird die durchschnittlich zu erbringende Industrieleistung aller Bauaufträge (außer Upgrade,
				// Schiffe und Truppen) ins Verhältnis mit der maximal möglichen Industrieleistung gesetzt.
				// Hier kompliziert berechnet, aber ich möchte auch alle möglichen Boni mit eingerechnet haben.
				int IPProd = m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetIndustryProd();
				// Arbeiter temporär auf maximum stellen
				USHORT workers   = m_pDoc->m_System[ko.x][ko.y].GetWorker(i);
				USHORT number	 = m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i, 0, NULL);
				if (workers == NULL)
				{
					short id = m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i, 1, &m_pDoc->BuildingInfo);
					if (id > NULL)
						IPProd = m_pDoc->GetBuildingInfo(id).GetIPProd() * number;
				}
				else
					IPProd = IPProd * number / workers;
				int midIPCosts = 0;
				int size = 0;
				for (int j = 0; j < m_pDoc->m_System[ko.x][ko.y].GetBuildableBuildings()->GetSize(); j++)
				{
					short id = m_pDoc->m_System[ko.x][ko.y].GetBuildableBuildings()->GetAt(j);
					if (m_pDoc->GetBuildingInfo(id).GetNeverReady() == FALSE)
					{
						midIPCosts += m_pDoc->GetBuildingInfo(id).GetNeededIndustry();
						size++;
					}
				}
				for (int j = 0; j < m_pDoc->m_System[ko.x][ko.y].GetBuildableShips()->GetSize(); j++)
				{
					short id = m_pDoc->m_System[ko.x][ko.y].GetBuildableShips()->GetAt(j);
					midIPCosts += m_pDoc->m_ShipInfoArray.GetAt(id-10000).GetNeededIndustry();
					size++;
				}
				if (size > 0)
					midIPCosts /= size;
				if (IPProd > 0)
					m_iPriorities[i] = midIPCosts / IPProd;
				else
					m_iPriorities[i] = midIPCosts;
				// wenn noch Bevölkerung ins System passen würde, so werden bevorzugt mehr Gebäude gebaut
				if (dHabMod > 1.0)
					m_iPriorities[i] = m_iPriorities[i] * dHabMod + nRestWorkers;
				if (m_iPriorities[i] > 255)
					m_iPriorities[i] = 255;
				break;
			}
		case ENERGY_WORKER:
			{
				if (m_pDoc->m_System[ko.x][ko.y].GetWorker(10) + nRestWorkers < 
					(m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i, 0, NULL) + m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(FOOD_WORKER, 0, NULL)))
				{
					m_iPriorities[i] = 0;
					break;
				}
				/// existieren Gebäude welche die jeweilige Priorität ermöglichen
				if (!this->CheckBuilding(i))
				{
					m_iPriorities[i] = 0;
					break;
				}
				// Hier wird die benötigte Energie im Verhältnis zu maximal möglichen Energie betrachtet
				int EnergyProd = m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetMaxEnergyProd();
				// Arbeiter temporär auf maximum stellen
				USHORT workers   = m_pDoc->m_System[ko.x][ko.y].GetWorker(i);
				USHORT number	 = m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i, 0, NULL);
				if (workers == NULL)
				{
					short id = m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i, 1, &m_pDoc->BuildingInfo);
					if (id > NULL)
						EnergyProd = m_pDoc->GetBuildingInfo(id).GetEnergyProd() * number;
				}
				else
					EnergyProd = EnergyProd * number / workers;
				int neededEnergy = 0;
				for (int j = 0; j < m_pDoc->m_System[ko.x][ko.y].GetAllBuildings()->GetSize(); j++)
				{
					const CBuildingInfo *buildingInfo = &m_pDoc->BuildingInfo.GetAt(m_pDoc->m_System[ko.x][ko.y].GetAllBuildings()->GetAt(j).GetRunningNumber() - 1);
					neededEnergy += buildingInfo->GetNeededEnergy();
				}
				if (EnergyProd >= neededEnergy)
					m_iPriorities[i] = 0;
				else if (EnergyProd > 0)
					m_iPriorities[i] = 10*neededEnergy / EnergyProd;
				else
					m_iPriorities[i] = neededEnergy;
				if (m_iPriorities[i] > 255)
					m_iPriorities[i] = 255;
				break;
			}
		case SECURITY_WORKER:
			{
				if (m_pDoc->m_System[ko.x][ko.y].GetWorker(10) + nRestWorkers < 
					(m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i, 0, NULL) + m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(FOOD_WORKER, 0, NULL)))
				{
					m_iPriorities[i] = 0;
					break;
				}
				// existieren Gebäude welche die jeweilige Priorität ermöglichen
				if (!this->CheckBuilding(i))
				{
					m_iPriorities[i] = 0;
					break;
				}
				CString race = m_pDoc->m_System[ko.x][ko.y].GetOwnerOfSystem();
				m_iPriorities[i] = m_pDoc->m_pAIPrios->GetIntelAI()->GetIntelPrio(race);
				if (m_iPriorities[i] > 255)
					m_iPriorities[i] = 255;
				break;
			}
		case RESEARCH_WORKER:
			{
				if (m_pDoc->m_System[ko.x][ko.y].GetWorker(10) + nRestWorkers < 
					(m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i, 0, NULL) + m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(FOOD_WORKER, 0, NULL)))
				{
					m_iPriorities[i] = 0;
					break;
				}
				/// existieren Gebäude welche die jeweilige Priorität ermöglichen
				if (!this->CheckBuilding(i))
				{
					m_iPriorities[i] = 0;
					break;
				}
				
				m_iPriorities[i] = rand()%3;
				
				// wenn noch Bevölkerung ins System passen würde, so werden bevorzugt mehr Gebäude gebaut
				if (dHabMod > 1.0)
					m_iPriorities[i] = m_iPriorities[i] * dHabMod + nRestWorkers;

				if (m_iPriorities[i] > 255)
					m_iPriorities[i] = 255;
				break;
			}
		// Alle anderen sind jetzt nur noch Minenarbeiter. Für die gilt immer das gleiche Berechnugsprinzip.
		default:
			{
				BYTE res = i-5;
				if (!resExist[res])
				{
					m_iPriorities[i] = NULL;
					continue;
				}
				if (m_pDoc->m_System[ko.x][ko.y].GetWorker(10) + nRestWorkers < 
					(m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i, 0, NULL) + m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(FOOD_WORKER, 0, NULL)))
				{
					m_iPriorities[i] = 0;
					break;
				}
				/// existieren Gebäude welche die jeweilige Priorität ermöglichen
				if (!this->CheckBuilding(i))
				{
					m_iPriorities[i] = 0;
					break;
				}
				int resProd = m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetResourceProd(res);
				// Arbeiter temporär auf maximum stellen
				USHORT workers   = m_pDoc->m_System[ko.x][ko.y].GetWorker(i);
				USHORT number	 = m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i, 0, NULL);
				if (workers == NULL)
				{
					short id = m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i, 1, &m_pDoc->BuildingInfo);
					if (id > NULL)
						resProd = m_pDoc->GetBuildingInfo(id).GetResourceProd(res) * number;
				}
				else
					resProd = resProd * number / workers;
				// durchschnittliche Reskosten
				int midResCosts = 0;
				int size = 0;
				// Benötigte Ressourcen der Gebäude
				for (int j = 0; j < m_pDoc->m_System[ko.x][ko.y].GetBuildableBuildings()->GetSize(); j++)
				{
					short id = m_pDoc->m_System[ko.x][ko.y].GetBuildableBuildings()->GetAt(j);
					if (m_pDoc->GetBuildingInfo(id).GetNeverReady() == FALSE)
						midResCosts += m_pDoc->GetBuildingInfo(id).GetNeededResource(res);
				}
				// benötigte Ressourcen durch die Updates
				for (int j = 0; j < m_pDoc->m_System[ko.x][ko.y].GetBuildableUpdates()->GetSize(); j++)
				{
					short id = m_pDoc->m_System[ko.x][ko.y].GetBuildableUpdates()->GetAt(j);
					USHORT preNumber = m_pDoc->m_System[ko.x][ko.y].GetNumberOfBuilding(m_pDoc->GetBuildingInfo(id).GetPredecessorID());
					midResCosts += (m_pDoc->GetBuildingInfo(id).GetNeededResource(res) * preNumber);
					size++;
				}
				// benötigte Ressourcen durch Schiffe (nur wenn das Schiff diese Ressource auch benötigt)
				for (int j = 0; j < m_pDoc->m_System[ko.x][ko.y].GetBuildableShips()->GetSize(); j++)
				{
					short id = m_pDoc->m_System[ko.x][ko.y].GetBuildableShips()->GetAt(j);
					if (m_pDoc->m_ShipInfoArray.GetAt(id - 10000).GetNeededResource(res) > 0)
					{
						//TRACE("System: %s - Ship: %s - Res: %d - Number: %d\n", m_pDoc->m_Sector[ko.x][ko.y].GetName(),
						//	m_pDoc->m_ShipInfoArray.GetAt(id - 10000).GetShipClass(), res, m_pDoc->m_ShipInfoArray.GetAt(id - 10000).GetNeededResource(res));
						midResCosts += m_pDoc->m_ShipInfoArray.GetAt(id - 10000).GetNeededResource(res);
						size++;
					}
				}
				size /= 3.0;
				if (size > 0)
					midResCosts /= size;

/*				if (m_pDoc->m_Sector[ko.x][ko.y].GetName() == "Romulus" && res == TITAN)
				{
					CString s;
					s.Format("midResCosts: %d\nTitanProd: %d\nLager: %d",midResCosts,resProd,m_pDoc->m_System[ko.x][ko.y].GetResourceStore(res)+1);
					AfxMessageBox(s);
				}
				/*if (size > 0)
					midResCosts /= size;
				float div = 100 * resProd / (float)(midResCosts*0.5f+1.0f);
				m_iPriorities[i] = (short)(div*10 / (m_pDoc->m_System[ko.x][ko.y].GetResourceStore(res)+1));*/
				if (resProd > 0)
					m_iPriorities[i] = (int)((100 * midResCosts / (m_pDoc->m_System[ko.x][ko.y].GetResourceStore(res)+1) / resProd));
				else
					m_iPriorities[i] = (int)((100 * midResCosts / ((m_pDoc->m_System[ko.x][ko.y].GetResourceStore(res)+1) * (number+1))));

				// wenn noch Bevölkerung ins System passen würde, so werden bevorzugt mehr Gebäude gebaut
				if (dHabMod > 1.0)
					m_iPriorities[i] = m_iPriorities[i] * dHabMod + nRestWorkers;

				if (m_iPriorities[i] > 255)
					m_iPriorities[i] = 255;
				break;
			}
		}
	}

	m_bCalcedPrio = TRUE;
	
	CString name;
	int id = ChooseBuilding();
	if (id == 0)
	{
		// Machen wir vielleicht ein Update
		if (m_pDoc->m_System[ko.x][ko.y].GetBuildableUpdates()->GetSize() > 0)
		{
			int random = rand()%m_pDoc->m_System[ko.x][ko.y].GetBuildableUpdates()->GetSize();
			id = m_pDoc->m_System[ko.x][ko.y].GetBuildableUpdates()->GetAt(random)*(-1);
			if (!MakeEntryInAssemblyList(id))
			// konnte kein Update in die Bauliste gesetzt werden, so wird hier nochmal versucht ein Schiff zu bauen
			{
				id = 0;
				BOOLEAN chooseCombatship = FALSE;
				BOOLEAN chooseColoship   = FALSE;
				BOOLEAN chooseTransport  = FALSE;
				int prio = GetShipBuildPrios(chooseCombatship, chooseColoship, chooseTransport);
				id = ChooseShip(prio, chooseCombatship, chooseColoship, chooseTransport);					
			}
		}
	}
	// Baulistencheck machen, wenn wir kein Schiff reingesetzt haben. 
	// Den Check nur machen, wenn wir ein Update oder ein Gebäude welches eine Maxanzahl voraussetzt hinzufügen wollen
	if (id != 0 && id < 10000)
	{
		// bei Updates die ID ins positive nehmen
		if (id < 0)
			id *= (-1);
		if (m_pDoc->GetBuildingInfo(id).GetMaxInEmpire().Number > 0)
		{
			// Wir müssen die GlobalBuilding Variable ändern, weil sich mittlerweile ja solch ein Gebäude
			// mehr in der Bauliste befindet. Nicht aber wenn es ein Upgrade ist.
			m_pDoc->m_GlobalBuildings.AddGlobalBuilding(id);
			// Da es nur einmal pro Imperium baubar war, dann Assemblylistcheck in jedem unserer Systeme
			// durchführen
			for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
				for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
					if (m_pMajor->GetRaceBuildingNumber() == m_pDoc->GetBuildingInfo(id).GetOwnerOfBuilding())
						m_pDoc->m_System[x][y].AssemblyListCheck(&m_pDoc->BuildingInfo, &m_pDoc->m_GlobalBuildings);
		}
	}
/*	
	if (id > 0 && id < 10000)
		name = m_pDoc->GetBuildingName(id);

	if (m_pDoc->m_Sector[ko.x][ko.y].GetName() == "Tinaca")
	{
		CString s;
		s.Format("System: %s\n\nFood: %d\nIndstry: %d\nEnergy: %d\nSecurity: %d\nResearch: %d\nTitan: %d\nDeuterium: %d\nDuranium: %d\nCrystal: %d\nIridium: %d\n\nchoosen Building: %s\nID: %d\nAssemblyListEntry: %d\nneeded IP: %d",
			m_pDoc->m_Sector[ko.x][ko.y].GetName(),
			m_iPriorities[0],m_iPriorities[1],m_iPriorities[2],m_iPriorities[3],m_iPriorities[4],m_iPriorities[5],m_iPriorities[6],
			m_iPriorities[7],m_iPriorities[8],m_iPriorities[9],name,id,m_pDoc->m_System[ko.x][ko.y].GetAssemblyList()->GetAssemblyListEntry(0),
			m_pDoc->m_System[ko.x][ko.y].GetAssemblyList()->GetNeededIndustryForBuild());
		AfxMessageBox(s);
	}
*/
#ifdef TRACE_AI
	if (id == 0)
		MYTRACE(MT::LEVEL_INFO, "CSystemAI::CalcPriorities(): Could not create buildcontract in system '%s'\n", m_pDoc->m_Sector[ko.x][ko.y].GetName());
#endif
}

/// Diese Funktion wählt ein zu bauendes Gebäude aus der Liste der baubaren Gebäude. Es werden nur Gebäude
/// ausgewählt, welche in die Prioritätenliste passen. Der Rückgabewert ist die ID des Bauauftrages. 
/// Wird <code>0</code> zurückgegeben, so wurde kein Gebäude gefunden, welches in das Anforderungsprofil passt.
int CSystemAI::ChooseBuilding()
{
	CPoint ko = m_KO;
	CString race = m_pDoc->m_System[ko.x][ko.y].GetOwnerOfSystem();
	BOOLEAN chooseCombatship = FALSE;
	BOOLEAN chooseColoship   = FALSE;
	BOOLEAN chooseTransport	 = FALSE;

	// Aus der Prioritätenliste wird derjenige Eintrag ausgewählt, welcher die höchste Priorität aufweist.
	// Dieser wird mit der höchsten Wahrscheinlichket ausgewählt.
	int choosenPrio = -1;
	int min = GetShipBuildPrios(chooseCombatship, chooseColoship, chooseTransport);
	int shipBuildPrio = min;
#ifdef TRACE_AI
	MYTRACE(MT::LEVEL_INFO, "CSystemAI::ChooseBuilding(): min priority after ships: %d\n", min);
#endif
	// sind Updates baubar, so werden die Prioritäten der anderen womöglich etwas verringert, so dass häufiger
	// zuerst die Updates gebaut werden. Außer wir haben freie Arbeiter übrig.
	int nUpdates = rand()%(m_pDoc->m_System[ko.x][ko.y].GetBuildableUpdates()->GetSize()+1);
	//min -= rand()%(m_pDoc->m_System[ko.x][ko.y].GetWorker(11)+1);
	min -= m_pDoc->m_System[ko.x][ko.y].GetWorker(11);
#ifdef TRACE_AI
	MYTRACE(MT::LEVEL_INFO, "CSystemAI::ChooseBuilding(): min priority after ships, updates and workers: %d\n", min);
#endif
	for (int i = FOOD_WORKER; i <= IRIDIUM_WORKER; i++)
	{
		if (m_iPriorities[i] > min)
		{
			int nRandom = rand()%(m_iPriorities[i] + 1);
			if (nRandom - nUpdates + m_pDoc->m_System[ko.x][ko.y].GetWorker(11) > 0)
			{
				min = m_iPriorities[i];
				choosenPrio = i;
			}
		}
	}
	if (choosenPrio != -1)
	{
#ifdef TRACE_AI
		MYTRACE(MT::LEVEL_INFO, "CSystemAI::ChooseBuilding(): choosen prio: %d\n", choosenPrio);
#endif
		// Gebäude auswählen
		for (int i = m_pDoc->m_System[ko.x][ko.y].GetBuildableBuildings()->GetUpperBound(); i >= 0; i--)
		{
			int id = m_pDoc->m_System[ko.x][ko.y].GetBuildableBuildings()->GetAt(i);
			// ist der Moralmalus des Gebäudes größer unserer Moralproduktion, so wird das Gebäude nicht gebaut
			if (m_pDoc->GetBuildingInfo(id).GetMoralProd() < NULL && abs(m_pDoc->GetBuildingInfo(id).GetMoralProd()) >= m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetMoralProd())
				continue;
			// finden wir hier eine Werft, so wird versucht die Werft jetzt zu bauen
			if (m_pDoc->GetBuildingInfo(id).GetShipYard())
				if (MakeEntryInAssemblyList(id))
					return id;
			switch (choosenPrio)
			{
			case FOOD_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetFoodProd() > 0 || m_pDoc->GetBuildingInfo(id).GetFoodBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case INDUSTRY_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetIPProd() > 0 || m_pDoc->GetBuildingInfo(id).GetIndustryBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case ENERGY_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetEnergyProd() > 0 || m_pDoc->GetBuildingInfo(id).GetEnergyBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case SECURITY_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetSPProd() > 0 || m_pDoc->GetBuildingInfo(id).GetSecurityBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case RESEARCH_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetFPProd() > 0 || m_pDoc->GetBuildingInfo(id).GetResearchBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case TITAN_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetTitanProd() > 0 || m_pDoc->GetBuildingInfo(id).GetTitanBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case DEUTERIUM_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetDeuteriumProd() > 0 || m_pDoc->GetBuildingInfo(id).GetDeuteriumBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case DURANIUM_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetDuraniumProd() > 0 || m_pDoc->GetBuildingInfo(id).GetDuraniumBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case CRYSTAL_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetCrystalProd() > 0 || m_pDoc->GetBuildingInfo(id).GetCrystalBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case IRIDIUM_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetIridiumProd() > 0 || m_pDoc->GetBuildingInfo(id).GetIridiumBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			default:
				return 0;
			}
		}
	}
	// Konnte kein Gebäude entsprechend unserer Priorität gefunden werden, so wird hier vlt. zufällig ein Gebäude
	// ausgwählt, welches keine Arbeiter benötigt. Dieses wird mit einer gewissen Wahrscheinlichkeit gebaut.
	// Dadurch werden dann auch Subraumscanner, Handelszentren usw. errichtet.
	// Wenn aber Schiffe mit hoher Wahrscheinlichkeit gebaut werden sollten, dann nicht.
	else if (rand()%(shipBuildPrio + 1) <= rand()%4)
	{
		// Umso mehr Updates baubar sind, desto häufiger wird hier aus der Funktion gesprungen.
		if (rand()%(nUpdates + 1) > 0)
			return 0;
		// Umso mehr freie Arbeiter vorhanden sind, desto eher wird hier aus der Funktion gesprungen
		if (rand()%(m_pDoc->m_System[ko.x][ko.y].GetWorker(11)+1)/2 > 0)
			return 0;

		// Liste erstellen, in der die ID's der möglichen Gebäude stehen.
		CArray<short> buildings;
		for (int i = 0; i < m_pDoc->m_System[ko.x][ko.y].GetBuildableBuildings()->GetSize(); i++)
		{
			int id = m_pDoc->m_System[ko.x][ko.y].GetBuildableBuildings()->GetAt(i);
			if (m_pDoc->GetBuildingInfo(id).GetWorker() == FALSE && m_pDoc->GetBuildingInfo(id).GetNeverReady() == FALSE)
				buildings.Add(id);
		}
		int random = 0;
		while (buildings.GetSize() > 0)
		{
			random = rand()%buildings.GetSize();
			// Gebäude mit einer negativen Moral werden nicht von der KI gebaut, wenn die Moralproduktion im System
			// dies nicht ausgleichen kann
			if (m_pDoc->GetBuildingInfo(buildings.GetAt(random)).GetMoralProd() < NULL
				&& abs(m_pDoc->GetBuildingInfo(buildings.GetAt(random)).GetMoralProd()) >= m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetMoralProd())
			{
				buildings.RemoveAt(random);
				continue;
			}
			if (MakeEntryInAssemblyList(buildings.GetAt(random)))
				return buildings.GetAt(random);
			else
				buildings.RemoveAt(random);
		}
	}
	return ChooseShip(shipBuildPrio, chooseCombatship, chooseColoship, chooseTransport);	
}

/// Diese Funktion wählt ein zu bauendes Schiff aus der Liste der baubaren Schiffe. Der Rückgabewert ist die ID
/// des Schiffes (also größer 10000!). Wird <code>0</code> zurückgegeben, so wurde kein baubares Schiffe gefunden.
/// Übergeben wird dafür die Priorität <code>prio</code> des zu bauenden Schiffes (mittels Funktion <function>
/// GetShipBuildPrios</function> ermitteln) und Wahrheitswert, in denen steht welcher Schiffstyp gebaut werden soll.
int CSystemAI::ChooseShip(int prio, BOOLEAN chooseCombatship, BOOLEAN chooseColoship, BOOLEAN chooseTransport)
{
	int min = prio;
	CPoint ko = m_KO;
	CString sRace = m_pDoc->m_System[ko.x][ko.y].GetOwnerOfSystem();
	if (sRace.IsEmpty())
		return 0;

	CRace* pRace = m_pDoc->GetRaceCtrl()->GetRace(sRace);
	ASSERT(pRace);
	if (pRace->GetType() != MAJOR)
		return 0;


	BYTE researchLevels[6] =
	{
		((CMajor*)pRace)->GetEmpire()->GetResearch()->GetBioTech(),
		((CMajor*)pRace)->GetEmpire()->GetResearch()->GetEnergyTech(),
		((CMajor*)pRace)->GetEmpire()->GetResearch()->GetCompTech(),
		((CMajor*)pRace)->GetEmpire()->GetResearch()->GetPropulsionTech(),
		((CMajor*)pRace)->GetEmpire()->GetResearch()->GetConstructionTech(),
		((CMajor*)pRace)->GetEmpire()->GetResearch()->GetWeaponTech()
	};

	// Wenn ein Kolonieschiff gebaut werden sollte
	if (chooseColoship && min > 0)
	{
		// ID des Schiffes suchen
		for (int j = 0; j < m_pDoc->m_ShipInfoArray.GetSize(); j++)
			// passt die Schiffsnummer zur Rassennummer
			if (m_pDoc->m_ShipInfoArray.GetAt(j).GetRace() == pRace->GetRaceShipNumber()
				&& m_pDoc->m_ShipInfoArray.GetAt(j).GetShipType() == COLONYSHIP
				&& m_pDoc->m_ShipInfoArray.GetAt(j).IsThisShipBuildableNow(researchLevels))
			{
				int id = m_pDoc->m_ShipInfoArray.GetAt(j).GetID();
				for (int i = 0; i < m_pDoc->m_System[ko.x][ko.y].GetBuildableShips()->GetSize(); i++)
					if (m_pDoc->m_System[ko.x][ko.y].GetBuildableShips()->GetAt(i) == id)
						if (MakeEntryInAssemblyList(id))
						{
							m_pDoc->m_pAIPrios->ChoosedColoShipPrio(sRace);
#ifdef TRACE_AI
							MYTRACE(MT::LEVEL_INFO, "CSystemAI::ChooseShip(): build colonyship in system: %s\n", m_pDoc->m_Sector[m_KO.x][m_KO.y].GetName());								
#endif
							return id;
						}
			}
	}
	// Wenn ein Transportschiff gebaut werden sollte
	else if (chooseTransport && min > 0)
	{
		// ID des Schiffes suchen
		for (int j = 0; j < m_pDoc->m_ShipInfoArray.GetSize(); j++)
			// passt die Schiffsnummer zur Rassennummer
			if (m_pDoc->m_ShipInfoArray.GetAt(j).GetRace() == pRace->GetRaceShipNumber()
				&& m_pDoc->m_ShipInfoArray.GetAt(j).GetShipType() == TRANSPORTER
				&& m_pDoc->m_ShipInfoArray.GetAt(j).IsThisShipBuildableNow(researchLevels))
			{
				int id = m_pDoc->m_ShipInfoArray.GetAt(j).GetID();
				for (int i = 0; i < m_pDoc->m_System[ko.x][ko.y].GetBuildableShips()->GetSize(); i++)
					if (m_pDoc->m_System[ko.x][ko.y].GetBuildableShips()->GetAt(i) == id)
						if (MakeEntryInAssemblyList(id))
						{
							m_pDoc->m_pAIPrios->ChoosedTransportShipPrio(sRace);
#ifdef TRACE_AI
							MYTRACE(MT::LEVEL_INFO, "CSystemAI::ChooseShip(): build transportship in system: %s\n", m_pDoc->m_Sector[m_KO.x][m_KO.y].GetName());								
#endif
							return id;
						}
			}
	}
	// Wenn ein Kriegsschiff gebaut werden soll
	else if (chooseCombatship && min > 0)
	{
		// Struktur mit Schiffs-ID und Schiffsstärke anlegen (dazu ein paar Operatoren definieren)
		struct SHIPLIST {
			int id;
			UINT strenght;
			
			bool operator< (const SHIPLIST& elem2) const { return strenght < elem2.strenght;}
			bool operator> (const SHIPLIST& elem2) const { return strenght > elem2.strenght;}
			SHIPLIST() : id(0), strenght(0) {}
			SHIPLIST(int _id, UINT _strenght) : id(_id), strenght(_strenght) {}
		};
				
		// Liste erstellen, in der die ID's und die Schiffsstärken der baubaren Schiffe stehen.
		CArray<SHIPLIST> ships;
		for (int i = 0; i < m_pDoc->m_System[ko.x][ko.y].GetBuildableShips()->GetSize(); i++)
		{
			int id = m_pDoc->m_System[ko.x][ko.y].GetBuildableShips()->GetAt(i);
			if (m_pDoc->m_ShipInfoArray.GetAt(id-10000).GetShipType() > COLONYSHIP)
			{
				UINT strenght = m_pDoc->m_ShipInfoArray.GetAt(id-10000).GetCompleteOffensivePower() +
					m_pDoc->m_ShipInfoArray.GetAt(id-10000).GetCompleteDefensivePower() / 2;
				ships.Add(SHIPLIST(id, strenght));
			}
		}
		// Feld nach der Stärke der Schiffe ordnen
		c_arraysort<CArray<SHIPLIST>, SHIPLIST> (ships, sort_desc);
#ifdef TRACE_AI
		MYTRACE(MT::LEVEL_INFO, "CSystemAI::ChooseShip(): build combatship in system: %s\n", m_pDoc->m_Sector[m_KO.x][m_KO.y].GetName());
		for (int i = 0; i < ships.GetSize(); i++)
			MYTRACE(MT::LEVEL_INFO, "CSystemAI::ChooseShip(): buildable combatships %s - ID: %d - Power: %d\n", m_pDoc->m_ShipInfoArray[ships.GetAt(i).id - 10000].GetShipClass(),
				ships.GetAt(i).id-10000, ships.GetAt(i).strenght);
#endif
		// zu 75% wird versucht das stärkste Schiff zu bauen
		for (int i = 0; i < ships.GetSize(); i++)
		{
			// entweder 75% oder es ist das schwächste/letzte Schiff im Feld
			if (rand()%4 < 3 || i == ships.GetUpperBound())
			{
				// konnte das Schiff im System gebaut werden, dann sind wir hier fertig
				if (MakeEntryInAssemblyList(ships.GetAt(i).id))
				{
#ifdef TRACE_AI
					MYTRACE(MT::LEVEL_INFO, "CSystemAI::ChooseShip(): choosen combatship to build: %s\n", m_pDoc->m_ShipInfoArray[ships.GetAt(i).id - 10000].GetShipClass());
#endif
					return ships.GetAt(i).id;
				}
				else
					ships.RemoveAt(i--);
			}
		}
	}
	return 0;
}

/// Funktion überprüft ob das Gebäude mit der ID <code>id</code> in die Bauliste gesetzt werden kann.
/// Wenn es in die Bauliste gesetzt werden konnte gibt die Funktion ein <code>TRUE</code> zurück.
BOOLEAN CSystemAI::MakeEntryInAssemblyList(short id)
{
	ASSERT(id);

	// Schwierigkeitsgrad geht hier mit ein.
	float difficulty = m_pDoc->GetDifficultyLevel();
	// Wenn ein menschlicher Spieler die Autobaufunktion in einem System aktiviert hat, dann bekommt er natürlich keinen
	// Bonus durch den Schwierigkeitsgrad
	if (m_pMajor->IsHumanPlayer() == true && m_pDoc->m_System[m_KO.x][m_KO.y].GetAutoBuild() == TRUE)
		difficulty = 1.0f;
	// Wenn gebäude die NeverReady gebaut werden sollen, dann ist der Schwierigkeitsgrad ein. Es bleibt also solange drin
	// wie es Industrie kostet
	if (id > 0 && id < 10000 && m_pDoc->GetBuildingInfo(id).GetNeverReady())
		difficulty = 1.0f;
	
	CPoint ko = m_KO;
	int RunningNumber = id;
	if (id < 0)
		RunningNumber *= (-1);
	// Bei Schiffen die Schiffe prüfen
	if (id >= 10000)
	{
		m_pDoc->m_System[ko.x][ko.y].GetAssemblyList()->CalculateNeededRessources(
			0, &m_pDoc->m_ShipInfoArray.GetAt(id-10000) , 0, m_pDoc->m_System[ko.x][ko.y].GetAllBuildings(), id,
			m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), difficulty);
	}
	// Sonst die Gebäude
	else
	{
		m_pDoc->m_System[ko.x][ko.y].GetAssemblyList()->CalculateNeededRessources(
			&m_pDoc->GetBuildingInfo(RunningNumber), 0, 0, m_pDoc->m_System[ko.x][ko.y].GetAllBuildings(), id,
			m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), difficulty);
	}
	return m_pDoc->m_System[ko.x][ko.y].GetAssemblyList()->MakeEntry(id, ko, m_pDoc->m_System);
}


/// Diese Funktion besetzt die Gebäude mit Arbeitern.
void CSystemAI::AssignWorkers()
{
	CPoint ko = m_KO;
	// Nahrungsversorgung wird zu allererst beachtet. Es wird immer solange besetzt, bis man eine positive Produktion
	// erreicht hat. Hat man zu Beginn schon eine positive Produktion, so wird versucht das Minimum der benötgten
	// Besetzung zu finden.
	while (m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetFoodProd() > 5)
	{
		if (m_pDoc->m_System[ko.x][ko.y].GetWorker(FOOD_WORKER) == 0)
			break;
		m_pDoc->m_System[ko.x][ko.y].GetWorker()->DekrementWorker(FOOD_WORKER);
		CalcProd();
	}
	
	while (m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetFoodProd() < 0)
	{
		if (m_pDoc->m_System[ko.x][ko.y].GetWorker(11) > 0)
		{
			if (m_pDoc->m_System[ko.x][ko.y].GetWorker(FOOD_WORKER) >= m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(FOOD_WORKER, 0, NULL))
				break;
			m_pDoc->m_System[ko.x][ko.y].GetWorker()->InkrementWorker(FOOD_WORKER);
			CalcProd();
		}
		else
		// von woanders einen Arbeiter versuchen abzuziehen
		{
			if (!DumpWorker())
				break;
		}
	}
	// Das nächstwichtige ist die Energieversorgung der Gebäude. Wir müssen soviel Energie produzieren, dass es
	// gerade reicht die Gebäude zu betreiben, welche Strom benötigen.
	int neededEnergy = 0;
	int usedEnergy = 0;
	for (int i = 0; i < m_pDoc->m_System[ko.x][ko.y].GetAllBuildings()->GetSize(); i++)
	{
		const CBuildingInfo *buildingInfo = &m_pDoc->BuildingInfo.GetAt(m_pDoc->m_System[ko.x][ko.y].GetAllBuildings()->GetAt(i).GetRunningNumber() - 1);
		if (buildingInfo->GetNeededEnergy() > 0)
		{
			neededEnergy += buildingInfo->GetNeededEnergy();
			if (!m_pDoc->m_System[ko.x][ko.y].GetAllBuildings()->GetAt(i).GetIsBuildingOnline())
				if ((m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetEnergyProd() - usedEnergy) >= buildingInfo->GetNeededEnergy())
				{
					m_pDoc->m_System[ko.x][ko.y].SetIsBuildingOnline(i, TRUE);
					usedEnergy += buildingInfo->GetNeededEnergy();
				}
		}
	}
	while (m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetMaxEnergyProd() > (neededEnergy + 15))
	{
		if (m_pDoc->m_System[ko.x][ko.y].GetWorker(ENERGY_WORKER) == 0)
			break;
		m_pDoc->m_System[ko.x][ko.y].GetWorker()->DekrementWorker(ENERGY_WORKER);
		CalcProd();		
	}
	while (m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetMaxEnergyProd() < neededEnergy)
	{
		if (m_pDoc->m_System[ko.x][ko.y].GetWorker(11) > 0)
		{
			if (m_pDoc->m_System[ko.x][ko.y].GetWorker(ENERGY_WORKER) >= m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(ENERGY_WORKER, 0, NULL))
				break;
			m_pDoc->m_System[ko.x][ko.y].GetWorker()->InkrementWorker(ENERGY_WORKER);
			CalcProd();			
		}
		else
		// von woanders einen Arbeiter versuchen abzuziehen
		{		
			if (!DumpWorker())
				break;
		}
	}
	// Ab jetzt werden die restlichen freien Arbeiter noch auf die anderen Gebäude verteilt. Dabei werden die Prioritäten
	// beachtet.
	if (m_bCalcedPrio)
	{
		// Die Prioritäten der einzelnen Bereiche sammeln, dann die prozentuale Verteilung berechnen.
		USHORT allPrio = 0;
		for (int i = INDUSTRY_WORKER; i <= IRIDIUM_WORKER; i++)
			if (i != ENERGY_WORKER)
			{
				allPrio += m_iPriorities[i];
				m_pDoc->m_System[ko.x][ko.y].GetWorker()->SetWorker(i, 0);
			}
		int percentage[10] = {0};
		for (int i = INDUSTRY_WORKER; i <= IRIDIUM_WORKER; i++)
			if (i != ENERGY_WORKER)
				if (allPrio != NULL)
					percentage[i] = (m_iPriorities[i] * 100) / allPrio;
		// Jetzt die Arbeiter nach ihrer prozentualen Verteilung zuweisung
		for (int i = INDUSTRY_WORKER; i <= IRIDIUM_WORKER; i++)
			if (i != ENERGY_WORKER)
			{
				m_pDoc->m_System[ko.x][ko.y].GetWorker()->CalculateFreeWorkers();
				USHORT freeWorkers = m_pDoc->m_System[ko.x][ko.y].GetWorker(11);
				int workers = (freeWorkers * percentage[i]) / 100;
				for (int j = 0; j < workers; j++)
					if (m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i,0,NULL) > m_pDoc->m_System[ko.x][ko.y].GetWorker(i))
					{
						m_pDoc->m_System[ko.x][ko.y].GetWorker()->InkrementWorker(i);
						if (m_pDoc->m_System[ko.x][ko.y].GetWorker(i) == m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i, 0, NULL))
							break;
					}
			}
	}
	// Wenn etwas in der Bauliste steht, dann werden alle Arbeiter entfernt (außer bei Nahrung und Energie). Weiterhin
	// wird bei der Arbeiterbesetzung bei der Industrie begonnen.
	if (m_pDoc->m_System[ko.x][ko.y].GetAssemblyList()->GetAssemblyListEntry(0) != 0)
	{
		if (m_pDoc->m_System[ko.x][ko.y].GetWorker(11) > 0)
			while (m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(INDUSTRY_WORKER,0,NULL) > m_pDoc->m_System[ko.x][ko.y].GetWorker(INDUSTRY_WORKER))
			{
				m_pDoc->m_System[ko.x][ko.y].GetWorker()->InkrementWorker(INDUSTRY_WORKER);
				m_pDoc->m_System[ko.x][ko.y].GetWorker()->CalculateFreeWorkers();
				if (m_pDoc->m_System[ko.x][ko.y].GetWorker(11) == 0)
					break;
			}
	}
	// Wenn nichts in der Bauliste steht, dann werden die Industriearbeiter erstmal komplett entfernt
	else
	{
		m_pDoc->m_System[ko.x][ko.y].GetWorker()->SetWorker(INDUSTRY_WORKER, 0);
		m_pDoc->m_System[ko.x][ko.y].GetWorker()->CalculateFreeWorkers();
	}

	// Jetzt werden zufällig noch freie Arbeiter auf die restlichen Gebäude verteilt. Es werden aber keine Arbeiter mehr
	// auf Nahrungs-, Industrie- und Energiegebäude verteilt. Das wurde oben schon alle gemacht.
	int numberOfWorkBuildings = 0;
	int workers = 0;
	for (int i = SECURITY_WORKER; i <= IRIDIUM_WORKER; i++)
	{
		numberOfWorkBuildings += m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i,0,NULL);
		workers += m_pDoc->m_System[ko.x][ko.y].GetWorker(i);
	}
	while (m_pDoc->m_System[ko.x][ko.y].GetWorker(11) > 0 && workers < numberOfWorkBuildings)
	{
		// Zufälligen Arbeiter zwischen Geheimdienst- und Iridiumarbeiter wählen
		BYTE i = rand()%7+3;
		// Alle Arbeiter werden mit einer zufälligen Anzahl besetzt
		if (m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(i,0,NULL) > m_pDoc->m_System[ko.x][ko.y].GetWorker(i))
		{
			workers++;
			m_pDoc->m_System[ko.x][ko.y].GetWorker()->InkrementWorker(i);
			m_pDoc->m_System[ko.x][ko.y].GetWorker()->CalculateFreeWorkers();
		}
	}
	
	// Sind jetzt immernoch ein paar freie Arbeiter übrig, dann wird versucht diese auf die Industriegebäude zu verteilen.
	// Denn dadruch bekommen wir bei Handelswaren mehr Credits.
	while (m_pDoc->m_System[ko.x][ko.y].GetWorker(11) > 0)
	{
		if (m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(INDUSTRY_WORKER,0,NULL) > m_pDoc->m_System[ko.x][ko.y].GetWorker(INDUSTRY_WORKER))
		{
			m_pDoc->m_System[ko.x][ko.y].GetWorker()->InkrementWorker(INDUSTRY_WORKER);
			m_pDoc->m_System[ko.x][ko.y].GetWorker()->CalculateFreeWorkers();
		}
		else
			break;
	}	
}

/// Diese Funktion versucht Arbeiter aus Bereichen abzuziehen. Dabei werden aber keine Arbeiter aus dem Nahrungs-
/// und Energiebereich abgezogen. Die Funktion berechnet die neue Arbeiterverteilung, auch die freien Arbeiter, und
/// gibt ein <code>TRUE</code> zurück, wenn Arbeiter abgezogen werden konnten. Ansonsten gibt sie <code>FALSE</code>
/// zurück.
BOOLEAN CSystemAI::DumpWorker()
{
	BOOLEAN getWorker = FALSE;
	for (int i = INDUSTRY_WORKER; i <= IRIDIUM_WORKER; i++)
		if (i != ENERGY_WORKER)
			if (m_pDoc->m_System[m_KO.x][m_KO.y].GetWorker(i) > 0)
			{
				getWorker = TRUE;
				m_pDoc->m_System[m_KO.x][m_KO.y].GetWorker()->DekrementWorker(i);
				m_pDoc->m_System[m_KO.x][m_KO.y].GetWorker()->InkrementWorker(11);
				break;
			}
	return getWorker;
}

/// Funktion reißt überflüssige Nahrungs- und Energiegebäude im System ab.
void CSystemAI::ScrapBuildings()
{
	CPoint ko = m_KO;
	// Nahrungs- und Energiegebäude, welche zuviel sind reißt die KI ab
	if (m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetFoodProd() >= 0)
	{
		// Nur wenn 80% der maximal Bevölkerung schon im System leben
		float currentHab = 0.0f;
		float maxHab = 0.0f;
		for (int i = 0; i < m_pDoc->m_Sector[ko.x][ko.y].GetPlanets()->GetSize(); i++)
		{
			currentHab += m_pDoc->m_Sector[ko.x][ko.y].GetPlanets()->GetAt(i).GetCurrentHabitant();
			maxHab += m_pDoc->m_Sector[ko.x][ko.y].GetPlanets()->GetAt(i).GetMaxHabitant();
		}
		if (currentHab > (maxHab * 0.8))
		{
			int n = m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(FOOD_WORKER, 0, NULL) - m_pDoc->m_System[ko.x][ko.y].GetWorker()->GetWorker(FOOD_WORKER);
			int id = m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(FOOD_WORKER, 1, &m_pDoc->BuildingInfo);
			while (n > 1)
			{
				m_pDoc->m_System[ko.x][ko.y].SetBuildingDestroy(id, TRUE);
				n--;
			}
		}
	}

	if (m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetEnergyProd() > 0)
	{
		int n = m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(ENERGY_WORKER, 0, NULL) - m_pDoc->m_System[ko.x][ko.y].GetWorker()->GetWorker(ENERGY_WORKER);
		int id = m_pDoc->m_System[ko.x][ko.y].GetNumberOfWorkbuildings(ENERGY_WORKER, 1, &m_pDoc->BuildingInfo);
		while (n > 3)
		{
			m_pDoc->m_System[ko.x][ko.y].SetBuildingDestroy(id, TRUE);
			n--;
		}
	}
}

/// Diese Funktion berechnet die Priorität, mit welcher ein gewisser Schiffstyp gebaut werden soll. Übergeben werden
/// dabei drei Referenzen auf Booleanvariablen, in welcher dann ein <code>TRUE</code> steht wurde ausgewählt.
/// @return ist die Priorität, mit welcher der Schiffstyp gebaut werden soll.
int CSystemAI::GetShipBuildPrios(BOOLEAN &chooseCombatship, BOOLEAN &chooseColoship, BOOLEAN &chooseTransport)
{
	chooseCombatship = chooseColoship = chooseTransport = FALSE;
	int min = 0;
	CPoint ko = m_KO;
	CString sRace = m_pDoc->m_System[ko.x][ko.y].GetOwnerOfSystem();
	if (sRace.IsEmpty())
		return min;

	CMajor* pMajor = dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(sRace));
	ASSERT(pMajor);

	if (m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetShipYard() == TRUE
		&& m_pDoc->m_System[ko.x][ko.y].GetBuildableShips()->GetSize() > 0)
	{
		if (m_pDoc->m_pAIPrios->GetColoShipPrio(sRace) > 0)
		{
			min = rand()%(m_pDoc->m_pAIPrios->GetColoShipPrio(sRace) + 1);
			#ifdef TRACE_AI
			MYTRACE(MT::LEVEL_INFO, "CSystemAI::GetShipBuildPrios(): Race %s - System: %s - ColonyShipPrio: %d (max %d)\n",sRace,m_pDoc->m_Sector[ko.x][ko.y].GetName(),min,m_pDoc->m_pAIPrios->GetColoShipPrio(sRace));
			#endif
			chooseColoship = TRUE;
		}
		if (m_pDoc->m_pAIPrios->GetTransportShipPrio(sRace) > 0)
		{
			int random = rand()%(m_pDoc->m_pAIPrios->GetTransportShipPrio(sRace) + 1);
			#ifdef TRACE_AI
			MYTRACE(MT::LEVEL_INFO, "CSystemAI::GetShipBuildPrios(): Race %s - System: %s - TransportShipPrio: %d (max %d)\n",sRace, m_pDoc->m_Sector[ko.x][ko.y].GetName(),random,m_pDoc->m_pAIPrios->GetTransportShipPrio(sRace));
			#endif
			if (random > min)
			{
				min = random;
				chooseColoship   = FALSE;
				chooseTransport  = TRUE;
			}
		}
		if (m_pDoc->m_pAIPrios->GetCombatShipPrio(sRace) > 0)
		{
			int random = rand()%(m_pDoc->m_pAIPrios->GetCombatShipPrio(sRace) + 1);
			#ifdef TRACE_AI
			MYTRACE(MT::LEVEL_INFO, "CSystemAI::GetShipBuildPrios(): Race %s - System: %s - CombatShipPrio: %d (max %d)\n",sRace,m_pDoc->m_Sector[ko.x][ko.y].GetName(),random,m_pDoc->m_pAIPrios->GetCombatShipPrio(sRace));
			#endif
			if (random > min)
			{
				// Schiffsbevölkerungsunterstützungskosten - Schiffsunterstützungskosten
				long shipCosts = pMajor->GetEmpire()->GetPopSupportCosts() - pMajor->GetEmpire()->GetShipCosts();
				// würde man durch die Schiffe negatives Credits machen und dies wäre höher als 5% des gesamten Creditsbestandes, dann wird kein Schiff gebaut!
				if (shipCosts < 0 && abs(shipCosts) > (long)(pMajor->GetEmpire()->GetCredits() * 0.05))
				{
					chooseCombatship = FALSE;
					#ifdef TRACE_AI
					MYTRACE(MT::LEVEL_INFO, "CSystemAI::GetShipBuildPrios(): Race %s - System: %s - can't build ships because of too high shipcosts!\n",sRace, m_pDoc->m_Sector[ko.x][ko.y].GetName());
					#endif							
				}
				else
				{
					min = random;
					chooseColoship   = FALSE;
					chooseTransport  = FALSE;
					chooseCombatship = TRUE;
				}
			}
		}
	}
	#ifdef TRACE_AI
	MYTRACE(MT::LEVEL_INFO, "CSystemAI::GetShipBuildPrios(): ... ready\n");
	#endif
	
	return min;
}

/// Diese Funktion berechnet die neue Nahrungs-, Industrie- und Energieproduktion im System. Sie sollte aufgerufen
/// werden, wenn Arbeiter aus diesen Gebieten verändert wurden. Dabei werden aber alle anderen Produktionen
/// gelöscht.
void CSystemAI::CalcProd()
{
	CPoint ko = m_KO;
	int NumberOfBuildings;
	NumberOfBuildings = m_pDoc->m_System[ko.x][ko.y].GetAllBuildings()->GetSize();
	// Alle Werte wieder auf NULL setzen	
	m_pDoc->m_System[ko.x][ko.y].GetProduction()->Reset();
	
	m_pDoc->m_System[ko.x][ko.y].GetWorker()->CheckWorkers();
	unsigned short foodWorker	  = m_pDoc->m_System[ko.x][ko.y].GetWorker()->GetWorker(0);
	unsigned short industryWorker = m_pDoc->m_System[ko.x][ko.y].GetWorker()->GetWorker(1);
	unsigned short energyWorker   = m_pDoc->m_System[ko.x][ko.y].GetWorker()->GetWorker(2);

	// Die einzelnen Produktionen berechnen
	for (int i = 0; i < NumberOfBuildings; i++)
	{
		const CBuildingInfo *buildingInfo = &m_pDoc->BuildingInfo.GetAt(m_pDoc->m_System[ko.x][ko.y].GetAllBuildings()->GetAt(i).GetRunningNumber() - 1);

		// Gebäude offline setzen
		if (buildingInfo->GetWorker() == TRUE)
		{
			m_pDoc->m_System[ko.x][ko.y].GetAllBuildings()->ElementAt(i).SetIsBuildingOnline(FALSE);
			// Jetzt wieder wenn möglich online setzen
			if (buildingInfo->GetFoodProd() > 0 && foodWorker > 0)
			{
				m_pDoc->m_System[ko.x][ko.y].GetAllBuildings()->ElementAt(i).SetIsBuildingOnline(TRUE);
				foodWorker--;
			}
			else if (buildingInfo->GetIPProd() > 0 && industryWorker > 0)
			{
				m_pDoc->m_System[ko.x][ko.y].GetAllBuildings()->ElementAt(i).SetIsBuildingOnline(TRUE);
				industryWorker--;
			}
			else if (buildingInfo->GetEnergyProd() > 0 && energyWorker > 0)
			{
				m_pDoc->m_System[ko.x][ko.y].GetAllBuildings()->ElementAt(i).SetIsBuildingOnline(TRUE);
				energyWorker--;
			}
		}
		// Die einzelnen Produktionen berechnen (ohne Boni)
		// vorher noch schauen, ob diese Gebäude auch online sind
		if (m_pDoc->m_System[ko.x][ko.y].GetAllBuildings()->GetAt(i).GetIsBuildingOnline() == TRUE)
			m_pDoc->m_System[ko.x][ko.y].GetProduction()->CalculateProduction(buildingInfo);
	}
		
	// Die Boni auf die einzelnen Produktionen berechnen
	short tmpFoodBoni		= m_pMajor->GetEmpire()->GetResearch()->GetBioTech() * TECHPRODBONUS;
	short tmpIndustryBoni	= m_pMajor->GetEmpire()->GetResearch()->GetConstructionTech() * TECHPRODBONUS;
	short tmpEnergyBoni		= m_pMajor->GetEmpire()->GetResearch()->GetEnergyTechBoni() * TECHPRODBONUS;
	
	short neededEnergy = 0;
	for (int i = 0; i < NumberOfBuildings; i++)
	{
		const CBuildingInfo *buildingInfo = &m_pDoc->BuildingInfo.GetAt(m_pDoc->m_System[ko.x][ko.y].GetAllBuildings()->GetAt(i).GetRunningNumber() - 1);

		// Hier die nötige Energie von der produzierten abziehen, geht aber nur hier, wenn wir keine Boni zur Energie reinmachen
		if (m_pDoc->m_System[ko.x][ko.y].GetAllBuildings()->GetAt(i).GetIsBuildingOnline() == TRUE && buildingInfo->GetNeededEnergy() > 0)
			neededEnergy += buildingInfo->GetNeededEnergy();

		if (m_pDoc->m_System[ko.x][ko.y].GetAllBuildings()->GetAt(i).GetIsBuildingOnline() == TRUE)
		{
			// Es wird IMMER abgerundet, gemacht durch "floor"
			tmpFoodBoni			+= buildingInfo->GetFoodBoni();
			tmpIndustryBoni		+= buildingInfo->GetIndustryBoni();
			tmpEnergyBoni		+= buildingInfo->GetEnergyBoni();			
		}
	}
	// Jetzt werden noch eventuelle Boni durch die Planetenklassen dazugerechnet
	for (int i = 0; i < m_pDoc->m_Sector[ko.x][ko.y].GetPlanets()->GetSize(); i++)
		if (m_pDoc->m_Sector[ko.x][ko.y].GetPlanets()->GetAt(i).GetColonized() == TRUE
			&& m_pDoc->m_Sector[ko.x][ko.y].GetPlanets()->GetAt(i).GetCurrentHabitant() > 0.0f)
		{
			if (m_pDoc->m_Sector[ko.x][ko.y].GetPlanets()->GetAt(i).GetBoni()[6] == TRUE)	// food
				tmpFoodBoni		+= (m_pDoc->m_Sector[ko.x][ko.y].GetPlanets()->GetAt(i).GetSize()+1) * 25;
			if (m_pDoc->m_Sector[ko.x][ko.y].GetPlanets()->GetAt(i).GetBoni()[7] == TRUE)	// energy
				tmpEnergyBoni	+= (m_pDoc->m_Sector[ko.x][ko.y].GetPlanets()->GetAt(i).GetSize()+1) * 25;
		}
	
	m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iFoodProd		+= (int)(tmpFoodBoni*m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iFoodProd/100);
	m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iIndustryProd	+= (int)(tmpIndustryBoni*m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iIndustryProd/100);
	m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iEnergyProd		+= (int)(tmpEnergyBoni*m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iEnergyProd/100);

	// Wenn das System blockiert wird, dann verringern sich bestimmte Produktionswerte
	if (m_pDoc->m_System[ko.x][ko.y].GetBlockade() > NULL)
	{
		//m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iFoodProd		-= (int)(m_pDoc->m_System[ko.x][ko.y].GetBlockade() * m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iFoodProd/100);
		m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iIndustryProd	-= (int)(m_pDoc->m_System[ko.x][ko.y].GetBlockade() * m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iIndustryProd/100);
		//m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iEnergyProd		-= (int)(m_pDoc->m_System[ko.x][ko.y].GetBlockade() * m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iEnergyProd/100);		
	}
	
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Hier die Boni durch die Uniqueforschung "Wirtschaft" -> 10% mehr Industrie
	if (m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(5)->GetFieldStatus(1) == RESEARCHED)
		m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iIndustryProd += 
		(int)(m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(5)->GetBonus(1)*m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iIndustryProd/100);
	// Hier die Boni durch die Uniqueforschung "Produktion"
	if (m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(6)->GetFieldStatus(1) == RESEARCHED)
		m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iFoodProd +=
		(int)(m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(6)->GetBonus(1)*m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iFoodProd/100); 
	// Wenn wir die Uniqueforschung "Produktion" gewählt haben, und dort mehr Energie haben wollen -> 20% mehr!
	else if (m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(6)->GetFieldStatus(3) == RESEARCHED)
		m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iEnergyProd +=
		(int)(m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(6)->GetBonus(3)*m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iEnergyProd/100); 
	
	// Maximalenergie, also hier noch ohne Abzüge durch energiebedürftige Gebäude
	m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iMaxEnergyProd = m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iEnergyProd;
	// hier die gesamte Energie durch energiebedürftige Gebäude abziehen
	m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iEnergyProd -= neededEnergy;
	
	// imperiumweite Moralprod mit aufrechnen
	m_pDoc->m_System[ko.x][ko.y].GetProduction()->AddMoralProd(m_pDoc->m_System[ko.x][ko.y].GetProduction()->GetMoralProdEmpireWide(m_pMajor->GetRaceID()));
	// Den Moralboni im System noch auf die einzelnen Produktionen anrechnen
	m_pDoc->m_System[ko.x][ko.y].GetProduction()->IncludeSystemMoral(m_pDoc->m_System[ko.x][ko.y].GetMoral());
	// benötigte Nahrung durch Bevölkerung von der Produktion abiehen
	if (!m_pMajor->HasSpecialAbility(NO_FOOD_NEEDED))
		// ceil, wird auf Kommezahl berechnet, z.B brauchen wir für 14.5 Mrd. Leute 145 Nahrung und nicht 140 bzw. 150
		m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iFoodProd -= (int)ceil(m_pDoc->m_System[ko.x][ko.y].GetHabitants()*10);
	else
		m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iFoodProd = m_pDoc->m_System[ko.x][ko.y].GetProduction()->m_iMaxFoodProd;
	
	// Jetzt noch die freien Arbeiter berechnen
	m_pDoc->m_System[ko.x][ko.y].GetWorker()->CalculateFreeWorkers();
}

/// Diese Funktion legt möglicherweise eine Handelsroute zu einem anderen System an. Dadurch wird halt auch die
/// Beziehung zu Minorraces verbessert.
void CSystemAI::ApplyTradeRoutes()
{
	CPoint ko = m_KO;
	CString race = m_pDoc->m_System[ko.x][ko.y].GetOwnerOfSystem();
	if (m_pDoc->m_System[ko.x][ko.y].CanAddTradeRoute(m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()) == TRUE)
	{
		// primär zu Minorraces
		map<CString, CMinor*>* pmMinors = m_pDoc->GetRaceCtrl()->GetMinors();
		for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); ++it)
		{
			CMinor* pMinor = it->second;
			if (pMinor->GetAgreement(race) >= TRADE_AGREEMENT && pMinor->GetAgreement(race) < MEMBERSHIP)
				if (m_pDoc->m_System[ko.x][ko.y].AddTradeRoute(pMinor->GetRaceKO(), m_pDoc->m_System, m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()) == FALSE)
					break;
		}
	}
	if (m_pDoc->m_System[ko.x][ko.y].CanAddTradeRoute(m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()) == TRUE)
	{
		// sekundär zu den anderen Majorraces
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
				if (m_pDoc->m_System[x][y].GetOwnerOfSystem() != "" && m_pDoc->m_System[x][y].GetOwnerOfSystem() != race)
					if (m_pMajor->GetAgreement(m_pDoc->m_System[x][y].GetOwnerOfSystem()) >= TRADE_AGREEMENT)
						m_pDoc->m_System[ko.x][ko.y].AddTradeRoute(CPoint(x,y), m_pDoc->m_System, m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo());							
	}
}

/// Funktion überprüft, ob für einen bestimmten Arbeitertyp auch ein Gebäude in der Liste der baubaren
/// Gebäude exisitiert. Wenn dies nicht der Fall ist, so ist auch die Priorität solch ein Gebäude zu bauen
/// gleich Null.
BOOLEAN CSystemAI::CheckBuilding(BYTE whichWorker) const
{
	for (int i = 0; i < m_pDoc->m_System[m_KO.x][m_KO.y].GetBuildableBuildings()->GetSize(); i++)
	{
		int id = m_pDoc->m_System[m_KO.x][m_KO.y].GetBuildableBuildings()->GetAt(i);
		CBuildingInfo* building = &m_pDoc->GetBuildingInfo(id);
		if (whichWorker == FOOD_WORKER && building->GetFoodProd() > 0)
			return TRUE;
		if (whichWorker == INDUSTRY_WORKER && building->GetIPProd() > 0)
			return TRUE;
		if (whichWorker == ENERGY_WORKER && building->GetEnergyProd() > 0)
			return TRUE;
		if (whichWorker == SECURITY_WORKER && building->GetSPProd() > 0)
			return TRUE;
		if (whichWorker == RESEARCH_WORKER && building->GetFPProd() > 0)
			return TRUE;
		if (whichWorker == TITAN_WORKER && building->GetTitanProd() > 0)
			return TRUE;
		if (whichWorker == DEUTERIUM_WORKER && building->GetDeuteriumProd() > 0)
			return TRUE;
		if (whichWorker == DURANIUM_WORKER && building->GetDuraniumProd() > 0)
			return TRUE;
		if (whichWorker == CRYSTAL_WORKER && building->GetCrystalProd() > 0)
			return TRUE;
		if (whichWorker == IRIDIUM_WORKER && building->GetIridiumProd() > 0)
			return TRUE;
	}	
	return FALSE;
}