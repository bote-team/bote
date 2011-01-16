// Empire.cpp: Implementierung der Klasse CEmpire.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Empire.h"
#include "System\System.h"
#include "Galaxy\Sector.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CEmpire, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CEmpire::CEmpire()
{
	Reset();
}
CEmpire::~CEmpire()
{
	Reset();
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CEmpire::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	m_Research.Serialize(ar);
	m_Intelligence.Serialize(ar);
	m_GlobalStorage.Serialize(ar);
		
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_byNumberOfSystems;
		ar << m_iCredits;
		ar << m_iCreditsChange;
		ar << m_iShipCosts;
		ar << m_iPopSupportCosts;
		ar << m_lFP;
		for (int i = 0; i <= DERITIUM; i++)
			ar << m_lResourceStorages[i];
		ar << m_sEmpireID;
		ar << m_Messages.GetSize();
		for (int i = 0; i < m_Messages.GetSize(); i++)
			m_Messages.GetAt(i).Serialize(ar);
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ClearMessagesAndEvents();
		int number = 0;
		ar >> m_byNumberOfSystems;
		ar >> m_iCredits;
		ar >> m_iCreditsChange;
		ar >> m_iShipCosts;
		ar >> m_iPopSupportCosts;
		ar >> m_lFP;
		for (int i = 0; i <= DERITIUM; i++)
			ar >> m_lResourceStorages[i];
		ar >> m_sEmpireID;
		ar >> number;
		m_Messages.SetSize(number);
		for (int i = 0; i < number; i++)
			m_Messages.GetAt(i).Serialize(ar);		
	}

	m_EventMessages.Serialize(ar);	
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion generiert die Liste der Systeme für das Imperium anhand aller Systeme.
void CEmpire::GenerateSystemList(const CSystem systems[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT], const CSector sectors[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT])
{
	m_SystemList.RemoveAll();
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (systems[x][y].GetOwnerOfSystem() == m_sEmpireID)
				m_SystemList.Add(SystemViewStruct(sectors[x][y].GetName(), CPoint(x,y)));
}

/// Funktion addiert die übergebene Anzahl an Forschungspunkten zu den Forschungspunkten des Imperiums.
void CEmpire::AddFP(int add)
{
	if (((long)m_lFP + add) < NULL)
		m_lFP = NULL;
	else
		m_lFP += add;
}

// Funktion setzt die Lager wieder auf NULL, damit wir die später wieder füllen können
// die wirklichen Lager in den einzelnen Systemen werden dabei nicht angerührt
void CEmpire::ClearAllPoints(void)
{
	memset(m_lResourceStorages, 0, sizeof(m_lResourceStorages));
	m_lFP = 0;
	m_Intelligence.ClearSecurityPoints();
}

/// Funktion löscht alle Nachrichten und Antworten an das Imperiums.
void CEmpire::ClearMessagesAndEvents(void)
{
	m_Messages.RemoveAll();
	for (int i = 0; i < m_EventMessages.GetSize(); i++)
		delete m_EventMessages.GetAt(i);
	m_EventMessages.RemoveAll();
}

void CEmpire::Reset(void)
{
	m_byNumberOfSystems = 1;
	m_sEmpireID = "";
	m_iCredits = 1000;
	m_iCreditsChange = 0;
	m_iShipCosts = 0;
	m_iPopSupportCosts = 0;
	
	m_Research.Reset();
	m_Intelligence.Reset();
	m_GlobalStorage.Reset();
	
	ClearAllPoints();
	ClearMessagesAndEvents();
	
	m_SystemList.RemoveAll();	
}