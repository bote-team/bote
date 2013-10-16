#include "stdafx.h"

#include <cassert>

#include "Manager.h"
#include "System.h"
#include "Resources.h"
#include "BotEDoc.h"
#include "Races/RaceController.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CSystemManager::CSystemManager()
{
	Reset();
}

CSystemManager::CSystemManager(const CSystemManager& o) :
	m_bActive(o.m_bActive),
	m_PriorityList(o.m_PriorityList)
{
}

CSystemManager::~CSystemManager(void)
{
}

CSystemManager& CSystemManager::operator=(const CSystemManager& o)
{
	if(this == &o)
		return *this;

	m_bActive = o.m_bActive;
	m_PriorityList = o.m_PriorityList;

	return *this;
}

void CSystemManager::Reset()
{
	m_bActive = false;


	m_PriorityList.clear();
	m_PriorityList.reserve(PrioritiesCount);

	m_PriorityList.push_back(WORKER::SECURITY_WORKER);
	m_PriorityList.push_back(WORKER::RESEARCH_WORKER);
	m_PriorityList.push_back(WORKER::TITAN_WORKER);
	m_PriorityList.push_back(WORKER::DEUTERIUM_WORKER);
	m_PriorityList.push_back(WORKER::DURANIUM_WORKER);
	m_PriorityList.push_back(WORKER::CRYSTAL_WORKER);
	m_PriorityList.push_back(WORKER::IRIDIUM_WORKER);
}

//////////////////////////////////////////////////////////////////////
// Serialisierungsfunktionen
//////////////////////////////////////////////////////////////////////

void CSystemManager::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar << m_bActive;
		for(std::vector<WORKER::Typ>::const_iterator it = m_PriorityList.begin();
				it != m_PriorityList.end(); ++it)
			ar << static_cast<int>(*it);
	}
	else
	{
		ar >> m_bActive;
		for(std::vector<WORKER::Typ>::iterator it = m_PriorityList.begin();
				it != m_PriorityList.end(); ++it)
		{
			int value;
			ar >> value;
			*it = static_cast<WORKER::Typ>(value);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// getting info
//////////////////////////////////////////////////////////////////////

bool CSystemManager::Active() const
{
	return m_bActive;
}

//////////////////////////////////////////////////////////////////////
// setters
//////////////////////////////////////////////////////////////////////

void CSystemManager::Activate()
{
	m_bActive = true;
}

void CSystemManager::DeActivate()
{
	m_bActive = false;
}

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

namespace //helpers for DistributeWorkers()
{
	void CalculateVariables(CSystem& system, const CPoint& p)
	{
		const CBotEDoc& doc = *resources::pDoc;
		const CSector& sector = doc.GetSector(p.x, p.y);
		const CMajor* major = dynamic_cast<CMajor*>(doc.GetRaceCtrl()->GetRace(system.GetOwnerOfSystem()));
		assert(major);
		system.CalculateVariables(sector.GetPlanets(), major);
	}

	bool IncreaseWorkersUntilSufficient(CSystem& system, int& workers_left_to_set, WORKER::Typ type,
		const CPoint& p, const CSystemProd& prod)
	{
		assert(type == WORKER::ENERGY_WORKER || type == WORKER::FOOD_WORKER);
		while(true)
		{
			const int value = (type == WORKER::ENERGY_WORKER) ? prod.GetEnergyProd() : prod.GetFoodProd();
			if(value >= 0)
				return true;
			if(workers_left_to_set <= 0)
				return false;
			const int number_of_buildings = system.GetNumberOfWorkbuildings(type, 0);
			const int workers_set = system.GetWorker(type);
			assert(workers_set <= number_of_buildings);
			if(workers_set == number_of_buildings)
				return false;
			system.SetWorker(type, CSystem::SET_WORKER_MODE_INCREMENT);
			--workers_left_to_set;
			CalculateVariables(system, p);
		}
	}
}

bool CSystemManager::DistributeWorkers(CSystem& system, const CPoint& p) const
{
	system.FreeAllWorkers();
	CalculateVariables(system, p);
	int workers_left_to_set = system.GetWorker(WORKER::ALL_WORKER);
	const CSystemProd& prod = *system.GetProduction();


	//energy
	if(!IncreaseWorkersUntilSufficient(system, workers_left_to_set, WORKER::ENERGY_WORKER, p, prod))
		return false;

	//food
	if(!IncreaseWorkersUntilSufficient(system, workers_left_to_set, WORKER::FOOD_WORKER, p, prod))
		return false;

	//industry
	const CAssemblyList& assembly_list = *system.GetAssemblyList();
	if(!assembly_list.IsEmpty())
	{
		if(workers_left_to_set <= 0)
			return false;
		const int max_buildings = system.GetNumberOfWorkbuildings(WORKER::INDUSTRY_WORKER, 0);
		const int industry_workers = min(max_buildings, workers_left_to_set);
		workers_left_to_set -= industry_workers;
		system.SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_SET, industry_workers);
		CalculateVariables(system, p);
		const int min_rounds = system.NeededRoundsToBuild(0);
		while(min_rounds == system.NeededRoundsToBuild(0))
		{
			system.SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_DECREMENT);
			++workers_left_to_set;
			CalculateVariables(system, p);
		}
		//undo last step of the loop
		system.SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_INCREMENT);
		--workers_left_to_set;
		//+1 worker in case of moral loss
		if(workers_left_to_set >= 1 && system.GetWorker(WORKER::INDUSTRY_WORKER) < max_buildings)
		{
			system.SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_INCREMENT);
			--workers_left_to_set;
		}
	}

	//security, research, titan, deuterium, duranium, crystal, iridium in order according to priorities
	for(std::vector<WORKER::Typ>::const_iterator it = m_PriorityList.begin();
			it != m_PriorityList.end(); ++it)
	{
		const int to_set = min(workers_left_to_set, system.GetNumberOfWorkbuildings(*it, 0));
		workers_left_to_set -= to_set;
		system.SetWorker(*it, CSystem::SET_WORKER_MODE_SET, to_set);
		if(workers_left_to_set == 0)
			break;
	}

	assert(workers_left_to_set >= 0);
	assert(system.SanityCheckWorkers());

	CalculateVariables(system, p);

	return true;
}

