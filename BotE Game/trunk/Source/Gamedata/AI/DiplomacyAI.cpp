#include "stdafx.h"
#include "DiplomacyAI.h"
#include "Botf2Doc.h"
#include "botf2.h"
#include "Races\RaceController.h"

IMPLEMENT_SERIAL (CDiplomacyInfo, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CDiplomacyInfo::CDiplomacyInfo(void)
{
	Reset();
}

CDiplomacyInfo::~CDiplomacyInfo(void)
{
	Reset();
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CDiplomacyInfo::CDiplomacyInfo(const CDiplomacyInfo & rhs)
{
	m_sToRace = rhs.m_sToRace;
	m_sFromRace = rhs.m_sFromRace;
	m_sText = rhs.m_sText;
	
	m_nFlag = rhs.m_nFlag;
	m_nType = rhs.m_nType;
	m_nSendRound = rhs.m_nSendRound;
	m_nCredits = rhs.m_nCredits;
	for (int i = 0; i < DERITIUM + 1; i++)
		m_nResources[i] = rhs.m_nResources[i];
	m_ptKO = rhs.m_ptKO;

	m_nDuration = rhs.m_nDuration;
	m_sWarpactEnemy = rhs.m_sWarpactEnemy;
	m_sCorruptedRace = rhs.m_sCorruptedRace;

	m_nAnswerStatus = rhs.m_nAnswerStatus;
	m_sHeadline = rhs.m_sHeadline;
	m_sWarPartner = rhs.m_sWarPartner;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CDiplomacyInfo & CDiplomacyInfo::operator=(const CDiplomacyInfo & rhs)
{
	if (this == &rhs)
		return *this;

	m_sToRace = rhs.m_sToRace;
	m_sFromRace = rhs.m_sFromRace;
	m_sText = rhs.m_sText;
	
	m_nFlag = rhs.m_nFlag;
	m_nType = rhs.m_nType;
	m_nSendRound = rhs.m_nSendRound;
	m_nCredits = rhs.m_nCredits;
	for (int i = 0; i < DERITIUM + 1; i++)
		m_nResources[i] = rhs.m_nResources[i];
	m_ptKO = rhs.m_ptKO;

	m_nDuration = rhs.m_nDuration;
	m_sWarpactEnemy = rhs.m_sWarpactEnemy;
	m_sCorruptedRace = rhs.m_sCorruptedRace;

	m_nAnswerStatus = rhs.m_nAnswerStatus;
	m_sHeadline = rhs.m_sHeadline;
	m_sWarPartner = rhs.m_sWarPartner;
	
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CDiplomacyInfo::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_sToRace;					// an welche Rasse (ID) geht das Angebot
		ar << m_sFromRace;					// von welcher Rasse (ID) kommt das Angebot
		ar << m_sText;						// Angebotstext
		
		ar << m_nFlag;						// Angebot, Antwort oder irgendwas anderes
		ar << m_nType;						// Art des Angebots (Krieg, NAP, Geschenk usw.)
		ar << m_nSendRound;					// in welcher Runde wurde das Angebot abgeschickt
		ar << m_nCredits;					// wieviele Credits werden als Geschenk angeboten
		for (int i = 0; i <= DERITIUM; i++)
			ar << m_nResources[i];			// welche Ressourcen werden mit als Geschenk angeboten
		ar << m_ptKO;						// aus welchem System stammen die Ressourcen

		ar << m_nDuration;					// Dauer des Angebots
		ar << m_sWarpactEnemy;				// der Gegner bei einem Kriegspaktangebot
		ar << m_sCorruptedRace;				// Hauptrasse, welche bei einer Bestechnung betroffen sein soll

		ar << m_nAnswerStatus;				// wurde das Angebot angenommen, abgelehnt oder nicht darauf reagiert
		ar << m_sHeadline;					// Überschrift bei Angebotsantwort
		ar << m_sWarPartner;				// Krieg aufgrund Diplomatiepartner
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_sToRace;					// an welche Rasse (ID) geht das Angebot
		ar >> m_sFromRace;					// von welcher Rasse (ID) kommt das Angebot
		ar >> m_sText;						// Angebotstext
		
		ar >> m_nFlag;						// Angebot, Antwort oder irgendwas anderes
		ar >> m_nType;						// Art des Angebots (Krieg, NAP, Geschenk usw.)
		ar >> m_nSendRound;					// in welcher Runde wurde das Angebot abgeschickt
		ar >> m_nCredits;					// wieviele Credits werden als Geschenk angeboten
		for (int i = 0; i <= DERITIUM; i++)
			ar >> m_nResources[i];			// welche Ressourcen werden mit als Geschenk angeboten
		ar >> m_ptKO;						// aus welchem System stammen die Ressourcen

		ar >> m_nDuration;					// Dauer des Angebots
		ar >> m_sWarpactEnemy;				// der Gegner bei einem Kriegspaktangebot
		ar >> m_sCorruptedRace;				// Hauptrasse, welche bei einer Bestechnung betroffen sein soll

		ar >> m_nAnswerStatus;				// wurde das Angebot angenommen, abgelehnt oder nicht darauf reagiert
		ar >> m_sHeadline;					// Überschrift bei Angebotsantwort
		ar >> m_sWarPartner;				// Krieg aufgrund Diplomatiepartner
	}
}

/// Funktion zum Zurücksetzen aller Werte
void CDiplomacyInfo::Reset(void)
{
	m_nCredits = 0;
	m_nDuration = 0;
	m_nFlag = -1;
	for (int res = TITAN; res <= DERITIUM; res++)
		m_nResources[res] = 0;
	m_nSendRound = -1;
	m_nType = NO_AGREEMENT;
	m_ptKO = CPoint(-1,-1);
	m_sCorruptedRace = "";
	m_sFromRace = "";
	m_sHeadline = "";
	m_sText = "";
	m_sToRace = "";
	m_sWarpactEnemy = "";
	m_sWarPartner = "";
	m_nAnswerStatus = NOT_REACTED;
}

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CDiplomacyAI::CDiplomacyAI(CRace* pRace) : m_pRace(pRace)
{
	ASSERT(m_pRace);

	m_pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(m_pDoc);
}

CDiplomacyAI::~CDiplomacyAI(void)
{
}