
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

	int Priority(WORKER::Typ type) const;

//////////////////////////////////////////////////////////////////////
// setters
//////////////////////////////////////////////////////////////////////

	void SetActive(bool is);
	void SetSafeMoral(bool is);
	void SetMaxIndustry(bool is);
	void SetNeglectFood(bool is);

	void ClearPriorities(bool refill_with_standard);

	void AddPriority(int value, WORKER::Typ type);

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

	//@return false in case of an error
	bool DistributeWorkers(CSystem& system, const CPoint& p) const;

	void CheckShipyard(CSystem& system) const;

	//@return true in case of expected famine
	bool CheckFamine(const CSystem& system) const;

//////////////////////////////////////////////////////////////////////
// Members
//////////////////////////////////////////////////////////////////////

	static const int max_priority = 100;

private:

	bool m_bActive;
	bool m_bSafeMoral;
	bool m_bMaxIndustry;
	bool m_bNeglectFood;
	std::map<int, WORKER::Typ> m_PriorityMap;//1-100 each

};
