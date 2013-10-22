
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
// getting info
//////////////////////////////////////////////////////////////////////

	bool Active() const;
	bool SafeMoral() const;
	bool MaxIndustry() const;
	bool NeglectFood() const;
	bool IndustryPrio() const;
	int MinMoral() const { return m_iMinMoral; }
	int MinMoralProd() const { return m_iMinMoralProd; }
	bool BombWarning() const { return m_bBombWarning; }

	int Priority(WORKER::Typ type) const;

//////////////////////////////////////////////////////////////////////
// setters
//////////////////////////////////////////////////////////////////////

	void SetActive(bool is);
	void SetSafeMoral(bool is);
	void SetMaxIndustry(bool is);
	void SetNeglectFood(bool is);
	void SetIndustryPrio(bool is);
	void SetMinMoral(int value) { m_iMinMoral = value; }
	void SetMinMoralProd(int value) { m_iMinMoralProd = value; }
	void SetBombWarning(int value) { m_bBombWarning = value; }

	void ClearPriorities();

	void AddPriority(WORKER::Typ type, int value);

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

	//@return false in case of an error
	bool DistributeWorkers(CSystem& system, const CPoint& p) const;

	bool CheckEnergyConsumers(CSystem& system, const CPoint& p);

	//@return true in case of expected famine
	static bool CheckFamine(const CSystem& system);

	static bool IsHandledEnergyConsumer(const CBuildingInfo& info);

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
	std::map<WORKER::Typ, int> m_PriorityMap;//1-100 each

};
