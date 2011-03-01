// GenSectorName.cpp: Implementierung der Klasse CGenSectorName.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IOData.h"
#include "GenSectorName.h"
#include <set>

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
	CString sSystemName = "No Name";

	if (!bMinor && m_strNames.GetSize() > 0)
	{
		int nRandom = rand()%m_strNames.GetSize();
		sSystemName = m_strNames.GetAt(nRandom);
		m_strNames.RemoveAt(nRandom);
	}
	else if (m_strRaceNames.GetSize() > 0)
	{
		int nRandom = rand()%m_strRaceNames.GetSize();
		sSystemName = m_strRaceNames.GetAt(nRandom);
		m_strRaceNames.RemoveAt(nRandom);
	}

	return sSystemName;
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////

// Resetfunktion, setzt alle Werte wieder auf NULL
void CGenSectorName::ReadSystemNames(void)
{
	for (int i = 0; i < m_strNames.GetSize(); )
		m_strNames.RemoveAt(i);
	for (int i = 0; i < m_strRaceNames.GetSize(); )
		m_strRaceNames.RemoveAt(i);
	m_strNames.RemoveAll();
	m_strRaceNames.RemoveAll();

	// zum Überprüfen das keine Systemnamen von kleinen Rassen bei den normalen Namen vorkommen
	std::set<CString> sNames;	
	
	// Standardnamen festlegen, alle Namen von Systemen werden aus Datei eingelesen
	CString csInput;						// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Names\\PlanetNames.data";	// Name des zu Öffnenden Files 
	CStdioFile file;						// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeText) && m_strNames.IsEmpty())	// Datei wird geöffnet
		while (file.ReadString(csInput))
		{
			m_strNames.Add(csInput);			// Konnte erfolgreich gelesen werden wird die jeweilige
			sNames.insert(csInput);
		}
	else
	{	
		AfxMessageBox("Fehler! Datei \"PlanetNames.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen

	// Systemnamen der MinorRaces einlesen
	fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Names\\RacePlanetNames.data";
	if (file.Open(fileName, CFile::modeRead | CFile::typeText) && m_strRaceNames.IsEmpty())	// Datei wird geöffnet
		while (file.ReadString(csInput))
		{
			// prüfen ob der Name schon bei den normalen Systemnamen vorkam.
			if (sNames.find(csInput) != sNames.end())
			{
				CString sError;
				sError.Format("Warning\n:The race-systemname %s allready exists in normal systemnames.\nThe minor in system %s is not in this game!\n\nPlease remove one of them to solve that problem!", csInput, csInput);
				AfxMessageBox(sError);
				continue;
			}
			m_strRaceNames.Add(csInput);		// Konnte erfolgreich gelesen werden wird die jeweilige
		}
	else
	{	
		AfxMessageBox("Fehler! Datei \"RacePlanetNames.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();
}
