#pragma once

#include "Botf2Doc.h"

class COldRoundDataCalculator
{
private:
	COldRoundDataCalculator(void);
public:
	~COldRoundDataCalculator(void);

	COldRoundDataCalculator(CBotf2Doc* pDoc);


	static void CreditsDestructionMoral(CMajor* pMajor, CSystem& system,
		const CArray<CBuildingInfo,CBuildingInfo>& BuildingInfo, float fDifficultyLevel);
	static int DeritiumForTheAI(bool human, const CSector& sector, const CSystem& system, float fDifficultyLevel);
	void ExecuteRebellion(CSector& sector, CSystem& system, CMajor* pMajor) const;
	void ExecuteFamine(CSector& sector, CSystem& system, CMajor* pMajor) const;
	void HandlePopulationEffects(const CSector& sector, CSystem& system, CMajor* pMajor) const;
	void SystemMessage(const CSector& sector, CMajor* pMajor, const CString& key,
		MESSAGE_TYPE::Typ message_typ, BYTE byFlag) const;
	void Build(const CSector& sector, CSystem& system, CMajor* pMajor, const CArray<CBuildingInfo,CBuildingInfo>& BuildingInfo) const;

private:
	void FinishBuild(const int to_build, const CSector& sector, CSystem& system, CMajor* pMajor,
		const CArray<CBuildingInfo,CBuildingInfo>& BuildingInfo) const;
private:
	CBotf2Doc* m_pDoc;
};
