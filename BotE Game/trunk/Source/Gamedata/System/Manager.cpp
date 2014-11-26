#include "stdafx.h"
#include "float.h"



#include "Manager.h"
#include "System.h"
#include "Resources.h"
#include "BotEDoc.h"
#include "Races/RaceController.h"
#include "Ships/Ships.h"

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
	m_iMinMoral(o.m_iMinMoral),
	m_iMinMoralProd(o.m_iMinMoralProd),
	m_bBombWarning(o.m_bBombWarning),
	m_bOnOffline(o.m_bOnOffline),
	m_PriorityMap(o.m_PriorityMap),
	m_IgnoredBuildings(o.m_IgnoredBuildings)
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
	m_bOnOffline = o.m_bOnOffline;
	m_IgnoredBuildings = o.m_IgnoredBuildings;

	return *this;
}

void CSystemManager::Reset()
{
	m_bActive = false;
	m_bSafeMoral = false;
	m_bMaxIndustry = false;
	m_bNeglectFood = false;
	m_iMinMoral = max_min_moral;
	m_iMinMoralProd = max_min_moral_prod;
	m_bBombWarning = true;
	m_bOnOffline = true;
	m_IgnoredBuildings.clear();

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
		ar << m_bOnOffline;
		ar << static_cast<int>(m_PriorityMap.size());
		for(std::map<WORKER::Typ, int>::const_iterator it = m_PriorityMap.begin();
				it != m_PriorityMap.end(); ++it)
		{
			AssertBotE(it->second > 0);
			ar << it->second;
			ar << static_cast<int>(it->first);
		}

		ar << static_cast<int>(m_IgnoredBuildings.size());
		for(std::set<int>::const_iterator i = m_IgnoredBuildings.begin();
			i != m_IgnoredBuildings.end(); ++i)
		{
			ar << *i;
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
		ar >> m_bOnOffline;

		ClearPriorities();
		int size;
		ar >> size;
		for(int i = 0; i < size; ++i)
		{
			int value;
			ar >> value;
			int key;
			ar >> key;
			AssertBotE(value > 0);
			AddPriority(static_cast<WORKER::Typ>(key), value);
		}

		ar >> size;
		m_IgnoredBuildings.clear();
		for(int i = 0; i < size; ++i)
		{
			int value;
			ar >> value;
			m_IgnoredBuildings.insert(value);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// getting
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
// setting
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
	AssertBotE(min_priority <= value && value <= max_priority);
	if(value > min_priority)
		m_PriorityMap.insert(std::pair<WORKER::Typ, int>(type, value));
}

void CSystemManager::SetIgnoredBuildings(const std::set<int>& ignored)
{
	m_IgnoredBuildings = ignored;
}

void CSystemManager::ToggleBuildingIgnored(int id)
{
	const std::set<int>::const_iterator i = m_IgnoredBuildings.find(id);
	if(i == m_IgnoredBuildings.end())
		m_IgnoredBuildings.insert(id);
	else
		m_IgnoredBuildings.erase(i);
}

void CSystemManager::UpgradeIgnoredBuilding(int old_id, int new_id)
{
	std::set<int>::iterator i = m_IgnoredBuildings.find(old_id);
	if(i == m_IgnoredBuildings.end())
		return;
	*i = new_id;
}

//////////////////////////////////////////////////////////////////////
// workers distribution
//////////////////////////////////////////////////////////////////////

//holds the relation data between a producible resource needing workers and the amount of workers it gets
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

	bool operator<(const DistributionElem& o) const
	{
		return m_dFPart < o.m_dFPart;
	}
};

//Class which calculates a default distribution (that is, without considering number of buildings) onto (currently)
//cathegories industry, security, research, titan, deuterium, duranium, crystal, iridium.
//Energy and food are currently not passed to this class.
//The only deciding factors are given priorities and number of all_workers (determined by system population).
//The whole class is basically an implementation of the following mathematical formula:
//workers_i = (all_workers - sum(workers_j, j = 1..(i-1))) / (1 + (1 / priority_i) * sum(priority_j, j = (i + 1)..n))
//where i = 1..n with n = 8 in our case (count of priorities).
//This means that workers_i : workers_j = priority_i : priority_j for i,j = 1..n; i and j symbolizing one of the
//cathegories from above.
//
//The algorithm first distributes workers according to the integer part of the resulting double worker counts,
//then sorts it according to the floating point part, and distributes any remaining workers onto those results
//with the highest floating point parts.
//
//The result is sorted according to number of workers the cathegory got (by the foating point result,
//not the integer one).
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

		//calculate the distribution with floating point results
		CalcDefaultWorkersDistributionDouble();

		//give any workers which remain after considering the integer parts
		//to those cthegories with the highest floating point parts
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
		AssertBotE(workers > 0.0f);

		double divisor = 0;
		std::map<WORKER::Typ, int>::const_iterator it = m_Priorities.find(type);
		const double weight_of_this = it->second;
		AssertBotE(it != m_Priorities.end());
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
		AssertBotE(Equals(resulting_workers, m_iAllWorkers));
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
		AssertBotE(false);
	}
};

//Class handling the actual distribution of workers.
class CWorkersDistributionCalculator
{
private:
	CSystem* m_pSystem;
	const CSystemProd* const m_pProd;

	//currently remaining workers
	//At the start of the algorithm all workers are unset, thus this is the same as system population at that point.
	int m_WorkersLeftToSet;

	//fills all remaining empty buildings of the given cathegory, only considering available workers and buildings
	bool FillRemainingSlots(WORKER::Typ type)
	{
		AssertBotE(m_WorkersLeftToSet >= 0);
		if(m_WorkersLeftToSet == 0)
			return false;
		m_WorkersLeftToSet += m_pSystem->GetWorker(type);
		m_pSystem->SetWorker(type, CSystem::SET_WORKER_MODE_SET, 0);
		const int buildings = m_pSystem->GetNumberOfWorkbuildings(type, 0);
		const int to_set = min(buildings, m_WorkersLeftToSet);
		SetWorker(type, CSystem::SET_WORKER_MODE_SET,to_set);
		return true;
	}

	//removes workers from a cathegory which has a store until production + store <= store,
	//so that nothing will be wasted on next turn change
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
			m_pSystem->CalculateVariables();
			const int prod = m_pSystem->GetProduction()->GetXProd(type);
			if(store + prod <= m_pSystem->GetXStoreMax(type))
				break;
			SetWorker(type, CSystem::SET_WORKER_MODE_DECREMENT);
			++unset;
		}
		return unset;
	}

	void OnBestIndustryWorkerCountFound(int& unset)
	{
		SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_INCREMENT);
		--unset;
		AssertBotE(unset >= 0);
	}

	//removes workers from factories until they are the minimum number to finish the current project
	//in still the same number of turns (as at the start of the function)
	int DecrementDueToWastedIndustry(bool max_industry)
	{
		m_pSystem->CalculateVariables();
		const CAssemblyList& assembly_list = *m_pSystem->GetAssemblyList();
		int unset = 0;
		if(assembly_list.IsEmpty() || assembly_list.GetWasBuildingBought())
			return unset;
		const int min_rounds = m_pSystem->NeededRoundsToBuild(0, true);
		AssertBotE(min_rounds >= 1);
		if(max_industry && min_rounds > 1 || m_pSystem->GetWorker(WORKER::INDUSTRY_WORKER) == 0)
			return unset;
		while(true)
		{
			SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_DECREMENT);
			++unset;
			m_pSystem->CalculateVariables();
			const int current_rounds = m_pSystem->NeededRoundsToBuild(0, true, false);
			if(m_pSystem->GetWorker(WORKER::INDUSTRY_WORKER) == 0)
			{
				if(min_rounds < current_rounds)
					OnBestIndustryWorkerCountFound(unset);
				return unset;
			}
			if(min_rounds < current_rounds)
			{
				OnBestIndustryWorkerCountFound(unset);
				return unset;
			}
		}
	}

	//worker setting function which makes according changes to m_WorkersLeftToSet, tracking how many
	//we still have to distribute at all
	void SetWorker(WORKER::Typ type, CSystem::SetWorkerMode mode, int value = -1)
	{
		if(mode == CSystem::SET_WORKER_MODE_INCREMENT)
		{
			AssertBotE(value == -1);
			m_pSystem->SetWorker(type, mode);
			--m_WorkersLeftToSet;
		}
		else if(mode == CSystem::SET_WORKER_MODE_DECREMENT)
		{
			AssertBotE(value == -1);
			m_pSystem->SetWorker(type, mode);
			++m_WorkersLeftToSet;
		}
		else if(mode == CSystem::SET_WORKER_MODE_SET)
		{
			AssertBotE(value >= 0);
			m_pSystem->SetWorker(type, mode, value);
			m_WorkersLeftToSet -= value;
		}
		AssertBotE(m_WorkersLeftToSet >= 0);
	}

	//if "safe moral" feature is active, we need to reserve a worker before distributing the others
	//onto the cathegories to be able to put him into industry afterwards
	// (it's unknown from which cathegory we should take this worker away otherwise)
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

	//distributes workers onto the cathegories which are on max; these are removed from the map afterwards
	//and no longer considered
	void DoMaxPriorities(std::map<WORKER::Typ, int>& prios, bool max_industry)
	{
		for(std::map<WORKER::Typ, int>::const_iterator it = prios.begin(); it != prios.end();)
		{
			AssertBotE(it->second <= CSystemManager::max_priority);
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

	CWorkersDistributionCalculator(CSystem& system) :
		m_pSystem(&system),
		m_pProd(system.GetProduction()),
		m_WorkersLeftToSet(system.GetWorker(WORKER::ALL_WORKER))
	{
	}

	void Prepare() const
	{
		m_pSystem->FreeAllWorkers();
		m_pSystem->CalculateVariables();
	}

	void Finish() const
	{
		AssertBotE(m_WorkersLeftToSet >= 0);
		#ifdef CONSISTENCY_CHECKS
			AssertBotE(m_pSystem->SanityCheckWorkers());
		#endif
		m_pSystem->CalculateVariables();
	}

	//Indreases workers in cathegories energy and food until we produce enough to suffice for the consumption we have
	bool IncreaseWorkersUntilSufficient(WORKER::Typ type, bool allow_insufficient)
	{
		AssertBotE(type == WORKER::ENERGY_WORKER || type == WORKER::FOOD_WORKER);
		if(m_pSystem->GetDisabledProductions()[type])
			return true;
		while(true)
		{
			const int value = (type == WORKER::ENERGY_WORKER) ? m_pProd->GetEnergyProd() : m_pProd->GetFoodProd();
			if(value >= 0)
				return true;
			if(m_WorkersLeftToSet <= 0)
				return allow_insufficient;
			const int number_of_buildings = m_pSystem->GetNumberOfWorkbuildings(type, 0);
			const int workers_set = m_pSystem->GetWorker(type);
			AssertBotE(workers_set <= number_of_buildings);
			if(workers_set == number_of_buildings)
				return allow_insufficient;
			SetWorker(type, CSystem::SET_WORKER_MODE_INCREMENT);
			m_pSystem->CalculateVariables();
		}
	}

	//distributes workers onto priorities remaining after max priorities have been processed
	//workers that cannot be set because of number of buildings are tried to be set into
	//the cathegory with next less workers
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
			AssertBotE(m_WorkersLeftToSet >= 0 && failed_to_set >= 0);
			failed_to_set += DecrementDueToFullStore(it->m_Type);
			if(it->m_Type == WORKER::INDUSTRY_WORKER) {
				failed_to_set += DecrementDueToWastedIndustry(max_industry);
			}
		}
	}

	//fills any workers remaining into remaining worker slots, with decreasing order of sense
	//at first we try to only put them if store isn't already full, then we produce industry
	//(gives a little money), at last we produce superfluous energy
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

	//puts an additional worker into industry, to prevent not finishing the project because
	//industry prod decreased due to loss of moral
	void SafeMoral()
	{
		const CAssemblyList& assembly_list = *m_pSystem->GetAssemblyList();
		AssertBotE(m_WorkersLeftToSet >= 0);
		if(assembly_list.IsEmpty() || m_WorkersLeftToSet == 0)
			return;
		const int max_buildings = m_pSystem->GetNumberOfWorkbuildings(WORKER::INDUSTRY_WORKER, 0);
		if(m_pSystem->GetWorker(WORKER::INDUSTRY_WORKER) < max_buildings)
			SetWorker(WORKER::INDUSTRY_WORKER, CSystem::SET_WORKER_MODE_INCREMENT);
	}

};

bool CSystemManager::DistributeWorkers(CSystem& system) const
{
	CWorkersDistributionCalculator calc(system);

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
	CEnergyConsumersChecker(CSystem& system) :
		m_pSystem(&system)
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

	bool CheckMoral(const CBuildingInfo& info, const CBuilding& building,
		int min_moral = CSystemManager::max_min_moral,
		int min_moral_prod = CSystemManager::max_min_moral_prod) const
	{
		AssertBotE(info.GetMoralProd() < 0);
		if(m_pSystem->GetMoral() < min_moral)
			return false;
		const int moral_prod = m_pSystem->GetProduction()->GetMoralProd();
		if(building.GetIsBuildingOnline())
			return moral_prod >= min_moral_prod;
		return moral_prod + info.GetMoralProd() >= min_moral_prod;
	}

	bool CheckStoreFull(RESOURCES::TYPE type, const CBuildingInfo& info, const CBuilding& building) const
	{
		const int max_store = m_pSystem->GetXStoreMax(type);
		const int store = m_pSystem->GetResourceStore(type);
		const int prod = info.GetXProd(type);
		if(building.GetIsBuildingOnline())
			return store + prod <= max_store;
		return store + m_pSystem->GetProduction()->GetResourceProd(type) + prod <= max_store;
	}

	bool BomberInSector() const
	{
		std::map<CString, CShipMap> ships = m_pSystem->ShipsInSector();
		for(std::map<CString, CShipMap>::const_iterator it = ships.begin(); it != ships.end();)
		{
			const DIPLOMATIC_AGREEMENT::Typ agreement = it->second.iterator_at(0)->second->Owner()
				->GetAgreement(m_pSystem->OwnerID());
			if(agreement != DIPLOMATIC_AGREEMENT::WAR)
			{
				it = ships.erase(it);
				continue;
			}
			++it;
		}
		return ships.empty() ? false : CheckShips(ships);
	}

private:
	CSystem* m_pSystem;

	bool CheckShips(const std::map<CString, CShipMap>& ships) const
	{
		const int scan_power = m_pSystem->GetScanPower(m_pSystem->OwnerID(), true);
		for(std::map<CString, CShipMap>::const_iterator race = ships.begin(); race != ships.end(); ++race)
			for(CShipMap::const_iterator ship = race->second.begin(); ship != race->second.end(); ++ship)
			{
				if(ship->second->GetTorpedoWeapons()->IsEmpty())
				{
					const boost::shared_ptr<CMajor> owner =
						boost::dynamic_pointer_cast<CMajor>(ship->second->Owner());
					if(owner && !owner->IsHumanPlayer())
						continue;
				}
				if(ship->second->IsAlien())
				{
					const boost::shared_ptr<CRace>& pAlien = ship->second->Owner();
					if(pAlien->GetRaceID() == StrToCStr(KAMPFSTATION))
						return false;
				}
				if(!ship->second->GetCloak() && m_pSystem->GetNeededScanPower(ship->second->OwnerID()) <= scan_power)
					return true;
			}
		return false;
	}

};

bool CSystemManager::CheckEnergyConsumers(CSystem& system)
{
	if(!m_bOnOffline)
		return false;
	CEnergyConsumersChecker checker(system);
	CArray<CBuilding>* buildings = system.GetAllBuildings();
	bool bomb_warning = false;
	bool defense_checked = false;
	const CSystemProd& prod = *system.GetProduction();
	int additional_available_energy = prod.GetAvailableEnergy();
	for(int i = 0; i < buildings->GetSize(); ++i)
	{
		CBuilding& building = buildings->GetAt(i);
		const CBuildingInfo& info = resources::BuildingInfo->GetAt(building.GetRunningNumber() - 1);
		const int needed = info.GetNeededEnergy();
		if(needed == 0)
			continue;
		if(m_IgnoredBuildings.find(info.GetRunningNumber()) != m_IgnoredBuildings.end())
			continue;
		bool should_be_online = building.GetIsBuildingOnline();
		if(info.GetShipYard() || info.GetShipBuildSpeed() > 0)
		{
			AssertBotE(!info.IsDefenseBuilding());
			should_be_online = checker.ShouldTakeShipyardOnline();
		}
		if(info.IsDefenseBuilding())
		{
			AssertBotE(!info.GetShipYard());
			if(bomb_warning)
				should_be_online = true;
			else if(!defense_checked)
			{
				bomb_warning = checker.BomberInSector();
				should_be_online = bomb_warning;
				defense_checked = true;
			}
			else
				should_be_online = false;
		}
		if(info.IsDeritiumRefinery())
			should_be_online = checker.CheckStoreFull(RESOURCES::DERITIUM, info, building);

		if(info.GetResistance() > 0)
			should_be_online = !system.Taken();

		if(info.GetShipTraining() > 0)
			should_be_online = system.GetOwnerOfShip(system.OwnerID(), true);

		if(info.GetTroopTraining() > 0)
			should_be_online = !system.GetTroops()->IsEmpty();

		bool minus_moral = false;
		if(info.GetMoralProd() < 0)
			minus_moral = should_be_online = checker.CheckMoral(info, building, m_iMinMoral, m_iMinMoralProd);

		const WORKER::Typ type = info.ProducesWorkerfull();
		if(type != WORKER::NONE)
		{
			should_be_online = system.HasStore(type) ?
				checker.CheckStoreFull(WorkerToResource(type), info, building) : true;
		}

		should_be_online = should_be_online || info.IsUsefulForProduction();
		should_be_online = should_be_online &&
			(info.OnlyImprovesProduction(minus_moral) ||
				(info.GetShipBuildSpeed() > 0 && checker.ShouldTakeShipyardOnline()));
		should_be_online = should_be_online || info.IsUsefulMoral();

		const bool is_online = building.GetIsBuildingOnline();
		if(should_be_online && !is_online)
		{
			if(additional_available_energy >= needed)
			{
				building.SetIsBuildingOnline(true);
				additional_available_energy -= needed;
				system.CalculateVariables();
			}
		}
		else if (!should_be_online && is_online)
		{
			const int needed = info.GetNeededEnergy();
			building.SetIsBuildingOnline(false);
			additional_available_energy += needed;
			system.CalculateVariables();
		}
	}
	AssertBotE(additional_available_energy >= 0);
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

