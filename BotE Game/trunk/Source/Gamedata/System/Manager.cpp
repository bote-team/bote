#include "stdafx.h"
#include "float.h"

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
	m_PriorityMap(o.m_PriorityMap),
	m_iMinMoral(o.m_iMinMoral),
	m_iMinMoralProd(o.m_iMinMoralProd),
	m_bBombWarning(o.m_bBombWarning)
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
	m_iMinMoral = o.m_iMinMoral;
	m_iMinMoralProd = o.m_iMinMoralProd;
	m_bBombWarning = o.m_bBombWarning;
	m_PriorityMap = o.m_PriorityMap;

	return *this;
}

void CSystemManager::Reset()
{
	m_bActive = false;
	m_bSafeMoral = true;
	m_bMaxIndustry = false;
	m_bNeglectFood = false;
	m_iMinMoral = max_min_moral;
	m_iMinMoralProd = max_min_moral_prod;
	m_bBombWarning = true;

	ClearPriorities();
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
		ar << m_iMinMoral;
		ar << m_iMinMoralProd;
		ar << m_bBombWarning;
		ar << static_cast<int>(m_PriorityMap.size());
		for(std::map<WORKER::Typ, int>::const_iterator it = m_PriorityMap.begin();
				it != m_PriorityMap.end(); ++it)
		{
			assert(it->second > 0);
			ar << it->second;
			ar << static_cast<int>(it->first);
		}
	}
	else
	{
		ar >> m_bActive;
		ar >> m_bSafeMoral;
		ar >> m_bMaxIndustry;
		ar >> m_bNeglectFood;
		ar >> m_iMinMoral;
		ar >> m_iMinMoralProd;
		ar >> m_bBombWarning;

		ClearPriorities();
		int map_size;
		ar >> map_size;
		for(int i = 0; i < map_size; ++i)
		{
			int value;
			ar >> value;
			int key;
			ar >> key;
			assert(value > 0);
			AddPriority(static_cast<WORKER::Typ>(key), value);
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
	std::map<WORKER::Typ, int>::const_iterator it = m_PriorityMap.find(type);
	if(it == m_PriorityMap.end())
		return min_priority;
	return it->second;
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

void CSystemManager::ClearPriorities()
{
	m_PriorityMap.clear();
}

void CSystemManager::AddPriority(WORKER::Typ type, int value)
{
	assert(min_priority <= value && value <= max_priority);
	if(value > min_priority)
		m_PriorityMap.insert(std::pair<WORKER::Typ, int>(type, value));
}

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

struct DistributionElem
{
	WORKER::Typ m_Type;
	double m_dCount;
	int m_iCount;
	double m_dFPart;

	DistributionElem(WORKER::Typ type, double count) :
		m_Type(type),
		m_dCount(count),
		m_iCount(static_cast<int>(floor(count))),
		m_dFPart(count - m_iCount)
	{};

	bool operator<(const DistributionElem& o)
	{
		return m_dFPart < o.m_dFPart;
	}
};

class DefaultDistributionCalculator
{
public:

	DefaultDistributionCalculator(int all_workers, const std::map<WORKER::Typ, int>& priorities) :
		m_iAllWorkers(all_workers),
		m_Priorities(priorities),
		m_Result(),
		m_iResultingWorkers(0)
	{};

	std::vector<DistributionElem> Calc()
	{
		if(m_Priorities.empty() || m_iAllWorkers == 0)
			return std::vector<DistributionElem>();
		CalcDefaultWorkersDistributionDouble();
		DistributeRemaining();

		std::sort(m_Result.begin(), m_Result.end(), &comp);

		return m_Result;
	}

private:

	const int m_iAllWorkers;
	const std::map<WORKER::Typ, int> m_Priorities;
	std::vector<DistributionElem> m_Result;
	int m_iResultingWorkers;

	static bool comp(const DistributionElem& left, const DistributionElem& right)
	{
		return left.m_dCount > right.m_dCount;
	}

	double WorkersOn(WORKER::Typ type, double workers) const
	{
		for(std::vector<DistributionElem>::const_iterator it = m_Result.begin(); it != m_Result.end(); ++it)
			workers -= it->m_dCount;
		assert(workers > 0.0f);

		double divisor = 0;
		std::map<WORKER::Typ, int>::const_iterator it = m_Priorities.find(type);
		const double weight_of_this = it->second;
		assert(it != m_Priorities.end());
		for(;;)
		{
			++it;
			if(it == m_Priorities.end())
				break;
			divisor += it->second;
		}
		divisor = 1.0f + divisor / weight_of_this;
		return workers / divisor;
	}

	void CalcDefaultWorkersDistributionDouble()
	{
		double resulting_workers = 0;
		for(std::map<WORKER::Typ, int>::const_iterator it = m_Priorities.begin();
			it != m_Priorities.end(); ++it)
		{
			const double workers = WorkersOn(it->first, m_iAllWorkers);
			resulting_workers += workers;
			m_iResultingWorkers += static_cast<int>(floor(workers));
			m_Result.push_back(DistributionElem(it->first, workers));
		}
		assert(Equals(resulting_workers, m_iAllWorkers));
	}

	void DistributeRemaining()
	{
		std::sort(m_Result.begin(), m_Result.end());
		for(std::vector<DistributionElem>::reverse_iterator it = m_Result.rbegin(); it != m_Result.rend(); ++it)
		{
			if(m_iResultingWorkers == m_iAllWorkers)
				return;
			++m_iResultingWorkers;
			it->m_iCount++;
		}
		assert(false);
	}
};

class CWorkersDistributionCalculator
{
private:
	CSystem* m_pSystem;
	const CSystemProd* const m_pProd;
	const CPoint m_Co;
	int m_WorkersLeftToSet;

	bool FillRemainingSlots(WORKER::Typ type)
	{
		assert(m_WorkersLeftToSet >= 0);
		if(m_WorkersLeftToSet == 0)
			return false;
		m_WorkersLeftToSet += m_pSystem->GetWorker(type);
		m_pSystem->SetWorker(type, CSystem::SET_WORKER_MODE_SET, 0);
		const int buildings = m_pSystem->GetNumberOfWorkbuildings(type, 0);
		const int to_set = min(buildings, m_WorkersLeftToSet);
		SetWorker(type, CSystem::SET_WORKER_MODE_SET,to_set);
		return true;
	}

	void CalculateVariables() const
	{
		const CBotEDoc& doc = *resources::pDoc;
		const CSector& sector = doc.GetSector(m_Co.x, m_Co.y);
		const CMajor* major = dynamic_cast<CMajor*>(doc.GetRaceCtrl()->GetRace(m_pSystem->GetOwnerOfSystem()));
		assert(major);
		m_pSystem->CalculateVariables(sector.GetPlanets(), major);
	}

	int DecrementDueToFullStore(WORKER::Typ type)
	{
		int unset = 0;
		if(!m_pSystem->HasStore(type))
			return unset;
		const int store = m_pSystem->GetResourceStore(type);
		while(true)
		{
			const int workers = m_pSystem->GetWorker(type);
			if(workers == 0)
				break;
			CalculateVariables();
			const int prod = m_pSystem->GetProduction()->GetXProd(type);
			if(store + prod <= m_pSystem->GetXStoreMax(type))
				break;
			SetWorker(type, CSystem::SET_WORKER_MODE_DECREMENT);
			++unset;
		}
		return unset;
	}

	int DecrementDueToWastedIndustry(bool max_industry)
	{
		CalculateVariables();
		const CAssemblyList& assembly_list = *m_pSystem->GetAssemblyList();
		int unset = 0;
		if(assembly_list.IsEmpty() || assembly_list.GetWasBuildingBought())
			return unset;
		const int min_rounds = m_pSystem->NeededRoundsToBuild(0, true);
		assert(min_rounds >= 1);
		if(max_industry && min_rounds > 1 || m_pSystem->GetWorker(WORKER::INDUSTRY_WORKER) == 0)
			return unset;
		while(true)
		{
			SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_DECREMENT);
			++unset;
			CalculateVariables();
			if(min_rounds < m_pSystem->NeededRoundsToBuild(0, true))
			{
				SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_INCREMENT);
				--unset;
				assert(unset >= 0);
				return unset;
			}
		}
	}

	void SetWorker(WORKER::Typ type, CSystem::SetWorkerMode mode, int value = -1)
	{
		if(mode == CSystem::SET_WORKER_MODE_INCREMENT)
		{
			assert(value == -1);
			m_pSystem->SetWorker(type, mode);
			--m_WorkersLeftToSet;
		}
		else if(mode == CSystem::SET_WORKER_MODE_DECREMENT)
		{
			assert(value == -1);
			m_pSystem->SetWorker(type, mode);
			++m_WorkersLeftToSet;
		}
		else if(mode == CSystem::SET_WORKER_MODE_SET)
		{
			assert(value >= 0);
			m_pSystem->SetWorker(type, mode, value);
			m_WorkersLeftToSet -= value;
		}
		assert(m_WorkersLeftToSet >= 0);
	}

	struct SafeMoralWorkerReserve
	{
		CWorkersDistributionCalculator* m_Calc;
		SafeMoralWorkerReserve(CWorkersDistributionCalculator& calc, bool safe_moral) :
			m_Calc(NULL)
		{
			if(safe_moral && calc.m_WorkersLeftToSet >= 1)
			{
				m_Calc = &calc;
				--m_Calc->m_WorkersLeftToSet;
			}
		}
		~SafeMoralWorkerReserve()
		{
			if(m_Calc)
				++m_Calc->m_WorkersLeftToSet;
		}
	};

	void DoMaxPriorities(std::map<WORKER::Typ, int>& prios, bool max_industry)
	{
		for(std::map<WORKER::Typ, int>::const_iterator it = prios.begin(); it != prios.end();)
		{
			assert(it->second <= CSystemManager::max_priority);
			if(it->second != CSystemManager::max_priority)
			{
				++it;
				continue;
			}
			FillRemainingSlots(it->first);
			DecrementDueToFullStore(it->first);
			if(it->first == WORKER::INDUSTRY_WORKER)
				DecrementDueToWastedIndustry(max_industry);
			it = prios.erase(it);
		}
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
			SetWorker(type, CSystem::SET_WORKER_MODE_INCREMENT);
			CalculateVariables();
		}
	}

	void DoPriorities(const std::map<WORKER::Typ, int>& priorities, bool max_industry, bool safe_moral)
	{
		SafeMoralWorkerReserve reserve(*this, safe_moral);
		std::map<WORKER::Typ, int> prios(priorities);
		DoMaxPriorities(prios, max_industry);
		DefaultDistributionCalculator decalc(m_WorkersLeftToSet, prios);
		const std::vector<DistributionElem>& result = decalc.Calc();
		int failed_to_set = 0;
		for(std::vector<DistributionElem>::const_iterator it = result.begin(); it != result.end(); ++it)
		{
			const int buildings = m_pSystem->GetNumberOfWorkbuildings(it->m_Type, 0);
			if(buildings >= it->m_iCount) {
				int try_set = it->m_iCount;
				if(it->m_Type != WORKER::INDUSTRY_WORKER)
					try_set += failed_to_set;
				if(buildings >= try_set) {
					if(it->m_Type != WORKER::INDUSTRY_WORKER)
						failed_to_set = 0;
					SetWorker(it->m_Type, CSystem::SET_WORKER_MODE_SET, try_set);
				}
				else {
					if(it->m_Type != WORKER::INDUSTRY_WORKER)
						failed_to_set -= buildings -it->m_iCount;
					SetWorker(it->m_Type, CSystem::SET_WORKER_MODE_SET, buildings);
				}
			}
			else {
				failed_to_set += it->m_iCount - buildings;
				SetWorker(it->m_Type, CSystem::SET_WORKER_MODE_SET, buildings);
			}
			assert(m_WorkersLeftToSet >= 0 && failed_to_set >= 0);
			failed_to_set += DecrementDueToFullStore(it->m_Type);
			if(it->m_Type == WORKER::INDUSTRY_WORKER) {
				failed_to_set += DecrementDueToWastedIndustry(max_industry);
			}
		}
	}

	void DoRemaining()
	{
		if(FillRemainingSlots(WORKER::FOOD_WORKER))
			DecrementDueToFullStore(WORKER::FOOD_WORKER);

		FillRemainingSlots(WORKER::RESEARCH_WORKER);

		if(FillRemainingSlots(WORKER::TITAN_WORKER))
			DecrementDueToFullStore(WORKER::TITAN_WORKER);
		if(FillRemainingSlots(WORKER::DEUTERIUM_WORKER))
			DecrementDueToFullStore(WORKER::DEUTERIUM_WORKER);
		if(FillRemainingSlots(WORKER::DURANIUM_WORKER))
			DecrementDueToFullStore(WORKER::DURANIUM_WORKER);
		if(FillRemainingSlots(WORKER::CRYSTAL_WORKER))
			DecrementDueToFullStore(WORKER::CRYSTAL_WORKER);
		if(FillRemainingSlots(WORKER::IRIDIUM_WORKER))
			DecrementDueToFullStore(WORKER::IRIDIUM_WORKER);

		FillRemainingSlots(WORKER::SECURITY_WORKER);

		FillRemainingSlots(WORKER::INDUSTRY_WORKER);

		FillRemainingSlots(WORKER::TITAN_WORKER);
		FillRemainingSlots(WORKER::DEUTERIUM_WORKER);
		FillRemainingSlots(WORKER::DURANIUM_WORKER);
		FillRemainingSlots(WORKER::CRYSTAL_WORKER);
		FillRemainingSlots(WORKER::IRIDIUM_WORKER);
		FillRemainingSlots(WORKER::FOOD_WORKER);

		FillRemainingSlots(WORKER::ENERGY_WORKER);
	}

	void SafeMoral()
	{
		const CAssemblyList& assembly_list = *m_pSystem->GetAssemblyList();
		assert(m_WorkersLeftToSet >= 0);
		if(assembly_list.IsEmpty() || m_WorkersLeftToSet == 0)
			return;
		const int max_buildings = m_pSystem->GetNumberOfWorkbuildings(WORKER::INDUSTRY_WORKER, 0);
		if(m_pSystem->GetWorker(WORKER::INDUSTRY_WORKER) < max_buildings)
			SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_INCREMENT);
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
	//industry, security, research, titan, deuterium, duranium, crystal, iridium
	//in order according to priorities
	calc.DoPriorities(m_PriorityMap, m_bMaxIndustry, m_bSafeMoral);
	if(m_bSafeMoral)
		calc.SafeMoral();
	//distribute any remaining workers
	calc.DoRemaining();
	calc.Finish();

	return true;
}

class CEnergyConsumersChecker
{
public:
	CEnergyConsumersChecker(CSystem& system, const CPoint& p) :
		m_pSystem(&system),
		m_Co(p)
		{}

	bool ShouldTakeShipyardOnline() const
	{
		const CAssemblyList& assembly_list = *m_pSystem->GetAssemblyList();
		if(assembly_list.IsEmpty())
			return false;
		const int nAssemblyListEntry = assembly_list.GetAssemblyListEntry(0);
		if(nAssemblyListEntry < 10000 || nAssemblyListEntry >= 20000)
			return false;
		return true;
	}

	bool CheckDefense(bool& bomb_warning) const
	{
		bomb_warning |= BomberInSector();
		return bomb_warning;
	}

	bool CheckMoral(const CBuildingInfo& info, const CBuilding& building, bool require_positive,
		int min_moral = CSystemManager::max_min_moral,
		int min_moral_prod = CSystemManager::max_min_moral_prod) const
	{
		if(info.GetMoralProd() > 0)
			return true;
		if(require_positive)
			return false;
		assert(info.GetMoralProd() < 0);
		if(m_pSystem->GetMoral() < min_moral)
			return false;
		const int moral_prod = m_pSystem->GetProduction()->GetMoralProd();
		if(building.GetIsBuildingOnline())
			return moral_prod >= min_moral_prod;
		return moral_prod + info.GetMoralProd() >= min_moral_prod;
	}

	bool CheckDeritiumRefinery(const CBuildingInfo& info, const CBuilding& building) const
	{
		const unsigned max_store = m_pSystem->GetDeritiumStoreMax();
		if(building.GetIsBuildingOnline())
			return m_pSystem->GetDeritiumStore() + m_pSystem->GetProduction()->GetDeritiumProd() <= max_store;
		return m_pSystem->GetDeritiumStore() + m_pSystem->GetProduction()->GetDeritiumProd()
			+ info.GetDeritiumProd() <= max_store;
	}

	void CalculateVariables() const
	{
		const CBotEDoc& doc = *resources::pDoc;
		const CSector& sector = doc.GetSector(m_Co.x, m_Co.y);
		const CMajor* major = dynamic_cast<CMajor*>(doc.GetRaceCtrl()->GetRace(m_pSystem->GetOwnerOfSystem()));
		assert(major);
		m_pSystem->CalculateVariables(sector.GetPlanets(), major);
	}

private:
	CSystem* m_pSystem;
	const CPoint m_Co;

	bool BomberInSector() const
	{
		const CBotEDoc& doc = *resources::pDoc;
		const CSector& sector = doc.GetSector(m_Co.x, m_Co.y);
		const std::set<CString> races = sector.ShipsInSector();
		for(std::set<CString>::const_iterator it = races.begin(); it != races.end(); ++it)
		{
			const DIPLOMATIC_AGREEMENT::Typ agreement = doc.GetRaceCtrl()->GetRace(*it)
				->GetAgreement(m_pSystem->GetOwnerOfSystem());
			if(agreement == DIPLOMATIC_AGREEMENT::WAR)
				return true;
		}
		return false;
	}
};

bool CSystemManager::CheckEnergyConsumers(CSystem& system, const CPoint& p)
{
	CEnergyConsumersChecker checker(system, p);
	CArray<CBuilding>* buildings = system.GetAllBuildings();
	bool bomb_warning = false;
	const CSystemProd& prod = *system.GetProduction();
	const int energy_consumption = prod.GetMaxEnergyProd() - prod.GetEnergyProd();
	int additional_available_energy = prod.GetPotentialEnergyProd() - energy_consumption;
	for(int i = 0; i < buildings->GetSize(); ++i)
	{
		CBuilding& building = buildings->GetAt(i);
		const CBuildingInfo& info = resources::BuildingInfo->GetAt(building.GetRunningNumber() - 1);
		const int needed = info.GetNeededEnergy();
		if(needed == 0)
			continue;
		bool should_be_online = building.GetIsBuildingOnline();
		if(info.GetShipYard())
		{
			assert(!info.IsDefenseBuilding());
			should_be_online = checker.ShouldTakeShipyardOnline();
			should_be_online = should_be_online || checker.CheckMoral(info, building, true);
		}
		if(info.IsDefenseBuilding())
		{
			assert(!info.GetShipYard());
			should_be_online = checker.CheckDefense(bomb_warning);
			should_be_online = should_be_online || checker.CheckMoral(info, building, true);
		}
		if(info.IsDeritiumRefinery())
			should_be_online = checker.CheckDeritiumRefinery(info, building);
		if(info.IsAcceptableMinusMoral())
			should_be_online = checker.CheckMoral(info, building, false, m_iMinMoral, m_iMinMoralProd);

		const bool is_online = building.GetIsBuildingOnline();
		if(should_be_online && !is_online)
		{
			if(additional_available_energy >= needed)
			{
				building.SetIsBuildingOnline(true);
				additional_available_energy -= needed;
				checker.CalculateVariables();
			}
		}
		else if (!should_be_online && is_online)
		{
			const int needed = info.GetNeededEnergy();
			building.SetIsBuildingOnline(false);
			additional_available_energy += needed;
			checker.CalculateVariables();
		}
	}
	assert(additional_available_energy >= 0);
	return m_bBombWarning && bomb_warning;
}

bool CSystemManager::CheckFamine(const CSystem& system)
{
	const int food_prod = system.GetProduction()->GetFoodProd();
	if(food_prod >= 0)
		return false;
	const int food_store = system.GetFoodStore();
	return food_store + food_prod < 0;
}

