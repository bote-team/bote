#include "stdafx.h"
#include "ShipHistory.h"

#include "Ships/Ships.h"

IMPLEMENT_SERIAL (CShipHistory, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CShipHistory::CShipHistory(void)
{
}

CShipHistory::~CShipHistory(void)
{
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
			ar << m_ShipHistory.GetAt(i).m_strTarget;
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
			ar >> shs.m_strTarget;
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
void CShipHistory::AddShip(const CShipHistoryStruct& ship, const CString& buildsector, short round)
{
	// Überprüfen, das dieses Schiff nicht schon in der Liste der Schiffe vorhanden ist
	for (int i = 0; i < m_ShipHistory.GetSize(); i++)
		if (ship.m_strShipName == m_ShipHistory.GetAt(i).m_strShipName)
		{
			MYTRACE("general")(MT::LEVEL_INFO, "Ship already exists in shiphistory: %d\n", ship.m_strShipName);
			return;
		}
	CShipHistoryStruct temp(ship);
	temp.m_strSectorName = buildsector;
	temp.m_strCurrentSector = buildsector;
	temp.m_iBuildRound = round;

	m_ShipHistory.Add(temp);
}

/// Funktion modifiziert den Eintrag in dem Feld <code>m_ShipHistory<code>. Dabei wird das Schiff übergeben, dessen
/// Werte modifiziert werden sollen. Wenn ein Schiff aus irgendeinem Grund (Kampf, Kolonisierung usw.) zerstört
/// wurde, wird für den Parameter <code>destroyRound<code> die aktuelle Runde der Zerstörung übergeben und
/// für den Parameter <code>destroyType<code> die Art der Zerstörung als CString übergeben. Außerdem wird der neue
/// Status des Schiffes im Parameter <code>status<code> übergeben, z.B. zerstört, vermisst usw.
/// Konnte das Schiff modifiziert werden, so gibt die Funktion <code>true</code> zurück, sonst <code>false</code>
bool CShipHistory::ModifyShip(const CShipHistoryStruct& ship, const CString& sector, short destroyRound,
	const CString& destroyType, const CString& status)
{
	for (int i = 0; i < m_ShipHistory.GetSize(); i++)
	{
		if (m_ShipHistory.GetAt(i).m_strShipName == ship.m_strShipName)
		{
			CShipHistoryStruct temp(ship);
			temp.m_strCurrentSector = sector;

			if (destroyRound != 0)
			{
				temp.m_iDestroyRound = destroyRound;
				temp.m_strKindOfDestroy = destroyType;
				temp.m_strSectorName = sector;
				temp.m_strCurrentTask = status;
			}
			m_ShipHistory.ElementAt(i) = temp;

			return true;
		}
	}
	return false;
}

/// Funktion gibt die Anzahl der noch lebenden Schiffe zurück, wenn der Parameter <code>shipAlive</code> wahr ist.
/// Ansonsten gibt die Funktion die Anzahl der zerstörten Schiffe zurück.
UINT CShipHistory::GetNumberOfShips(BOOLEAN shipAlive) const
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

