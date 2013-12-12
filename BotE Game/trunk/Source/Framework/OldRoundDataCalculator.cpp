#include "stdafx.h"
#include "OldRoundDataCalculator.h"

#include "Races/RaceController.h"
#include "General/Loc.h"
#include "ClientWorker.h"



COldRoundDataCalculator::COldRoundDataCalculator(void)
{
}

COldRoundDataCalculator::~COldRoundDataCalculator(void)
{
}

COldRoundDataCalculator::COldRoundDataCalculator(CBotEDoc* pDoc)
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

	// Hier die Gebäude abreißen, die angeklickt wurden
	if (system.DestroyBuildings())
		system.CalculateNumberOfWorkbuildings(&BuildingInfo);

	// Variablen berechnen lassen, bevor der Planet wächst -> diese ins Lager
	// nur berechnen, wenn das System auch jemandem gehört, ansonsten würde auch die Mind.Prod. ins Lager kommen
	// In CalculateStorages wird auch die Systemmoral berechnet. Wenn wir einen Auftrag mit
	// NeverReady (z.B. Kriegsrecht) in der Bauliste haben, und dieser Moral produziert, dann diese
	// auf die Moral anrechnen. Das wird dann in CalculateStorages gemacht.
	int list = system.GetAssemblyList()->GetAssemblyListEntry(0);
	if (list > 0 && list < 10000 && BuildingInfo[list-1].GetNeverReady() && system.GetMoral() <= 85)
		prod->AddMoralProd(BuildingInfo[list-1].GetMoralProd());
}

int COldRoundDataCalculator::DeritiumForTheAI(bool human, const CSystem& system, float fDifficultyLevel) {
	// KI Anpassungen (KI bekommt zufälig etwas Deritium geschenkt)
	int diliAdd = 0;
	if (!human && system.GetProduction()->GetDeritiumProd() == 0)
	{
		// umso höher der Schwierigkeitsgrad, desto höher die Wahrscheinlichkeit, das die KI
		// Deritium auf ihrem Systemen geschenkt bekommt
		int temp = rand()%((int)(fDifficultyLevel * 7.5));
		//TRACE("KI: System: %s - DiliAddProb: %d (NULL for adding Dili) - Difficulty: %.2lf\n",sector.GetName(), temp, fDifficultyLevel);
		if (temp == 0)
			diliAdd = 1;
	}
	return diliAdd;
}

void COldRoundDataCalculator::ExecuteRebellion(CSystem& system, CMajor* pMajor) const
{
	CEmpire* pEmpire = pMajor->GetEmpire();
	const CPoint& co = system.GetCo();
	const CString& sectorname = system.GetName();

	// Nachricht über Rebellion erstellen
	CString news = CLoc::GetString("REBELLION_IN_SYSTEM", FALSE, sectorname);
	CEmpireNews message;
	message.CreateNews(news, EMPIRE_NEWS_TYPE::SOMETHING, "", co);
	pEmpire->AddMsg(message);
	resources::pClientWorker->SetToEmpireViewFor(*pMajor);

	// zusätzliche Eventnachricht (Lose a System to Rebellion #18) wegen der Moral an das Imperium
	message.CreateNews(pMajor->GetMoralObserver()->AddEvent(18, pMajor->GetRaceMoralNumber(), sectorname), EMPIRE_NEWS_TYPE::SOMETHING, "", co);
	pEmpire->AddMsg(message);

	if (system.GetMinorRace())
	{
		const boost::shared_ptr<CMinor>& pMinor = boost::dynamic_pointer_cast<CMinor>(system.HomeOf());
		AssertBotE(pMinor);

		if (!system.Taken())
		{
			pMinor->SetAgreement(pMajor->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);
			pMajor->SetAgreement(pMinor->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);

			pMinor->SetRelation(pMajor->GetRaceID(), (-(rand()%50+20)));
			news = CLoc::GetString("MINOR_CANCELS_MEMBERSHIP", FALSE, pMinor->GetName());
			CEmpireNews message;
			message.CreateNews(news, EMPIRE_NEWS_TYPE::DIPLOMACY, "", co);
			pEmpire->AddMsg(message);
		}
		boost::shared_ptr<CMajor> empty;
		pMinor->SetOwner(empty);
		system.ChangeOwner(pMinor->GetRaceID(), CSystem::OWNING_STATUS_INDEPENDENT_MINOR);
	}
	else
		system.ChangeOwner("", CSystem::OWNING_STATUS_REBELLED);

	// wichtige Variablen zurücksetzen
	system.SetShipPort(FALSE, pMajor->GetRaceID());
}

void COldRoundDataCalculator::ExecuteFamine(CSystem& system, CMajor* pMajor)
{
	const CPoint& co = system.GetCo();
	CEmpire* pEmpire = pMajor->GetEmpire();
	system.LetPlanetsShrink((float)(system.GetFoodStore()) * 0.01f);
	// nur wenn die Moral über 50 ist sinkt die Moral durch Hungersnöte
	if (system.GetMoral() > 50)
		system.SetMoral((short)(system.GetFoodStore() / (system.GetHabitants() + 1))); // +1, wegen Division durch NULL umgehen
	system.SetFoodStore(0);

	CString news = CLoc::GetString("FAMINE", FALSE, system.GetName());
	CEmpireNews message;
	message.CreateNews(news, EMPIRE_NEWS_TYPE::SOMETHING, "", co, false, 1);
	pEmpire->AddMsg(message);
	resources::pClientWorker->SetToEmpireViewFor(*pMajor);
}

void COldRoundDataCalculator::SystemMessage(const CSystem& sector, CMajor* pMajor, const CString& key,
		EMPIRE_NEWS_TYPE::Typ message_typ, BYTE byFlag) {
	const CString& news = CLoc::GetString(key, FALSE, sector.GetName());
	CEmpireNews message;
	message.CreateNews(news, message_typ, "", sector.GetCo(), false, byFlag);
	pMajor->GetEmpire()->AddMsg(message);
	resources::pClientWorker->SetToEmpireViewFor(*pMajor);
}

void COldRoundDataCalculator::HandlePopulationEffects(CSystem& system, CMajor* pMajor) {
	const float fCurrentHabitants = system.GetCurrentHabitants();
	CEmpire* pEmpire = pMajor->GetEmpire();
	pEmpire->AddPopSupportCosts((UINT)fCurrentHabitants * POPSUPPORT_MULTI);
	// Funktion gibt TRUE zurück, wenn wir durch die Bevölkerung eine neue Handelsroute anlegen können
	if (system.SetHabitants(fCurrentHabitants))
	{
		// wenn die Spezialforschung "mindestens 1 Handelsroute erforscht wurde, dann die Meldung erst bei
		// der 2.ten Handelroute bringen
		// Hier die Boni durch die Uniqueforschung "Handel" -> mindestens eine Handelsroute
		bool bMinOneRoute = pEmpire->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED;
		if (bMinOneRoute == false || (bMinOneRoute == true && (int)(system.GetHabitants() / TRADEROUTEHAB) > 1))
			SystemMessage(system, pMajor, "ENOUGH_HABITANTS_FOR_NEW_TRADEROUTE", EMPIRE_NEWS_TYPE::ECONOMY, 4);
	}
}

static void MilitaryMessage(const CSector& sector, CMajor* pMajor, const CString& key,
		const CString& object_name) {
	const CString& s = CLoc::GetString(key,FALSE,
		object_name,sector.GetName());
	CEmpireNews message;
	message.CreateNews(s,EMPIRE_NEWS_TYPE::MILITARY,sector.GetName(),sector.GetCo());
	pMajor->GetEmpire()->AddMsg(message);
}

void COldRoundDataCalculator::FinishBuild(const int to_build, CSystem& system,
		CMajor* pMajor, const CArray<CBuildingInfo,CBuildingInfo>& BuildingInfo) const {
	int list = to_build;
	const CPoint& co = system.GetCo();
	// Ab jetzt die Abfrage ob Gebäude oder ein Update fertig wurde
	if (list > 0 && list < 10000 && !BuildingInfo[list-1].GetNeverReady())	// Es wird ein Gebäude gebaut
	{
		// Die Nachricht, dass neues Gebäude fertig ist mit allen Daten generieren
		CEmpireNews message;
		message.CreateNews(BuildingInfo[list-1].GetBuildingName(), EMPIRE_NEWS_TYPE::ECONOMY, system.GetName(), co);
		pMajor->GetEmpire()->AddMsg(message);
		// Gebäude bauen
		m_pDoc->BuildBuilding(list, co);
		// und Gebäude (welches letztes im Feld) ist auch gleich online setzen, wenn
		// genügend Arbeiter da sind
		unsigned short CheckValue = system.SetNewBuildingOnline(&BuildingInfo);
		// Nachricht generieren das das Gebäude nicht online genommen werden konnte
		const CSystemManager& manager = system.Manager();
		if (CheckValue == 1 && !manager.Active())
			SystemMessage(system, pMajor, "NOT_ENOUGH_WORKER", EMPIRE_NEWS_TYPE::SOMETHING, 1);
		else if (CheckValue == 2 && !manager.Active())
			SystemMessage(system, pMajor, "NOT_ENOUGH_ENERGY", EMPIRE_NEWS_TYPE::SOMETHING, 2);
	}
	else if (list < 0)	// Es wird ein Update gemacht
	{
		list *= (-1);
		// Die Nachricht, dass neues Gebäudeupdate fertig wurde, mit allen Daten generieren
		CEmpireNews message;
		message.CreateNews(BuildingInfo[list-1].GetBuildingName(),EMPIRE_NEWS_TYPE::ECONOMY,system.GetName(),co,true);
		pMajor->GetEmpire()->AddMsg(message);
		// Vorgänger von "list" holen
		// Gebäude mit RunningNumbner == nPredecessorID werden durch UpdateBuilding() gelöscht und
		// deren Anzahl wird zurückgegeben.
		USHORT nPredecessorID = BuildingInfo[list-1].GetPredecessorID();
		AssertBotE(nPredecessorID > 0);
		const CBuildingInfo* pPredecessorBuilding = &(BuildingInfo[nPredecessorID - 1]);
		AssertBotE(pPredecessorBuilding->GetRunningNumber() == nPredecessorID);
		int nNumberOfNewBuildings = system.UpdateBuildings(nPredecessorID, pPredecessorBuilding->GetNeededEnergy());
		// So, nun bauen wir so viel mal das nächste
		for (int z = 0; z < nNumberOfNewBuildings; z++)
		{
			m_pDoc->BuildBuilding(list,co);

			// falls das geupgradete Gebäude Energie benötigt wird versucht es gleich online zu setzen
			const CSystemManager& manager = system.Manager();
			if (m_pDoc->GetBuildingInfo(list).GetNeededEnergy() > 0 && system.SetNewBuildingOnline(&BuildingInfo) == 2 && !manager.Active())
				SystemMessage(system, pMajor, "NOT_ENOUGH_ENERGY", EMPIRE_NEWS_TYPE::SOMETHING, 2);
		}
	}
	else if (list >= 10000 && list < 20000)	// Es wird ein Schiff gebaut
	{
		m_pDoc->BuildShip(list, co, pMajor->GetRaceID());
		MilitaryMessage(system, pMajor, "SHIP_BUILDING_FINISHED", m_pDoc->m_ShipInfoArray[list-10000].GetShipTypeAsString());
	}
	else if (list >= 20000)					// Es wird eine Truppe gebaut
	{
		m_pDoc->BuildTroop(list-20000, co);
		MilitaryMessage(system, pMajor, "TROOP_BUILDING_FINISHED", m_pDoc->m_TroopInfo[list-20000].GetName());
	}
}

static void CreditsIfBought(CAssemblyList* assembly_list, const int IPProd, CEmpire* pEmpire) {
	// Wenn Gebäude gekauft wurde, dann die in der letzten Runde noch erbrachte IP-Leistung
	// den Credits des Imperiums gutschreiben, IP-Leistung darf aber nicht größer der Baukosten sein
	if (assembly_list->GetWasBuildingBought() == TRUE)
	{
		int goods = IPProd;
		if (goods > assembly_list->GetBuildCosts())
			goods = assembly_list->GetBuildCosts();
		pEmpire->SetCredits(goods);
		assembly_list->SetWasBuildingBought(FALSE);
	}
}

void COldRoundDataCalculator::Build(CSystem& system, CMajor* pMajor,
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
			FinishBuild(list, system, pMajor, BuildingInfo);
			// Nach CalculateBuildInAssemblyList wird ClearAssemblyList() aufgerufen, wenn der Auftrag fertig wurde.
			// Normalerweise wird nach ClearAssemblyList() die Funktion CalculateVariables() aufgerufen, wegen Geld durch
			// Handelsgüter wenn nix mehr drin steht. Hier mal testweise weggelassen, weil diese Funktion
			// später eh für das System aufgerufen wird und wir bis jetzt glaub ich keine Notwendigkeit
			// haben die Funktion CalculateVariables() aufzurufen.
			assembly_list->ClearAssemblyList(system.GetCo(), m_pDoc->m_Systems);
			// Wenn die Bauliste nach dem letzten gebauten Gebäude leer ist, eine Nachricht generieren
			if (assembly_list->GetAssemblyListEntry(0) == 0)
				SystemMessage(system, pMajor, "EMPTY_BUILDLIST", EMPIRE_NEWS_TYPE::SOMETHING, 0);
		}
		assembly_list->CalculateNeededRessourcesForUpdate(&BuildingInfo, system.GetAllBuildings(), pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
	}

	system.TrainTroops();
}
