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
	m_bSafeMoral(o.m_bSafeMoral),
	m_bMaxIndustry(o.m_bMaxIndustry),
	m_bNeglectFood(o.m_bNeglectFood),
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
	m_bSafeMoral = o.m_bSafeMoral;
	m_bMaxIndustry = o.m_bMaxIndustry;
	m_bNeglectFood = o.m_bNeglectFood;
	m_PriorityMap = o.m_PriorityMap;

	return *this;
}

void CSystemManager::Reset()
{
	m_bActive = false;
	m_bSafeMoral = true;
	m_bMaxIndustry = false;
	m_bNeglectFood = false;

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
		ar << m_bSafeMoral;
		ar << m_bMaxIndustry;
		ar << m_bNeglectFood;
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
		ar >> m_bSafeMoral;
		ar >> m_bMaxIndustry;
		ar >> m_bNeglectFood;

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

bool CSystemManager::SafeMoral() const
{
	return m_bSafeMoral;
}

bool CSystemManager::MaxIndustry() const
{
	return m_bMaxIndustry;
}

bool CSystemManager::NeglectFood() const
{
	return m_bNeglectFood;
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

void CSystemManager::SetSafeMoral(bool is)
{
	m_bSafeMoral = is;
}

void CSystemManager::SetMaxIndustry(bool is)
{
	m_bMaxIndustry = is;
}

void CSystemManager::SetNeglectFood(bool is)
{
	m_bNeglectFood = is;
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
		const CPoint& p, const CSystemProd& prod, bool allow_insufficient)
	{
		assert(type == WORKER::ENERGY_WORKER || type == WORKER::FOOD_WORKER);
		while(true)
		{
			const int value = (type == WORKER::ENERGY_WORKER) ? prod.GetEnergyProd() : prod.GetFoodProd();
			if(value >= 0)
				return true;
			if(workers_left_to_set <= 0)
				return allow_insufficient;
			const int number_of_buildings = system.GetNumberOfWorkbuildings(type, 0);
			const int workers_set = system.GetWorker(type);
			assert(workers_set <= number_of_buildings);
			if(workers_set == number_of_buildings)
				return allow_insufficient;
			system.SetWorker(type, CSystem::SET_WORKER_MODE_INCREMENT);
			--workers_left_to_set;
			CalculateVariables(system, p);
		}
	}

	int FillRemainingSlots(CSystem& system, WORKER::Typ type, int workers_left_to_set)
	{
		const int current_workers = system.GetWorker(type);
		const int buildings = system.GetNumberOfWorkbuildings(type, 0);
		assert(0 <= current_workers && current_workers <= buildings);
		const int still_free = buildings - current_workers;
		if(still_free == 0)
			return 0;
		const int to_set = min(still_free, workers_left_to_set);
		system.SetWorker(type, CSystem::SET_WORKER_MODE_SET, current_workers + to_set);
		return to_set;
	}
}

bool CSystemManager::DistributeWorkers(CSystem& system, const CPoint& p) const
{
	system.FreeAllWorkers();
	CalculateVariables(system, p);
	int workers_left_to_set = system.GetWorker(WORKER::ALL_WORKER);
	const CSystemProd& prod = *system.GetProduction();


	//energy
	if(!IncreaseWorkersUntilSufficient(system, workers_left_to_set, WORKER::ENERGY_WORKER, p, prod, false))
		return false;

	//food
	if(!m_bNeglectFood || CheckFamine(system))
		if(!IncreaseWorkersUntilSufficient(system, workers_left_to_set, WORKER::FOOD_WORKER, p, prod, true))
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
		assert(min_rounds >= 1);
		if((!m_bMaxIndustry || min_rounds == 1) && !assembly_list.GetWasBuildingBought())
		{
			while(min_rounds == system.NeededRoundsToBuild(0))
			{
				system.SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_DECREMENT);
				++workers_left_to_set;
				CalculateVariables(system, p);
			}
			//undo last step of the loop
			system.SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_INCREMENT);
			--workers_left_to_set;
			if(m_bSafeMoral && min_rounds == 1 && workers_left_to_set >= 1
				&& system.GetWorker(WORKER::INDUSTRY_WORKER) < max_buildings)
			{
				//+1 worker in case of moral loss
				system.SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_INCREMENT);
				--workers_left_to_set;
			}
		}
	}

	//security, research, titan, deuterium, duranium, crystal, iridium in order according to priorities
	for(std::map<int, WORKER::Typ>::const_reverse_iterator it = m_PriorityMap.rbegin();
		it != m_PriorityMap.rend(); ++it)
	{
		const int to_set = min(workers_left_to_set, system.GetNumberOfWorkbuildings(it->second, 0));
		workers_left_to_set -= to_set;
		system.SetWorker(it->second, CSystem::SET_WORKER_MODE_SET, to_set);
		if(system.HasStore(it->second))
		{
			const int store = system.GetResourceStore(it->second);
			while(true)
			{
				const int workers = system.GetWorker(it->second);
				if(workers == 0)
					break;
				CalculateVariables(system, p);
				const int prod = system.GetProduction()->GetXProd(it->second);
				if(store + prod <= MAX_RES_STORE)
					break;
				system.SetWorker(it->second, CSystem::SET_WORKER_MODE_DECREMENT);
				++workers_left_to_set;
			}
		}
		if(workers_left_to_set == 0)
			break;
	}

	//distribute any remaining workers
	if(workers_left_to_set > 0 && system.GetFoodStore() < MAX_FOOD_STORE)
		workers_left_to_set -= FillRemainingSlots(system, WORKER::FOOD_WORKER, workers_left_to_set);
	if(workers_left_to_set > 0)
		workers_left_to_set -= FillRemainingSlots(system, WORKER::INDUSTRY_WORKER, workers_left_to_set);
	if(workers_left_to_set > 0)
		workers_left_to_set -= FillRemainingSlots(system, WORKER::TITAN_WORKER, workers_left_to_set);
	if(workers_left_to_set > 0)
		workers_left_to_set -= FillRemainingSlots(system, WORKER::DEUTERIUM_WORKER, workers_left_to_set);
	if(workers_left_to_set > 0)
		workers_left_to_set -= FillRemainingSlots(system, WORKER::DURANIUM_WORKER, workers_left_to_set);
	if(workers_left_to_set > 0)
		workers_left_to_set -= FillRemainingSlots(system, WORKER::CRYSTAL_WORKER, workers_left_to_set);
	if(workers_left_to_set > 0)
		workers_left_to_set -= FillRemainingSlots(system, WORKER::IRIDIUM_WORKER, workers_left_to_set);
	if(workers_left_to_set > 0)
		workers_left_to_set -= FillRemainingSlots(system, WORKER::FOOD_WORKER, workers_left_to_set);
	if(workers_left_to_set > 0)
		workers_left_to_set -= FillRemainingSlots(system, WORKER::ENERGY_WORKER, workers_left_to_set);

	assert(workers_left_to_set >= 0);
#ifdef CONSISTENCY_CHECKS
	assert(system.SanityCheckWorkers());
#endif

	CalculateVariables(system, p);

	return true;
}

static bool ShouldTakeShipyardOnline(const CSystem& system)
{
	const CAssemblyList& assembly_list = *system.GetAssemblyList();
	if(assembly_list.IsEmpty())
		return false;
	const int nAssemblyListEntry = assembly_list.GetAssemblyListEntry(0);
	if(nAssemblyListEntry < 10000 || nAssemblyListEntry >= 20000)
		return false;
	return true;
}

void CSystemManager::CheckShipyard(CSystem& system) const
{
	const bool should_be_online = ShouldTakeShipyardOnline(system);
	CArray<CBuilding>* buildings = system.GetAllBuildings();
	for(int i = 0; i < buildings->GetSize(); ++i)
	{
		CBuilding& building = buildings->GetAt(i);
		const CBuildingInfo& info = resources::BuildingInfo->GetAt(building.GetRunningNumber() - 1);
		if(info.GetShipYard() && info.GetNeededEnergy() > 0)
			building.SetIsBuildingOnline(should_be_online);
	}
}

bool CSystemManager::CheckFamine(const CSystem& system) const
{
	const int food_prod = system.GetProduction()->GetFoodProd();
	if(food_prod >= 0)
		return false;
	const int food_store = system.GetFoodStore();
	return food_store + food_prod < 0;
}

