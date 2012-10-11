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
#include "Anomaly.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CSector, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CSector::CSector(void) :
	m_Attributes(0),
	m_bySunColor(0),
	m_iShipPathPoints(0),
	m_KO(-1, -1)
{
	m_pAnomaly = NULL;
}

CSector::CSector(const CSector& other) :
	m_KO(other.m_KO),
	m_strSectorName(other.m_strSectorName),
	m_Attributes(other.m_Attributes),
	m_sOwnerOfSector(other.m_sOwnerOfSector),
	m_sColonyOwner(other.m_sColonyOwner),
	m_byStatus(other.m_byStatus),
	m_bShipPort(other.m_bShipPort),
	m_bOutpost(other.m_bOutpost),
	m_bStarbase(other.m_bStarbase),
	m_bWhoIsOwnerOfShip(other.m_bWhoIsOwnerOfShip),
	m_mNumbersOfShips(other.m_mNumbersOfShips),
	m_bIsStationBuild(other.m_bIsStationBuild),
	m_iScanPower(other.m_iScanPower),
	m_iNeededScanPower(other.m_iNeededScanPower),
	m_iNeededStationPoints(other.m_iNeededStationPoints),
	m_iStartStationPoints(other.m_iStartStationPoints),
	m_bySunColor(other.m_bySunColor),
	m_byOwnerPoints(other.m_byOwnerPoints),
	m_Planets(other.m_Planets),
	m_iShipPathPoints(other.m_iShipPathPoints)
{
	m_pAnomaly=NULL;

};

CSector& CSector::operator=(const CSector& other){
	m_KO = other.m_KO;
	m_strSectorName = other.m_strSectorName;
	m_Attributes = other.m_Attributes;
	m_sOwnerOfSector = other.m_sOwnerOfSector;
	m_sColonyOwner = other.m_sColonyOwner;
	m_byStatus = other.m_byStatus;
	m_bShipPort = other.m_bShipPort;
	m_bOutpost = other.m_bOutpost;
	m_bStarbase = other.m_bStarbase;
	m_bWhoIsOwnerOfShip = other.m_bWhoIsOwnerOfShip;
	m_mNumbersOfShips = other.m_mNumbersOfShips;
	m_bIsStationBuild = other.m_bIsStationBuild;
	m_iScanPower = other.m_iScanPower;
	m_iNeededScanPower = other.m_iNeededScanPower;
	m_iNeededStationPoints = other.m_iNeededStationPoints;
	m_iStartStationPoints = other.m_iStartStationPoints;
	m_bySunColor = other.m_bySunColor;
	m_byOwnerPoints = other.m_byOwnerPoints;
	m_Planets = other.m_Planets;
	m_iShipPathPoints = other.m_iShipPathPoints;
	m_pAnomaly = other.m_pAnomaly;

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
		for (map<CString, BYTE>::const_iterator it = m_byStatus.begin(); it != m_byStatus.end(); ++it)
			ar << it->first << it->second;
		ar << m_bShipPort.size();
		for (set<CString>::const_iterator it = m_bShipPort.begin(); it != m_bShipPort.end(); ++it)
			ar << *it;
		ar << m_bOutpost.size();
		for (set<CString>::const_iterator it = m_bOutpost.begin(); it != m_bOutpost.end(); ++it)
			ar << *it;
		ar << m_bStarbase.size();
		for (set<CString>::const_iterator it = m_bStarbase.begin(); it != m_bStarbase.end(); ++it)
			ar << *it;
		ar << m_bIsStationBuild.size();
		for (set<CString>::const_iterator it = m_bIsStationBuild.begin(); it != m_bIsStationBuild.end(); ++it)
			ar << *it;
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
	for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
		currentHabitants += m_Planets.at(i).GetCurrentHabitant();
	return currentHabitants;
}

/// Diese Funktion berechnet die vorhandenen Rohstoffe der Planeten im Sektor. Übergebn wird dafür ein Feld für
/// die Ressourcen <code>res</code>.
void CSector::GetAvailableResources(BOOLEAN bResources[DERITIUM + 1], BOOLEAN bOnlyColonized/* = true */)
{
	for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
	{
		CPlanet* pPlanet = &m_Planets[i];
		if (!pPlanet->GetHabitable())
			continue;

		// wenn nur kolonisierte Planeten betrachtet werden sollen und der Planet nicht kolonisiert ist,
		// dann nächsten Planeten betrachten
		if (bOnlyColonized && !pPlanet->GetColonized())
			continue;

		BOOLEAN bExists[DERITIUM + 1] = {FALSE};
		pPlanet->GetAvailableResources(bExists);
		for (int res = TITAN; res <= DERITIUM; res++)
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
						planet.SetGraphicType(rand()%GRAPHICNUMBER);
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
						BOOLEAN bRes[DERITIUM + 1] = {FALSE};
						this->GetAvailableResources(bRes, true);
						// gibt es kein Deritium
						for (int i = TITAN; i <= IRIDIUM; i++)
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
						if (!bRes[DERITIUM])
						{
							for (int p = 0; p < static_cast<int>(this->GetPlanets().size()); p++)
								if (this->GetPlanet(p)->GetCurrentHabitant() > 0 && this->GetPlanet(p)->GetColonized())
								{
									this->GetPlanet(p)->SetBoni(DERITIUM, TRUE);
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
	m_mNumbersOfShips.clear();
	// Die benötigte Scanpower um Schiffe sehen zu können wieder auf NULL setzen
	m_iNeededScanPower.clear();
	m_iScanPower.clear();
	// Sagen das erstmal kein Außenposten und keine Sternbasis in dem Sektor steht
	m_bOutpost.clear();
	m_bStarbase.clear();
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

	for (set<CString>::const_iterator it = m_bOutpost.begin(); it != m_bOutpost.end(); ++it)
	{
		SetOwned(TRUE);
		m_sOwnerOfSector = *it;
		return;
	}

	for (set<CString>::const_iterator it = m_bStarbase.begin(); it != m_bStarbase.end(); ++it)
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
	m_byStatus.clear();
	m_iScanPower.clear();
	m_iNeededScanPower.clear();
	m_bShipPort.clear();
	m_bWhoIsOwnerOfShip.clear();
	m_mNumbersOfShips.clear();
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
void CSector::DrawSectorsName(CDC *pDC, CBotf2Doc* pDoc, CMajor* pPlayer)
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
		CMajor* pOwner = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(pDoc->GetSystem(m_KO).GetOwnerOfSystem()));
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
	for (map<CString, CRace*>::const_iterator it = pmRaces->begin(); it != pmRaces->end(); ++it)
	{
		if (ShouldDrawShip(*pPlayer, it->first))
		{
			// kann keine Diplomatie aufgenommen werden, dann das Alien Symbol zeichnen
			if (pPlayer != it->second && it->second->HasSpecialAbility(SPECIAL_NO_DIPLOMACY))
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
			g->DrawImage(ship, pt.x + 45 - nCount * 12, pt.y, 35, 35);
			delete ship;
			nCount++;
		}

		// Jetzt werden die Stationen wenn möglich gezeichnet
		if (ShouldDrawOutpost(*pPlayer, it->first))
		{
			// kann keine Diplomatie aufgenommen werden, dann das Alien Symbol zeichnen
			if (pPlayer != it->second && it->second->HasSpecialAbility(SPECIAL_NO_DIPLOMACY))
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
	const CCommandLineParameters* const clp = dynamic_cast<CBotf2App*>(AfxGetApp())->GetCommandLineParameters();
	if(clp->SeeAllOfMap())
		return 200;

	unsigned scan_power_due_to_ship_number = 0;
	if(bWith_ships) {
		const CBotf2Doc* pDoc = dynamic_cast<CBotf2App*>(AfxGetApp())->GetDocument();
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
