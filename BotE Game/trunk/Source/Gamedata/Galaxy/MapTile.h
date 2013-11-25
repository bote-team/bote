/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
/*
 *@file
 * properties of a map location without a sun system
 */

#pragma once

#include "BoteStandardTypes.h"


#include "resources.h"
#include "CommandLineParameters.h"
#include "boost/exception/all.hpp"

using namespace std;

// forward declaration
class CBotEDoc;
class CMajor;
class CAnomaly;
class CRace;
class CShips;
class CShip;

class CMapTile
{

public:

	enum DISCOVER_STATUS
	{
		DISCOVER_STATUS_NONE		= 0,
		DISCOVER_STATUS_SCANNED		= 1,
		DISCOVER_STATUS_KNOWN		= 2,
		DISCOVER_STATUS_FULL_KNOWN	= 3
	};

//////////////////////////////////////////////////////////////////////
// construction/destruction
//////////////////////////////////////////////////////////////////////

	/// Konstruktor
	CMapTile(void);
	CMapTile(int x, int y);

	///Kopierkontruktor
	CMapTile(const CMapTile& other);

	CMapTile& operator=(const CMapTile&);

	/// Destruktor
	virtual ~CMapTile(void);

protected:
	/// Resetfunktion für die Klasse CMapTile
	void Reset();
public:

//////////////////////////////////////////////////////////////////////
// serialization
//////////////////////////////////////////////////////////////////////

protected:
	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);
public:

//////////////////////////////////////////////////////////////////////
// getting
//////////////////////////////////////////////////////////////////////

// Zugriffsfunktionen zum Lesen der Membervariablen
	/// Diese Funktion gibt die Koordinaten des Sektors zurück.
	CPoint GetKO() const {return m_KO;}

	/**
	 * Funktion gibt den Namen des Sektors zurück. Wenn in ihm kein Sonnensystem ist, dann wird "" zurückgegeben.
	 * Wenn man aber den Parameter <code>longName<code> beim Aufruf der Funktion auf <code>TRUE<code> setzt, wird
	 * versucht ein genauerer Sektorname zu generieren.
	 */
	CString GetName(BOOLEAN longName = FALSE) const;

//// SectorAttributes ////
	/// Diese Funktion gibt zurück, ob sich in diesem Sektor ein Sonnensystem befindet.
	BOOLEAN GetSunSystem(void) const {return m_bSunSystem;}

//// DISCOVER_STATUS ////
	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob dieser Sektor von der Majorrace
	/// <code>Race</code> gescannt wurde.
	bool GetScanned(const CString& sRace) const
	{
		const CCommandLineParameters* const clp = resources::pClp;
		if(clp->SeeAllOfMap())
			return true;

		map<CString, DISCOVER_STATUS>::const_iterator it = m_Status.find(sRace);
		if (it != m_Status.end())
			return it->second >= DISCOVER_STATUS_SCANNED;
		return false;
	}

	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob der Name dieses Sektor der
	/// Majorrace <code>Race</code> bekannt ist.
	bool GetKnown(const CString& sRace) const
	{
		const CCommandLineParameters* const clp = resources::pClp;
		if(clp->SeeAllOfMap())
			return true;

		map<CString, DISCOVER_STATUS>::const_iterator it = m_Status.find(sRace);
		if (it != m_Status.end())
			return it->second >= DISCOVER_STATUS_KNOWN;
		return false;
	}

	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob die Majorrace <code>Race</code>
	/// den kompletten Sektor (inkl. der Planeten) kennt.
	bool GetFullKnown(const CString& sRace) const
	{
		const CCommandLineParameters* const clp = resources::pClp;
		if(clp->SeeAllOfMap())
			return true;

		map<CString, DISCOVER_STATUS>::const_iterator it = m_Status.find(sRace);
		if (it != m_Status.end())
			return it->second >= DISCOVER_STATUS_FULL_KNOWN;
		return false;
	}

	const DISCOVER_STATUS GetDiscoverStatus(const CString& sRace) const
	{
		const CCommandLineParameters* const clp = resources::pClp;
		if(clp->SeeAllOfMap())
			return DISCOVER_STATUS_FULL_KNOWN;

		map<CString, DISCOVER_STATUS>::const_iterator it = m_Status.find(sRace);
		if (it != m_Status.end())
			return it->second;
		return DISCOVER_STATUS_NONE;
	}

	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob die Majorrace <code>Race</code>
	/// eine online Werft (bzw. kann auch eine Station sein) in diesem Sektor besitzt.
	bool GetShipPort(const CString& sRace) const
	{
		if (m_bShipPort.find(sRace) != m_bShipPort.end())
			return true;
		return false;
	}

	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob die Majorrace <code>Race</code>
	/// einen Aussenposten in diesem Sektor besitzt.
	bool GetOutpost(const CString& sRace) const
	{
		return m_Outpost == sRace;
	}

	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob die Majorrace <code>Race</code>
	/// eine Sternbasis in diesem Sektor besitzt.
	bool GetStarbase(const CString& sRace) const
	{
		return m_Starbase == sRace;
	}

//// ships in sector ////
	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob die Rasse <code>Race</code> ein
	/// bzw. mehrere Schiffe in diesem Sektor hat.
	BOOLEAN GetOwnerOfShip(const CString& sRace) const
	{
		if (m_bWhoIsOwnerOfShip.find(sRace) != m_bWhoIsOwnerOfShip.end())
			return true;
		return false;
	}

	std::set<CString> ShipsInSector() const { return m_bWhoIsOwnerOfShip; }

	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob von irgendwem ein Schiff in diesem
	/// Sektor ist
	BOOLEAN GetIsShipInSector(void) const {return !m_bWhoIsOwnerOfShip.empty();}

	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob von irgendwem eine Station in diesem
	/// Sektor ist
	BOOLEAN GetIsStationInSector(void) const {return HasOutpost() || HasStarbase();}

	bool HasOutpost() const { return !m_Outpost.IsEmpty(); }
	bool HasStarbase() const { return !m_Starbase.IsEmpty(); }

	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob die Majorrace <code>Race</code>
	/// gerade eine Station in diesem Sektor baut.
	BOOLEAN GetIsStationBuilding(const CString& sRace) const
	{
		return m_IsStationBuild.find(sRace) != m_IsStationBuild.end();
	}
	SHIP_ORDER::Typ StationWork(const CString& sRace) const
	{
		const std::map<CString, SHIP_ORDER::Typ>::const_iterator result = m_IsStationBuild.find(sRace);
		AssertBotE(result != m_IsStationBuild.end());
		return result->second;
	}

	/// Diese Funktion gibt die benötigten Punkte zum Stationenbau zurück, die die Majorrace
	/// <code>Race</code> in diesem Sektor noch zur Fertigstellung einer Station benötigt.
	short GetNeededStationPoints(const CString& sRace) const
	{
		map<CString, short>::const_iterator it = m_iNeededStationPoints.find(sRace);
		if (it != m_iNeededStationPoints.end())
			return it->second;
		else
			return false;
	}

	/// Diese Funktion gibt die Punkte zum Stationenbau zurück, die die Majorrace
	/// <code>Race</code> zu Beginn des Stationenbaus benötigt hat.
	short GetStartStationPoints(const CString& sRace) const
	{
		map<CString, short>::const_iterator it = m_iStartStationPoints.find(sRace);
		if (it != m_iStartStationPoints.end())
			return it->second;
		else
			return false;
	}

//// scan power ////
	/// Diese Funktion gibt die Scanpower zurück, die die Majorrace <code>Race</code> in diesem Sektor hat.
	short GetScanPower(const CString& sRace, bool bWith_ships = true) const;

	/// Diese Funktion gibt die Scanpower zurück, die man benötigt, um das Schiff/die Schiffe der
	/// Majorrace <code>Race</code> in diesem Sektor zu entdecken.
	short GetNeededScanPower(const CString& sRace) const
	{
		map<CString, short>::const_iterator it = m_iNeededScanPower.find(sRace);
		if (it != m_iNeededScanPower.end())
			return it->second;
		else
			return MAXSHORT;
	}

	/// Diese Funktion gibt einen Zeiger auf eine eventuell vorhandene Anomalie zurück (<code>NULL</code> wenn kein vorhanden)
	const CAnomaly* GetAnomaly(void) const {return m_pAnomaly;}

//////////////////////////////////////////////////////////////////////
// setting
//////////////////////////////////////////////////////////////////////

	/// Funktion legt den Namen des Sektors fest.
	void SetSectorsName(const CString& nameOfSunSystem) {m_strSectorName = nameOfSunSystem;}

//// SectorAttributes ////
	/// Diese Funktion legt fest, ob sich ein Sonnensystem in diesem Sektor befindet.
	void SetSunSystem(BOOLEAN is) { m_bSunSystem = is; }

//// DISCOVER_STATUS ////
	/// Diese Funktion legt fest, ob der Sektor von der Majorrace <code>Race</code> gescannt ist.
	void SetScanned(const CString& Race) {if (this->GetScanned(Race) == FALSE) m_Status[Race] = CMapTile::DISCOVER_STATUS_SCANNED;}

	/// Diese Funktion legt fest, ob die Majorrace <code>Race</code> den Namen des Sektor kennt.
	void SetKnown(const CString& Race) {if (this->GetKnown(Race) == FALSE) m_Status[Race] = CMapTile::DISCOVER_STATUS_KNOWN;}

	/// Diese Funktion legt fest, ob die Majorrace <code>Race</code> den Sektor kompletten Sektor
	/// (inkl. der ganzen Planeten) kennt.
	void SetFullKnown(const CString& Race) {m_Status[Race] = CMapTile::DISCOVER_STATUS_FULL_KNOWN;}

	/// Funktion legt fest, ob die Majorrace <code>Race</code> ein oder auch mehrer Schiffe in diesem Sektor hat.
	void SetOwnerOfShip(BOOLEAN is, const CString& sOwner)
	{
		if (is)
			m_bWhoIsOwnerOfShip.insert(sOwner);
		else
			m_bWhoIsOwnerOfShip.erase(sOwner);
	}

//////////////////////////////////////////////////////////////////////
// stations
//////////////////////////////////////////////////////////////////////

	//is station buildable in this sector by race according to whatever station exists ?
	bool IsStationBuildable(SHIP_ORDER::Typ order, const CString& race) const;

	/// Funktion legt fest, ob die Majorrace <code>Race</code> gerade eine Station in diesem Sektor baut.
	void SetIsStationBuilding(SHIP_ORDER::Typ type, const CString& Race)
	{
		if (type != SHIP_ORDER::NONE)
			m_IsStationBuild.insert(std::pair<CString, SHIP_ORDER::Typ>(Race, type));
		else
			m_IsStationBuild.erase(Race);
	}

	/// Diese Funktion legt fest, ob die Majorrace <code>Race</code> eine online Werft (bzw. auch durch eine
	/// Station erhalten) in diesem Sektor besitzt.
	void SetShipPort(BOOLEAN is, const CString& Race)
	{
		if (is)
			m_bShipPort.insert(Race);
		else
			m_bShipPort.erase(Race);
	}

	/// Diese Funktion legt fest, ob die Majorrace <code>Race</code> einen Aussenposten in diesem Sektor unterhält.
	void SetOutpost(const CString& Race)
	{
		m_Outpost = Race;
	}
	void UnsetOutpost(const CString& Race)
	{
		if(m_Outpost == Race)
			m_Outpost.Empty();
	}

	/// Diese Funktion legt fest, ob die Majorrace <code>Race</code> eine Sternbasis in diesem Sektor unterhält.
	void SetStarbase(const CString& Race)
	{
		m_Starbase = Race;
	}
	void UnsetStarbase(const CString& Race)
	{
		if(m_Starbase == Race)
			m_Starbase.Empty();
	}

	void BuildStation(SHIP_TYPE::Typ station, const CString& race);

	/// Diese Funktion zieht <code>sub</code> Punkte von den noch benötigten Stationbaupunkten der Majorrace
	/// <code>Race</code> ab.
	BOOLEAN SetNeededStationPoints(int sub, const CString& Race)
	{
		m_iNeededStationPoints[Race] -= sub;
		if (m_iNeededStationPoints[Race] <= 0)
		{
			m_iNeededStationPoints.erase(Race);
			return TRUE;
		}
		else return FALSE;
	}

	/// Diese Funktion legt die Stationsbaupunkte <code>value</code> der Majorrace <code>Race</code> fest,
	/// welche zu Beginn des Baus benötigt werden.
	void SetStartStationPoints(int value, const CString& Race)
	{
		if (value)
		{
			m_iStartStationPoints[Race] = value;
			m_iNeededStationPoints[Race] = value;
		}
		else
		{
			m_iStartStationPoints.erase(Race);
			m_iNeededStationPoints.erase(Race);
		}
	}

//////////////////////////////////////////////////////////////////////
// scanning
//////////////////////////////////////////////////////////////////////

	//Add 1 to the number of race's ships in this sector
	void IncrementNumberOfShips(const CString& race);

	/// Funktion legt die Scanpower <code>scanpower</code>, welche die Majorrace <code>Race</code>
	/// in diesem Sektor hat, fest.
	void SetScanPower(short scanpower, const CString& Race);

	/// Function puts a scanned square over this sector in the middle, if range == 1, 9 sectors are affected
	/// for instance. Function calculates and sets the scan power values for each of these sectors.
	/// @param sector: The middle sector where the scan power affecting object is. This can be a ship,
	/// a scanning deteriorating anomaly, or a continuum scanner (or continuum scanner upgrade).
	/// @param range: range of the object
	/// @param power: scan power of the object
	/// @param race: race who gets these scan powers (can be a minor race)
	/// @param bBetterScanner: are we on a scanning improving anomaly (implies the scan power affecting object is a ship or base)
	/// @param patrolship: is this a patrolship (implies the scan power affecting object is a ship or base)
	/// @param anomaly: is the scan power affecting object a scanning deteriorating anomaly
	void PutScannedSquare(unsigned range, const int power,
		const CRace& race, bool bBetterScanner = false, bool patrolship = false, bool anomaly = false) const;

	/// Funktion legt die Scanpower <code>scanpower</code> fest, welche benötigt wird, um ein Schiff der
	/// Majorrace <code>Race</code> in diesem Sektor zu erkennen.
	void SetNeededScanPower(short scanpower, const CString& Race)
	{
		if (scanpower != MAXSHORT)
			m_iNeededScanPower[Race] = scanpower;
		else
			m_iNeededScanPower.erase(Race);
	}

//////////////////////////////////////////////////////////////////////
// drawing
//////////////////////////////////////////////////////////////////////

	// Zeichenfunktionen für diese Klasse
	/// Diese Funktion zeichnet den Namen des Sektors.
	void DrawSectorsName(CDC *pDC, CBotEDoc* pDoc, CMajor* pPlayer);

	//Should any their_race_id's ship in this sector be visible to our_race ?
	//not for outposts/starbases
	bool ShouldDrawShip(const CMajor& our_race, const CString& their_race_id) const;
	//Should any their_race_id's outpost/starbase (perhaps under construction) in this sector be visible to our_race ?
	//only for outposts/starbases
	bool ShouldDrawOutpost(const CMajor& our_race, const CString& their_race_id) const;
	/// Diese Funktion zeichnet die entsprechenden Schiffssymbole in den Sektor
	void DrawShipSymbolInSector(Graphics *g, CBotEDoc* pDoc, CMajor* pPlayer) const;

//////////////////////////////////////////////////////////////////////
// owner
//////////////////////////////////////////////////////////////////////

	/// Diese Funktion addiert Besitzerpunkte <code>ownerPoints</code> zu den Punkten des Besitzers <code>owner</owner>
	/// dazu.
	void AddOwnerPoints(BYTE ownerPoints, const CString& sOwner) {m_byOwnerPoints[sOwner] += ownerPoints;}

	/// Diese Funktion gibt zurück, wer im Besitz dieses Sektors ist.
	const boost::shared_ptr<CRace>& Owner() const;
	CString OwnerID() const;

	bool Free() const { return !m_Owner; }

protected:
	/// Diese Funktion berechnet anhand der Besitzerpunkte und anderen Enflüssen, wem dieser Sektor schlussendlich
	/// gehört. Übergeben wird dafür auch der mögliche Besitzer des Systems in diesem Sektor.
	void CalculateOwner();

	void SetOwner(const CString& id);
public:

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

	/// Funktion erzeugt eine zufällige Anomalie im Sektor.
	void CreateAnomaly(void);

protected:
	/// In jeder neuen Runde die IsTerraforming und IsStationBuilding Variablen auf FALSE setzen, wenn Schiffe eine Aktion
	/// machen, werden diese Variablen später ja wieder korrekt gesetzt. Außerdem werden auch die Besitzerpunkte wieder
	/// gelöscht.
	void ClearAllPoints();
public:

//////////////////////////////////////////////////////////////////////
// members
//////////////////////////////////////////////////////////////////////

protected:
	/// Die Koordinate des Sektors auf der Map
	CPoint m_KO;

	/// Wem gehört der Sektor?
	boost::shared_ptr<CRace> m_Owner;

	/// Der Name des Sectors
	CString m_strSectorName;

	bool m_bSunSystem;// Ist ein Sonnensystem in diesem Sektor?
private:

	/// Variable speichert den Status über diesen Sektor, wobei 0 -> nichts, 1 -> gescannt,
	/// 2 -> Name bekannt, 3 -> alles inkl. Planeten bekannt, bedeutet
	CBotEMap<CString, DISCOVER_STATUS, BYTE> m_Status;

	/// Gibts in diesem Sektor eine online Werft (bzw. auch Station)
	CBotESet<CString> m_bShipPort;

	/// Besitzt die jeweilige Rasse in dem Sektor einen Außenposten?
	CString m_Outpost;

	/// Besitzt die jeweilige Rasse in dem Sektor eine Sternbasis?
	CString m_Starbase;

	/// Hat eine Majorrace ein Schiff in diesem Sektor?
	CBotESet<CString> m_bWhoIsOwnerOfShip;

	//race CString has this many ships in this sector
	CBotEMap<CString, unsigned> m_mNumbersOfShips;

	/// Baut eine bestimmte Majorrasse gerade eine Station in dem Sektor?
	CBotEMap<CString, SHIP_ORDER::Typ, unsigned> m_IsStationBuild;

	/// Scanstärke der jeweiligen Major/Minorrace in dem Sektor
	CBotEMap<CString, short> m_iScanPower;

	/// benötigte Scanstärke der Majorrace, um ihr Schiff in diesem Sektor erkennen zu können
	CBotEMap<CString, short> m_iNeededScanPower;

	/// Die aktuell benötigten Stationsbaupunkte bis zur Fertigstellung
	CBotEMap<CString, short> m_iNeededStationPoints;

	/// Die anfänglich benötigten Stationsbaupunkte (benötigt zur prozentualen Anzeige)
	CBotEMap<CString, short> m_iStartStationPoints;

	/// Punktem welche angeben, wer den größten Einfluss auf diesen Sektor hat. Wer die meisten Punkte hat und kein
	/// anderer in diesem Sektor einen Außenposten oder eine Kolonie besitzt, dem gehört der Sektor
	CBotEMap<CString, BYTE> m_byOwnerPoints;

	/// Mögliche Anomalie in dem Sektor (NULL wenn keine vorhanden)
	CAnomaly* m_pAnomaly;
};
