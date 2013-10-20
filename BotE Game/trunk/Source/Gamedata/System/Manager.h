
/*
 *@file
 */

#pragma once

#include <vector>
#include <map>

#include "Constants.h"

class CSystem;

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

	int Priority(WORKER::Typ type) const;

//////////////////////////////////////////////////////////////////////
// setters
//////////////////////////////////////////////////////////////////////

	void SetActive(bool is);
	void SetSafeMoral(bool is);
	void SetMaxIndustry(bool is);
	void SetNeglectFood(bool is);
	void SetIndustryPrio(bool is);

	void ClearPriorities();

	void AddPriority(WORKER::Typ type, int value);

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

	//@return false in case of an error
	bool DistributeWorkers(CSystem& system, const CPoint& p) const;

	static void CheckShipyard(CSystem& system);

	//@return true in case of expected famine
	static bool CheckFamine(const CSystem& system);

//////////////////////////////////////////////////////////////////////
// Members
//////////////////////////////////////////////////////////////////////

	static const int max_priority = 10;
	static const int min_priority = 0;

private:

	bool m_bActive;
	bool m_bSafeMoral;
	bool m_bMaxIndustry;
	bool m_bNeglectFood;
	bool m_bIndustryPrio;
	std::map<WORKER::Typ, int> m_PriorityMap;//1-100 each

};
