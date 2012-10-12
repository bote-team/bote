#include "stdafx.h"
#include "NewRoundDataCalculator.h"

#include "Races/RaceController.h"

CNewRoundDataCalculator::CNewRoundDataCalculator(void)
{
}

CNewRoundDataCalculator::~CNewRoundDataCalculator(void)
{
}

CNewRoundDataCalculator::CNewRoundDataCalculator(CBotf2Doc* pDoc)
{
	m_pDoc = pDoc;
}

void CNewRoundDataCalculator::GetIntelligenceBoniFromSpecialTechsAndSetThem(
	const std::map<CString, CMajor*>* pmMajors) {
	// Geheimdienstboni aus Spezialforschungen holen
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CIntelligence* pIntelligence = it->second->GetEmpire()->GetIntelligence();
		const CResearchComplex* pResearchComplex = it->second->GetEmpire()->GetResearch()->GetResearchInfo()
			->GetResearchComplex(RESEARCH_COMPLEX::SECURITY);
		// Die Boni auf die einzelnen Geheimdienstgebiete berechnen
		if (pResearchComplex->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
			pIntelligence->AddInnerSecurityBonus(pResearchComplex->GetBonus(1));
		else if (pResearchComplex->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
		{
			pIntelligence->AddEconomyBonus(pResearchComplex->GetBonus(2), 1);
			pIntelligence->AddMilitaryBonus(pResearchComplex->GetBonus(2), 1);
			pIntelligence->AddScienceBonus(pResearchComplex->GetBonus(2), 1);
		}
		else if (pResearchComplex->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
		{
			pIntelligence->AddEconomyBonus(pResearchComplex->GetBonus(3), 0);
			pIntelligence->AddMilitaryBonus(pResearchComplex->GetBonus(3), 0);
			pIntelligence->AddScienceBonus(pResearchComplex->GetBonus(3), 0);
		}
	}
}

static void EmitLostRouteMessage(unsigned deletedRoutes, const CString& single_key, const CString& multi_key,
		const CString& sectorname, const CPoint& co, CEmpire* pEmpire) {
	CString news;
	if (deletedRoutes == 1)
		news = CResourceManager::GetString(single_key,FALSE,sectorname);
	else
	{
		CString lost; lost.Format("%u",deletedRoutes);
		news = CResourceManager::GetString(multi_key,FALSE,lost,sectorname);
	}
	CMessage message;
	message.GenerateMessage(news, MESSAGE_TYPE::ECONOMY, "", co, FALSE, 4);
	pEmpire->AddMessage(message);
}

void CNewRoundDataCalculator::CheckRoutes(const CSector& sector, CSystem& system, CMajor* pMajor) {
	CEmpire* pEmpire = pMajor->GetEmpire();
	const CPoint& co = sector.GetKO();
	const CString& sector_name = sector.GetName();
	bool select_empire_view = false;

	unsigned deletedTradeRoutes = 0;
	deletedTradeRoutes += system.CheckTradeRoutesDiplomacy(*m_pDoc, co);
	deletedTradeRoutes += system.CheckTradeRoutes(pEmpire->GetResearch()->GetResearchInfo());
	if(deletedTradeRoutes > 0) {
		select_empire_view = true;
		EmitLostRouteMessage(deletedTradeRoutes, "LOST_ONE_TRADEROUTE", "LOST_TRADEROUTES",
			sector_name, co, pEmpire);
	}
	unsigned deletedResourceRoutes = 0;
	deletedResourceRoutes += system.CheckResourceRoutesExistence(*m_pDoc);
	deletedResourceRoutes += system.CheckResourceRoutes(pEmpire->GetResearch()->GetResearchInfo());
	if(deletedResourceRoutes > 0) {
		select_empire_view = true;
		EmitLostRouteMessage(deletedResourceRoutes, "LOST_ONE_RESOURCEROUTE", "LOST_RESOURCEROUTES",
			sector_name, co, pEmpire);
	}

	if(select_empire_view && pMajor->IsHumanPlayer()) {
		const network::RACE client = m_pDoc->m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
		m_pDoc->m_iSelectedView[client] = EMPIRE_VIEW;
	}
}

void CNewRoundDataCalculator::CalcIntelligenceBoni(const CSystemProd* production, CIntelligence* intelligence) {
	// Hier die gesamten Sicherheitsboni der Imperien berechnen
	intelligence->AddInnerSecurityBonus(production->GetInnerSecurityBoni());
	intelligence->AddEconomyBonus(production->GetEconomySpyBoni(), 0);
	intelligence->AddEconomyBonus(production->GetEconomySabotageBoni(), 1);
	intelligence->AddScienceBonus(production->GetScienceSpyBoni(), 0);
	intelligence->AddScienceBonus(production->GetScienceSabotageBoni(), 1);
	intelligence->AddMilitaryBonus(production->GetMilitarySpyBoni(), 0);
	intelligence->AddMilitaryBonus(production->GetMilitarySabotageBoni(), 1);
}

void CNewRoundDataCalculator::CalcMoral(const CSector& sector, CSystem& system, CArray<CTroopInfo>& TroopInfo) {
	// Wurde das System militärisch erobert, so verringert sich die Moral pro Runde etwas
	if (sector.GetTakenSector() && system.GetMoral() > 70)
		system.SetMoral(-rand()%2);
	// möglicherweise wird die Moral durch stationierte Truppen etwas stabilisiert
	system.IncludeTroopMoralValue(&TroopInfo);
}

void CNewRoundDataCalculator::CalcExtraVisibilityAndRangeDueToDiplomacy(CSector& sector, const std::map<CString, CMajor*>* pmMajors) {
	for (map<CString, CMajor*>::const_iterator i = pmMajors->begin(); i != pmMajors->end(); ++i)
	{
		for (map<CString, CMajor*>::const_iterator j = pmMajors->begin(); j != pmMajors->end(); ++j)
		{
			if(i == j) continue;
			const DIPLOMATIC_AGREEMENT::Typ agreement = i->second->GetAgreement(j->first);
			if (sector.GetScanned(i->first))
			{
				if (agreement >= DIPLOMATIC_AGREEMENT::COOPERATION)
					sector.SetScanned(j->first);
				if (agreement >= DIPLOMATIC_AGREEMENT::AFFILIATION)
				{
					const short iscanpower = sector.GetScanPower(i->first, false);
					const short jscanpower = sector.GetScanPower(j->first, false);
					if(iscanpower > jscanpower)
						sector.SetScanPower(iscanpower, j->first);
				}
			}
			if (sector.GetKnown(i->first))
			{
				if (agreement >= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					sector.SetScanned(j->first);
				if (agreement >= DIPLOMATIC_AGREEMENT::COOPERATION)
					sector.SetKnown(j->first);
			}
			if (sector.GetOwnerOfSector() == i->first)
			{
				if (agreement >= DIPLOMATIC_AGREEMENT::TRADE)
					sector.SetScanned(j->first);
				if (agreement >= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					sector.SetKnown(j->first);
			}
			if (sector.GetShipPort(i->first))
				if (agreement >= DIPLOMATIC_AGREEMENT::COOPERATION)
					sector.SetShipPort(TRUE, j->first);
		}//for (map<CString, CMajor*>::const_iterator j = pmMajors->begin(); j != pmMajors->end(); ++j)
	}//for (map<CString, CMajor*>::const_iterator i = pmMajors->begin(); i != pmMajors->end(); ++i)
}

void CNewRoundDataCalculator::AddShipPortsFromMinors(const std::map<CString, CMajor*>& pmMajors) {
	const map<CString, CMinor*>* pmMinors = m_pDoc->m_pRaceCtrl->GetMinors();
	for (map<CString, CMajor*>::const_iterator i = pmMajors.begin(); i != pmMajors.end(); ++i)
	{
		CMajor* pMajor = i->second;
		for (map<CString, CMinor*>::const_iterator j = pmMinors->begin(); j != pmMinors->end(); ++j)
		{
			const CMinor* pMinor = j->second;
			const DIPLOMATIC_AGREEMENT::Typ agreement = pMinor->GetAgreement(pMajor->GetRaceID());
			if (pMinor->GetSpaceflightNation()&& (
					agreement == DIPLOMATIC_AGREEMENT::COOPERATION ||
					agreement == DIPLOMATIC_AGREEMENT::AFFILIATION))
			{
				CPoint p = pMinor->GetRaceKO();
				if (p != CPoint(-1,-1))
					m_pDoc->GetSector(p).SetShipPort(TRUE, pMajor->GetRaceID());
			}
		}
	}
}

void CNewRoundDataCalculator::CalcPreLoop() {
	CSystemProd::ResetMoralEmpireWide();
	// Hier müssen wir nochmal die Systeme durchgehen und die imperienweite Moralproduktion auf die anderen System
	// übertragen
	for(std::vector<CSector>::iterator se = m_pDoc->m_Sectors.begin(); se != m_pDoc->m_Sectors.end(); ++se) {
		CSystem* sy = &m_pDoc->GetSystemForSector(*se);
		if(se->GetSunSystem() && sy->GetOwnerOfSystem() != "") {
			// imperiumsweite Moralproduktion aus diesem System berechnen
			sy->CalculateEmpireWideMoralProd(&m_pDoc->BuildingInfo);
			//Building scan power and range in a system isn't influenced by other systems, is it...?
			//This needs to be here in the first loop, since when calculating the scan power that
			//other majors get due to affiliation, the scan powers in all sectors are not yet calculated correctly.
			//For instance, if the system (1,1) scans the sector (0,0) since the loop
			//starts at (0,0) in the top left; so when transferring the scanpower in (0,0)
			//it is not yet updated.
			const CSystemProd& production = *sy->GetProduction();
			const int scan_power = production.GetScanPower();
			if(scan_power > 0)
				m_pDoc->PutScannedSquareOverCoords(*se, production.GetScanRange(), scan_power,
					*m_pDoc->m_pRaceCtrl->GetRace(sy->GetOwnerOfSystem()));
		}
	}
}