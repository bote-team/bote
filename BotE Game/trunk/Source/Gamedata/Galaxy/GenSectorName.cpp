// GenSectorName.cpp: Implementierung der Klasse CGenSectorName.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IOData.h"
#include "GenSectorName.h"
#include "Loc.h"
#include "Constants.h"
#include "Galaxy/MapTile.h"

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

/// Funktion initialisiert alle möglichen Systemnamen.
/// @pram vMinorRaceSystemNames Namen aller Minorracesysteme
void CGenSectorName::Init(const CStringArray& vMinorRaceSystemNames, const std::vector<std::string>& forced_minors)
{
	ReadSectorNames();

	// zum Überprüfen das keine Systemnamen von kleinen Rassen bei den normalen Namen vorkommen
	std::set<CString> sNames;

	for (int i = 0; i < m_strNames.GetSize(); i++)
		sNames.insert(m_strNames.GetAt(i));

	for (int i = 0; i < vMinorRaceSystemNames.GetSize(); i++)
	{
		CString sMinorRaceSystemName = vMinorRaceSystemNames.GetAt(i);
		// prüfen ob der Name schon bei den normalen Systemnamen vorkam.
		if (sNames.find(sMinorRaceSystemName) != sNames.end())
		{
			CString sError;
			sError.Format("Warning\n:The race-systemname %s allready exists in normal systemnames.\nThe minor in system %s is not in this game!\n\nPlease remove one of them to solve that problem!", sMinorRaceSystemName, sMinorRaceSystemName);
			AfxMessageBox(sError);
			continue;
		}

		m_strRaceNames.Add(sMinorRaceSystemName);
	}

	m_ForcedMinors = forced_minors;
}

// Funktion zur Generierung der Sonnensystemnamen
CString CGenSectorName::GetNextRandomSectorName(const CPoint& ptKO, bool& bMinor)
{
	CString sName;

	if (bMinor && m_strRaceNames.GetSize() > 0)
	{
		if(!m_ForcedMinors.empty())
		{
			sName = StrToCStr(*m_ForcedMinors.begin());
			m_ForcedMinors.erase(m_ForcedMinors.begin());
			for(int i = 0; i < m_strRaceNames.GetSize(); ++i)
			{
				if(m_strRaceNames.GetAt(i) == sName)
				{
					m_strRaceNames.RemoveAt(i);
					return sName;
				}
			}
			AssertBotE(false);
		}
		else
		{
			int nRandom = rand()%m_strRaceNames.GetSize();
			sName = m_strRaceNames.GetAt(nRandom);
			m_strRaceNames.RemoveAt(nRandom);
		}
		return sName;
	}

	bMinor = false;
	// versuchen einen normalen Systemnamen zu verwenden
	if (m_strNames.GetSize() > 0)
	{
		int nRandom = rand()%m_strNames.GetSize();
		sName = m_strNames.GetAt(nRandom);
		m_strNames.RemoveAt(nRandom);
		return sName;
	}

	// Standardname zurückgeben
	sName = CMapTile::GenerateName("", ptKO);
	return sName;
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////

// Resetfunktion, setzt alle Werte wieder auf NULL
void CGenSectorName::ReadSectorNames(void)
{
	m_strNames.RemoveAll();

	// Standardnamen festlegen, alle Namen von Systemen werden aus Datei eingelesen
	CString csInput;						// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Names\\PlanetNames.data";	// Name des zu Öffnenden Files
	CStdioFile file;
	if (file.Open(fileName, CFile::modeRead | CFile::typeText) && m_strNames.IsEmpty())	// Datei wird geöffnet
		while (file.ReadString(csInput))
		{
			m_strNames.Add(csInput);			// Konnte erfolgreich gelesen werden wird die jeweilige
		}
	else
	{
		AfxMessageBox("Fehler! Datei \"PlanetNames.data\" kann nicht geöffnet werden...");
		exit(1);
	}

	// Datei wird geschlossen
	file.Close();
}
