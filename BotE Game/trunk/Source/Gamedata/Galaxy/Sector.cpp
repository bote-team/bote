/*
 *@file
 * properties of a map location with a sun system (but not neccessarily any buildings)
 * The central elements of this class are the planets and the functions doing something with them.
 */

#include "stdafx.h"
#include "Sector.h"
#include "IOData.h"
#include "GenSectorName.h"
#include "BotEDoc.h"
#include "General/Loc.h"
#include "Ships/ships.h"
#include "ClientWorker.h"
#include "Races/Major.h"
#include "Races/RaceController.h"
#include "float.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// construction/destruction
//////////////////////////////////////////////////////////////////////
CSector::CSector(void) :
	CMapTile(),
	m_Planets(),
	m_sColonyOwner(),
	m_bySunColor(0)
{
	Reset(false);
}

CSector::CSector(int x, int y) :
	CMapTile(x, y),
	m_Planets(),
	m_sColonyOwner(),
	m_bySunColor(0)
{
	Reset(false);
}

CSector::CSector(const CSector& other) :
	CMapTile(other),
	m_Planets(other.m_Planets),
	m_sColonyOwner(other.m_sColonyOwner),
	m_bySunColor(other.m_bySunColor),
	m_HomeOf(other.m_HomeOf)
{
};

CSector& CSector::operator=(const CSector& other){
	if(this != &other )
	{
		CMapTile::operator =(other);
		m_Planets = other.m_Planets;
		m_sColonyOwner = other.m_sColonyOwner;
		m_bySunColor = other.m_bySunColor;
		m_HomeOf = other.m_HomeOf;
	}

	return *this;
};

CSector::~CSector(void)
{
	Reset(false);
}

/// Resetfunktion für die Klasse CSector
void CSector::Reset(bool call_up)
{
	if(call_up)
		CMapTile::Reset(call_up);

	m_sColonyOwner.Empty();
	m_HomeOf.reset();
	m_Planets.clear();

	ClearAllPoints(false);
}

//////////////////////////////////////////////////////////////////////
// serialization
//////////////////////////////////////////////////////////////////////
void CSector::Serialize(CArchive &ar)
{
	CMapTile::Serialize(ar);
	// Wird geschrieben?
	if (ar.IsStoring())
	// Alle Variablen in der richtigen Reihenfolge schreiben
	{
		ar << m_sColonyOwner;
		ar << HomeOfID();
		// Nur wenn ein Sonnensystem in dem Sektor ist müssen die folgenden Variablen gespeichert werden
		if (m_bSunSystem)
		{
			ar << m_bySunColor;
			ar << m_Planets.size();
			for(iterator it = begin(); it != end(); ++it)
				it->Serialize(ar);
		}
	}
	else
	// Alle Variablen in der richtigen Reihenfolge lesen
	{
		ar >> m_sColonyOwner;
		CString home_of;
		ar >> home_of;
		if(home_of.IsEmpty())
			m_HomeOf.reset();
		else if(home_of != HomeOfID())
			SetHomeOf(home_of);
		// Nur wenn ein Sonnensystem in dem Sektor ist müssen die folgenden Variablen geladen werden
		if (m_bSunSystem)
		{
			ar >> m_bySunColor;
			int number = 0;
			ar >> number;
			m_Planets.clear();
			m_Planets.resize(number);
			for(iterator it = begin(); it != end(); ++it)
				it->Serialize(ar);
		}
		else
		{
			m_bySunColor = 0;
			m_Planets.clear();
		}
	}
}

//////////////////////////////////////////////////////////////////////
// iterators
//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////
// getting
//////////////////////////////////////////////////////////////////////

CString CSector::HomeOfID() const
{
	if(!m_HomeOf)
		return "";
	return m_HomeOf->GetRaceID();
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

//////////////////////////////////////////////////////////////////////
// setting
//////////////////////////////////////////////////////////////////////

void CSector::SetHomeOf(const CString& race_id)
{
	if(race_id.IsEmpty())
	{
		m_HomeOf.reset();
		return;
	}
	m_HomeOf = resources::pDoc->GetRaceCtrl()->GetRaceSafe(race_id);
}

//////////////////////////////////////////////////////////////////////
// planets
//////////////////////////////////////////////////////////////////////

/// Funktion generiert den Sektor. Dabei wird als Parameter die Wahrscheinlichkeit, ob in dem Sektor ein
/// Sonnensystem ist, im Paramter <code>sunProb</code> in Prozent übergeben. Im Parameter <code>minorProb</code>
/// wird die Wahrscheinlichkeit in Prozent übergeben, dass sich in dem Sektor eine Minorrace befindet.
void CSector::GenerateSector(int sunProb, int minorProb)
{
	if (m_bSunSystem)
		return;

	// Zahl[0,99] generieren und vergleichen (Sonnensystem?)
	if (rand()%100 >= (100 - sunProb))
	{
		m_bSunSystem = true;
		// Zahl[0,99] generieren und vergleichen (Minorrace?)
		bool bMinor = rand()%100 >= (100 - minorProb);
		m_sName = CGenSectorName::GetInstance()->GetNextRandomSectorName(m_Co, bMinor);
		// bMinor wird in der Generierungsfunktion angepasst, falls es keine Minorracesystemnamen mehr gibt
		if(bMinor)
		{
			m_HomeOf = resources::pDoc->GetRaceCtrl()->GetMinorRace(m_sName);
			AssertBotE(m_HomeOf);
		}

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

	if (m_bSunSystem)
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
							zone = planet.Create(m_sName, zone, i, true);
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
				zone = planet.Create(m_sName, zone, i, GetMinorRace());
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
		if(i->second->GetCo() != m_Co || i->second->GetCurrentOrder() != SHIP_ORDER::TERRAFORM)
			continue;
		const unsigned planet = i->second->GetTerraform();
		CPlanet& p = m_Planets.at(planet);
		AssertBotE(p.GetHabitable());
		//It is allowed to terraform the same planet with 2+ independent ships
		if(p.GetIsTerraforming() || p.GetTerraformed())
			continue;
		p.SetIsTerraforming(TRUE);
		unsigned erased = terraformable.erase(planet);
		AssertBotE(erased == 1);
	}
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
			AssertBotE(oddHab <= FLT_EPSILON);
			break;
		}
	}
}

void CSector::Terraforming(CShip& ship)
{
	const short nPlanet = ship.GetTerraform();
	AssertBotE(-1 <= nPlanet && nPlanet < static_cast<int>(m_Planets.size()));
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

void CSector::SystemEventPlanetMovement(CString& message)
{
	const int nSize = m_Planets.size();
	AssertBotE(nSize >= 1);
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

					const boost::shared_ptr<CEventRandom> EmpireEvent = boost::make_shared<CEventRandom>(
						major.GetRaceID(),"demographic",CLoc::GetString("SYSTEMEVENTPLANETDEMOGRAPHICTITLE"),message);
					major.GetEmpire()->PushEvent(EmpireEvent);
				}
			}
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

/// In jeder neuen Runde die IsTerraforming und IsStationBuilding Variablen auf FALSE setzen, wenn Schiffe eine Aktion
/// machen, werden diese Variablen später ja wieder korrekt gesetzt. Außerdem werden auch die Besitzerpunkte wieder
/// gelöscht.
void CSector::ClearAllPoints(bool call_up)
{
	// Funktion bei jeder neuen Runde anfangs aufrufen!!! Wenn nämlich in diesem Sektor gerade keine Station einer
	// Rasse gebaut wird, dann setzen wir auch die noch gebrauchten Punkte und die anfänglich gebrauchten Punkte
	// wieder auf NULL

	// Falls der Planet gerade geterraformt wird, wird er hier erstmal wieder auf FALSE gesetzt.
	for(iterator it = m_Planets.begin(); it != m_Planets.end(); ++it)
		it->SetIsTerraforming(FALSE);

	if(call_up)
		CMapTile::ClearAllPoints(call_up);
}
