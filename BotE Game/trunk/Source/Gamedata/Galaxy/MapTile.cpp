/*
 *@file
 * properties of a map location with or without a sun system (and with or without any buildings)
 */

#include "stdafx.h"
#include "MapTile.h"
#include "IOData.h"
#include "FontLoader.h"
#include "Races\RaceController.h"
#include "Anomaly.h"
#include "General/Loc.h"
#include "Ships/ships.h"
#include "BotEDoc.h"
#include "Races/Major.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// construction/destruction
//////////////////////////////////////////////////////////////////////
CMapTile::CMapTile(void) :
	m_KO(-1, -1),
	m_pAnomaly(NULL)
{
	Reset();
}

CMapTile::CMapTile(int x, int y) :
	m_KO(x, y),
	m_pAnomaly(NULL)
{
	Reset();
}

CMapTile::CMapTile(const CMapTile& other) :
	m_KO(other.m_KO),
	m_sOwnerOfSector(other.m_sOwnerOfSector),
	m_strSectorName(other.m_strSectorName),
	m_bSunSystem(other.m_bSunSystem),
	m_bOwned(other.m_bOwned),
	m_bMinor(other.m_bMinor),
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
	m_byOwnerPoints(other.m_byOwnerPoints)
{
	m_pAnomaly=NULL;
	if(other.GetAnomaly())
		m_pAnomaly = new CAnomaly(*other.m_pAnomaly);
};

CMapTile& CMapTile::operator=(const CMapTile& other){
	if(this != &other )
	{
		m_KO = other.m_KO;
		m_sOwnerOfSector = other.m_sOwnerOfSector;
		m_strSectorName = other.m_strSectorName;
		m_bSunSystem = other.m_bSunSystem;
		m_bOwned = other.m_bOwned;
		m_bMinor = other.m_bMinor;
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
		m_byOwnerPoints = other.m_byOwnerPoints;
		m_pAnomaly=NULL;
		if(other.GetAnomaly())
			m_pAnomaly = new CAnomaly(*other.m_pAnomaly);
	}

	return *this;
};

CMapTile::~CMapTile(void)
{
	Reset();
}

/// Resetfunktion für die Klasse CMapTile
void CMapTile::Reset()
{
	m_bSunSystem = false;
	m_bOwned = false;
	m_bMinor = false;

	// Maps löschen
	m_Status.clear();
	m_iStartStationPoints.clear();
	m_iNeededStationPoints.clear();

	m_sOwnerOfSector = "";
	m_strSectorName = "";

	delete m_pAnomaly;
	m_pAnomaly = NULL;

	ClearAllPoints();
}

//////////////////////////////////////////////////////////////////////
// serialization
//////////////////////////////////////////////////////////////////////
void CMapTile::Serialize(CArchive &ar)
{
	// alle Maps speichern
	// Maps laden
	m_Status.Serialize(ar);
	m_bShipPort.Serialize(ar);
	m_IsStationBuild.Serialize(ar);
	m_bWhoIsOwnerOfShip.Serialize(ar);
	m_mNumbersOfShips.Serialize(ar);
	m_iNeededStationPoints.Serialize(ar);
	m_iStartStationPoints.Serialize(ar);
	m_iScanPower.Serialize(ar);
	m_iNeededScanPower.Serialize(ar);
	m_byOwnerPoints.Serialize(ar);

	// Wird geschrieben?
	if (ar.IsStoring())
	// Alle Variablen in der richtigen Reihenfolge schreiben
	{
		ar << m_bSunSystem;
		ar << m_bOwned;
		ar << m_bMinor;
		ar << m_KO;
		ar << m_Outpost;
		ar << m_Starbase;
		ar << m_sOwnerOfSector;

		// Nur wenn ein Sonnensystem in dem Sektor ist müssen die folgenden Variablen gespeichert werden
		if (GetSunSystem())
			ar << m_strSectorName;

		ar << m_pAnomaly;
	}
	else
	// Alle Variablen in der richtigen Reihenfolge lesen
	{
		ar >> m_bSunSystem;
		ar >> m_bOwned;
		ar >> m_bMinor;
		ar >> m_KO;
		m_Outpost.Empty();
		ar >> m_Outpost;
		m_Starbase.Empty();
		ar >> m_Starbase;
		ar >> m_sOwnerOfSector;
		// Nur wenn ein Sonnensystem in dem Sektor ist müssen die folgenden Variablen geladen werden
		if (GetSunSystem())
			ar >> m_strSectorName;
		else
			m_strSectorName = "";

		delete m_pAnomaly;
		ar >> m_pAnomaly;
	}
}

//////////////////////////////////////////////////////////////////////
// getting
//////////////////////////////////////////////////////////////////////

/// Funktion gibt den Namen des Sektors zurück. Wenn in ihm kein Sonnensystem ist, dann wird "" zurückgegeben.
/// Wenn man aber den Parameter <code>longName<code> beim Aufruf der Funktion auf <code>TRUE<code> setzt, wird
/// versucht ein genauerer Sektorname zu generieren.
CString CMapTile::GetName(BOOLEAN longName) const
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

/// Diese Funktion gibt die Scanpower zurück, die die Majorrace <code>Race</code> in diesem Sektor hat.
short CMapTile::GetScanPower(const CString& sRace, bool bWith_ships) const
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

//////////////////////////////////////////////////////////////////////
// stations
//////////////////////////////////////////////////////////////////////

static bool StationBuildContinuable(const CString& race, const CMapTile& sector) {
	const CString& owner = sector.GetOwnerOfSector();
	return owner.IsEmpty() || owner == race || sector.GetIsStationBuilding(race);
}

bool CMapTile::IsStationBuildable(SHIP_ORDER::Typ order, const CString& race) const {
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

void CMapTile::BuildStation(SHIP_TYPE::Typ station, const CString& race) {
	if(station == SHIP_TYPE::OUTPOST)
		m_Outpost = race;
	else {
		AssertBotE(station == SHIP_TYPE::STARBASE);
		AssertBotE(m_Outpost == race || m_Starbase == race);
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

//////////////////////////////////////////////////////////////////////
// scanning
//////////////////////////////////////////////////////////////////////

void CMapTile::IncrementNumberOfShips(const CString& race) {
	const std::map<CString, unsigned>::iterator found = m_mNumbersOfShips.find(race);
	if(found == m_mNumbersOfShips.end()) {
		m_mNumbersOfShips.insert(std::pair<CString, unsigned>(race, 1));
		return;
	}
	++(found->second);
}

/// Funktion legt die Scanpower <code>scanpower</code>, welche die Majorrace <code>Race</code>
/// in diesem Sektor hat, fest.
void CMapTile::SetScanPower(short scanpower, const CString& Race)
{
	if (scanpower)
		m_iScanPower[Race] = scanpower;
	else
		m_iScanPower.erase(Race);
}

void CMapTile::PutScannedSquare(unsigned range, const int power,
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
					CMapTile& scanned_sector = static_cast<CMapTile&>(pDoc->GetSector(x, y));
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

//////////////////////////////////////////////////////////////////////
// drawing
//////////////////////////////////////////////////////////////////////
/// Diese Funktion zeichnet den Namen des Sektors.
void CMapTile::DrawSectorsName(CDC *pDC, CBotEDoc* pDoc, CMajor* pPlayer)
{
	// befindet sich kein Sonnensystem oder keine Anomalie im Sektor
	if (!GetSunSystem() && !GetAnomaly())
		return;

	AssertBotE(pDC);
	AssertBotE(pDoc);
	AssertBotE(pPlayer);

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

bool CMapTile::ShouldDrawShip(const CMajor& our_race, const CString& their_race_id) const {
	if(!GetOwnerOfShip(their_race_id))
		return false;
	const CString& our_id = our_race.GetRaceID();
	if(our_id == their_race_id)
		return true;
	if(GetNeededScanPower(their_race_id) < GetScanPower(our_id))
		return true;
	return our_race.GetAgreement(their_race_id) >= DIPLOMATIC_AGREEMENT::AFFILIATION;
}

bool CMapTile::ShouldDrawOutpost(const CMajor& our_race, const CString& their_race_id) const {
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
void CMapTile::DrawShipSymbolInSector(Graphics *g, CBotEDoc* pDoc, CMajor* pPlayer) const
{
	AssertBotE(g);
	AssertBotE(pDoc);
	AssertBotE(pPlayer);

	// alle Rassen holen
	map<CString, CRace*>* pmRaces = pDoc->GetRaceCtrl()->GetRaces();
	AssertBotE(pmRaces);

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

//////////////////////////////////////////////////////////////////////
// owner
//////////////////////////////////////////////////////////////////////

/// Diese Funktion berechnet anhand der Besitzerpunkte und anderen Enflüssen, wem dieser Sektor schlussendlich
/// gehört.
void CMapTile::CalculateOwner()
{
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
	CString newOwner;
	for (map<CString, BYTE>::const_iterator it = m_byOwnerPoints.begin(); it != m_byOwnerPoints.end(); ++it)
	{
		if (it->second > mostPoints)
		{
			mostPoints = it->second;
			newOwner = it->first;
		}
		// bei Gleichstand wird der Besitzer wieder gelöscht
		else if (it->second == mostPoints)
			newOwner.Empty();
	}
	if (!newOwner.IsEmpty())
	{
		SetOwned(TRUE);
		SetScanned(newOwner);
	}
	else
		SetOwned(FALSE);
	m_sOwnerOfSector = newOwner;
}

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

/// Funktion erzeugt eine zufällige Anomalie im Sektor.
void CMapTile::CreateAnomaly(void)
{
	if (m_pAnomaly)
	{
		delete m_pAnomaly;
		m_pAnomaly = NULL;
	}

	m_pAnomaly = new CAnomaly();
}

/// In jeder neuen Runde die IsTerraforming und IsStationBuilding Variablen auf FALSE setzen, wenn Schiffe eine Aktion
/// machen, werden diese Variablen später ja wieder korrekt gesetzt. Außerdem werden auch die Besitzerpunkte wieder
/// gelöscht.
void CMapTile::ClearAllPoints()
{
	// Funktion bei jeder neuen Runde anfangs aufrufen!!! Wenn nämlich in diesem Sektor gerade keine Station einer
	// Rasse gebaut wird, dann setzen wir auch die noch gebrauchten Punkte und die anfänglich gebrauchten Punkte
	// wieder auf NULL

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