// Sector.cpp: Implementierung der Klasse CSector.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sector.h"
#include "IOData.h"
#include "GenSectorName.h"
#include "FontLoader.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CSector, CObject, 1)

// statische Variablen initialisieren
CFont* CSector::m_Font = NULL;
COLORREF CSector::m_TextColor = RGB(0,0,0);

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CSector::CSector(void)
{	
}

CSector::~CSector(void)
{	
}

///////////////////////////////////////////////////////////////////////
// Speicher/Laden - Funktion
///////////////////////////////////////////////////////////////////////
void CSector::Serialize(CArchive &ar)
{
	// Funktion der Basisklasse aufrufen
	CObject::Serialize(ar);
	
	// Wird geschrieben?
	if (ar.IsStoring())
	// Alle Variablen in der richtigen Reihenfolge schreiben
	{
		ar << m_Attributes;
		ar << m_KO;
		
		// alle Maps speichern
		ar << m_byStatus.size();
		for (map<CString, BYTE>::const_iterator it = m_byStatus.begin(); it != m_byStatus.end(); it++)
			ar << it->first << it->second;
		ar << m_bShipPort.size();
		for (set<CString>::const_iterator it = m_bShipPort.begin(); it != m_bShipPort.end(); it++)
			ar << *it;
		ar << m_bOutpost.size();
		for (set<CString>::const_iterator it = m_bOutpost.begin(); it != m_bOutpost.end(); it++)
			ar << *it;
		ar << m_bStarbase.size();
		for (set<CString>::const_iterator it = m_bStarbase.begin(); it != m_bStarbase.end(); it++)
			ar << *it;
		ar << m_bIsStationBuild.size();
		for (set<CString>::const_iterator it = m_bIsStationBuild.begin(); it != m_bIsStationBuild.end(); it++)
			ar << *it;
		ar << m_bWhoIsOwnerOfShip.size();
		for (set<CString>::const_iterator it = m_bWhoIsOwnerOfShip.begin(); it != m_bWhoIsOwnerOfShip.end(); it++)
			ar << *it;
		ar << m_iNeededStationPoints.size();
		for (map<CString, short>::const_iterator it = m_iNeededStationPoints.begin(); it != m_iNeededStationPoints.end(); it++)
			ar << it->first << it->second;
		ar << m_iStartStationPoints.size();
		for (map<CString, short>::const_iterator it = m_iStartStationPoints.begin(); it != m_iStartStationPoints.end(); it++)
			ar << it->first << it->second;
		ar << m_iScanPower.size();
		for (map<CString, short>::const_iterator it = m_iScanPower.begin(); it != m_iScanPower.end(); it++)
			ar << it->first << it->second;
		ar << m_iNeededScanPower.size();
		for (map<CString, short>::const_iterator it = m_iNeededScanPower.begin(); it != m_iNeededScanPower.end(); it++)
			ar << it->first << it->second;
		ar << m_byOwnerPoints.size();
		for (map<CString, BYTE>::const_iterator it = m_byOwnerPoints.begin(); it != m_byOwnerPoints.end(); it++)
			ar << it->first << it->second;
		
		ar << m_sColonyOwner;
		ar << m_sOwnerOfSector;		
		
		// Nur wenn ein Sonnensystem in dem Sektor ist müssen die folgenden Variablen gespeichert werden
		if (GetSunSystem())
		{
			ar << m_strSectorName;
			ar << m_bySunColor;
			ar << m_Planets.GetSize();
			for (int i = 0; i < m_Planets.GetSize(); i++)
				m_Planets.GetAt(i).Serialize(ar);
		}		
	}
	else
	// Alle Variablen in der richtigen Reihenfolge lesen
	{
		int number = 0;
		ar >> m_Attributes;
		ar >> m_KO;
		
		// Maps laden
		m_byStatus.clear();
		size_t mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			BYTE value;
			ar >> key;
			ar >> value;
			m_byStatus[key] = value;
		}
		m_bShipPort.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString value;
			ar >> value;
			m_bShipPort.insert(value);
		}
		m_bOutpost.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString value;
			ar >> value;
			m_bOutpost.insert(value);
		}
		m_bStarbase.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString value;
			ar >> value;
			m_bStarbase.insert(value);
		}
		m_bIsStationBuild.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString value;
			ar >> value;
			m_bIsStationBuild.insert(value);
		}
		m_bWhoIsOwnerOfShip.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString value;
			ar >> value;
			m_bWhoIsOwnerOfShip.insert(value);
		}
		m_iNeededStationPoints.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			short value;
			ar >> key;
			ar >> value;
			m_iNeededStationPoints[key] = value;
		}
		m_iStartStationPoints.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			short value;
			ar >> key;
			ar >> value;
			m_iStartStationPoints[key] = value;
		}
		m_iScanPower.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			short value;
			ar >> key;
			ar >> value;
			m_iScanPower[key] = value;
		}
		m_iNeededScanPower.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			short value;
			ar >> key;
			ar >> value;
			m_iNeededScanPower[key] = value;
		}
		m_byOwnerPoints.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			BYTE value;
			ar >> key;
			ar >> value;
			m_byOwnerPoints[key] = value;
		}

		ar >> m_sColonyOwner;
		ar >> m_sOwnerOfSector;		
		// Nur wenn ein Sonnensystem in dem Sektor ist müssen die folgenden Variablen geladen werden
		if (GetSunSystem())
		{
			ar >> m_strSectorName;
			ar >> m_bySunColor;
			ar >> number;
			m_Planets.RemoveAll();
			m_Planets.SetSize(number);
			for (int i = 0; i < number; i++)
				m_Planets.GetAt(i).Serialize(ar);
		}
		else
		{
			m_strSectorName = "";
			m_bySunColor = 0;
			m_Planets.RemoveAll();
		}		
		m_Font = NULL;
		m_iShipPathPoints = 0;
	}
}

//////////////////////////////////////////////////////////////////
// Zugriffsfunktionen
//////////////////////////////////////////////////////////////////
/// Funktion gibt den Namen des Sektors zurück. Wenn in ihm kein Sonnensystem ist, dann wird "" zurückgegeben.
/// Wenn man aber den Parameter <code>longName<code> beim Aufruf der Funktion auf <code>TRUE<code> setzt, wird
/// versucht ein genauerer Sektorname zu generieren.
CString CSector::GetName(BOOLEAN longName) const
{
	if (!longName)
		return m_strSectorName;
	else
	{
		if (GetSunSystem())
			return m_strSectorName;
		else
		{
			CString s;
			s.Format("%s %c%i", CResourceManager::GetString("SECTOR"), (char)(m_KO.y+97), m_KO.x + 1);
			return s;
		}
	}
}

/// Funktion gibt alle Einwohner aller Planeten in dem Sektor zurück.
float CSector::GetCurrentHabitants() const
{
	float currentHabitants = 0.0f;
	for (int i = 0; i < m_Planets.GetSize(); i++)
		currentHabitants += m_Planets.GetAt(i).GetCurrentHabitant();
	return currentHabitants;
}

/// Diese Funktion berechnet die vorhandenen Rohstoffe der Planeten im Sektor. Übergebn wird dafür ein Feld für
/// die Ressourcen <code>res</code>.
void CSector::GetAvailableResources(BOOLEAN bResources[DILITHIUM + 1], BOOLEAN bOnlyColonized/* = true */)
{
	for (int i = 0; i < m_Planets.GetSize(); i++)
	{
		CPlanet* pPlanet = &m_Planets[i];
		if (!pPlanet->GetHabitable())
			continue;
		
		// wenn nur kolonisierte Planeten betrachtet werden sollen und der Planet nicht kolonisiert ist,
		// dann nächsten Planeten betrachten
		if (bOnlyColonized && !pPlanet->GetColonized())
			continue;

		BOOLEAN bExists[DILITHIUM + 1] = {FALSE};
		pPlanet->GetAvailableResources(bExists);
		for (int res = TITAN; res <= DILITHIUM; res++)
			bResources[res] |= bExists[res];
	}
}

//////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////

/// Funktion generiert den Sektor. Dabei wird als Parameter die Wahrscheinlichkeit, ob in dem Sektor ein
/// Sonnensystem ist, im Paramter <code>sunProb</code> in Prozent übergeben. Im Parameter <code>minorProb</code>
/// wird die Wahrscheinlichkeit in Prozent übergeben, dass sich in dem Sektor eine Minorrace befindet.
void CSector::GenerateSector(int sunProb, int minorProb)
{
	if (GetSunSystem())
		return;

	// Zahl[0,99] generieren und vergleichen (Sonnensystem?)
	if (rand()%100 >= (100 - sunProb))
	{
		SetSunSystem(TRUE);
		// Zahl[0,99] generieren und vergleichen (Minorrace?)
		if (rand()%100 >= (100 - minorProb))
		{
			SetMinorRace(TRUE);
			// Wenn eine kleine Rasse drauf lebt
			m_strSectorName = CGenSectorName::GetInstance()->GetNextRandomSectorName(true);
			float currentHabitants = 0.0f;
			USHORT random = rand()%3+1;
			// Solange Planeten generieren, bis mind. eine zufällige Anzahl Bevölkerung darauf leben
			do 
			{												
				this->CreatePlanets();
				currentHabitants = this->GetCurrentHabitants();
				if (currentHabitants > 20.000f)
					break;
				// Wenn schon Bevölkerung vorhanden ist und die maximale Bevölkerungsanzahl hoch genug ist,
				// so wird abgebrochen
				if (currentHabitants > 0.0f)
				{
					float maxHabitants = 0.0f;
					for (int i = 0; i < m_Planets.GetSize(); i++)
						maxHabitants += m_Planets.GetAt(i).GetMaxHabitant();
					if (maxHabitants > (40.000f + random * 7))
						break;
				}				
			} while (currentHabitants <= (15.000f / random));
		}
		// wenn keine Minorrace drauf lebt
		else
		{
			m_strSectorName = CGenSectorName::GetInstance()->GetNextRandomSectorName(false);
			this->CreatePlanets();
		}
	}
}

/// Diese Funktion generiert die Planeten in dem Sektor.
void CSector::CreatePlanets(const CString& sMajorID)
{
	for (int i = 0; i < m_Planets.GetSize(); )
		m_Planets.RemoveAt(i);
	m_Planets.RemoveAll();
	
	if (GetSunSystem())
	{
		// Es gibt 7 verschiedene Sonnenfarben
		m_bySunColor = rand()%7;

		if (sMajorID == "MAJOR1")
		{
			CPlanet planet;
			m_bySunColor = 6;
			// Merkur generieren
			planet.SetType(PLANETCLASS_B);
			planet.SetColonisized(FALSE);
			planet.SetMaxHabitant(0);
			planet.SetCurrentHabitant(0);
			planet.SetHabitable(FALSE);
			planet.SetTerraformed(FALSE);
			planet.SetSize(SMALL);
			planet.SetName("Merkur");
			planet.SetClass('B');
			planet.SetPlanetGrowth();
			m_Planets.Add(planet);
			// Venus generieren
			planet.SetType(PLANETCLASS_N);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(10.000);
			planet.SetCurrentHabitant(4.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(SMALL);
			planet.SetName("Venus");
			planet.SetClass('N');
			planet.SetPlanetGrowth();
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,0,0,0,0,0,1);
			// Erde generieren
			planet.SetType(PLANETCLASS_M);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(19.000);
			planet.SetCurrentHabitant(7.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(SMALL);
			planet.SetName("Erde");
			planet.SetClass('M');
			planet.SetPlanetGrowth();
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,0,0,0,1,1,0);
			// Mars generieren
			planet.SetType(PLANETCLASS_K);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(13.000);
			planet.SetCurrentHabitant(5.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(SMALL);
			planet.SetName("Mars");
			planet.SetClass('K');
			planet.SetPlanetGrowth();
			m_Planets.Add(planet);
			// Jupiter generieren
			planet.SetType(PLANETCLASS_J);
			planet.SetColonisized(FALSE);
			planet.SetMaxHabitant(0);
			planet.SetCurrentHabitant(0);
			planet.SetHabitable(FALSE);
			planet.SetTerraformed(FALSE);
			planet.SetSize(GIANT);
			planet.SetName("Jupiter");
			planet.SetClass('J');
			planet.SetPlanetGrowth();
			m_Planets.Add(planet);
			// Saturn generieren
			planet.SetType(PLANETCLASS_J);
			planet.SetColonisized(FALSE);
			planet.SetMaxHabitant(0);
			planet.SetCurrentHabitant(0);
			planet.SetHabitable(FALSE);
			planet.SetTerraformed(FALSE);
			planet.SetSize(GIANT);
			planet.SetName("Saturn");
			planet.SetClass('J');
			planet.SetPlanetGrowth();
			m_Planets.Add(planet);
			// Uranus generieren
			planet.SetType(PLANETCLASS_J);
			planet.SetColonisized(FALSE);
			planet.SetMaxHabitant(0);
			planet.SetCurrentHabitant(0);
			planet.SetHabitable(FALSE);
			planet.SetTerraformed(FALSE);
			planet.SetSize(BIG);
			planet.SetName("Uranus");
			planet.SetClass('J');
			planet.SetPlanetGrowth();
			m_Planets.Add(planet);
			// Neptune generieren
			planet.SetType(PLANETCLASS_J);
			planet.SetColonisized(FALSE);
			planet.SetMaxHabitant(0);
			planet.SetCurrentHabitant(0);
			planet.SetHabitable(FALSE);
			planet.SetTerraformed(FALSE);
			planet.SetSize(BIG);
			planet.SetName("Neptun");
			planet.SetClass('J');
			planet.SetPlanetGrowth();
			m_Planets.Add(planet);
			// Pluto generieren
			planet.SetType(PLANETCLASS_C);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(8.000);
			planet.SetCurrentHabitant(2.250);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(SMALL);
			planet.SetName("Pluto");
			planet.SetClass('C');
			planet.SetPlanetGrowth();
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(1,0,0,0,0,0,0,0);
		}
		else if (sMajorID == "MAJOR2")
		{
			CPlanet planet;
			m_bySunColor = 2;
			// Ferenginar 1 generieren
			planet.SetType(PLANETCLASS_B);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(0);
			planet.SetCurrentHabitant(0);
			planet.SetHabitable(FALSE);
			planet.SetTerraformed(FALSE);
			planet.SetSize(BIG);
			planet.SetName(m_strSectorName + " 1");
			planet.SetClass('B');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			// Ferenginar 2 generieren
			planet.SetType(PLANETCLASS_M);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(16.500);
			planet.SetCurrentHabitant(5.500);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(BIG);
			planet.SetName(m_strSectorName + " 2");
			planet.SetClass('M');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,0,0,0,1,1,0);
			// Ferenginar 3 generieren
			planet.SetType(PLANETCLASS_O);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(12.000);
			planet.SetCurrentHabitant(4.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(BIG);
			planet.SetName(m_strSectorName + " 3");
			planet.SetClass('O');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,1,0,0,0,0,1,0);
			// Ferenginar 4 generieren
			planet.SetType(PLANETCLASS_L);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(8.500);
			planet.SetCurrentHabitant(3.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(NORMAL);
			planet.SetName(m_strSectorName + " 4");
			planet.SetClass('L');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			// Ferenginar 5 generieren
			planet.SetType(PLANETCLASS_Q);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(7.500);
			planet.SetCurrentHabitant(1.500);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(NORMAL);
			planet.SetName(m_strSectorName + " 5");
			planet.SetClass('Q');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			// Ferenginar 6 generieren
			planet.SetType(PLANETCLASS_S);
			planet.SetColonisized(FALSE);
			planet.SetMaxHabitant(0);
			planet.SetCurrentHabitant(0);
			planet.SetHabitable(FALSE);
			planet.SetTerraformed(FALSE);
			planet.SetSize(GIANT);
			planet.SetName(m_strSectorName + " 6");
			planet.SetClass('S');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			// Ferenginar 7 generieren
			planet.SetType(PLANETCLASS_C);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(2.500);
			planet.SetCurrentHabitant(0.500);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(SMALL);
			planet.SetName(m_strSectorName + " 7");
			planet.SetClass('C');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
		}
		else if (sMajorID == "MAJOR3")
		{
			CPlanet planet;
			m_bySunColor = 2;
			// Qo´nos 1 generieren
			planet.SetType(PLANETCLASS_N);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(8.000);
			planet.SetCurrentHabitant(2.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(BIG);
			planet.SetName(m_strSectorName + " 1");
			planet.SetClass('N');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,0,0,0,0,0,1);
			// Qo´nos 2 generieren
			planet.SetType(PLANETCLASS_H);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(15.000);
			planet.SetCurrentHabitant(3.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(BIG);
			planet.SetName(m_strSectorName + " 2");
			planet.SetClass('H');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,0,0,0,0,0,1);
			// Kling generieren
			planet.SetType(PLANETCLASS_M);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(25.000);
			planet.SetCurrentHabitant(5.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(BIG);
//			planet.SetName("Klinzhai");
			planet.SetName(m_strSectorName + " 3");
			planet.SetClass('M');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,0,0,0,1,1,0);
			// Qo´nos 4 generieren
			planet.SetType(PLANETCLASS_P);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(12.000);
			planet.SetCurrentHabitant(2.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(BIG);
			planet.SetName(m_strSectorName + " 4");
			planet.SetClass('P');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,0,0,0,0,0,1);
		}
		else if (sMajorID == "MAJOR4")
		{
			CPlanet planet;
			m_bySunColor = 1;
			// Romulus 1 generieren
			planet.SetType(PLANETCLASS_N);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(2.000);
			planet.SetCurrentHabitant(1.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(NORMAL);
			planet.SetName(m_strSectorName + " 1");
			planet.SetClass('N');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,0,0,0,0,0,1);
			// Romulus generieren
			planet.SetType(PLANETCLASS_M);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(28.000);
			planet.SetCurrentHabitant(7.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(GIANT);
	//		planet.SetName("Romulus");
			planet.SetName(m_strSectorName + " 2");
			planet.SetClass('M');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,0,0,0,1,1,0);
			// Remus generieren
			planet.SetType(PLANETCLASS_Q);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(8.000);
			planet.SetCurrentHabitant(2.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(NORMAL);
	//		planet.SetName("Remus");
			planet.SetName(m_strSectorName + " 3");
			planet.SetClass('Q');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			// Romulus 4 generieren
			planet.SetType(PLANETCLASS_P);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(14.000);
			planet.SetCurrentHabitant(4.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(BIG);
			planet.SetName(m_strSectorName + " 4");
			planet.SetClass('P');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,0,0,0,0,0,1);			
		}
		else if (sMajorID == "MAJOR5")
		{
			CPlanet planet;
			m_bySunColor = 4;
			// Cardassia Prime generieren
			planet.SetType(PLANETCLASS_M);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(11.000);
			planet.SetCurrentHabitant(7.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(NORMAL);
			planet.SetName(m_strSectorName + " Prime");
			planet.SetClass('M');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,0,0,0,1,1,0);
			// Cardassia 2
			planet.SetType(PLANETCLASS_G);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(3.000);
			planet.SetCurrentHabitant(1.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(GIANT);
			planet.SetName(m_strSectorName + " 2");
			planet.SetClass('G');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			// Cardassia 3
			planet.SetType(PLANETCLASS_M);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(17.000);
			planet.SetCurrentHabitant(4.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(BIG);
			planet.SetName(m_strSectorName + " 3");
			planet.SetClass('M');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			// Cardassia 4
			planet.SetType(PLANETCLASS_M);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(19.000);
			planet.SetCurrentHabitant(4.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(BIG);
			planet.SetName(m_strSectorName + " 4");
			planet.SetClass('M');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			// Cardassia 5
			planet.SetType(PLANETCLASS_M);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(8.000);
			planet.SetCurrentHabitant(2.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(SMALL);
			planet.SetName(m_strSectorName + " 5");
			planet.SetClass('M');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,0,0,0,0,1,0);
			// Cardassia 6
			planet.SetType(PLANETCLASS_J);
			planet.SetColonisized(FALSE);
			planet.SetMaxHabitant(0);
			planet.SetCurrentHabitant(0);
			planet.SetHabitable(FALSE);
			planet.SetTerraformed(FALSE);
			planet.SetSize(GIANT);
			planet.SetName(m_strSectorName + " 6");
			planet.SetClass('J');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			// Cardassia 7
			planet.SetType(PLANETCLASS_P);
			planet.SetColonisized(FALSE);
			planet.SetMaxHabitant(7.000);
			planet.SetCurrentHabitant(0);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(FALSE);
			planet.SetSize(NORMAL);
			planet.SetName(m_strSectorName + " 7");
			planet.SetClass('P');
			planet.SetStartTerraformPoints(6);
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,0,0,0,0,0,1);
		}
		else if (sMajorID == "MAJOR6")
		{
			CPlanet planet;
			m_bySunColor = 5;
			// Omarion 1 genereriren
			planet.SetType(PLANETCLASS_B);
			planet.SetColonisized(FALSE);
			planet.SetMaxHabitant(0);
			planet.SetCurrentHabitant(0);
			planet.SetHabitable(FALSE);
			planet.SetTerraformed(FALSE);
			planet.SetSize(SMALL);
			planet.SetName(m_strSectorName + " 1");
			planet.SetClass('B');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			// Omarion 2 genereriren
			planet.SetType(PLANETCLASS_M);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(18.000);
			planet.SetCurrentHabitant(6.500);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(BIG);
			planet.SetName(m_strSectorName + " 2");
			planet.SetClass('M');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,0,0,0,1,1,0);
			// Omarion 3 (Kurill)
			planet.SetType(PLANETCLASS_L);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(15.000);
			planet.SetCurrentHabitant(3.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(NORMAL);
	//		planet.SetName("Kurill");
			planet.SetName(m_strSectorName + " 3");
			planet.SetClass('L');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			// Omarion 4
			planet.SetType(PLANETCLASS_S);
			planet.SetColonisized(FALSE);
			planet.SetMaxHabitant(0);
			planet.SetCurrentHabitant(0);
			planet.SetHabitable(FALSE);
			planet.SetTerraformed(FALSE);
			planet.SetSize(GIANT);
			planet.SetName(m_strSectorName + " 4");
			planet.SetClass('S');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			// Omarion 5
			planet.SetType(PLANETCLASS_T);
			planet.SetColonisized(FALSE);
			planet.SetMaxHabitant(0);
			planet.SetCurrentHabitant(0);
			planet.SetHabitable(FALSE);
			planet.SetTerraformed(FALSE);
			planet.SetSize(GIANT);
			planet.SetName(m_strSectorName + " 5");
			planet.SetClass('T');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			// Omarion 6
			planet.SetType(PLANETCLASS_P);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(15.000);
			planet.SetCurrentHabitant(2.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(BIG);
			planet.SetName(m_strSectorName + " 6");
			planet.SetClass('P');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,0,0,0,0,0,1);
			// Omarion 7 (Gründerwelt)
			planet.SetType(PLANETCLASS_R);
			planet.SetColonisized(TRUE);
			planet.SetMaxHabitant(7.000);
			planet.SetCurrentHabitant(1.000);
			planet.SetHabitable(TRUE);
			planet.SetTerraformed(TRUE);
			planet.SetSize(BIG);
//			planet.SetName("Founderhome");
			planet.SetName(m_strSectorName + " 7");
			planet.SetClass('R');
			planet.SetPlanetGrowth();
			planet.SetGraphicType(rand()%GRAPHICNUMBER);
			m_Planets.Add(planet);
			m_Planets.GetAt(m_Planets.GetUpperBound()).SetBoni(0,0,1,0,0,0,0,0);
		}
		else
		{
			// dreimal die Zufallsfunktion aufgerufen, damit die mittlere Planetenanzahl häufiger als ganz wenig oder
			// sehr viele Planeten vorkommt.
			short number = (rand()%8+1 + rand()%8+1 + rand()%8+1 + 1) / 3;
			BYTE zone = HOT;
			for (int i = 0; i < number; i++)
			{
				CPlanet planet;
				zone = planet.Create(m_strSectorName, zone, i, GetMinorRace());
				m_Planets.Add(planet);				
			}
		}
	}
}

/// Diese Funktion führt das Planetenwachstum für diesen Sektor durch.
void CSector::LetPlanetsGrowth()
{
	for (int i = 0; i < m_Planets.GetSize(); i++)
		m_Planets.GetAt(i).PlanetGrowth();
}

/// Diese Funktion lässt die Bevölkerung auf allen Planeten zusammen um den übergebenen Wert <code>Value</code>
/// schrumpfen.
void CSector::LetPlanetsShrink(float Value)
{
	// aktuelle Einwohner auf den einzelnen Planeten
	float* Habitants = new float[m_Planets.GetSize()];

	// alle Einwohner im Sector
	float allHabitants = 0.0f;
	for (int i = 0; i < m_Planets.GetSize(); i++)
	{
		allHabitants += m_Planets[i].GetCurrentHabitant();
		Habitants[i] = m_Planets[i].GetCurrentHabitant();
	}
	for (int i = 0; i < m_Planets.GetSize(); i++)
		if (Habitants[i] > 0)
		{
			float prozAnteil = Habitants[i] / allHabitants;
			Habitants[i] += prozAnteil * Value;
			m_Planets[i].SetCurrentHabitant(Habitants[i]);
			if (m_Planets[i].GetCurrentHabitant() <= 0)
			{
				m_Planets[i].SetCurrentHabitant(0);
				m_Planets[i].SetColonisized(FALSE);
			}
		}
	delete Habitants;
}

/// In jeder neuen Runde die IsTerraforming und IsStationBuilding Variablen auf FALSE setzen, wenn Schiffe eine Aktion
/// machen, werden diese Variablen später ja wieder korrekt gesetzt. Außerdem werden auch die Besitzerpunkte wieder
/// gelöscht.
void CSector::ClearAllPoints()
{
	// Funktion bei jeder neuen Runde anfangs aufrufen!!! Wenn nämlich in diesem Sektor gerade keine Station einer
	// Rasse gebaut wird, dann setzen wir auch die noch gebrauchten Punkte und die anfänglich gebrauchten Punkte
	// wieder auf NULL
	
	// Falls der Planet gerade geterraformt wird, wird er hier erstmal wieder auf FALSE gesetzt.
	for (int i = 0; i < m_Planets.GetSize(); i++)
		m_Planets[i].SetIsTerraforming(FALSE);
	
	m_byOwnerPoints.clear();
	
	// nun können alle StartStationPoint die auf 0 stehen in der Map gelöscht werden
	for (map<CString, short>::iterator it = m_iStartStationPoints.begin(); it != m_iStartStationPoints.end(); )
	{
		if (m_bIsStationBuild.find(it->first) == m_bIsStationBuild.end())
			it->second = 0;

		if (it->second == 0)
			it = m_iStartStationPoints.erase(it++);
		else
			++it;
	}
	m_bIsStationBuild.clear();
	
	m_iShipPathPoints = 0;

	m_bWhoIsOwnerOfShip.clear();
	// Die benötigte Scanpower um Schiffe sehen zu können wieder auf NULL setzen
	m_iNeededScanPower.clear();
	m_iScanPower.clear();
	// Sagen das erstmal kein Außenposten und keine Sternbasis in dem Sektor steht
	m_bOutpost.clear();
	m_bStarbase.clear();
	m_bShipPort.clear();	
}

/// Diese Funktion berechnet anhand der Besitzerpunkte und anderen Enflüssen, wem dieser Sektor schlussendlich
/// gehört.
void CSector::CalculateOwner(const CString& sSystemOwner)
{
	// Wenn in diesem Sektor das System jemanden gehört, oder hier eine Schiffswerft durch Außenposten oder Sternbasis
	// steht, dann ändert sich nichts am Besitzer
	if (sSystemOwner != "")
	{
		SetOwned(TRUE);
		m_sOwnerOfSector = sSystemOwner;
		return;		
	}
	// Sektor gehört einer Minorrace
	else if (m_sOwnerOfSector != "" && sSystemOwner == "" && this->GetMinorRace() == TRUE)
		return;
	
	for (set<CString>::const_iterator it = m_bShipPort.begin(); it != m_bShipPort.end(); it++)
	{
		SetOwned(TRUE);
		m_sOwnerOfSector = *it;
		return;
	}
	
	// Ist obiges nicht eingetreten, so gehört demjenigen der Sektor, wer die meisten Besitzerpunkte hat. Ist hier
	// Gleichstand haben wir neutrales Gebiet. Es werden mindst. 2 Punkte benötigt, um als neuer Besitzer des Sektors
	// zu gelten.
	BYTE mostPoints = 1;
	CString newOwner = "";
	for (map<CString, BYTE>::const_iterator it = m_byOwnerPoints.begin(); it != m_byOwnerPoints.end(); it++)
	{
		if (it->second > mostPoints)
		{
			mostPoints = it->second;
			newOwner = it->first;
		}
		// bei Gleichstand wird der Besitzer wieder gelöscht
		else if (it->second == mostPoints)
			newOwner = "";
	}
	if (newOwner != "")
	{
		SetOwned(TRUE);
		SetScanned(newOwner);
	}
	else
		SetOwned(FALSE);
	m_sOwnerOfSector = newOwner;
}

/// Resetfunktion für die Klasse CSector
void CSector::Reset()
{	
	m_Attributes = 0;
	
	// Maps löschen
	m_byStatus.clear();
	m_iScanPower.clear();
	m_iNeededScanPower.clear();
	m_bShipPort.clear();
	m_bWhoIsOwnerOfShip.clear();
	m_bOutpost.clear();
	m_bStarbase.clear();
	m_bIsStationBuild.clear();
	m_iStartStationPoints.clear();
	m_iNeededStationPoints.clear();
	m_byOwnerPoints.clear();
	
	m_sOwnerOfSector = "";
	m_sColonyOwner = "";
	m_strSectorName = "";
	m_iShipPathPoints = 0;	
	m_Planets.RemoveAll();
}

////////////////////////////////////////////////////////////////
// Zeichenfunktionen
////////////////////////////////////////////////////////////////
/// Diese Funktion zeichnet den Namen des Sektors.
void CSector::DrawSectorsName(CDC *pDC, CBotf2Doc* pDoc, CMajor* pPlayer)
{
	// befindet sich ein Sonnensystem im Sektor
	if (!GetSunSystem())
		return;

	ASSERT(pDC);
	ASSERT(pDoc);
	ASSERT(pPlayer);

	// Ist Sektor bekannt dann zeichne den Systemnamen
	if (this->GetKnown(pPlayer->GetRaceID()) == TRUE)
	{
		if (!m_Font)
		{
			m_Font = new CFont();
			m_TextColor = CFontLoader::CreateFont(pPlayer, 0, 0, m_Font);
		}
		CFont* oldfont = pDC->SelectObject(m_Font);
		pDC->SetTextColor(m_TextColor);
		pDC->SetBkMode(TRANSPARENT);
		CMajor* pOwner = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(pDoc->GetSystem(m_KO).GetOwnerOfSystem()));
		if (pOwner)
		{
			if (pPlayer->IsRaceContacted(pOwner->GetRaceID()) == true || pPlayer->GetRaceID() == pOwner->GetRaceID())
				pDC->SetTextColor(pOwner->GetDesign()->m_clrGalaxySectorText);			
		}
		// Systemnamen zeichnen
		pDC->DrawText(m_strSectorName,
		//	CRect(m_KO.x*STARMAP_SECTOR_WIDTH-50,m_KO.y*STARMAP_SECTOR_HEIGHT,m_KO.x*STARMAP_SECTOR_WIDTH+90,m_KO.y*STARMAP_SECTOR_HEIGHT+40),
		CRect(m_KO.x*STARMAP_SECTOR_WIDTH, m_KO.y*STARMAP_SECTOR_HEIGHT, m_KO.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH,m_KO.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT),
			DT_CENTER | DT_BOTTOM | DT_SINGLELINE);
		pDC->SelectObject(oldfont);		
	}	
}

/// Diese Funktion zeichnet die entsprechenden Schiffssymbole in den Sektor
void CSector::DrawShipSymbolInSector(Graphics *g, CBotf2Doc* pDoc, CMajor* pPlayer)
{
	ASSERT(g);
	ASSERT(pDoc);
	ASSERT(pPlayer);

	// alle Rassen holen
	map<CString, CRace*>* pmRaces = pDoc->GetRaceCtrl()->GetRaces();
	ASSERT(pmRaces);
	
	CString sFilePath;
	short nCount = 0;
	
	CPoint pt;
	pt.x = m_KO.x * STARMAP_SECTOR_WIDTH;
	pt.y = m_KO.y * STARMAP_SECTOR_HEIGHT;

	// durch alle Rassen iterieren und Schiffsymbole zeichnen
	CString sAppPath = CIOData::GetInstance()->GetAppPath();
	for (map<CString, CRace*>::const_iterator it = pmRaces->begin(); it != pmRaces->end(); it++)
	{		
		if (pPlayer->GetRaceID() == it->first && this->GetOwnerOfShip(it->first) == TRUE
			|| this->GetOwnerOfShip(it->first) == TRUE && this->GetNeededScanPower(it->first) < this->GetScanPower(pPlayer->GetRaceID()))
		{
			// ist der besitzer des Schiffes unbekannt, dann Fragezeichen zeichnen
			if (pPlayer != it->second && pPlayer->IsRaceContacted(it->first) == false)
				sFilePath = sAppPath + "Graphics\\Symbols\\Unknown.bop";
			else
				sFilePath = sAppPath + "Graphics\\Symbols\\" + it->first + ".bop";
			
			Bitmap* ship = Bitmap::FromFile(sFilePath.AllocSysString());
			// konnte die Grafik nicht geladen werden, dann wird ein Standardsymbol geladen
			if (!ship || ship->GetLastStatus() != Ok)
			{
				sFilePath = sAppPath + "Graphics\\Symbols\\Default.bop";
				ship = Bitmap::FromFile(sFilePath.AllocSysString());
			}
			g->DrawImage(ship, pt.x + 45 - nCount * 12, pt.y, 35, 35);
			delete ship;
			nCount++;
		}
		
		// Jetzt werden die Stationen wenn möglich gezeichnet
		if ((pPlayer->GetRaceID() == it->first || this->GetScanPower(pPlayer->GetRaceID()) > 0) &&
			(this->GetIsStationBuilding(it->first) == TRUE || this->GetOutpost(it->first) == TRUE || this->GetStarbase(it->first) == TRUE))
		{
			// ist der besitzer des Schiffes unbekannt, dann Fragezeichen zeichnen
			if (pPlayer != it->second && pPlayer->IsRaceContacted(it->first) == false)
				sFilePath = sAppPath + "Graphics\\Symbols\\Unknown.bop";
			else
				sFilePath = sAppPath + "Graphics\\Symbols\\" + it->first + ".bop";
			
			Bitmap* ship = Bitmap::FromFile(sFilePath.AllocSysString());
			// konnte die Grafik nicht geladen werden, dann wird ein Standardsymbol geladen
			if (!ship || ship->GetLastStatus() != Ok)
			{
				sFilePath = sAppPath + "Graphics\\Symbols\\Default.bop";
				ship = Bitmap::FromFile(sFilePath.AllocSysString());
			}
			g->DrawImage(ship, pt.x, pt.y + 45, 35, 35);
			delete ship;
		}
	}
}