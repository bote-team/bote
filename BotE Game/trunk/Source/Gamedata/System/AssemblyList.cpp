#include "stdafx.h"
#include "AssemblyList.h"
#include "System.h"

IMPLEMENT_SERIAL (CAssemblyList, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CAssemblyList::CAssemblyList(void)
{
	Reset();
}

CAssemblyList::~CAssemblyList(void)
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CAssemblyList::CAssemblyList(const CAssemblyList & rhs)
{
	m_iNeededIndustryForBuild = rhs.m_iNeededIndustryForBuild;
	m_iNeededTitanForBuild = rhs.m_iNeededTitanForBuild;
	m_iNeededDeuteriumForBuild = rhs.m_iNeededDeuteriumForBuild;
	m_iNeededDuraniumForBuild = rhs.m_iNeededDuraniumForBuild;
	m_iNeededCrystalForBuild = rhs.m_iNeededCrystalForBuild;
	m_iNeededIridiumForBuild = rhs.m_iNeededIridiumForBuild;
	m_iNeededDilithiumForBuild = rhs.m_iNeededDilithiumForBuild;
	m_bWasBuildingBought = rhs.m_bWasBuildingBought;
	m_iBuildCosts = rhs.m_iBuildCosts;
	for (int i = 0; i < ALE; i++)
	{
		m_iEntry[i] = rhs.m_iEntry[i];
		m_iNeededIndustryInAssemblyList[i] = rhs.m_iNeededIndustryInAssemblyList[i];
		m_iNeededTitanInAssemblyList[i] = rhs.m_iNeededTitanInAssemblyList[i];
		m_iNeededDeuteriumInAssemblyList[i] = rhs.m_iNeededDeuteriumInAssemblyList[i];
		m_iNeededDuraniumInAssemblyList[i] = rhs.m_iNeededDuraniumInAssemblyList[i];
		m_iNeededCrystalInAssemblyList[i] = rhs.m_iNeededCrystalInAssemblyList[i];
		m_iNeededIridiumInAssemblyList[i] = rhs.m_iNeededIridiumInAssemblyList[i];
		m_iNeededDilithiumInAssemblyList[i] = rhs.m_iNeededDilithiumInAssemblyList[i];
	}
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CAssemblyList & CAssemblyList::operator=(const CAssemblyList & rhs)
{
	if (this == &rhs)
		return *this;
	m_iNeededIndustryForBuild = rhs.m_iNeededIndustryForBuild;
	m_iNeededTitanForBuild = rhs.m_iNeededTitanForBuild;
	m_iNeededDeuteriumForBuild = rhs.m_iNeededDeuteriumForBuild;
	m_iNeededDuraniumForBuild = rhs.m_iNeededDuraniumForBuild;
	m_iNeededCrystalForBuild = rhs.m_iNeededCrystalForBuild;
	m_iNeededIridiumForBuild = rhs.m_iNeededIridiumForBuild;
	m_iNeededDilithiumForBuild = rhs.m_iNeededDilithiumForBuild;
	m_bWasBuildingBought = rhs.m_bWasBuildingBought;
	m_iBuildCosts = rhs.m_iBuildCosts;
	for (int i = 0; i < ALE; i++)
	{
		m_iEntry[i] = rhs.m_iEntry[i];
		m_iNeededIndustryInAssemblyList[i] = rhs.m_iNeededIndustryInAssemblyList[i];
		m_iNeededTitanInAssemblyList[i] = rhs.m_iNeededTitanInAssemblyList[i];
		m_iNeededDeuteriumInAssemblyList[i] = rhs.m_iNeededDeuteriumInAssemblyList[i];
		m_iNeededDuraniumInAssemblyList[i] = rhs.m_iNeededDuraniumInAssemblyList[i];
		m_iNeededCrystalInAssemblyList[i] = rhs.m_iNeededCrystalInAssemblyList[i];
		m_iNeededIridiumInAssemblyList[i] = rhs.m_iNeededIridiumInAssemblyList[i];
		m_iNeededDilithiumInAssemblyList[i] = rhs.m_iNeededDilithiumInAssemblyList[i];
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CAssemblyList::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		for (int i = 0; i < ALE; i++)
		{
			ar << m_iEntry[i];
			// Variablen geben die noch verbleibenden Kosten der Elemente in der Bauliste an
			ar << m_iNeededIndustryInAssemblyList[i];
			ar << m_iNeededTitanInAssemblyList[i];	
			ar << m_iNeededDeuteriumInAssemblyList[i];
			ar << m_iNeededDuraniumInAssemblyList[i];	
			ar << m_iNeededCrystalInAssemblyList[i];	
			ar << m_iNeededIridiumInAssemblyList[i];	
			ar << m_iNeededDilithiumInAssemblyList[i];
		}
		// Variablen, die Angeben, wieviel Industrie und Rohstoffe zum Bau benötigt werden
		ar << m_iNeededIndustryForBuild;			
		ar << m_iNeededTitanForBuild;				
		ar << m_iNeededDeuteriumForBuild;			
		ar << m_iNeededDuraniumForBuild;			
		ar << m_iNeededCrystalForBuild;				
		ar << m_iNeededIridiumForBuild;				
		ar << m_iNeededDilithiumForBuild;			
		// Wurde das Gebäude gekauft in dieser Runde gekauft
		ar << m_bWasBuildingBought;			
		// Die Baukosten eines Auftrages
		ar << m_iBuildCosts;	
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		for (int i = 0; i < ALE; i++)
		{
			ar >> m_iEntry[i];
			// Variablen geben die noch verbleibenden Kosten der Elemente in der Bauliste an
			ar >> m_iNeededIndustryInAssemblyList[i];
			ar >> m_iNeededTitanInAssemblyList[i];	
			ar >> m_iNeededDeuteriumInAssemblyList[i];
			ar >> m_iNeededDuraniumInAssemblyList[i];	
			ar >> m_iNeededCrystalInAssemblyList[i];	
			ar >> m_iNeededIridiumInAssemblyList[i];	
			ar >> m_iNeededDilithiumInAssemblyList[i];
		}
		// Variablen, die Angeben, wieviel Industrie und Rohstoffe zum Bau benötigt werden
		ar >> m_iNeededIndustryForBuild;			
		ar >> m_iNeededTitanForBuild;				
		ar >> m_iNeededDeuteriumForBuild;			
		ar >> m_iNeededDuraniumForBuild;			
		ar >> m_iNeededCrystalForBuild;				
		ar >> m_iNeededIridiumForBuild;				
		ar >> m_iNeededDilithiumForBuild;			
		// Wurde das Gebäude gekauft in dieser Runde gekauft
		ar >> m_bWasBuildingBought;			
		// Die Baukosten eines Auftrages
		ar >> m_iBuildCosts;	
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion gibt die jeweiligen verbleibenden Kosten für einen Bauauftrag zurück.
UINT CAssemblyList::GetNeededResourceInAssemblyList(USHORT entry, BYTE res) const
{
	switch (res)
	{
	case TITAN:			return this->GetNeededTitanInAssemblyList(entry);
	case DEUTERIUM:		return this->GetNeededDeuteriumInAssemblyList(entry);
	case DURANIUM:		return this->GetNeededDuraniumInAssemblyList(entry);
	case CRYSTAL:		return this->GetNeededCrystalInAssemblyList(entry);
	case IRIDIUM:		return this->GetNeededIridiumInAssemblyList(entry);
	case DILITHIUM:		return this->GetNeededDilithiumInAssemblyList(entry);
	default:			return NULL;
	}
}

// Funktion gibt die jeweiligen Kosten für einen Bauauftrag zurück.
UINT CAssemblyList::GetNeededResourceForBuild(BYTE res) const
{
	switch (res)
	{
	case TITAN:			return this->GetNeededTitanForBuild();
	case DEUTERIUM:		return this->GetNeededDeuteriumForBuild();
	case DURANIUM:		return this->GetNeededDuraniumForBuild();
	case CRYSTAL:		return this->GetNeededCrystalForBuild();
	case IRIDIUM:		return this->GetNeededIridiumForBuild();
	case DILITHIUM:		return this->GetNeededDilithiumForBuild();
	default:			return NULL;
	}	
}

// Funktion berechnet die benötigten Rohstoffe. Übergeben wird die Information des gewünschten Gebäudes bzw. des
// gewünschten Schiffes oder der gewünschten Truppe, der RunningNumber des Bauauftrages, einen Zeiger auf alle
// vorhandenen Gebäude in dem System und ein Zeiger auf die Forschung
void CAssemblyList::CalculateNeededRessources(CBuildingInfo* buildingInfo, CShipInfo* shipInfo, CTroopInfo* troopInfo,
											  BuildingArray* m_Buildings, int RunningNumber, CResearchInfo* ResearchInfo, float modifier)
{
	// Wenn ein Update, dann muß die RunningNumber des buildingInfo positiv sein und eins größer
	// -> dies beim Aufruf dieser Funktion beachten
	if (RunningNumber > 0 && RunningNumber < 10000)		// Also kein Update, sondern ein echtes Gebäude
	{
		m_iNeededIndustryForBuild = buildingInfo->GetNeededIndustry();
		m_iNeededTitanForBuild = buildingInfo->GetNeededTitan();
		m_iNeededDeuteriumForBuild = buildingInfo->GetNeededDeuterium();
		m_iNeededDuraniumForBuild = buildingInfo->GetNeededDuranium();
		m_iNeededCrystalForBuild = buildingInfo->GetNeededCrystal();
		m_iNeededIridiumForBuild = buildingInfo->GetNeededIridium();
		m_iNeededDilithiumForBuild = buildingInfo->GetNeededDilithium();
	}
	else if (RunningNumber < 0)	// Wollen also ein Update machen
	{
		// Berechnen, wieviel Rohstoffe und Industry wir benötigen
		int NumberOfBuildings = m_Buildings->GetSize();
		RunningNumber *= (-1);
		m_iNeededIndustryForBuild = 0;
		m_iNeededTitanForBuild = 0;
		m_iNeededDeuteriumForBuild = 0;
		m_iNeededDuraniumForBuild = 0;
		m_iNeededCrystalForBuild = 0;
		m_iNeededIridiumForBuild = 0;
		m_iNeededDilithiumForBuild = 0;
		for (int i = 0; i < NumberOfBuildings; i++)
		{
			if (m_Buildings->GetAt(i).GetRunningNumber() == buildingInfo->GetPredecessorID())
			{
				// zur Zeit 80% der Rohstoffe aller Gebäude des Typs aufbringen
				// und 60% der Industrie
				m_iNeededTitanForBuild += (int)(buildingInfo->GetNeededTitan()*0.8);
				m_iNeededDeuteriumForBuild += (int)(buildingInfo->GetNeededDeuterium()*0.8);
				m_iNeededDuraniumForBuild += (int)(buildingInfo->GetNeededDuranium()*0.8);
				m_iNeededCrystalForBuild += (int)(buildingInfo->GetNeededCrystal()*0.8);
				m_iNeededIridiumForBuild += (int)(buildingInfo->GetNeededIridium()*0.8);
				m_iNeededDilithiumForBuild += (int)(buildingInfo->GetNeededDilithium()*0.8);
				m_iNeededIndustryForBuild += (int)(buildingInfo->GetNeededIndustry()*0.6);
			}
		}
	}
	else if (RunningNumber >= 10000 && RunningNumber < 20000)	// Es handelt sich um ein Schiff
	{
		m_iNeededIndustryForBuild = shipInfo->GetNeededIndustry();
		m_iNeededTitanForBuild = shipInfo->GetNeededTitan();
		m_iNeededDeuteriumForBuild = shipInfo->GetNeededDeuterium();
		m_iNeededDuraniumForBuild = shipInfo->GetNeededDuranium();
		m_iNeededCrystalForBuild = shipInfo->GetNeededCrystal();
		m_iNeededIridiumForBuild = shipInfo->GetNeededIridium();
		m_iNeededDilithiumForBuild = shipInfo->GetNeededDilithium();
		// hier auch noch den eventuellen Bonus durch die Uniqueforschung "Allgemeine Schifftechnik"
		if (ResearchInfo->GetResearchComplex(2)->GetFieldStatus(3) == RESEARCHED)	// 2 -> Allgemeine Schiffstechnik
		{
			m_iNeededIndustryForBuild	-= (int)(ResearchInfo->GetResearchComplex(2)->GetBonus(3)*m_iNeededIndustryForBuild/100);
			m_iNeededTitanForBuild		-= (int)(ResearchInfo->GetResearchComplex(2)->GetBonus(3)*m_iNeededTitanForBuild/100);
			m_iNeededDeuteriumForBuild	-= (int)(ResearchInfo->GetResearchComplex(2)->GetBonus(3)*m_iNeededDeuteriumForBuild/100);
			m_iNeededDuraniumForBuild	-= (int)(ResearchInfo->GetResearchComplex(2)->GetBonus(3)*m_iNeededDuraniumForBuild/100);
			m_iNeededCrystalForBuild	-= (int)(ResearchInfo->GetResearchComplex(2)->GetBonus(3)*m_iNeededCrystalForBuild/100);
			m_iNeededIridiumForBuild	-= (int)(ResearchInfo->GetResearchComplex(2)->GetBonus(3)*m_iNeededIridiumForBuild/100);
		}
		// hier auch noch den eventuellen Bonus durch die Uniqueforschung "friedliche Schifftechnik"
		// aber nur wenn es sich um ein Transportschiff oder Kolonieschiff handelt
		if (ResearchInfo->GetResearchComplex(3)->GetFieldStatus(3) == RESEARCHED && shipInfo->GetShipType() <= COLONYSHIP) 	// 3 -> Allgemeine Schiffstechnik
		{
			m_iNeededIndustryForBuild	-= (int)(ResearchInfo->GetResearchComplex(3)->GetBonus(3)*m_iNeededIndustryForBuild/100);
			m_iNeededTitanForBuild		-= (int)(ResearchInfo->GetResearchComplex(3)->GetBonus(3)*m_iNeededTitanForBuild/100);
			m_iNeededDeuteriumForBuild	-= (int)(ResearchInfo->GetResearchComplex(3)->GetBonus(3)*m_iNeededDeuteriumForBuild/100);
			m_iNeededDuraniumForBuild	-= (int)(ResearchInfo->GetResearchComplex(3)->GetBonus(3)*m_iNeededDuraniumForBuild/100);
			m_iNeededCrystalForBuild	-= (int)(ResearchInfo->GetResearchComplex(3)->GetBonus(3)*m_iNeededCrystalForBuild/100);
			m_iNeededIridiumForBuild	-= (int)(ResearchInfo->GetResearchComplex(3)->GetBonus(3)*m_iNeededIridiumForBuild/100);
		}
	}
	else if (RunningNumber >= 20000)	// es handelt sich um Truppen
	{
		m_iNeededIndustryForBuild = troopInfo->GetNeededIndustry();
		m_iNeededTitanForBuild = troopInfo->GetNeededResources()[TITAN];
		m_iNeededDeuteriumForBuild = troopInfo->GetNeededResources()[DEUTERIUM];
		m_iNeededDuraniumForBuild = troopInfo->GetNeededResources()[DURANIUM];
		m_iNeededCrystalForBuild = troopInfo->GetNeededResources()[CRYSTAL];
		m_iNeededIridiumForBuild = troopInfo->GetNeededResources()[IRIDIUM];
		m_iNeededDilithiumForBuild = NULL;
		// hier auch noch den eventuellen Bonus durch die Uniqueforschung "Truppen"
		if (ResearchInfo->GetResearchComplex(4)->GetFieldStatus(3) == RESEARCHED)	// 4 -> Truppen
		{
			m_iNeededIndustryForBuild	-= (int)(ResearchInfo->GetResearchComplex(4)->GetBonus(3)*m_iNeededIndustryForBuild/100);
			m_iNeededTitanForBuild		-= (int)(ResearchInfo->GetResearchComplex(4)->GetBonus(3)*m_iNeededTitanForBuild/100);
			m_iNeededDeuteriumForBuild	-= (int)(ResearchInfo->GetResearchComplex(4)->GetBonus(3)*m_iNeededDeuteriumForBuild/100);
			m_iNeededDuraniumForBuild	-= (int)(ResearchInfo->GetResearchComplex(4)->GetBonus(3)*m_iNeededDuraniumForBuild/100);
			m_iNeededCrystalForBuild	-= (int)(ResearchInfo->GetResearchComplex(4)->GetBonus(3)*m_iNeededCrystalForBuild/100);
			m_iNeededIridiumForBuild	-= (int)(ResearchInfo->GetResearchComplex(4)->GetBonus(3)*m_iNeededIridiumForBuild/100);
		}
	}
	// hier noch den eventuellen Bonus durch die Uniqueforschung "Wirtschaft"
	if (ResearchInfo->GetResearchComplex(5)->GetFieldStatus(2) == RESEARCHED)	// 5 -> Wirtschaft
	{
		m_iNeededIndustryForBuild	-= (int)(ResearchInfo->GetResearchComplex(5)->GetBonus(2)*m_iNeededIndustryForBuild/100);
		m_iNeededTitanForBuild		-= (int)(ResearchInfo->GetResearchComplex(5)->GetBonus(2)*m_iNeededTitanForBuild/100);
		m_iNeededDeuteriumForBuild	-= (int)(ResearchInfo->GetResearchComplex(5)->GetBonus(2)*m_iNeededDeuteriumForBuild/100);
		m_iNeededDuraniumForBuild	-= (int)(ResearchInfo->GetResearchComplex(5)->GetBonus(2)*m_iNeededDuraniumForBuild/100);
		m_iNeededCrystalForBuild	-= (int)(ResearchInfo->GetResearchComplex(5)->GetBonus(2)*m_iNeededCrystalForBuild/100);
		m_iNeededIridiumForBuild	-= (int)(ResearchInfo->GetResearchComplex(5)->GetBonus(2)*m_iNeededIridiumForBuild/100);
	}
	// Modifikator noch mit einrechnen
	m_iNeededIndustryForBuild	= (UINT)(m_iNeededIndustryForBuild * modifier);
	m_iNeededTitanForBuild		= (UINT)(m_iNeededTitanForBuild * modifier);
	m_iNeededDeuteriumForBuild	= (UINT)(m_iNeededDeuteriumForBuild * modifier);
	m_iNeededDuraniumForBuild	= (UINT)(m_iNeededDuraniumForBuild * modifier);
	m_iNeededCrystalForBuild	= (UINT)(m_iNeededCrystalForBuild * modifier);
	m_iNeededIridiumForBuild	= (UINT)(m_iNeededIridiumForBuild * modifier);
	m_iNeededDilithiumForBuild	= (UINT)(m_iNeededDilithiumForBuild * modifier);
}

// Funktion berechnet die Kosten für jedes Update in der Bauliste. Wird in der NextRound() Funktion aufgerufen.
// Weil in der Zwischenzeit hätte der Spieler ein Gebäude bauen können, das die Kosten mancher Updates verändern
// könnte. Übergeben wird ein Zeiger auf das Feld aller Gebäudeinformationen, ein Zeiger auf alle stehenden
// Gebäude im System und ein Zeiger auf die Forschungsinformation.
void CAssemblyList::CalculateNeededRessourcesForUpdate(BuildingInfoArray* follower, BuildingArray* m_Buildings, CResearchInfo* ResearchInfo)
{
	int NumberOfBuildings = m_Buildings->GetSize();
	for (int j = 0; j < ALE; j++)
		if (m_iEntry[j] < 0)
		{
			// Runningnumber des Gebäudes, welches hier geupdated werden soll
			// dies wäre der Vorgänger der RunningNumber die in der Bauliste steht
			USHORT RunningNumber = follower->GetAt(follower->GetAt(m_iEntry[j]*(-1)-1).GetPredecessorID()-1).GetRunningNumber();
			m_iNeededIndustryForBuild	= 0;
			m_iNeededTitanForBuild		= 0;
			m_iNeededDeuteriumForBuild	= 0;
			m_iNeededDuraniumForBuild	= 0;
			m_iNeededCrystalForBuild	= 0;
			m_iNeededIridiumForBuild	= 0;
			m_iNeededDilithiumForBuild	= 0;
			for (int i = 0; i < NumberOfBuildings; i++)
				if (m_Buildings->GetAt(i).GetRunningNumber() == RunningNumber)
				{
					// zur Zeit 80% der Rohstoffe aller Gebäude des Typs aufbringen und 60% der Industrie
					// Kosten sind die prozentualen des Nachfolgers
					m_iNeededTitanForBuild		+= (int)(follower->GetAt(m_iEntry[j]*(-1)-1).GetNeededTitan()*0.8);
					m_iNeededDeuteriumForBuild	+= (int)(follower->GetAt(m_iEntry[j]*(-1)-1).GetNeededDeuterium()*0.8);
					m_iNeededDuraniumForBuild	+= (int)(follower->GetAt(m_iEntry[j]*(-1)-1).GetNeededDuranium()*0.8);
					m_iNeededCrystalForBuild	+= (int)(follower->GetAt(m_iEntry[j]*(-1)-1).GetNeededCrystal()*0.8);
					m_iNeededIridiumForBuild	+= (int)(follower->GetAt(m_iEntry[j]*(-1)-1).GetNeededIridium()*0.8);
					m_iNeededIndustryForBuild	+= (int)(follower->GetAt(m_iEntry[j]*(-1)-1).GetNeededIndustry()*0.6);
					// Total Cost == (Cost of Level X+1 Building) - ((Cost of Level X Building) * Recycle factor)
					
				}
			// hier noch den eventuellen Bonus durch die Uniqueforschung "Wirtschaft"
			if (ResearchInfo->GetResearchComplex(5)->GetFieldStatus(2) == RESEARCHED)	// 5 -> Wirtschaft
			{
				m_iNeededIndustryForBuild	-= (int)(ResearchInfo->GetResearchComplex(5)->GetBonus(2)*m_iNeededIndustryForBuild/100);
				m_iNeededTitanForBuild		-= (int)(ResearchInfo->GetResearchComplex(5)->GetBonus(2)*m_iNeededTitanForBuild/100);
				m_iNeededDeuteriumForBuild	-= (int)(ResearchInfo->GetResearchComplex(5)->GetBonus(2)*m_iNeededDeuteriumForBuild/100);
				m_iNeededDuraniumForBuild	-= (int)(ResearchInfo->GetResearchComplex(5)->GetBonus(2)*m_iNeededDuraniumForBuild/100);
				m_iNeededCrystalForBuild	-= (int)(ResearchInfo->GetResearchComplex(5)->GetBonus(2)*m_iNeededCrystalForBuild/100);
				m_iNeededIridiumForBuild	-= (int)(ResearchInfo->GetResearchComplex(5)->GetBonus(2)*m_iNeededIridiumForBuild/100);
			}
			// Wenn an der ersten Stelle der Auftragsliste ein Upgrade steht, kann man die Berechnung nicht so einfach
			// durchführen, als hätte man es erst reingestellt. Denn dann würde der Bau nicht voranschreiten.
			// Man muss beachten, dass schon an diesem Auftrag gebaut wurde. Zuviel aufgebrachte Ressourcen bekommt man
			// aber nicht zurück.
			if (j == 0)
			{
				if ((ULONG)m_iNeededIndustryInAssemblyList[j] > m_iNeededIndustryForBuild)
					m_iNeededIndustryInAssemblyList[j]	= m_iNeededIndustryForBuild;
				if (m_iNeededTitanInAssemblyList[j]		> m_iNeededTitanForBuild)
					m_iNeededTitanInAssemblyList[j]		= m_iNeededTitanForBuild;
				if (m_iNeededDeuteriumInAssemblyList[j] > m_iNeededDeuteriumForBuild)
					m_iNeededDeuteriumInAssemblyList[j] = m_iNeededDeuteriumForBuild;
				if (m_iNeededDuraniumInAssemblyList[j]	> m_iNeededDuraniumForBuild)
					m_iNeededDuraniumInAssemblyList[j]	= m_iNeededDuraniumForBuild;
				if (m_iNeededCrystalInAssemblyList[j]	> m_iNeededCrystalForBuild)
					m_iNeededCrystalInAssemblyList[j]	= m_iNeededCrystalForBuild;
				if (m_iNeededIridiumInAssemblyList[j]	> m_iNeededIridiumForBuild)
					m_iNeededIridiumInAssemblyList[j]	= m_iNeededIridiumForBuild;
			}
			// Wenn der Auftrag nicht an erster Stelle in der Auftragsliste steht, dann kann man jedes Mal die
			// Ressourcen wieder von vorn berechnne lassen. Also so als hätten wir dieses Upgrade gerade reingestellt.
			else
			{
				m_iNeededIndustryInAssemblyList[j]	= m_iNeededIndustryForBuild;
				m_iNeededTitanInAssemblyList[j]		= m_iNeededTitanForBuild;
				m_iNeededDeuteriumInAssemblyList[j] = m_iNeededDeuteriumForBuild;
				m_iNeededDuraniumInAssemblyList[j]	= m_iNeededDuraniumForBuild;
				m_iNeededCrystalInAssemblyList[j]	= m_iNeededCrystalForBuild;
				m_iNeededIridiumInAssemblyList[j]	= m_iNeededIridiumForBuild;
			}
		}
}

// Diese Funktion entfernt die benötigten Ressourcen aus dem lokalen Lager des Systems und falls Ressourcenrouten
// bestehen auch die Ressourcen in den Startsystemen der Route. Aber nur falls dies auch notwendig sein sollte.
void CAssemblyList::RemoveResourceFromStorage(BYTE res, const CPoint &ko, CSystem systems[][STARMAP_SECTORS_VCOUNT], CArray<CPoint>* routesFrom)
{
	if (ko == CPoint(-1,-1))
		return;

	CSystem *system = &systems[ko.x][ko.y];
	
	// für Deritium gibt es keine Ressourcenroute
	if (res != DILITHIUM)
	{
		// zuerst wird immer versucht, die Ressourcen aus dem lokalen Lager zu nehmen
		long remainingRes = GetNeededResourceInAssemblyList(0, res) - system->GetRessourceStore(res);
		// werden zusätzliche Ressourcen aus anderen Lagern benötigt, so kann das lokale Lager
		// auf NULL gesetzt werden
		if (remainingRes > 0)
		{
			*system->GetRessourceStorages(res) = NULL;
			// zusätzliche Ressourcen müssen aus den Lagern der Systeme mit den Ressourcenrouten
			// bezogen werden. Dafür ein Feld anlegen, indem alle Startsysteme mit der zur Ressouce passenden
			// Ressourcenroute beinhaltet sind.
			struct ROUTELIST {
				CResourceRoute *route;
				CPoint fromSystem;
				
				ROUTELIST() : route(0), fromSystem(0) {}
				ROUTELIST(CResourceRoute *_route, CPoint _fromSystem) : route(_route), fromSystem(_fromSystem) {}
			};			
			CArray<ROUTELIST> routes;			
			for (int j = 0; j < routesFrom->GetSize(); j++)
			{
				CPoint p = routesFrom->GetAt(j);
				for (int k = 0; k < systems[p.x][p.y].GetResourceRoutes()->GetSize(); k++)
					if (systems[p.x][p.y].GetResourceRoutes()->GetAt(k).GetResource() == res)
					{
						// prüfen das die Route nicht schon verwendet wird
						bool bUsed = false;
						for (int l = 0; l < routes.GetSize(); l++)
							if (routes.GetAt(l).fromSystem == p)
							{
								bUsed = true;
								break;
							}
						if (!bUsed)
							routes.Add(ROUTELIST(&systems[p.x][p.y].GetResourceRoutes()->ElementAt(k), p));						
					}
			}
			// in routes sind nun die Zeiger auf die richtigen Ressourcenrouten, also die Routen, welche auch den
			// passenden Rohstoff liefern könnten.
			while (routes.GetSize())
			{
				// zufällig eine Route aus den möglichen auswählen, damit nicht immer das gleiche System zuerst
				// die Rohstoffe liefern muss, falls mehrere Routen der selben Art ins System eingehen.
				int random = rand()%routes.GetSize();
				int percent = NULL;
				CPoint start = routes.GetAt(random).fromSystem;
				// sind im jeweiligen Lager des Startsystem genügend Rohstoffe vorhanden
				if (systems[start.x][start.y].GetRessourceStore(res) >= (ULONG)remainingRes)
				{
					*systems[start.x][start.y].GetRessourceStorages(res) -= remainingRes;
					if (GetNeededResourceInAssemblyList(0, res) > NULL)
						percent = 100 * remainingRes / GetNeededResourceInAssemblyList(0, res);
					routes.GetAt(random).route->SetPercent((BYTE)percent);
					remainingRes = 0;
				}
				else
				{
					remainingRes -= systems[start.x][start.y].GetRessourceStore(res);
					if (GetNeededResourceInAssemblyList(0, res) > NULL)
						percent = 100 * systems[start.x][start.y].GetRessourceStore(res) / GetNeededResourceInAssemblyList(0, res);
					routes.GetAt(random).route->SetPercent((BYTE)percent);
					*systems[start.x][start.y].GetRessourceStorages(res) = NULL;
				}
				// ROUTELIST Eintrag entfernen, wenn dieser abgearbeitet wurde
				routes.RemoveAt(random);				
			}
			ASSERT(remainingRes == NULL);
		}
		// anderenfalls werden nur die benötigten Ressourcen aus dem lokalen Lager abgezogen
		else
			*system->GetRessourceStorages(res) -= GetNeededResourceInAssemblyList(0, res);
	}
	else
		*system->GetRessourceStorages(res) -= m_iNeededDilithiumInAssemblyList[0];
}

BOOLEAN CAssemblyList::MakeEntry(int runningNumber, const CPoint &ko, CSystem systems[][STARMAP_SECTORS_VCOUNT])
{
	// Die Assemblylist durchgehen, ob wir einen Eintrag finden, der noch 0 ist
	// dort können wir den nächsten speichern, gibt es keinen, dann ist die
	// Bauliste voll
	int entry = ALE + 1;		// Wert der in for-Schleife nicht angenommen werden kann
	for (int i = 0; i < ALE; i++)
		if (m_iEntry[i] == 0)
		{	
			entry = i;
			break;
		}
	if (entry == ALE+1)
		return FALSE;	// Bauliste ist schon voll!
	
	CSystem* system = &systems[ko.x][ko.y];
	// Ressourcenrouten durchgehen und womöglich die möglichen max. zusätzlichen Ressourcen erfragen
	CArray<CPoint> routesFrom;
	ULONG resourcesFromRoutes[DILITHIUM + 1];
	ULONG nResInDistSys[DILITHIUM + 1];
	CPoint ptResourceDistributorKOs[DILITHIUM + 1];

	for (int i = 0; i <= DILITHIUM; i++)
	{
		resourcesFromRoutes[i] = 0;
		nResInDistSys[i] = 0;
		ptResourceDistributorKOs[i] = CPoint(-1,-1);		
	}
	
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
	{
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			if (systems[x][y].GetOwnerOfSystem() == system->GetOwnerOfSystem() && CPoint(x,y) != ko)
			{
				for (int i = 0; i < systems[x][y].GetResourceRoutes()->GetSize(); i++)
				{
					if (systems[x][y].GetResourceRoutes()->GetAt(i).GetKO() == ko)
					{
						if (systems[x][y].GetBlockade() == NULL && systems[ko.x][ko.y].GetBlockade() == NULL)
						{
							routesFrom.Add(CPoint(x,y));
							BYTE res = systems[x][y].GetResourceRoutes()->GetAt(i).GetResource();
							resourcesFromRoutes[res] += systems[x][y].GetRessourceStore(res);
						}
					}
				}
				// gilt nicht bei Blockaden
				if (systems[x][y].GetBlockade() == NULL && systems[ko.x][ko.y].GetBlockade() == NULL)
				{
					for (int res = TITAN; res <= DILITHIUM; res++)
					{
						if (systems[x][y].GetProduction()->GetResourceDistributor(res))
						{
							ptResourceDistributorKOs[res] = CPoint(x,y);
							nResInDistSys[res] = systems[x][y].GetRessourceStore(res);
						}
					}
				}
			}			
		}
	}
	// Überprüfen, ob wir genügend Rohstoffe in dem Lager haben
	for (int res = TITAN; res <= DILITHIUM; res++)
	{
		UINT nNeededRes = this->GetNeededResourceForBuild(res);
		if (*system->GetRessourceStorages(res) + resourcesFromRoutes[res] < nNeededRes && nResInDistSys[res] < nNeededRes)
			return FALSE;
	}
	
	// Ansonsten gibt es genügend Rohstoffe
	m_iEntry[entry] = runningNumber;
	// Was wir für das notwendige Projekt alles so brauchen speichern
	m_iNeededIndustryInAssemblyList[entry] = m_iNeededIndustryForBuild;
	m_iNeededTitanInAssemblyList[entry]	   = m_iNeededTitanForBuild;
	m_iNeededDeuteriumInAssemblyList[entry]= m_iNeededDeuteriumForBuild;
	m_iNeededDuraniumInAssemblyList[entry] = m_iNeededDuraniumForBuild;
	m_iNeededCrystalInAssemblyList[entry]  = m_iNeededCrystalForBuild;
	m_iNeededIridiumInAssemblyList[entry]  = m_iNeededIridiumForBuild;
	m_iNeededDilithiumInAssemblyList[entry]= m_iNeededDilithiumForBuild;
	// Nur wenn es der erste Eintrag im Baumenü ist wird alles abgezogen
	// ansonsten erst, nachdem das Projekt im ersten Eintrag fertig ist
	if (entry == 0)
	{
		for (int res = TITAN; res <= DILITHIUM; res++)
		{
			UINT nNeededRes = this->GetNeededResourceForBuild(res);
			if (nNeededRes > 0)
			{
				// Ressource wird aus eigenem System bzw. über Ressourcenroute geholt
				if (*system->GetRessourceStorages(res) + resourcesFromRoutes[res] >= nNeededRes)
					RemoveResourceFromStorage(res, ko, systems, &routesFrom);
				// reicht das nicht, so wird Ressource aus dem Verteier geholt
				else
				{
					CArray<CPoint> vNullRoutes;
					RemoveResourceFromStorage(res, ptResourceDistributorKOs[res], systems, &vNullRoutes);
				}
			}
		}		
	}

	// Eintrag konnte gesetzt werden
	return TRUE;
}

// Funktion berechnet die Kosten des Bauauftrags, wenn man dieses sofort kaufen will.
// Die Kosten des Sofortkaufes sind von den Marktpreisen abhängig.
void CAssemblyList::CalculateBuildCosts(USHORT resPrices[5])
{
	if (m_iNeededIndustryInAssemblyList[0] > 1)
	{
		m_iBuildCosts = 0;
		// Baukosten sind nun von den Kursen am Markt abhängig. Da die Preise am Markt für
		// 1000 Einheiten gelten wird hier durch 1000 geteilt. Die Preise vom Markt sind die,
		// welche wir am Anfang der Runde an unserer Börse hatten. Damit ist es nicht möglich die
		// Preise in der selben Runde wo wir kaufen wollen noch zu drücken.
		
		//  EDIT: 
		//		Ich teile mittlerweile die Ressourcenpreise durch 2000, da der Kauf immer etwas teuer war.
		//		Dies hat aber rein balancingtechnische Gründe.
		// /EDIT
		m_iBuildCosts =         m_iNeededIndustryInAssemblyList[0];
		m_iBuildCosts += ((int)(m_iNeededTitanInAssemblyList[0]*resPrices[TITAN]/2000));
		m_iBuildCosts += ((int)(m_iNeededDeuteriumInAssemblyList[0]*resPrices[DEUTERIUM]/2000));
		m_iBuildCosts += ((int)(m_iNeededDuraniumInAssemblyList[0]*resPrices[DURANIUM]/2000));
		m_iBuildCosts += ((int)(m_iNeededCrystalInAssemblyList[0]*resPrices[CRYSTAL]/2000));
		m_iBuildCosts += ((int)(m_iNeededIridiumInAssemblyList[0]*resPrices[IRIDIUM]/2000));
	}
}

// Funktion setzt die noch restlichen Baukosten auf 1 und sagt, dass wir jetzt was gekauft haben 
// Wenn wir kaufen können bestimmt die Fkt "CalculateBuildCosts()". Diese Fkt. immer vorher aufrufen.
// Die Latinumkosten werden zurückgegeben
int CAssemblyList::BuyBuilding(int EmpiresLatinum)
{
	// EmpiresLatinum ist das aktuelle Latinum des Imperiums    
	if (m_iNeededIndustryInAssemblyList[0] > 1)
		if (m_iBuildCosts <= EmpiresLatinum)
		{	
			m_iNeededIndustryInAssemblyList[0] = 1;
			m_bWasBuildingBought = TRUE;
			return m_iBuildCosts;	// Gibt die Kosten zurück, wenn diese kleiner/gleich dem Latinumbestand sind
		}
	return 0;	// ansonsten gibts ne NULL zurück
}

// Die Funktion berechnet was nach dem Tick noch an verbleibender Industrieleistung aufzubringen ist.
// Ist der Bauauftrag fertig gibt die Funktion ein TRUE zurück. Wenn wir ein TRUE zurückbekommen, müssen
// wir direkt danach die Funktion ClearAssemblyList() aufrufen!
BOOLEAN CAssemblyList::CalculateBuildInAssemblyList(USHORT m_iIndustryProd)
{
	// Nur normale Gebäude und Updates
	m_iNeededIndustryInAssemblyList[0] -= m_iIndustryProd;
	// Die benötigten Rohstoffe werden intern pro Runde halbiert
	// -> wenn wir also einen Auftrag abrechen, bekommen wir nicht
	// immer den vollen Betrag zurück.
	// z.B. nach 1 Runde  nur noch 3/4      => 0,75%
	// z.B. nach 2 Runden nur noch 9/16     => 0,5625%
	// z.B. nach 5 Runden nur noch 243/1024 => 0,2373046875%
	m_iNeededTitanInAssemblyList[0]		= (int)(m_iNeededTitanInAssemblyList[0]*0.75);
	m_iNeededDeuteriumInAssemblyList[0] = (int)(m_iNeededDeuteriumInAssemblyList[0]*0.75);
	m_iNeededDuraniumInAssemblyList[0]  = (int)(m_iNeededDuraniumInAssemblyList[0]*0.75);
	m_iNeededCrystalInAssemblyList[0]   = (int)(m_iNeededCrystalInAssemblyList[0]*0.75);
	m_iNeededIridiumInAssemblyList[0]	= (int)(m_iNeededIridiumInAssemblyList[0]*0.75);
	m_iNeededDilithiumInAssemblyList[0] = (int)(m_iNeededDilithiumInAssemblyList[0]*0.75);
	if (m_iNeededIndustryInAssemblyList[0] <= 0)
	{
		m_iNeededIndustryInAssemblyList[0] = 0;
		m_iNeededTitanInAssemblyList[0]	   = 0;
		m_iNeededDeuteriumInAssemblyList[0]= 0;
		m_iNeededDuraniumInAssemblyList[0] = 0;
		m_iNeededCrystalInAssemblyList[0]  = 0;
		m_iNeededIridiumInAssemblyList[0]  = 0;
		m_iNeededDilithiumInAssemblyList[0]= 0;
		return TRUE;
		// Wenn wir TRUE zurückbekommen müssen wir direkt danach die
		// Funktion ClearAssemblyList() aufrufen!
	}	
	return FALSE;
}

// Funktion löscht einen Eintrag aus der Bauliste, wenn das Gebäude fertig wurde oder wir den ersten
// Eintrag manuell löschen möchten. Nach Aufruf dieser Funktion muß unbedingt die Funktion
// CalculateVariables() aufgerufen werden.
void CAssemblyList::ClearAssemblyList(const CPoint &ko, CSystem systems[][STARMAP_SECTORS_VCOUNT])
{
	// Alle prozentualen Anteile eines womöglich früheren Bauauftrages aus den Ressourcenrouten löschen
	CSystem* system = &systems[ko.x][ko.y];	
	
	CArray<CPoint> routesFrom;
	ULONG resourcesFromRoutes[DILITHIUM + 1];
	ULONG nResInDistSys[DILITHIUM + 1];
	CPoint ptResourceDistributorKOs[DILITHIUM + 1];

	for (int i = 0; i <= DILITHIUM; i++)
	{
		resourcesFromRoutes[i] = 0;
		nResInDistSys[i] = 0;
		ptResourceDistributorKOs[i] = CPoint(-1,-1);		
	}
	
	// Ressourcenrouten durchgehen und womöglich die möglichen max. zusätzlichen Ressourcen erfragen
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
	{
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			if (systems[x][y].GetOwnerOfSystem() == system->GetOwnerOfSystem() && CPoint(x,y) != ko)
			{
				for (int i = 0; i < systems[x][y].GetResourceRoutes()->GetSize(); i++)
				{
					if (systems[x][y].GetResourceRoutes()->GetAt(i).GetKO() == ko)
					{
						// prozentualen Anteil vom alten Auftrag zurücksetzen
						systems[x][y].GetResourceRoutes()->ElementAt(i).SetPercent(0);
						// Ressourcen über Route holen
						if (systems[x][y].GetBlockade() == NULL && systems[ko.x][ko.y].GetBlockade() == NULL)
						{
							routesFrom.Add(CPoint(x,y));
							BYTE res = systems[x][y].GetResourceRoutes()->GetAt(i).GetResource();
							resourcesFromRoutes[res] += systems[x][y].GetRessourceStore(res);							
						}
					}
				}
				// gilt nicht bei Blockaden
				if (systems[x][y].GetBlockade() == NULL && systems[ko.x][ko.y].GetBlockade() == NULL)
				{
					for (int res = TITAN; res <= DILITHIUM; res++)
					{
						if (systems[x][y].GetProduction()->GetResourceDistributor(res))
						{
							ptResourceDistributorKOs[res] = CPoint(x,y);
							nResInDistSys[res] = systems[x][y].GetRessourceStore(res);
						}
					}
				}
			}			
		}
	}

	// AssemblyList Eintrag des gebauten Gebäudes/Updates/Schiffes löschen, wenn wir noch den
	// Eintrag an der nächsten Stelle haben (sprich AssemblyList[1] != 0), dann alle 
	// anderen Einträge um eins nach vorn verschieben -> letzter wird frei
	m_iEntry[0] = 0;
	m_iNeededIndustryInAssemblyList[0] = 0;
	m_iNeededTitanInAssemblyList[0]    = 0;
	m_iNeededDeuteriumInAssemblyList[0]= 0;
	m_iNeededDuraniumInAssemblyList[0] = 0;
	m_iNeededCrystalInAssemblyList[0]  = 0;
	m_iNeededIridiumInAssemblyList[0]  = 0;
	m_iNeededDilithiumInAssemblyList[0]= 0;

	for (int i = 0; i < ALE - 1; i++)
	{
		// wenn der nächste Eintrag ungleich 0 ist
		if (m_iEntry[i + 1] != 0)
		{
			m_iEntry[i] = m_iEntry[i + 1];
			m_iNeededIndustryInAssemblyList[i] = m_iNeededIndustryInAssemblyList[i + 1];
			m_iNeededTitanInAssemblyList[i]	   = m_iNeededTitanInAssemblyList[i + 1];
			m_iNeededDeuteriumInAssemblyList[i]= m_iNeededDeuteriumInAssemblyList[i + 1];
			m_iNeededDuraniumInAssemblyList[i] = m_iNeededDuraniumInAssemblyList[i + 1];
			m_iNeededCrystalInAssemblyList[i]  = m_iNeededCrystalInAssemblyList[i + 1];
			m_iNeededIridiumInAssemblyList[i]  = m_iNeededIridiumInAssemblyList[i + 1];
			m_iNeededDilithiumInAssemblyList[i]= m_iNeededDilithiumInAssemblyList[i + 1];

			// den Nachfolger überall auf NULL setzen
			m_iEntry[i + 1] = 0;
			m_iNeededIndustryInAssemblyList[i + 1] = 0;
			m_iNeededTitanInAssemblyList[i + 1]    = 0;
			m_iNeededDeuteriumInAssemblyList[i + 1]= 0;
			m_iNeededDuraniumInAssemblyList[i + 1] = 0;
			m_iNeededCrystalInAssemblyList[i + 1]  = 0;
			m_iNeededIridiumInAssemblyList[i + 1]  = 0;
		}
		else
			break;
	}

	// Checken, ob der nächste Eintrag auch baubar ist -> genügend RES im Lager
	// normalerweise kann man in der Bauliste ja nur Einträge vornehmen, wenn
	// man genügend RES hat. Also sollte er auch baubar sein, wenn die IP
	// erbracht wurden. Aber durch Zufallsereignisse oder Börsenverkäufe von RES
	// kann man später ja zu wening davon haben. Und weil die RES erst abgezogen
	// wird, wenn das Gebäude an erster Stelle in der Bauliste rückt, müssen
	// wird das überprüfen. Haben wir nicht genug RES, wird der Bauauftrag
	// gecancelt
	
	// Überprüfen, ob wir genügend Rohstoffe in dem Lager haben
	for (int res = TITAN; res <= DILITHIUM; res++)
	{
		UINT nNeededRes = this->GetNeededResourceInAssemblyList(0, res);
		if (*system->GetRessourceStorages(res) + resourcesFromRoutes[res] < nNeededRes && nResInDistSys[res] < nNeededRes)
		{
			// Wenn nicht -> dann Eintrag wieder entfernen
			ClearAssemblyList(ko, systems);
			return;
		}
	}

	// Wenn er baubar ist, dann die Ressourcen entfernen	
	for (int res = TITAN; res <= DILITHIUM; res++)
	{
		UINT nNeededRes = this->GetNeededResourceInAssemblyList(0, res);
		if (nNeededRes > 0)
		{
			// Ressource wird aus eigenem System bzw. über Ressourcenroute geholt
			if (*system->GetRessourceStorages(res) + resourcesFromRoutes[res] >= nNeededRes)
				RemoveResourceFromStorage(res, ko, systems, &routesFrom);
			// reicht das nicht, so wird Ressource aus dem Verteiler geholt
			else
			{
				CArray<CPoint> vNullRoutes;
				RemoveResourceFromStorage(res, ptResourceDistributorKOs[res], systems, &vNullRoutes);
			}
		}
	}
}

// Ordnet die Bauliste so, dass keine leeren Einträge in der Mitte vorkommen können.
// Wird z.B. aufgerufen, nachdem wir einen Auftrag aus der Bauliste entfernt haben.
// Darf aber nicht aufgerufen werden, wenn wir den 0. Eintrag entfernen, dann müssen
// wir ClearAssemblyList() aufrufen, weil diese Funktion die nötigen Rohstoffe gleich mit abzieht.
void CAssemblyList::AdjustAssemblyList(short entry)
{
	m_iEntry[entry] = 0;
	for (int i = entry; i < ALE-1; i++)	// Bauliste durchgehen
		if (m_iEntry[i] == 0)			// wenn der Eintrag 0 ist -> also nix
			if (i+1 != 0)				// und der nächste Eintrag nicht 0 ist
			{							// nächsten Eintrag um eins nach vorn
				m_iEntry[i] = m_iEntry[i+1];
				m_iNeededIndustryInAssemblyList[i]  = m_iNeededIndustryInAssemblyList[i+1];
				m_iNeededTitanInAssemblyList[i]		= m_iNeededTitanInAssemblyList[i+1];
				m_iNeededDeuteriumInAssemblyList[i] = m_iNeededDeuteriumInAssemblyList[i+1];
				m_iNeededDuraniumInAssemblyList[i]  = m_iNeededDuraniumInAssemblyList[i+1];
				m_iNeededCrystalInAssemblyList[i]	= m_iNeededCrystalInAssemblyList[i+1];
				m_iNeededIridiumInAssemblyList[i]	= m_iNeededIridiumInAssemblyList[i+1];
				m_iNeededDilithiumInAssemblyList[i] = m_iNeededDilithiumInAssemblyList[i+1];
				m_iEntry[i+1] = 0;
			}
}

// Resetfunktion
void CAssemblyList::Reset()
{
	m_iNeededIndustryForBuild = 0;	// benötigte IP zum Bau
	m_iNeededTitanForBuild = 0;		// benötigtes Titan zum Bau
	m_iNeededDeuteriumForBuild = 0;	// benötigtes Deuterium zum Bau
	m_iNeededDuraniumForBuild = 0;	// benötigtes Duranium zum Bau
	m_iNeededCrystalForBuild = 0;	// benötigtes Crystal zum Bau
	m_iNeededIridiumForBuild = 0;	// benötigtes Iridium zum Bau
	m_iNeededDilithiumForBuild = 0;	// benötigtes Dilithium zum Bau
	m_bWasBuildingBought = 0;		// Wurde ein Gebäude gekauft
	m_iBuildCosts = 0;				// Die Baukosten eines Bauauftrags
	for (int i = 0; i < ALE; i++)
	{
		m_iEntry[i] = 0;						// Bauauftrag
		m_iNeededIndustryInAssemblyList[i] = 0; // noch benötigte IP zum fertigstellen des Projektes
		m_iNeededTitanInAssemblyList[i] = 0;	// noch benötigtes Titan zum fertigstellen des Projektes
		m_iNeededDeuteriumInAssemblyList[i] = 0;// noch benötigtes Deuterium zum fertigstellen des Projektes
		m_iNeededDuraniumInAssemblyList[i] = 0;	// noch benötigtes Duranium zum fertigstellen des Projektes
		m_iNeededCrystalInAssemblyList[i] = 0;	// noch benötigtes Crystal zum fertigstellen des Projektes
		m_iNeededIridiumInAssemblyList[i] = 0;	// noch benötigtes Iridium zum fertigstellen des Projektes
		m_iNeededDilithiumInAssemblyList[i] = 0;// noch benötigtes Dilithium zum fertigstellen des Projektes
	}
}