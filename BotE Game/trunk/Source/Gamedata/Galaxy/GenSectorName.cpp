// GenSectorName.cpp: Implementierung der Klasse CGenSectorName.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IOData.h"
#include "GenSectorName.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CGenSectorName::CGenSectorName(void)
{
	ReadSystemNames();
}

CGenSectorName::~CGenSectorName(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
/// @return Instanz dieser Klasse
CGenSectorName* CGenSectorName::GetInstance(void)
{
	static CGenSectorName instance; 
    return &instance;
}

// Funktion zur Generierung der Sonnensystemnamen
CString CGenSectorName::GetNextRandomSectorName(bool bMinor)
{
	/*
	 * Funktion gibt einen Namen für einen normalen Sektor oder einen Sektor mit
	 * Minorrace zurück. Wenn keine normalen Sektornamen mehr vorhanden sind, dann nimmt
	 * die Funktion notfalls auch den Namen eines Minorracesektors.
	 */
	CString systemName = "No Name";

	if (!bMinor && m_strName.GetSize() > 0)
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

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////

// Resetfunktion, setzt alle Werte wieder auf NULL
void CGenSectorName::ReadSystemNames(void)
{
	for (int i = 0; i < m_strName.GetSize(); )
		m_strName.RemoveAt(i);
	for (int i = 0; i < m_strRaceName.GetSize(); )
		m_strRaceName.RemoveAt(i);
	m_strName.RemoveAll();
	m_strRaceName.RemoveAll();
	
	// Standardnamen festlegen, alle Namen von Systemen werden aus Datei eingelesen
	CString csInput;						// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Names\\PlanetNames.data";	// Name des zu Öffnenden Files 
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
	fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Names\\RacePlanetNames.data";
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
