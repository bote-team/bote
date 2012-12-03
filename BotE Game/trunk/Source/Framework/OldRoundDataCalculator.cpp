#include "stdafx.h"
#include "OldRoundDataCalculator.h"

#include "Races/RaceController.h"
#include "General/ResourceManager.h"
#include <cassert>


COldRoundDataCalculator::COldRoundDataCalculator(void)
{
}

COldRoundDataCalculator::~COldRoundDataCalculator(void)
{
}

COldRoundDataCalculator::COldRoundDataCalculator(CBotf2Doc* pDoc)
{
	m_pDoc = pDoc;
}

void COldRoundDataCalculator::CreditsDestructionMoral(CMajor* pMajor, CSystem& system,
		const CArray<CBuildingInfo,CBuildingInfo>& BuildingInfo, float fDifficultyLevel) {
	// spielt es der Computer, so bekommt er etwas mehr Credits
	CSystemProd* prod = system.GetProduction();
	CEmpire* pEmpire = pMajor->GetEmpire();
	if (!pMajor->AHumanPlays())
		pEmpire->SetCredits(static_cast<int>(prod->GetCreditsProd() / fDifficultyLevel));
	else
		pEmpire->SetCredits(prod->GetCreditsProd());

	// Hier die Geb�ude abrei�en, die angeklickt wurden
	if (system.DestroyBuildings())
		system.CalculateNumberOfWorkbuildings(&BuildingInfo);

	// Variablen berechnen lassen, bevor der Planet w�chst -> diese ins Lager
	// nur berechnen, wenn das System auch jemandem geh�rt, ansonsten w�rde auch die Mind.Prod. ins Lager kommen
	// In CalculateStorages wird auch die Systemmoral berechnet. Wenn wir einen Auftrag mit
	// NeverReady (z.B. Kriegsrecht) in der Bauliste haben, und dieser Moral produziert, dann diese
	// auf die Moral anrechnen. Das wird dann in CalculateStorages gemacht.
	int list = system.GetAssemblyList()->GetAssemblyListEntry(0);
	if (list > 0 && list < 10000 && BuildingInfo[list-1].GetNeverReady() && system.GetMoral() <= 85)
		prod->AddMoralProd(BuildingInfo[list-1].GetMoralProd());
}

int COldRoundDataCalculator::DeritiumForTheAI(bool human, const CSector& /*sector*/, const CSystem& system, float fDifficultyLevel) {
	// KI Anpassungen (KI bekommt zuf�lig etwas Deritium geschenkt)
	int diliAdd = 0;
	if (!human && system.GetProduction()->GetDeritiumProd() == 0)
	{
		// umso h�her der Schwierigkeitsgrad, desto h�her die Wahrscheinlichkeit, das die KI
		// Deritium auf ihrem Systemen geschenkt bekommt
		int temp = rand()%((int)(fDifficultyLevel * 7.5));
		//TRACE("KI: System: %s - DiliAddProb: %d (NULL for adding Dili) - Difficulty: %.2lf\n",sector.GetName(), temp, fDifficultyLevel);
		if (temp == 0)
			diliAdd = 1;
	}
	return diliAdd;
}

void COldRoundDataCalculator::ExecuteRebellion(CSector& sector, CSystem& system, CMajor* pMajor) const {
	CEmpire* pEmpire = pMajor->GetEmpire();
	const CPoint& co = sector.GetKO();
	const CString& sectorname = sector.GetName();
	const CRaceController* pRaceCtrl = m_pDoc->GetRaceCtrl();
	sector.SetOwned(FALSE);
	sector.SetShipPort(FALSE, pMajor->GetRaceID());

	CString news = CResourceManager::GetString("REBELLION_IN_SYSTEM", FALSE, sectorname);
	CMessage message;
	message.GenerateMessage(news, MESSAGE_TYPE::SOMETHING, "", co, FALSE);
	pEmpire->AddMessage(message);

	// zus�tzliche Eventnachricht (Lose a System to Rebellion #18) wegen der Moral an das Imperium
	message.GenerateMessage(pMajor->GetMoralObserver()->AddEvent(18, pMajor->GetRaceMoralNumber(), sectorname), MESSAGE_TYPE::SOMETHING, "", co, FALSE);
	pEmpire->AddMessage(message);

	if (pMajor->IsHumanPlayer()) {
		const network::RACE client = pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
		m_pDoc->m_iSelectedView[client] = EMPIRE_VIEW;
	}

	if (sector.GetMinorRace()) {
		CMinor* pMinor = pRaceCtrl->GetMinorRace(sectorname);
		assert(pMinor);
		sector.SetOwnerOfSector(pMinor->GetRaceID());

		if (sector.GetTakenSector() == FALSE) {
			pMinor->SetAgreement(pMajor->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);
			pMajor->SetAgreement(pMinor->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);

			pMinor->SetRelation(pMajor->GetRaceID(), (-(rand()%50+20)));
			news = CResourceManager::GetString("MINOR_CANCELS_MEMBERSHIP", FALSE, pMinor->GetRaceName());
			CMessage message;
			message.GenerateMessage(news, MESSAGE_TYPE::DIPLOMACY, "", co, FALSE);
			pEmpire->AddMessage(message);
		}
	} else {
		sector.SetOwnerOfSector("");
		system.SetOwnerOfSystem("");
		sector.SetTakenSector(FALSE);
	}
}

void COldRoundDataCalculator::ExecuteFamine(CSector& sector, CSystem& system, CMajor* pMajor) const {
	const CPoint& co = sector.GetKO();
	CEmpire* pEmpire = pMajor->GetEmpire();
	sector.LetPlanetsShrink((float)(system.GetFoodStore()) * 0.01f);
	// nur wenn die Moral �ber 50 ist sinkt die Moral durch Hungersn�te
	if (system.GetMoral() > 50)
		system.SetMoral((short)(system.GetFoodStore() / (system.GetHabitants() + 1))); // +1, wegen Division durch NULL umgehen
	system.SetFoodStore(0);

	CString news = CResourceManager::GetString("FAMINE", FALSE, sector.GetName());
	CMessage message;
	message.GenerateMessage(news, MESSAGE_TYPE::SOMETHING, "", co, FALSE, 1);
	pEmpire->AddMessage(message);
	if (pMajor->IsHumanPlayer()) {
		const network::RACE client = m_pDoc->m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
		m_pDoc->m_iSelectedView[client] = EMPIRE_VIEW;
	}
}

void COldRoundDataCalculator::SystemMessage(const CSector& sector, CMajor* pMajor, const CString& key,
		MESSAGE_TYPE::Typ message_typ, BYTE byFlag) const {
	const CString& news = CResourceManager::GetString(key, FALSE, sector.GetName());
	CMessage message;
	message.GenerateMessage(news, message_typ, "", sector.GetKO(), FALSE, byFlag);
	pMajor->GetEmpire()->AddMessage(message);
	if (pMajor->IsHumanPlayer()) {
		const network::RACE client = m_pDoc->m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
		m_pDoc->m_iSelectedView[client] = EMPIRE_VIEW;
	}
}

void COldRoundDataCalculator::HandlePopulationEffects(const CSector& sector, CSystem& system, CMajor* pMajor) const {
	const float fCurrentHabitants = sector.GetCurrentHabitants();
	CEmpire* pEmpire = pMajor->GetEmpire();
	pEmpire->AddPopSupportCosts((USHORT)fCurrentHabitants * POPSUPPORT_MULTI);
	// Funktion gibt TRUE zur�ck, wenn wir durch die Bev�lkerung eine neue Handelsroute anlegen k�nnen
	if (system.SetHabitants(fCurrentHabitants))
	{
		// wenn die Spezialforschung "mindestens 1 Handelsroute erforscht wurde, dann die Meldung erst bei
		// der 2.ten Handelroute bringen
		// Hier die Boni durch die Uniqueforschung "Handel" -> mindestens eine Handelsroute
		bool bMinOneRoute = pEmpire->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED;
		if (bMinOneRoute == false || (bMinOneRoute == true && (int)(system.GetHabitants() / TRADEROUTEHAB) > 1))
			SystemMessage(sector, pMajor, "ENOUGH_HABITANTS_FOR_NEW_TRADEROUTE", MESSAGE_TYPE::ECONOMY, 4);
	}
}

static void MilitaryMessage(const CSector& sector, CMajor* pMajor, const CString& key,
		const CString& object_name) {
	const CString& s = CResourceManager::GetString(key,FALSE,
		object_name,sector.GetName());
	CMessage message;
	message.GenerateMessage(s,MESSAGE_TYPE::MILITARY,sector.GetName(),sector.GetKO(),FALSE);
	pMajor->GetEmpire()->AddMessage(message);
}

void COldRoundDataCalculator::FinishBuild(const int to_build, const CSector& sector, CSystem& system,
		CMajor* pMajor, const CArray<CBuildingInfo,CBuildingInfo>& BuildingInfo) const {
	int list = to_build;
	const CPoint& co = sector.GetKO();
	// Ab jetzt die Abfrage ob Geb�ude oder ein Update fertig wurde
	if (list > 0 && list < 10000 && !BuildingInfo[list-1].GetNeverReady())	// Es wird ein Geb�ude gebaut
	{
		// Die Nachricht, dass neues Geb�ude fertig ist mit allen Daten generieren
		CMessage message;
		message.GenerateMessage(BuildingInfo[list-1].GetBuildingName(), MESSAGE_TYPE::ECONOMY, sector.GetName(), co, FALSE);
		pMajor->GetEmpire()->AddMessage(message);
		// Geb�ude bauen
		m_pDoc->BuildBuilding(list, co);
		// und Geb�ude (welches letztes im Feld) ist auch gleich online setzen, wenn
		// gen�gend Arbeiter da sind
		unsigned short CheckValue = system.SetNewBuildingOnline(&BuildingInfo);
		// Nachricht generierenm das das Geb�ude nicht online genommen werden konnte
		if (CheckValue == 1)
			SystemMessage(sector, pMajor, "NOT_ENOUGH_WORKER", MESSAGE_TYPE::SOMETHING, 1);
		else if (CheckValue == 2)
			SystemMessage(sector, pMajor, "NOT_ENOUGH_ENERGY", MESSAGE_TYPE::SOMETHING, 2);
	}
	else if (list < 0)	// Es wird ein Update gemacht
	{
		list *= (-1);
		// Die Nachricht, dass neues Geb�udeupdate fertig wurde, mit allen Daten generieren
		CMessage message;
		message.GenerateMessage(BuildingInfo[list-1].GetBuildingName(),MESSAGE_TYPE::ECONOMY,sector.GetName(),co,TRUE);
		pMajor->GetEmpire()->AddMessage(message);
		// Vorg�nger von "list" holen
		// Geb�ude mit RunningNumbner == pre werden durch UpdateBuilding() gel�scht und
		// deren Anzahl wird zur�ckgegeben.
		USHORT pre = BuildingInfo[list-1].GetPredecessorID();
		int NumberOfNewBuildings = system.UpdateBuildings(pre);
		// So, nun bauen wir so viel mal das n�chste
		for (int z = 0; z < NumberOfNewBuildings; z++)
		{
			m_pDoc->BuildBuilding(list,co);
			// falls das geupgradete Geb�ude Energie ben�tigt wird versucht es gleich online zu setzen
			if (m_pDoc->GetBuildingInfo(list).GetNeededEnergy() > NULL && system.SetNewBuildingOnline(&BuildingInfo) == 2)
				SystemMessage(sector, pMajor, "NOT_ENOUGH_ENERGY", MESSAGE_TYPE::SOMETHING, 2);
		}
	}
	else if (list >= 10000 && list < 20000)	// Es wird ein Schiff gebaut
	{
		m_pDoc->BuildShip(list, co, pMajor->GetRaceID());
		MilitaryMessage(sector, pMajor, "SHIP_BUILDING_FINISHED", m_pDoc->m_ShipInfoArray[list-10000].GetShipTypeAsString());
	}
	else if (list >= 20000)					// Es wird eine Truppe gebaut
	{
		m_pDoc->BuildTroop(list-20000, co);
		MilitaryMessage(sector, pMajor, "TROOP_BUILDING_FINISHED", m_pDoc->m_TroopInfo[list-20000].GetName());
	}
}

static void CreditsIfBought(CAssemblyList* assembly_list, const int IPProd, CEmpire* pEmpire) {
	// Wenn Geb�ude gekauft wurde, dann die in der letzten Runde noch erbrachte IP-Leistung
	// den Credits des Imperiums gutschreiben, IP-Leistung darf aber nicht gr��er der Baukosten sein
	if (assembly_list->GetWasBuildingBought() == TRUE)
	{
		int goods = IPProd;
		if (goods > assembly_list->GetBuildCosts())
			goods = assembly_list->GetBuildCosts();
		pEmpire->SetCredits(goods);
		assembly_list->SetWasBuildingBought(FALSE);
	}
}

void COldRoundDataCalculator::Build(const CSector& sector, CSystem& system, CMajor* pMajor,
		const CArray<CBuildingInfo,CBuildingInfo>& BuildingInfo) const {
	// Hier berechnen, wenn was in der Bauliste ist, und ob es fertig wird usw.
	CAssemblyList* assembly_list = system.GetAssemblyList();
	const int list = assembly_list->GetAssemblyListEntry(0);
	if (list != 0)	// wenn was drin ist
	{
		const int IPProd = system.CalcIPProd(BuildingInfo, list);
		// Ein Bauauftrag ist fertig gestellt worden
		if (assembly_list->CalculateBuildInAssemblyList(IPProd))
		{
			CreditsIfBought(assembly_list, IPProd, pMajor->GetEmpire());
			FinishBuild(list, sector, system, pMajor, BuildingInfo);
			// Nach CalculateBuildInAssemblyList wird ClearAssemblyList() aufgerufen, wenn der Auftrag fertig wurde.
			// Normalerweise wird nach ClearAssemblyList() die Funktion CalculateVariables() aufgerufen, wegen Geld durch
			// Handelsg�ter wenn nix mehr drin steht. Hier mal testweise weggelassen, weil diese Funktion
			// sp�ter eh f�r das System aufgerufen wird und wir bis jetzt glaub ich keine Notwendigkeit
			// haben die Funktion CalculateVariables() aufzurufen.
			assembly_list->ClearAssemblyList(sector.GetKO(), m_pDoc->m_Systems);
			// Wenn die Bauliste nach dem letzten gebauten Geb�ude leer ist, eine Nachricht generieren
			if (assembly_list->GetAssemblyListEntry(0) == 0)
				SystemMessage(sector, pMajor, "EMPTY_BUILDLIST", MESSAGE_TYPE::SOMETHING, 0);
		}
		assembly_list->CalculateNeededRessourcesForUpdate(&BuildingInfo, system.GetAllBuildings(), pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
	}
}
