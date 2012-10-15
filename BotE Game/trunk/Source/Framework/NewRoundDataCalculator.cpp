#include "stdafx.h"
#include "NewRoundDataCalculator.h"

#include "Races/RaceController.h"

#include <cassert>

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

namespace {
	struct SectorSettings {
		SectorSettings() : scanpower(0), scanned(false), known(false), port(false) {}
		SectorSettings(const CSector& sector, const CString& race) :
				scanpower(sector.GetScanPower(race, false)),
				scanned(sector.GetScanned(race)),
				known(sector.GetKnown(race)),
				port(sector.GetShipPort(race))
			{}
		short scanpower;
		bool scanned;
		bool known;
		bool port;

		bool operator <(const SectorSettings& o) const {
			return scanpower < o.scanpower || scanned < o.scanned || known < o.known || port < o.port;
		}
		SectorSettings& operator =(const SectorSettings& o) {
			scanpower = o.scanpower;
			scanned = o.scanned;
			known = o.known;
			port = o.port;
			return *this;
		}
	};

	std::map<CString, SectorSettings> new_sector_settings;

	static void GiveDiploGoodies(const CSector& sector, const DIPLOMATIC_AGREEMENT::Typ agreement,
							const CString& to, const CString& from, const bool bMinorPort = false) {
		SectorSettings settings;
		const std::map<CString, SectorSettings>::const_iterator found = new_sector_settings.find(to);
		found == new_sector_settings.end() ?
			settings = SectorSettings(sector, to) : settings = found->second;
		const SectorSettings old_settings = settings;

		const CSector::DISCOVER_STATUS from_disc_status = sector.GetDiscoverStatus(from);
		if (agreement >= DIPLOMATIC_AGREEMENT::AFFILIATION) {
			settings.scanpower = max(settings.scanpower, sector.GetScanPower(from, false));
		}

		if(from_disc_status >= CSector::DISCOVER_STATUS_KNOWN) {
			if (agreement >= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
				settings.scanned = true;
			if (agreement >= DIPLOMATIC_AGREEMENT::COOPERATION)
				settings.known = true;
		}
		else if(from_disc_status >= CSector::DISCOVER_STATUS_SCANNED)
			if (agreement >= DIPLOMATIC_AGREEMENT::COOPERATION)
				settings.scanned = true;

		if (sector.GetOwnerOfSector() == from) {
			if (agreement >= DIPLOMATIC_AGREEMENT::TRADE)
				settings.scanned = true;
			if (agreement >= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
				settings.known = true;
		}
		if (bMinorPort || sector.GetShipPort(from))
			if (agreement >= DIPLOMATIC_AGREEMENT::COOPERATION)
				settings.port = true;

		if(old_settings < settings)
			new_sector_settings[to] = settings;
	}

	static void GiveDiploGoodiesMajorOrMinor(const CSector& sector, const CRace& left,
			const CRace& right, bool bMinor) {
		const CString& right_id = right.GetRaceID();
		const DIPLOMATIC_AGREEMENT::Typ agreement = left.GetAgreement(right_id);
		if(agreement < DIPLOMATIC_AGREEMENT::TRADE)
			return;
		const CString& left_id = left.GetRaceID();
		if(bMinor) {
			const CMinor* pMinor = dynamic_cast<const CMinor*>(&right);
			assert(pMinor);
			GiveDiploGoodies(sector, agreement, left_id, right_id,
				sector.GetKO() == pMinor->GetRaceKO() && pMinor->GetSpaceflightNation());
		}
		else {
			GiveDiploGoodies(sector, agreement, left_id, right_id);
			GiveDiploGoodies(sector, agreement, right_id, left_id);
		}
	}
}

void CNewRoundDataCalculator::CalcExtraVisibilityAndRangeDueToDiplomacy(
		CSector& sector, const std::map<CString, CMajor*>* pMajors, const std::map<CString, CMinor*>* pMinors) {

	for(std::map<CString, CMajor*>::const_iterator left_major = pMajors->begin(); left_major != pMajors->end(); ++left_major) {
		for(std::map<CString, CMinor*>::const_iterator minor = pMinors->begin(); minor != pMinors->end(); ++minor) {
			GiveDiploGoodiesMajorOrMinor(sector, *left_major->second, *minor->second, true);
		}

		std::map<CString, CMajor*>::const_iterator right_major = left_major; ++right_major;
		for(; right_major != pMajors->end(); ++right_major) {
			GiveDiploGoodiesMajorOrMinor(sector, *left_major->second, *right_major->second, false);
		}
	}

	for(std::map<CString, SectorSettings>::const_iterator settings = new_sector_settings.begin();
					settings != new_sector_settings.end(); ++settings) {
		sector.SetScanned(settings->first);
		sector.SetKnown(settings->first);
		sector.SetScanPower(settings->second.scanpower, settings->first);
		sector.SetShipPort(TRUE, settings->first);
	}
	new_sector_settings.clear();
}