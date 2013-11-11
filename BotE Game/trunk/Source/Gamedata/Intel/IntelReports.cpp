#include "stdafx.h"
#include "IntelReports.h"
#include "EcoIntelObj.h"
#include "ScienceIntelObj.h"
#include "MilitaryIntelObj.h"
#include "DiplomacyIntelObj.h"

IMPLEMENT_SERIAL (CIntelReports, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CIntelReports::CIntelReports(void)
{
	m_nActiveReport = -1;
	m_bAddedReport = FALSE;
	m_pAttemptObject = NULL;
	RemoveAllReports();
}

CIntelReports::~CIntelReports(void)
{
	RemoveAllReports();
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CIntelReports::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		BYTE type = MAXBYTE;
		if (m_pAttemptObject)
			type = m_pAttemptObject->GetType();
		ar << type;
		if (m_pAttemptObject)
			m_pAttemptObject->Serialize(ar);
	}
	else if (ar.IsLoading())
	{
		m_nActiveReport = -1;
		RemoveAllReports();
		BYTE type;
		ar >> type;
		if (type != MAXBYTE)
		{
			switch (type)
			{
			case 0: m_pAttemptObject = new CEcoIntelObj(); break;
			case 1: m_pAttemptObject = new CScienceIntelObj(); break;
			case 2: m_pAttemptObject = new CMilitaryIntelObj(); break;
			case 3: m_pAttemptObject = new CDiplomacyIntelObj(); break;
			}
			m_pAttemptObject->Serialize(ar);
		}
		else
			m_pAttemptObject = 0;
	}

	m_IntelObjects.Serialize(ar);
	// Da bei jeder neuen Runde diese Serialisierungsfunktion aufgerufen wird, kann hier die Variable
	// wieder auf FALSE gesetzt werden
	m_bAddedReport = FALSE;
}

//////////////////////////////////////////////////////////////////////
// Zugriffsfunktionen
//////////////////////////////////////////////////////////////////////
/// Funktion legt ein neues Anschlagsobjekt an.
void CIntelReports::CreateAttemptObject(CIntelObject* spyReport)
{
	AssertBotE(spyReport);
	if (m_pAttemptObject)
	{
		delete m_pAttemptObject;
		m_pAttemptObject = NULL;
	}

	BYTE type = spyReport->GetType();
	switch (type)
	{
	case 0: m_pAttemptObject = new CEcoIntelObj(*dynamic_cast<CEcoIntelObj*>(spyReport)); break;
	case 1: m_pAttemptObject = new CScienceIntelObj(*dynamic_cast<CScienceIntelObj*>(spyReport)); break;
	case 2: m_pAttemptObject = new CMilitaryIntelObj(*dynamic_cast<CMilitaryIntelObj*>(spyReport)); break;
	case 3: m_pAttemptObject = new CDiplomacyIntelObj(*dynamic_cast<CDiplomacyIntelObj*>(spyReport)); break;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion löscht alle Geheimdienstreports
void CIntelReports::RemoveAllReports()
{
	for (int i = 0; i < m_IntelObjects.GetSize(); i++)
		delete m_IntelObjects.GetAt(i);
	if (m_pAttemptObject)
	{
		delete m_pAttemptObject;
		m_pAttemptObject = NULL;
	}
	m_IntelObjects.RemoveAll();
}

/// Funktion legt einen bestimmten Geheimdienstbericht als aktiven Bericht fest.
void CIntelReports::SetActiveReport(short n)
{
	if (n < m_IntelObjects.GetSize())
		m_nActiveReport = n;
	else
		m_nActiveReport = m_IntelObjects.GetUpperBound();
}

void CIntelReports::SortAllReports(bool bDesc/* = true*/)
{
	if (bDesc)
		c_arraysort<CObArray, CIntelObject*>(m_IntelObjects, sort_pdesc);
	else
		c_arraysort<CObArray, CIntelObject*>(m_IntelObjects, sort_pasc);
}
