// System.cpp: Implementierung der Klasse CSystem.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "System.h"
#include "botf2.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_SERIAL (CSystem, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CSystem::CSystem()
{
	ResetSystem();
}

CSystem::~CSystem()
{
	m_BuildingDestroy.RemoveAll();
	m_Buildings.RemoveAll();
	m_BuildableBuildings.RemoveAll();
	m_AllwaysBuildableBuildings.RemoveAll();
	m_BuildableUpdates.RemoveAll();
	m_BuildableWithoutAssemblylistCheck.RemoveAll();
	m_BuildableShips.RemoveAll();
	m_BuildableTroops.RemoveAll();
	m_TradeRoutes.RemoveAll();
	m_ResourceRoutes.RemoveAll();
	m_Troops.RemoveAll();
}

CSystem::CSystem(const CSystem &other){
	/*m_AssemblyList=other.m_AssemblyList;
	m_Production=other.m_Production;
	m_Workers=other.m_Workers;
	m_sOwnerOfSystem=other.m_sOwnerOfSystem;
	m_dHabitants=other.m_dHabitants;
	m_iMoral=other.m_iMoral;
	m_byBlockade=other.m_byBlockade;
	m_iFoodStore=other.m_iFoodStore;
	m_iTitanStore=other.m_iTitanStore;
	m_iDeuteriumStore=other.m_iDeuteriumStore;
	m_iDuraniumStore=other.m_iDuraniumStore;
		ar << m_iCrystalStore;
		ar << m_iIridiumStore;
		ar << m_iDeritiumStore;
		ar << m_iFoodBuildings;
		ar << m_iIndustryBuildings;
		ar << m_iEnergyBuildings;	
		ar << m_iSecurityBuildings;
		ar << m_iResearchBuildings;
		ar << m_iTitanMines;
		ar << m_iDeuteriumMines;
		ar << m_iDuraniumMines;
		ar << m_iIridiumMines;
		ar << m_iCrystalMines;*/


}

CSystem& CSystem::operator=(const CSystem&){
	return (*this);
	
};

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CSystem::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	m_AssemblyList.Serialize(ar);
	m_Production.Serialize(ar);
	m_Workers.Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_sOwnerOfSystem;
		ar << m_dHabitants;
		ar << m_iMoral;
		ar << m_byBlockade;
		ar << m_iFoodStore;
		ar << m_iTitanStore;
		ar << m_iDeuteriumStore;
		ar << m_iDuraniumStore;
		ar << m_iCrystalStore;
		ar << m_iIridiumStore;
		ar << m_iDeritiumStore;
		ar << m_iFoodBuildings;
		ar << m_iIndustryBuildings;
		ar << m_iEnergyBuildings;	
		ar << m_iSecurityBuildings;
		ar << m_iResearchBuildings;
		ar << m_iTitanMines;
		ar << m_iDeuteriumMines;
		ar << m_iDuraniumMines;
		ar << m_iIridiumMines;
		ar << m_iCrystalMines;
		for (int i = WORKER::FOOD_WORKER; i <= WORKER::IRIDIUM_WORKER; i++)
			ar << m_bDisabledProductions[i];
		ar << m_Buildings.GetSize();
		for (int i = 0; i < m_Buildings.GetSize(); i++)
			m_Buildings.GetAt(i).Serialize(ar);
		m_BuildableBuildings.Serialize(ar);
		m_BuildableUpdates.Serialize(ar);
		m_BuildableShips.Serialize(ar);
		m_BuildableTroops.Serialize(ar);
		m_AllwaysBuildableBuildings.Serialize(ar);
		m_BuildableWithoutAssemblylistCheck.Serialize(ar);
		m_BuildingDestroy.Serialize(ar);
		ar << m_TradeRoutes.GetSize();
		for (int i = 0; i < m_TradeRoutes.GetSize(); i++)
			m_TradeRoutes.GetAt(i).Serialize(ar);
		ar << m_byMaxTradeRoutesFromHab;
		ar << m_ResourceRoutes.GetSize();
		for (int i = 0; i < m_ResourceRoutes.GetSize(); i++)
			m_ResourceRoutes.GetAt(i).Serialize(ar);
		ar << m_Troops.GetSize();
		for (int i = 0; i < m_Troops.GetSize(); i++)
			m_Troops.GetAt(i).Serialize(ar);
		ar << m_bAutoBuild;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		int number = 0;
		ar >> m_sOwnerOfSystem;
		ar >> m_dHabitants;
		ar >> m_iMoral;
		ar >> m_byBlockade;
		ar >> m_iFoodStore;
		ar >> m_iTitanStore;
		ar >> m_iDeuteriumStore;
		ar >> m_iDuraniumStore;
		ar >> m_iCrystalStore;
		ar >> m_iIridiumStore;
		ar >> m_iDeritiumStore;
		ar >> m_iFoodBuildings;
		ar >> m_iIndustryBuildings;
		ar >> m_iEnergyBuildings;	
		ar >> m_iSecurityBuildings;
		ar >> m_iResearchBuildings;
		ar >> m_iTitanMines;
		ar >> m_iDeuteriumMines;
		ar >> m_iDuraniumMines;
		ar >> m_iIridiumMines;
		ar >> m_iCrystalMines;
		for (int i = WORKER::FOOD_WORKER; i <= WORKER::IRIDIUM_WORKER; i++)
			ar >> m_bDisabledProductions[i];
		ar >> number;
		m_Buildings.RemoveAll();
		m_Buildings.SetSize(number);
		for (int i = 0; i < number; i++)
			m_Buildings.GetAt(i).Serialize(ar);
		m_BuildableBuildings.RemoveAll();
		m_BuildableUpdates.RemoveAll();
		m_BuildableShips.RemoveAll();
		m_BuildableTroops.RemoveAll();
		m_AllwaysBuildableBuildings.RemoveAll();
		m_BuildableWithoutAssemblylistCheck.RemoveAll();
		m_BuildingDestroy.RemoveAll();

		m_BuildableBuildings.Serialize(ar);
		m_BuildableUpdates.Serialize(ar);
		m_BuildableShips.Serialize(ar);
		m_BuildableTroops.Serialize(ar);
		m_AllwaysBuildableBuildings.Serialize(ar);
		m_BuildableWithoutAssemblylistCheck.Serialize(ar);
		m_BuildingDestroy.Serialize(ar);
		ar >> number;
		m_TradeRoutes.RemoveAll();
		m_TradeRoutes.SetSize(number);
		for (int i = 0; i < number; i++)
			m_TradeRoutes.GetAt(i).Serialize(ar);
		ar >> m_byMaxTradeRoutesFromHab;
		ar >> number;
		m_ResourceRoutes.RemoveAll();
		m_ResourceRoutes.SetSize(number);
		for (int i = 0; i < number; i++)
			m_ResourceRoutes.GetAt(i).Serialize(ar);
		ar >> number;
		m_Troops.RemoveAll();
		m_Troops.SetSize(number);
		for (int i = 0; i < number; i++)
			m_Troops.GetAt(i).Serialize(ar);
		ar >> m_bAutoBuild;
	}
}

//////////////////////////////////////////////////////////////////////
// Zugriffsfunktionen
//////////////////////////////////////////////////////////////////////

int CSystem::GetNeededRoundsToCompleteProject(int nID)
{
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);

	int nRounds = 0;

	if (GetProduction()->GetIndustryProd() <= 0)
		return nRounds;

	CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sOwnerOfSystem));
	if (!pMajor)
		return nRounds;

	// handelt es sich um ein Update von Gebäuden?
	if (nID < 0)
	{
		// Hier Berechnung der noch verbleibenden Runden, bis das Update fertig wird
		m_AssemblyList.CalculateNeededRessources(&pDoc->GetBuildingInfo(abs(nID)), NULL, NULL, &m_Buildings, nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
		
		nRounds = (int)ceil((float)(m_AssemblyList.GetNeededIndustryForBuild())
			/ ((float)GetProduction()->GetIndustryProd() 
			* (100 + GetProduction()->GetUpdateBuildSpeed()) / 100));		
	}
	// handelt es sich um ein Gebäude
	else if (nID < 10000)
	{
		m_AssemblyList.CalculateNeededRessources(&pDoc->GetBuildingInfo(nID), NULL, NULL, &m_Buildings, nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
		
		if (pDoc->GetBuildingInfo(nID).GetNeverReady())
			return nRounds;

		nRounds = (int)ceil((float)(m_AssemblyList.GetNeededIndustryForBuild())
			/ ((float)GetProduction()->GetIndustryProd()
			* (100 + GetProduction()->GetBuildingBuildSpeed()) / 100));
	}
	// handelt es sich um ein Schiff
	else if (nID < 20000)
	{
		// Hier Berechnung der noch verbleibenden Runden, bis das Projekt fertig wird
		m_AssemblyList.CalculateNeededRessources(NULL, &pDoc->m_ShipInfoArray.GetAt(nID - 10000), NULL, &m_Buildings, nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
		if (GetProduction()->GetShipYardEfficiency() == 0)
			return nRounds;

		nRounds = (int)ceil((float)(m_AssemblyList.GetNeededIndustryForBuild())
			/ ((float)GetProduction()->GetIndustryProd() * GetProduction()->GetShipYardEfficiency() / 100
			* (100 + GetProduction()->GetShipBuildSpeed()) / 100));
	}
	// handelt es sich um eine Truppe
	else
	{
		// Hier Berechnung der noch verbleibenden Runden, bis das Projekt fertig wird
		m_AssemblyList.CalculateNeededRessources(NULL, NULL, &pDoc->m_TroopInfo.GetAt(nID - 20000), &m_Buildings, nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
		if (GetProduction()->GetBarrackEfficiency() == 0)
			return nRounds;

		nRounds = (int)ceil((float)(m_AssemblyList.GetNeededIndustryForBuild())
			/ ((float)GetProduction()->GetIndustryProd() * GetProduction()->GetBarrackEfficiency() / 100
			* (100 + GetProduction()->GetTroopBuildSpeed()) / 100));
	}
	
	return nRounds;
}

// Funktion gibt die Anzahl oder die RunningNumber (ID) der Gebäude zurück, welche Arbeiter benötigen.
// Wir übergeben dafür als Parameter den Typ des Gebäudes (FARM, BAUHOF usw.) und einen Modus.
// Ist der Modus NULL, dann bekommen wir die Anzahl zurück, ist der Modus EINS, dann die RunningNumber.
USHORT CSystem::GetNumberOfWorkbuildings(WORKER::Typ nWorker, int Modus, BuildingInfoArray* buildingInfos) const
{
	// "Modus" gibt an, ob wir die Anzahl der Gebäude oder die aktuelle RunningNumber
	// des speziellen Gebäudes zurückgegeben wird
	// Modus == 0 -> Anzahl
	// Modus == 1 -> RunningNumber
	USHORT returnValue = 0;
	USHORT runningNumber = 0;
	if (Modus == 0)
	{
		if (nWorker == WORKER::FOOD_WORKER) returnValue = m_iFoodBuildings;
		else if (nWorker == WORKER::INDUSTRY_WORKER) returnValue = m_iIndustryBuildings;
		else if (nWorker == WORKER::ENERGY_WORKER) returnValue = m_iEnergyBuildings;
		else if (nWorker == WORKER::SECURITY_WORKER) returnValue = m_iSecurityBuildings;
		else if (nWorker == WORKER::RESEARCH_WORKER) returnValue = m_iResearchBuildings;
		else if (nWorker == WORKER::TITAN_WORKER) returnValue = m_iTitanMines;
		else if (nWorker == WORKER::DEUTERIUM_WORKER) returnValue = m_iDeuteriumMines;
		else if (nWorker == WORKER::DURANIUM_WORKER) returnValue = m_iDuraniumMines;
		else if (nWorker == WORKER::CRYSTAL_WORKER) returnValue = m_iCrystalMines;
		else if (nWorker == WORKER::IRIDIUM_WORKER) returnValue = m_iIridiumMines;
		else if (nWorker == WORKER::ALL_WORKER) returnValue = m_Workers.GetWorker(WORKER::ALL_WORKER);
		return returnValue;
	}
	else if (Modus == 1)
	{
		for (int i = 0; i < m_Buildings.GetSize(); i++)
		{
			const CBuildingInfo* buildingInfo = &buildingInfos->GetAt(m_Buildings.GetAt(i).GetRunningNumber() - 1);
			
			if (buildingInfo->GetWorker())
			{
				if (nWorker == WORKER::FOOD_WORKER)
				{
					if (buildingInfo->GetFoodProd())
					{
						runningNumber = m_Buildings.GetAt(i).GetRunningNumber();
						return runningNumber;
					}
				}
				else if (nWorker == WORKER::INDUSTRY_WORKER)
				{
					if (buildingInfo->GetIPProd())
					{
						runningNumber = m_Buildings.GetAt(i).GetRunningNumber();
						return runningNumber;
					}
				}
				else if (nWorker == WORKER::ENERGY_WORKER)
				{
					if (buildingInfo->GetEnergyProd())
					{
						runningNumber = m_Buildings.GetAt(i).GetRunningNumber();
						return runningNumber;
					}
				}
				else if (nWorker == WORKER::SECURITY_WORKER)
				{
					if (buildingInfo->GetSPProd())
					{
						runningNumber = m_Buildings.GetAt(i).GetRunningNumber();
						return runningNumber;
					}
				}
				else if (nWorker == WORKER::RESEARCH_WORKER)
				{
					if (buildingInfo->GetFPProd())
					{
						runningNumber = m_Buildings.GetAt(i).GetRunningNumber();
						return runningNumber;
					}
				}
				else if (nWorker == WORKER::TITAN_WORKER)
				{
					if (buildingInfo->GetTitanProd())
					{
						runningNumber = m_Buildings.GetAt(i).GetRunningNumber();
						return runningNumber;
					}
				}
				else if (nWorker == WORKER::DEUTERIUM_WORKER)
				{
					if (buildingInfo->GetDeuteriumProd())
					{
						runningNumber = m_Buildings.GetAt(i).GetRunningNumber();
						return runningNumber;
					}
				}
				else if (nWorker == WORKER::DURANIUM_WORKER)
				{
					if (buildingInfo->GetDuraniumProd())
					{
						runningNumber = m_Buildings.GetAt(i).GetRunningNumber();
						return runningNumber;
					}
				}
				else if (nWorker == WORKER::CRYSTAL_WORKER)
				{
					if (buildingInfo->GetCrystalProd())
					{
						runningNumber = m_Buildings.GetAt(i).GetRunningNumber();
						return runningNumber;
					}
				}
				else if (nWorker == WORKER::IRIDIUM_WORKER)
				{
					if (buildingInfo->GetIridiumProd())
					{
						runningNumber = m_Buildings.GetAt(i).GetRunningNumber();
						return runningNumber;
					}
				}
			}
		}
	}
	return 0;
}

// Funktion gibt die Anzahl des Gebäudes mit der übergebenen RunningNumber zurück.
USHORT CSystem::GetNumberOfBuilding(USHORT runningNumber) const
{
	USHORT number = 0;
	for (int i = 0; i < m_Buildings.GetSize(); i++)
		if (runningNumber == m_Buildings.GetAt(i).GetRunningNumber())
			number++;
	return number;
}

// Funktion gibt den Lagerinhalt der Ressource zurück, die an die Funktion übergeben wurde.
UINT CSystem::GetResourceStore(USHORT res) const
{
	switch (res)
	{
	case TITAN: {return this->GetTitanStore();}
	case DEUTERIUM: {return this->GetDeuteriumStore();}
	case DURANIUM: {return this->GetDuraniumStore();}
	case CRYSTAL: {return this->GetCrystalStore();}
	case IRIDIUM: {return this->GetIridiumStore();}
	case DERITIUM: {return this->GetDeritiumStore();}
	}
	return 0;
}

// Funktion gibt einen Zeiger auf den Lagerinhalt der Ressource zurück, die an die Funktion übergeben wurde.
UINT* CSystem::GetResourceStorages(USHORT res)
{
	switch (res)
	{
	case TITAN: {return &m_iTitanStore;}
	case DEUTERIUM: {return &m_iDeuteriumStore;}
	case DURANIUM: {return &m_iDuraniumStore;}
	case CRYSTAL: {return &m_iCrystalStore;}
	case IRIDIUM: {return &m_iIridiumStore;}
	case DERITIUM: {return &m_iDeritiumStore;}
	}
	return 0;
}

// Funktion gibt die Anzahl der aktuell in dem Feld gespeicherten Gebäude mit der RunningNumber == number zurück.
// Diese Gebäude sollen am Ende der Runde abgerissen werden.
USHORT CSystem::GetBuildingDestroy(int RunningNumber)
{
	USHORT number = 0; 
	for (int i = 0; i < m_BuildingDestroy.GetSize(); i++)
		if (m_BuildingDestroy.GetAt(i) == RunningNumber)
			number++;
	return number;
}

// Funktion sagt ob ein bestimmtes Gebäude in dem System baubar ist oder nicht. Als Parameter werden dafür
// die RunningNumber des gewünschten Gebäudes und der Wert übergeben.
void CSystem::SetBuildableBuildings(int RunningNumber, BOOLEAN TrueOrFalse)
{
	if (TrueOrFalse == FALSE)
	{
		for (int t = 0; t < m_BuildableBuildings.GetSize(); t++)
			if (m_BuildableBuildings.GetAt(t) == RunningNumber)
			{
				m_BuildableBuildings.RemoveAt(t);
				break;
			}
	}
	else
		m_BuildableBuildings.Add(RunningNumber);
}

// Funktion setzt den neuen Besitzer des Systems. Übergeben wird der Besitzer.
void CSystem::SetOwnerOfSystem(const CString& sOwnerOfSystem)
{
	// ändert sich nichts, dann aus der Funktion springen
	if (m_sOwnerOfSystem == sOwnerOfSystem)
		return;

	// neuen Besitzer festlegen
	m_sOwnerOfSystem = sOwnerOfSystem;

	// alle alten Ressourcen- und Handelsrouten löschen
	m_TradeRoutes.RemoveAll();
	m_ResourceRoutes.RemoveAll();

	// Bauliste hart löschen
	m_AssemblyList.Reset();
}

// Funktion sagt ob ein bestimmtes Gebäudeupdate in dem System baubar ist oder nicht. Als Parameter werden dafür
// die RunningNumber des gewünschten Gebäudes und der Wert übergeben.
void CSystem::SetBuildableUpdates(int RunningNumber, BOOLEAN TrueOrFalse)
{
	if (TrueOrFalse == FALSE)
	{
		for (int t = 0; t < m_BuildableUpdates.GetSize(); t++)
			if (m_BuildableUpdates.GetAt(t) == RunningNumber)
			{
				m_BuildableUpdates.RemoveAt(t);
				break;
			}
	}
	else
		m_BuildableUpdates.Add(RunningNumber);
}

// Setzt ein Gebäude mit der richtigen RunningNumber online bzw. offline
void CSystem::SetIsBuildingOnline(int index, BOOLEAN newStatus)
{
	ASSERT(index < m_Buildings.GetSize());
	m_Buildings.ElementAt(index).SetIsBuildingOnline(newStatus);
}

// Komplette Zugriffsfunktion für das Arbeiterobjekt. Bei Modus 0 wird der "WhatWorker" inkrementiert, bei Modus 2 wird
// er dekrementiert und bei Modus 2 wird der "WhatWorker" auf den Wert von Value gesetzt.
void CSystem::SetWorker(WORKER::Typ nWhatWorker, int Value, int Modus)
{
	// Modus == 0 --> Inkrement
	// Modus == 1 --> Dekrement
	// Modus == 2 --> SetWorkers inkl. Value
	if (Modus == 0)
		m_Workers.InkrementWorker(nWhatWorker);
	else if (Modus == 1)
		m_Workers.DekrementWorker(nWhatWorker);
	else if (Modus == 2)
		m_Workers.SetWorker(nWhatWorker, Value);
}

// Funktion setzt alle vorhandenen Arbeiter soweit wie möglich in Gebäude, die Arbeiter benötigen.
void CSystem::SetWorkersIntoBuildings()
{
	m_Workers.SetWorker(WORKER::ALL_WORKER, (int)(m_dHabitants));
	m_Workers.CalculateFreeWorkers();
	int numberOfWorkBuildings = 0;
	int workers = 0;
	for (int i = WORKER::FOOD_WORKER; i <= WORKER::IRIDIUM_WORKER; i++)
	{
		WORKER::Typ nWorker = (WORKER::Typ)i;
		numberOfWorkBuildings += GetNumberOfWorkbuildings(nWorker,0,NULL);
		workers += m_Workers.GetWorker(nWorker);
	}
	while (m_Workers.GetWorker(WORKER::FREE_WORKER) > 0 && workers < numberOfWorkBuildings)
	{
		for (int i = WORKER::FOOD_WORKER; i <= WORKER::IRIDIUM_WORKER; i++)
		{
			WORKER::Typ nWorker = (WORKER::Typ)i;
			if (m_Workers.GetWorker(WORKER::FREE_WORKER) > 0 && GetNumberOfWorkbuildings(nWorker,0,NULL) > m_Workers.GetWorker(nWorker))
			{
				workers++;
				m_Workers.InkrementWorker(nWorker);
				m_Workers.DekrementWorker(WORKER::FREE_WORKER);
			}
		}
	}
}

// Funktion addiert resAdd zum Lagerinhalt der jeweiligen Ressource.
void CSystem::SetResourceStore(USHORT res, int resAdd)
{
	// Überprüfung das wir bei Lagerverkleinerung keine negativen Lager bekommen können
	if (resAdd < 0)
		if ((resAdd + (int)this->GetResourceStore(res)) < 0)
			resAdd = this->GetResourceStore(res) * (-1);
	// zum Lager hinzufügen/entfernen
	switch (res)
	{
	case TITAN:		{m_iTitanStore += resAdd; break;}
	case DEUTERIUM: {m_iDeuteriumStore += resAdd; break;}
	case DURANIUM:	{m_iDuraniumStore += resAdd; break;}
	case CRYSTAL:	{m_iCrystalStore += resAdd; break;}
	case IRIDIUM:	{m_iIridiumStore += resAdd; break;}
	case DERITIUM:	{m_iDeritiumStore += resAdd; break;}
	}
}

// Diese Funktion setzt die abzureißenden Gebäude fest. Die zu übergebenden Parameter sind die RunningNumber
// des Gebäudes und einen Wert 1 für hinzufügen und 0 für wieder aus der Liste nehmen
void CSystem::SetBuildingDestroy(int RunningNumber, BOOLEAN add)
{
	// Gebäude zu Liste hinzufügen
	if (add == TRUE)
	{
		USHORT maxDestroy = this->GetNumberOfBuilding(RunningNumber);
		if (m_byBlockade > NULL)
			maxDestroy -= maxDestroy * m_byBlockade / 100;
		if (maxDestroy > this->GetBuildingDestroy(RunningNumber))
			m_BuildingDestroy.Add(RunningNumber);
	}
	else
	{
		for (int i = 0; i < m_BuildingDestroy.GetSize(); i++)
			if (m_BuildingDestroy.GetAt(i) == RunningNumber)
			{
				m_BuildingDestroy.RemoveAt(i);
				break;
			}
	}
}

// Funktion setzt die Bevölkerungsanzahl des Systems. Übergeben wird die Bevölkerung aller Planeten des Sektors.
// Gleichzeitig überprüft die Funktion auch, ob man eine weitere Handelsroute aufgrund der Bevölkerung bekommt, dann
// gibt die Funktion ein <code>TRUE</code> zurück, ansonsten <code>FALSE</code>.
BOOLEAN CSystem::SetHabitants(double habitants)
{
	m_dHabitants = habitants;

	if (((USHORT)m_dHabitants / TRADEROUTEHAB) > m_byMaxTradeRoutesFromHab)
	{
		m_byMaxTradeRoutesFromHab++;
		if (m_sOwnerOfSystem != "")
			return TRUE;
	}
	// Wenn die Bevölkerung wieder zu niedrig ist, dann die Varibale wieder runterzählen
	else if (((USHORT)m_dHabitants / TRADEROUTEHAB) < m_byMaxTradeRoutesFromHab)
		m_byMaxTradeRoutesFromHab--;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
// Funktion berechnet aus den Eigenschaften der stehenden Gebäude alle Attribute der System-Klasse.
void CSystem::CalculateVariables(BuildingInfoArray* buildingInfos, CResearchInfo* ResearchInfo, CArray<CPlanet>* planets, CMajor* pOwner, const CString *sMonopolOwner)
{
	int NumberOfBuildings;
	NumberOfBuildings = m_Buildings.GetSize();
	// Alle werde wieder auf NULL setzen	
	m_Production.Reset();
	// Die Anzahl der Arbeiter aus der aktuellen Bevölkerung berechnen und auch an die Klasse CWorker übergeben
	m_Workers.SetWorker(WORKER::ALL_WORKER,(int)(m_dHabitants));
	m_Workers.CheckWorkers();
	// Die Creditsprod. aus der Bevölkerung berechnen und modifizieren durch jeweilige Rasseneigenschaft
	m_Production.m_iCreditsProd = (int)(m_dHabitants);

	float fCreditsMulti = 1.0f;
	if (pOwner->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
		fCreditsMulti += 0.5f;
	if (pOwner->IsRaceProperty(RACE_PROPERTY::PRODUCER))
		fCreditsMulti += 0.25f;
	if (pOwner->IsRaceProperty(RACE_PROPERTY::WARLIKE) || pOwner->IsRaceProperty(RACE_PROPERTY::HOSTILE))
		fCreditsMulti -= 0.1f;
	if (pOwner->IsRaceProperty(RACE_PROPERTY::SNEAKY))
		fCreditsMulti -= 0.2f;
	if (pOwner->IsRaceProperty(RACE_PROPERTY::SECRET))
		fCreditsMulti -= 0.25f;
	if (pOwner->IsRaceProperty(RACE_PROPERTY::SOLOING))
		fCreditsMulti -= 0.5f;

	fCreditsMulti = max(fCreditsMulti, 0.0f);
	m_Production.m_iCreditsProd = (int)(m_Production.m_iCreditsProd * fCreditsMulti);
	// Die Gebäude online setzen, wenn das Objekt der Klasse CWorker das sagt
	// zuerst die Anzahl der Arbeiter auslesen und schauen ob die Arbeiter vielleicht größer sind als die 
	// Anzahl der jeweiligen Gebäude (z.B. durch Abriß aus letzter Runde) -> dann Arbeiter auf Gebäudeanzahl verringern
	if (m_Workers.GetWorker(WORKER::FOOD_WORKER) > m_iFoodBuildings) m_Workers.SetWorker(WORKER::FOOD_WORKER,m_iFoodBuildings);
	unsigned short foodWorker = m_Workers.GetWorker(WORKER::FOOD_WORKER);
	if (m_Workers.GetWorker(WORKER::INDUSTRY_WORKER) > m_iIndustryBuildings) m_Workers.SetWorker(WORKER::INDUSTRY_WORKER,m_iIndustryBuildings);
	unsigned short industryWorker = m_Workers.GetWorker(WORKER::INDUSTRY_WORKER);
	if (m_Workers.GetWorker(WORKER::ENERGY_WORKER) > m_iEnergyBuildings) m_Workers.SetWorker(WORKER::ENERGY_WORKER,m_iEnergyBuildings);
	unsigned short energyWorker = m_Workers.GetWorker(WORKER::ENERGY_WORKER);
	if (m_Workers.GetWorker(WORKER::SECURITY_WORKER) > m_iSecurityBuildings) m_Workers.SetWorker(WORKER::SECURITY_WORKER,m_iSecurityBuildings);
	unsigned short securityWorker = m_Workers.GetWorker(WORKER::SECURITY_WORKER);
	if (m_Workers.GetWorker(WORKER::RESEARCH_WORKER) > m_iResearchBuildings) m_Workers.SetWorker(WORKER::RESEARCH_WORKER,m_iResearchBuildings);
	unsigned short researchWorker = m_Workers.GetWorker(WORKER::RESEARCH_WORKER);
	if (m_Workers.GetWorker(WORKER::TITAN_WORKER) > m_iTitanMines) m_Workers.SetWorker(WORKER::TITAN_WORKER,m_iTitanMines);
	unsigned short titanWorker = m_Workers.GetWorker(WORKER::TITAN_WORKER);
	if (m_Workers.GetWorker(WORKER::DEUTERIUM_WORKER) > m_iDeuteriumMines) m_Workers.SetWorker(WORKER::DEUTERIUM_WORKER,m_iDeuteriumMines);
	unsigned short deuteriumWorker = m_Workers.GetWorker(WORKER::DEUTERIUM_WORKER);
	if (m_Workers.GetWorker(WORKER::DURANIUM_WORKER) > m_iDuraniumMines) m_Workers.SetWorker(WORKER::DURANIUM_WORKER,m_iDuraniumMines);
	unsigned short duraniumWorker = m_Workers.GetWorker(WORKER::DURANIUM_WORKER);
	if (m_Workers.GetWorker(WORKER::CRYSTAL_WORKER) > m_iCrystalMines) m_Workers.SetWorker(WORKER::CRYSTAL_WORKER,m_iCrystalMines);
	unsigned short crystalWorker = m_Workers.GetWorker(WORKER::CRYSTAL_WORKER);
	if (m_Workers.GetWorker(WORKER::IRIDIUM_WORKER) > m_iIridiumMines) m_Workers.SetWorker(WORKER::IRIDIUM_WORKER,m_iIridiumMines);
	unsigned short iridiumWorker = m_Workers.GetWorker(WORKER::IRIDIUM_WORKER);
	
	// Wenn wir Handelsgüter in der Bauliste stehen haben, dann Anzahl der Online-Fabs in Credits umrechnen
	if (m_AssemblyList.GetAssemblyListEntry(0) == 0)
		m_Production.m_iCreditsProd += industryWorker;

	// Die einzelnen Produktionen berechnen
	for (int i = 0; i < NumberOfBuildings; i++)
	{
		const CBuildingInfo* buildingInfo = &buildingInfos->GetAt(m_Buildings.GetAt(i).GetRunningNumber() - 1);

		// Bei einer Blockade werden auch die Werften offline gesetzt
		if (m_byBlockade >= 100 && buildingInfo->GetShipYard() && buildingInfo->GetNeededEnergy() > 0)
			m_Buildings.GetAt(i).SetIsBuildingOnline(FALSE);

		// Gebäude offline setzen
		if (buildingInfo->GetWorker() == TRUE)
		{
			m_Buildings.ElementAt(i).SetIsBuildingOnline(FALSE);
			// Jetzt wieder wenn möglich online setzen		
			if (buildingInfo->GetFoodProd() > 0 && foodWorker > 0)
			{
				m_Buildings.ElementAt(i).SetIsBuildingOnline(TRUE);
				foodWorker--;
			}
			else if (buildingInfo->GetIPProd() > 0 && industryWorker > 0)
			{
				m_Buildings.ElementAt(i).SetIsBuildingOnline(TRUE);
				industryWorker--;
			}
			else if (buildingInfo->GetEnergyProd() > 0 && energyWorker > 0)
			{
				m_Buildings.ElementAt(i).SetIsBuildingOnline(TRUE);
				energyWorker--;
			}
			else if (buildingInfo->GetSPProd() > 0 && securityWorker > 0)
			{
				m_Buildings.ElementAt(i).SetIsBuildingOnline(TRUE);
				securityWorker--;
			}
			else if (buildingInfo->GetFPProd() > 0 && researchWorker > 0)
			{
				m_Buildings.ElementAt(i).SetIsBuildingOnline(TRUE);
				researchWorker--;
			}
			else if (buildingInfo->GetTitanProd() > 0 && titanWorker > 0)
			{
				m_Buildings.ElementAt(i).SetIsBuildingOnline(TRUE);
				titanWorker--;
			}
			else if (buildingInfo->GetDeuteriumProd() > 0 && deuteriumWorker > 0)
			{
				m_Buildings.ElementAt(i).SetIsBuildingOnline(TRUE);
				deuteriumWorker--;
			}
			else if (buildingInfo->GetDuraniumProd() > 0 && duraniumWorker > 0)
			{
				m_Buildings.ElementAt(i).SetIsBuildingOnline(TRUE);
				duraniumWorker--;
			}
			else if (buildingInfo->GetCrystalProd() > 0 && crystalWorker > 0)
			{
				m_Buildings.ElementAt(i).SetIsBuildingOnline(TRUE);
				crystalWorker--;
			}
			else if (buildingInfo->GetIridiumProd() > 0 && iridiumWorker > 0)
			{
				m_Buildings.ElementAt(i).SetIsBuildingOnline(TRUE);
				iridiumWorker--;
			}
		}
		
		// Die einzelnen Produktionen berechnen (ohne Boni)
		// vorher noch schauen, ob diese Gebäude auch online sind
		if (m_Buildings.GetAt(i).GetIsBuildingOnline() == TRUE)
			m_Production.CalculateProduction(buildingInfo);
	}

	// falls vorhanden, deaktiverte Produktionen auf 0 setzen
	m_Production.DisableProductions(m_bDisabledProductions);

	// Credits durch Handelsrouten berechnen
	m_Production.m_iCreditsProd += CreditsFromTradeRoutes();

	// Besitzt jemand ein Monopol auf eine Ressource, so verdoppelt sich seine Produktion
	if (sMonopolOwner[TITAN] == m_sOwnerOfSystem)
		m_Production.m_iTitanProd *= 2;
	if (sMonopolOwner[DEUTERIUM] == m_sOwnerOfSystem)
		m_Production.m_iDeuteriumProd *= 2;
	if (sMonopolOwner[DURANIUM] == m_sOwnerOfSystem)
		m_Production.m_iDuraniumProd *= 2;
	if (sMonopolOwner[CRYSTAL] == m_sOwnerOfSystem)
		m_Production.m_iCrystalProd *= 2;
	if (sMonopolOwner[IRIDIUM] == m_sOwnerOfSystem)
		m_Production.m_iIridiumProd *= 2;
	
	// Die Boni auf die einzelnen Produktionen berechnen
	short tmpFoodBoni		= pOwner->GetEmpire()->GetResearch()->GetBioTech() * TECHPRODBONUS;
	short tmpIndustryBoni	= pOwner->GetEmpire()->GetResearch()->GetConstructionTech() * TECHPRODBONUS;
	short tmpEnergyBoni		= pOwner->GetEmpire()->GetResearch()->GetEnergyTech() * TECHPRODBONUS;
	short tmpSecurityBoni	= 0;
	short tmpResearchBoni	= 0;
	short tmpTitanBoni		= 0;
	short tmpDeuteriumBoni	= 0;
	short tmpDuraniumBoni	= 0;
	short tmpCrystalBoni	= 0;
	short tmpIridiumBoni	= 0;
	short tmpDeritiumBoni	= 0;
	short tmpCreditsBoni	= 0;
	short tmpAllRessourcesBoni = 0;

	short neededEnergy = 0;
	for (int i = 0; i < NumberOfBuildings; i++)
	{
		const CBuildingInfo* buildingInfo = &buildingInfos->GetAt(m_Buildings.GetAt(i).GetRunningNumber() - 1);

		// Hier die nötige Energie von der produzierten abziehen, geht aber nur hier, wenn wir keine Boni zur Energie reinmachen
		if (m_Buildings.GetAt(i).GetIsBuildingOnline() == TRUE && buildingInfo->GetNeededEnergy() > 0)
			//m_Production.m_iEnergyProd -= m_Buildings.GetAt(i).GetNeededEnergy();
			neededEnergy += buildingInfo->GetNeededEnergy();

		if (m_Buildings.GetAt(i).GetIsBuildingOnline() == TRUE)
		{
			// Es wird IMMER abgerundet, gemacht durch "floor"
			tmpFoodBoni			+= buildingInfo->GetFoodBoni();
			tmpIndustryBoni		+= buildingInfo->GetIndustryBoni();
			tmpEnergyBoni		+= buildingInfo->GetEnergyBoni();
			tmpSecurityBoni		+= buildingInfo->GetSecurityBoni();
			tmpResearchBoni		+= buildingInfo->GetResearchBoni();
			tmpAllRessourcesBoni = buildingInfo->GetAllRessourcesBoni();
			tmpTitanBoni		+= buildingInfo->GetTitanBoni() + tmpAllRessourcesBoni;
			tmpDeuteriumBoni	+= buildingInfo->GetDeuteriumBoni() + tmpAllRessourcesBoni;
			tmpDuraniumBoni		+= buildingInfo->GetDuraniumBoni() + tmpAllRessourcesBoni;
			tmpCrystalBoni		+= buildingInfo->GetCrystalBoni() + tmpAllRessourcesBoni;
			tmpIridiumBoni		+= buildingInfo->GetIridiumBoni() + tmpAllRessourcesBoni;
			tmpDeritiumBoni	+= buildingInfo->GetDeritiumBoni(); 
			tmpCreditsBoni		+= buildingInfo->GetCreditsBoni();
		}
	}
	
	// Jetzt werden noch eventuelle Boni durch die Planetenklassen dazugerechnet
	BYTE deritiumProdMulti = 0;
	for (int i = 0; i < planets->GetSize(); i++)
	{
		if (planets->GetAt(i).GetColonized() == TRUE && planets->GetAt(i).GetCurrentHabitant() > 0.0f)
		{
			// pro Planetengröße gibt es 25% Bonus
			if (planets->GetAt(i).GetBoni()[TITAN] == TRUE)
				tmpTitanBoni	+= (planets->GetAt(i).GetSize()+1) * 25;
			if (planets->GetAt(i).GetBoni()[DEUTERIUM] == TRUE)
				tmpDeuteriumBoni+= (planets->GetAt(i).GetSize()+1) * 25;
			if (planets->GetAt(i).GetBoni()[DURANIUM] == TRUE)
				tmpDuraniumBoni	+= (planets->GetAt(i).GetSize()+1) * 25;
			if (planets->GetAt(i).GetBoni()[CRYSTAL] == TRUE)
				tmpCrystalBoni	+= (planets->GetAt(i).GetSize()+1) * 25;
			if (planets->GetAt(i).GetBoni()[IRIDIUM] == TRUE)
				tmpIridiumBoni	+= (planets->GetAt(i).GetSize()+1) * 25;
			if (planets->GetAt(i).GetBoni()[6] == TRUE)	// food
				tmpFoodBoni		+= (planets->GetAt(i).GetSize()+1) * 25;
			if (planets->GetAt(i).GetBoni()[7] == TRUE)	// energy
				tmpEnergyBoni	+= (planets->GetAt(i).GetSize()+1) * 25;
			// Menge des abgebauten Deritiums mit der Anzahl der kolonisierten Planeten mit Deritiumvorkommen
			// multiplizieren
			if (planets->GetAt(i).GetBoni()[DERITIUM] == TRUE)
				deritiumProdMulti += 1;
		}
	}
	m_Production.m_iDeritiumProd	*= deritiumProdMulti;

	m_Production.m_iFoodProd		+= (int)(tmpFoodBoni*m_Production.m_iFoodProd/100);
	m_Production.m_iIndustryProd	+= (int)(tmpIndustryBoni*m_Production.m_iIndustryProd/100);
	m_Production.m_iEnergyProd		+= (int)(tmpEnergyBoni*m_Production.m_iEnergyProd/100);
	m_Production.m_iSecurityProd	+= (int)(tmpSecurityBoni*m_Production.m_iSecurityProd/100);
	m_Production.m_iResearchProd	+= (int)(tmpResearchBoni*m_Production.m_iResearchProd/100);
	m_Production.m_iTitanProd		+= (int)(tmpTitanBoni*m_Production.m_iTitanProd/100);
	m_Production.m_iDeuteriumProd	+= (int)(tmpDeuteriumBoni*m_Production.m_iDeuteriumProd/100);
	m_Production.m_iDuraniumProd	+= (int)(tmpDuraniumBoni*m_Production.m_iDuraniumProd/100);
	m_Production.m_iCrystalProd		+= (int)(tmpCrystalBoni*m_Production.m_iCrystalProd/100);
	m_Production.m_iIridiumProd		+= (int)(tmpIridiumBoni*m_Production.m_iIridiumProd/100);
	m_Production.m_iDeritiumProd	+= (int)(tmpDeritiumBoni*m_Production.m_iDeritiumProd/100);
	m_Production.m_iCreditsProd		+= (int)(tmpCreditsBoni*m_Production.m_iCreditsProd/100);

	// Wenn das System blockiert wird, dann verringern sich bestimmte Produktionswerte
	if (m_byBlockade > NULL)
	{
		//m_Production.m_iFoodProd		-= (int)(m_byBlockade * m_Production.m_iFoodProd/100);
		m_Production.m_iIndustryProd	-= (int)(m_byBlockade * m_Production.m_iIndustryProd/100);
		//m_Production.m_iEnergyProd	-= (int)(m_byBlockade * m_Production.m_iEnergyProd/100);
		m_Production.m_iSecurityProd	-= (int)(m_byBlockade * m_Production.m_iSecurityProd/100);
		m_Production.m_iResearchProd	-= (int)(m_byBlockade * m_Production.m_iResearchProd/100);
		m_Production.m_iTitanProd		-= (int)(m_byBlockade * m_Production.m_iTitanProd/100);
		m_Production.m_iDeuteriumProd	-= (int)(m_byBlockade * m_Production.m_iDeuteriumProd/100);
		m_Production.m_iDuraniumProd	-= (int)(m_byBlockade * m_Production.m_iDuraniumProd/100);
		m_Production.m_iCrystalProd		-= (int)(m_byBlockade * m_Production.m_iCrystalProd/100);
		m_Production.m_iIridiumProd		-= (int)(m_byBlockade * m_Production.m_iIridiumProd/100);
		m_Production.m_iDeritiumProd	-= (int)(m_byBlockade * m_Production.m_iDeritiumProd/100);
		m_Production.m_iCreditsProd		-= (int)(m_byBlockade * m_Production.m_iCreditsProd/100);
		if (m_byBlockade >= 100)
			m_Production.m_bShipYard	= false;
	}

	// Hier noch Boni auf das Spezialzeug geben (z.B. Scanpower usw.)
	m_Production.m_iScanPower		+= (int)(m_Production.m_iScanPowerBoni*m_Production.m_iScanPower/100);
	m_Production.m_iScanRange		+= (int)(m_Production.m_iScanRangeBoni*m_Production.m_iScanRange/100);
	m_Production.m_iGroundDefend	+= (int)(m_Production.m_iGroundDefendBoni*m_Production.m_iGroundDefend/100);
	m_Production.m_iShipDefend		+= (int)(m_Production.m_iShipDefendBoni*m_Production.m_iShipDefend/100);
	m_Production.m_iShieldPower		+= (int)(m_Production.m_iShieldPowerBoni*m_Production.m_iShieldPower/100);

	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Hier die Boni durch die Uniqueforschung "Wirtschaft" -> 10% mehr Industrie
	if (ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		m_Production.m_iIndustryProd += (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetBonus(1)*m_Production.m_iIndustryProd/100);
	else if (ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
		m_Production.m_iCreditsProd += (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetBonus(3) * m_Production.m_iCreditsProd / 100);
	// Hier die Boni durch die Uniqueforschung "Produktion"
	if (ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PRODUCTION)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		m_Production.m_iFoodProd += (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PRODUCTION)->GetBonus(1)*m_Production.m_iFoodProd/100); 
	else if (ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PRODUCTION)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
	{
		m_Production.m_iTitanProd		+= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PRODUCTION)->GetBonus(2)*m_Production.m_iTitanProd/100);
		m_Production.m_iDeuteriumProd	+= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PRODUCTION)->GetBonus(2)*m_Production.m_iDeuteriumProd/100);
		m_Production.m_iDuraniumProd	+= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PRODUCTION)->GetBonus(2)*m_Production.m_iDuraniumProd/100);
		m_Production.m_iCrystalProd		+= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PRODUCTION)->GetBonus(2)*m_Production.m_iCrystalProd/100);
		m_Production.m_iIridiumProd		+= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PRODUCTION)->GetBonus(2)*m_Production.m_iIridiumProd/100);
	}
	// Wenn wir die Uniqueforschung "Produktion" gewählt haben, und dort mehr Energie haben wollen -> 20% mehr!
	else if (ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PRODUCTION)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
		m_Production.m_iEnergyProd += (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PRODUCTION)->GetBonus(3)*m_Production.m_iEnergyProd/100); 
	
	// Hier die Boni durch die Uniqueforschung "Forschung und Sicherheit"
	if (ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::DEVELOPMENT_AND_SECURITY)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		m_Production.m_iResearchProd += (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::DEVELOPMENT_AND_SECURITY)->GetBonus(1)*m_Production.m_iResearchProd/100);
	else if (ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::DEVELOPMENT_AND_SECURITY)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
		m_Production.m_iSecurityProd += (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::DEVELOPMENT_AND_SECURITY)->GetBonus(1)*m_Production.m_iSecurityProd/100);
	else if (ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::DEVELOPMENT_AND_SECURITY)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
	{
		m_Production.m_iResearchProd += (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::DEVELOPMENT_AND_SECURITY)->GetBonus(3)*m_Production.m_iResearchProd/100);
		m_Production.m_iSecurityProd += (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::DEVELOPMENT_AND_SECURITY)->GetBonus(3)*m_Production.m_iSecurityProd/100);
	}
	
	// Maximalenergie, also hier noch ohne Abzüge durch energiebedürftige Gebäude
	m_Production.m_iMaxEnergyProd = m_Production.m_iEnergyProd;
	// hier die gesamte Energie durch energiebedürftige Gebäude abziehen
	m_Production.m_iEnergyProd -= neededEnergy;
	
	// imperiumweite Moralprod mit aufrechnen
	m_Production.m_iMoralProd += m_Production.m_iMoralProdEmpireWide[m_sOwnerOfSystem];
	// Den Moralboni im System noch auf die einzelnen Produktionen anrechnen
	m_Production.IncludeSystemMoral(m_iMoral);
	// benötigte Nahrung durch Bevölkerung von der Produktion abiehen
	if (!pOwner->HasSpecialAbility(SPECIAL_NEED_NO_FOOD))
		// ceil, wird auf Kommezahl berechnet, z.B brauchen wir für 14.5 Mrd. Leute 145 Nahrung und nicht 140 bzw. 150
		m_Production.m_iFoodProd -= (int)ceil(m_dHabitants*10);
	else
		m_Production.m_iFoodProd = m_Production.m_iMaxFoodProd;

	// Jetzt noch die freien Arbeiter berechnen
	m_Workers.CalculateFreeWorkers();
}

// Funktion berechnet die Lagerinhalte des Systems. Aufrufen bei Ende bzw. Beginn einer neuen Runde.
// Gibt die Funktion TRUE zurück hat sich das System Aufgrund zu schlechter Moral vom Besitzer losgesagt.
BOOLEAN CSystem::CalculateStorages(CResearchInfo* researchInfo, int diliAdd)
{
	// wie oben gesagt, noch überarbeiten, wenn negatives Lager sein würdem dann darf das System nicht mehr wachsen
	// bzw. wird kleiner!!!
	m_iFoodStore += m_Production.m_iFoodProd;
	m_iTitanStore += m_Production.m_iTitanProd;
	m_iDeuteriumStore += m_Production.m_iDeuteriumProd;
	m_iDuraniumStore += m_Production.m_iDuraniumProd;
	m_iCrystalStore += m_Production.m_iCrystalProd;
	m_iIridiumStore += m_Production.m_iIridiumProd;
	m_iDeritiumStore += m_Production.m_iDeritiumProd + diliAdd;
	
	// Lagerobergrenzen
	if (m_iFoodStore > MAX_FOOD_STORE)
		m_iFoodStore = MAX_FOOD_STORE;
	if (m_iTitanStore > MAX_RES_STORE)
		m_iTitanStore = MAX_RES_STORE;
	if (m_iDeuteriumStore > MAX_RES_STORE)
		m_iDeuteriumStore = MAX_RES_STORE;
	if (m_iDuraniumStore > MAX_RES_STORE)
		m_iDuraniumStore = MAX_RES_STORE;
	if (m_iCrystalStore > MAX_RES_STORE)
		m_iCrystalStore = MAX_RES_STORE;
	if (m_iIridiumStore > MAX_RES_STORE)
		m_iIridiumStore = MAX_RES_STORE;
	
	short multi = 1;
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Hier die Boni durch die Uniqueforschung "Lager und Transport" -> doppeltes Deritiumlager
	if (researchInfo->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		multi = researchInfo->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetBonus(1);
	if ((int)m_iDeritiumStore > MAX_DERITIUM_STORE * multi)
		m_iDeritiumStore = MAX_DERITIUM_STORE * multi;

	m_iMoral += (short)m_Production.m_iMoralProd;
	
	// Wenn wir eine Moral von -1 haben, senkt sich der Wert auf maximal noch 85
	if (m_iMoral < 85 && m_Production.m_iMoralProd == -1)
		m_iMoral = 85;
	// Wenn wir eine Moral von -2 haben, senkt sich der Wert auf maximal noch 70
	else if (m_iMoral < 70 && m_Production.m_iMoralProd == -2)
		m_iMoral = 70;

	// Moralanhebung bzw. -absenkung mal als "Lager" gesehen -> Moral, die ins Lager/System kommt
	// hier noch berechnen, das Moral von Stufen abhängt
	if (m_iMoral <= 30) m_iMoral -= 4;
	else if (m_iMoral <= 40) m_iMoral -= 3;
	else if (m_iMoral <= 50) m_iMoral -= 2;
	else if (m_iMoral <= 60) m_iMoral -= 1;
	else if (m_iMoral >= 190) m_iMoral -= 5;
	else if (m_iMoral >= 173) m_iMoral -= 4;
	else if (m_iMoral >= 152) m_iMoral -= 3;
	else if (m_iMoral >= 121) m_iMoral -= 2;
	else if (m_iMoral >= 110) m_iMoral -= 1;

	// ohne weiter Moralprod gehts wieder auf 100 zurück, wenn die Moral nicht größer 110 ist
	if (m_iMoral > 100 && m_iMoral <= 109 && m_Production.m_iMoralProd == 0) m_iMoral--; 
	if (m_iMoral > 200) m_iMoral -= 6;
	if (m_iMoral < 0) m_iMoral = 0;
	if (m_iMoral > 200) m_iMoral = 200;
	
	// Jetzt überprüfen, ob das System sich bei einer Moral von unter 30 vom Imperium lossagt
	if (m_iMoral < 30)
	{
		// zufällig, nicht zwangsläufig sagt es sich los, erst bei einer Moral von 8
		USHORT random = rand()%(m_iMoral+1);
		if (random < 8)
		{
			m_sOwnerOfSystem = "";
			m_Production.m_iFoodProd = 10;
			m_Production.m_iIndustryProd = 5;	
			m_iMoral = 65;
			if (m_iFoodStore < 0)
				m_iFoodStore = 0;
			return TRUE;		// Das System hat sich losgesagt
		}
	}
	return FALSE;
}

// Funktion löscht alle Gebäude, die die übergebene RunningNumber haben und gibt deren Anzahl zurück.
// -> Danach muß AddBuilding() mit dem Nachfolger gleich der Anzahl aufgerufen werden.
int CSystem::UpdateBuildings(int RunningNumber)
{
	int counter = 0;
	for (int i = 0; i < m_Buildings.GetSize(); i++)
		if (m_Buildings.GetAt(i).GetRunningNumber() == RunningNumber)
		{
			m_Buildings.RemoveAt(i--);
			counter++;
		}
	return counter;
}

// Funktion reißt alle Gebäude ab, die in der Variable m_BuildingDestroy stehen. Funktion wird in der Doc 
// bei NextRound() aufgerufen.
bool CSystem::DestroyBuildings(void)
{
	bool destroy = !m_BuildingDestroy.IsEmpty();
	for (int i = 0; i < m_BuildingDestroy.GetSize(); i++)
		// Schleife von hinten durchlaufen, weil Arbeiter die Gebäude von "vorn" besetzten
		// und ich nicht gerade die online-Gebäude abreißen will, sondern die wahrscheinlich offline
		for (int j = m_Buildings.GetUpperBound(); j >= 0; j--)
			if (m_Buildings.GetAt(j).GetRunningNumber() == m_BuildingDestroy.GetAt(i))
			{
				m_Buildings.RemoveAt(j);
				m_BuildingDestroy.RemoveAt(i--);
				break;
			}
	if (destroy)
		m_BuildingDestroy.RemoveAll();		
	return destroy;
}

// Funktion berechnet die baubaren Gebäude und Gebäudeupdates in dem System.
void CSystem::CalculateBuildableBuildings(CSector* sector, BuildingInfoArray* buildingInfo, CMajor* pMajor, CGlobalBuildings* globals)
{
	CEmpire* empire = pMajor->GetEmpire();
	ASSERT(empire);

	m_BuildableWithoutAssemblylistCheck.RemoveAll();
	m_BuildableWithoutAssemblylistCheck.FreeExtra();
	m_BuildableBuildings.RemoveAll();	
/*
	Algorithmus:

	1. schon stehende Gebäude und Gebäude in der Liste der immer baubaren Gebäude 
	   durchgehen und stehende Gebäude gleichzeitig nach Voraussetzungen prüfen
		1.1 - JA)   können dieses bauen und Punkt (0.) durchführen
		1.2 - NEIN) können es nicht bauen

	2. Gebäudeinfos durchgehen, nach allg. Voraussetzungen checken und prüfen, dass
	   diese ID oder ein Äquivalent NICHT schon in der Liste der baubaren Gebäude sind
		2.1 - JA)   Wenn Gebäude eine ID des Vorgängers besitzt
			2.1.1 - JA)   Schauen ob dieses Vorgängergebäude im System steht
				2.1.1.1 - JA)	Gebäudeupgrade in die Liste nehmen
				2.1.1.2 - NEIN) können es nicht bauen
			2.1.2 - NEIN) Überprüfen ob ein Nachfolger des Gebäudes in der Liste der baubaren Gebäude steht (*)
				2.1.2.1 - JA)	können es nicht bauen
				2.1.2.2 - NEIN) Überprüfen ob ein Nachfolger des Gebäudes in dem System steht (*)
					2.1.2.2.1 - JA)	  können es nicht bauen
					2.1.2.2.1 - NEIN) können es bauen
		2.2 - NEIN) können es nicht bauen

	(*) -> nur wenn dieses Gebäude auch updatebar ist!!! Ansonsten können wir es bauen

	3. zusätzlichen Baulistencheck durchführen, d.h wenn es z.B. nur einmal pro Imperium
	   baubar ist, dann darf es sich auch in keiner Bauliste befinden. Auch Updates im System
	   dürfen nicht schon in der Bauliste stehen.
		3.1 - JA)   Auftrag bleibt baubar
		3.2 - NEIN) Auftrag ist nicht mehr baubar

	0. mindst. baubare Gebäude durchgehen und checken ob unsere ID in der Liste vorkommt
	0.1 - JA)   nichts weiter machen
	0.2 - NEIN) möglichen Vorgänger unseres Gebäudes in der Liste suchen. Gefunden?
		0.2.1 - JA)   Vorgänger ID mit unserer ID ersetzen
		0.2.2 - NEIN) Unserer ID in die Liste einschreiben
		
	// gut wäre eine Variable, die alle ID´s speichert, die wir vor dem Baulistencheck bauen können.
	// Wenn wir dann einen Baulistencheck machen, bleibt diese Variable davon unberührt. Nur die Variablen,
	// in denen die nun baubaren Aufträge stehen werden verändert. Wenn ich einen Auftrag in die Assemblylist
	// nehme, wird der Baulistencheck ausgeführt. Auch wenn ich diesen wieder aus der Bauliste nehme wird der
	// Baulistencheck ausgeführt. Nun könnte man noch einbauen, das dieser nicht gleich für jedes Gebäude 
	// gemacht wird, sondern nur bei denen, bei denen es notwendig ist.
*/
// ------------------------------------ 1 -------------------------------------------- //
	// schon stehende Gebäude checken
	USHORT minID = 0;
	BOOLEAN equivalents = FALSE;	// steht in dem System ein Gebäude einer anderen Rasse (eins was wir nicht hätten bauen können)

	for (int i = 0; i < m_Buildings.GetSize(); i++)
	{
		// Wenn wir auf das Gebäude in der Infoliste zugreifen wollen, dann steht dieses dort immer
		// an der Stelle ID-1, weil das Feld bei 0 beginnt und unsere ID's aber bei 1 anfangen!!!
		USHORT id = m_Buildings.GetAt(i).GetRunningNumber();
		BOOLEAN found = FALSE;
		// Da das Feld der Gebäude im System aufsteigend nach der RunningNumber sortiert ist, brauchen wir immer erst
		// checken, wenn wir eine neue RunningNumber haben
		if (id > minID)
		{
			// steht hier ein Gebäude einer anderen Rasse?
			if (buildingInfo->GetAt(id-1).GetOwnerOfBuilding() != pMajor->GetRaceBuildingNumber())
				equivalents = TRUE;
			minID = id;
			// Checken das das Gebäude nicht schon in der Liste der baubaren Gebäude vorkommt
			for (int k = 0; k < m_BuildableWithoutAssemblylistCheck.GetSize(); k++)
				if (m_BuildableWithoutAssemblylistCheck.GetAt(k) == id)
				{
					found = TRUE;
					break;
				}
			// Checken ob wir es durch die vorhanden Planeten bauen können
			if (found == FALSE && CheckPlanet(&buildingInfo->GetAt(id-1),sector))
				// allg. Voraussetzungen des Gebäudes checken
				if (CheckGeneralConditions(&buildingInfo->GetAt(id-1),sector,globals,pMajor))
				{
					// Wenn wir hier angekommen sind, dann könnten wir das Gebäude bauen.
					// Ist es ein Gebäude, welches wir manchmal mindst immer bauen können (alle Gebäude, die Arbeiter
					// benötigen!!!), dann müssen wir noch Punkt (0.) durchführen.
					BOOLEAN id_in_list = FALSE;
					const CBuildingInfo* info = &buildingInfo->GetAt(m_Buildings.GetAt(i).GetRunningNumber() - 1);

					if (info->GetWorker())
					{
						for (int j = 0; j < m_AllwaysBuildableBuildings.GetSize(); j++)
							if (id == m_AllwaysBuildableBuildings.GetAt(j))
							{
								id_in_list = TRUE;	// gefunden -> können abbrechen
								break;
							}
						// Wir haben unseres Gebäude nicht schon in der Liste der immer baubaren Gebäude gefunden,
						// also mit Punkt (0.2) fortfahren!
						if (id_in_list == FALSE && buildingInfo->GetAt(id-1).GetPredecessorID() != 0)
						{
							BOOLEAN found_predecessor = FALSE;
							// Vorgänger unseres Gebäudes finden (es werden noch nicht alle möglichen Vorgänger gesucht!!!)
							for (int j = 0; j < m_AllwaysBuildableBuildings.GetSize(); j++)
								if (m_AllwaysBuildableBuildings.GetAt(j) == buildingInfo->GetAt(id-1).GetPredecessorID())
								{
									m_AllwaysBuildableBuildings.SetAt(j, id);
									found_predecessor = TRUE;
									break;
								}
							// Haben wir keinen Vorgänger gefunden, so fügen wir dieses Gebäude ein
							if (found_predecessor == FALSE)
								m_AllwaysBuildableBuildings.Add(id);
						}
					}
					// Nachdem wir Punkt (0.) durchgeführt haben fügen wir unser jetziges Gebäude noch in die Liste 
					// der baubaren Gebäude und Updates ein (ohne Baulistencheck)
					m_BuildableWithoutAssemblylistCheck.Add(id);
				}
		}
	}

	// Hier noch die Gebäude aus der immer baubaren Gebäudeliste in die Liste der baubaren Gebäude hinzufügen,
	// wenn sie nicht schon drin stehen
	USHORT size = m_BuildableWithoutAssemblylistCheck.GetSize();
	for (int i = 0; i < m_AllwaysBuildableBuildings.GetSize(); i++)
	{
		// Gibt es in der Liste der immer baubaren Gebäude ein Gebäude einer anderen Rasse?
		if (buildingInfo->GetAt(m_AllwaysBuildableBuildings.GetAt(i)-1).GetOwnerOfBuilding() != pMajor->GetRaceBuildingNumber())
			equivalents = TRUE;
		BOOLEAN found = FALSE;
		for (int j = 0; j < size; j++)
			if (m_AllwaysBuildableBuildings.GetAt(i) == m_BuildableWithoutAssemblylistCheck.GetAt(j))
			{
				found = TRUE;
				break;
			}
		if (found == FALSE)
			m_BuildableWithoutAssemblylistCheck.Add(m_AllwaysBuildableBuildings.GetAt(i));
	}	

// ------------------------------------ 2 -------------------------------------------- //
	// alle anderen Gebäudeinfos durchgehen
	for (int i = 0; i < buildingInfo->GetSize(); i++)
	{
		// Überprüfen, dass dieses Gebäude nicht schon in der Liste der 
		// baubaren Gebäude und Updates vorhanden ist
		BOOLEAN found = FALSE;
		for (int j = 0; j < m_BuildableWithoutAssemblylistCheck.GetSize(); j++)
		{
			// schauen ob es in der Liste der baubaren Gebäude ist
			if (m_BuildableWithoutAssemblylistCheck.GetAt(j) == buildingInfo->GetAt(i).GetRunningNumber())
			{
				found = TRUE;	// ist schon in der Liste der baubaren Gebäude
				break;
			}
		}

		// Gebäude ist ein Never-Ready-Gebäude und produziert Moral, dann kann es erst gebaut werden, wenn
		// die Moral 2.5 x unter der Produktion des Gebäudes liegt
		if (buildingInfo->GetAt(i).GetNeverReady() && buildingInfo->GetAt(i).GetMoralProd() > 0)
			if (m_iMoral > 100 - buildingInfo->GetAt(i).GetMoralProd() * 2.5)
				continue;
		
		// Haben wir noch nicht dieses Gebäude in der Liste gefunden, so könnten wir es vielleicht bauen, deswegen
		// jetzt die ganzen Voraussetzungen prüfen
		if (found == FALSE && CheckTech(&buildingInfo->GetAt(i), empire->GetResearch()) == TRUE)
			// Checken ob wir es durch die vorhanden Planeten bauen können
			if (CheckPlanet(&buildingInfo->GetAt(i),sector))
				// allg. Voraussetzungen des Gebäudes checken
				if (CheckGeneralConditions(&buildingInfo->GetAt(i),sector,globals,pMajor))
				{
					// Wenn wir hier sind sind wir mittlerweile schon bei Punkt (2.1) angekommen
					// Überprüfen ob das Gebäude einen Vorgänger besitzt
					if (buildingInfo->GetAt(i).GetPredecessorID() != 0)	// Punkt 2.1.1
					{
						BOOLEAN found_predecessor = FALSE;
						// Schauen ob ein Gebäude mit dieser ID im System schon steht
						for (int k = 0; k < m_Buildings.GetSize(); k++)
							if (m_Buildings.GetAt(k).GetRunningNumber() == buildingInfo->GetAt(i).GetPredecessorID())
							{
								found_predecessor = TRUE;
								break;
							}						
						// Haben wir einen Vorgänger des Gebäudes im System stehen
						if (found_predecessor == TRUE)	// Punkt 2.1.1.1
						{
							// Gebäudeupgrade in die Liste nehmen
							m_BuildableWithoutAssemblylistCheck.Add(buildingInfo->GetAt(i).GetRunningNumber()*(-1));
						}
						// ansonsten haben wir keinen Vorgänger des Gebäudes im System stehen // Punkt 2.1.1.2						
					}
					// Gebäude hat keinen Vorgänger
					// es gehört zu einer Rasse oder kann von jeder Rasse gebaut werden -> Sprich 0
					else if (buildingInfo->GetAt(i).GetOwnerOfBuilding() == 0 || buildingInfo->GetAt(i).GetOwnerOfBuilding() == pMajor->GetRaceBuildingNumber())	// Punkt 2.1.2
					{
						// schauen das sich nicht schon ein Äquivalent in der Liste befindet, aber nur wenn das Gebäude eine andere Rasse bauen
						// kann oder wenn es jeder bauen kann
						if (equivalents == TRUE)
						{
							for (int j = 0; j < m_BuildableWithoutAssemblylistCheck.GetSize(); j++)
							{
								short tid = m_BuildableWithoutAssemblylistCheck.GetAt(j);
								// Nur wenn es sich hierbei nicht um ein Upgrade handelt!
								if (tid > 0)
									// Nur die Äquivalenz abfragen, zu der das Gebäude "tid" gehört
									if (buildingInfo->GetAt(i).GetEquivalent(buildingInfo->GetAt(tid-1).GetOwnerOfBuilding()) == tid)
									{
										found = TRUE;
										break;
									}
							}							
							// Schauen, das sich nicht schon ein Äquivalent bei den stehenden Gebäuden befindet
							if (found == FALSE)
							{
								minID = 0;
								for (int j = 0; j < m_Buildings.GetSize(); j++)
								{
									USHORT tid = m_Buildings.GetAt(j).GetRunningNumber();
									if (tid > minID)	// Weil geordnetes Feld
										if (buildingInfo->GetAt(i).GetEquivalent(buildingInfo->GetAt(tid-1).GetOwnerOfBuilding()) == tid)
										{
											found = TRUE;
											break;
										}
								}								
							}
						}
						// Gibt es schon ein Äquivalenzgebäude in dem System, so können wir die nicht bauen
						if (found == TRUE)
							continue;
						
						// Nachfolgerchecks brauchen wir nur machen, wenn das Gebäude auch updatebar ist!
						if (buildingInfo->GetAt(i).GetUpgradeable())
						{							
							// Überprüfen ob ein Nachfolger des Gebäudes in der Liste der baubaren Gebäude steht
							if (CheckFollower(buildingInfo,buildingInfo->GetAt(i).GetRunningNumber(),0,equivalents) == FALSE)
								// Überprüfen ob ein Nachfolger des Gebäudes schon im System steht
								if (CheckFollower(buildingInfo,buildingInfo->GetAt(i).GetRunningNumber(),1,equivalents) == FALSE)
									m_BuildableWithoutAssemblylistCheck.Add(buildingInfo->GetAt(i).GetRunningNumber());							
						}
						// Ansonsten können wir es bauen.
						else
							m_BuildableWithoutAssemblylistCheck.Add(buildingInfo->GetAt(i).GetRunningNumber());
					}
				}
	}
	
	// Baulistencheck noch durchführen und fertig sind wir
	AssemblyListCheck(buildingInfo,globals);
	// Sortierung in der Reihenfolge nach ihrer ID
	c_arraysort<CArray<short,short>,short>(m_BuildableBuildings,sort_asc);
}

BOOLEAN CSystem::AssemblyListCheck(BuildingInfoArray* buildingInfo, CGlobalBuildings* globals, int ID)
{
	// zu allererst die Liste der baubaren Gebäude und Updates mit der Liste der in dieser Runde baubaren Gebäude
	// vor dem AssemblyListCheck füllen
	m_BuildableBuildings.RemoveAll();
	m_BuildableUpdates.RemoveAll();
	m_BuildableBuildings.FreeExtra();
	m_BuildableUpdates.FreeExtra();
	for (int i = 0; i < m_BuildableWithoutAssemblylistCheck.GetSize(); i++)
	{
		// Unterscheidung zwischen Upgrades und normalen Gebäuden. Auch bei den Upgrades muss geschaut werden, dass dieses Gebäude
		// noch nicht zu oft vorkommt
		int entry = m_BuildableWithoutAssemblylistCheck.GetAt(i);
		if (entry < 0)
			entry *= (-1);

		// wenn dieses Gebäude nur X mal pro Imperium vorkommen darf, dann darf dieses Gebäude nicht mehr als X mal
		// als globales Gebäude vorkommen
		bool bFound = false;
		if (buildingInfo->GetAt(entry - 1).GetMaxInEmpire() > 0)
		{
			short ID = buildingInfo->GetAt(entry - 1).GetRunningNumber();
			short nCount = globals->GetCountGlobalBuilding(m_sOwnerOfSystem, ID);
			if (nCount >= buildingInfo->GetAt(entry - 1).GetMaxInEmpire())
				bFound = true;
		}
		// wir haben das Gebäude nicht in der Liste der globalen Gebäude gefunden
		if (!bFound)
		{
			if (m_BuildableWithoutAssemblylistCheck.GetAt(i) > 0)
				m_BuildableBuildings.Add(entry);
			else
				m_BuildableUpdates.Add(entry);
		}
	}
	USHORT inSystem = 0;
	// nach Updates in der Bauliste suchen. Wenn wir ein Update in der Bauliste gefunden haben, so
	// müssen wir dieses und all deren Vorgänger aus der Liste der baubaren Gebäude entfernen
	for (int i = 0; i < ALE; i++)
	{
		if (m_AssemblyList.GetAssemblyListEntry(i) < 0)
		{
			// Vorgänger des Updates aus der Liste entfernen
			for (int t = 0; t < m_BuildableBuildings.GetSize(); t++)
			{
				USHORT pre = buildingInfo->GetAt(m_AssemblyList.GetAssemblyListEntry(i)*(-1)-1).GetPredecessorID();
				if (pre == m_BuildableBuildings.GetAt(t))
				{
					m_BuildableBuildings.RemoveAt(t--);
					// Haben wir einen Vorgänger entfernt, dann auch alle Updates des Vorgängers, die in der Liste 
					// der Updates sind entfernen

					// HIER FIXEN: Auch wenn ein Vorgänger nicht mehr baubar war müssen wir das Update aus der Bauliste
					// entfernen!!!
					/*	
					for (int s = 0; s < m_BuildableUpdates.GetSize(); s++)
					{
						USHORT pre2 = buildingInfo->GetAt(m_BuildableUpdates.GetAt(s)-1).GetPredecessorID();
						if (pre2 == pre)
							m_BuildableUpdates.RemoveAt(s--);
					}*/
				}
				for (int s = 0; s < m_BuildableUpdates.GetSize(); s++)
				{
					USHORT pre2 = buildingInfo->GetAt(m_BuildableUpdates.GetAt(s)-1).GetPredecessorID();
					if (pre2 == pre)
						m_BuildableUpdates.RemoveAt(s--);
				}
			}
		}
		else if (m_AssemblyList.GetAssemblyListEntry(i) < 10000 && m_AssemblyList.GetAssemblyListEntry(i) != 0)
		{
			// hier die maximal baubaren Gebäude pro Imperium checken
			// Die Gebäude befinden sich auch schon in der GlobalBuildings Variable, wenn sie schon in der
			// Bauliste sind
			USHORT nID = m_AssemblyList.GetAssemblyListEntry(i);
			if (buildingInfo->GetAt(nID - 1).GetMaxInEmpire() > 0)
			{	
				int nCount = globals->GetCountGlobalBuilding(m_sOwnerOfSystem, nID);
				// Haben wir dieses Gebäude schon "oft genug" stehen und in der Bauliste der Systeme, dann aus der
				// Bauliste nehmen
				if (nCount >= buildingInfo->GetAt(nID - 1).GetMaxInEmpire())
					for (int t = 0; t < m_BuildableBuildings.GetSize(); t++)
						if (nID == m_BuildableBuildings.GetAt(t))
						{
							m_BuildableBuildings.RemoveAt(t--);
							break;
						}
			}
			// hier die maximal baubaren Gebäude pro System checken
			if (buildingInfo->GetAt(m_AssemblyList.GetAssemblyListEntry(i)-1).GetMaxInSystem().Number > 0)
			{
				USHORT n = 0;
				for (int j = 0; j < m_Buildings.GetSize(); j++)
				{
					if (m_Buildings.GetAt(j).GetRunningNumber() == buildingInfo->GetAt(m_AssemblyList.GetAssemblyListEntry(i)-1).GetMaxInSystem().RunningNumber)
						n++;
					if (n >= buildingInfo->GetAt(m_AssemblyList.GetAssemblyListEntry(i)-1).GetMaxInSystem().Number)
						break;
				}
				// Bauliste durchgehen und wenn dieses Gebeäude in dieser vorkommt, dann auch mitzählen
				for (int j = 0; j < ALE; j++)
					if (m_AssemblyList.GetAssemblyListEntry(i) == m_AssemblyList.GetAssemblyListEntry(j))
						if (m_AssemblyList.GetAssemblyListEntry(j) > 0 && m_AssemblyList.GetAssemblyListEntry(j) < 10000)
							if (m_AssemblyList.GetAssemblyListEntry(j) == buildingInfo->GetAt(m_AssemblyList.GetAssemblyListEntry(j)-1).GetMaxInSystem().RunningNumber)
								n++;
				// Haben wir dieses Gebäude schon "oft genug" stehen und in der Bauliste des Systems, dann aus der
				// Bauliste nehmen
				if (n >= buildingInfo->GetAt(m_AssemblyList.GetAssemblyListEntry(i)-1).GetMaxInSystem().Number)
					for (int t = 0; t < m_BuildableBuildings.GetSize(); t++)
						if (m_AssemblyList.GetAssemblyListEntry(i) == m_BuildableBuildings.GetAt(t))
						{
							m_BuildableBuildings.RemoveAt(t--);
							break;
						}
			}
		}
	}
	return FALSE;
}

// Funktion berechnet die baubaren Schiffe in dem System.
void CSystem::CalculateBuildableShips(CBotf2Doc* pDoc, const CPoint& p)
{
	ASSERT(pDoc);	
	m_BuildableShips.RemoveAll();
	m_BuildableShips.FreeExtra();
	// Hier jetzt schauen ob wir eine Werft haben und anhand der größe der Werft können wir bestimmte
	// Schiffstypen bauen
	CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sOwnerOfSystem));
	if (!pMajor)
		return;
	
	// exisitiert eine Werft?
	if (m_Production.GetShipYard())
	{
		// Array mit baubaren Minorraceschiffen füllen
		int nMinorShipNumber = -1;
		if (pDoc->GetSector(p).GetMinorRace())
		{
			CMinor* pMinor = pDoc->GetRaceCtrl()->GetMinorRace(pDoc->GetSector(p).GetName());
			if (pMinor)
				nMinorShipNumber = pMinor->GetRaceShipNumber();
		}

		CResearch* pResearch = pMajor->GetEmpire()->GetResearch();
		CArray<USHORT> obsoleteClasses;

		for (int i = 0; i < pDoc->GetShipInfos()->GetSize(); i++)
		{
			CShipInfo* pShipInfo = &(pDoc->GetShipInfos()->GetAt(i));
			// Außenposten und Sternenbasen können wir natürlich nicht hier bauen
			if ((pShipInfo->GetRace() == pMajor->GetRaceShipNumber() || pShipInfo->GetRace() == nMinorShipNumber)
				&& pShipInfo->GetShipType() != SHIP_TYPE::OUTPOST && pShipInfo->GetShipType() != SHIP_TYPE::STARBASE)
			{
				// Forschungsstufen checken
				if (pResearch->GetBioTech() <  pShipInfo->GetBioTech())
					continue;
				if (pResearch->GetEnergyTech() < pShipInfo->GetEnergyTech())
					continue;
				if (pResearch->GetCompTech() < pShipInfo->GetComputerTech())
					continue;
				if (pResearch->GetPropulsionTech() < pShipInfo->GetPropulsionTech())
					continue;
				if (pResearch->GetConstructionTech() < pShipInfo->GetConstructionTech())
					continue;
				if (pResearch->GetWeaponTech() < pShipInfo->GetWeaponTech())
					continue;

				// Wenn durch dieses Schiff ein anderes Schiff veraltet ist (nur wenn es technologisch baubar ist)
				if (pShipInfo->GetObsoleteShipClass() != "")
				{
					for (int j = 0; j < pDoc->GetShipInfos()->GetSize(); j++)
						if (pDoc->GetShipInfos()->GetAt(j).GetShipClass() == pShipInfo->GetObsoleteShipClass())
						{
							obsoleteClasses.Add(pDoc->GetShipInfos()->GetAt(j).GetID());
							break;
						}
				}

				// Wenn das Schiff nur in einem bestimmten System gebaut werden kann, dann hier checken
				if (!pShipInfo->GetOnlyInSystem().IsEmpty())
				{
					if (pShipInfo->GetOnlyInSystem() != pDoc->GetSector(p).GetName())
						continue;
					// der Besitzer der Schiffsklasse wird auf den Besitzer des Schiffes gesetzt. Somit kann
					// eine Majorrace dann auch die Schiffe der Minorrace bauen
					else
						pShipInfo->SetRace(pMajor->GetRaceShipNumber());
				}
				else if (pShipInfo->GetRace() == nMinorShipNumber)
					continue;

				// gibt es eine Werft welche diese Schiffsgröße bauen kann?
				if (pShipInfo->GetShipSize() > m_Production.GetMaxBuildableShipSize())
					continue;

				// wenn die Credits unter 0 gefallen sind können keine Kampfschiffe mehr gebaut werden
				if (pShipInfo->IsNonCombat() == false && pMajor->GetEmpire()->GetCredits() < 0)
					continue;

				// Schiff in die Liste der baubaren Schiffe aufnehmen
				m_BuildableShips.Add(pShipInfo->GetID());
			}
		}
		// Nochmal die jetzt baubare Schiffsliste durchgehen und schauen, ob manche Schiffe veraltet sind und somit
		// nicht mehr baubar sind
		for (int i = 0; i < obsoleteClasses.GetSize(); i++)
		{
			for (int j = 0; j < m_BuildableShips.GetSize(); j++)
				if (m_BuildableShips.GetAt(j) == obsoleteClasses.GetAt(i))
				{
					m_BuildableShips.RemoveAt(j);
					break;
				}
		}
	}
}

// Diese Funktion berechnet die baubaren Truppen in diesem System
void CSystem::CalculateBuildableTroops(const CArray<CTroopInfo>* troopInfos, const CResearch *research)
{
	m_BuildableTroops.RemoveAll();
	m_BuildableTroops.FreeExtra();
	// Hier jetzt schauen ob wir eine Kaserne haben
	if (m_Production.GetBarrack())
		for (int i = 0; i < troopInfos->GetSize(); i++)
			if (troopInfos->GetAt(i).GetOwner() == m_sOwnerOfSystem)
			{
				BOOLEAN buildable = TRUE;
				// zuerstmal die Forschungsstufen checken
				if (research->GetBioTech() < troopInfos->GetAt(i).GetNeededTechlevel(0))
					buildable = FALSE;
				else if (research->GetEnergyTech() < troopInfos->GetAt(i).GetNeededTechlevel(1))
					buildable = FALSE;
				else if (research->GetCompTech() < troopInfos->GetAt(i).GetNeededTechlevel(2))
					buildable = FALSE;
				else if (research->GetPropulsionTech() < troopInfos->GetAt(i).GetNeededTechlevel(3))
					buildable = FALSE;
				else if (research->GetConstructionTech() < troopInfos->GetAt(i).GetNeededTechlevel(4))
					buildable = FALSE;
				else if (research->GetWeaponTech() < troopInfos->GetAt(i).GetNeededTechlevel(5))
					buildable = FALSE;
				if (buildable == TRUE)
					m_BuildableTroops.Add(troopInfos->GetAt(i).GetID());
			}
}

// Funktion berechnet die Anzahl aller Farmen, Bauhöfe usw., also alle Gebäude die Arbeiter benötigen.
// Sie muß am Rundenanfang vor CalculateVariables() aufgerufen werden und sortiert gleichzeitig das
// CArray m_Buildings nach der RunningNumber.
// In der Doc-Klasse nach der Funktion DestroyBuildings() und zu Beginn aufrufen!
void CSystem::CalculateNumberOfWorkbuildings(BuildingInfoArray *buildingInfos)
{
	// Sortierung
	c_arraysort<CArray<CBuilding>,CBuilding>(m_Buildings,sort_asc);

	// Berechnung der Gebäude
	m_iFoodBuildings = 0;
	m_iIndustryBuildings = 0;
	m_iEnergyBuildings = 0;
	m_iSecurityBuildings = 0;
	m_iResearchBuildings = 0;
	m_iTitanMines = 0;
	m_iDeuteriumMines = 0;
	m_iDuraniumMines = 0;
	m_iIridiumMines = 0;
	m_iCrystalMines = 0;
	for (int i = 0; i < m_Buildings.GetSize(); i++)
	{
		const CBuildingInfo* buildingInfo = &buildingInfos->GetAt(m_Buildings.GetAt(i).GetRunningNumber() - 1);
		
		if (buildingInfo->GetWorker() == TRUE)
		{
			if (buildingInfo->GetFoodProd() > 0)
				m_iFoodBuildings++;
			else if (buildingInfo->GetIPProd() > 0)
				m_iIndustryBuildings++;
			else if (buildingInfo->GetEnergyProd() > 0)
				m_iEnergyBuildings++;
			else if (buildingInfo->GetSPProd() > 0)
				m_iSecurityBuildings++;
			else if (buildingInfo->GetFPProd() > 0)
				m_iResearchBuildings++;
			else if (buildingInfo->GetTitanProd() > 0)
				m_iTitanMines++;
			else if (buildingInfo->GetDeuteriumProd() > 0)
				m_iDeuteriumMines++;
			else if (buildingInfo->GetDuraniumProd() > 0)
				m_iDuraniumMines++;
			else if (buildingInfo->GetCrystalProd() > 0)
				m_iCrystalMines++;
			else if (buildingInfo->GetIridiumProd() > 0)
				m_iIridiumMines++;
		}
	}
}

// Funktion berechnet die imperiumweite Moralproduktion, welche aus diesem System generiert wird.
void CSystem::CalculateEmpireWideMoralProd(BuildingInfoArray *buildingInfos)
{
	for (int i = 0; i < m_Buildings.GetSize(); i++)
		if (m_Buildings.GetAt(i).GetIsBuildingOnline())
		{
			const CBuildingInfo *buildingInfo = &buildingInfos->GetAt(m_Buildings.GetAt(i).GetRunningNumber() - 1);
			m_Production.m_iMoralProdEmpireWide[m_sOwnerOfSystem] += buildingInfo->GetMoralProdEmpire();
		}
}

// Funktion setzt das letzte Gebäude, welches gebaut wurde online, sofern dies möglich ist.
int CSystem::SetNewBuildingOnline(BuildingInfoArray *buildingInfos)
{
	unsigned short CheckValue = 0;
	unsigned short lastBuilding = m_Buildings.GetUpperBound();

	const CBuildingInfo *buildingInfo = &buildingInfos->GetAt(m_Buildings.GetAt(lastBuilding).GetRunningNumber() - 1);

	if (m_Workers.GetWorker(WORKER::FREE_WORKER) > 0 && buildingInfo->GetWorker() == TRUE)
	{
		if (buildingInfo->GetFoodProd() > 0)
			m_Workers.InkrementWorker(WORKER::FOOD_WORKER);
		if (buildingInfo->GetIPProd() > 0)
			m_Workers.InkrementWorker(WORKER::INDUSTRY_WORKER);
		if (buildingInfo->GetEnergyProd() > 0)
			m_Workers.InkrementWorker(WORKER::ENERGY_WORKER);
		if (buildingInfo->GetSPProd() > 0)
			m_Workers.InkrementWorker(WORKER::SECURITY_WORKER);
		if (buildingInfo->GetFPProd() > 0)
			m_Workers.InkrementWorker(WORKER::RESEARCH_WORKER);
		if (buildingInfo->GetTitanProd() > 0)
			m_Workers.InkrementWorker(WORKER::TITAN_WORKER);
		if (buildingInfo->GetDeuteriumProd() > 0)
			m_Workers.InkrementWorker(WORKER::DEUTERIUM_WORKER);
		if (buildingInfo->GetDuraniumProd() > 0)
			m_Workers.InkrementWorker(WORKER::DURANIUM_WORKER);
		if (buildingInfo->GetCrystalProd() > 0)
			m_Workers.InkrementWorker(WORKER::CRYSTAL_WORKER);
		if (buildingInfo->GetIridiumProd() > 0)
			m_Workers.InkrementWorker(WORKER::IRIDIUM_WORKER);
	}
	else if (m_Buildings.GetAt(lastBuilding).GetIsBuildingOnline())
		CheckValue = 0;
	else
		CheckValue = 1;
	if (buildingInfo->GetNeededEnergy() > 0)
	{
		if (buildingInfo->GetNeededEnergy() <= m_Production.m_iEnergyProd)
		{
			m_Buildings.ElementAt(lastBuilding).SetIsBuildingOnline(TRUE);
			CheckValue = 0;
		}
		else
			CheckValue = 2;
	}
	return CheckValue;	// Wenn CheckValue == 0, dann konnte das Gebäude in Betrieb genommen werden
						// Wenn CheckValue == 1, dann fehlen Arbeiter
						// Wenn CheckValue == 2, dann fehlt Energie im System
}

// Funktion überprüft Gebäude die Energie benötigen und schaltet diese gegebenfalls ab,
// falls zuwenig Energie im System vorhanden ist.
// Diese Funktion aufrufen, bevor wir CalculateVariables() usw. aufrufen, weil wir ja die bösen Onlinegebäude vorher
// ausschalten wollen.
bool CSystem::CheckEnergyBuildings(const BuildingInfoArray* pvBuildingInfos)
{	
	bool bRet = false;	
	for (int i = 0; i < m_Buildings.GetSize(); i++)
	{
		// ist genügend Energie vorhanden, so muss nichts überprüft werden
		if (m_Production.m_iEnergyProd >= 0)
			return bRet;

		// anderenfalls müssen die Gebäude nacheinander solange abgeschaltet werden, bis wieder genügend Energie vorhanden ist
		const CBuildingInfo* pBuildingInfo = &pvBuildingInfos->GetAt(m_Buildings.GetAt(i).GetRunningNumber() - 1);
		if (m_Production.m_iEnergyProd < 0 && pBuildingInfo->GetNeededEnergy() > 0 && m_Buildings[i].GetIsBuildingOnline())
		{
			// Gebäude abschalten
			m_Buildings[i].SetIsBuildingOnline(FALSE);
			// vorhande Energie um den Differenzbetrag erhöhen
			m_Production.m_iEnergyProd += pBuildingInfo->GetNeededEnergy();
			bRet = true;			
		}
	}
	// Wenn bRet == false, dann ist genug Energie vorhanden, ansonsten wurde mindst. ein Gebäude abgeschaltet
	return bRet;
}

// Funktion baut die Gebäude der Minorrace, wenn wir eine Mitgliedschaft mit dieser erreicht haben.
void CSystem::BuildBuildingsForMinorRace(CSector* sector, BuildingInfoArray* buildingInfo, USHORT averageTechlevel, const CMinor* pMinor)
{
	// alle Gebäude die wir nach Systemeroberung nicht haben dürfen werden aus der Liste der aktuellen Gebäude entfernt
	RemoveSpecialRaceBuildings(buildingInfo);

	if (m_Buildings.GetSize() < 5)
	{
		// in exist[.] steht dann, ob wir einen Rohstoff abbauen können, wenn ja, dann können wir auch das Gebäude bauen
		BOOLEAN exist[DERITIUM + 1] = {0};
		sector->GetAvailableResources(exist, true);

		// Schauen, welche Gebäudestufe ungefähr in dem System steht
		// Jetzt nach der Fortschrittlichkeit der kleinen Rasse gehen
		short level = 0;
		switch (pMinor->GetTechnologicalProgress())
		{
		case 0:	// sehr rückständig
			{
				USHORT temp = rand()%3+1;
				level = averageTechlevel-temp;				
				break;
			}
		case 1:	// rückständig
			{
				USHORT temp = rand()%2+1;
				level = averageTechlevel-temp;				
				break;
			}
		case 2:	// normal fortschrittlich
			{
				USHORT temp = rand()%3;
				level = averageTechlevel+1-temp;
				break;
			}
		case 3:	// fortschrittlich
			{
				USHORT temp = rand()%2+1;
				level = averageTechlevel+temp;
				break;
			}
		case 4:	// sehr fortschrittlich
			{
				USHORT temp = rand()%3+1;
				level = averageTechlevel+temp;
				break;
			}
		}

		level = max(0, level);		
		// Researchlevels für Minor zusammenbauen
		BYTE researchLevels[6] = {level, level, level, level, level, level};

		// die gültige Eigenschaft der Minor festlegen
		vector<RACE_PROPERTY::Typ> vProperty;
		if (pMinor->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
			vProperty.push_back(RACE_PROPERTY::FINANCIAL);
		if (pMinor->IsRaceProperty(RACE_PROPERTY::WARLIKE))
			vProperty.push_back(RACE_PROPERTY::WARLIKE);
		if (pMinor->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
			vProperty.push_back(RACE_PROPERTY::AGRARIAN);
		if (pMinor->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
			vProperty.push_back(RACE_PROPERTY::INDUSTRIAL);
		if (pMinor->IsRaceProperty(RACE_PROPERTY::SECRET))
			vProperty.push_back(RACE_PROPERTY::SECRET);
		if (pMinor->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
			vProperty.push_back(RACE_PROPERTY::SCIENTIFIC);
		if (pMinor->IsRaceProperty(RACE_PROPERTY::PRODUCER))
			vProperty.push_back(RACE_PROPERTY::PRODUCER);
		if (pMinor->IsRaceProperty(RACE_PROPERTY::PACIFIST))
			vProperty.push_back(RACE_PROPERTY::PACIFIST);
		if (pMinor->IsRaceProperty(RACE_PROPERTY::SNEAKY))
			vProperty.push_back(RACE_PROPERTY::SNEAKY);
		if (pMinor->IsRaceProperty(RACE_PROPERTY::SOLOING))
			vProperty.push_back(RACE_PROPERTY::SOLOING);
		if (pMinor->IsRaceProperty(RACE_PROPERTY::HOSTILE))
			vProperty.push_back(RACE_PROPERTY::HOSTILE);

		if (vProperty.size() == 0)
			vProperty.push_back(RACE_PROPERTY::NOTHING_SPECIAL);
		
		RACE_PROPERTY::Typ nProperty = vProperty[rand()%vProperty.size()];
		// Jetzt die Gebäude bauen und auch das System nach der Art der MinorRace angleichen
		// z.B. bei einer landwirtschaftlichen Rasse viele Farmen
		// Die Rassenartzuweisung müsste man vielleicht noch dynamisch machen. Bis jetzt wird immer
		// davon ausgegangen, dass z.B. die Major auf dem Cardassianerplatz bestimmte Eigenschaften vertritt.
		// Dies kann sich durch Modifikationen aber verändern.
		// ALPHA5 -> fromRace müsste noch fest in der Minorrace stehen, daher ist folgender Block vorerst entfernt
		BYTE fromRace = rand()%6 + 1; // liegt erstmal zwischen human und dominion
				
		USHORT runningNumber[10] = {0,0,0,0,0,0,0,0,0,0};
		USHORT nShipYard = 0;
		for (int i = 0; i < buildingInfo->GetSize(); i++)
		{
			if (fromRace == buildingInfo->GetAt(i).GetOwnerOfBuilding())
			{
				if (!buildingInfo->GetAt(i).IsBuildingBuildableNow(researchLevels))
					continue;

				// wenn noch gar keine Gebäude standen, also die Rasse frisch vermitgliedert wurde und Raumschiffe bauen
				// kann, so bekommt sie die erste baubare Werft
				if (nShipYard == 0 && m_Buildings.GetSize() == 0 && pMinor->GetSpaceflightNation())
				{
					if (buildingInfo->GetAt(i).GetShipYard() && buildingInfo->GetAt(i).GetPredecessorID() == 0 && buildingInfo->GetAt(i).GetMaxInEmpire() == 0)
						nShipYard = buildingInfo->GetAt(i).GetRunningNumber();		
				}
				
				if (buildingInfo->GetAt(i).GetFoodProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE)
				{				
					runningNumber[0] = buildingInfo->GetAt(i).GetRunningNumber();
				}
				else if (buildingInfo->GetAt(i).GetIPProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE)
				{
					runningNumber[1] = buildingInfo->GetAt(i).GetRunningNumber();
				}
				else if (buildingInfo->GetAt(i).GetEnergyProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE)
				{
					runningNumber[2] = buildingInfo->GetAt(i).GetRunningNumber();
				}
				else if (buildingInfo->GetAt(i).GetSPProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE)
				{
					runningNumber[3] = buildingInfo->GetAt(i).GetRunningNumber();
				}
				else if (buildingInfo->GetAt(i).GetFPProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE)
				{
					runningNumber[4] = buildingInfo->GetAt(i).GetRunningNumber();
				}
				else if (buildingInfo->GetAt(i).GetTitanProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE && exist[TITAN] == TRUE)
				{
					runningNumber[5] = buildingInfo->GetAt(i).GetRunningNumber();
				}
				else if (buildingInfo->GetAt(i).GetDeuteriumProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE && exist[DEUTERIUM] == TRUE)
				{
					runningNumber[6] = buildingInfo->GetAt(i).GetRunningNumber();
				}
				else if (buildingInfo->GetAt(i).GetDuraniumProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE && exist[DURANIUM] == TRUE)
				{
					runningNumber[7] = buildingInfo->GetAt(i).GetRunningNumber();
				}
				else if (buildingInfo->GetAt(i).GetCrystalProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE && exist[CRYSTAL] == TRUE)
				{
					runningNumber[8] = buildingInfo->GetAt(i).GetRunningNumber();
				}
				else if (buildingInfo->GetAt(i).GetIridiumProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE && exist[IRIDIUM] == TRUE)
				{
					runningNumber[9] = buildingInfo->GetAt(i).GetRunningNumber();
				}
			}
		}
		
		// Jetzt haben wir die ganzen RunningNumbers der baubaren Gebäude, nun gucken welche Art von Rasse wir haben
		// und danach zufällig die Gebäude bauen
		USHORT numberOfBuildings = 0;
		numberOfBuildings += (int)m_dHabitants+1 + rand()%5;
		int foodBuildings = 0;
		while (numberOfBuildings > 0)
		{
			USHORT build = rand()%10;
			
			USHORT temp;
			switch (nProperty)
			{
			// Wenn die Rasse eine landwirtschaftliche Rasse ist
			case RACE_PROPERTY::AGRARIAN:
				{
					if (build >= 5)
					{
						temp = rand()%3;
						if (temp == 0)
							build = 0;
					}
					else if (build == 4 || build == 2)
					{
						temp = rand()%2;
						if (temp == 0)
							build = 0;
					}
					else if (build == 3)
						build = 0;
					break;
				}
			case RACE_PROPERTY::INDUSTRIAL:
				{
					if (build >= 5)
					{
						temp = rand()%3;
						if (temp == 0)
							build = 1;
					}
					else if (build == 4 || build == 2)
					{
						temp = rand()%2;
						if (temp == 0)
							build = 1;
					}
					else if (build == 3)
					{
						temp = rand()%2;
						if (temp == 0)
							build = 0;
						else
							build = 1;
					}
					break;
				}
			case RACE_PROPERTY::SECRET:
				{
					if (build >= 5)
					{
						temp = rand()%3;
						if (temp == 0)
							build = 3;
					}
					else if (build == 4)
					{
						temp = rand()%4;
						if (temp == 0)
							build = 3;
					}
					else if (build == 2)
					{
						temp = rand()%4;
						if (temp == 0)
							build = 0;
						else if (temp == 1)
							build = 3;
					}
					break;
				}
			case RACE_PROPERTY::SCIENTIFIC:
				{
					if (build >= 5)
					{
						temp = rand()%3;
						if (temp == 0)
							build = 4;
					}
					else if (build == 1)
					{
						temp = rand()%6;
						if (temp == 0)
							build = 4;
					}
					else if (build == 2)
					{
						temp = rand()%4;
						if (temp == 0)
							build = 4;
					}
					else if (build == 3)
					{
						temp = rand()%2;
						if (temp == 0)
							build = 0;
						else
							build = 4;
					}
					break;
				}
			case RACE_PROPERTY::PRODUCER:
				{
					if (build < 5 && build > 1)
					{
						temp = rand()%5;
						if (temp == 0)
							build = 5;
						else if (temp == 1)
							build = 6;
						else if (temp == 2)
							build = 7;
						else if (temp == 3)
							build = 8;
						else if (temp == 4)
							build = 9;
					}
					break;
				}
			case RACE_PROPERTY::SNEAKY:
				break;
			case RACE_PROPERTY::SOLOING:
				break;
			case RACE_PROPERTY::NOTHING_SPECIAL:
				break;
				// Pazifisten, kriegerische Rassen sowie Finanzrassen bekommen nur weniger Geheimdienst drauf
			default:
				{
					if (build == 2 || build == 3)
					{
						if (rand()%2 == NULL)
							build = 1;
					}					
				}
			}
			
			if (runningNumber[build] != 0)
			{
				if (build == 0)
					foodBuildings++;
				CBuilding building(runningNumber[build]);
				building.SetIsBuildingOnline(buildingInfo->GetAt(runningNumber[build] - 1).GetAllwaysOnline());
				m_Buildings.Add(building);
				numberOfBuildings--;
			}
		}
		// wenn zuwenig Nahrungsgebäude vorhanden sind, so werden hier noch ein paar mehr hinzugefügt
		while (runningNumber[0] != 0 && foodBuildings < m_dHabitants/4)
		{
			CBuilding building(runningNumber[0]);
			building.SetIsBuildingOnline(buildingInfo->GetAt(runningNumber[0] - 1).GetAllwaysOnline());
			m_Buildings.Add(building);
			foodBuildings++;
		}

		// Nun Lager füllen, später vlt. noch ändern nach der Rassenart (bei landwirtschaftl. mehr Nahrung im Lager)
		// Ressourcen sind abhängig von der Anzahl der jeweiligen Gebäude und dem technologischen Fortschritt der
		// Minorrace
		this->CalculateNumberOfWorkbuildings(buildingInfo);
		this->SetFoodStore(this->GetFoodStore() + rand()%(this->GetNumberOfWorkbuildings(WORKER::FOOD_WORKER,0,NULL) * (pMinor->GetTechnologicalProgress() + 1) * 100 + 1));
		for (int res = TITAN; res <= IRIDIUM; res++)
		{
			WORKER::Typ nWorker = (WORKER::Typ)(res + 5);
			int resAdd = rand()%(this->GetNumberOfWorkbuildings(nWorker, 0, NULL) * (pMinor->GetTechnologicalProgress() + 1) * 100 + 1);
			this->SetResourceStore(res, resAdd);			
		}

		// wenn möglich Werft bauen
		if (nShipYard != 0)
		{
			CBuilding building(nShipYard);
			building.SetIsBuildingOnline(buildingInfo->GetAt(nShipYard - 1).GetAllwaysOnline());
			m_Buildings.Add(building);
			SetNewBuildingOnline(buildingInfo);
		}
	}
}

// Diese Funktion entfernt alle speziellen Gebäude aus der Gebäudeliste. Diese Funktion sollte nach Eroberung des Systems
// aufgerufen werden. Danach sind keine Gebäude mehr vorhanden, die nur x mal pro Imperium baubar gewesen oder die nur die Rasse
// selbst bauen darf.
// @param pvBuildingInfos Zeiger auf den Vektor mit allen Gebäudeinformationen
void CSystem::RemoveSpecialRaceBuildings(const BuildingInfoArray* pvBuildingInfos)
{
	for (int i = 0; i < m_Buildings.GetSize(); i++)
	{
		const CBuildingInfo *pInfo = &pvBuildingInfos->GetAt(m_Buildings[i].GetRunningNumber() - 1);
		if (pInfo->GetMaxInEmpire() > 0 || pInfo->GetOnlyRace() || pInfo->GetShipYard() || pInfo->GetBarrack())
			m_Buildings.RemoveAt(i--);
	}
}

// Funktion berechnet und baut die Startgebäude in einem System, nachdem wir einen Planeten
// in diesem kolonisiert haben.
void CSystem::BuildBuildingsAfterColonization(CSector *sector, BuildingInfoArray *buildingInfo, USHORT colonizationPoints)
{
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(sector->GetOwnerOfSector()));
	ASSERT(pMajor);

	// alle Gebäude die wir nach Systemeroberung nicht haben dürfen werden aus der Liste der aktuellen Gebäude entfernt
	RemoveSpecialRaceBuildings(buildingInfo);

	BYTE byRaceBuildingID = pMajor->GetRaceBuildingNumber();

	BYTE researchLevels[6] =
		{
			pMajor->GetEmpire()->GetResearch()->GetBioTech(),
			pMajor->GetEmpire()->GetResearch()->GetEnergyTech(),
			pMajor->GetEmpire()->GetResearch()->GetCompTech(),
			pMajor->GetEmpire()->GetResearch()->GetPropulsionTech(),
			pMajor->GetEmpire()->GetResearch()->GetConstructionTech(),
			pMajor->GetEmpire()->GetResearch()->GetWeaponTech()
		};

	// in exist[.] steht dann, ob wir einen Rohstoff abbauen können, wenn ja, dann können wir auch das Gebäude bauen
	BOOLEAN exist[DERITIUM + 1] = {0};
	sector->GetAvailableResources(exist, true);

	USHORT start = 0;
	for (int i = 0; i < buildingInfo->GetSize(); i++)
		if (buildingInfo->GetAt(i).GetOwnerOfBuilding() == byRaceBuildingID)
		{
			start = i;
			break;
		}
	
	short counter[10] = {0};
	USHORT runningNumber[10] = {0,0,0,0,0,0,0,0,0,0};
	for (int i = start; i < buildingInfo->GetSize(); i++)
	{
		// Abbruchbedingung, wenn das Gebäude eh nicht mehr zu den Gebäuden des Sektorbesitzers gehören.
		// Dafür muss aber die Gebäudeliste geordnet nach den Besitzern vorliegen (das sie aktuell auch ist)
		if (buildingInfo->GetAt(i).GetOwnerOfBuilding() != byRaceBuildingID)
			break;
		// wenn das Gebäude nicht gebaut werden kann, dann mit dem nächsten weitermachen
		if (!buildingInfo->GetAt(i).IsBuildingBuildableNow(researchLevels))
			continue;

		if (buildingInfo->GetAt(i).GetFoodProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE && counter[0] < colonizationPoints)
		{
			counter[0]++;
			runningNumber[0] = buildingInfo->GetAt(i).GetRunningNumber();

		}
		else if (buildingInfo->GetAt(i).GetIPProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE && counter[1] < colonizationPoints)
		{
			counter[1]++;
			runningNumber[1] = buildingInfo->GetAt(i).GetRunningNumber();
		}
		else if (buildingInfo->GetAt(i).GetEnergyProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE && counter[2] < colonizationPoints)
		{
			counter[2]++;
			runningNumber[2] = buildingInfo->GetAt(i).GetRunningNumber();
		}
		else if (buildingInfo->GetAt(i).GetSPProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE && counter[3] < colonizationPoints)
		{
			counter[3]++;
			runningNumber[3] = buildingInfo->GetAt(i).GetRunningNumber();
		}
		else if (buildingInfo->GetAt(i).GetFPProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE && counter[4] < colonizationPoints)
		{
			counter[4]++;
			runningNumber[4] = buildingInfo->GetAt(i).GetRunningNumber();
		}
		else if (buildingInfo->GetAt(i).GetTitanProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE && counter[5] < colonizationPoints && exist[0] == TRUE)
		{
			counter[5]++;
			runningNumber[5] = buildingInfo->GetAt(i).GetRunningNumber();
		}
		else if (buildingInfo->GetAt(i).GetDeuteriumProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE && counter[6] < colonizationPoints && exist[1] == TRUE)
		{
			counter[6]++;
			runningNumber[6] = buildingInfo->GetAt(i).GetRunningNumber();
		}
		else if (buildingInfo->GetAt(i).GetDuraniumProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE && counter[7] < colonizationPoints && exist[2] == TRUE)
		{
			counter[7]++;
			runningNumber[7] = buildingInfo->GetAt(i).GetRunningNumber();
		}
		else if (buildingInfo->GetAt(i).GetCrystalProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE && counter[8] < colonizationPoints && exist[3] == TRUE)
		{
			counter[8]++;
			runningNumber[8] = buildingInfo->GetAt(i).GetRunningNumber();
		}
		else if (buildingInfo->GetAt(i).GetIridiumProd() > 0 && buildingInfo->GetAt(i).GetWorker() == TRUE && counter[9] < colonizationPoints && exist[4] == TRUE)
		{
			counter[9]++;
			runningNumber[9] = buildingInfo->GetAt(i).GetRunningNumber();
		}		
	}
	// wenn schon Gebäude eines Typs stehen, dann dürfen keine des gleichen Typ zusätzlich gebaut werden. Z.B. wenn
	// schon Typ 4 Automatikfarmen stehen darf das Kolonieschiff nicht auch noch Primitive Farmen bauen.
	this->CalculateNumberOfWorkbuildings(buildingInfo);
	for (int build = WORKER::FOOD_WORKER; build <= WORKER::IRIDIUM_WORKER; build++)
	{
		WORKER::Typ nWorker = (WORKER::Typ)build;
		if (this->GetNumberOfWorkbuildings(nWorker, 0, NULL) > 0)
			runningNumber[build] = 0;
		if (runningNumber[build] != 0)
		{
			// Gebäude anlegen, umso besser das Kolonieschiff ist, desto mehr Gebäude stehen dann auf dem System
			for (int i = 0; i < colonizationPoints*2; i++)
			{
				CBuilding building(runningNumber[build]);
				building.SetIsBuildingOnline(buildingInfo->GetAt(runningNumber[build]-1).GetAllwaysOnline());				
				m_Buildings.Add(building);
			}
			// Gebäude mit Arbeitern besetzen
			this->SetWorker(nWorker,colonizationPoints*2,2);
		}
	}
	this->CalculateNumberOfWorkbuildings(buildingInfo);

	// Wenn das System nach einer Bombardierung komplett ausgelöscht wurde und von uns wieder neu kolonisiert wurde,
	// so müssen die mindst. immer baubaren Gebäude gelöscht werden. Dies wird hier jedesmal gemacht, wenn man
	// eine neue Kolonie gründet
	m_AllwaysBuildableBuildings.RemoveAll();
	
}
// Funktion überprüft, ob wie aufgrund der Bevölkerung hier im System überhaupt (noch) eine Handelsroute
// anlegen können
BOOLEAN CSystem::CanAddTradeRoute(CResearchInfo* researchInfo)
{
	// aller 20Mrd. Bevölkerung bekommt man eine neue Handelsroute
	USHORT currentTradeRoutes = m_TradeRoutes.GetSize();
	short addResRoute = 1;
	USHORT maxTradeRoutes = (USHORT)(m_dHabitants / TRADEROUTEHAB) + m_Production.GetAddedTradeRoutes();
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Hier die Boni durch die Uniqueforschung "Lager und Transport" -> eine Ressourcenroute mehr
	if (researchInfo->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
		addResRoute += researchInfo->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetBonus(3);
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Bei dieser Spezialforschung gibt es womöglich auch eine Ressourcenroute mehr
	// Hier die Boni durch die Uniqueforschung "Handel" -> mindestens eine Handelsroute
	if (researchInfo->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
	{
		if (maxTradeRoutes == NULL)
		{
			addResRoute += researchInfo->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetBonus(3);
			maxTradeRoutes += researchInfo->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetBonus(3);
		}
	}
	if (m_ResourceRoutes.GetSize() > addResRoute)
		currentTradeRoutes += m_ResourceRoutes.GetSize() - addResRoute;
	return (currentTradeRoutes < maxTradeRoutes);
}

// Funktion generiert eine neue Handelsroute. Wenn die Funktion <code>TRUE</code> zurückgibt, dann konnte die
// Handelsroute erfolgreich angelegt werden. Als Parameter wird dabei die Koordinate <code>dest</code>des
// Zielsektors übergeben sowie ein Zeiger auf alle Systeme <code>systems</code> auf der Map..
BOOLEAN CSystem::AddTradeRoute(CPoint dest, std::vector<std::vector<CSystem>>& systems/*[][STARMAP_SECTORS_VCOUNT]*/, CResearchInfo* researchInfo)
{
	BOOLEAN canAddTradeRoute = CanAddTradeRoute(researchInfo);
	// als erstes überprüfen, das noch keine Handelsroute aus einem System, welches den gleichen Besitzer wie
	// dieses System hat, zu diesem Ziel führt. Man kann immer nur eine Handelsroute zu einem bestimmten
	// System haben.
	if (canAddTradeRoute)
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
				if (systems[x][y].GetOwnerOfSystem() == this->GetOwnerOfSystem())
					if (&systems[x][y] != this)
						for (int i = 0; i < systems[x][y].GetTradeRoutes()->GetSize(); i++)
							if (systems[x][y].GetTradeRoutes()->GetAt(i).GetDestKO() == dest)
								return FALSE;

	// nachschauen, das nicht schon eine Handelsroute mit dem Ziel vorhanden ist, wenn dies der Fall ist, dann wird
	// diese Handelsroute gelöscht
	for (int i = 0; i < m_TradeRoutes.GetSize(); i++)
		if (m_TradeRoutes.GetAt(i).GetDestKO() == dest)
		{
			// Wenn wir eine Route aufheben wollen, so die Dauer der Handelsroute negativ setzen. Wenn die Handelsroute
			// eh nur noch weniger als 5 Runden dauert, dann wird die Kündigungsdauer auf diese kürzere Zeit eingestellt.
			if (m_TradeRoutes.GetAt(i).GetDuration() > 0)
			{
				// weniger als 5 Runden
				if (m_TradeRoutes.GetAt(i).GetDuration() < 5)
					m_TradeRoutes.ElementAt(i).SetDuration(m_TradeRoutes.GetAt(i).GetDuration()-6);
				// ansonsten normal einstellen
				else
					m_TradeRoutes.ElementAt(i).SetDuration(-1);
				return TRUE;
			}
			else
				return FALSE;
		}

	if (canAddTradeRoute)
	{
		CTradeRoute route;
		route.GenerateTradeRoute(dest);
		m_TradeRoutes.Add(route);
		return TRUE;
	}
	return FALSE;
}

// Funktion gibt das gesamte Credits zurück, was alle Handelsrouten aus diesem System generiert haben.
USHORT CSystem::CreditsFromTradeRoutes()
{
	USHORT credits = 0;
	for (int i = 0; i < m_TradeRoutes.GetSize(); i++)
		credits += m_TradeRoutes.GetAt(i).GetCredits(this->m_Production.GetIncomeOnTradeRoutes());
	return credits;
}

// Funkt überprüft alle Handelsrouten in dem System, ob sie Aufgrund der Bevölkerung oder der Gebäude noch
// da sein können. Wurden Handelsrouten gelöscht, so gibt die Funktion die Anzahl der Löschungen zurück
BYTE CSystem::CheckTradeRoutes(CResearchInfo* researchInfo)
{
	BYTE number = 0;
	// Dauer der Handelsrouten überprüfen. Wenn eine negative Dauer von -5 erreicht wurde, so wird die Handelsroute
	// gelöscht, wenn eine positive Dauer von 1 erreicht wurde, so verlängert sich die Dauer wieder auf 20 Runden.
	// Bei allen anderen Fällen wird um eins dekrementiert.
	for (int i = 0; i < m_TradeRoutes.GetSize(); i++)
	{
		// Wenn das System blockiert wird, dann produziert die Handelsroute kein Credits
		if (GetBlockade() > NULL)
			m_TradeRoutes.GetAt(i).SetCredits(0);
		if (m_TradeRoutes.GetAt(i).GetDuration() <= -5)
		{
			m_TradeRoutes.RemoveAt(i--);
			number++;
		}
		else if (m_TradeRoutes.GetAt(i).GetDuration() == 1)
			m_TradeRoutes.ElementAt(i).SetDuration(20);
		else
			m_TradeRoutes.ElementAt(i).SetDuration(m_TradeRoutes.GetAt(i).GetDuration()-1);
	}

	// Hier Bevölkerungs und Gebäudecheck durchführen
	USHORT currentTradeRoutes = m_TradeRoutes.GetSize();
	short addResRoute = 1;
	USHORT maxTradeRoutes = (USHORT)(m_dHabitants / TRADEROUTEHAB) + m_Production.GetAddedTradeRoutes();
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Hier die Boni durch die Uniqueforschung "Lager und Transport" -> eine Ressourcenroute mehr
	if (researchInfo->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
		addResRoute += researchInfo->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetBonus(3);
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Bei dieser Spezialforschung gibt es womöglich auch eine Ressourcenroute mehr
	// Hier die Boni durch die Uniqueforschung "Handel" -> mindestens eine Handelsroute
	if (researchInfo->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
	{		
		if (maxTradeRoutes == NULL)
		{
			addResRoute += researchInfo->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetBonus(3);
			maxTradeRoutes += researchInfo->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetBonus(3);
		}
	}
	if (m_ResourceRoutes.GetSize() > addResRoute)
		currentTradeRoutes += m_ResourceRoutes.GetSize() - addResRoute;
	while (currentTradeRoutes > maxTradeRoutes)
	{
		if (m_TradeRoutes.GetSize() > 0)
		{
			m_TradeRoutes.RemoveAt(m_TradeRoutes.GetUpperBound());
			number++;
			currentTradeRoutes--;
		}
		else
			break;
	}
	return number;
}

// Funktion generiert eine neue Ressourcenroute. Wenn die Funktion <code>TRUE</code> zurückgibt, dann konnte die
// Ressourcenroute erfolgreich angelegt werden. Als Parameter wird dabei die Koordinate <code>dest</code> des
// Zielsektors übergeben sowie die Art der betroffenen Ressource <code>res</code> und einen Zeiger auf alle
// Systeme <code>systems</code> auf der Map.
BOOLEAN CSystem::AddResourceRoute(CPoint dest, BYTE res, std::vector<std::vector<CSystem>>& systems/*[][STARMAP_SECTORS_VCOUNT]*/, CResearchInfo* researchInfo)
{
	// Es gibt immer mindst. eine Ressourcenroute pro System. Insgesamt stehen "Anzahl Handelsrouten + 1" Ressourcen-
	// routen zur Vrefügung.
	short addResRoute = 1;
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Hier die Boni durch die Uniqueforschung "Lager und Transport" -> eine Ressourcenroute mehr
	if (researchInfo->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
		addResRoute += researchInfo->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetBonus(3);
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Bei dieser Spezialforschung gibt es womöglich auch eine Ressourcenroute mehr
	// Hier die Boni durch die Uniqueforschung "Handel" -> mindestens eine Handelsroute
	if (researchInfo->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
	{
		USHORT maxTradeRoutes = (USHORT)(m_dHabitants / TRADEROUTEHAB) + m_Production.GetAddedTradeRoutes();
		if (maxTradeRoutes == NULL)
			addResRoute += researchInfo->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetBonus(3);
	}
		
	USHORT maxResourceRoutes = (USHORT)(m_dHabitants / TRADEROUTEHAB) + m_Production.GetAddedTradeRoutes() + addResRoute;
	
	if (systems[dest.x][dest.y].GetOwnerOfSystem() != this->GetOwnerOfSystem())
		return FALSE;
	if (systems[dest.x][dest.y].GetHabitants() == 0.0f || this->GetHabitants() == 0.0f)
		return FALSE;
	if (maxResourceRoutes <= m_ResourceRoutes.GetSize() + m_TradeRoutes.GetSize())
		return FALSE;
	// gibt es schon eine Route mit dem Rohstoff ins Zielsystem?
	for (int i = 0; i < m_ResourceRoutes.GetSize(); i++)
		if (m_ResourceRoutes.GetAt(i).GetResource() == res && m_ResourceRoutes.GetAt(i).GetKO() == dest)
			return FALSE;
	
	CResourceRoute route;
	route.GenerateResourceRoute(dest, res);
	m_ResourceRoutes.Add(route);
	return TRUE;
}

// Funktion überprüft alle Ressourcenrouten in dem System, ob sie Aufgrund der Bevölkerung oder der Gebäude noch
// da sein dürfen. Wurden Ressourcenrouten gelöscht, so gibt die Funktion die Anzahl der Löschungen zurück.
BYTE CSystem::CheckResourceRoutes(CResearchInfo* researchInfo)
{
	// Es gibt immer mindst. eine Ressourcenroute pro System. Insgesamt stehen "Anzahl Handelsrouten + 1" Ressourcen-
	// routen zur Vrefügung.
	short addResRoute = 1;
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Hier die Boni durch die Uniqueforschung "Lager und Transport" -> eine Ressourcenroute mehr
	if (researchInfo->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
		addResRoute += researchInfo->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetBonus(3);
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Bei dieser Spezialforschung gibt es womöglich auch eine Ressourcenroute mehr
	// Hier die Boni durch die Uniqueforschung "Handel" -> mindestens eine Handelsroute
	if (researchInfo->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
	{
		USHORT maxTradeRoutes = (USHORT)(m_dHabitants / TRADEROUTEHAB) + m_Production.GetAddedTradeRoutes();
		if (maxTradeRoutes == NULL)
			addResRoute += researchInfo->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetBonus(3);
	}

	USHORT maxResourceRoutes = (USHORT)(m_dHabitants / TRADEROUTEHAB) + m_Production.GetAddedTradeRoutes() + addResRoute;
	USHORT currentResourceRoutes = m_ResourceRoutes.GetSize() + m_TradeRoutes.GetSize();
	BYTE number = 0;
	while (currentResourceRoutes > maxResourceRoutes)
	{
		if (m_ResourceRoutes.GetSize() > 0)
		{
			m_ResourceRoutes.RemoveAt(m_ResourceRoutes.GetUpperBound());
			number++;
			currentResourceRoutes--;
		}
		else
			break;
	}
	return number;
}

// Wenn in diesem System Truppen stationiert sind, dann wird deren Moralwert mit einbezogen.
// Ist die Moral im System unter 100, so wird der Moralwert der Einheit dazuaddiert, wenn er über 100 ist, dann wird
// der Moralwert abgezogen.
void CSystem::IncludeTroopMoralValue(CArray<CTroopInfo>* troopInfo)
{
	if (m_Troops.GetSize() > 0 && m_iMoral != 100)
		for (int i = 0; i < m_Troops.GetSize(); i++)
		{
			BYTE id = m_Troops.GetAt(i).GetID();
			// Wenn die Systemmoral kleiner als 100 ist, dann wird der Moralwert der Truppe dazuaddiert
			if (m_iMoral < 100)
			{
				m_iMoral += troopInfo->GetAt(id).GetMoralValue();
				if (m_iMoral > 100)
					m_iMoral = 100;
			}
			// Wenn die Systemmoral größer als 100 ist, dann wird der Moralwert der Truppe abgezogen
			else if (m_iMoral > 100)
			{
				m_iMoral -= troopInfo->GetAt(id).GetMoralValue();
				if (m_iMoral < 100)
					m_iMoral = 100;
			}
		}
}

// Alle deaktivierten Produktionen zurücksetzen
void CSystem::ClearDisabledProductions()
{
	memset(m_bDisabledProductions, false, sizeof(m_bDisabledProductions));
}

// Funktion setzt alle Werte wieder zurück auf standard.
void CSystem::ResetSystem()
{
	m_sOwnerOfSystem = "";			// Der Besitzer des Systems
	m_dHabitants = 0.0f;			// Einwohner in dem System
	m_iMoral = 100;					// Moral im System
	m_byBlockade = 0;				// Prozentwert der Blockade im System
	// Anzahl der Gebäude des Types
	m_iFoodBuildings = 0;			// Anzahl der Nahrungsgebäude in dem System
	m_iIndustryBuildings = 0;		// Anzahl der Industriegebäude in dem System
	m_iEnergyBuildings = 0;			// Anzahl der Energiegebäude in dem System
	m_iSecurityBuildings = 0;		// Anzahl der Geheimdienstgebäude in dem System
	m_iResearchBuildings = 0;		// Anzahl der Forschungsgebäude in dem System
	m_iTitanMines = 0;				// Anzahl der Titanminen in dem System
	m_iDeuteriumMines = 0;			// Anzahl der Deuteriumminen in dem System
	m_iDuraniumMines = 0;			// Anzahl der Duraniumminen in dem System
	m_iIridiumMines = 0;			// Anzahl der Iridiumminen in dem System
	m_iCrystalMines = 0;			// Anzahl der Crystalminen in dem System
	// Lagerkapazitäten
	m_iFoodStore = 1000;
	m_iTitanStore = 0;
	m_iDeuteriumStore = 0;
	m_iDuraniumStore = 0;
	m_iCrystalStore = 0;
	m_iIridiumStore = 0;
	m_iDeritiumStore = 0;
	m_BuildingDestroy.RemoveAll();
	m_Production.Reset();
	m_AssemblyList.Reset();
	m_Buildings.RemoveAll();
	m_BuildableBuildings.RemoveAll();
	m_AllwaysBuildableBuildings.RemoveAll();
	m_BuildableUpdates.RemoveAll();
	m_BuildableWithoutAssemblylistCheck.RemoveAll();
	m_BuildableShips.RemoveAll();
	m_BuildableTroops.RemoveAll();
	m_TradeRoutes.RemoveAll();
	m_ResourceRoutes.RemoveAll();
	m_byMaxTradeRoutesFromHab = 0;
	m_Troops.RemoveAll();
	m_bAutoBuild = FALSE;

	// Deaktivierte Produktionen zurücksetzen
	ClearDisabledProductions();
}

//////////////////////////////////////////////////////////////////////
// Hilfsfunktionen
//////////////////////////////////////////////////////////////////////
BOOLEAN CSystem::CheckTech(CBuildingInfo* building, CResearch* research)
{
	// nötige Forschungsstufen checken
	if (research->GetBioTech() <  building->GetBioTech())
		return 0;
	if (research->GetEnergyTech() < building->GetEnergyTech())
		return 0;
	if (research->GetCompTech() < building->GetCompTech())
		return 0;		
	if (research->GetPropulsionTech() < building->GetPropulsionTech())
		return 0;
	if (research->GetConstructionTech() < building->GetConstructionTech())
		return 0;
	if (research->GetWeaponTech() < building->GetWeaponTech())
		return 0;
	return 1;
}

BOOLEAN CSystem::CheckPlanet(CBuildingInfo* building, CSector* sector)
{
	BOOLEAN Ok = FALSE;
	BOOLEAN deritium = FALSE;
	int number = sector->GetNumberOfPlanets();
	CPlanet planet;
	for (int i = 0; i < number && Ok == FALSE; i++)
	{
		planet = *sector->GetPlanet(i);
		if (planet.GetColonized())
		{
			//{M,O,L,P,H,Q,K,G,R,F,C,N,A,B,E,Y,I,J,S,T}
			if (building->GetPlanetTypes(PLANETCLASS_A) == 1 && planet.GetClass() == 'A' && Ok == FALSE)	// nach A suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_B) == 1 && planet.GetClass() == 'B' && Ok == FALSE)	// nach B suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_C) == 1 && planet.GetClass() == 'C' && Ok == FALSE)	// nach C suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_E) == 1 && planet.GetClass() == 'E' && Ok == FALSE)	// nach E suchen
				Ok = TRUE;	// D fehlt wieder, weil ja Asteroid
			if (building->GetPlanetTypes(PLANETCLASS_F) == 1 && planet.GetClass() == 'F' && Ok == FALSE)	// nach F suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_G) == 1 && planet.GetClass() == 'G' && Ok == FALSE)	// nach G suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_H) == 1 && planet.GetClass() == 'H' && Ok == FALSE)	// nach H suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_I) == 1 && planet.GetClass() == 'I' && Ok == FALSE)	// nach I suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_J) == 1 && planet.GetClass() == 'J' && Ok == FALSE)	// nach J suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_K) == 1 && planet.GetClass() == 'K' && Ok == FALSE)	// nach K suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_L) == 1 && planet.GetClass() == 'L' && Ok == FALSE)	// nach L suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_M) == 1 && planet.GetClass() == 'M' && Ok == FALSE)	// nach M suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_N) == 1 && planet.GetClass() == 'N' && Ok == FALSE)	// nach N suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_O) == 1 && planet.GetClass() == 'O' && Ok == FALSE)	// nach O suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_P) == 1 && planet.GetClass() == 'P' && Ok == FALSE)	// nach P suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_Q) == 1 && planet.GetClass() == 'Q' && Ok == FALSE)	// nach Q suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_R) == 1 && planet.GetClass() == 'R' && Ok == FALSE)	// nach R suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_S) == 1 && planet.GetClass() == 'S' && Ok == FALSE)	// nach S suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_T) == 1 && planet.GetClass() == 'T' && Ok == FALSE)	// nach T suchen
				Ok = TRUE;
			if (building->GetPlanetTypes(PLANETCLASS_Y) == 1 && planet.GetClass() == 'Y' && Ok == FALSE)	// nach Y suchen
				Ok = TRUE;
		}
	}
	
	// Wenn das Gebäude Deritium produziert, so muss auf einem kolonisierten Planeten auch Deritium vorhanden sein
	if (building->GetDeritiumProd() > 0)
	{
		for (int i = 0; i < sector->GetNumberOfPlanets(); i++)
			if (sector->GetPlanet(i)->GetColonized())		
				deritium |= sector->GetPlanet(i)->GetBoni()[DERITIUM];
		if (!deritium || !Ok)
			return 0;
	}
	if (Ok == FALSE)	// Haben also kein Planet im System gefunden, auf dem wir das Gebäude bauen könnten
		return 0;
	else
		return 1;
}

BOOLEAN CSystem::CheckGeneralConditions(CBuildingInfo* building, CSector* sector, CGlobalBuildings* globals, CMajor* pMajor)
{
/*	Allgemeine Voraussetzungen
	--------------------------
	benötigte Anzahl eigener Systeme
	max X mal pro System
	max X mal pro Imperium
	Nur Heimatplanet
	Nur eigene Kolonie
	Nur Minorraceplanet
	Nur erobertes System
	Nur baubar in System mit Name
	Nur wirklicher Besitzer des Gebäudes
	minimale Bevölkerung im System
	mindst. X Gebäude von ID im System
	mindts. X Gebäude von ID im Imperium	
*/	
	// benötigte Anzahl eigener Systeme
	if (building->GetNeededSystems() > pMajor->GetEmpire()->GetNumberOfSystems())
		return FALSE;
	// Nur Heimatplanet checken
	if (building->GetOnlyHomePlanet())
	{
		if (m_sOwnerOfSystem == pMajor->GetRaceID() && sector->GetName() != pMajor->GetHomesystemName())
			return FALSE;
	}
	// Nur eigene Kolonie checken (Heimatsystem ist auch eine eigene Kolonie)
	if (building->GetOnlyOwnColony())
		if (sector->GetColonyOwner() != m_sOwnerOfSystem)
			return FALSE;
	// Nur Minorraceplanet checken
	if (building->GetOnlyMinorRace())
		if (sector->GetMinorRace() == FALSE)
			return FALSE;
	// Nur erobertes System checken
	if (building->GetOnlyTakenSystem())
		if (sector->GetTakenSector() == FALSE)
			return FALSE;
	// Nur baubar in System mit Name checken
	if (building->GetOnlyInSystemWithName() != "0" && building->GetOnlyInSystemWithName() != "")
		if (building->GetOnlyInSystemWithName() != sector->GetName())
			return FALSE;
	// Nur wirklicher Besitzer des Gebäudes
	if (building->GetOnlyRace())
		if (pMajor->GetRaceBuildingNumber() != building->GetOwnerOfBuilding())
			return FALSE;
	// Minimale Bevölkerungsanzahl checken
	if (building->GetMinHabitants() > 0)
		if (m_dHabitants < (double)building->GetMinHabitants())
			return FALSE;
	// mindst. X Gebäude von ID im System
	if (building->GetMinInSystem().Number > 0)
	{
		USHORT number = 0;
		for (int i = 0; i < m_Buildings.GetSize(); i++)
		{
			if (m_Buildings.GetAt(i).GetRunningNumber() == building->GetMinInSystem().RunningNumber)
				number++;
			if (number >= building->GetMinInSystem().Number)
				break;
		}
		if (number < building->GetMinInSystem().Number)
			return FALSE;
	}
	// max. X Gebäude von ID im System
	if (building->GetMaxInSystem().Number > 0)
	{
		USHORT number = 0;
		for (int i = 0; i < m_Buildings.GetSize(); i++)
		{
			if (m_Buildings.GetAt(i).GetRunningNumber() == building->GetMaxInSystem().RunningNumber)
				number++;
			if (number >= building->GetMaxInSystem().Number)
				return FALSE;
		}
	}
	// max. X Gebäude von ID im Imperium
	if (building->GetMaxInEmpire() > 0)
	{
		int nCount = globals->GetCountGlobalBuilding(m_sOwnerOfSystem, building->GetRunningNumber());
		if (nCount >= building->GetMaxInEmpire())
			return FALSE;
	}	

	return TRUE;
}

// Diese private Hilfsfunktion überprüft, ob es einen Nachfolger zu unserem Gebäude in der Liste der baubaren
// Gebäude (-> flag == 0) oder als stehendes Gebäude (-> flag == 1) im System schon steht. Die Funktion 
// gibt TRUE zurück, wenn wir einen Nachfolger gefunden haben, ansonsten FALSE.
BOOLEAN CSystem::CheckFollower(BuildingInfoArray* buildings, USHORT id, BOOLEAN flag, BOOLEAN equivalence)
{
	// Checken ob ein potentieller Nachfolger in der Liste der baubaren Gebäude vorkommt
	if (flag == 0)
	{	
		/*	zum Algorithmus:
			Ich durchsuche jede ID im Feld der baubaren Gebäude nach potentiellen Vorgängern. Ist dann einer
			der Vorgänger meine ID, dann können wir abbrechen.
		*/
		for (int i = 0; i < m_BuildableWithoutAssemblylistCheck.GetSize(); i++)
		{
			// Weil zu diesem Zeitpunkt auch Gebäudeupgrades (negative ID) in diesem Feld vorkommen können, darf ich
			// nur die echten Gebäude betrachten
			if (m_BuildableWithoutAssemblylistCheck.GetAt(i) > 0)
			{
				// VorgängerID eines Gebäudes aus der Liste der baubaren Gebäude holen
				USHORT pre = buildings->GetAt(m_BuildableWithoutAssemblylistCheck.GetAt(i)-1).GetPredecessorID();
				// Haben wir jetzt schon den Vorgänger gefunden (eher unwahrscheinlich)
				if (pre == id)
					return TRUE;
				// jetzt in die Schleife um sich immer neue ID´s zu holen
				int z = 0;
				do {
					// VorgängerID des Vorgängers holen
					if (pre > 0)
					{
						USHORT oldPre = pre;
						pre = buildings->GetAt(pre-1).GetPredecessorID();
						if (oldPre == pre)
						{							
							CString s;
							s.Format("ERROR with Buildings Predecessor ID:\nBuilding: %s\n\nID: %d\nit is the same like the ID of the building", buildings->GetAt(oldPre-1).GetBuildingName(), oldPre);
							AfxMessageBox(s);
							break;
						}			
					}					
				} while (pre != id && pre != 0);// mache das solange wir keinen Vorgänger gefunden haben oder es keinen Vorgänger gibt
				// Gibt es einen Vorgänger
				if (pre == id)
					return TRUE;				
											
				// So, sind wir jetzt noch dabei und haben in dem System auch Gebäude von anderen Rassen stehen (also der
				// Parameter equivalence == TRUE), dannn müssen wir auch noch bei dem Äquivalenzgebäude nach Vorgängern suchen
				if (equivalence == TRUE)
				{
					// VorgängerID des entsprechenden Äquivalenzgebäudes holen
					USHORT race = buildings->GetAt(id-1).GetOwnerOfBuilding();
					//USHORT race = buildings->GetAt(m_BuildableWithoutAssemblylistCheck.GetAt(i)-1).GetOwnerOfBuilding();
					USHORT equi = buildings->GetAt(m_BuildableWithoutAssemblylistCheck.GetAt(i)-1).GetEquivalent(race);
/*					if (equi > 0)
					{					
						pre = buildings->GetAt(equi-1).GetPredecessorID();	// pre ist Vorgänger des Äquivalenzgebäudes
						// Jetzt wie oben die do - while Schleife durchlaufen
						do {
							// VorgängerID des "Vorgängers holen
							if (pre > 0)
								pre = buildings->GetAt(pre-1).GetPredecessorID();
						} while (pre != equi && pre != 0);// mache das solange wir keinen Vorgänger gefunden haben oder es keinen Vorgänger gibt
						// Gibt es einen Vorgänger
						if (pre == equi)
							return TRUE;
					}*/
					if (equi == id)
						return TRUE;
					pre = buildings->GetAt(m_BuildableWithoutAssemblylistCheck.GetAt(i)-1).GetPredecessorID();
					// Jetzt wir oben die do - while Schleife durchlaufen
					do {
						// VorgängerID des "Vorgängers holen
						if (pre > 0)
						{
							USHORT oldPre = pre;
							equi = buildings->GetAt(pre-1).GetEquivalent(race);
							pre = buildings->GetAt(pre-1).GetPredecessorID();
							if (oldPre == pre)
							{							
								CString s;
								s.Format("ERROR with Buildings Predecessor ID:\nBuilding: %s\n\nID: %d\nit is the same like the ID of the building", buildings->GetAt(oldPre-1).GetBuildingName(), oldPre);
								AfxMessageBox(s);
								break;
							}	
						}						
					} while (id != equi && pre != 0); // mache das solange wir keinen Vorgänger gefunden haben oder es keinen Vorgänger gibt
					// Gibt es einen Vorgänger
					if (equi == id)
						return TRUE;					
				}
			}
		}
	}
	// Checken ob ein potentieller Nachfolger schon im System steht. Normalerweise ist dieser schon in der Liste
	// der baubaren Gebäude, aber z.B. Gebäude die nur einmal pro System gebaut werden können sind nicht in dieser Liste.
	// z.B. wenn wir eine geupdatede Werft einer anderen Rasse in dem System stehen haben, darf ich nicht einen
	// potentiellen äquivalenten Vorgänger dieser Werft unsererseits bauen.
	else
	{	
		USHORT minID = 0;
		// Der Algorithmus ist wie der obere. Nur wird hier das Feld der stehenden Gebäude im System durchlaufen
		for (int i = 0; i < m_Buildings.GetSize(); i++)
		{
			// Da das Gebäudefeld nach der RunningNumber geordnet ist, brauchen wir erst wieder checken
			// wenn wir ein Gebäude mit einer neune RunningNumber haben
			if (m_Buildings.GetAt(i).GetRunningNumber() > minID)
				minID = m_Buildings.GetAt(i).GetRunningNumber();
			{
				// VorgängerID eines Gebäudes aus der Liste der stehenden Gebäude holen
				USHORT pre = buildings->GetAt(m_Buildings.GetAt(i).GetRunningNumber()-1).GetPredecessorID();
				// Haben wir jetzt schon den Vorgänger gefunden (eher unwahrscheinlich)
				if (pre == id)
					return TRUE;				
				// jetzt in die Schleife um sich immer neue ID´s zu holen
				do {
					// VorgängerID des "Vorgängers holen
					if (pre > 0)
					{
						USHORT oldPre = pre;
						pre = buildings->GetAt(pre-1).GetPredecessorID();
						if (oldPre == pre)
						{							
							CString s;
							s.Format("ERROR with Buildings Predecessor ID:\nBuilding: %s\n\nID: %d\nit is the same like the ID of the building", buildings->GetAt(oldPre-1).GetBuildingName(), oldPre);
							AfxMessageBox(s);
							break;
						}
					}					
				} while (pre != id && pre != 0);// mache das solange wir keinen Vorgänger gefunden haben oder es keinen Vorgänger gibt
				// Gibt es einen Vorgänger
				if (pre == id)
					return TRUE;				

				// So, sind wir jetzt noch dabei und haben in dem System auch Gebäude von anderen Rassen stehen (also der
				// Parameter equivalence == TRUE), dannn müssen wir auch noch bei dem Äquivalenzgebäude nach Vorgängern suchen
				if (equivalence == TRUE)
				{
					// VorgängerID des entsprechenden Äquivalenzgebäudes holen
					USHORT race = buildings->GetAt(id-1).GetOwnerOfBuilding();
					USHORT equi = buildings->GetAt(m_Buildings.GetAt(i).GetRunningNumber()-1).GetEquivalent(race);
					if (equi == id)
						return TRUE;
					pre = buildings->GetAt(m_Buildings.GetAt(i).GetRunningNumber()-1).GetPredecessorID();
					// Jetzt wir oben die do - while Schleife durchlaufen
					do {
						// VorgängerID des "Vorgängers holen
						if (pre > 0)
						{
							USHORT oldPre = pre;
							equi = buildings->GetAt(pre-1).GetEquivalent(race);
							pre = buildings->GetAt(pre-1).GetPredecessorID();
							if (oldPre == pre)
							{							
								CString s;
								s.Format("ERROR with Buildings Predecessor ID:\nBuilding: %s\n\nID: %d\nit is the same like the ID of the building", buildings->GetAt(oldPre-1).GetBuildingName(), oldPre);
								AfxMessageBox(s);
								break;
							}	
						}						
					} while (id != equi && pre != 0);// mache das solange wir keinen Vorgänger gefunden haben oder es keinen Vorgänger gibt
					// Gibt es einen Vorgänger
					if (id == equi)
						return TRUE;					
				}
			}
		}
	}
	return FALSE;
}