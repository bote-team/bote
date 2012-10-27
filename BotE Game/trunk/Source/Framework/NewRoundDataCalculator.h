#pragma once

#include "Botf2Doc.h"

class CNewRoundDataCalculator
{
private:
	CNewRoundDataCalculator(void);
public:
	~CNewRoundDataCalculator(void);

	CNewRoundDataCalculator(CBotf2Doc* pDoc);


	static void GetIntelligenceBoniFromSpecialTechsAndSetThem(const std::map<CString, CMajor*>* pmMajors);

	//Function checks, whether trade and resource routes starting in this system are still valid.
	//If not, removes them and queues messages about that.
	void CheckRoutes(const CSector& sector, CSystem& system, CMajor* pMajor);

	static void CalcIntelligenceBoni(const CSystemProd* production, CIntelligence* intelligence);
	static void CalcMoral(const CSector& sector, CSystem& system, CArray<CTroopInfo>& TroopInfo);

	//Function calculates and transfers additional knowledge about the given sector, scan powers and
	//movement range due to diplomatic contracts from minor races or other major ones.
	static void CalcExtraVisibilityAndRangeDueToDiplomacy(
		CSector& sector, const std::map<CString, CMajor*>* pMajors, const std::map<CString, CMinor*>* pMinors);

	//Function calculates possible effects from sectors or systems which are iterated upon later
	//onto earlier sectors or systems. Such as moral empire wide, which influences
	//all systems starting from (0,0).
	//These effects have effects onto other things which are calculated later,
	//the second time it is iterated over all sectors/systems (after CalcPreLoop())
	void CalcPreLoop();

private:
	CBotf2Doc* m_pDoc;
};