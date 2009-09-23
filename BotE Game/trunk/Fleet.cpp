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
	for (int i = 0; i < m_SP.GetSize(); )
		m_SP.RemoveAt(i);
	m_SP.RemoveAll();	
}

CFleet::~CFleet()
{
	for (int i = 0; i < m_SP.GetSize(); )
		m_SP.RemoveAt(i);
	m_SP.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CFleet::CFleet(const CFleet & rhs)
{
	for (int i = 0; i < rhs.m_SP.GetSize(); i++)
		m_SP.Add(rhs.m_SP.GetAt(i));
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CFleet & CFleet::operator=(const CFleet & rhs)
{
	if (this == &rhs)
		return *this;
	for (int i = 0; i < rhs.m_SP.GetSize(); i++)
		m_SP.Add(rhs.m_SP.GetAt(i));
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CFleet::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_SP.GetSize();
		for (int i = 0; i < m_SP.GetSize(); i++)
			m_SP.GetAt(i).Serialize(ar);		
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		int number = 0;
		ar >> number;
		m_SP.RemoveAll();
		m_SP.SetSize(number);
		for (int i = 0; i < number; i++)
			m_SP.GetAt(i).Serialize(ar);
	}
}

//////////////////////////////////////////////////////////////////////
// Funktionen der Klasse CFleet
//////////////////////////////////////////////////////////////////////

// Funktion um ein Schiff aus der Flotte zu entfernen. Das n-te Schiff in der Flotte wird entfernt und
// von der Funktion zurückgegeben
CShip CFleet::RemoveShipFromFleet(int n)
{
	if (m_SP.GetSize() <= n)
	{
		MYTRACE(MT::LEVEL_ERROR, "CFleet::RemoveShipFromFleet(): Kann nicht auf Schiffselement zugreifen, da Feldindex nicht exisitiert!");
		exit(1);
	}
	CShip tempShip = m_SP.GetAt(n);
	m_SP.RemoveAt(n);
	return tempShip;
}

// Funktion berechnet die Geschwindigkeit der Flotte. Der Parameter der hier übergeben werden sollte
// ist der this-Zeiger des Schiffsobjektes, welches die Flotte besitzt
BYTE CFleet::GetFleetSpeed(CShip* ship)
{
	BYTE speed = 127;
	if (ship != 0)
		speed = ship->GetSpeed();
	for (int i = 0; i < m_SP.GetSize(); i++)
		if (m_SP.GetAt(i).GetSpeed() < speed)
			speed = m_SP.GetAt(i).GetSpeed();
	if (speed == 127)
		speed = 0;
	return speed;
}

// Funktion berechnet die Reichweite der Flotte. Der Parameter der hier übergeben werden sollte
// ist der this-Zeiger des Schiffsobjektes, welches die Flotte besitzt
BYTE CFleet::GetFleetRange(CShip* ship)
{
	BYTE range = 127;
	if (ship != 0)
		range = ship->GetRange();
	for (int i = 0; i < m_SP.GetSize(); i++)
		if (m_SP.GetAt(i).GetRange() < range)
			range = m_SP.GetAt(i).GetRange();
	if (range == 127)
		range = 0;
	return range;
}

// Funktion berechnet den Schiffstyp der Flotte. Wenn hier nur der selbe Schiffstyp in der Flotte vorkommt,
// dann gibt die Funktion diesen Schiffstyp zurück. Wenn verschiedene Schiffstypen in der Flotte vorkommen,
// dann liefert und die Funktion ein -1. Der Parameter der hier übergeben werden sollte ist der this-Zeiger
// des Schiffsobjektes, welches die Flotte besitzt
short CFleet::GetFleetShipType(CShip* ship)
{
	short type = ship->GetShipType();	
	for (int i = 0; i < m_SP.GetSize(); i++)
		if (m_SP.GetAt(i).GetShipType() != type)
			return -1;
	return type;
}

// Funktion übernimmt die Befehle des hier als Zeiger übergebenen Schiffsobjektes an alle Mitglieder der Flotte
void CFleet::AdoptCurrentOrders(CShip* ship)
{
	for (int i = 0; i < m_SP.GetSize(); i++)
	{
		m_SP.ElementAt(i).SetTerraformingPlanet(ship->GetTerraformingPlanet());
		if (ship->GetCurrentOrder() != ASSIGN_FLAGSHIP && ship->GetCurrentOrder() != TRANSPORT)
			m_SP.ElementAt(i).SetCurrentOrder(ship->GetCurrentOrder());
		m_SP.ElementAt(i).SetKO(ship->GetKO());
		m_SP.ElementAt(i).SetTargetKO(ship->GetTargetKO(),0);
	}
}

// Diese Funktion liefert TRUE wenn die Flotte den "order" ausführen kann. Als Schiffszeiger muß das Schiff
// übergeben werden, welches die Flotte beinhaltet. Kann die Flotte den Befehl nicht befolgen liefert die
// Funktion FALSE zurück
BOOLEAN CFleet::CheckOrder(CShip* ship, BYTE order)
{
/*
	// Schiffsbefehle
	#define AVOID               0
	#define ATTACK              1
	#define CLOAK               2
	#define ATTACK_SYSTEM       3
	#define RAID_SYSTEM         4
	#define BLOCKADE_SYSTEM		5
	#define DESTROY_SHIP        6
	#define COLONIZE            7
	#define TERRAFORM           8
	#define BUILD_OUTPOST       9
	#define BUILD_STARBASE		10
	#define ASSIGN_FLAGSHIP     11
	#define CREATE_FLEET        12
	#define TRANSPORT			13
	#define FOLLOW_SHIP			14
	#define TRAIN_SHIP			15
*/
	// AVOID, ATTACK, RAID_SYSTEM, DESTROY_SHIP, CREATE_FLEET, FOLLOW_SHIP, TRAIN_SHIP
	// können wenn möglich immer von jedem Schiff ausgeführt werden. Deshalb können wir hier immer
	// ein TRUE zurückgeben bzw. es ist erst gar nicht nötig die "CheckOrder" Funktion aufzurufen
	if (order == AVOID || order == ATTACK || order == RAID_SYSTEM || order == DESTROY_SHIP
		|| order == CREATE_FLEET || order == FOLLOW_SHIP || order == TRAIN_SHIP)
		return TRUE;

	// ASSIGN_FLAGSHIP und TRANSPORT können nicht als Befehl an eine Flotte gegeben werden, daher wird hier
	// ein FALSE zurückgegeben
	else if (order == ASSIGN_FLAGSHIP || order == TRANSPORT)
		return FALSE;

	// bei den restlichen Befehlen müssen wir einige Berechnungen anstellen.
	else
	{
		// Tarnen können wir die gesamte Flotte nur, wenn in ihr auch nur Schiffe vorkommen, die die Tarn-
		// fähigkeit besitzen
		if (order == CLOAK)
		{
			if (ship->GetStealthPower() < 4)
				return FALSE;
			for (int i = 0; i < m_SP.GetSize(); i++)
			{
				if (m_SP.GetAt(i).GetStealthPower() < 4)
					return FALSE;
			}
			// Haben wir bis jetzt kein FALSE zurückgegeben kann sich jedes Schiff in der Flotte tarnen
			// und wir können ein TRUE zurückgeben
			return TRUE;
		}
		// Wenn der Befehl kolonisieren oder terraformen lautet kann das die gesamte Flotte nur, wenn jedes 
		// Schiff in der Flotte "ColonizePoints" beitzt
		else if (order == COLONIZE || order == TERRAFORM)
		{
			if (ship->GetColonizePoints() < 1)
				return FALSE;
			for (int i = 0; i < m_SP.GetSize(); i++)
			{
				if (m_SP.GetAt(i).GetColonizePoints() < 1)
					return FALSE;
			}
			// Haben wir bis jetzt kein FALSE zurückgegeben besitzt jedes Schiff in der Flotte "ColonizePoints"
			// und wir können ein TRUE zurückgeben
			return TRUE;
		}
		// Wenn der Befehl Außenposten oder Sternbasis bauen lautet kann das die gesamte Flotte nur, wenn jedes 
		// Schiff in der Flotte "StationBuildPoints" beitzt
		else if (order == BUILD_OUTPOST || order == BUILD_STARBASE)
		{
			if (ship->GetStationBuildPoints() < 1)
				return FALSE;
			for (int i = 0; i < m_SP.GetSize(); i++)
			{
				if (m_SP.GetAt(i).GetStationBuildPoints() < 1)
					return FALSE;
			}
			// Haben wir bis jetzt kein FALSE zurückgegeben besitzt jedes Schiff in der Flotte "StationBuildPoints"
			// und wir können ein TRUE zurückgeben
			return TRUE;
		}
		// Bei einem Blockadebefehl müssen alle Schiffe in der Flotte die Eigenschaft "Blockadeschiff" besitzen
		else if (order == BLOCKADE_SYSTEM)
		{
			if (ship->HasSpecial(BLOCKADESHIP) == FALSE)
				return FALSE;
			for (int i = 0; i < m_SP.GetSize(); i++)
			{
				if (m_SP.GetAt(i).HasSpecial(BLOCKADESHIP) == FALSE)
					return FALSE;
			}
			return TRUE;
		}
		// Bei einem Systemangriff müssen alle Schiffe in der Flotte ungetarnt sein
		else if (order == ATTACK_SYSTEM)
		{
			if (ship->GetCloak())
				return FALSE;
			for (int i = 0; i < m_SP.GetSize(); i++)
			{
				if (m_SP.GetAt(i).GetCloak())
					return FALSE;
			}
			return TRUE;
		}
	}
	return FALSE;
}

// Funktion löscht die gesamte Flotte
void CFleet::DeleteFleet()
{
	for (int i = 0; i < m_SP.GetSize(); )
		m_SP.RemoveAt(i);
	m_SP.RemoveAll();
}
