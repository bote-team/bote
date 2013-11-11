#include "stdafx.h"
#include "SystemAI.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"
#include "AIPrios.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CSystemAI::CSystemAI(CBotEDoc* pDoc) : m_pMajor()
{
	AssertBotE(pDoc);
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
	CString sRace = m_pDoc->GetSystem(ko.x, ko.y).GetOwnerOfSystem();
	if (sRace.IsEmpty())
	{
		CString s;
		s.Format("Error in CSystemAI::ExecuteSystemAI(): no race controls system %d,%d!", ko.x, ko.y);
		AfxMessageBox(s);
		return;
	}

	// Besitzer des Systems holen
	m_pMajor = dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(sRace));
	AssertBotE(m_pMajor);
	if (!m_pMajor)
		return;

	m_KO = ko;
	KillTroops();
	PerhapsBuy();
	CalcPriorities();
	AssignWorkers();
	ScrapBuildings();
	ApplyTradeRoutes();
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
void CSystemAI::KillTroops() {
	CSystem& system = m_pDoc->GetSystem(m_KO.x, m_KO.y);
	CArray<CTroop>& troops = *system.GetTroops();
	troops.RemoveAll();
}

/// Diese Funktion kauft unter Umständen den aktuellen Bauauftrag. Somit kommt der Ausbaue schneller voran.
void CSystemAI::PerhapsBuy()
{
	CPoint p = m_KO;
	CSystem& system = m_pDoc->GetSystem(p.x, p.y);

	// Wenn kein Bauauftrag in der Liste steht, so kann die Funktion sofort verlassen werden.
	int id = system.GetAssemblyList()->GetAssemblyListEntry(0);
	if (id == NULL)
		return;
	int roundToBuild = 0;
	if (system.GetProduction()->GetIndustryProd() > 0)
	{
		// Never-Ready Auftrag
		if (id > 0 && id < 10000 && m_pDoc->GetBuildingInfo(id).GetNeverReady())
			roundToBuild = 0;
		// Bei Upgrades
		else if (id < 0)
		{
			roundToBuild = (int)ceil((float)(system.GetAssemblyList()->GetNeededIndustryInAssemblyList(0))
				/((float)system.GetProduction()->GetIndustryProd()
					* (100+system.GetProduction()->GetUpdateBuildSpeed())/100));
		}
		// Bei Gebäuden
		else if (id < 10000)
		{
			roundToBuild = (int)ceil((float)(system.GetAssemblyList()->GetNeededIndustryInAssemblyList(0))
				/((float)system.GetProduction()->GetIndustryProd()
					* (100+system.GetProduction()->GetBuildingBuildSpeed())/100));
		}
		// Bei Schiffen Wertfeffiziens mitbeachten
		else if (id < 20000 && system.GetProduction()->GetShipYardEfficiency() > 0)
		{
			roundToBuild = (int)ceil((float)(system.GetAssemblyList()->GetNeededIndustryInAssemblyList(0))
				/((float)system.GetProduction()->GetIndustryProd() * system.GetProduction()->GetShipYardEfficiency() / 100
					* (100+system.GetProduction()->GetShipBuildSpeed())/100));
		}
		// Bei Truppen die Kaserneneffiziens beachten
		else if (system.GetProduction()->GetBarrackEfficiency() > 0)
		{
			roundToBuild = (int)ceil((float)(system.GetAssemblyList()->GetNeededIndustryInAssemblyList(0))
				/((float)system.GetProduction()->GetIndustryProd() * system.GetProduction()->GetBarrackEfficiency() / 100
					* (100+system.GetProduction()->GetTroopBuildSpeed())/100));
		}
	}
	// Sobald eine Rundendauer vorhanden ist, kann über den Kauf des Auftrages nachgedacht werden.
	if (roundToBuild > 1)
	{
		system.GetAssemblyList()->CalculateBuildCosts(m_pMajor->GetTrade()->GetRessourcePriceAtRoundStart());
		int costs = system.GetAssemblyList()->GetBuildCosts();
		int value = (m_pMajor->GetEmpire()->GetCredits() / costs) * 5;
		// Umso mehr Credits das Imperium besitzt, desto eher wird gekauft. Außerdem wird bei einer niedrigen Moral
		// eher versucht den Kauf zu tätigen, um nächstes Mal einen Polizeistaat oder ähnliches schneller starten zu können
		if (rand()%100 < value || (value > 0 && system.GetMoral() < (rand()%21 + 60)))
		{
			costs = system.GetAssemblyList()->BuyBuilding(m_pMajor->GetEmpire()->GetCredits());
			if (costs != 0)
			{
				system.GetAssemblyList()->SetWasBuildingBought(TRUE);
				m_pMajor->GetEmpire()->SetCredits(-costs);
				// Die Preise an der Börse anpassen, da wir ja bestimmte Mengen Ressourcen gekauft haben
				// Achtung, hier flag == 1 setzen bei Aufruf der Funktion BuyRessource!!!!
				m_pMajor->GetTrade()->BuyRessource(RESOURCES::TITAN,	 system.GetAssemblyList()->GetNeededTitanInAssemblyList(0),p,m_pMajor->GetEmpire()->GetCredits(),true);
				m_pMajor->GetTrade()->BuyRessource(RESOURCES::DEUTERIUM,system.GetAssemblyList()->GetNeededDeuteriumInAssemblyList(0),p,m_pMajor->GetEmpire()->GetCredits(),true);
				m_pMajor->GetTrade()->BuyRessource(RESOURCES::DURANIUM, system.GetAssemblyList()->GetNeededDuraniumInAssemblyList(0),p,m_pMajor->GetEmpire()->GetCredits(),true);
				m_pMajor->GetTrade()->BuyRessource(RESOURCES::CRYSTAL,  system.GetAssemblyList()->GetNeededCrystalInAssemblyList(0),p,m_pMajor->GetEmpire()->GetCredits(),true);
				m_pMajor->GetTrade()->BuyRessource(RESOURCES::IRIDIUM,  system.GetAssemblyList()->GetNeededIridiumInAssemblyList(0),p,m_pMajor->GetEmpire()->GetCredits(),true);
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
	if ((m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->GetAssemblyListEntry(0) >= 10000
		&& m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->GetAssemblyListEntry(0) <= 20000
		&& m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetShipYard() == FALSE)
		|| (m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->GetAssemblyListEntry(0) >= 20000
		&& m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetBarrack() == FALSE))
	{
		// Bauauftrag entfernen
		// CHECK WW: KI sollte hier anteilige Ressourcen zurückbekommen
		m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->ClearAssemblyList(ko, m_pDoc->m_Systems);
		m_pDoc->GetSystem(ko.x, ko.y).CalculateVariables();
	}

	// Wenn die Moral in dem System sehr niedrig ist, dann wird versucht ein Moralgebäude bzw. Polizeistaat oder ähnliches
	// laufen zu lassen. Dadurch wird versucht zu verhindern, dass sich Systeme lossagen
	if (m_pDoc->GetSystem(ko.x, ko.y).GetMoral() < (rand()%16 + 70))
	{
		// ist Kriegsrecht, Polizeitstaat oder ähnliches nicht in der Bauliste?
		short nEntry = m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->GetAssemblyListEntry(0);
		if (nEntry > 0 && nEntry < 10000)
			if (m_pDoc->GetBuildingInfo(nEntry).GetMoralProd() > 0 && m_pDoc->GetBuildingInfo(nEntry).GetNeverReady())
				// dann ist schon das richige Gebäude in der Liste und es braucht kein neues gesucht zu werden
				return;

		// wenn das Gebäude nicht mehr lang zum fertigbauen braucht (weniger als 3 - 6 Runden), dann wird es auch nicht entfernt
		int nIP = m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetIndustryProd();
		if (nEntry != 0 && nIP > 0)
		{
			int nRoundToBuild = 0;
			// Updates
			if (nEntry < 0)
			{
				nRoundToBuild = (int)ceil((float)(m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->GetNeededIndustryInAssemblyList(0)) / ((float)nIP * (100+m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetUpdateBuildSpeed())/100));
			}
			// normales Gebäude
			else if (nEntry < 10000)
			{
				if (m_pDoc->GetBuildingInfo(nEntry).GetNeverReady() == false)
				{
					nRoundToBuild = (int)ceil((float)(m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->GetNeededIndustryInAssemblyList(0)) / ((float)nIP * (100+m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetBuildingBuildSpeed())/100));
				}
			}
			// Schiffe
			else if (nEntry < 20000)
			{
				if (m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetShipYardEfficiency() > 0)
				{
					nRoundToBuild = (int)ceil((float)(m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->GetNeededIndustryInAssemblyList(0)) / ((float)nIP * m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetShipYardEfficiency() / 100 * (100+m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetShipBuildSpeed())/100));
				}
			}
			else
			{
				if (m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetBarrackEfficiency() > 0)
				{
					nRoundToBuild = (int)ceil((float)(m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->GetNeededIndustryInAssemblyList(0)) / ((float)nIP * m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetBarrackEfficiency() / 100	* (100+m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetTroopBuildSpeed())/100));
				}
			}
			// braucht der Auftrag noch länger als 3 bis 6 Runden oder die Moral ist auf unter 50 gefallen
			if (nRoundToBuild > rand()%4 + 3 || m_pDoc->GetSystem(ko.x, ko.y).GetMoral() < 50)
			{
				// Bau abbrechen
				// CHECK WW: KI sollte hier anteilige Ressourcen zurückbekommen
				m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->ClearAssemblyList(ko, m_pDoc->m_Systems);
				m_pDoc->GetSystem(ko.x, ko.y).CalculateVariables();
				MYTRACE("ai")(MT::LEVEL_INFO, "CSystemAI::CalcPriorities(): Removed current buildorder because of low moral in System '%s'\n", m_pDoc->GetSector(ko.x, ko.y).GetName());
			}
		}

		// Kriegsrecht, Polizeistaat oder ähnliches suchen, welches in die Bauliste gesetzt werden kann um die Moral zu erhöhen
		CArray<short> buildings;
		for (int i = 0; i < m_pDoc->GetSystem(ko.x, ko.y).GetBuildableBuildings()->GetSize(); i++)
		{
			int id = m_pDoc->GetSystem(ko.x, ko.y).GetBuildableBuildings()->GetAt(i);
			if (m_pDoc->GetBuildingInfo(id).GetMoralProd() > 0 && m_pDoc->GetBuildingInfo(id).GetNeverReady())
				buildings.Add(id);
		}

		while (buildings.GetSize() > 0)
		{
			int nRandom = rand()%buildings.GetSize();
			if (MakeEntryInAssemblyList(buildings.GetAt(nRandom)))
			{
				MYTRACE("ai")(MT::LEVEL_INFO, "CSystemAI::CalcPriorities(): Found building to increase moral in System '%s'\n", m_pDoc->GetSector(ko.x, ko.y).GetName());
				// Moralverbesserungsgebäude gefunden -> aus Funktion springen
				return;
			}
			else
				buildings.RemoveAt(nRandom);
		}
	}

	// Checken ob schon ein Eintrag in der Bauliste ist, wenn ja dann brauchen wir hier überhaupt nichts zu machen
	if (m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->GetAssemblyListEntry(0) != 0)
		return;

	double dMaxHab = 0.0;
	const CSector& sector = m_pDoc->GetSector(ko.x, ko.y);
	for(CSector::const_iterator it = sector.begin(); it != sector.end(); ++it)
		if (it->GetCurrentHabitant() > 0.0)
			dMaxHab += it->GetMaxHabitant();
	// wenn die Maximale Anzahl an Einwohnern 1.25 mal größer als die aktuelle Anzahl der Einwoher ist, dann
	// werden Gebäudebauprioritäten verdoppelt
	dMaxHab	= max(1.0, dMaxHab);

	// zuallererst werden erstmal alle Prioritäten berechnet
	// Nahrung
	m_iPriorities[WORKER::FOOD_WORKER] = GetFoodPrio(dMaxHab);
	// Industrie
	m_iPriorities[WORKER::INDUSTRY_WORKER] = GetIndustryPrio(dMaxHab);
	// Energie
	m_iPriorities[WORKER::ENERGY_WORKER] = GetEnergyPrio(dMaxHab);
	// Geheimdienst
	m_iPriorities[WORKER::SECURITY_WORKER] = GetIntelPrio(dMaxHab);
	// Forschung
	m_iPriorities[WORKER::RESEARCH_WORKER] = GetResearchPrio(dMaxHab);
	// Titan
	m_iPriorities[WORKER::TITAN_WORKER] = GetResourcePrio(WORKER::TITAN_WORKER, dMaxHab);
	// Deuterium
	m_iPriorities[WORKER::DEUTERIUM_WORKER] = GetResourcePrio(WORKER::DEUTERIUM_WORKER, dMaxHab);
	// Duranium
	m_iPriorities[WORKER::DURANIUM_WORKER] = GetResourcePrio(WORKER::DURANIUM_WORKER, dMaxHab);
	// Kristalle
	m_iPriorities[WORKER::CRYSTAL_WORKER] = GetResourcePrio(WORKER::CRYSTAL_WORKER, dMaxHab);
	// Iridium
	m_iPriorities[WORKER::IRIDIUM_WORKER] = GetResourcePrio(WORKER::IRIDIUM_WORKER, dMaxHab);

	m_bCalcedPrio = TRUE;

	CString name;
	int id = ChooseBuilding();
	if (id == 0)
	{
		// Machen wir vielleicht ein Update
		if (m_pDoc->GetSystem(ko.x, ko.y).GetBuildableUpdates()->GetSize() > 0)
		{
			int random = rand()%m_pDoc->GetSystem(ko.x, ko.y).GetBuildableUpdates()->GetSize();
			id = m_pDoc->GetSystem(ko.x, ko.y).GetBuildableUpdates()->GetAt(random)*(-1);
			if (!MakeEntryInAssemblyList(id))
			// konnte kein Update in die Bauliste gesetzt werden, so wird hier nochmal versucht ein Schiff zu bauen
			{
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
		if (m_pDoc->GetBuildingInfo(id).GetMaxInEmpire() > 0)
		{
			// Wir müssen die GlobalBuilding Variable ändern, weil sich mittlerweile ja solch ein Gebäude
			// mehr in der Bauliste befindet. Nicht aber wenn es ein Upgrade ist.
			m_pDoc->m_GlobalBuildings.AddGlobalBuilding(m_pMajor->GetRaceID(), id);
			// Da es nur einmal pro Imperium baubar war, dann Assemblylistcheck in jedem unserer Systeme
			// durchführen
			for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
				for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
					if (m_pMajor->GetRaceBuildingNumber() == m_pDoc->GetBuildingInfo(id).GetOwnerOfBuilding())
						m_pDoc->GetSystem(ko.x, ko.y).AssemblyListCheck(&m_pDoc->BuildingInfo, &m_pDoc->m_GlobalBuildings);
		}
	}
/*
	if (id > 0 && id < 10000)
		name = m_pDoc->GetBuildingName(id);

	if (m_pDoc->GetSector(ko.x, ko.y).GetName() == "Tinaca")
	{
		CString s;
		s.Format("System: %s\n\nFood: %d\nIndstry: %d\nEnergy: %d\nSecurity: %d\nResearch: %d\nTitan: %d\nDeuterium: %d\nDuranium: %d\nCrystal: %d\nIridium: %d\n\nchoosen Building: %s\nID: %d\nAssemblyListEntry: %d\nneeded IP: %d",
			m_pDoc->GetSector(ko.x, ko.y).GetName(),
			m_iPriorities[0],m_iPriorities[1],m_iPriorities[2],m_iPriorities[3],m_iPriorities[4],m_iPriorities[5],m_iPriorities[6],
			m_iPriorities[7],m_iPriorities[8],m_iPriorities[9],name,id,m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->GetAssemblyListEntry(0),
			m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->GetNeededIndustryForBuild());
		AfxMessageBox(s);
	}
*/
	if (id == 0)
		MYTRACE("ai")(MT::LEVEL_INFO, "CSystemAI::CalcPriorities(): Could not create buildcontract in system '%s'\n", m_pDoc->GetSector(ko.x, ko.y).GetName());
}

bool CSystemAI::CheckMoral(const CBuildingInfo& bi, bool build) const {
	//yes, there is a minor with such useless building...
	if(bi.GetMoralProdEmpire() < 0)
		return false;
	const short mp = bi.GetMoralProd();
	if(mp >= 0)
		return true;
	// I would usually only build, if I could still maintain moral +5 in this system
	// don't know empire wide moral prod, usually at least +1
	const CSystem& s = m_pDoc->GetSystem(m_KO.x, m_KO.y);
	if(build)
		return 2 <= s.GetProduction()->GetMoralProd() + mp;
	//in practice, AI will take minus moral buildings online very seldom, and that's better
	return s.GetMoral() > 173;
}

/// Diese Funktion wählt ein zu bauendes Gebäude aus der Liste der baubaren Gebäude. Es werden nur Gebäude
/// ausgewählt, welche in die Prioritätenliste passen. Der Rückgabewert ist die ID des Bauauftrages.
/// Wird <code>0</code> zurückgegeben, so wurde kein Gebäude gefunden, welches in das Anforderungsprofil passt.
int CSystemAI::ChooseBuilding()
{
	CPoint ko = m_KO;
	CString race = m_pDoc->GetSystem(ko.x, ko.y).GetOwnerOfSystem();
	BOOLEAN chooseCombatship = FALSE;
	BOOLEAN chooseColoship   = FALSE;
	BOOLEAN chooseTransport	 = FALSE;

	// Aus der Prioritätenliste wird derjenige Eintrag ausgewählt, welcher die höchste Priorität aufweist.
	// Dieser wird mit der höchsten Wahrscheinlichket ausgewählt.
	WORKER::Typ nChoosenPrio = WORKER::NONE;
	int min = GetShipBuildPrios(chooseCombatship, chooseColoship, chooseTransport);
	int shipBuildPrio = min;
	MYTRACE("ai")(MT::LEVEL_INFO, "CSystemAI::ChooseBuilding(): min priority after ships: %d\n", min);
	// sind Updates baubar, so werden die Prioritäten der anderen womöglich etwas verringert, so dass häufiger
	// zuerst die Updates gebaut werden. Außer wir haben freie Arbeiter übrig.
	int nUpdates = rand()%(m_pDoc->GetSystem(ko.x, ko.y).GetBuildableUpdates()->GetSize()+1);
	min -= m_pDoc->GetSystem(ko.x, ko.y).GetWorker(WORKER::FREE_WORKER);
	MYTRACE("ai")(MT::LEVEL_INFO, "CSystemAI::ChooseBuilding(): min priority after ships, updates and workers: %d\n", min);
	for (int i = WORKER::FOOD_WORKER; i <= WORKER::IRIDIUM_WORKER; i++)
	{
		WORKER::Typ nWorker = (WORKER::Typ)i;
		if (m_iPriorities[nWorker] > min)
		{
			int nRandom = rand()%(m_iPriorities[nWorker] + 1);
			if (nRandom - nUpdates + m_pDoc->GetSystem(ko.x, ko.y).GetWorker(WORKER::FREE_WORKER) > 0)
			{
				min = m_iPriorities[nWorker];
				nChoosenPrio = nWorker;
			}
		}
	}

	if (nChoosenPrio != WORKER::NONE)
	{
		MYTRACE("ai")(MT::LEVEL_INFO, "CSystemAI::ChooseBuilding(): choosen prio: %d\n", nChoosenPrio);
		// Gebäude auswählen
		for (int i = m_pDoc->GetSystem(ko.x, ko.y).GetBuildableBuildings()->GetUpperBound(); i >= 0; i--)
		{
			int id = m_pDoc->GetSystem(ko.x, ko.y).GetBuildableBuildings()->GetAt(i);
			// ist der Moralmalus des Gebäudes größer unserer Moralproduktion, so wird das Gebäude nicht gebaut
			if (!CheckMoral(m_pDoc->GetBuildingInfo(id), true))
				continue;
			// finden wir hier eine Werft, so wird versucht die Werft jetzt zu bauen
			if (m_pDoc->GetBuildingInfo(id).GetShipYard())
				if (MakeEntryInAssemblyList(id))
					return id;
			switch (nChoosenPrio)
			{
			case WORKER::FOOD_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetFoodProd() > 0 || m_pDoc->GetBuildingInfo(id).GetFoodBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case WORKER::INDUSTRY_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetIPProd() > 0 || m_pDoc->GetBuildingInfo(id).GetIndustryBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case WORKER::ENERGY_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetEnergyProd() > 0 || m_pDoc->GetBuildingInfo(id).GetEnergyBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case WORKER::SECURITY_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetSPProd() > 0 || m_pDoc->GetBuildingInfo(id).GetSecurityBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case WORKER::RESEARCH_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetFPProd() > 0 || m_pDoc->GetBuildingInfo(id).GetResearchBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case WORKER::TITAN_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetTitanProd() > 0 || m_pDoc->GetBuildingInfo(id).GetTitanBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case WORKER::DEUTERIUM_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetDeuteriumProd() > 0 || m_pDoc->GetBuildingInfo(id).GetDeuteriumBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case WORKER::DURANIUM_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetDuraniumProd() > 0 || m_pDoc->GetBuildingInfo(id).GetDuraniumBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case WORKER::CRYSTAL_WORKER:
				if (m_pDoc->GetBuildingInfo(id).GetCrystalProd() > 0 || m_pDoc->GetBuildingInfo(id).GetCrystalBoni() > 0)
					if (MakeEntryInAssemblyList(id))
						return id;
				break;
			case WORKER::IRIDIUM_WORKER:
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
		if (rand()%(m_pDoc->GetSystem(ko.x, ko.y).GetWorker(WORKER::FREE_WORKER)+1)/2 > 0)
			return 0;

		// Liste erstellen, in der die ID's der möglichen Gebäude stehen.
		CArray<short> buildings;
		for (int i = 0; i < m_pDoc->GetSystem(ko.x, ko.y).GetBuildableBuildings()->GetSize(); i++)
		{
			int id = m_pDoc->GetSystem(ko.x, ko.y).GetBuildableBuildings()->GetAt(i);
			if (m_pDoc->GetBuildingInfo(id).GetWorker() == FALSE && m_pDoc->GetBuildingInfo(id).GetNeverReady() == FALSE)
				buildings.Add(id);
		}
		while (buildings.GetSize() > 0)
		{
			const int random = rand()%buildings.GetSize();
			// Gebäude mit einer negativen Moral werden nicht von der KI gebaut, wenn die Moralproduktion im System
			// dies nicht ausgleichen kann
			if (m_pDoc->GetBuildingInfo(buildings.GetAt(random)).GetMoralProd() < NULL
				&& abs(m_pDoc->GetBuildingInfo(buildings.GetAt(random)).GetMoralProd()) >= m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetMoralProd())
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
	CString sRace = m_pDoc->GetSystem(ko.x, ko.y).GetOwnerOfSystem();
	if (sRace.IsEmpty())
		return 0;

	CRace* pRace = m_pDoc->GetRaceCtrl()->GetRace(sRace);
	AssertBotE(pRace);
	if (!pRace->IsMajor())
		return 0;

	const CResearch& research = *dynamic_cast<CMajor*>(pRace)->GetEmpire()->GetResearch();
	const BYTE researchLevels[6] =
	{

		research.GetBioTech(),
		research.GetEnergyTech(),
		research.GetCompTech(),
		research.GetPropulsionTech(),
		research.GetConstructionTech(),
		research.GetWeaponTech()
	};

	// Wenn ein Kolonieschiff gebaut werden sollte
	if (chooseColoship && min > 0)
	{
		// ID des Schiffes suchen
		for (int j = 0; j < m_pDoc->m_ShipInfoArray.GetSize(); j++)
			// passt die Schiffsnummer zur Rassennummer
			if(dynamic_cast<CMajor*>(pRace)->CanBuildShip(SHIP_TYPE::COLONYSHIP,
				researchLevels, m_pDoc->m_ShipInfoArray.GetAt(j)))
			{
				int id = m_pDoc->m_ShipInfoArray.GetAt(j).GetID();
				for (int i = 0; i < m_pDoc->GetSystem(ko.x, ko.y).GetBuildableShips()->GetSize(); i++)
					if (m_pDoc->GetSystem(ko.x, ko.y).GetBuildableShips()->GetAt(i) == id)
						if (MakeEntryInAssemblyList(id))
						{
							m_pDoc->m_pAIPrios->ChoosedColoShipPrio(sRace);
							MYTRACE("ai")(MT::LEVEL_INFO, "CSystemAI::ChooseShip(): build colonyship in system: %s\n", m_pDoc->GetSector(m_KO.x, m_KO.y).GetName());
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
			if(dynamic_cast<CMajor*>(pRace)->CanBuildShip(SHIP_TYPE::TRANSPORTER,
				researchLevels, m_pDoc->m_ShipInfoArray.GetAt(j)))
			{
				int id = m_pDoc->m_ShipInfoArray.GetAt(j).GetID();
				for (int i = 0; i < m_pDoc->GetSystem(ko.x, ko.y).GetBuildableShips()->GetSize(); i++)
					if (m_pDoc->GetSystem(ko.x, ko.y).GetBuildableShips()->GetAt(i) == id)
						if (MakeEntryInAssemblyList(id))
						{
							m_pDoc->m_pAIPrios->ChoosedTransportShipPrio(sRace);
							MYTRACE("ai")(MT::LEVEL_INFO, "CSystemAI::ChooseShip(): build transportship in system: %s\n", m_pDoc->GetSector(m_KO.x, m_KO.y).GetName());
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
		for (int i = 0; i < m_pDoc->GetSystem(ko.x, ko.y).GetBuildableShips()->GetSize(); i++)
		{
			int id = m_pDoc->GetSystem(ko.x, ko.y).GetBuildableShips()->GetAt(i);
			if (m_pDoc->m_ShipInfoArray.GetAt(id-10000).GetShipType() > SHIP_TYPE::COLONYSHIP)
			{
				UINT strenght = m_pDoc->m_ShipInfoArray.GetAt(id-10000).GetCompleteOffensivePower() +
					m_pDoc->m_ShipInfoArray.GetAt(id-10000).GetCompleteDefensivePower() / 2;
				ships.Add(SHIPLIST(id, strenght));
			}
		}
		// Feld nach der Stärke der Schiffe ordnen
		c_arraysort<CArray<SHIPLIST>, SHIPLIST> (ships, sort_desc);
		if(MT::CMyTrace::IsLoggingEnabledFor("ai"))
		{
			MYTRACE_CHECKED("ai")(MT::LEVEL_INFO, "CSystemAI::ChooseShip(): build combatship in system: %s\n", m_pDoc->GetSector(m_KO.x, m_KO.y).GetName());
			for (int i = 0; i < ships.GetSize(); i++)
				MYTRACE_CHECKED("ai")(MT::LEVEL_INFO, "CSystemAI::ChooseShip(): buildable combatships %s - ID: %d - Power: %d\n", m_pDoc->m_ShipInfoArray[ships.GetAt(i).id - 10000].GetShipClass(),
					ships.GetAt(i).id-10000, ships.GetAt(i).strenght);
		}
		// zu 75% wird versucht das stärkste Schiff zu bauen
		for (int i = 0; i < ships.GetSize(); i++)
		{
			// entweder 75% oder es ist das schwächste/letzte Schiff im Feld
			if (rand()%4 < 3 || i == ships.GetUpperBound())
			{
				// konnte das Schiff im System gebaut werden, dann sind wir hier fertig
				if (MakeEntryInAssemblyList(ships.GetAt(i).id))
				{
					MYTRACE("ai")(MT::LEVEL_INFO, "CSystemAI::ChooseShip(): choosen combatship to build: %s\n", m_pDoc->m_ShipInfoArray[ships.GetAt(i).id - 10000].GetShipClass());
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
	AssertBotE(id);

	// Schwierigkeitsgrad geht hier mit ein.
	float difficulty = m_pDoc->GetDifficultyLevel();
	// Wenn ein menschlicher Spieler die Autobaufunktion in einem System aktiviert hat, dann bekommt er natürlich keinen
	// Bonus durch den Schwierigkeitsgrad
	if (m_pMajor->IsHumanPlayer() == true && m_pDoc->GetSystem(m_KO.x, m_KO.y).GetAutoBuild() == TRUE)
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
		m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->CalculateNeededRessources(
			0, &m_pDoc->m_ShipInfoArray.GetAt(id-10000) , 0, m_pDoc->GetSystem(ko.x, ko.y).GetAllBuildings(), id,
			m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), difficulty);
	}
	// Sonst die Gebäude
	else
	{
		m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->CalculateNeededRessources(
			&m_pDoc->GetBuildingInfo(RunningNumber), 0, 0, m_pDoc->GetSystem(ko.x, ko.y).GetAllBuildings(), id,
			m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo(), difficulty);
	}
	return m_pDoc->GetSystem(ko.x, ko.y).GetAssemblyList()->MakeEntry(id, ko, m_pDoc->m_Systems);
}


/// Diese Funktion besetzt die Gebäude mit Arbeitern.
void CSystemAI::AssignWorkers()
{
	const CPoint& ko = m_KO;
	CSystem& system = m_pDoc->GetSystem(ko.x, ko.y);

	// Nahrungsversorgung wird zu allererst beachtet. Es wird immer solange besetzt, bis man eine positive Produktion
	// erreicht hat. Hat man zu Beginn schon eine positive Produktion, so wird versucht das Minimum der benötgten
	// Besetzung zu finden.
	while (system.GetProduction()->GetFoodProd() > 5)
	{
		if (system.GetWorker(WORKER::FOOD_WORKER) == 0)
			break;
		system.GetWorker()->DekrementWorker(WORKER::FOOD_WORKER);
		CalcProd();
	}

	while (system.GetProduction()->GetFoodProd() < 0)
	{
		if (system.GetWorker(WORKER::FREE_WORKER) > 0)
		{
			if (system.GetWorker(WORKER::FOOD_WORKER) >= system.GetNumberOfWorkbuildings(WORKER::FOOD_WORKER, 0))
				break;
			system.GetWorker()->InkrementWorker(WORKER::FOOD_WORKER);
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
	for (int i = 0; i < system.GetAllBuildings()->GetSize(); i++)
	{
		const CBuildingInfo *buildingInfo = &m_pDoc->BuildingInfo.GetAt(system.GetAllBuildings()->GetAt(i).GetRunningNumber() - 1);
		if (buildingInfo->GetNeededEnergy() > 0)
		{
			if (CheckMoral(*buildingInfo, false))
			{
				neededEnergy += buildingInfo->GetNeededEnergy();
				if (!system.GetAllBuildings()->GetAt(i).GetIsBuildingOnline())
					if (system.GetProduction()->GetEnergyProd() >= buildingInfo->GetNeededEnergy() + usedEnergy)
					{
						system.SetIsBuildingOnline(i, TRUE);
						usedEnergy += buildingInfo->GetNeededEnergy();
					}
			}
			else
			{
				// gebäude abschalten wenn gegen Moral
				system.SetIsBuildingOnline(i, FALSE);
			}
		}
	}
	while (system.GetProduction()->GetMaxEnergyProd() > (neededEnergy + 15))
	{
		if (system.GetWorker(WORKER::ENERGY_WORKER) == 0)
			break;
		system.GetWorker()->DekrementWorker(WORKER::ENERGY_WORKER);
		CalcProd();
	}
	while (system.GetProduction()->GetMaxEnergyProd() < neededEnergy)
	{
		if (system.GetWorker(WORKER::FREE_WORKER) > 0)
		{
			if (system.GetWorker(WORKER::ENERGY_WORKER) >= system.GetNumberOfWorkbuildings(WORKER::ENERGY_WORKER, 0))
				break;
			system.GetWorker()->InkrementWorker(WORKER::ENERGY_WORKER);
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
		for (int i = WORKER::INDUSTRY_WORKER; i <= WORKER::IRIDIUM_WORKER; i++)
		{
			WORKER::Typ nWorker = (WORKER::Typ)i;
			if (nWorker != WORKER::ENERGY_WORKER)
			{
				allPrio += m_iPriorities[nWorker];
				system.GetWorker()->SetWorker(nWorker, 0);
			}
		}

		int percentage[WORKER::ALL_WORKER] = {0};
		if (allPrio != 0)
		{
			for (int i = WORKER::INDUSTRY_WORKER; i <= WORKER::IRIDIUM_WORKER; i++)
			{
				WORKER::Typ nWorker = (WORKER::Typ)i;
				if (nWorker != WORKER::ENERGY_WORKER)
					percentage[nWorker] = (m_iPriorities[nWorker] * 100) / allPrio;
			}
		}

		// Jetzt die Arbeiter nach ihrer prozentualen Verteilung zuweisung
		for (int i = WORKER::INDUSTRY_WORKER; i <= WORKER::IRIDIUM_WORKER; i++)
		{
			WORKER::Typ nWorker = (WORKER::Typ)i;
			if (nWorker != WORKER::ENERGY_WORKER)
			{
				system.GetWorker()->CalculateFreeWorkers();
				USHORT freeWorkers = system.GetWorker(WORKER::FREE_WORKER);
				int workers = (freeWorkers * percentage[nWorker]) / 100;
				for (int j = 0; j < workers; j++)
				{
					if (system.GetNumberOfWorkbuildings(nWorker,0) > system.GetWorker(nWorker))
					{
						system.GetWorker()->InkrementWorker(nWorker);
						if (system.GetWorker(nWorker) == system.GetNumberOfWorkbuildings(nWorker, 0))
							break;
					}
				}
			}
		}
	}

	// Wenn etwas in der Bauliste steht, dann werden alle Arbeiter entfernt (außer bei Nahrung und Energie). Weiterhin
	// wird bei der Arbeiterbesetzung bei der Industrie begonnen.
	if (system.GetAssemblyList()->GetAssemblyListEntry(0) != 0)
	{
		if (system.GetWorker(WORKER::FREE_WORKER) > 0)
			while (system.GetNumberOfWorkbuildings(WORKER::INDUSTRY_WORKER,0) > system.GetWorker(WORKER::INDUSTRY_WORKER))
			{
				system.GetWorker()->InkrementWorker(WORKER::INDUSTRY_WORKER);
				system.GetWorker()->CalculateFreeWorkers();
				if (system.GetWorker(WORKER::FREE_WORKER) == 0)
					break;
			}
	}
	// Wenn nichts in der Bauliste steht, dann werden die Industriearbeiter erstmal komplett entfernt
	else
	{
		system.GetWorker()->SetWorker(WORKER::INDUSTRY_WORKER, 0);
		system.GetWorker()->CalculateFreeWorkers();
	}

	// Jetzt werden zufällig noch freie Arbeiter auf die restlichen Gebäude verteilt. Es werden aber keine Arbeiter mehr
	// auf Nahrungs-, Industrie- und Energiegebäude verteilt. Das wurde oben schon alle gemacht.
	int numberOfWorkBuildings = 0;
	int workers = 0;
	for (int i = WORKER::SECURITY_WORKER; i <= WORKER::IRIDIUM_WORKER; i++)
	{
		WORKER::Typ nWorker = (WORKER::Typ)i;
		numberOfWorkBuildings += system.GetNumberOfWorkbuildings(nWorker,0);
		workers += system.GetWorker(nWorker);
	}

	while (system.GetWorker(WORKER::FREE_WORKER) > 0 && workers < numberOfWorkBuildings)
	{
		// Zufälligen Arbeiter zwischen Geheimdienst- und Iridiumarbeiter wählen
		WORKER::Typ nWorker = (WORKER::Typ)(rand()%WORKER::ALL_WORKER);
		// Alle Arbeiter werden mit einer zufälligen Anzahl besetzt
		if (system.GetNumberOfWorkbuildings(nWorker,0) > system.GetWorker(nWorker))
		{
			workers++;
			system.GetWorker()->InkrementWorker(nWorker);
			system.GetWorker()->CalculateFreeWorkers();
		}
	}

	// Sind jetzt immernoch ein paar freie Arbeiter übrig, dann wird versucht diese auf die Industriegebäude zu verteilen.
	// Denn dadruch bekommen wir bei Handelswaren mehr Credits.
	while (system.GetWorker(WORKER::FREE_WORKER) > 0)
	{
		if (system.GetNumberOfWorkbuildings(WORKER::INDUSTRY_WORKER,0) > system.GetWorker(WORKER::INDUSTRY_WORKER))
		{
			system.GetWorker()->InkrementWorker(WORKER::INDUSTRY_WORKER);
			system.GetWorker()->CalculateFreeWorkers();
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
	for (int i = WORKER::INDUSTRY_WORKER; i <= WORKER::IRIDIUM_WORKER; i++)
	{
		WORKER::Typ nWorker = (WORKER::Typ)i;
		if (nWorker != WORKER::ENERGY_WORKER)
		{
			if (m_pDoc->GetSystem(m_KO.x, m_KO.y).GetWorker(nWorker) > 0)
			{
				getWorker = TRUE;
				m_pDoc->GetSystem(m_KO.x, m_KO.y).GetWorker()->DekrementWorker(nWorker);
				m_pDoc->GetSystem(m_KO.x, m_KO.y).GetWorker()->InkrementWorker(WORKER::FREE_WORKER);
				break;
			}
		}
	}

	return getWorker;
}

/// Funktion reißt überflüssige Nahrungs- und Energiegebäude im System ab.
void CSystemAI::ScrapBuildings()
{
	CPoint ko = m_KO;
	// Nahrungs- und Energiegebäude, welche zuviel sind reißt die KI ab
	if (m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetFoodProd() >= 0)
	{
		// Nur wenn 80% der maximal Bevölkerung schon im System leben
		float currentHab = 0.0f;
		float maxHab = 0.0f;
		const CSector& sector = m_pDoc->GetSector(ko.x, ko.y);
		for(CSector::const_iterator it = sector.begin(); it != sector.end(); ++it)
		{
			currentHab += it->GetCurrentHabitant();
			maxHab += it->GetMaxHabitant();
		}
		if (currentHab > (maxHab * 0.8))
		{
			int n = m_pDoc->GetSystem(ko.x, ko.y).GetNumberOfWorkbuildings(WORKER::FOOD_WORKER, 0) - m_pDoc->GetSystem(ko.x, ko.y).GetWorker()->GetWorker(WORKER::FOOD_WORKER);
			int id = m_pDoc->GetSystem(ko.x, ko.y).GetNumberOfWorkbuildings(WORKER::FOOD_WORKER, 1);
			while (n > 1)
			{
				m_pDoc->GetSystem(ko.x, ko.y).SetBuildingDestroy(id, TRUE);
				n--;
			}
		}
	}

	if (m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetEnergyProd() > 0)
	{
		int n = m_pDoc->GetSystem(ko.x, ko.y).GetNumberOfWorkbuildings(WORKER::ENERGY_WORKER, 0) - m_pDoc->GetSystem(ko.x, ko.y).GetWorker()->GetWorker(WORKER::ENERGY_WORKER);
		int id = m_pDoc->GetSystem(ko.x, ko.y).GetNumberOfWorkbuildings(WORKER::ENERGY_WORKER, 1);
		while (n > 3)
		{
			m_pDoc->GetSystem(ko.x, ko.y).SetBuildingDestroy(id, TRUE);
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
	CString sRace = m_pDoc->GetSystem(ko.x, ko.y).GetOwnerOfSystem();
	if (sRace.IsEmpty())
		return min;

	CMajor* pMajor = dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(sRace));
	AssertBotE(pMajor);

	if (m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetShipYard() == TRUE
		&& m_pDoc->GetSystem(ko.x, ko.y).GetBuildableShips()->GetSize() > 0)
	{
		if (m_pDoc->m_pAIPrios->GetColoShipPrio(sRace) > 0)
		{
			min = rand()%(m_pDoc->m_pAIPrios->GetColoShipPrio(sRace) + 1);
			MYTRACE("ai")(MT::LEVEL_INFO, "CSystemAI::GetShipBuildPrios(): Race %s - System: %s - ColonyShipPrio: %d (max %d)\n",sRace,m_pDoc->GetSector(ko.x, ko.y).GetName(),min,m_pDoc->m_pAIPrios->GetColoShipPrio(sRace));
			chooseColoship = TRUE;
		}
		if (m_pDoc->m_pAIPrios->GetTransportShipPrio(sRace) > 0)
		{
			int random = rand()%(m_pDoc->m_pAIPrios->GetTransportShipPrio(sRace) + 1);
			MYTRACE("ai")(MT::LEVEL_INFO, "CSystemAI::GetShipBuildPrios(): Race %s - System: %s - TransportShipPrio: %d (max %d)\n",sRace, m_pDoc->GetSector(ko.x, ko.y).GetName(),random,m_pDoc->m_pAIPrios->GetTransportShipPrio(sRace));
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
			MYTRACE("ai")(MT::LEVEL_INFO, "CSystemAI::GetShipBuildPrios(): Race %s - System: %s - CombatShipPrio: %d (max %d)\n",sRace,m_pDoc->GetSector(ko.x, ko.y).GetName(),random,m_pDoc->m_pAIPrios->GetCombatShipPrio(sRace));
			if (random > min)
			{
				// Schiffsbevölkerungsunterstützungskosten - Schiffsunterstützungskosten
				int shipCosts = pMajor->GetEmpire()->GetPopSupportCosts() - pMajor->GetEmpire()->GetShipCosts();
				// würde man durch die Schiffe negatives Credits machen und dies wäre höher als 5% des gesamten Creditsbestandes, dann wird kein Schiff gebaut!
				if (shipCosts < 0 && abs(shipCosts) > (long)(pMajor->GetEmpire()->GetCredits() * 0.05))
				{
					chooseCombatship = FALSE;
					MYTRACE("ai")(MT::LEVEL_INFO, "CSystemAI::GetShipBuildPrios(): Race %s - System: %s - can't build ships because of too high shipcosts!\n",sRace, m_pDoc->GetSector(ko.x, ko.y).GetName());
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
	MYTRACE("ai")(MT::LEVEL_INFO, "CSystemAI::GetShipBuildPrios(): ... ready\n");

	return min;
}

/// Diese Funktion berechnet die neue Nahrungs-, Industrie- und Energieproduktion im System. Sie sollte aufgerufen
/// werden, wenn Arbeiter aus diesen Gebieten verändert wurden. Dabei werden aber alle anderen Produktionen
/// gelöscht.
void CSystemAI::CalcProd()
{
	CSystem* pSystem = &m_pDoc->GetSystem(m_KO.x, m_KO.y);

	CSystemProd* pProduction = pSystem->GetProduction();
	if (!pProduction)
	{
		AssertBotE(pProduction);
		return;
	}

	// Alle Werte wieder auf NULL setzen
	pProduction->Reset();

	pSystem->GetWorker()->CheckWorkers();
	unsigned short foodWorker	  = pSystem->GetWorker()->GetWorker(WORKER::FOOD_WORKER);
	unsigned short industryWorker = pSystem->GetWorker()->GetWorker(WORKER::INDUSTRY_WORKER);
	unsigned short energyWorker   = pSystem->GetWorker()->GetWorker(WORKER::ENERGY_WORKER);

	// Die einzelnen Produktionen berechnen
	int nNumberOfBuildings = pSystem->GetAllBuildings()->GetSize();
	for (int i = 0; i < nNumberOfBuildings; i++)
	{
		const CBuildingInfo *buildingInfo = &m_pDoc->BuildingInfo.GetAt(pSystem->GetAllBuildings()->GetAt(i).GetRunningNumber() - 1);

		// Gebäude offline setzen
		if (buildingInfo->GetWorker() == TRUE)
		{
			pSystem->GetAllBuildings()->ElementAt(i).SetIsBuildingOnline(FALSE);
			// Jetzt wieder wenn möglich online setzen
			if (buildingInfo->GetFoodProd() > 0 && foodWorker > 0)
			{
				pSystem->GetAllBuildings()->ElementAt(i).SetIsBuildingOnline(TRUE);
				foodWorker--;
			}
			else if (buildingInfo->GetIPProd() > 0 && industryWorker > 0)
			{
				pSystem->GetAllBuildings()->ElementAt(i).SetIsBuildingOnline(TRUE);
				industryWorker--;
			}
			else if (buildingInfo->GetEnergyProd() > 0 && energyWorker > 0)
			{
				pSystem->GetAllBuildings()->ElementAt(i).SetIsBuildingOnline(TRUE);
				energyWorker--;
			}
		}
		// Die einzelnen Produktionen berechnen (ohne Boni)
		// vorher noch schauen, ob diese Gebäude auch online sind
		pProduction->CalculateProduction(buildingInfo, pSystem->GetAllBuildings()->GetAt(i).GetIsBuildingOnline() == TRUE);
	}

	// falls vorhanden, deaktiverte Produktionen auf 0 setzen
	pProduction->DisableProductions(pSystem->GetDisabledProductions());

	// Die Boni auf die einzelnen Produktionen berechnen
	short tmpFoodBoni		= m_pMajor->GetEmpire()->GetResearch()->GetBioTech() * TECHPRODBONUS;
	short tmpIndustryBoni	= m_pMajor->GetEmpire()->GetResearch()->GetConstructionTech() * TECHPRODBONUS;
	short tmpEnergyBoni		= m_pMajor->GetEmpire()->GetResearch()->GetEnergyTech() * TECHPRODBONUS;

	short neededEnergy = 0;
	for (int i = 0; i < nNumberOfBuildings; i++)
	{
		const CBuildingInfo *buildingInfo = &m_pDoc->BuildingInfo.GetAt(pSystem->GetAllBuildings()->GetAt(i).GetRunningNumber() - 1);

		// Hier die nötige Energie von der produzierten abziehen, geht aber nur hier, wenn wir keine Boni zur Energie reinmachen
		if (pSystem->GetAllBuildings()->GetAt(i).GetIsBuildingOnline() == TRUE && buildingInfo->GetNeededEnergy() > 0)
			neededEnergy += buildingInfo->GetNeededEnergy();

		if (pSystem->GetAllBuildings()->GetAt(i).GetIsBuildingOnline() == TRUE)
		{
			// Es wird IMMER abgerundet, gemacht durch "floor"
			tmpFoodBoni			+= buildingInfo->GetFoodBoni();
			tmpIndustryBoni		+= buildingInfo->GetIndustryBoni();
			tmpEnergyBoni		+= buildingInfo->GetEnergyBoni();
		}
	}
	const CSector& sector = m_pDoc->GetSector(m_KO.x, m_KO.y);
	// Jetzt werden noch eventuelle Boni durch die Planetenklassen dazugerechnet
	for(CSector::const_iterator it = sector.begin(); it != sector.end(); ++it)
	{
		if (it->GetColonized() == TRUE && it->GetCurrentHabitant() > 0.0f)
		{
			if (it->GetBoni()[6] == TRUE)	// food
				tmpFoodBoni		+= (it->GetSize()+1) * 25;
			if (it->GetBoni()[7] == TRUE)	// energy
				tmpEnergyBoni	+= (it->GetSize()+1) * 25;
		}
	}

	pProduction->m_iFoodProd		+= (int)(tmpFoodBoni * pProduction->m_iFoodProd / 100);
	pProduction->m_iIndustryProd	+= (int)(tmpIndustryBoni * pProduction->m_iIndustryProd / 100);
	pProduction->m_iEnergyProd		+= (int)(tmpEnergyBoni * pProduction->m_iEnergyProd / 100);

	// Wenn das System blockiert wird, dann verringern sich bestimmte Produktionswerte
	if (pSystem->GetBlockade() > 0)
	{
		//m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->m_iFoodProd		-= (int)(m_pDoc->GetSystem(ko.x, ko.y).GetBlockade() * m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->m_iFoodProd/100);
		pProduction->m_iIndustryProd	-= (int)(pSystem->GetBlockade() * pProduction->m_iIndustryProd/100);
		//m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->m_iEnergyProd		-= (int)(m_pDoc->GetSystem(ko.x, ko.y).GetBlockade() * m_pDoc->GetSystem(ko.x, ko.y).GetProduction()->m_iEnergyProd/100);
	}

	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Hier die Boni durch die Uniqueforschung "Wirtschaft" -> 10% mehr Industrie
	if (m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		pProduction->m_iIndustryProd += (int)(m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetBonus(1)*pProduction->m_iIndustryProd/100);
	// Hier die Boni durch die Uniqueforschung "Produktion"
	if (m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::PRODUCTION)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		pProduction->m_iFoodProd += (int)(m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::PRODUCTION)->GetBonus(1)*pProduction->m_iFoodProd/100);
	// Wenn wir die Uniqueforschung "Produktion" gewählt haben, und dort mehr Energie haben wollen -> 20% mehr!
	else if (m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::PRODUCTION)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
		pProduction->m_iEnergyProd += (int)(m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::PRODUCTION)->GetBonus(3)*pProduction->m_iEnergyProd/100);

	// Maximalenergie, also hier noch ohne Abzüge durch energiebedürftige Gebäude
	pProduction->m_iMaxEnergyProd = pProduction->m_iEnergyProd;
	// hier die gesamte Energie durch energiebedürftige Gebäude abziehen
	pProduction->m_iEnergyProd -= neededEnergy;

	// imperiumweite Moralprod mit aufrechnen
	pProduction->AddMoralProd(pProduction->GetMoralProdEmpireWide(m_pMajor->GetRaceID()));
	// Den Moralboni im System noch auf die einzelnen Produktionen anrechnen
	pProduction->IncludeSystemMoral(pSystem->GetMoral());
	// benötigte Nahrung durch Bevölkerung von der Produktion abiehen
	if (!m_pMajor->HasSpecialAbility(SPECIAL_NEED_NO_FOOD))
		// ceil, wird auf Kommezahl berechnet, z.B brauchen wir für 14.5 Mrd. Leute 145 Nahrung und nicht 140 bzw. 150
		pProduction->m_iFoodProd -= (int)ceil(pSystem->GetHabitants()*10);
	else
		pProduction->m_iFoodProd = pProduction->m_iMaxFoodProd;

	// Jetzt noch die freien Arbeiter berechnen
	pSystem->GetWorker()->CalculateFreeWorkers();
}

/// Diese Funktion legt möglicherweise eine Handelsroute zu einem anderen System an. Dadurch wird halt auch die
/// Beziehung zu Minorraces verbessert.
void CSystemAI::ApplyTradeRoutes()
{
	CPoint ko = m_KO;
	CString race = m_pDoc->GetSystem(ko.x, ko.y).GetOwnerOfSystem();
	if (m_pDoc->GetSystem(ko.x, ko.y).CanAddTradeRoute(m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()) == TRUE)
	{
		// primär zu Minorraces
		map<CString, CMinor*>* pmMinors = m_pDoc->GetRaceCtrl()->GetMinors();
		for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); ++it)
		{
			CMinor* pMinor = it->second;
			// nicht zu Aliens bzw. Minors ohne Heimatsystem
			if (pMinor->IsAlienRace() || pMinor->GetRaceKO() == CPoint(-1, -1))
				continue;

			if (pMinor->GetAgreement(race) >= DIPLOMATIC_AGREEMENT::TRADE && pMinor->GetAgreement(race) < DIPLOMATIC_AGREEMENT::MEMBERSHIP)
				if (m_pDoc->GetSystem(ko.x, ko.y).AddTradeRoute(pMinor->GetRaceKO(), m_pDoc->m_Systems, m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()) == FALSE)
					break;
		}
	}
	if (m_pDoc->GetSystem(ko.x, ko.y).CanAddTradeRoute(m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo()) == TRUE)
	{
		// sekundär zu den anderen Majorraces
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
				if (m_pDoc->GetSystem(x, y).GetOwnerOfSystem() != "" && m_pDoc->GetSystem(x, y).GetOwnerOfSystem() != race)
					if (m_pMajor->GetAgreement(m_pDoc->GetSystem(x, y).GetOwnerOfSystem()) >= DIPLOMATIC_AGREEMENT::TRADE)
						m_pDoc->GetSystem(ko.x, ko.y).AddTradeRoute(CPoint(x,y), m_pDoc->m_Systems, m_pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
	}
}

/// Funktion überprüft, ob für einen bestimmten Arbeitertyp auch ein Gebäude in der Liste der baubaren
/// Gebäude exisitiert. Wenn dies nicht der Fall ist, so ist auch die Priorität solch ein Gebäude zu bauen
/// gleich Null.
bool CSystemAI::CheckBuilding(WORKER::Typ nWorker) const
{
	for (int i = 0; i < m_pDoc->GetSystem(m_KO.x, m_KO.y).GetBuildableBuildings()->GetSize(); i++)
	{
		int id = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetBuildableBuildings()->GetAt(i);
		const CBuildingInfo* building = &m_pDoc->GetBuildingInfo(id);

		if (nWorker == WORKER::FOOD_WORKER && building->GetFoodProd() > 0)
			return true;
		if (nWorker == WORKER::INDUSTRY_WORKER && building->GetIPProd() > 0)
			return true;
		if (nWorker == WORKER::ENERGY_WORKER && building->GetEnergyProd() > 0)
			return true;
		if (nWorker == WORKER::SECURITY_WORKER && building->GetSPProd() > 0)
			return true;
		if (nWorker == WORKER::RESEARCH_WORKER && building->GetFPProd() > 0)
			return true;
		if (nWorker == WORKER::TITAN_WORKER && building->GetTitanProd() > 0)
			return true;
		if (nWorker == WORKER::DEUTERIUM_WORKER && building->GetDeuteriumProd() > 0)
			return true;
		if (nWorker == WORKER::DURANIUM_WORKER && building->GetDuraniumProd() > 0)
			return true;
		if (nWorker == WORKER::CRYSTAL_WORKER && building->GetCrystalProd() > 0)
			return true;
		if (nWorker == WORKER::IRIDIUM_WORKER && building->GetIridiumProd() > 0)
			return true;
	}
	return false;
}

/// Funktion berechnet die Priorität der Nahrungsmittelproduktion im System
/// @param dMaxHab maximale Bevölkerungszahl im System
/// @return Priotität der Nahrungsmittelproduktion
int CSystemAI::GetFoodPrio(double dMaxHab) const
{
	// wenn die Maximale Anzahl an Einwohnern 1.25 mal größer als die aktuelle Anzahl der Einwoher ist, dann
	// werden Gebäudebauprioritäten verdoppelt
	double dCurHab = max(1.0, m_pDoc->GetSystem(m_KO.x, m_KO.y).GetHabitants());
	// Restliche Arbeiter berechnen (diesen Wert aber durch 1.5 teilen, damit er nicht so stark eingeht)
	int nRestWorkers = floor(dMaxHab - dCurHab) / 1.5;

	if (m_pDoc->GetSystem(m_KO.x, m_KO.y).GetWorker(WORKER::ALL_WORKER) + nRestWorkers < m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(WORKER::FOOD_WORKER, 0))
		return 0;

	/// existieren Gebäude welche die jeweilige Priorität ermöglichen
	if (!CheckBuilding(WORKER::FOOD_WORKER))
		return 0;

	// Hier wird die aktuelle Nahrungsmittelproduktion mit dem Inhalt des Lagers verglichen.
	// Haben wir eine positive Nahrungsproduktion, so brauchen wir keine neuen Nahrungsgebäude
	// und können daher eine geringe Priorität ansetzen.
	int nFoodProd = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetProduction()->GetFoodProd();
	if (nFoodProd >= 0)
		return 0;

	int nPrio = 0;
	// Hier müssen wir die negative Nahrungsprduktion gegen den aktuellen Lagerinhalt rechnen
	nFoodProd *= (-5);
	long div = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetFoodStore() + 1;
	if (div > 0)
		nPrio = nFoodProd * 100 / div;
	else
		nPrio = nFoodProd * 100;
	nPrio *= 4;

	return min(nPrio, 255);
}

/// Funktion berechnet die Priorität der Industrieproduktion im System
/// @param dMaxHab maximale Bevölkerungszahl im System
/// @return Priotität der Industrieproduktion
int CSystemAI::GetIndustryPrio(double dMaxHab) const
{
	// wenn die Maximale Anzahl an Einwohnern 1.25 mal größer als die aktuelle Anzahl der Einwoher ist, dann
	// werden Gebäudebauprioritäten verdoppelt
	double dCurHab = max(1.0, m_pDoc->GetSystem(m_KO.x, m_KO.y).GetHabitants());
	// Restliche Arbeiter berechnen (diesen Wert aber durch 1.5 teilen, damit er nicht so stark eingeht)
	int nRestWorkers = floor(dMaxHab - dCurHab) / 1.5;

	if (m_pDoc->GetSystem(m_KO.x, m_KO.y).GetWorker(WORKER::ALL_WORKER) + nRestWorkers < m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(WORKER::INDUSTRY_WORKER, 0) + m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(WORKER::FOOD_WORKER, 0))
		return 0;

	// existieren Gebäude welche die jeweilige Priorität ermöglichen
	if (!CheckBuilding(WORKER::INDUSTRY_WORKER))
		return 0;

	// Hier wird die durchschnittlich zu erbringende Industrieleistung aller Bauaufträge (außer Upgrade,
	// Schiffe und Truppen) ins Verhältnis mit der maximal möglichen Industrieleistung gesetzt.
	// Hier kompliziert berechnet, aber ich möchte auch alle möglichen Boni mit eingerechnet haben.
	int nIPProd = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetProduction()->GetIndustryProd();

	// Arbeiter temporär auf Maximum stellen
	USHORT nWorkers  = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetWorker(WORKER::INDUSTRY_WORKER);
	USHORT nNumber	 = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(WORKER::INDUSTRY_WORKER, 0);
	if (nWorkers == 0)
	{
		short nID = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(WORKER::INDUSTRY_WORKER, 1);
		if (nID > 0)
			nIPProd = m_pDoc->GetBuildingInfo(nID).GetIPProd() * nNumber;
	}
	else
	{
		nIPProd = nIPProd * nNumber / nWorkers;
	}

	int nMidIPCosts = 0;
	int nSize = 0;
	for (int i = 0; i < m_pDoc->GetSystem(m_KO.x, m_KO.y).GetBuildableBuildings()->GetSize(); i++)
	{
		short nID = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetBuildableBuildings()->GetAt(i);
		if (!m_pDoc->GetBuildingInfo(nID).GetNeverReady())
		{
			nMidIPCosts += m_pDoc->GetBuildingInfo(nID).GetNeededIndustry();
			nSize++;
		}
	}

	for (int i = 0; i < m_pDoc->GetSystem(m_KO.x, m_KO.y).GetBuildableShips()->GetSize(); i++)
	{
		short nID = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetBuildableShips()->GetAt(i);
		nMidIPCosts += m_pDoc->m_ShipInfoArray.GetAt(nID-10000).GetNeededIndustry();
		nSize++;
	}

	if (nSize > 0)
		nMidIPCosts /= nSize;

	int nPrio = 0;
	if (nIPProd > 0)
		nPrio = nMidIPCosts / nIPProd;
	else
		nPrio = nMidIPCosts;

	// wenn noch Bevölkerung ins System passen würde, so werden bevorzugt mehr Gebäude gebaut
	double dHabMod = max(1.0, dMaxHab / dCurHab);
	nPrio = nPrio * dHabMod + nRestWorkers;

	return min(nPrio, 255);
}

/// Funktion berechnet die Priorität der Industrieproduktion im System
/// @param dMaxHab maximale Bevölkerungszahl im System
/// @return Priotität der Industrieproduktion
int CSystemAI::GetEnergyPrio(double dMaxHab) const
{
	// wenn die Maximale Anzahl an Einwohnern 1.25 mal größer als die aktuelle Anzahl der Einwoher ist, dann
	// werden Gebäudebauprioritäten verdoppelt
	double dCurHab = max(1.0, m_pDoc->GetSystem(m_KO.x, m_KO.y).GetHabitants());
	// Restliche Arbeiter berechnen (diesen Wert aber durch 1.5 teilen, damit er nicht so stark eingeht)
	int nRestWorkers = floor(dMaxHab - dCurHab) / 1.5;

	if (m_pDoc->GetSystem(m_KO.x, m_KO.y).GetWorker(WORKER::ALL_WORKER) + nRestWorkers < m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(WORKER::ENERGY_WORKER, 0) + m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(WORKER::FOOD_WORKER, 0))
		return 0;

	// existieren Gebäude welche die jeweilige Priorität ermöglichen
	if (!CheckBuilding(WORKER::ENERGY_WORKER))
		return 0;

	// Hier wird die benötigte Energie im Verhältnis zu maximal möglichen Energie betrachtet
	int nEnergyProd = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetProduction()->GetMaxEnergyProd();
	// Arbeiter temporär auf maximum stellen
	USHORT nWorkers = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetWorker(WORKER::ENERGY_WORKER);
	USHORT nNumber	= m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(WORKER::ENERGY_WORKER, 0);
	if (nWorkers == 0)
	{
		short nID = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(WORKER::ENERGY_WORKER, 1);
		if (nID > NULL)
			nEnergyProd = m_pDoc->GetBuildingInfo(nID).GetEnergyProd() * nNumber;
	}
	else
	{
		nEnergyProd = nEnergyProd * nNumber / nWorkers;
	}

	int nNeededEnergy = 0;
	for (int i = 0; i < m_pDoc->GetSystem(m_KO.x, m_KO.y).GetAllBuildings()->GetSize(); i++)
	{
		const CBuildingInfo *buildingInfo = &m_pDoc->BuildingInfo.GetAt(m_pDoc->GetSystem(m_KO.x, m_KO.y).GetAllBuildings()->GetAt(i).GetRunningNumber() - 1);
		nNeededEnergy += buildingInfo->GetNeededEnergy();
	}

	if (nEnergyProd >= nNeededEnergy)
		return 0;

	int nPrio = 0;
	if (nEnergyProd > 0)
		nPrio = 10 * nNeededEnergy / nEnergyProd;
	else
		nPrio = nNeededEnergy;

	return min(nPrio, 255);
}

/// Funktion berechnet die Priorität der Geheimdienstproduktion im System
/// @param dMaxHab maximale Bevölkerungszahl im System
/// @return Priotität der Geheimdienstproduktion
int CSystemAI::GetIntelPrio(double dMaxHab) const
{
	// wenn die Maximale Anzahl an Einwohnern 1.25 mal größer als die aktuelle Anzahl der Einwoher ist, dann
	// werden Gebäudebauprioritäten verdoppelt
	double dCurHab = max(1.0, m_pDoc->GetSystem(m_KO.x, m_KO.y).GetHabitants());
	// Restliche Arbeiter berechnen (diesen Wert aber durch 1.5 teilen, damit er nicht so stark eingeht)
	int nRestWorkers = floor(dMaxHab - dCurHab) / 1.5;

	if (m_pDoc->GetSystem(m_KO.x, m_KO.y).GetWorker(WORKER::ALL_WORKER) + nRestWorkers < m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(WORKER::SECURITY_WORKER, 0) + m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(WORKER::FOOD_WORKER, 0))
		return 0;

	// existieren Gebäude welche die jeweilige Priorität ermöglichen
	if (!CheckBuilding(WORKER::SECURITY_WORKER))
		return 0;

	CString sRace = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetOwnerOfSystem();
	int nPrio = m_pDoc->m_pAIPrios->GetIntelAI()->GetIntelPrio(sRace);

	return min(nPrio, 255);
}

/// Funktion berechnet die Priorität der Forschungsproduktion im System
/// @param dMaxHab maximale Bevölkerungszahl im System
/// @return Priotität der Forschungsproduktion
int CSystemAI::GetResearchPrio(double dMaxHab) const
{
	// wenn die Maximale Anzahl an Einwohnern 1.25 mal größer als die aktuelle Anzahl der Einwoher ist, dann
	// werden Gebäudebauprioritäten verdoppelt
	double dCurHab = max(1.0, m_pDoc->GetSystem(m_KO.x, m_KO.y).GetHabitants());
	// Restliche Arbeiter berechnen (diesen Wert aber durch 1.5 teilen, damit er nicht so stark eingeht)
	int nRestWorkers = floor(dMaxHab - dCurHab) / 1.5;

	if (m_pDoc->GetSystem(m_KO.x, m_KO.y).GetWorker(WORKER::ALL_WORKER) + nRestWorkers < m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(WORKER::RESEARCH_WORKER, 0) + m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(WORKER::FOOD_WORKER, 0))
		return 0;

	// existieren Gebäude welche die jeweilige Priorität ermöglichen
	if (!CheckBuilding(WORKER::RESEARCH_WORKER))
		return 0;

	int nPrio = rand()%3;
	// wenn noch Bevölkerung ins System passen würde, so werden bevorzugt mehr Gebäude gebaut
	double dHabMod = max(1.0, dMaxHab / dCurHab);
	nPrio = nPrio * dHabMod + nRestWorkers;

	return min(nPrio, 255);
}

/// Funktion berechnet die Priorität einer bestimmten Ressourcenproduktion im System
/// @param nWorker Arbeiter für eine bestimmte Ressource
/// @param dMaxHab maximale Bevölkerungszahl im System
/// @return Priotität der Forschungsproduktion
int CSystemAI::GetResourcePrio(WORKER::Typ nWorker, double dMaxHab) const
{
	// vorhandene Ressourcen durch die Planeten holen. Wenn eine Ressource nicht vorhanden ist, wird die entsprechende
	// Priorität auf NULL gesetzt. Denn dafür haben wir dann auch keine Gebäude in der Bauliste.
	BOOLEAN bResExist[RESOURCES::DERITIUM + 1] = {0};
	m_pDoc->GetSector(m_KO.x, m_KO.y).GetAvailableResources(bResExist, true);

	int nRes = nWorker - 5;
	AssertBotE(nRes >= RESOURCES::TITAN && nRes <= RESOURCES::DERITIUM);
	if (!bResExist[nRes])
	{
		// nicht im System vorhanden
		return 0;
	}

	// wenn die Maximale Anzahl an Einwohnern 1.25 mal größer als die aktuelle Anzahl der Einwoher ist, dann
	// werden Gebäudebauprioritäten verdoppelt
	double dCurHab = max(1.0, m_pDoc->GetSystem(m_KO.x, m_KO.y).GetHabitants());
	// Restliche Arbeiter berechnen (diesen Wert aber durch 1.5 teilen, damit er nicht so stark eingeht)
	int nRestWorkers = floor(dMaxHab - dCurHab) / 1.5;

	if (m_pDoc->GetSystem(m_KO.x, m_KO.y).GetWorker(WORKER::ALL_WORKER) + nRestWorkers < m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(nWorker, 0) + m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(WORKER::FOOD_WORKER, 0))
		return 0;

	// existieren Gebäude welche die jeweilige Priorität ermöglichen
	if (!CheckBuilding(nWorker))
		return 0;

	int nResProd = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetProduction()->GetResourceProd(nRes);
	// Arbeiter temporär auf maximum stellen
	USHORT nWorkers	= m_pDoc->GetSystem(m_KO.x, m_KO.y).GetWorker(nWorker);
	USHORT nNumber	= m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(nWorker, 0);
	if (nWorkers == 0)
	{
		short nID = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfWorkbuildings(nWorker, 1);
		if (nID > 0)
			nResProd = m_pDoc->GetBuildingInfo(nID).GetResourceProd(nRes) * nNumber;
	}
	else
	{
		nResProd = nResProd * nNumber / nWorkers;
	}

	// durchschnittliche Ressourcenkosten
	int nMidResCosts = 0;
	int nSize = 0;

	// Benötigte Ressourcen der Gebäude
	for (int j = 0; j < m_pDoc->GetSystem(m_KO.x, m_KO.y).GetBuildableBuildings()->GetSize(); j++)
	{
		short nID = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetBuildableBuildings()->GetAt(j);
		if (!m_pDoc->GetBuildingInfo(nID).GetNeverReady())
			nMidResCosts += m_pDoc->GetBuildingInfo(nID).GetNeededResource(nRes);
	}

	// benötigte Ressourcen durch die Updates
	for (int j = 0; j < m_pDoc->GetSystem(m_KO.x, m_KO.y).GetBuildableUpdates()->GetSize(); j++)
	{
		short nID = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetBuildableUpdates()->GetAt(j);
		USHORT nPreNumber = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetNumberOfBuilding(m_pDoc->GetBuildingInfo(nID).GetPredecessorID());
		nMidResCosts += (m_pDoc->GetBuildingInfo(nID).GetNeededResource(nRes) * nPreNumber);
		nSize++;
	}

	// benötigte Ressourcen durch Schiffe (nur wenn das Schiff diese Ressource auch benötigt)
	for (int j = 0; j < m_pDoc->GetSystem(m_KO.x, m_KO.y).GetBuildableShips()->GetSize(); j++)
	{
		short nID = m_pDoc->GetSystem(m_KO.x, m_KO.y).GetBuildableShips()->GetAt(j);
		if (m_pDoc->m_ShipInfoArray.GetAt(nID - 10000).GetNeededResource(nRes) > 0)
		{
			nMidResCosts += m_pDoc->m_ShipInfoArray.GetAt(nID - 10000).GetNeededResource(nRes);
			nSize++;
		}
	}

	nSize /= 3.0;
	if (nSize > 0)
		nMidResCosts /= nSize;

	int nPrio = 0;
	if (nResProd > 0)
		nPrio = (int)((100 * nMidResCosts / (m_pDoc->GetSystem(m_KO.x, m_KO.y).GetResourceStore(nRes)+1) / nResProd));
	else
		nPrio = (int)((100 * nMidResCosts / ((m_pDoc->GetSystem(m_KO.x, m_KO.y).GetResourceStore(nRes)+1) * (nNumber + 1))));

	// wenn noch Bevölkerung ins System passen würde, so werden bevorzugt mehr Gebäude gebaut
	double dHabMod = max(1.0, dMaxHab / dCurHab);
	nPrio = nPrio * dHabMod + nRestWorkers;

	return min(nPrio, 255);
}
