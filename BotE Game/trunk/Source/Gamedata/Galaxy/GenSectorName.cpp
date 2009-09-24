// GenSectorName.cpp: Implementierung der Klasse CGenSectorName.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Botf2.h"
#include "GenSectorName.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_SERIAL (CGenSectorName, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CGenSectorName::CGenSectorName()
{
	
}

CGenSectorName::~CGenSectorName()
{
	for (int i = 0; i < m_strName.GetSize(); )
		m_strName.RemoveAt(i);
	for (int i = 0; i < m_strRaceName.GetSize(); )
		m_strRaceName.RemoveAt(i);
	m_strName.RemoveAll();
	m_strRaceName.RemoveAll();
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CGenSectorName::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		m_strName.Serialize(ar);
		m_strRaceName.Serialize(ar);
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		m_strName.RemoveAll();
		m_strName.Serialize(ar);
		m_strRaceName.RemoveAll();
		m_strRaceName.Serialize(ar);
	}
}

// Funktion zur Generierung der Sonnensystemnamen
CString CGenSectorName::GenerateSectorName(BOOLEAN IsMinorRaceInSector)
{
	/*
	 * Funktion gibt einen Namen für einen normalen Sektor oder einen Sektor mit
	 * Minorrace zurück. Wenn keine normalen Sektornamen mehr vorhanden sind, dann nimmt
	 * die Funktion notfalls auch den Namen eines Minorracesektors.
	 */
	CString systemName = "No Name";

	if (IsMinorRaceInSector == FALSE && m_strName.GetSize() > 0)
	{
		USHORT random = rand()%m_strName.GetSize();
		systemName = m_strName.GetAt(random);
		m_strName.RemoveAt(random);
	}
	else if (m_strRaceName.GetSize() > 0)
	{
		USHORT random = rand()%m_strRaceName.GetSize();
		systemName = m_strRaceName.GetAt(random);
		m_strRaceName.RemoveAt(random);
	}
	return systemName;
}

// Resetfunktion, setzt alle Werte wieder auf NULL
void CGenSectorName::ReadSystemNames()
{
	for (int i = 0; i < m_strName.GetSize(); )
		m_strName.RemoveAt(i);
	for (int i = 0; i < m_strRaceName.GetSize(); )
		m_strRaceName.RemoveAt(i);
	m_strName.RemoveAll();
	m_strRaceName.RemoveAll();
	
	// Standardnamen festlegen, alle Namen von Systemen werden aus Datei eingelesen
	CString csInput;						// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Names\\PlanetNames.data";	// Name des zu Öffnenden Files 
	CStdioFile file;						// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeText) && m_strName.IsEmpty())	// Datei wird geöffnet
		while (file.ReadString(csInput))
			m_strName.Add(csInput);			// Konnte erfolgreich gelesen werden wird die jeweilige
	else
	{	
		AfxMessageBox("Fehler! Datei \"PlanetNames.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen

// Systemnamen der MinorRaces einlesen
	fileName =*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Names\\RacePlanetNames.data";
	if (file.Open(fileName, CFile::modeRead | CFile::typeText) && m_strRaceName.IsEmpty())	// Datei wird geöffnet
		while (file.ReadString(csInput))
			m_strRaceName.Add(csInput);		// Konnte erfolgreich gelesen werden wird die jeweilige
	else
	{	
		AfxMessageBox("Fehler! Datei \"RacePlanetNames.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();
}
