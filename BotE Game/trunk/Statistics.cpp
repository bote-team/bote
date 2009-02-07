// Statistcs.cpp: Implementierung der Klasse CStatistcs.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdafx.h"
#include "Statistics.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CStatistics, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CStatistics::CStatistics()
{
	m_iAverageTechLevel = 0;
	for (int i = TITAN; i <= IRIDIUM; i++)
		m_lAverageResourceStorages[i] = 0;
}

CStatistics::~CStatistics()
{

}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CStatistics::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_iAverageTechLevel;
		for (int i = TITAN; i <= IRIDIUM; i++)
			ar << m_lAverageResourceStorages[i];
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_iAverageTechLevel;
		for (int i = TITAN; i <= IRIDIUM; i++)
			ar >> m_lAverageResourceStorages[i];		
	}
}
//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

void CStatistics::CalculateAverageTechLevel(CEmpire* empire)
{
	// Wieviel Imperien gibt es noch? Hier anhand der Anzahl der Systeme geschaut
	USHORT n = 0;
	m_iAverageTechLevel = 0;
	USHORT bio			= 0;
	USHORT energy		= 0;
	USHORT computer		= 0;
	USHORT propulsion	= 0;
	USHORT construction	= 0;
	USHORT weapon		= 0;
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		if (empire[i].GetNumberOfSystems() > 0)
		{
			n++;
			bio += empire[i].GetResearch()->GetBioTech();
			energy += empire[i].GetResearch()->GetEnergyTech();
			computer += empire[i].GetResearch()->GetCompTech();
			propulsion += empire[i].GetResearch()->GetPropulsionTech();
			construction += empire[i].GetResearch()->GetConstructionTech();
			weapon += empire[i].GetResearch()->GetWeaponTech();
		}
	}
	if (n > 0)
		m_iAverageTechLevel = (bio + energy + computer + propulsion + construction + weapon) / (6 * n);
}

void CStatistics::CalculateAverageResourceStorages(CEmpire* empire)
{
	// Wieviel Imperien gibt es noch? Hier anhand der Anzahl der Systeme geschaut
	USHORT n = 0;
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		m_lAverageResourceStorages[i] = 0;
		for (int j = HUMAN; j <= DOMINION; j++)
		{
			if (empire[j].GetNumberOfSystems() > 0)
			{
				n++;
				m_lAverageResourceStorages[i] += empire[j].GetStorage()[i];
			}
		}
		if (n > 0)
			m_lAverageResourceStorages[i] /= n;
	}
}