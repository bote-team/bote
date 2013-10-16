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
	m_PriorityMap(o.m_PriorityMap)
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
	m_PriorityMap = o.m_PriorityMap;

	return *this;
}

void CSystemManager::Reset()
{
	m_bActive = false;

	ClearPriorities(true);
}

//////////////////////////////////////////////////////////////////////
// Serialisierungsfunktionen
//////////////////////////////////////////////////////////////////////

void CSystemManager::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar << m_bActive;
		ar << static_cast<int>(m_PriorityMap.size());
		for(std::map<int, WORKER::Typ>::const_iterator it = m_PriorityMap.begin();
				it != m_PriorityMap.end(); ++it)
		{
			ar << it->first;
			ar << static_cast<int>(it->second);
		}
	}
	else
	{
		ar >> m_bActive;

		ClearPriorities(false);
		int map_size;
		ar >> map_size;
		for(int i = 0; i < map_size; ++i)
		{
			int key;
			ar >> key;
			int value;
			ar >> value;
			AddPriority(key, static_cast<WORKER::Typ>(value));
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

int CSystemManager::Priority(WORKER::Typ type) const
{
	for(std::map<int, WORKER::Typ>::const_iterator it = m_PriorityMap.begin(); it != m_PriorityMap.end(); ++it)
	{
		if(it->second == type)
			return it->first;
	}
	assert(false);
	return 0;
}

//////////////////////////////////////////////////////////////////////
// setters
//////////////////////////////////////////////////////////////////////

void CSystemManager::SetActive(bool is)
{
	m_bActive = is;
}

void CSystemManager::ClearPriorities(bool refill_with_standard)
{
	m_PriorityMap.clear();
	if(!refill_with_standard)
		return;
	int i = 1;
	AddPriority(i++, WORKER::SECURITY_WORKER);
	AddPriority(i++, WORKER::RESEARCH_WORKER);
	AddPriority(i++, WORKER::TITAN_WORKER);
	AddPriority(i++, WORKER::DEUTERIUM_WORKER);
	AddPriority(i++, WORKER::DURANIUM_WORKER);
	AddPriority(i++, WORKER::CRYSTAL_WORKER);
	AddPriority(i++, WORKER::IRIDIUM_WORKER);
}

void CSystemManager::AddPriority(int value, WORKER::Typ type)
{
	assert(1 <= value && value <= max_priority);
	while(m_PriorityMap.find(value) != m_PriorityMap.end())
	{
		if(value > max_priority / 2.0f)
			--value;
		else
			++value;
	}
	assert(1 <= value && value <= max_priority);
	m_PriorityMap.insert(std::pair<int, WORKER::Typ>(value, type));
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
	for(std::map<int, WORKER::Typ>::const_reverse_iterator it = m_PriorityMap.rbegin();
		it != m_PriorityMap.rend(); ++it)
	{
		const int to_set = min(workers_left_to_set, system.GetNumberOfWorkbuildings(it->second, 0));
		workers_left_to_set -= to_set;
		system.SetWorker(it->second, CSystem::SET_WORKER_MODE_SET, to_set);
		if(workers_left_to_set == 0)
			break;
	}

	assert(workers_left_to_set >= 0);
	assert(system.SanityCheckWorkers());

	CalculateVariables(system, p);

	return true;
}

