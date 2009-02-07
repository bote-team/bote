// Sector.cpp: Implementierung der Klasse CSector.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "stdafx.h"
#include "Sector.h"
#include "FontLoader.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CSector, CObject, 1)

// statische Variablen initialisieren
CFont* CSector::m_Font = NULL;
CGenSectorName* CSector::m_NameGenerator = NULL;
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
// Kopierkonstruktor
///////////////////////////////////////////////////////////////////////
CSector::CSector(const CSector & rhs)
{
	m_Attributes = rhs.m_Attributes;
	m_KO = rhs.m_KO;
	for (int i = 0; i <= DOMINION; i++)
	{
		m_byStatus[i] = rhs.m_byStatus[i];
		m_iScanPower[i] = rhs.m_iScanPower[i];
		m_iNeededScanPower[i] = rhs.m_iNeededScanPower[i];
		m_bShipPort[i] = rhs.m_bShipPort[i];
		m_bOutpost[i] = rhs.m_bOutpost[i];
		m_bStarbase[i] = rhs.m_bStarbase[i];
		m_bIsStationBuild[i] = rhs.m_bIsStationBuild[i];
		m_bWhoIsOwnerOfShip[i] = rhs.m_bWhoIsOwnerOfShip[i];
		m_iNeededStationPoints[i] = rhs.m_iNeededStationPoints[i];
		m_iStartStationPoints[i] = rhs.m_iStartStationPoints[i];
		m_byOwnerPoints[i] = rhs.m_byOwnerPoints[i];
	}
	m_byOwnerOfSector = rhs.m_byOwnerOfSector;
	m_strSectorName = rhs.m_strSectorName;
	m_bySunColor = rhs.m_bySunColor;
	m_byColonyOwner = rhs.m_byColonyOwner;
	m_iShipPathPoints = rhs.m_iShipPathPoints;	// muss nicht serialisiert werden
	for (int i = 0; i < rhs.m_Planets.GetSize(); i++)
		m_Planets.Add(rhs.m_Planets.GetAt(i));	
}

///////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
///////////////////////////////////////////////////////////////////////
CSector & CSector::operator=(const CSector & rhs)
{
	if (this == &rhs)
		return *this;
	m_Attributes = rhs.m_Attributes;
	m_KO = rhs.m_KO;
	for (int i = 0; i <= DOMINION; i++)
	{
		m_byStatus[i] = rhs.m_byStatus[i];
		m_iScanPower[i] = rhs.m_iScanPower[i];
		m_iNeededScanPower[i] = rhs.m_iNeededScanPower[i];
		m_bShipPort[i] = rhs.m_bShipPort[i];
		m_bOutpost[i] = rhs.m_bOutpost[i];
		m_bStarbase[i] = rhs.m_bStarbase[i];
		m_bIsStationBuild[i] = rhs.m_bIsStationBuild[i];
		m_bWhoIsOwnerOfShip[i] = rhs.m_bWhoIsOwnerOfShip[i];
		m_iNeededStationPoints[i] = rhs.m_iNeededStationPoints[i];
		m_iStartStationPoints[i] = rhs.m_iStartStationPoints[i];
		m_byOwnerPoints[i] = rhs.m_byOwnerPoints[i];
	}
	m_byOwnerOfSector = rhs.m_byOwnerOfSector;
	m_strSectorName = rhs.m_strSectorName;
	m_bySunColor = rhs.m_bySunColor;
	m_byColonyOwner = rhs.m_byColonyOwner;
	m_iShipPathPoints = rhs.m_iShipPathPoints;	// muss nicht serialisiert werden
	for (int i = 0; i < rhs.m_Planets.GetSize(); i++)
		m_Planets.Add(rhs.m_Planets.GetAt(i));	
	return *this;
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
		for (int i = 0; i <= DOMINION; i++)
		{
			ar << m_byStatus[i];
			ar << m_bShipPort[i];
			ar << m_bOutpost[i];
			ar << m_bStarbase[i];
			ar << m_bIsStationBuild[i];
			ar << m_bWhoIsOwnerOfShip[i];
			ar << m_iNeededStationPoints[i];
			ar << m_iStartStationPoints[i];
			ar << m_iScanPower[i];
			ar << m_iNeededScanPower[i];
			ar << m_byOwnerPoints[i];
		}
		ar << m_byColonyOwner;
		ar << m_byOwnerOfSector;		
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
		for (int i = 0; i <= DOMINION; i++)
		{
			ar >> m_byStatus[i];
			ar >> m_bShipPort[i];
			ar >> m_bOutpost[i];
			ar >> m_bStarbase[i];
			ar >> m_bIsStationBuild[i];
			ar >> m_bWhoIsOwnerOfShip[i];
			ar >> m_iNeededStationPoints[i];
			ar >> m_iStartStationPoints[i];
			ar >> m_iScanPower[i];
			ar >> m_iNeededScanPower[i];
			ar >> m_byOwnerPoints[i];
		}
		ar >> m_byColonyOwner;
		ar >> m_byOwnerOfSector;		
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
			s.Format("Sektor %c%i",(char)(m_KO.y+97),m_KO.x+1);
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
void CSector::GetAvailableResources(BOOLEAN res[5])
{
	for (int i = 0; i < m_Planets.GetSize(); i++)
	{	// steht so auch in der BuildBuildingsForMinorRace() Funktion in Klasse CSystem, also wenn hier was geändert auch dort ändern!!
		CPlanet planet = m_Planets.GetAt(i);
		if (planet.GetClass() == 'C' && planet.GetColonized() == TRUE)
		{	res[0] |= TRUE;  res[1] |= FALSE; res[2] |= FALSE; res[3] |= FALSE; res[4] |= TRUE;}
		else if (planet.GetClass() == 'F' && planet.GetColonized() == TRUE)
		{	res[0] |= TRUE;  res[1] |= FALSE; res[2] |= TRUE;  res[3] |= FALSE; res[4] |= FALSE;}
		else if (planet.GetClass() == 'G' && planet.GetColonized() == TRUE)
		{	res[0] |= TRUE;  res[1] |= FALSE; res[2] |= TRUE;  res[3] |= TRUE;  res[4] |= FALSE;}
		else if (planet.GetClass() == 'H' && planet.GetColonized() == TRUE)
		{	res[0] |= FALSE; res[1] |= FALSE; res[2] |= FALSE; res[3] |= FALSE; res[4] |= TRUE;}
		else if (planet.GetClass() == 'K' && planet.GetColonized() == TRUE)
		{	res[0] |= TRUE;  res[1] |= FALSE; res[2] |= TRUE;  res[3] |= FALSE; res[4] |= FALSE;}
		else if (planet.GetClass() == 'L' && planet.GetColonized() == TRUE)
		{	res[0] |= TRUE;  res[1] |= TRUE;  res[2] |= FALSE; res[3] |= FALSE; res[4] |= FALSE;}
		else if (planet.GetClass() == 'M' && planet.GetColonized() == TRUE)
		{	res[0] |= TRUE;  res[1] |= TRUE;  res[2] |= TRUE;  res[3] |= TRUE;  res[4] |= TRUE;}
		else if (planet.GetClass() == 'N' && planet.GetColonized() == TRUE)
		{	res[0] |= FALSE; res[1] |= TRUE;  res[2] |= FALSE; res[3] |= FALSE; res[4] |= FALSE;}
		else if (planet.GetClass() == 'O' && planet.GetColonized() == TRUE)
		{	res[0] |= FALSE; res[1] |= TRUE;  res[2] |= FALSE; res[3] |= FALSE; res[4] |= FALSE;}
		else if (planet.GetClass() == 'P' && planet.GetColonized() == TRUE)
		{	res[0] |= TRUE;  res[1] |= FALSE; res[2] |= FALSE; res[3] |= TRUE;  res[4] |= TRUE;}
		else if (planet.GetClass() == 'Q' && planet.GetColonized() == TRUE)
		{	res[0] |= FALSE;  res[1] |= FALSE; res[2] |= FALSE; res[3] |= TRUE;  res[4] |= FALSE;}
		else if (planet.GetClass() == 'R' && planet.GetColonized() == TRUE)
		{	res[0] |= FALSE; res[1] |= FALSE; res[2] |= TRUE;  res[3] |= FALSE; res[4] |= FALSE;}
	}
}

//////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////

/// Funktion initialisiert den Namensgenerator für die CSector Klasse. Muss zu Beginn aufgerufen werden.
void CSector::InitNameGenerator()
{
	if (CSector::m_NameGenerator == NULL)
	{
		CSector::m_NameGenerator = new CGenSectorName();
		CSector::m_NameGenerator->ReadSystemNames();
	}
}

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
			m_strSectorName = CSector::m_NameGenerator->GenerateSectorName(TRUE);
			m_byOwnerOfSector = UNKNOWN;
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
					if (maxHabitants > (40.000f + random*7))
						currentHabitants = 0.0f;
				}				
			} while (currentHabitants <= (15.000f/random));
		}
		// wenn keine Minorrace drauf lebt
		else
		{
			m_strSectorName = CSector::m_NameGenerator->GenerateSectorName(FALSE);
			this->CreatePlanets();
		}
	}
}

/// Diese Funktion generiert die Planeten in dem Sektor.
void CSector::CreatePlanets(BYTE majorNumber)
{
	ASSERT(majorNumber <= DOMINION);

	for (int i = 0; i < m_Planets.GetSize(); )
		m_Planets.RemoveAt(i);
	m_Planets.RemoveAll();
	
	if (GetSunSystem())
	{
		// Es gibt 7 verschiedene Sonnenfarben
		m_bySunColor = rand()%7;

		if (majorNumber == HUMAN)
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
		else if (majorNumber == FERENGI)
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
		else if (majorNumber == KLINGON)
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
		else if (majorNumber == ROMULAN)
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
		else if (majorNumber == CARDASSIAN)
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
		else if (majorNumber == DOMINION)
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
				CPlanet* planet = new CPlanet();
				zone = planet->GeneratePlanet(zone, m_strSectorName, i, GetMinorRace());
				m_Planets.Add(*planet);
				delete planet;
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
	memset(Habitants, 0, m_Planets.GetSize() * sizeof(*Habitants));
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
			float prozAnteil = Habitants[i]/allHabitants;
			m_Planets[i].SetCurrentHabitant(Habitants[i] += prozAnteil*Value);
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
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		m_byOwnerPoints[i] = 0;
		if (m_bIsStationBuild[i] == FALSE)
			this->SetStartStationPoints(-1,i);
		else
			m_bIsStationBuild[i] = FALSE;
	}
	m_iShipPathPoints = 0;
}

/// Diese Funktion berechnet anhand der Besitzerpunkte und anderen Enflüssen, wem dieser Sektor schlussendlich
/// gehört.
void CSector::CalculateOwner(BYTE systemOwner)
{
	// Wenn in diesem Sektor das System jemanden gehört, oder hier eine Schiffswerft durch Außenposten oder Sternbasis
	// steht, dann ändert sich nichts am Besitzer
	if (systemOwner != NOBODY)
	{
		SetOwned(TRUE);
		m_byOwnerOfSector = systemOwner;
		return;		
	}
	// Sektor gehört einer Minorrace
	else if (this->m_byOwnerOfSector == UNKNOWN)
		return;
	for (int i = HUMAN; i <= DOMINION; i++)
		if (m_bShipPort[i] == TRUE)
		{
			SetOwned(TRUE);
			m_byOwnerOfSector = i;
			return;
		}
	// Ist obiges nicht eingetreten, so gehört demjenigen der Sektor, wer die meisten Besitzerpunkte hat. Ist hier
	// Gleichstand haben wir neutrales Gebiet. Es werden mindst. 2 Punkte benötigt, um als neuer Besitzer des Sektors
	// zu gelten.
	BYTE mostPoints = 1;
	BYTE newOwner = NOBODY;
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		if (m_byOwnerPoints[i] > mostPoints)
		{
			mostPoints = m_byOwnerPoints[i];
			newOwner = i;
		}
		// bei Gleichstand wird der Besitzer wieder gelöscht
		else if (m_byOwnerPoints[i] == mostPoints)
			newOwner = NOBODY;
	}
	if (newOwner != NOBODY)
	{
		SetOwned(TRUE);
		SetScanned(newOwner);
	}
	else
		SetOwned(FALSE);
	m_byOwnerOfSector = newOwner;
}

/// Resetfunktion für die Klasse CSector
void CSector::Reset()
{
	m_Attributes = 0;
	for (int z = 0; z <= DOMINION; z++)
	{
		m_byStatus[z] = 0;
		m_iScanPower[z] = 0;
		m_iNeededScanPower[z] = MAXSHORT;
		m_bShipPort[z] = FALSE;
		m_bWhoIsOwnerOfShip[z] = FALSE;
		m_bOutpost[z] = FALSE;
		m_bStarbase[z] = FALSE;
		m_bIsStationBuild[z] = FALSE;
		m_iStartStationPoints[z] = -1;
		m_iNeededStationPoints[z] = 0;
		m_byOwnerPoints[z] = 0;
	}
	m_byOwnerOfSector = NOBODY;
	m_byColonyOwner = NOBODY;
	m_strSectorName = "";
	m_iShipPathPoints = 0;	
	m_Planets.RemoveAll();
}

////////////////////////////////////////////////////////////////
// Zeichenfunktionen
////////////////////////////////////////////////////////////////
/// Diese Funktion zeichnet den Namen des Sektors.
void CSector::DrawSectorsName(CDC *pDC, BYTE ownerOfSystem, int playersRace, BOOLEAN knowOwner) const
{
	// Ist Sektor bekannt und es befindet sich ein Sonnensystem darin, zeichne den Systemnamen
	if (GetSunSystem() && this->GetKnown(playersRace) == TRUE)
	{
		if (!m_Font)
		{
			m_Font = new CFont();
			m_TextColor = CFontLoader::CreateFont(playersRace, 0, 0, m_Font);
		}
		CFont* oldfont = pDC->SelectObject(m_Font);
		pDC->SetTextColor(m_TextColor);
		pDC->SetBkMode(TRANSPARENT);
		if (ownerOfSystem == HUMAN && knowOwner == TRUE)
			pDC->SetTextColor(RGB(0,200,255));
		else if (ownerOfSystem == FERENGI && knowOwner == TRUE)
			pDC->SetTextColor(RGB(220,220,0));
		else if (ownerOfSystem == KLINGON && knowOwner == TRUE)
			pDC->SetTextColor(RGB(225,20,25));
		else if (ownerOfSystem == ROMULAN && knowOwner == TRUE)
			pDC->SetTextColor(RGB(50,170,0));
		else if (ownerOfSystem == CARDASSIAN && knowOwner == TRUE)
			pDC->SetTextColor(RGB(180,180,180));
		else if (ownerOfSystem == DOMINION && knowOwner == TRUE)
			pDC->SetTextColor(RGB(200,225,255));
		// Systemnamen zeichnen
		pDC->DrawText(m_strSectorName,
		//	CRect(m_KO.x*STARMAP_SECTOR_WIDTH-50,m_KO.y*STARMAP_SECTOR_HEIGHT,m_KO.x*STARMAP_SECTOR_WIDTH+90,m_KO.y*STARMAP_SECTOR_HEIGHT+40),
		CRect(m_KO.x*STARMAP_SECTOR_WIDTH, m_KO.y*STARMAP_SECTOR_HEIGHT, m_KO.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH,m_KO.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT),
			DT_CENTER | DT_BOTTOM | DT_SINGLELINE);
		pDC->SelectObject(oldfont);		
	}
	// Schiffssymbole im Sektor zeichnen
	DrawShipSymbolInSector(pDC, playersRace);
}

/// Diese Funktion zeichnet die entsprechenden Schiffssymbole in den Sektor
void CSector::DrawShipSymbolInSector(CDC* pDC, int PlayersRace) const
{
	// MultiRace == TRUE, wenn Schiffe verschiedener Rassen in dem System sind
	pDC->SetBkMode(TRANSPARENT);
	CString s = "";
	CRect ShipSymbol;
	USHORT count = 0;
	CPoint KO;
	KO.x = m_KO.x * STARMAP_SECTOR_WIDTH;
	KO.y = m_KO.y * STARMAP_SECTOR_HEIGHT;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT)); 
	strcpy_s(lf.lfFaceName, "Trekbats");
	if (m_bWhoIsOwnerOfShip[HUMAN] == TRUE && m_iNeededScanPower[HUMAN] < m_iScanPower[PlayersRace]
		|| PlayersRace == HUMAN && m_bWhoIsOwnerOfShip[HUMAN] == TRUE)
	{
		CFont font;
		lf.lfHeight = 18;
		font.CreateFontIndirect(&lf);
		s.Format("S");
	//	pDC->SetTextColor(RGB(227,231,255));
		pDC->SetTextColor(RGB(90,180,255));
		CFont* oldfont = pDC->SelectObject(&font);
		ShipSymbol.SetRect(KO.x, KO.y, KO.x+STARMAP_SECTOR_WIDTH, KO.y+STARMAP_SECTOR_HEIGHT);
		count++;
		pDC->DrawText(s, ShipSymbol, DT_RIGHT | DT_TOP | DT_SINGLELINE);
		pDC->SelectObject(oldfont);
	}
	if (m_bWhoIsOwnerOfShip[FERENGI] == TRUE && m_iNeededScanPower[FERENGI] < m_iScanPower[PlayersRace]
		|| PlayersRace == FERENGI && m_bWhoIsOwnerOfShip[FERENGI] == TRUE)
	{
		CFont font;
		lf.lfHeight = 17;
		font.CreateFontIndirect(&lf);
		s.Format("A");
		pDC->SetTextColor(RGB(22,183,32));
		CFont* oldfont = pDC->SelectObject(&font);
		ShipSymbol.SetRect(KO.x, KO.y, KO.x+STARMAP_SECTOR_WIDTH-count*7, KO.y+STARMAP_SECTOR_HEIGHT);
		count++;
		pDC->DrawText(s, ShipSymbol, DT_RIGHT | DT_TOP | DT_SINGLELINE);
		pDC->SelectObject(oldfont);
	}
	if (m_bWhoIsOwnerOfShip[KLINGON] == TRUE && m_iNeededScanPower[KLINGON] < m_iScanPower[PlayersRace]
		|| PlayersRace == KLINGON && m_bWhoIsOwnerOfShip[KLINGON] == TRUE)
	{
		CFont font;
		lf.lfHeight = 19;
		font.CreateFontIndirect(&lf);
		s.Format("K");
		pDC->SetTextColor(RGB(216,32,38));
		CFont* oldfont = pDC->SelectObject(&font);
		ShipSymbol.SetRect(KO.x, KO.y, KO.x+STARMAP_SECTOR_WIDTH-count*7, KO.y+STARMAP_SECTOR_HEIGHT);
		count++;
		pDC->DrawText(s, ShipSymbol, DT_RIGHT | DT_TOP | DT_SINGLELINE);
		pDC->SelectObject(oldfont);
	}
	if (m_bWhoIsOwnerOfShip[ROMULAN] == TRUE && m_iNeededScanPower[ROMULAN] < m_iScanPower[PlayersRace]
		|| PlayersRace == ROMULAN && m_bWhoIsOwnerOfShip[ROMULAN] == TRUE)
	{
		CFont font;
		lf.lfHeight = 19;
		font.CreateFontIndirect(&lf);
		s.Format("R");
		pDC->SetTextColor(RGB(86,221,188));
		CFont* oldfont = pDC->SelectObject(&font);
		ShipSymbol.SetRect(KO.x, KO.y, KO.x+STARMAP_SECTOR_WIDTH-count*7, KO.y+STARMAP_SECTOR_HEIGHT);
		count++;
		pDC->DrawText(s, ShipSymbol, DT_RIGHT | DT_TOP | DT_SINGLELINE);
		pDC->SelectObject(oldfont);
	}
	if (m_bWhoIsOwnerOfShip[CARDASSIAN] == TRUE && m_iNeededScanPower[CARDASSIAN] < m_iScanPower[PlayersRace]
		|| PlayersRace == CARDASSIAN && m_bWhoIsOwnerOfShip[CARDASSIAN] == TRUE)
	{
		CFont font;
		lf.lfHeight = 23;
		font.CreateFontIndirect(&lf);
		s.Format("C");
		pDC->SetTextColor(RGB(224,141,24));
		CFont* oldfont = pDC->SelectObject(&font);
		ShipSymbol.SetRect(KO.x, KO.y, KO.x+STARMAP_SECTOR_WIDTH-count*7, KO.y+STARMAP_SECTOR_HEIGHT);
		count++;
		pDC->DrawText(s, ShipSymbol, DT_RIGHT | DT_TOP | DT_SINGLELINE);
		pDC->SelectObject(oldfont);
	}
	if (m_bWhoIsOwnerOfShip[DOMINION] == TRUE && m_iNeededScanPower[DOMINION] < m_iScanPower[PlayersRace]
		|| PlayersRace == DOMINION && m_bWhoIsOwnerOfShip[DOMINION] == TRUE)
	{
		CFont font;
		lf.lfHeight = 23;
		font.CreateFontIndirect(&lf);
		s.Format("D");
		pDC->SetTextColor(RGB(255,255,255));
		CFont* oldfont = pDC->SelectObject(&font);
		ShipSymbol.SetRect(KO.x, KO.y, KO.x+STARMAP_SECTOR_WIDTH-count*7, KO.y+STARMAP_SECTOR_HEIGHT);
		count++;
		pDC->DrawText(s, ShipSymbol, DT_RIGHT | DT_TOP | DT_SINGLELINE);
		pDC->SelectObject(oldfont);
	}
	count = 0;
	
	// Jetzt werden die Stationen wenn möglich gezeichnet
	if ((m_bIsStationBuild[HUMAN] == TRUE || m_bOutpost[HUMAN] == TRUE || m_bStarbase[HUMAN] == TRUE) &&
		(PlayersRace == HUMAN || m_iScanPower[PlayersRace] > 0))
	{
		CFont font;
		lf.lfHeight = 18;
		font.CreateFontIndirect(&lf);
		s.Format("S");
		pDC->SetTextColor(RGB(90,180,255));
		CFont* oldfont = pDC->SelectObject(&font);
		ShipSymbol.SetRect(KO.x, KO.y, KO.x+STARMAP_SECTOR_WIDTH, KO.y+STARMAP_SECTOR_HEIGHT);
		count++;
		pDC->DrawText(s, ShipSymbol, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
		pDC->SelectObject(oldfont);
	}
	if ((m_bIsStationBuild[FERENGI] == TRUE || m_bOutpost[FERENGI] == TRUE || m_bStarbase[FERENGI] == TRUE) &&
		(PlayersRace == FERENGI || m_iScanPower[PlayersRace] > 0))
	{
		CFont font;
		lf.lfHeight = 17;
		font.CreateFontIndirect(&lf);
		s.Format("A");
		pDC->SetTextColor(RGB(22,183,32));
		CFont* oldfont = pDC->SelectObject(&font);
		ShipSymbol.SetRect(KO.x, KO.y, KO.x+STARMAP_SECTOR_WIDTH-count*7, KO.y+STARMAP_SECTOR_HEIGHT);
		count++;
		pDC->DrawText(s, ShipSymbol, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
		pDC->SelectObject(oldfont);
	}
	if ((m_bIsStationBuild[KLINGON] == TRUE || m_bOutpost[KLINGON] == TRUE || m_bStarbase[KLINGON] == TRUE) &&
		(PlayersRace == KLINGON || m_iScanPower[PlayersRace] > 0))
	{
		CFont font;
		lf.lfHeight = 19;
		font.CreateFontIndirect(&lf);
		s.Format("K");
		pDC->SetTextColor(RGB(216,32,38));
		CFont* oldfont = pDC->SelectObject(&font);
		ShipSymbol.SetRect(KO.x, KO.y, KO.x+STARMAP_SECTOR_WIDTH-count*7, KO.y+STARMAP_SECTOR_HEIGHT);
		count++;
		pDC->DrawText(s, ShipSymbol, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
		pDC->SelectObject(oldfont);
	}
	if ((m_bIsStationBuild[ROMULAN] == TRUE || m_bOutpost[ROMULAN] == TRUE || m_bStarbase[ROMULAN] == TRUE) &&
		(PlayersRace == ROMULAN || m_iScanPower[PlayersRace] > 0))
	{
		CFont font;
		lf.lfHeight = 19;
		font.CreateFontIndirect(&lf);
		s.Format("R");
		pDC->SetTextColor(RGB(86,221,188));
		CFont* oldfont = pDC->SelectObject(&font);
		ShipSymbol.SetRect(KO.x, KO.y, KO.x+STARMAP_SECTOR_WIDTH-count*7, KO.y+STARMAP_SECTOR_HEIGHT);
		count++;
		pDC->DrawText(s, ShipSymbol, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
		pDC->SelectObject(oldfont);
	}
	if ((m_bIsStationBuild[CARDASSIAN] == TRUE || m_bOutpost[CARDASSIAN] == TRUE || m_bStarbase[CARDASSIAN] == TRUE) &&
		(PlayersRace == CARDASSIAN || m_iScanPower[PlayersRace] > 0))
	{
		CFont font;
		lf.lfHeight = 23;
		font.CreateFontIndirect(&lf);
		s.Format("C");
		pDC->SetTextColor(RGB(224,141,24));
		CFont* oldfont = pDC->SelectObject(&font);
		ShipSymbol.SetRect(KO.x, KO.y, KO.x+STARMAP_SECTOR_WIDTH-count*7, KO.y+STARMAP_SECTOR_HEIGHT);
		count++;
		pDC->DrawText(s, ShipSymbol, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
		pDC->SelectObject(oldfont);
	}
	if ((m_bIsStationBuild[DOMINION] == TRUE || m_bOutpost[DOMINION] == TRUE || m_bStarbase[DOMINION] == TRUE) &&
		(PlayersRace == DOMINION || m_iScanPower[PlayersRace] > 0))
	{
		CFont font;
		lf.lfHeight = 23;
		font.CreateFontIndirect(&lf);
		s.Format("D");
		pDC->SetTextColor(RGB(255,255,255));
		CFont* oldfont = pDC->SelectObject(&font);
		ShipSymbol.SetRect(KO.x, KO.y, KO.x+STARMAP_SECTOR_WIDTH-count*7, KO.y+STARMAP_SECTOR_HEIGHT);
		count++;
		pDC->DrawText(s, ShipSymbol, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
		pDC->SelectObject(oldfont);
	}
}