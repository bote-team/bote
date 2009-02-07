#include "stdafx.h"
#include "ShipHistory.h"

IMPLEMENT_SERIAL (CShipHistory, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CShipHistory::CShipHistory(void)
{
}

CShipHistory::~CShipHistory(void)
{
	m_ShipHistory.RemoveAll();
}
/*
//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CShipHistory::CShipHistory(const CShipHistory &rhs)
{
	
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CShipHistory & CShipHistory::operator=(const CShipHistory & rhs)
{
	if (this == &rhs)
		return *this;
	
	return *this;
}
*/

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CShipHistory::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_ShipHistory.GetSize();
		for (int i = 0; i < m_ShipHistory.GetSize(); i++)
		{
			ar << m_ShipHistory.GetAt(i).m_iBuildRound;
			ar << m_ShipHistory.GetAt(i).m_iDestroyRound;
			ar << m_ShipHistory.GetAt(i).m_iExperiance;
			ar << m_ShipHistory.GetAt(i).m_strCurrentSector;
			ar << m_ShipHistory.GetAt(i).m_strCurrentTask;
			ar << m_ShipHistory.GetAt(i).m_strKindOfDestroy;
			ar << m_ShipHistory.GetAt(i).m_strSectorName;
			ar << m_ShipHistory.GetAt(i).m_strShipClass;
			ar << m_ShipHistory.GetAt(i).m_strShipName;
			ar << m_ShipHistory.GetAt(i).m_strShipType;
		}
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		int number = 0;
		ar >> number;
		m_ShipHistory.RemoveAll();
		CShipHistoryStruct shs;
		for (int i = 0; i < number; i++)
		{
			ar >> shs.m_iBuildRound;
			ar >> shs.m_iDestroyRound;
			ar >> shs.m_iExperiance;
			ar >> shs.m_strCurrentSector;
			ar >> shs.m_strCurrentTask;
			ar >> shs.m_strKindOfDestroy;
			ar >> shs.m_strSectorName;
			ar >> shs.m_strShipClass;
			ar >> shs.m_strShipName;
			ar >> shs.m_strShipType;
			m_ShipHistory.Add(shs);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion fügt das Schiff, auf welches der übergebene Zeiger zeigt dem Feld hinzu. Alle Angaben werden
/// dabei automatisch gemacht. Es wird ebenfalls überprüft, dass dieses Schiff nicht schon hinzugefügt wurde.
/// Zusätzlich müssen als Parameter noch der Name des Systems übergeben werden, in dem das Schiff gebaut wurde,
/// sowie die aktuelle Runde.
void CShipHistory::AddShip(CShip* ship, CString buildsector, short round)
{
	// Überprüfen, das dieses Schiff nicht schon in der Liste der Schiffe vorhanden ist
	for (int i = 0; i < m_ShipHistory.GetSize(); i++)
		if (ship->GetShipName() == m_ShipHistory.GetAt(i).m_strShipName)
		{
			AfxMessageBox("BUG: Ship -" + ship->GetShipName() + "-  allready exists in shiphistory!\nPlease post a bugreport at www.birth-of-the-empires.de.vu");
			return;
		}
	CShipHistoryStruct temp;
	temp.m_strShipName = ship->GetShipName();
	temp.m_strShipType = ship->GetShipTypeAsString();
	temp.m_strShipClass = ship->GetShipClass();
	temp.m_strSectorName = buildsector;
	temp.m_strCurrentSector = buildsector;
	temp.m_strCurrentTask = ship->GetCurrentOrderAsString();
	temp.m_strKindOfDestroy = "";
	temp.m_iBuildRound = round;
	temp.m_iDestroyRound = 0;
	temp.m_iExperiance = ship->GetCrewExperience();
		
	m_ShipHistory.Add(temp);
}

/// Funktion modifiziert den Eintrag in dem Feld <code>m_ShipHistory<code>. Dabei wird das Schiff übergeben, dessen
/// Werte modifiziert werden sollen. Wenn ein Schiff aus irgendeinem Grund (Kampf, Kolonisierung usw.) zerstört
/// wurde, wird für den Parameter <code>destroyRound<code> die aktuelle Runde der Zerstörung übergeben und
/// für den Parameter <code>destroyType<code> die Art der Zerstörung als CString übergeben. Außerdem wird der neue
/// Status des Schiffes im Parameter <code>status<code> übergeben, z.B. zerstört, vermisst usw.
void CShipHistory::ModifyShip(CShip* ship, CString sector, short destroyRound, CString destroyType, CString status)
{
	for (int i = 0; i < m_ShipHistory.GetSize(); i++)
		if (m_ShipHistory.GetAt(i).m_strShipName == ship->GetShipName())
		{
			m_ShipHistory.ElementAt(i).m_strCurrentSector = sector;
			m_ShipHistory.ElementAt(i).m_strCurrentTask = ship->GetCurrentOrderAsString();
			m_ShipHistory.ElementAt(i).m_iExperiance = ship->GetCrewExperience();
			if (destroyRound != 0)
			{
				m_ShipHistory.ElementAt(i).m_iDestroyRound = destroyRound;
				m_ShipHistory.ElementAt(i).m_strKindOfDestroy = destroyType;
				m_ShipHistory.ElementAt(i).m_strSectorName = sector;
				m_ShipHistory.ElementAt(i).m_strCurrentTask = status;
			}
			break;
		}
}

/// Funktion entfernt ein bestimmtes Schiff aus der Schiffshistory.
void CShipHistory::RemoveShip(const CShip* ship)
{
	for (int i = 0; i < m_ShipHistory.GetSize(); i++)
		if (m_ShipHistory.GetAt(i).m_strShipName == ship->GetShipName())
		{
			m_ShipHistory.RemoveAt(i);
			return;
		}
	AfxMessageBox("BUG: Ship -" + ship->GetShipName() + "- doesn't exist in shiphistory!\nPlease post a bugreport at www.birth-of-the-empires.de.vu");
}

/// Funktion gibt die Anzahl der noch lebenden Schiffe zurück, wenn der Parameter <code>shipAlive</code> wahr ist.
/// Ansonsten gibt die Funktion die Anzahl der zerstörten Schiffe zurück.
UINT CShipHistory::GetNumberOfShips(BOOLEAN shipAlive)
{
	UINT number = 0;
	for (UINT i = 0; i < GetSizeOfShipHistory(); i++)
	{
		if (IsShipAlive(i) && shipAlive)
			number++;
		else if (!IsShipAlive(i) && !shipAlive)
			number++;
	}
	return number;
}

/// Resetfunktion für die CShipHistory Klasse
void CShipHistory::Reset(void)
{
	for (int i = 0; i < m_ShipHistory.GetSize(); )
		m_ShipHistory.RemoveAt(i);
	m_ShipHistory.RemoveAll();
}