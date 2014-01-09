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
#include "boost/make_shared.hpp"
#include "Ships/ShipMap.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// construction/destruction
//////////////////////////////////////////////////////////////////////
CMapTile::CMapTile(void) :
	CInGameEntity(),
	m_Ships(),
	m_pAnomaly()
{
	Reset(false);
}

CMapTile::CMapTile(int x, int y) :
	CInGameEntity(x, y),
	m_Ships(),
	m_pAnomaly()
{
	Reset(false);
}

CMapTile::CMapTile(const CMapTile& other) :
	CInGameEntity(other),
	m_bSunSystem(other.m_bSunSystem),
	m_Status(other.m_Status),
	m_bShipPort(other.m_bShipPort),
	m_Ships(other.m_Ships),
	m_IsStationBuild(other.m_IsStationBuild),
	m_iScanPower(other.m_iScanPower),
	m_iNeededScanPower(other.m_iNeededScanPower),
	m_iNeededStationPoints(other.m_iNeededStationPoints),
	m_iStartStationPoints(other.m_iStartStationPoints),
	m_byOwnerPoints(other.m_byOwnerPoints),
	m_pAnomaly(other.m_pAnomaly)
{
};

CMapTile& CMapTile::operator=(const CMapTile& other){
	if(this != &other )
	{
		CInGameEntity::operator=(other);

		m_bSunSystem = other.m_bSunSystem;
		m_Status = other.m_Status;
		m_bShipPort = other.m_bShipPort;
		m_Ships = other.m_Ships;
		m_IsStationBuild = other.m_IsStationBuild;
		m_iScanPower = other.m_iScanPower;
		m_iNeededScanPower = other.m_iNeededScanPower;
		m_iNeededStationPoints = other.m_iNeededStationPoints;
		m_iStartStationPoints = other.m_iStartStationPoints;
		m_byOwnerPoints = other.m_byOwnerPoints;
		m_pAnomaly = other.m_pAnomaly;
	}

	return *this;
};

CMapTile::~CMapTile(void)
{
	Reset(false);
}

/// Resetfunktion für die Klasse CMapTile
void CMapTile::Reset(bool call_up)
{
	if(call_up)
		CInGameEntity::Reset();

	m_bSunSystem = false;

	// Maps löschen
	m_Status.clear();
	m_iStartStationPoints.clear();
	m_iNeededStationPoints.clear();

	m_pAnomaly.reset();

	ClearAllPoints(false);
}

//////////////////////////////////////////////////////////////////////
// serialization
//////////////////////////////////////////////////////////////////////
void CMapTile::Serialize(CArchive &ar)
{
	CInGameEntity::Serialize(ar);

	// Wird geschrieben?
	if (ar.IsStoring())
	// Alle Variablen in der richtigen Reihenfolge schreiben
	{
		// alle Maps speichern
		ar << m_Status.size();
		for (map<CString, DISCOVER_STATUS>::const_iterator it = m_Status.begin(); it != m_Status.end(); ++it)
			ar << it->first << static_cast<BYTE>(it->second);
		ar << m_bShipPort.size();
		for (set<CString>::const_iterator it = m_bShipPort.begin(); it != m_bShipPort.end(); ++it)
			ar << *it;
		ar << m_IsStationBuild.size();
		for (std::map<CString, SHIP_ORDER::Typ>::const_iterator it = m_IsStationBuild.begin();
				it != m_IsStationBuild.end(); ++it)
			ar << it->first << static_cast<unsigned>(it->second);
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

		ar << m_bSunSystem;
		bool anomaly = m_pAnomaly;
		ar << anomaly;
		if(anomaly)
			m_pAnomaly->Serialize(ar);
	}
	else
	// Alle Variablen in der richtigen Reihenfolge lesen
	{
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

		ar >> m_bSunSystem;
		bool anomaly(false);
		ar >> anomaly;
		if(anomaly)
		{
			if(!m_pAnomaly)
				CreateAnomaly();
			m_pAnomaly->Serialize(ar);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// getting
//////////////////////////////////////////////////////////////////////

CString CMapTile::CoordsName(bool pretty) const
{
	CString s;
	if(pretty)
	{
		if(m_bSunSystem)
		{
			AssertBotE(!m_sName.IsEmpty());
			s = GenerateName(m_sName, m_Co);
			return s;
		}
		else if(const boost::shared_ptr<const CShips>& station = GetStation())
		{
			s = GenerateName(station->GetName(), m_Co);
			return s;
		}
	}
	s = GenerateName("", m_Co);
	return s;
}

CString CMapTile::GenerateName(const CString& pure_name, const CPoint& co)
{
	CString s;
	if(pure_name.IsEmpty())
		s.Format("%s %s", CLoc::GetString("SECTOR"), CPointToCString(co));
	else
		s.Format("%s %s", pure_name, CPointToCString(co));
	return s;
}

const boost::shared_ptr<const CShips> CMapTile::GetStation(const CString& sRace) const
{
	const boost::shared_ptr<const CShips>& outpost = GetStation(SHIP_TYPE::OUTPOST, sRace);
	const boost::shared_ptr<const CShips>& starbase = GetStation(SHIP_TYPE::STARBASE, sRace);
	AssertBotE(!outpost || !starbase);
	if(outpost)
		return outpost;
	else
		return starbase;
}

boost::shared_ptr<const CShips> CMapTile::GetStation(SHIP_TYPE::Typ type, const CString& sRace) const
{
	if(sRace.IsEmpty())
	{
		for(std::map<CString, CShipMap>::const_iterator i = m_Ships.begin(); i != m_Ships.end(); ++i)
			for(CShipMap::const_iterator i2 = i->second.begin(); i2 != i->second.end(); ++i2)
				if(i2->second->GetShipType() == type)
					return i2->second;

	}
	else
	{
		std::map<CString, CShipMap>::const_iterator found = m_Ships.find(sRace);
		if(found == m_Ships.end())
			return boost::shared_ptr<CShips>();
		AssertBotE(!found->second.empty());
		for(CShipMap::const_iterator i = found->second.begin(); i != found->second.end(); ++i)
			if(i->second->GetShipType() == type)
				return i->second;
	}
	return boost::shared_ptr<CShips>();
}

bool CMapTile::GetOwnerOfShip(const CString& sRace, bool consider_stations_ships) const
{
	std::map<CString, CShipMap>::const_iterator found = m_Ships.find(sRace);
	if(found == m_Ships.end())
		return false;
	AssertBotE(!found->second.empty());
	if(consider_stations_ships)
		return true;
	for(CShipMap::const_iterator it = found->second.begin(); it != found->second.end(); ++it)
		if(!it->second->IsStation())
			return true;
	return false;
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
		for(std::map<CString, CShipMap>::const_iterator it = m_Ships.begin(); it != m_Ships.end(); ++it) {
			if(pRace->GetRaceID() == it->first || pRace->GetAgreement(it->first) >= DIPLOMATIC_AGREEMENT::AFFILIATION)
			{
				scan_power_due_to_ship_number += it->second.GetSize();
				for(CShipMap::const_iterator itt = it->second.begin(); itt != it->second.end(); ++itt)
					scan_power_due_to_ship_number += itt->second->GetFleetSize();
			}

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
	const CString& owner = sector.OwnerID();
	return owner.IsEmpty() || owner == race || sector.GetIsStationBuilding(race);
}

bool CMapTile::IsStationBuildable(SHIP_ORDER::Typ order, const CString& race) const {
	if(order == SHIP_ORDER::BUILD_OUTPOST && !GetStation())
		return StationBuildContinuable(race, *this);
	if(order == SHIP_ORDER::BUILD_STARBASE && GetStation(SHIP_TYPE::OUTPOST, race))
		return StationBuildContinuable(race, *this);
	if(order == SHIP_ORDER::UPGRADE_OUTPOST && GetStation(SHIP_TYPE::OUTPOST, race)
		|| order == SHIP_ORDER::UPGRADE_STARBASE && GetStation(SHIP_TYPE::STARBASE, race)) {
		const CBotEDoc* pDoc = resources::pDoc;
		const boost::shared_ptr<const CMajor> pMajor =
			boost::dynamic_pointer_cast<CMajor>(pDoc->GetRaceCtrl()->GetRaceSafe(race));
		const SHIP_TYPE::Typ type = (order == SHIP_ORDER::UPGRADE_OUTPOST)
			? SHIP_TYPE::OUTPOST : SHIP_TYPE::STARBASE;
		const int bestbuildableID = pMajor->BestBuildableVariant(type, pDoc->m_ShipInfoArray);
		if(bestbuildableID == -1)
			return false;
		const CShipInfo& bestbuildableinfo = pDoc->m_ShipInfoArray.GetAt(bestbuildableID-10000);
		for(CShipMap::const_iterator k = pDoc->m_ShipMap.begin(); k != pDoc->m_ShipMap.end(); ++k)
			if (k->second->GetShipType() == type && k->second->GetCo() == m_Co)
			{
				const CShipInfo& info = pDoc->m_ShipInfoArray.GetAt(k->second->GetID()-10000);
				if (info.GetBaseIndustry() < bestbuildableinfo.GetBaseIndustry()) {
					return StationBuildContinuable(race, *this);
				}
				CShips temp(bestbuildableinfo);
				temp.AddSpecialResearchBoni(pMajor);
				if(k->second->IsWorseThan(temp))
					return StationBuildContinuable(race, *this);
				break;
			}
	}
	return false;
}

void CMapTile::BuildStation(SHIP_TYPE::Typ station, const CString& race) {
	if(station == SHIP_TYPE::STARBASE)
	{
		const boost::shared_ptr<const CShips>& outpost = GetStation(SHIP_TYPE::OUTPOST, race);
		const boost::shared_ptr<const CShips>& starbase = GetStation(SHIP_TYPE::STARBASE, race);
		AssertBotE(outpost && outpost->OwnerID() == race || starbase && starbase->OwnerID() == race);
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
		const CRace& race, bool bBetterScanner, bool patrolship, bool anomaly) const {
	const CString& race_id = race.GetRaceID();
	float boni = 1.0f;
	// Wenn das Schiff die Patrouillieneigenschaft besitzt und sich in einem eigenen Sektor befindet,
	// dann wird die Scanleistung um 20% erhöht.
	if(patrolship) {
		if(race_id == OwnerID() || race.GetAgreement(OwnerID()) >= DIPLOMATIC_AGREEMENT::AFFILIATION)
			boni = 1.2f;
	}
	if(bBetterScanner) {
		range *= 1.5;
		boni += 0.5;
	}
	const int intrange = static_cast<int>(range);
	for (int i = -intrange; i <= intrange; ++i) {
		const int x = m_Co.x + i;
		if(0 <= x && x < STARMAP_SECTORS_HCOUNT) {
			for (int j = -intrange; j <= intrange; ++j) {
				const int y = m_Co.y + j;
				if(0 <= y && y < STARMAP_SECTORS_VCOUNT) {
					CBotEDoc* pDoc = resources::pDoc;
					CMapTile& scanned_sector = static_cast<CMapTile&>(pDoc->GetSystem(x, y));
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
	if (!m_bSunSystem && !m_pAnomaly)
		return;

	AssertBotE(pDC);
	AssertBotE(pDoc);
	AssertBotE(pPlayer);

	// Ist Sektor bekannt dann zeichne den Systemnamen
	if (this->GetKnown(pPlayer->GetRaceID()) == TRUE)
	{
		COLORREF clrTextColor = CFontLoader::GetFontColor(pPlayer, 0);
		pDC->SetTextColor(clrTextColor);
		if (m_Owner && m_Owner->IsMajor())
		{
			if (pPlayer->IsRaceContacted(OwnerID()) == true || pPlayer->GetRaceID() == OwnerID())
				pDC->SetTextColor(dynamic_cast<CMajor*>(m_Owner.get())->GetDesign()->m_clrGalaxySectorText);
		}
		// Systemnamen zeichnen
		if (!m_pAnomaly)
			pDC->DrawText(m_sName, CRect(m_Co.x*STARMAP_SECTOR_WIDTH, m_Co.y*STARMAP_SECTOR_HEIGHT, m_Co.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH,m_Co.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT), DT_CENTER | DT_BOTTOM | DT_SINGLELINE);
		else
		{
			//CRect(m_Co.x*STARMAP_SECTOR_WIDTH-50,m_Co.y*STARMAP_SECTOR_HEIGHT,m_Co.x*STARMAP_SECTOR_WIDTH+90,m_Co.y*STARMAP_SECTOR_HEIGHT+40)
			pDC->DrawText(m_pAnomaly->GetMapName(m_Co), CRect(m_Co.x*STARMAP_SECTOR_WIDTH, m_Co.y*STARMAP_SECTOR_HEIGHT,m_Co.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH,m_Co.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT), DT_CENTER | DT_BOTTOM | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		}
	}
}

bool CMapTile::ShouldDrawShip(const CMajor& our_race, const CString& their_race_id) const {
	if(!GetOwnerOfShip(their_race_id, false))
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
		&& !GetStation(SHIP_TYPE::OUTPOST, their_race_id)
		&& !GetStation(SHIP_TYPE::STARBASE, their_race_id))
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
	const CRaceController& race_ctrl = *pDoc->GetRaceCtrl();

	CString sFilePath;
	short nCount = 0;

	CPoint pt;
	pt.x = m_Co.x * STARMAP_SECTOR_WIDTH;
	pt.y = m_Co.y * STARMAP_SECTOR_HEIGHT;

	// durch alle Rassen iterieren und Schiffsymbole zeichnen
	CString sAppPath = CIOData::GetInstance()->GetAppPath();
	for (CRaceController::const_iterator it = race_ctrl.begin(); it != race_ctrl.end(); ++it)
	{
		if (ShouldDrawShip(*pPlayer, it->first))
		{
			// handelt es sich um ein Alienschiff?
			if (pPlayer != it->second.get() && it->second->IsAlien())
				sFilePath = sAppPath + "Graphics\\Symbols\\Entity.bop";
			// ist der Besitzer des Schiffes unbekannt, dann Fragezeichen zeichnen
			else if (pPlayer != it->second.get() && pPlayer->IsRaceContacted(it->first) == false)
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
			if (pPlayer != it->second.get() && it->second->IsAlien())
				sFilePath = sAppPath + "Graphics\\Symbols\\Entity.bop";
			// ist der besitzer des Schiffes unbekannt, dann Fragezeichen zeichnen
			else if (pPlayer != it->second.get() && pPlayer->IsRaceContacted(it->first) == false)
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
	const boost::shared_ptr<const CShips>& station = GetStation();
	if(station)
	{
		SetOwner(station->OwnerID());
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
		SetScanned(newOwner);
	SetOwner(newOwner);
}

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

/// Funktion erzeugt eine zufällige Anomalie im Sektor.
void CMapTile::CreateAnomaly(void)
{
	m_pAnomaly = boost::make_shared<CAnomaly>(CAnomaly());
}

/// In jeder neuen Runde die IsTerraforming und IsStationBuilding Variablen auf FALSE setzen, wenn Schiffe eine Aktion
/// machen, werden diese Variablen später ja wieder korrekt gesetzt. Außerdem werden auch die Besitzerpunkte wieder
/// gelöscht.
void CMapTile::ClearAllPoints(bool /*call_up*/)
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

	// Die benötigte Scanpower um Schiffe sehen zu können wieder auf NULL setzen
	m_iNeededScanPower.clear();
	m_iScanPower.clear();
	m_bShipPort.clear();
}

void CMapTile::AddShip(const boost::shared_ptr<CShips>& ship)
{
	std::map<CString, CShipMap>::iterator race_ships =
		m_Ships.insert(std::pair<CString, CShipMap>(ship->OwnerID(), CShipMap(NULL, true))).first;
	race_ships->second.Add(ship);
}

void CMapTile::EraseShip(const boost::shared_ptr<const CShips>& ship)
{
	std::map<CString, CShipMap>::iterator race_ships = m_Ships.find(ship->OwnerID());
	AssertBotE(race_ships != m_Ships.end());
	CShipMap::iterator to_erase = race_ships->second.find(ship->MapTileKey());
	AssertBotE(to_erase != race_ships->second.end());
	race_ships->second.EraseAt(to_erase);
	if(race_ships->second.empty())
		m_Ships.erase(race_ships);
}

void CMapTile::ClearShips()
{
	while(!m_Ships.empty())
	{
		CShipMap& ships = m_Ships[m_Ships.begin()->first];
		EraseShip(ships.front());
	}
}

bool CMapTile::CheckSanity() const
{
	AssertBotE(!GetStation(SHIP_TYPE::OUTPOST) || !GetStation(SHIP_TYPE::STARBASE));
	for(std::map<CString, CShipMap>::const_iterator it = m_Ships.begin(); it != m_Ships.end(); ++it)
		for(CShipMap::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			if(it2->second->GetCo() != m_Co)
				return false;
	return true;
}