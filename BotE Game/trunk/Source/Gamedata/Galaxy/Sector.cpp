// Sector.cpp: Implementierung der Klasse CSector.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sector.h"
#include "IOData.h"
#include "GenSectorName.h"
#include "FontLoader.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"
#include "Anomaly.h"
#include "General/Loc.h"
#include "Ships/ships.h"
#include "ClientWorker.h"

#include <cassert>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CSector::CSector(void) :
	m_KO(-1, -1),
	m_Attributes(0),
	m_bySunColor(0)
{
	m_pAnomaly = NULL;
}

CSector::CSector(const CSector& other) :
	m_KO(other.m_KO),
	m_strSectorName(other.m_strSectorName),
	m_Attributes(other.m_Attributes),
	m_sOwnerOfSector(other.m_sOwnerOfSector),
	m_sColonyOwner(other.m_sColonyOwner),
	m_Status(other.m_Status),
	m_bShipPort(other.m_bShipPort),
	m_Outpost(other.m_Outpost),
	m_Starbase(other.m_Starbase),
	m_bWhoIsOwnerOfShip(other.m_bWhoIsOwnerOfShip),
	m_mNumbersOfShips(other.m_mNumbersOfShips),
	m_IsStationBuild(other.m_IsStationBuild),
	m_iScanPower(other.m_iScanPower),
	m_iNeededScanPower(other.m_iNeededScanPower),
	m_iNeededStationPoints(other.m_iNeededStationPoints),
	m_iStartStationPoints(other.m_iStartStationPoints),
	m_bySunColor(other.m_bySunColor),
	m_byOwnerPoints(other.m_byOwnerPoints),
	m_Planets(other.m_Planets)
{
	m_pAnomaly=NULL;
	if(other.GetAnomaly())
		m_pAnomaly = new CAnomaly(*other.m_pAnomaly);
};

CSector& CSector::operator=(const CSector& other){
	if(this != &other )
	{
		m_KO = other.m_KO;
		m_strSectorName = other.m_strSectorName;
		m_Attributes = other.m_Attributes;
		m_sOwnerOfSector = other.m_sOwnerOfSector;
		m_sColonyOwner = other.m_sColonyOwner;
		m_Status = other.m_Status;
		m_bShipPort = other.m_bShipPort;
		m_Outpost = other.m_Outpost;
		m_Starbase = other.m_Starbase;
		m_bWhoIsOwnerOfShip = other.m_bWhoIsOwnerOfShip;
		m_mNumbersOfShips = other.m_mNumbersOfShips;
		m_IsStationBuild = other.m_IsStationBuild;
		m_iScanPower = other.m_iScanPower;
		m_iNeededScanPower = other.m_iNeededScanPower;
		m_iNeededStationPoints = other.m_iNeededStationPoints;
		m_iStartStationPoints = other.m_iStartStationPoints;
		m_bySunColor = other.m_bySunColor;
		m_byOwnerPoints = other.m_byOwnerPoints;
		m_Planets = other.m_Planets;
		m_pAnomaly=NULL;
		if(other.GetAnomaly())
			m_pAnomaly = new CAnomaly(*other.m_pAnomaly);
	}

	return *this;
};

CSector::~CSector(void)
{
	if (m_pAnomaly!=NULL)
	{
		delete m_pAnomaly;
		m_pAnomaly = NULL;
	}
}

///////////////////////////////////////////////////////////////////////
// Speicher/Laden - Funktion
///////////////////////////////////////////////////////////////////////
void CSector::Serialize(CArchive &ar)
{
	// Wird geschrieben?
	if (ar.IsStoring())
	// Alle Variablen in der richtigen Reihenfolge schreiben
	{
		ar << m_Attributes;
		ar << m_KO;

		// alle Maps speichern
		ar << m_Status.size();
		for (map<CString, DISCOVER_STATUS>::const_iterator it = m_Status.begin(); it != m_Status.end(); ++it)
			ar << it->first << static_cast<BYTE>(it->second);
		ar << m_bShipPort.size();
		for (set<CString>::const_iterator it = m_bShipPort.begin(); it != m_bShipPort.end(); ++it)
			ar << *it;
		ar << m_Outpost;
		ar << m_Starbase;
		ar << m_IsStationBuild.size();
		for (std::map<CString, SHIP_ORDER::Typ>::const_iterator it = m_IsStationBuild.begin();
				it != m_IsStationBuild.end(); ++it)
			ar << it->first << static_cast<unsigned>(it->second);
		ar << m_bWhoIsOwnerOfShip.size();
		for (set<CString>::const_iterator it = m_bWhoIsOwnerOfShip.begin(); it != m_bWhoIsOwnerOfShip.end(); ++it)
			ar << *it;
		ar << m_mNumbersOfShips.size();
		for(std::map<CString, unsigned>::const_iterator it = m_mNumbersOfShips.begin(); it != m_mNumbersOfShips.end(); ++it)
			ar << it->first << it->second;
		ar << m_iNeededStationPoints.size();
		for (map<CString, short>::const_iterator it = m_iNeededStationPoints.begin(); it != m_iNeededStationPoints.end(); ++it)
			ar << it->first << it->second;
		ar << m_iStartStationPoints.size();
		for (map<CString, short>::const_iterator it = m_iStartStationPoints.begin(); it != m_iStartStationPoints.end(); ++it)
			ar << it->first << it->second;
		ar << m_iScanPower.size();
		for (map<CString, short>::const_iterator it = m_iScanPower.begin(); it != m_iScanPower.end(); ++it)
			ar << it->first << it->second;
		ar << m_iNeededScanPower.size();
		for (map<CString, short>::const_iterator it = m_iNeededScanPower.begin(); it != m_iNeededScanPower.end(); ++it)
			ar << it->first << it->second;
		ar << m_byOwnerPoints.size();
		for (map<CString, BYTE>::const_iterator it = m_byOwnerPoints.begin(); it != m_byOwnerPoints.end(); ++it)
			ar << it->first << it->second;

		ar << m_sColonyOwner;
		ar << m_sOwnerOfSector;

		// Nur wenn ein Sonnensystem in dem Sektor ist müssen die folgenden Variablen gespeichert werden
		if (GetSunSystem())
		{
			ar << m_strSectorName;
			ar << m_bySunColor;
			ar << m_Planets.size();
			for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
				m_Planets.at(i).Serialize(ar);
		}

		ar << m_pAnomaly;
	}
	else
	// Alle Variablen in der richtigen Reihenfolge lesen
	{
		ar >> m_Attributes;
		ar >> m_KO;

		// Maps laden
		m_Status.clear();
		size_t mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			BYTE value;
			ar >> key;
			ar >> value;
			m_Status[key] = static_cast<DISCOVER_STATUS>(value);
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
		m_Outpost.Empty();
		ar >> m_Outpost;
		m_Starbase.Empty();
		ar >> m_Starbase;
		m_IsStationBuild.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			ar >> key;
			unsigned value;
			ar >> value;
			m_IsStationBuild.insert(std::pair<CString, SHIP_ORDER::Typ>(key, static_cast<SHIP_ORDER::Typ>(value)));
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
		m_mNumbersOfShips.clear();
		mapSize = 0;
		ar >> mapSize;
		for(unsigned i = 0; i < mapSize; ++i)
		{
			CString key;
			ar >> key;
			unsigned value;
			ar >> value;
			m_mNumbersOfShips.insert(std::pair<CString, unsigned>(key, value));
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
			int number = 0;
			ar >> number;
			m_Planets.clear();
			m_Planets.resize(number);
			for (int i = 0; i < number; i++)
				m_Planets.at(i).Serialize(ar);
		}
		else
		{
			m_strSectorName = "";
			m_bySunColor = 0;
			m_Planets.clear();
		}

		if (VERSION >= 0.72)
		{
			if (m_pAnomaly)
			{
				delete m_pAnomaly;
				m_pAnomaly = NULL;
			}
			ar >> m_pAnomaly;
		}
	}
}

CSector::const_iterator CSector::begin() const
{
	return m_Planets.begin();
}
CSector::const_iterator CSector::end() const
{
	return m_Planets.end();
}

CSector::iterator CSector::begin()
{
	return m_Planets.begin();
}
CSector::iterator CSector::end()
{
	return m_Planets.end();
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
			s.Format("%s %c%i", CLoc::GetString("SECTOR"), (char)(m_KO.y+97), m_KO.x + 1);
			return s;
		}
	}
}

/// Funktion gibt alle Einwohner aller Planeten in dem Sektor zurück.
float CSector::GetCurrentHabitants() const
{
	float currentHabitants = 0.0f;
	for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
		currentHabitants += m_Planets.at(i).GetCurrentHabitant();
	return currentHabitants;
}

/// Diese Funktion berechnet die vorhandenen Rohstoffe der Planeten im Sektor. Übergebn wird dafür ein Feld für
/// die Ressourcen <code>res</code>.
void CSector::GetAvailableResources(BOOLEAN bResources[RESOURCES::DERITIUM + 1], BOOLEAN bOnlyColonized/* = true */) const
{
	for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
	{
		const CPlanet* pPlanet = &m_Planets[i];
		if (!pPlanet->GetHabitable())
			continue;

		// wenn nur kolonisierte Planeten betrachtet werden sollen und der Planet nicht kolonisiert ist,
		// dann nächsten Planeten betrachten
		if (bOnlyColonized && !pPlanet->GetColonized())
			continue;

		BOOLEAN bExists[RESOURCES::DERITIUM + 1] = {FALSE};
		pPlanet->GetAvailableResources(bExists);
		for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
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
		bool bMinor = rand()%100 >= (100 - minorProb);
		m_strSectorName = CGenSectorName::GetInstance()->GetNextRandomSectorName(m_KO, bMinor);
		// bMinor wird in der Generierungsfunktion angepasst, falls es keine Minorracesystemnamen mehr gibt
		SetMinorRace(bMinor);

		// Es konnte ein Sektor für eine Minorrace generiert werden
		if (bMinor)
		{
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
					for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
						maxHabitants += m_Planets.at(i).GetMaxHabitant();
					if (maxHabitants > (40.000f + random * 7))
						break;
				}
			} while (currentHabitants <= (15.000f / random));
		}
		else
		{
			// Wenn keine Minorrace im Sektor lebt
			this->CreatePlanets();
		}
	}
}

/// Diese Funktion generiert die Planeten in dem Sektor.
void CSector::CreatePlanets(const CString& sMajorID)
{
	m_Planets.clear();

	if (GetSunSystem())
	{
		// Es gibt 7 verschiedene Sonnenfarben
		m_bySunColor = rand()%7;

		if (sMajorID != "")
		{
			CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MajorPlanets.data";
			CStdioFile file;
			// Datei wird geöffnet
			if (file.Open(fileName, CFile::modeRead | CFile::typeText))
			{
				CString csInput;
				CString data[18];
				file.ReadString(csInput);
				bool endoffile=false;
				while(csInput!=sMajorID&&!endoffile) endoffile=!file.ReadString(csInput);//Zur Daten der gesuchten Majorrace springen
				if(!endoffile)//Wenn Major daten vorhanden
				{
					m_Planets.clear();
					file.ReadString(csInput);//Sonnenfarbe
					m_bySunColor = atoi(csInput);
					file.ReadString(csInput);//Planetenanzahl
					int planeten=atoi(csInput);
					CPlanet planet;
					for(int i=0;i<planeten;i++)
					{
						for(int j=0;j<18;j++)
						{
							file.ReadString(csInput);
							data[j]=csInput;
						}
						planet.SetName(data[0]);//Planeten Name
						planet.SetType(atoi(data[1]));  //Planetentyp
						planet.SetColonisized(atoi(data[2])); //Ist der Planet koloniesiert
						float maxHabitant=atoi(data[3])/1000;//für Einwohnerzahlen mit Nachkommastellen
						planet.SetMaxHabitant(maxHabitant); //Maximale Einwohner
						float curHabitant=atoi(data[4])/1000;
						planet.SetCurrentHabitant(curHabitant); //aktuelle Einwohner
						planet.SetHabitable(atoi(data[5]));//bewohnbar
						planet.SetTerraformed(atoi(data[6]));//terraformed
						planet.SetSize((PLANT_SIZE::Typ)atoi(data[7]));//Größe
						planet.SetClass(data[8][0]);//Planetenklasse Char
						planet.SetPlanetGrowth();
						planet.SetGraphicType(rand()%PLANET_CLASSES::GRAPHICNUMBER);
						planet.SetBoni(atoi(data[9]),atoi(data[10]),atoi(data[11]),atoi(data[12]),atoi(data[13]),atoi(data[14]),atoi(data[15]),atoi(data[16]));//Boni 8 Zeilen
						planet.SetStartTerraformPoints(atoi(data[17]));//Terraformpoints
						planet.SetHasIndividualGraphic(true);
						m_Planets.push_back(planet);
					}
				}
				else
				{
					while (true)//Falls Major in der Datei nicht definiert ist wird der bisherige Generierungsalgorithmus benutzt
					{
						m_Planets.clear();

						short number = (rand()%8+1 + rand()%8+1 + rand()%8+1 + 1) / 3;
						PLANET_ZONE::Typ zone = PLANET_ZONE::HOT;
						// mit welcher Plantenzone soll zufällig begonnen werden
						int nRandom = rand()%10;
						if (nRandom == 0)
							zone = PLANET_ZONE::COOL;
						else if (nRandom < 3)
							zone = PLANET_ZONE::TEMPERATE;

						for (int i = 0; i < number; i++)
						{
							CPlanet planet;
							zone = planet.Create(m_strSectorName, zone, i, true);
							m_Planets.push_back(planet);
						}

						// aktuelle Bevölkerung prüfen
						float fCurrentHabitants = this->GetCurrentHabitants();
						if (fCurrentHabitants > 25.000f || fCurrentHabitants < 10.000f)
							continue;

						// maximale Bevölkerung prüfen
						float fMaxHabitants = 0.0f;
						for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
							fMaxHabitants += m_Planets.at(i).GetMaxHabitant();
						if (fMaxHabitants > 65.000f || fMaxHabitants < 45.000f)
							continue;

						// prüfen ob alle Rohstoffe vorhanden sind
						bool bResOkay = true;
						BOOLEAN bRes[RESOURCES::DERITIUM + 1] = {FALSE};
						this->GetAvailableResources(bRes, true);
						// gibt es kein Deritium
						for (int i = RESOURCES::TITAN; i <= RESOURCES::IRIDIUM; i++)
						{
							if (!bRes[i])
							{
								bResOkay = false;
								break;
							}
						}
						if (!bResOkay)
							continue;

						// Deritium überprüfen
						if (!bRes[RESOURCES::DERITIUM])
						{
							for(CSector::iterator it = begin(); it != end(); ++it)
								if (it->GetCurrentHabitant() > 0 && it->GetColonized())
								{
									it->SetBoni(RESOURCES::DERITIUM, TRUE);
									break;
								}
						}

						// Sektor für Majorrace konnte erstellt werden
						break;
					}

				}
			}
			else
			{
				AfxMessageBox("ERROR! Could not open file \"MajorPlanets.data\"...");
				exit(1);
			}
			file.Close();
		}
		else
		{
			int nSizeCounter = 0;
			// dreimal die Zufallsfunktion aufgerufen, damit die mittlere Planetenanzahl häufiger als ganz wenig oder
			// sehr viele Planeten vorkommt.
			short number = (rand()%8+1 + rand()%8+1 + rand()%8+1 + 1) / 3;
			PLANET_ZONE::Typ zone = PLANET_ZONE::HOT;
			// mit welcher Plantenzone soll zufällig begonnen werden
			int nRandom = rand()%10;
			if (nRandom == 0)
				zone = PLANET_ZONE::COOL;
			else if (nRandom < 3)
				zone = PLANET_ZONE::TEMPERATE;

			for (int i = 0; i < number; i++)
			{
				CPlanet planet;
				zone = planet.Create(m_strSectorName, zone, i, GetMinorRace());
				m_Planets.push_back(planet);

				// nicht zu viele große Planeten generieren, da diese dann nicht mehr
				// in die View passen

				// kleine und mittlere Planeten zählen die Systemgrößte um 1 hoch
				if (planet.GetSize() <= 1)
					nSizeCounter += 1;
				// alle anderen um 2
				else
					nSizeCounter += 2;

				if (nSizeCounter > 10)
					break;
			}
		}
	}
}

/// Funktion erzeugt eine zufällige Anomalie im Sektor.
void CSector::CreateAnomaly(void)
{
	if (m_pAnomaly)
	{
		delete m_pAnomaly;
		m_pAnomaly = NULL;
	}

	m_pAnomaly = new CAnomaly();
}

/// Diese Funktion führt das Planetenwachstum für diesen Sektor durch.
void CSector::LetPlanetsGrowth()
{
	for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
		m_Planets.at(i).PlanetGrowth();
}

/// Diese Funktion lässt die Bevölkerung auf allen Planeten zusammen um den übergebenen Wert <code>Value</code>
/// schrumpfen.
void CSector::LetPlanetsShrink(float Value)
{
	// aktuelle Einwohner auf den einzelnen Planeten
	float* Habitants = new float[m_Planets.size()];

	// alle Einwohner im Sector
	float allHabitants = 0.0f;
	for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
	{
		allHabitants += m_Planets[i].GetCurrentHabitant();
		Habitants[i] = m_Planets[i].GetCurrentHabitant();
	}
	for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
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
	delete[] Habitants;
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
	for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
		m_Planets[i].SetIsTerraforming(FALSE);

	m_byOwnerPoints.clear();

	// nun können alle StartStationPoint die auf 0 stehen in der Map gelöscht werden
	for (map<CString, short>::iterator it = m_iStartStationPoints.begin(); it != m_iStartStationPoints.end(); )
	{
		if (m_IsStationBuild.find(it->first) == m_IsStationBuild.end())
			it->second = 0;

		if (it->second == 0)
			it = m_iStartStationPoints.erase(it++);
		else
			++it;
	}
	m_IsStationBuild.clear();

	m_bWhoIsOwnerOfShip.clear();
	m_mNumbersOfShips.clear();
	// Die benötigte Scanpower um Schiffe sehen zu können wieder auf NULL setzen
	m_iNeededScanPower.clear();
	m_iScanPower.clear();
	// Sagen das erstmal kein Außenposten und keine Sternbasis in dem Sektor steht
	m_Outpost.Empty();
	m_Starbase.Empty();
	m_bShipPort.clear();
}

#pragma warning (push)
#pragma warning (disable : 4702)
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

	if(!m_Outpost.IsEmpty())
	{
		SetOwned(TRUE);
		m_sOwnerOfSector = m_Outpost;
		return;
	}
	if(!m_Starbase.IsEmpty())
	{
		SetOwned(TRUE);
		m_sOwnerOfSector = m_Starbase;
		return;
	}

	// Ist obiges nicht eingetreten, so gehört demjenigen der Sektor, wer die meisten Besitzerpunkte hat. Ist hier
	// Gleichstand haben wir neutrales Gebiet. Es werden mindst. 2 Punkte benötigt, um als neuer Besitzer des Sektors
	// zu gelten.
	BYTE mostPoints = 1;
	CString newOwner = "";
	for (map<CString, BYTE>::const_iterator it = m_byOwnerPoints.begin(); it != m_byOwnerPoints.end(); ++it)
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
#pragma warning (pop)

/// Resetfunktion für die Klasse CSector
void CSector::Reset()
{
	m_Attributes = 0;

	// Maps löschen
	m_Status.clear();
	m_iScanPower.clear();
	m_iNeededScanPower.clear();
	m_bShipPort.clear();
	m_bWhoIsOwnerOfShip.clear();
	m_mNumbersOfShips.clear();
	m_Outpost.Empty();
	m_Starbase.Empty();
	m_IsStationBuild.clear();
	m_iStartStationPoints.clear();
	m_iNeededStationPoints.clear();
	m_byOwnerPoints.clear();

	m_sOwnerOfSector = "";
	m_sColonyOwner = "";
	m_strSectorName = "";
	m_Planets.clear();

	if (m_pAnomaly)
	{
		delete m_pAnomaly;
		m_pAnomaly = NULL;
	}
}

////////////////////////////////////////////////////////////////
// Zeichenfunktionen
////////////////////////////////////////////////////////////////
/// Diese Funktion zeichnet den Namen des Sektors.
void CSector::DrawSectorsName(CDC *pDC, CBotEDoc* pDoc, CMajor* pPlayer)
{
	// befindet sich kein Sonnensystem oder keine Anomalie im Sektor
	if (!GetSunSystem() && !GetAnomaly())
		return;

	ASSERT(pDC);
	ASSERT(pDoc);
	ASSERT(pPlayer);

	// Ist Sektor bekannt dann zeichne den Systemnamen
	if (this->GetKnown(pPlayer->GetRaceID()) == TRUE)
	{
		COLORREF clrTextColor = CFontLoader::GetFontColor(pPlayer, 0);
		pDC->SetTextColor(clrTextColor);
		CMajor* pOwner = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(pDoc->GetSystem(m_KO.x, m_KO.y).GetOwnerOfSystem()));
		if (pOwner)
		{
			if (pPlayer->IsRaceContacted(pOwner->GetRaceID()) == true || pPlayer->GetRaceID() == pOwner->GetRaceID())
				pDC->SetTextColor(pOwner->GetDesign()->m_clrGalaxySectorText);
		}
		// Systemnamen zeichnen
		if (m_pAnomaly == NULL)
			pDC->DrawText(m_strSectorName, CRect(m_KO.x*STARMAP_SECTOR_WIDTH, m_KO.y*STARMAP_SECTOR_HEIGHT, m_KO.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH,m_KO.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT), DT_CENTER | DT_BOTTOM | DT_SINGLELINE);
		else
		{
			//CRect(m_KO.x*STARMAP_SECTOR_WIDTH-50,m_KO.y*STARMAP_SECTOR_HEIGHT,m_KO.x*STARMAP_SECTOR_WIDTH+90,m_KO.y*STARMAP_SECTOR_HEIGHT+40)
			pDC->DrawText(m_pAnomaly->GetMapName(m_KO), CRect(m_KO.x*STARMAP_SECTOR_WIDTH, m_KO.y*STARMAP_SECTOR_HEIGHT,m_KO.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH,m_KO.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT), DT_CENTER | DT_BOTTOM | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		}
	}
}

bool CSector::ShouldDrawShip(const CMajor& our_race, const CString& their_race_id) const {
	if(!GetOwnerOfShip(their_race_id))
		return false;
	const CString& our_id = our_race.GetRaceID();
	if(our_id == their_race_id)
		return true;
	if(GetNeededScanPower(their_race_id) < GetScanPower(our_id))
		return true;
	return our_race.GetAgreement(their_race_id) >= DIPLOMATIC_AGREEMENT::AFFILIATION;
}

bool CSector::ShouldDrawOutpost(const CMajor& our_race, const CString& their_race_id) const {
	if(!GetIsStationBuilding(their_race_id)
		&& !GetOutpost(their_race_id)
		&& !GetStarbase(their_race_id))
		return false;
	const CString& our_id = our_race.GetRaceID();
	if(our_id == their_race_id)
		return true;
	if(GetScanPower(our_id) > 0)
		return true;
	return our_race.GetAgreement(their_race_id) >= DIPLOMATIC_AGREEMENT::AFFILIATION;
}

/// Diese Funktion zeichnet die entsprechenden Schiffssymbole in den Sektor
void CSector::DrawShipSymbolInSector(Graphics *g, CBotEDoc* pDoc, CMajor* pPlayer) const
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
	for (map<CString, CRace*>::const_iterator it = pmRaces->begin(); it != pmRaces->end(); ++it)
	{
		if (ShouldDrawShip(*pPlayer, it->first))
		{
			// handelt es sich um ein Alienschiff?
			if (pPlayer != it->second && it->second->IsAlienRace())
				sFilePath = sAppPath + "Graphics\\Symbols\\Entity.bop";
			// ist der Besitzer des Schiffes unbekannt, dann Fragezeichen zeichnen
			else if (pPlayer != it->second && pPlayer->IsRaceContacted(it->first) == false)
				sFilePath = sAppPath + "Graphics\\Symbols\\Unknown.bop";
			// sonst das Rassensymbol zeichnen
			else
				sFilePath = sAppPath + "Graphics\\Symbols\\" + it->first + ".bop";

			Bitmap* ship = Bitmap::FromFile(CComBSTR(sFilePath));
			// konnte die Grafik nicht geladen werden, dann wird ein Standardsymbol geladen
			if (!ship || ship->GetLastStatus() != Ok)
			{
				sFilePath = sAppPath + "Graphics\\Symbols\\Default.bop";
				ship = Bitmap::FromFile(CComBSTR(sFilePath));
			}
			g->DrawImage(ship, pt.x + 45 - nCount * 12, pt.y, 35, 35);
			delete ship;
			nCount++;
		}

		// Jetzt werden die Stationen wenn möglich gezeichnet
		if (ShouldDrawOutpost(*pPlayer, it->first))
		{
			// kann keine Diplomatie aufgenommen werden, dann das Alien Symbol zeichnen
			if (pPlayer != it->second && it->second->IsAlienRace())
				sFilePath = sAppPath + "Graphics\\Symbols\\Entity.bop";
			// ist der besitzer des Schiffes unbekannt, dann Fragezeichen zeichnen
			else if (pPlayer != it->second && pPlayer->IsRaceContacted(it->first) == false)
				sFilePath = sAppPath + "Graphics\\Symbols\\Unknown.bop";
			// sonst das Rassensymbol zeichnen
			else
				sFilePath = sAppPath + "Graphics\\Symbols\\" + it->first + ".bop";

			Bitmap* ship = Bitmap::FromFile(CComBSTR(sFilePath));
			// konnte die Grafik nicht geladen werden, dann wird ein Standardsymbol geladen
			if (!ship || ship->GetLastStatus() != Ok)
			{
				sFilePath = sAppPath + "Graphics\\Symbols\\Default.bop";
				ship = Bitmap::FromFile(CComBSTR(sFilePath));
			}
			g->DrawImage(ship, pt.x, pt.y + 45, 35, 35);
			delete ship;
		}
	}
}

void CSector::IncrementNumberOfShips(const CString& race) {
	const std::map<CString, unsigned>::iterator found = m_mNumbersOfShips.find(race);
	if(found == m_mNumbersOfShips.end()) {
		m_mNumbersOfShips.insert(std::pair<CString, unsigned>(race, 1));
		return;
	}
	++(found->second);
}

/// Diese Funktion gibt die Scanpower zurück, die die Majorrace <code>Race</code> in diesem Sektor hat.
short CSector::GetScanPower(const CString& sRace, bool bWith_ships) const
{
	const CCommandLineParameters* const clp = resources::pClp;
	if(clp->SeeAllOfMap())
		return 200;

	unsigned scan_power_due_to_ship_number = 0;
	if(bWith_ships) {
		const CBotEDoc* pDoc = resources::pDoc;
		const CRaceController* pCtrl = pDoc->GetRaceCtrl();
		const CRace* pRace = pCtrl->GetRace(sRace);
		for(std::map<CString, unsigned>::const_iterator it = m_mNumbersOfShips.begin(); it != m_mNumbersOfShips.end(); ++ it) {
			if(pRace->GetRaceID() == it->first || pRace->GetAgreement(it->first) >= DIPLOMATIC_AGREEMENT::AFFILIATION)
				scan_power_due_to_ship_number += it->second;
		}
	}
	map<CString, short>::const_iterator it = m_iScanPower.find(sRace);
	if (it != m_iScanPower.end())
		return it->second + scan_power_due_to_ship_number;
	return scan_power_due_to_ship_number;
}

/// Funktion legt die Scanpower <code>scanpower</code>, welche die Majorrace <code>Race</code>
/// in diesem Sektor hat, fest.
void CSector::SetScanPower(short scanpower, const CString& Race)
{
	if (scanpower)
		m_iScanPower[Race] = scanpower;
	else
		m_iScanPower.erase(Race);
}

void CSector::PutScannedSquare(unsigned range, const int power,
		const CRace& race, bool bBetterScanner, bool patrolship, bool anomaly) {
	const CString& race_id = race.GetRaceID();
	float boni = 1.0f;
	// Wenn das Schiff die Patrouillieneigenschaft besitzt und sich in einem eigenen Sektor befindet,
	// dann wird die Scanleistung um 20% erhöht.
	if(patrolship) {
		if(race_id == m_sOwnerOfSector || race.GetAgreement(m_sOwnerOfSector) >= DIPLOMATIC_AGREEMENT::AFFILIATION)
			boni = 1.2f;
	}
	if(bBetterScanner) {
		range *= 1.5;
		boni += 0.5;
	}
	const int intrange = static_cast<int>(range);
	for (int i = -intrange; i <= intrange; ++i) {
		const int x = m_KO.x + i;
		if(0 <= x && x < STARMAP_SECTORS_HCOUNT) {
			for (int j = -intrange; j <= intrange; ++j) {
				const int y = m_KO.y + j;
				if(0 <= y && y < STARMAP_SECTORS_VCOUNT) {
					CBotEDoc* pDoc = resources::pDoc;
					CSector& scanned_sector = pDoc->GetSector(x, y);
					// Teiler für die Scanstärke berechnen
					int div = max(abs(i), abs(j));
					if(anomaly)
						div *= 2;
					div = max(div, 1);
					const int old_scan_power = scanned_sector.GetScanPower(race_id, false);
					int new_scan_power = 0;
					if(anomaly) {
						new_scan_power = old_scan_power + power / div;
					} else {
						new_scan_power = (power * boni) / div;
						new_scan_power = max(old_scan_power, new_scan_power);
						if(race.IsMajor())
							scanned_sector.SetScanned(race_id);
					}
					scanned_sector.SetScanPower(new_scan_power, race_id);
				}//if(0 <= y && y < STARMAP_SECTORS_VCOUNT)
			}//for (int j = -range; j <= range; ++j)
		}//if(0 <= x && x < STARMAP_SECTORS_HCOUNT)
	}//for (int i = -range; i <= range; ++i)
}

static bool StationBuildContinuable(const CString& race, const CSector& sector) {
	const CString& owner = sector.GetOwnerOfSector();
	return owner.IsEmpty() || owner == race || sector.GetIsStationBuilding(race);
}

bool CSector::IsStationBuildable(SHIP_ORDER::Typ order, const CString& race) const {
	if(order == SHIP_ORDER::BUILD_OUTPOST && !GetIsStationInSector())
		return StationBuildContinuable(race, *this);
	if(order == SHIP_ORDER::BUILD_STARBASE && GetOutpost(race))
		return StationBuildContinuable(race, *this);
	if(order == SHIP_ORDER::UPGRADE_OUTPOST && GetOutpost(race) 
		|| order == SHIP_ORDER::UPGRADE_STARBASE && GetStarbase(race)) {
		const CBotEDoc* pDoc = resources::pDoc;
		CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(race));
		SHIP_TYPE::Typ type = (order == SHIP_ORDER::UPGRADE_OUTPOST) 
			? SHIP_TYPE::OUTPOST : SHIP_TYPE::STARBASE;
		USHORT bestbuildableID = pMajor->BestBuildableVariant(type, pDoc->m_ShipInfoArray);
		USHORT industry = pDoc->m_ShipInfoArray.GetAt(bestbuildableID-10000).GetBaseIndustry();
		for(CShipMap::const_iterator k = pDoc->m_ShipMap.begin(); k != pDoc->m_ShipMap.end(); ++k)
			if (k->second->GetShipType() == type && k->second->GetKO() == m_KO) {
				if (pDoc->m_ShipInfoArray.GetAt(k->second->GetID()-10000).GetBaseIndustry() 
					< industry) {
					return StationBuildContinuable(race, *this);
				}
				break;
			}
	}
	return false;
}

void CSector::RecalcPlanetsTerraformingStatus() {
	const CShipMap& sm = resources::pDoc->m_ShipMap;
	std::set<unsigned> terraformable;
	//unset terraforming status for all planets
	for(std::vector<CPlanet>::iterator i = m_Planets.begin(); i != m_Planets.end(); ++i) {
		i->SetIsTerraforming(FALSE);
		if(i->GetHabitable() && !i->GetTerraformed())
			terraformable.insert(i - m_Planets.begin());
	}
	//reset it for those which are actually terraformed at present
	for(CShipMap::const_iterator i = sm.begin(); i != sm.end(); ++i) {
		if(terraformable.empty())
			break;
		if(i->second->GetKO() != m_KO || i->second->GetCurrentOrder() != SHIP_ORDER::TERRAFORM)
			continue;
		const unsigned planet = i->second->GetTerraform();
		CPlanet& p = m_Planets.at(planet);
		assert(p.GetHabitable());
		//It is allowed to terraform the same planet with 2+ independent ships
		if(p.GetIsTerraforming() || p.GetTerraformed())
			continue;
		p.SetIsTerraforming(TRUE);
#pragma warning(push)
#pragma warning(disable: 4189)
		unsigned erased = terraformable.erase(planet);
		assert(erased == 1);
	}
}
#pragma warning(pop)

void CSector::BuildStation(SHIP_TYPE::Typ station, const CString& race) {
	if(station == SHIP_TYPE::OUTPOST)
		m_Outpost = race;
	else {
		assert(station == SHIP_TYPE::STARBASE);
		assert(m_Outpost == race);
		m_Outpost.Empty();
		m_Starbase = race;
	}
	// Nur wenn der Sektor uns selbst gehört oder niemanden gehört und keine Minorrace darin lebt
	if(m_sOwnerOfSector == race || (!GetOwned() && ! GetMinorRace())) {
		SetOwned(TRUE);
		m_sOwnerOfSector = race;
	}
	SetScanned(race);
	SetShipPort(TRUE, race);

	// Wenn eine Station fertig wurde für alle Rassen die Punkte wieder canceln
	m_IsStationBuild.clear();
	m_iStartStationPoints.clear();
	m_iNeededStationPoints.clear();
}

int CSector::CountOfTerraformedPlanets() const
{
	int count = 0;
	for(std::vector<CPlanet>::const_iterator it = m_Planets.begin(); it != m_Planets.end(); ++it)
		if(it->IsColonizable())
			++count;
	return count;
}

void CSector::DistributeColonists(const float colonists)
{
	//Die Bevölkerung, welche bei der Kolonisierung
	// auf das System kommt, wird auf die einzelnen Planeten gleichmäßig aufgeteilt.
	float oddHab = 0.0f;	// Überschüssige Kolonisten, wenn ein Planet zu klein ist
	// Geterraformte Planeten durchgehen und die Bevölkerung auf diese verschieben
	for(std::vector<CPlanet>::iterator it = m_Planets.begin(); it != m_Planets.end(); ++it)
	{
		if(!it->IsColonizable())
			continue;
		const float max_hab = it->GetMaxHabitant();
		if (colonists > max_hab)
		{
			oddHab += colonists - max_hab;
			it->SetCurrentHabitant(max_hab);
		}
		else
			it->SetCurrentHabitant(colonists);
		it->SetColonisized(TRUE);
	}

	if (oddHab <= 0.0f)
		return;
	// die übrigen Kolonisten auf die Planeten verteilen
	for(std::vector<CPlanet>::iterator it = m_Planets.begin(); it != m_Planets.end(); ++it)
	{
		const float current_hab = it->GetCurrentHabitant();
		const float max_hab = it->GetMaxHabitant();
		if (!it->GetTerraformed() || current_hab <= 0.0f || current_hab >= max_hab)
			continue;
		const float try_new_hab = oddHab + current_hab;
		oddHab -= max_hab - current_hab;
		if (try_new_hab > max_hab)
			it->SetCurrentHabitant(max_hab);
		else {
			it->SetCurrentHabitant(try_new_hab);
			assert(oddHab <= 0);
			break;
		}
	}
}

void CSector::Colonize(CSystem& sy, const CShips& ship, CMajor& major)
{
	const CString shipowner = ship.GetOwnerOfShip();
	CEmpire* empire = major.GetEmpire();
	// Gebäude bauen, wenn wir das System zum ersten Mal kolonisieren,
	// sprich das System noch niemanden gehört
	if (sy.GetOwnerOfSystem().IsEmpty())
	{
		// Sector- und Systemwerte ändern
		SetOwned(TRUE);
		m_sOwnerOfSector = shipowner;
		m_sColonyOwner = shipowner;
		sy.SetOwnerOfSystem(shipowner);
		// Gebäude nach einer Kolonisierung bauen
		sy.BuildBuildingsAfterColonization(this,resources::BuildingInfo,ship.GetColonizePoints());
		// Nachricht an das Imperium senden, das ein System neu kolonisiert wurde
		CString s = CLoc::GetString("FOUND_COLONY_MESSAGE",FALSE,GetName());
		CEmpireNews message;
		message.CreateNews(s,EMPIRE_NEWS_TYPE::SOMETHING,GetName(),m_KO);
		empire->AddMsg(message);

		// zusätzliche Eventnachricht (Colonize a system #12) wegen der Moral an das Imperium
		message.CreateNews(major.GetMoralObserver()->AddEvent(12, major.GetRaceMoralNumber(), GetName()), EMPIRE_NEWS_TYPE::SOMETHING, "", m_KO);
		empire->AddMsg(message);
		if (major.IsHumanPlayer())
		{
			resources::pClientWorker->AddSoundMessage(SNDMGR_MSG_CLAIMSYSTEM, major , 0);
			resources::pClientWorker->SetToEmpireViewFor(major);
			CEventColonization* eventScreen = new CEventColonization(major.GetRaceID(), CLoc::GetString("COLOEVENT_HEADLINE", FALSE, GetName()), CLoc::GetString("COLOEVENT_TEXT_" + major.GetRaceID(), FALSE, GetName()));
			empire->GetEvents()->Add(eventScreen);
			s.Format("Added Colonization-Eventscreen for Race %s in System %s", major.GetRaceName(), GetName());
			MYTRACE("general")(MT::LEVEL_INFO, s);
		}
	}
	else
	{
		// Nachricht an das Imperium senden, das ein Planet kolonisiert wurde
		CString s = CLoc::GetString("NEW_PLANET_COLONIZED",FALSE,GetName());
		CEmpireNews message;
		message.CreateNews(s,EMPIRE_NEWS_TYPE::SOMETHING,GetName(),m_KO);
		empire->AddMsg(message);
		resources::pClientWorker->SetToEmpireViewFor(major);
	}
	sy.SetHabitants(GetCurrentHabitants());

	sy.CalculateNumberOfWorkbuildings(resources::BuildingInfo);
	sy.CalculateVariables(resources::BuildingInfo, empire->GetResearch()->GetResearchInfo(), m_Planets, &major, CTrade::GetMonopolOwner());
}

void CSector::SystemEventPlanetMovement(CString& message)
{
	const int nSize = m_Planets.size();
	assert(nSize >= 1);
	int nPlanet=rand()%nSize;
	while(!(m_Planets.at(nPlanet).GetHabitable()))
		nPlanet=rand()%nSize;
	CPlanet& planet = m_Planets.at(nPlanet);
	const float old_habitants = planet.GetMaxHabitant();
	const float habitants_change = old_habitants * ((rand() % 11) / 10.0f) * ((rand() % 2 == 1) ? 1 : -1);
	const float new_habitants = min(max(old_habitants + habitants_change, 1),100);
	if(old_habitants != new_habitants)
	{
		planet.SetMaxHabitant(new_habitants);
		CString habitants_change_string;
		habitants_change_string.Format("%.3f", habitants_change);
		message=CLoc::GetString("SYSTEMEVENTPLANETMOVEMENT",false,planet.GetPlanetName(), habitants_change_string);
	}
}

void CSector::SystemEventDemographic(CString& message, CMajor& major)
{
	// Es sollte hier immer mindestens 1 habitabler bewohnter Planet im System sein...
	for(int i = 0; i < 100; ++i) {
		CPlanet& planet = m_Planets.at(rand()%m_Planets.size());
		if(planet.GetHabitable() && planet.GetCurrentHabitant() > 1) {
			const float old_habitants = planet.GetCurrentHabitant();
			const float habitants_change = old_habitants * ((rand() % 11) / 10.0f) * -1;
			const float new_habitants = max(old_habitants + habitants_change, 1);
			if(old_habitants != new_habitants)
			{
				planet.SetCurrentHabitant(new_habitants);
				CString habitants_change_string;
				habitants_change_string.Format("%.3f", habitants_change * -1);
				message=CLoc::GetString("SYSTEMEVENTPLANETDEMOGRAPHICLONG",false,planet.GetPlanetName(), habitants_change_string);

				if (major.IsHumanPlayer())
				{
					resources::pClientWorker->SetToEmpireViewFor(major);

					CEventRandom* EmpireEvent=new CEventRandom(major.GetRaceID(),"demographic",CLoc::GetString("SYSTEMEVENTPLANETDEMOGRAPHICTITLE"),message);
					major.GetEmpire()->GetEvents()->Add(EmpireEvent);
				}
			}
			break;
		}
	}
}

void CSector::Terraforming(CShip& ship)
{
	const short nPlanet = ship.GetTerraform();
	assert(-1 <= nPlanet && nPlanet < static_cast<int>(m_Planets.size()));
	if (nPlanet != -1)
		m_Planets.at(nPlanet).SetIsTerraforming(TRUE);
	else
		ship.SetTerraform(-1);
}

bool CSector::PerhapsMinorExtends(BYTE TechnologicalProgress)
{
	bool bColonized = false;
	for(CSector::iterator it = begin(); it != end(); ++it)
	{
		// ist der Planet noch nicht geterraformt
		if (it->GetColonized() == FALSE && it->GetHabitable() == TRUE && it->GetTerraformed() == FALSE)
		{
			// mit einer gewissen Wahrscheinlichkeit wird der Planet geterraformt und kolonisiert
			if (rand()%200 >= (200 - (TechnologicalProgress+1)))
			{
				bColonized = true;
				it->SetNeededTerraformPoints(it->GetNeededTerraformPoints());
				it->SetTerraformed(TRUE);
				it->SetColonisized(TRUE);
				it->SetIsTerraforming(FALSE);
				if (it->GetMaxHabitant() < 1.0f)
					it->SetCurrentHabitant(it->GetMaxHabitant());
				else
					it->SetCurrentHabitant(1.0f);
			}
		}
		// ist der Planet schon geterraformt
		else if (it->GetColonized() == FALSE && it->GetTerraformed() == TRUE)
		{
			// dann wird mit einer größeren Wahrscheinlichkeit kolonisiert
			if (rand()%200 >= (200 - 3*(TechnologicalProgress+1)))
			{
				bColonized = true;
				it->SetColonisized(TRUE);
				if (it->GetMaxHabitant() < 1.0f)
					it->SetCurrentHabitant(it->GetMaxHabitant());
				else
					it->SetCurrentHabitant(1.0f);
			}
		}
	}
	return bColonized;
}

void CSector::CreateDeritiumForSpaceflightMinor()
{
	BOOLEAN bRes[RESOURCES::DERITIUM + 1] = {FALSE};
	GetAvailableResources(bRes, true);
	// gibt es kein Deritium=
	if (!bRes[RESOURCES::DERITIUM])
	{
		for(CSector::iterator it = begin(); it != end(); ++it)
		{
			if (it->GetCurrentHabitant() > 0 && it->GetColonized())
			{
				it->SetBoni(RESOURCES::DERITIUM, TRUE);
				break;
			}
		}
	}
}

bool CSector::Terraform(const CShips& ship)
{
	return m_Planets.at(ship.GetTerraform()).SetNeededTerraformPoints(ship.GetColonizePoints()) ? true : false;
}
