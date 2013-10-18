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

class CWorkersDistributionCalculator
{
private:
	CSystem* m_pSystem;
	const CSystemProd* const m_pProd;
	const CPoint m_Co;
	int m_WorkersLeftToSet;

	int FillRemainingSlots(WORKER::Typ type) const
	{
		const int current_workers = m_pSystem->GetWorker(type);
		const int buildings = m_pSystem->GetNumberOfWorkbuildings(type, 0);
		assert(0 <= current_workers && current_workers <= buildings);
		const int still_free = buildings - current_workers;
		if(still_free == 0)
			return 0;
		const int to_set = min(still_free, m_WorkersLeftToSet);
		m_pSystem->SetWorker(type, CSystem::SET_WORKER_MODE_SET, current_workers + to_set);
		return to_set;
	}

	void CalculateVariables() const
	{
		const CBotEDoc& doc = *resources::pDoc;
		const CSector& sector = doc.GetSector(m_Co.x, m_Co.y);
		const CMajor* major = dynamic_cast<CMajor*>(doc.GetRaceCtrl()->GetRace(m_pSystem->GetOwnerOfSystem()));
		assert(major);
		m_pSystem->CalculateVariables(sector.GetPlanets(), major);
	}

public:

	CWorkersDistributionCalculator(CSystem& system, const CPoint& p) :
		m_pSystem(&system),
		m_pProd(system.GetProduction()),
		m_Co(p),
		m_WorkersLeftToSet(system.GetWorker(WORKER::ALL_WORKER))
	{
	}

	void Prepare() const
	{
		m_pSystem->FreeAllWorkers();
		CalculateVariables();
	}

	void Finish() const
	{
		assert(m_WorkersLeftToSet >= 0);
		#ifdef CONSISTENCY_CHECKS
			assert(m_pSystem->SanityCheckWorkers());
		#endif
		CalculateVariables();
	}

	bool IncreaseWorkersUntilSufficient(WORKER::Typ type, bool allow_insufficient)
	{
		assert(type == WORKER::ENERGY_WORKER || type == WORKER::FOOD_WORKER);
		while(true)
		{
			const int value = (type == WORKER::ENERGY_WORKER) ? m_pProd->GetEnergyProd() : m_pProd->GetFoodProd();
			if(value >= 0)
				return true;
			if(m_WorkersLeftToSet <= 0)
				return allow_insufficient;
			const int number_of_buildings = m_pSystem->GetNumberOfWorkbuildings(type, 0);
			const int workers_set = m_pSystem->GetWorker(type);
			assert(workers_set <= number_of_buildings);
			if(workers_set == number_of_buildings)
				return allow_insufficient;
			m_pSystem->SetWorker(type, CSystem::SET_WORKER_MODE_INCREMENT);
			--m_WorkersLeftToSet;
			CalculateVariables();
		}
	}

	bool DoIndustry(bool max_industry, bool safe_moral)
	{
		const CAssemblyList& assembly_list = *m_pSystem->GetAssemblyList();
		if(assembly_list.IsEmpty())
			return true;
		if(m_WorkersLeftToSet <= 0)
			return false;
		const int max_buildings = m_pSystem->GetNumberOfWorkbuildings(WORKER::INDUSTRY_WORKER, 0);
		const int industry_workers = min(max_buildings, m_WorkersLeftToSet);
		m_WorkersLeftToSet -= industry_workers;
		m_pSystem->SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_SET, industry_workers);
		CalculateVariables();
		const int min_rounds = m_pSystem->NeededRoundsToBuild(0);
		assert(min_rounds >= 1);
		if((!max_industry || min_rounds == 1) && !assembly_list.GetWasBuildingBought())
		{
			while(min_rounds == m_pSystem->NeededRoundsToBuild(0)
				&& m_pSystem->GetWorker(WORKER::INDUSTRY_WORKER) > 0)
			{
				m_pSystem->SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_DECREMENT);
				++m_WorkersLeftToSet;
				CalculateVariables();
			}
			//undo last step of the loop
			m_pSystem->SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_INCREMENT);
			--m_WorkersLeftToSet;
			if(safe_moral && min_rounds == 1 && m_WorkersLeftToSet >= 1
				&& m_pSystem->GetWorker(WORKER::INDUSTRY_WORKER) < max_buildings)
			{
				//+1 worker in case of moral loss
				m_pSystem->SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_INCREMENT);
				--m_WorkersLeftToSet;
			}
		}
		return true;
	}

	void DoPriorities(const std::map<int, WORKER::Typ>& priorities)
	{
		for(std::map<int, WORKER::Typ>::const_reverse_iterator it = priorities.rbegin();
			it != priorities.rend(); ++it)
		{
			const int to_set = min(m_WorkersLeftToSet, m_pSystem->GetNumberOfWorkbuildings(it->second, 0));
			m_WorkersLeftToSet -= to_set;
			m_pSystem->SetWorker(it->second, CSystem::SET_WORKER_MODE_SET, to_set);
			if(m_pSystem->HasStore(it->second))
			{
				const int store = m_pSystem->GetResourceStore(it->second);
				while(true)
				{
					const int workers = m_pSystem->GetWorker(it->second);
					if(workers == 0)
						break;
					CalculateVariables();
					const int prod = m_pSystem->GetProduction()->GetXProd(it->second);
					if(store + prod <= MAX_RES_STORE)
						break;
					m_pSystem->SetWorker(it->second, CSystem::SET_WORKER_MODE_DECREMENT);
					++m_WorkersLeftToSet;
				}
			}
			if(m_WorkersLeftToSet == 0)
				break;
		}
	}

	void DoRemaining()
	{
		//distribute any remaining workers
		if(m_WorkersLeftToSet > 0 && m_pSystem->GetFoodStore() < MAX_FOOD_STORE)
			m_WorkersLeftToSet -= FillRemainingSlots(WORKER::FOOD_WORKER);
		if(m_WorkersLeftToSet > 0)
			m_WorkersLeftToSet -= FillRemainingSlots(WORKER::INDUSTRY_WORKER);
		if(m_WorkersLeftToSet > 0)
			m_WorkersLeftToSet -= FillRemainingSlots(WORKER::TITAN_WORKER);
		if(m_WorkersLeftToSet > 0)
			m_WorkersLeftToSet -= FillRemainingSlots(WORKER::DEUTERIUM_WORKER);
		if(m_WorkersLeftToSet > 0)
			m_WorkersLeftToSet -= FillRemainingSlots(WORKER::DURANIUM_WORKER);
		if(m_WorkersLeftToSet > 0)
			m_WorkersLeftToSet -= FillRemainingSlots(WORKER::CRYSTAL_WORKER);
		if(m_WorkersLeftToSet > 0)
			m_WorkersLeftToSet -= FillRemainingSlots(WORKER::IRIDIUM_WORKER);
		if(m_WorkersLeftToSet > 0)
			m_WorkersLeftToSet -= FillRemainingSlots(WORKER::FOOD_WORKER);
		if(m_WorkersLeftToSet > 0)
			m_WorkersLeftToSet -= FillRemainingSlots(WORKER::ENERGY_WORKER);
	}

};

bool CSystemManager::DistributeWorkers(CSystem& system, const CPoint& p) const
{
	CWorkersDistributionCalculator calc(system, p);

	calc.Prepare();
	//energy
	if(!calc.IncreaseWorkersUntilSufficient(WORKER::ENERGY_WORKER, false))
		return false;
	//food
	if(!m_bNeglectFood || CheckFamine(system))
		if(!calc.IncreaseWorkersUntilSufficient(WORKER::FOOD_WORKER, true))
			return false;
	//industry
	if(!calc.DoIndustry(m_bMaxIndustry, m_bSafeMoral))
		return false;
	//security, research, titan, deuterium, duranium, crystal, iridium in order according to priorities
	calc.DoPriorities(m_PriorityMap);
	//distribute any remaining workers
	calc.DoRemaining();
	calc.Finish();

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

void CSystemManager::CheckShipyard(CSystem& system)
{
	const bool should_be_online = ShouldTakeShipyardOnline(system);
	CArray<CBuilding>* buildings = system.GetAllBuildings();
	for(int i = 0; i < buildings->GetSize(); ++i)
	{
		CBuilding& building = buildings->GetAt(i);
		const CBuildingInfo& info = resources::BuildingInfo->GetAt(building.GetRunningNumber() - 1);
		const int needed = info.GetNeededEnergy();
		if(info.GetShipYard() && needed > 0)
		{
			if(should_be_online && system.GetProduction()->GetEnergyProd() >= needed)
				building.SetIsBuildingOnline(should_be_online);
			else if(!should_be_online && info.GetMoralProd() <= 0)
				building.SetIsBuildingOnline(should_be_online);
		}
	}
}

bool CSystemManager::CheckFamine(const CSystem& system)
{
	const int food_prod = system.GetProduction()->GetFoodProd();
	if(food_prod >= 0)
		return false;
	const int food_store = system.GetFoodStore();
	return food_store + food_prod < 0;
}

