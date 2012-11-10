// Fleet.cpp: Implementierung der Klasse CFleet.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Fleet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CFleet, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CFleet::CFleet()
{
}

CFleet::~CFleet()
{
	m_vShips.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CFleet::CFleet(const CFleet & rhs)
{
	if (this == &rhs)
		return;

	m_vShips.RemoveAll();
	const CShipArray& shiparray = rhs.m_vShips;
	for(CShipArray::const_iterator it = shiparray.begin(); it != shiparray.end(); ++it)
		m_vShips.Add(m_vShips.end(), *it);
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CFleet & CFleet::operator=(const CFleet & rhs)
{
	if (this == &rhs)
		return *this;

	m_vShips.RemoveAll();
	const CShipArray& shiparray = rhs.m_vShips;
	for(CShipArray::const_iterator it = shiparray.begin(); it != shiparray.end(); ++it)
		m_vShips.Add(m_vShips.end(), *it);

	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CFleet::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);

	m_vShips.Serialize(ar);
}

//////////////////////////////////////////////////////////////////////
// Funktionen der Klasse CFleet
//////////////////////////////////////////////////////////////////////

// Funktion um ein Schiff aus der Flotte zu entfernen. Das n-te Schiff in der Flotte wird entfernt und
// von der Funktion zurückgegeben
void CFleet::RemoveShipFromFleet(UINT nIndex)
{
	ASSERT(nIndex < (UINT)m_vShips.GetSize());
	m_vShips.RemoveAt(nIndex);
}

// Funktion berechnet die Geschwindigkeit der Flotte. Der Parameter der hier übergeben werden sollte
// ist der this-Zeiger des Schiffsobjektes, welches die Flotte besitzt
BYTE CFleet::GetFleetSpeed(const CShip* ship) const
{
	BYTE speed = 127;
	if (ship != 0)
		speed = ship->GetSpeed();
	for (int i = 0; i < m_vShips.GetSize(); i++)
		if (m_vShips.GetAt(i).GetSpeed() < speed)
			speed = m_vShips.GetAt(i).GetSpeed();
	if (speed == 127)
		speed = 0;
	return speed;
}

// Funktion berechnet die Reichweite der Flotte. Der Parameter der hier übergeben werden sollte
// ist der this-Zeiger des Schiffsobjektes, welches die Flotte besitzt
SHIP_RANGE::Typ CFleet::GetFleetRange(const CShip* pShip) const
{
	SHIP_RANGE::Typ nRange = SHIP_RANGE::LONG;
	if (pShip)
		nRange = min(pShip->GetRange(), nRange);

	for (int i = 0; i < m_vShips.GetSize(); i++)
		nRange = min(m_vShips.GetAt(i).GetRange(), nRange);

	return nRange;
}

// Funktion berechnet den Schiffstyp der Flotte. Wenn hier nur der selbe Schiffstyp in der Flotte vorkommt,
// dann gibt die Funktion diesen Schiffstyp zurück. Wenn verschiedene Schiffstypen in der Flotte vorkommen,
// dann liefert und die Funktion ein -1. Der Parameter der hier übergeben werden sollte ist der this-Zeiger
// des Schiffsobjektes, welches die Flotte besitzt
short CFleet::GetFleetShipType(const CShip* ship) const
{
	short type = ship->GetShipType();
	for (int i = 0; i < m_vShips.GetSize(); i++)
		if (m_vShips.GetAt(i).GetShipType() != type)
			return -1;
	return type;
}

// Funktion berechnet die minimale Stealthpower der Flotte. Der Parameter der hier übergeben werden sollte
// ist der this-Zeiger bzw. die Adresse des Schiffsobjektes, welches die Flotte besitzt
BYTE CFleet::GetFleetStealthPower(const CShip* ship) const
{
	BYTE stealthPower = MAXBYTE;
	if (ship != NULL)
	{
		stealthPower = ship->GetStealthPower() * 20;
		if (ship->GetStealthPower() > 3 && ship->GetCloak() == false)
			stealthPower = 3 * 20;
	}
	if (stealthPower == 0)
		return 0;

	for (int i = 0; i < m_vShips.GetSize(); i++)
	{
		if (stealthPower == 0)
			return 0;

		BYTE fleetStealthPower = m_vShips.GetAt(i).GetStealthPower() * 20;
		if (m_vShips.GetAt(i).GetStealthPower() > 3  && m_vShips.GetAt(i).GetCloak() == false)
			fleetStealthPower = 3 * 20;
		if (fleetStealthPower < stealthPower)
			stealthPower = fleetStealthPower;
	}

	return stealthPower;
}

// Funktion übernimmt die Befehle des hier als Zeiger übergebenen Schiffsobjektes an alle Mitglieder der Flotte
void CFleet::AdoptCurrentOrders(const CShip* ship)
{
	for (int i = 0; i < m_vShips.GetSize(); i++)
		m_vShips.ElementAt(i).AdoptOrdersFrom(*ship);
}

// Diese Funktion liefert TRUE wenn die Flotte den "order" ausführen kann. Als Schiffszeiger muß das Schiff
// übergeben werden, welches die Flotte beinhaltet. Kann die Flotte den Befehl nicht befolgen liefert die
// Funktion FALSE zurück
BOOLEAN CFleet::CheckOrder(const CShip* ship, SHIP_ORDER::Typ nOrder) const
{
/*
	// Schiffsbefehle
	AVOID               0
	ATTACK              1
	CLOAK               2
	ATTACK_SYSTEM       3
	RAID_SYSTEM         4
	BLOCKADE_SYSTEM		5
	DESTROY_SHIP        6
	COLONIZE            7
	TERRAFORM           8
	BUILD_OUTPOST       9
	BUILD_STARBASE		10
	ASSIGN_FLAGSHIP     11
	CREATE_FLEET        12
	TRANSPORT			13
	FOLLOW_SHIP			14
	TRAIN_SHIP			15
	WAIT_SHIP_ORDER		16
	SENTRY_SHIP_ORDER	17
	REPAIR				18
*/
	// AVOID, ATTACK, RAID_SYSTEM, DESTROY_SHIP, CREATE_FLEET, FOLLOW_SHIP, TRAIN_SHIP, WAIT_SHIP_ORDER, SENTRY_SHIP_ORDER, REPAIR
	// können wenn möglich immer von jedem Schiff ausgeführt werden. Deshalb können wir hier immer
	// ein TRUE zurückgeben bzw. es ist erst gar nicht nötig die "CheckOrder" Funktion aufzurufen
	if (nOrder == SHIP_ORDER::AVOID || nOrder == SHIP_ORDER::ATTACK || nOrder == SHIP_ORDER::RAID_SYSTEM || nOrder == SHIP_ORDER::DESTROY_SHIP
		|| nOrder == SHIP_ORDER::CREATE_FLEET || nOrder == SHIP_ORDER::FOLLOW_SHIP || nOrder == SHIP_ORDER::TRAIN_SHIP
		|| nOrder == SHIP_ORDER::WAIT_SHIP_ORDER || nOrder == SHIP_ORDER::SENTRY_SHIP_ORDER
		|| nOrder == SHIP_ORDER::REPAIR)
		return TRUE;

	// ASSIGN_FLAGSHIP und TRANSPORT können nicht als Befehl an eine Flotte gegeben werden, daher wird hier
	// ein FALSE zurückgegeben
	else if (nOrder == SHIP_ORDER::ASSIGN_FLAGSHIP || nOrder == SHIP_ORDER::TRANSPORT)
		return FALSE;

	// bei den restlichen Befehlen müssen wir einige Berechnungen anstellen.
	else
	{
		// Tarnen können wir die gesamte Flotte nur, wenn in ihr auch nur Schiffe vorkommen, die die Tarn-
		// fähigkeit besitzen
		if (nOrder == SHIP_ORDER::CLOAK)
		{
			if (ship->GetStealthPower() < 4)
				return FALSE;
			for (int i = 0; i < m_vShips.GetSize(); i++)
			{
				if (m_vShips.GetAt(i).GetStealthPower() < 4)
					return FALSE;
			}
			// Haben wir bis jetzt kein FALSE zurückgegeben kann sich jedes Schiff in der Flotte tarnen
			// und wir können ein TRUE zurückgeben
			return TRUE;
		}
		// Wenn der Befehl kolonisieren oder terraformen lautet kann das die gesamte Flotte nur, wenn jedes
		// Schiff in der Flotte "ColonizePoints" beitzt
		else if (nOrder == SHIP_ORDER::COLONIZE || nOrder == SHIP_ORDER::TERRAFORM)
		{
			if (ship->GetColonizePoints() < 1)
				return FALSE;
			for (int i = 0; i < m_vShips.GetSize(); i++)
			{
				if (m_vShips.GetAt(i).GetColonizePoints() < 1)
					return FALSE;
			}
			// Haben wir bis jetzt kein FALSE zurückgegeben besitzt jedes Schiff in der Flotte "ColonizePoints"
			// und wir können ein TRUE zurückgeben
			return TRUE;
		}
		// Wenn der Befehl Außenposten oder Sternbasis bauen lautet kann das die gesamte Flotte nur, wenn jedes
		// Schiff in der Flotte "StationBuildPoints" beitzt
		else if (nOrder == SHIP_ORDER::BUILD_OUTPOST || nOrder == SHIP_ORDER::BUILD_STARBASE)
		{
			if (ship->GetStationBuildPoints() < 1)
				return FALSE;
			for (int i = 0; i < m_vShips.GetSize(); i++)
			{
				if (m_vShips.GetAt(i).GetStationBuildPoints() < 1)
					return FALSE;
			}
			// Haben wir bis jetzt kein FALSE zurückgegeben besitzt jedes Schiff in der Flotte "StationBuildPoints"
			// und wir können ein TRUE zurückgeben
			return TRUE;
		}
		// Bei einem Blockadebefehl müssen alle Schiffe in der Flotte die Eigenschaft "Blockadeschiff" besitzen
		else if (nOrder == SHIP_ORDER::BLOCKADE_SYSTEM)
		{
			if (!ship->HasSpecial(SHIP_SPECIAL::BLOCKADESHIP))
				return FALSE;
			for (int i = 0; i < m_vShips.GetSize(); i++)
			{
				if (!m_vShips.GetAt(i).HasSpecial(SHIP_SPECIAL::BLOCKADESHIP))
					return FALSE;
			}
			return TRUE;
		}
		// Bei einem Systemangriff müssen alle Schiffe in der Flotte ungetarnt sein
		else if (nOrder == SHIP_ORDER::ATTACK_SYSTEM)
		{
			if (ship->GetCloak())
				return FALSE;
			for (int i = 0; i < m_vShips.GetSize(); i++)
			{
				if (m_vShips.GetAt(i).GetCloak())
					return FALSE;
			}
			return TRUE;
		}
	}
	return FALSE;
}

bool CFleet::AllOnTactic(COMBAT_TACTIC::Typ tactic) const {
	const unsigned size = m_vShips.GetSize();
	for(unsigned i = 0; i < size; ++i)
	{
		if(m_vShips.ElementAt(i).GetCombatTactic() != tactic)
			return false;
	}
	return true;
}

BOOL CFleet::IsEmpty() const {
	return m_vShips.empty();
}

const CShipArray& CFleet::ShipArray() const {
	return m_vShips;
}

CShip CFleet::MakeFirstShipTheLeadingShipFleet() {
	// erstes Schiff aus der Flotte holen
	CShip& new_fleet_ship = m_vShips.GetAt(0);
	// für dieses eine Flotte erstellen
	new_fleet_ship.CreateFleet();
	CShipArray::iterator it = ++m_vShips.begin();
	for(; it != m_vShips.end(); ++it)
		new_fleet_ship.AddShipToFleet(*it);
	return new_fleet_ship;
}