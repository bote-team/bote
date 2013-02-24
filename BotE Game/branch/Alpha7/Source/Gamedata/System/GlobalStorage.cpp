#include "stdafx.h"
#include "GlobalStorage.h"
#include "System.h"

IMPLEMENT_SERIAL (CGlobalStorage, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CGlobalStorage::CGlobalStorage(void)
{
	this->Reset();
}

CGlobalStorage::~CGlobalStorage(void)
{
	m_ResOut.RemoveAll();
	m_ResIn.RemoveAll();
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CGlobalStorage::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);

	m_ResIn.Serialize(ar);
	m_ResOut.Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_byPercentLosing;
		ar << m_iMaxTakeFromStorage;
		ar << m_iTakeFromStorage;
		for (int i = TITAN; i <= IRIDIUM; i++)
			ar << m_nResourceStorages[i];
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_byPercentLosing;
		ar >> m_iMaxTakeFromStorage;
		ar >> m_iTakeFromStorage;
		for (int i = TITAN; i <= IRIDIUM; i++)
			ar >> m_nResourceStorages[i];
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Diese Funktion gibt die Menge der Ressource <code>res</code> zurück, die von dem System mit der Koordinate
/// <code>ko</code> in das globale Lager kommen sollen.
UINT CGlobalStorage::GetAddedResource(BYTE res, const CPoint& ko) const
{
	UINT resIn = 0;
	for (int i = 0; i < m_ResIn.GetSize(); i++)
		if (m_ResIn.GetAt(i).ptKO == ko && m_ResIn.GetAt(i).nRes == res)
		{
			resIn += m_ResIn.GetAt(i).nResTransfer;
			break;
		}
	return resIn;
}

/// Diese Funktion gibt die Menge der Ressource <code>res</code> zurück, die in das System mit der Koordinate
/// <code>ko</code> aus dem globale Lager kommen sollen.
UINT CGlobalStorage::GetSubResource(BYTE res, const CPoint& ko) const
{
	UINT resOut = 0;
	for (int i = 0; i < m_ResOut.GetSize(); i++)
		if (m_ResOut.GetAt(i).ptKO == ko && m_ResOut.GetAt(i).nRes == res)
		{
			resOut += m_ResOut.GetAt(i).nResTransfer;
			break;
		}
	return resOut;
}

/// Diese Funktion gibt die gesamte Menge der Ressource <code>res</code> zurück, die in das globale Lager kommen soll.
UINT CGlobalStorage::GetAllAddedResource(BYTE res) const
{
	UINT resIn = 0;
	for (int i = 0; i < m_ResIn.GetSize(); i++)
		if (m_ResIn.GetAt(i).nRes == res)
			resIn += m_ResIn.GetAt(i).nResTransfer;
	return resIn;
}

/// Diese Funktion gibt die gesamte Menge der Ressource <code>res</code> zurück, die aus dem globalen Lager entfernt
/// werden soll.
UINT CGlobalStorage::GetAllSubResource(BYTE res) const
{
	UINT resOut = 0;
	for (int i = 0; i < m_ResOut.GetSize(); i++)
		if (m_ResOut.GetAt(i).nRes == res)
			resOut += m_ResOut.GetAt(i).nResTransfer;
	return resOut;
}

/// Diese Funktion gibt <code>TRUE</code> zurück, wenn sich irgendeine Ressource im globalen Lager befindet.
BOOLEAN CGlobalStorage::IsFilled() const
{
	for (int i = TITAN; i <= IRIDIUM; i++)
		if (m_nResourceStorages[i] > NULL)
			return TRUE;
	return FALSE;
}

/// Diese Funktion addiert die mit dem Parameter <code>add</code> übergebende Anzahl der Ressource
/// <code>res</code> zum Lagerinhalt. Im Parameter <code>ko</code> wird die Systemkoordinate übergeben,
/// von wo aus man die Transaktion führt. Der Rückgabewert der Funktion ist eine Menge, einer
/// Ressource, die in der selben Runde schon aus dem globalen Lager in das System kommen soll.
UINT CGlobalStorage::AddRessource(UINT add, BYTE res, const CPoint& ko)
{
	UINT getBack = 0;
	// Zu Beginn in dem Feld suchen, ob man Ressourcen aus dem globalen Lager in der neuen Runde ins System
	// verschieben will. Diese werden zuerst entfernt.
	for (int i = 0; i < m_ResOut.GetSize(); i++)
		if (m_ResOut.GetAt(i).ptKO == ko && m_ResOut.GetAt(i).nRes == res)
		{
			// Wenn die Addition kleiner als die Menge der aus dem Lager gehenden Ressourcen ist
			if (add < m_ResOut.GetAt(i).nResTransfer)
			{
				m_ResOut.GetAt(i).nResTransfer -= add;
				m_iTakeFromStorage -= add;
				return add;
			}
			// Wenn die Addition gleich der Menge der aus dem Lager gehenden Ressourcen ist
			else if (add == m_ResOut.GetAt(i).nResTransfer)
			{
				m_ResOut.RemoveAt(i--);
				m_iTakeFromStorage -= add;
				return add;
			}
			// Wenn die Addition größer der Menge der aus dem Lager gehenden Ressourcen ist
			else
			{
			/*	add -= m_ResOut.GetAt(i).resTransfer;
				getBack = m_ResOut.GetAt(i).resTransfer;
				m_iTakeFromStorage -= getBack;
				m_ResOut.RemoveAt(i--);
				break;*/
				m_iTakeFromStorage -= m_ResOut.GetAt(i).nResTransfer;
				m_ResOut.RemoveAt(i--);
				return add;
			}
		}

	// Zuerst mal schauen, ob dieses System mit der Koordinate ko schon im Feld vorhanden ist. Wenn
	// dies der Fall ist, dann den Ressourceneintrag in diesem Feld erhöhen
	for (int i = 0; i < m_ResIn.GetSize(); i++)
		if (m_ResIn.GetAt(i).ptKO == ko && m_ResIn.GetAt(i).nRes == res)
		{
			m_ResIn.GetAt(i).nResTransfer += add;
			return getBack;
		}
	// Hat man das System noch nicht in dem Feld, dann neuen Eintrag erstellen
	StorageStruct ss;
	ss.ptKO = ko;
	ss.nRes = res;
	ss.nResTransfer = add;
	m_ResIn.Add(ss);
	return getBack;
}

/// Diese Funktion subtrahiert die mit dem Parameter <code>sub</code> übergebende Anzahl der Ressource
/// <code>res</code> vom Lagerinhalt. Im Parameter <code>ko</code> wird die Systemkoordinate übergeben, von wo aus
/// mam die Transaktion führt. Dabei beachtet die Funktion, dass nicht schon zuviel aus dem Lager genommen
/// wurde und das der Lagerinhalt nicht negativ werden kann. Der Rückgabewert der Funktion ist eine Menge, einer
/// Ressource, die in der selben Runde schon aus dem gleichen System addiert wurde. Somit kann man die Waren
/// auch sofort wieder herausnehmen.
UINT CGlobalStorage::SubRessource(UINT sub, BYTE res, const CPoint& ko)
{
	UINT getBack = 0;
	// Wenn man schon aus diesem System Ressourcen ins globale Lager verschieben möchte, so werden erstmal diese
	// Ressourcen abgezogen. Der Rest wird dann wirklich aus dem globalen Lager genommen.
	for (int i = 0; i < m_ResIn.GetSize(); i++)
		if (m_ResIn.GetAt(i).ptKO == ko && m_ResIn.GetAt(i).nRes == res)
		{
			// Wenn die Subtraktion kleiner als die Menge der in das Lager gehenden Ressourcen ist
			if (sub < m_ResIn.GetAt(i).nResTransfer)
			{
				m_ResIn.GetAt(i).nResTransfer -= sub;
				return sub;
			}
			// Wenn die Subtraktion gleich der Menge in das Lager gehenden Ressourcen ist
			else if (sub == m_ResIn.GetAt(i).nResTransfer)
			{
				m_ResIn.RemoveAt(i--);
				return sub;
			}
			// Wenn die Subtraktion größer der Menge in das Lager gehenden Ressourcen ist
			else
			{
				sub -= m_ResIn.GetAt(i).nResTransfer;
				getBack = m_ResIn.GetAt(i).nResTransfer;
				m_ResIn.RemoveAt(i--);
				break;
			}
		}
	// Überprüfen, dass nicht zuviel Ressourcen aus dem Lager in dieser Runde genommen werden können
	if ((m_iTakeFromStorage + sub) > m_iMaxTakeFromStorage)
		sub = m_iMaxTakeFromStorage - m_iTakeFromStorage;
	// Überprüfen, dass man nicht mehr Ressourcen aus dem Lager nehmen kann, als darin vorhanden sind
	if ((UINT)(sub + this->GetAllSubResource(res)) > m_nResourceStorages[res])
		sub = m_nResourceStorages[res] - this->GetAllSubResource(res);
	if (sub == 0)
		return getBack;
	m_iTakeFromStorage += sub;
	// Zuerst mal schauen, ob dieses System mit der Koordinate ko schon im Feld vorhanden ist. Wenn
	// dies der Fall ist, dann den Ressourceneintrag in diesem Feld erhöhen
	for (int i = 0; i < m_ResOut.GetSize(); i++)
		if (m_ResOut.GetAt(i).ptKO == ko && m_ResOut.GetAt(i).nRes == res)
		{
			m_ResOut.GetAt(i).nResTransfer += sub;
			return getBack;
		}
	// Hat man das System noch nicht in dem Feld, dann neuen Eintrag erstellen
	StorageStruct ss;
	ss.ptKO = ko;
	ss.nRes = res;
	ss.nResTransfer = sub;
	m_ResOut.Add(ss);
	return getBack;
}

/// Diese Funktion führt am Lagerinhalt alle möglichen Änderungen durch, die bei jeder neuen Runde eintreten
/// können. Dabei füllt sie auch die Lager der entsprechenden Systeme.
void CGlobalStorage::Calculate(std::vector<CSystem>& systems)
{
	m_iTakeFromStorage = 0;
	// zuerst werden die Ressourcen aus dem globalen Lager in die Systeme übertragen
	for (int i = 0; i < m_ResOut.GetSize(); )
	{
		// Sicherheitsabfrage, falls man doch durch irgendeinen Zufall mal mehr aus dem Lager nehmen würde als
		// darin vorhanden ist
		if (m_nResourceStorages[m_ResOut.GetAt(i).nRes] >= m_ResOut.GetAt(i).nResTransfer)
		{
			systems.at(m_ResOut.GetAt(i).ptKO.x+(m_ResOut.GetAt(i).ptKO.y)*STARMAP_SECTORS_HCOUNT).SetResourceStore(m_ResOut.GetAt(i).nRes, m_ResOut.GetAt(i).nResTransfer);
			m_nResourceStorages[m_ResOut.GetAt(i).nRes] -= m_ResOut.GetAt(i).nResTransfer;
		}
		else
		{
			systems.at(m_ResOut.GetAt(i).ptKO.x+(m_ResOut.GetAt(i).ptKO.y)*STARMAP_SECTORS_HCOUNT).SetResourceStore(m_ResOut.GetAt(i).nRes, m_nResourceStorages[m_ResOut.GetAt(i).nRes]);
			m_nResourceStorages[m_ResOut.GetAt(i).nRes] = 0;
		}
		m_ResOut.RemoveAt(i);
	}
	// dann wird der Inhalt des globalen Lagers mit den Ressourcen gefüllt, die aus den Systemen kommen
	for (int i = 0; i < m_ResIn.GetSize(); )
	{
		m_nResourceStorages[m_ResIn.GetAt(i).nRes] += m_ResIn.GetAt(i).nResTransfer;
		m_ResIn.RemoveAt(i);
	}
	// zuletzt verschwindet ein gewisser Anteil aus dem globalen Lager. Von jeder Ressource gehen "m_iPercentLosing"
	// Prozent verloren
	for (int i = TITAN; i <= IRIDIUM; i++)
		m_nResourceStorages[i] -= m_nResourceStorages[i] * m_byPercentLosing / 100;
}

/// Resetfunktion für die Klasse CGlobalStorage, welche alle Werte wieder auf Ausgangswerte setzt.
void CGlobalStorage::Reset()
{
	for (int i = TITAN; i <= IRIDIUM; i++)
		m_nResourceStorages[i] = 0;
	m_byPercentLosing = 15;
	m_iTakeFromStorage = 0;
	m_iMaxTakeFromStorage = 1000;
	m_ResOut.RemoveAll();
	m_ResIn.RemoveAll();
}
