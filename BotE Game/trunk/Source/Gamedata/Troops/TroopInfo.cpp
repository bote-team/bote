#include "stdafx.h"
#include "TroopInfo.h"
#include "HTMLStringBuilder.h"
#include "General/Loc.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CTroopInfo::CTroopInfo(void)
{
	m_strName = "";
	m_strDescription = "";
	m_strGraphicfile= "ImageMissing.bop";
	m_byMaintenanceCosts = 0;
	memset(m_byNeededTechs, 0, sizeof(m_byNeededTechs));
	memset(m_iNeededResources, 0, sizeof(m_iNeededResources));
	m_iNeededIndustry = 0;
	m_byID = 0;
	m_sOwner = "";
	m_byOffense = 0;
	m_byDefense = 0;
	m_iExperiance = 0;
	m_iSize = 0;
	m_byMoralValue = 0;
}

/// Konstruktor mit kompletter Parameterübergabe
CTroopInfo::CTroopInfo(const CString& name, const CString& desc, const CString& file, BYTE offense,BYTE defense, BYTE costs, BYTE techs[6], USHORT res[5],
					   USHORT ip, BYTE ID, const CString& sOwner, USHORT size, BYTE moralValue)
{
	m_strName = name;
	m_strDescription = desc;
	m_strGraphicfile = file;
	m_byMaintenanceCosts = costs;
	for (int i = 0; i < 6; i++)
		m_byNeededTechs[i] = techs[i];
	for (int i = TITAN; i <= IRIDIUM; i++)
		m_iNeededResources[i] = res[i];
	m_iNeededIndustry = ip;
	m_byID = ID;
	m_sOwner = sOwner;
	m_byOffense = offense;
	m_byDefense = defense;
	m_iExperiance = 0;
	m_iSize = size;
	m_byMoralValue = moralValue;
}

CTroopInfo::~CTroopInfo(void)
{
}

// Serialisierungsfunktion

void CTroopInfo::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_strName;
		ar << m_strDescription;
		ar << m_strGraphicfile;
		ar << m_byMaintenanceCosts;
		for (int i = 0; i < 6; i++)
		ar << m_byNeededTechs[i];
		for (int i = TITAN; i <= IRIDIUM; i++)
		ar << m_iNeededResources[i];
		ar << m_iNeededIndustry;
		ar << m_byID;
		ar << m_sOwner;
		ar << m_byOffense;
		ar << m_byDefense;
		ar << m_iExperiance;
		ar << m_iSize;
		ar << m_byMoralValue;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_strName;
		ar >> m_strDescription;
		ar >> m_strGraphicfile;
		ar >> m_byMaintenanceCosts;
		for (int i = 0; i < 6; i++)
			ar >> m_byNeededTechs[i];
		for (int i = TITAN; i <= IRIDIUM; i++)
			ar >> m_iNeededResources[i];
		ar >> m_iNeededIndustry;
		ar >> m_byID;
		ar >> m_sOwner;
		ar >> m_byOffense;
		ar >> m_byDefense;
		ar >> m_iExperiance;
		ar >> m_iSize;
		ar >> m_byMoralValue;
	}
}

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

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

/// Funktion erstellt eine Tooltipinfo zur Truppe
/// @return	der erstellte Tooltip-Text
CString CTroopInfo::GetTooltip() const
{
	CString sName = GetName();
	sName = CHTMLStringBuilder::GetHTMLColor(sName);
	sName = CHTMLStringBuilder::GetHTMLHeader(sName, _T("h3"));
	sName = CHTMLStringBuilder::GetHTMLCenter(sName);
	sName += CHTMLStringBuilder::GetHTMLStringNewLine();
	sName += CHTMLStringBuilder::GetHTMLStringNewLine();
		
	CString sValues = "";
	CString s;
	s.Format("%s: %d",CLoc::GetString("OPOWER"), GetOffense());
	sValues += s;
	sValues += CHTMLStringBuilder::GetHTMLStringNewLine();
	s.Format("%s: %d",CLoc::GetString("DPOWER"), GetDefense());
	sValues += s;
	sValues += CHTMLStringBuilder::GetHTMLStringNewLine();
	s.Format("%s: %d",CLoc::GetString("MORALVALUE"), GetMoralValue());
	sValues += s;
	sValues += CHTMLStringBuilder::GetHTMLStringNewLine();
	s.Format("%s: %d",CLoc::GetString("PLACE"), GetSize());
	sValues += s;
	// Unterhaltskosten noch nicht anzeigen, da diese aktuell nicht beachtet werden
	//sValues += CHTMLStringBuilder::GetHTMLStringNewLine();
	//s.Format("%s: %d",CLoc::GetString("MAINTENANCE_COSTS"), GetMaintenanceCosts());
	//sValues += s;
	// Erfahrung nicht anzeigen, da bei den Tooltips aktuell immer auf das Infoobjekt gegangen wird und dieses nie Erfahrung hat
	//sValues += CHTMLStringBuilder::GetHTMLStringNewLine();
	//s.Format("%s: %d",CLoc::GetString("EXPERIANCE"),GetExperiance());
	//sValues += s;
	sValues = CHTMLStringBuilder::GetHTMLColor(sValues);
	sValues = CHTMLStringBuilder::GetHTMLHeader(sValues, _T("h5"));
	sValues += CHTMLStringBuilder::GetHTMLStringNewLine();
	sValues += CHTMLStringBuilder::GetHTMLStringNewLine();
	sValues += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sValues += CHTMLStringBuilder::GetHTMLStringNewLine();

	CString sDesc = GetDescription();
	sDesc = CHTMLStringBuilder::GetHTMLColor(sDesc, _T("silver"));
	sDesc = CHTMLStringBuilder::GetHTMLHeader(sDesc, _T("h5"));
		
	CString sTip = sName + sValues + sDesc;
	return sTip;
}