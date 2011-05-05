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
USHORT CWorker::GetWorker(int WhatWorker) const
{
	USHORT returnValue = 0;
	if (WhatWorker == 0) returnValue = m_iFoodWorker;
	else if (WhatWorker == 1) returnValue = m_iIndustryWorker;
	else if (WhatWorker == 2) returnValue = m_iEnergyWorker;
	else if (WhatWorker == 3) returnValue = m_iSecurityWorker;
	else if (WhatWorker == 4) returnValue = m_iResearchWorker;
	else if (WhatWorker == 5) returnValue = m_iTitanWorker;
	else if (WhatWorker == 6) returnValue = m_iDeuteriumWorker;
	else if (WhatWorker == 7) returnValue = m_iDuraniumWorker;
	else if (WhatWorker == 8) returnValue = m_iCrystalWorker;
	else if (WhatWorker == 9) returnValue = m_iIridiumWorker;
	else if (WhatWorker == 10) returnValue = m_iAllWorkers;
	else if (WhatWorker == 11) returnValue = m_iFreeWorkers;
	return returnValue;
}

void CWorker::SetWorker(int WhatWorker, int Value)
{
	if (WhatWorker == 0) m_iFoodWorker = Value;
	else if (WhatWorker == 1) m_iIndustryWorker = Value;
	else if (WhatWorker == 2) m_iEnergyWorker  = Value;
	else if (WhatWorker == 3) m_iSecurityWorker = Value;
	else if (WhatWorker == 4) m_iResearchWorker = Value;
	else if (WhatWorker == 5) m_iTitanWorker = Value;
	else if (WhatWorker == 6) m_iDeuteriumWorker = Value;
	else if (WhatWorker == 7) m_iDuraniumWorker = Value;
	else if (WhatWorker == 8) m_iCrystalWorker = Value;
	else if (WhatWorker == 9) m_iIridiumWorker = Value;
	else if (WhatWorker == 10) m_iAllWorkers = Value;
	else if (WhatWorker == 11) m_iFreeWorkers = Value;
}

void CWorker::InkrementWorker(int WhatWorker)
{
	if (WhatWorker == 0) m_iFoodWorker++;
	else if (WhatWorker == 1) m_iIndustryWorker++;
	else if (WhatWorker == 2) m_iEnergyWorker ++;
	else if (WhatWorker == 3) m_iSecurityWorker++;
	else if (WhatWorker == 4) m_iResearchWorker++;
	else if (WhatWorker == 5) m_iTitanWorker++;
	else if (WhatWorker == 6) m_iDeuteriumWorker++;
	else if (WhatWorker == 7) m_iDuraniumWorker++;
	else if (WhatWorker == 8) m_iCrystalWorker++;
	else if (WhatWorker == 9) m_iIridiumWorker++;
	else if (WhatWorker == 10) m_iAllWorkers++;
	else if (WhatWorker == 11) m_iFreeWorkers++;
}

void CWorker::DekrementWorker(int WhatWorker)
{
	if (WhatWorker == 0) m_iFoodWorker--;
	else if (WhatWorker == 1) m_iIndustryWorker--;
	else if (WhatWorker == 2) m_iEnergyWorker --;
	else if (WhatWorker == 3) m_iSecurityWorker--;
	else if (WhatWorker == 4) m_iResearchWorker--;
	else if (WhatWorker == 5) m_iTitanWorker--;
	else if (WhatWorker == 6) m_iDeuteriumWorker--;
	else if (WhatWorker == 7) m_iDuraniumWorker--;
	else if (WhatWorker == 8) m_iCrystalWorker--;
	else if (WhatWorker == 9) m_iIridiumWorker--;
	else if (WhatWorker == 10) m_iAllWorkers--;
	else if (WhatWorker == 11) m_iFreeWorkers--;
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

