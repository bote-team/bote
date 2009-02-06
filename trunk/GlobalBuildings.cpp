#include "stdafx.h"
#include "GlobalBuildings.h"

IMPLEMENT_SERIAL (CGlobalBuildings, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CGlobalBuildings::CGlobalBuildings(void)
{
}

CGlobalBuildings::~CGlobalBuildings(void)
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CGlobalBuildings::CGlobalBuildings(const CGlobalBuildings &rhs)
{
	for (int i = 0; i < rhs.m_GlobalBuildings.GetSize(); i++)
		m_GlobalBuildings.Add(rhs.m_GlobalBuildings.GetAt(i));
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CGlobalBuildings & CGlobalBuildings::operator=(const CGlobalBuildings & rhs)
{
	if (this == &rhs)
		return *this;
	for (int i = 0; i < rhs.m_GlobalBuildings.GetSize(); i++)
		m_GlobalBuildings.Add(rhs.m_GlobalBuildings.GetAt(i));
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CGlobalBuildings::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	m_GlobalBuildings.Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
// Funktion löscht ein globales Gebäude aus dem Feld der globalen Gebäude. Diese Funktion sollte aufgerufen
// werden, wenn wir ein solches Gebäude abreißen oder verlieren.
void CGlobalBuildings::DeleteGlobalBuilding(USHORT id)
{
	for (int i = 0; i < m_GlobalBuildings.GetSize(); i++)
		if (m_GlobalBuildings.GetAt(i) == id)
		{
			m_GlobalBuildings.RemoveAt(i);
			break;
		}
}

// Resetfunktion für die Klasse CGlobalBuildings
void CGlobalBuildings::Reset()
{
	m_GlobalBuildings.RemoveAll();
}