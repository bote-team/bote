// Worker.cpp: Implementierung der Klasse CWorker.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdafx.h"
#include "Worker.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CWorker, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CWorker::CWorker()
{
	m_iFoodWorker = 0;
	m_iIndustryWorker = 0;
	m_iEnergyWorker = 0;
	m_iSecurityWorker = 0;
	m_iResearchWorker = 0;
	m_iTitanWorker = 0;
	m_iDeuteriumWorker = 0;
	m_iDuraniumWorker = 0;
	m_iCrystalWorker = 0;
	m_iIridiumWorker = 0;
	m_iAllWorkers = 0;
	m_iFreeWorkers = 10;
}

CWorker::~CWorker()
{

}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CWorker::CWorker(const CWorker &rhs)
{
	m_iFoodWorker = rhs.m_iFoodWorker;
	m_iIndustryWorker = rhs.m_iIndustryWorker;
	m_iEnergyWorker = rhs.m_iEnergyWorker;
	m_iSecurityWorker = rhs.m_iSecurityWorker;
	m_iResearchWorker = rhs.m_iResearchWorker;
	m_iTitanWorker = rhs.m_iTitanWorker;
	m_iDeuteriumWorker = rhs.m_iDeuteriumWorker;
	m_iDuraniumWorker = rhs.m_iDuraniumWorker;
	m_iCrystalWorker = rhs.m_iCrystalWorker;
	m_iIridiumWorker = rhs.m_iIridiumWorker;
	m_iAllWorkers = rhs.m_iAllWorkers;
	m_iFreeWorkers = rhs.m_iFreeWorkers;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CWorker & CWorker::operator=(const CWorker & rhs)
{
	if (this == &rhs)
		return *this;
	m_iFoodWorker = rhs.m_iFoodWorker;
	m_iIndustryWorker = rhs.m_iIndustryWorker;
	m_iEnergyWorker = rhs.m_iEnergyWorker;
	m_iSecurityWorker = rhs.m_iSecurityWorker;
	m_iResearchWorker = rhs.m_iResearchWorker;
	m_iTitanWorker = rhs.m_iTitanWorker;
	m_iDeuteriumWorker = rhs.m_iDeuteriumWorker;
	m_iDuraniumWorker = rhs.m_iDuraniumWorker;
	m_iCrystalWorker = rhs.m_iCrystalWorker;
	m_iIridiumWorker = rhs.m_iIridiumWorker;
	m_iAllWorkers = rhs.m_iAllWorkers;
	m_iFreeWorkers = rhs.m_iFreeWorkers;	
	return *this;
}

//////////////////////////////////////////////////////////////////////
// Serialisierungsfunktion
//////////////////////////////////////////////////////////////////////
void CWorker::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_iFoodWorker;
		ar << m_iIndustryWorker;
		ar << m_iEnergyWorker;
		ar << m_iSecurityWorker;
		ar << m_iResearchWorker;
		ar << m_iTitanWorker;
		ar << m_iDeuteriumWorker;
		ar << m_iDuraniumWorker;
		ar << m_iCrystalWorker;
		ar << m_iIridiumWorker;
		ar << m_iAllWorkers;
		ar << m_iFreeWorkers;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_iFoodWorker;
		ar >> m_iIndustryWorker;
		ar >> m_iEnergyWorker;
		ar >> m_iSecurityWorker;
		ar >> m_iResearchWorker;
		ar >> m_iTitanWorker;
		ar >> m_iDeuteriumWorker;
		ar >> m_iDuraniumWorker;
		ar >> m_iCrystalWorker;
		ar >> m_iIridiumWorker;
		ar >> m_iAllWorkers;
		ar >> m_iFreeWorkers;		
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
USHORT CWorker::GetWorker(WORKER::Typ nWorker) const
{
	switch (nWorker)
	{
	case WORKER::FOOD_WORKER:		return m_iFoodWorker;
	case WORKER::INDUSTRY_WORKER:	return m_iIndustryWorker;
	case WORKER::ENERGY_WORKER:		return m_iEnergyWorker;
	case WORKER::SECURITY_WORKER:	return m_iSecurityWorker;
	case WORKER::RESEARCH_WORKER:	return m_iResearchWorker;
	case WORKER::TITAN_WORKER:		return m_iTitanWorker;
	case WORKER::DEUTERIUM_WORKER:	return m_iDeuteriumWorker;
	case WORKER::DURANIUM_WORKER:	return m_iDuraniumWorker;
	case WORKER::CRYSTAL_WORKER:	return m_iCrystalWorker;
	case WORKER::IRIDIUM_WORKER:	return m_iIridiumWorker;
	case WORKER::ALL_WORKER:		return m_iAllWorkers;
	case WORKER::FREE_WORKER:		return m_iFreeWorkers;
	default: ASSERT(FALSE);
	}
	
	return 0;
}

void CWorker::SetWorker(WORKER::Typ nWorker, int Value)
{
	switch (nWorker)
	{
	case WORKER::FOOD_WORKER:		m_iFoodWorker = Value; break;
	case WORKER::INDUSTRY_WORKER:	m_iIndustryWorker = Value; break;
	case WORKER::ENERGY_WORKER:		m_iEnergyWorker = Value; break;
	case WORKER::SECURITY_WORKER:	m_iSecurityWorker = Value; break;
	case WORKER::RESEARCH_WORKER:	m_iResearchWorker = Value; break;
	case WORKER::TITAN_WORKER:		m_iTitanWorker = Value; break;
	case WORKER::DEUTERIUM_WORKER:	m_iDeuteriumWorker = Value; break;
	case WORKER::DURANIUM_WORKER:	m_iDuraniumWorker = Value; break;
	case WORKER::CRYSTAL_WORKER:	m_iCrystalWorker = Value; break;
	case WORKER::IRIDIUM_WORKER:	m_iIridiumWorker = Value; break;
	case WORKER::ALL_WORKER:		m_iAllWorkers = Value; break;
	case WORKER::FREE_WORKER:		m_iFreeWorkers = Value; break;
	default: ASSERT(FALSE);
	}
}

void CWorker::InkrementWorker(WORKER::Typ nWorker)
{
	switch (nWorker)
	{
	case WORKER::FOOD_WORKER:		m_iFoodWorker++; break;
	case WORKER::INDUSTRY_WORKER:	m_iIndustryWorker++; break;
	case WORKER::ENERGY_WORKER:		m_iEnergyWorker++; break;
	case WORKER::SECURITY_WORKER:	m_iSecurityWorker++; break;
	case WORKER::RESEARCH_WORKER:	m_iResearchWorker++; break;
	case WORKER::TITAN_WORKER:		m_iTitanWorker++; break;
	case WORKER::DEUTERIUM_WORKER:	m_iDeuteriumWorker++; break;
	case WORKER::DURANIUM_WORKER:	m_iDuraniumWorker++; break;
	case WORKER::CRYSTAL_WORKER:	m_iCrystalWorker++; break;
	case WORKER::IRIDIUM_WORKER:	m_iIridiumWorker++; break;
	case WORKER::ALL_WORKER:		m_iAllWorkers++; break;
	case WORKER::FREE_WORKER:		m_iFreeWorkers++; break;
	default: ASSERT(FALSE);
	}
}

void CWorker::DekrementWorker(WORKER::Typ nWorker)
{
	switch (nWorker)
	{
	case WORKER::FOOD_WORKER:		m_iFoodWorker--; break;
	case WORKER::INDUSTRY_WORKER:	m_iIndustryWorker--; break;
	case WORKER::ENERGY_WORKER:		m_iEnergyWorker--; break;
	case WORKER::SECURITY_WORKER:	m_iSecurityWorker--; break;
	case WORKER::RESEARCH_WORKER:	m_iResearchWorker--; break;
	case WORKER::TITAN_WORKER:		m_iTitanWorker--; break;
	case WORKER::DEUTERIUM_WORKER:	m_iDeuteriumWorker--; break;
	case WORKER::DURANIUM_WORKER:	m_iDuraniumWorker--; break;
	case WORKER::CRYSTAL_WORKER:	m_iCrystalWorker--; break;
	case WORKER::IRIDIUM_WORKER:	m_iIridiumWorker--; break;
	case WORKER::ALL_WORKER:		m_iAllWorkers--; break;
	case WORKER::FREE_WORKER:		m_iFreeWorkers--; break;
	default: ASSERT(FALSE);
	}
}

void CWorker::CalculateFreeWorkers()
{
	m_iFreeWorkers = m_iAllWorkers-m_iFoodWorker-m_iIndustryWorker-m_iEnergyWorker-m_iSecurityWorker-m_iResearchWorker
		-m_iTitanWorker-m_iDeuteriumWorker-m_iDuraniumWorker-m_iCrystalWorker-m_iIridiumWorker;
}

// Checked, ob wir zuviele Arbeiter zugewiesen haben, obwohl wir gar nicht so viele haben
// z.B. durch Nahrungsmangel kann so was passieren
void CWorker::CheckWorkers()
{
	short otherWorkers = m_iFoodWorker+m_iIndustryWorker+m_iEnergyWorker+m_iSecurityWorker+m_iResearchWorker
		+m_iTitanWorker+m_iDeuteriumWorker+m_iDuraniumWorker+m_iCrystalWorker+m_iIridiumWorker;
	if (m_iAllWorkers < otherWorkers)
	{
		short diff = otherWorkers - m_iAllWorkers;	// Differenz der zuvielen Arbeiter
		while (diff > 0)
		{			
			if (m_iResearchWorker > 0)
			{
				m_iResearchWorker--;
				diff--;
				if (diff == 0) break;
			}
			if (m_iSecurityWorker > 0)
			{
				m_iSecurityWorker--;
				diff--;
				if (diff == 0) break;
			}
			if (m_iIndustryWorker > 0)
			{
				m_iIndustryWorker--;
				diff--;
				if (diff == 0) break;
			}
			if (m_iTitanWorker > 0)
			{
				m_iTitanWorker--;
				diff--;
				if (diff == 0) break;
			}
			if (m_iDeuteriumWorker > 0)
			{
				m_iDeuteriumWorker--;
				diff--;
				if (diff == 0) break;
			}
			if (m_iDuraniumWorker > 0)
			{
				m_iDuraniumWorker--;
				diff--;
				if (diff == 0) break;
			}
			if (m_iCrystalWorker > 0)
			{
				m_iCrystalWorker--;
				diff--;
				if (diff == 0) break;
			}
			if (m_iIridiumWorker > 0)
			{
				m_iIridiumWorker--;
				diff--;
				if (diff == 0) break;
			}
			if (m_iEnergyWorker > 0)
			{
				m_iEnergyWorker--;
				diff--;
				if (diff == 0) break;
			}
			if (m_iFoodWorker > 0)
			{
				m_iFoodWorker--;
				diff--;
				if (diff == 0) break;
			}
		}
	}
}

