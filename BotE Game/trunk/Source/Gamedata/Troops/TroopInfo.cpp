#include "stdafx.h"
#include "TroopInfo.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CTroopInfo::CTroopInfo(void)
{
	m_strName = "";
	m_strDescription = "";
	m_byMaintenanceCosts = 0;
	memset(m_byNeededTechs, 0, 6 * sizeof(*m_byNeededTechs));
	memset(m_iNeededResources, 0, 5 * sizeof(*m_iNeededResources));
	m_iNeededIndustry = 0;
	m_byID = 0;
	m_sOwner = "";
	m_byPower = 0;
	m_iExperiance = 0;
	m_iSize = 0;
	m_byMoralValue = 0;
}

/// Konstruktor mit kompletter Parameterübergabe
CTroopInfo::CTroopInfo(const CString& name, const CString& desc, BYTE power, BYTE costs, BYTE techs[6], USHORT res[5],
					   USHORT ip, BYTE ID, const CString& sOwner, USHORT size, BYTE moralValue)
{
	m_strName = name;
	m_strDescription = desc;
	m_byMaintenanceCosts = costs;
	for (int i = 0; i < 6; i++)
		m_byNeededTechs[i] = techs[i];
	for (int i = TITAN; i <= IRIDIUM; i++)
		m_iNeededResources[i] = res[i];
	m_iNeededIndustry = ip;
	m_byID = ID;
	m_sOwner = sOwner;
	m_byPower = power;
	m_iExperiance = 0;
	m_iSize = size;
	m_byMoralValue = moralValue;
}

CTroopInfo::~CTroopInfo(void)
{
}

/// Funktion gibt zurück, ob die Truppe mit der aktuellen Forschung einer Rasse baubar ist.
	/// @param researchLevels Forschungsstufen der Rasse
	/// @return Wahrheitswert
bool CTroopInfo::IsThisTroopBuildableNow(const BYTE reserachLevels[6]) const
{
	// zuerstmal die Forschungsstufen checken
	for (int i = 0; i < 6; i++)
		if (reserachLevels[i] < this->m_byNeededTechs[i])
		return false;
	return true;
}

