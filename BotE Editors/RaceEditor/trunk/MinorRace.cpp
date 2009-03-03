// MinorRace.cpp: Implementierung der Klasse CMinorRace.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MinorRace.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CMinorRace::CMinorRace()
{
	
}

CMinorRace::~CMinorRace()
{

}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CMinorRace::CMinorRace(const CMinorRace & rhs)
{
	m_strRaceName = rhs.m_strRaceName;
	m_strHomePlanet = rhs.m_strHomePlanet;
	m_strRaceDescription = rhs.m_strRaceDescription;
	m_strGraphicName = rhs.m_strGraphicName;
	for (int i = 0; i <= DOMINION; i++)
		m_iRelationship[i] = rhs.m_iRelationship[i];
	m_iTechnologicalProgress = rhs.m_iTechnologicalProgress;
	m_iKind = rhs.m_iKind;
	m_iCorruptibility = rhs.m_iCorruptibility;
	m_bSpaceflightNation = rhs.m_bSpaceflightNation;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CMinorRace & CMinorRace::operator=(const CMinorRace & rhs)
{
	if (this == &rhs)
		return *this;
		m_strRaceName = rhs.m_strRaceName;
	m_strHomePlanet = rhs.m_strHomePlanet;
	m_strRaceDescription = rhs.m_strRaceDescription;
	m_strGraphicName = rhs.m_strGraphicName;
	for (int i = 0; i <= DOMINION; i++)
		m_iRelationship[i] = rhs.m_iRelationship[i];
	m_iTechnologicalProgress = rhs.m_iTechnologicalProgress;
	m_iKind = rhs.m_iKind;
	m_iCorruptibility = rhs.m_iCorruptibility;
	m_bSpaceflightNation = rhs.m_bSpaceflightNation;
	return *this;
}

void CMinorRace::Reset()
{
	m_strRaceName.Format("Species %d",rand()%10000+1);		// Der Name der Rasse
	m_strHomePlanet.Format("System %d",rand()%10000+1);		// Der Name des Heimatplaneten der Minorrace
	m_strRaceDescription = "no description available";		// Die Beschreibung der Minorrace
	m_strGraphicName = m_strRaceName + ".jpg";				// Der Name der Grafikdatei
	for (int i = 0; i <= DOMINION; i++)
		m_iRelationship[i] = 50;	// Die Beziehung der Minorrace zu den einzelnen Majorraces
	m_iTechnologicalProgress = 2;	// wie fortschrittlich ist die Minorrace?
	m_iKind = 0;					// Art der Minorrace (landwirtschaftlich, kriegerish, industriell usw.)
	m_iCorruptibility = 2;			// wie stark ändert sich die Beziehung beim Geschenke geben?
	m_bSpaceflightNation = FALSE;	// ist die Minorrace eine Raumfahrtnation
}
