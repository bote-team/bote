
/*
 *@file
 */

#pragma once

#include <vector>
#include <map>

#include "Constants.h"

class CSystem;
class CBuildingInfo;

class CSystemManager
{

public:

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

	CSystemManager();
	CSystemManager(const CSystemManager& o);
	virtual ~CSystemManager(void);

	CSystemManager& operator=(const CSystemManager& o);

	void Reset();

//////////////////////////////////////////////////////////////////////
// Serialisierungsfunktionen
//////////////////////////////////////////////////////////////////////

	void Serialize(CArchive& ar);

//////////////////////////////////////////////////////////////////////
// getting
//////////////////////////////////////////////////////////////////////

	bool Active() const;
	bool SafeMoral() const;
	bool MaxIndustry() const;
	bool NeglectFood() const;
	bool IndustryPrio() const;
	int MinMoral() const { return m_iMinMoral; }
	int MinMoralProd() const { return m_iMinMoralProd; }
	bool BombWarning() const { return m_bBombWarning; }
	bool OnOffline() const { return m_bOnOffline; }

	int Priority(WORKER::Typ type) const;

//////////////////////////////////////////////////////////////////////
// setting
//////////////////////////////////////////////////////////////////////

	void SetActive(bool is);
	void SetSafeMoral(bool is);
	void SetMaxIndustry(bool is);
	void SetNeglectFood(bool is);
	void SetIndustryPrio(bool is);
	void SetMinMoral(int value) { m_iMinMoral = value; }
	void SetMinMoralProd(int value) { m_iMinMoralProd = value; }
	void SetBombWarning(int value) { m_bBombWarning = value; }
	void SetOnOffline(bool is) { m_bOnOffline = is; }

	void ClearPriorities();

	void AddPriority(WORKER::Typ type, int value);

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

	//Distributes workers according to priorities and other manager settings
	//@return false in case of an error
	bool DistributeWorkers(CSystem& system) const;

	//Takes energy buildings in the system on/offline
	bool CheckEnergyConsumers(CSystem& system);

	//@return true in case of expected famine
	static bool CheckFamine(const CSystem& system);

//////////////////////////////////////////////////////////////////////
// Members
//////////////////////////////////////////////////////////////////////

	static const int max_priority = 40;
	static const int min_priority = 0;

	static const int max_min_moral = 200;
	static const int min_min_moral = 0;
	static const int max_min_moral_prod = 8;
	static const int min_min_moral_prod = -8;

private:

	bool m_bActive;
	bool m_bSafeMoral;
	bool m_bMaxIndustry;
	bool m_bNeglectFood;
	int m_iMinMoral;
	int m_iMinMoralProd;
	bool m_bBombWarning;
	bool m_bOnOffline;
	std::map<WORKER::Typ, int> m_PriorityMap;//min_priority + 1 till max_priority each, borders included
	//priorities with min_priority are not added

};
