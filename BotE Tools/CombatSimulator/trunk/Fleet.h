/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// Fleet.h: Schnittstelle für die Klasse CFleet.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLEET_H__EBC5B38C_84F8_4C71_B3CE_03B4ECFA1D1E__INCLUDED_)
#define AFX_FLEET_H__EBC5B38C_84F8_4C71_B3CE_03B4ECFA1D1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Ship.h"

class CFleet : public CObject  
{
	friend class CShip;
public:
	DECLARE_SERIAL (CFleet)
	// Standardkonstruktor
	CFleet();
	
	// Destruktor
	virtual ~CFleet();
	
	// Kopierkonstruktor
	CFleet(const CFleet & rhs);
	
	// Zuweisungsoperatur
	CFleet & operator=(const CFleet &);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Funktion gibt ein einzelnes Schiff aus der Flotte zurück
	CShip GetShipFromFleet(int n) {return m_SP.ElementAt(n);}

	// Funktion gibt einen Zeiger auf ein Schiff aus der Flotte zurück
	CShip* GetPointerOfShipFromFleet(int n) {return &m_SP.ElementAt(n);}
	
	// Funktion um ein Schiff zur Flotte hinzuzufügen
	void AddShipToFleet(CShip ship) {m_SP.Add(ship);}
	
	// Funktion um ein Schiff aus der Flotte zu entfernen. Das n-te Schiff in der Flotte wird entfernt und
	// von der Funktion zurückgegeben
	CShip RemoveShipFromFleet(int n);
	
	// Funktion liefert die Anzahl der Schiffe in der Flotte
	USHORT GetFleetSize() {return (USHORT)m_SP.GetSize();}
	
	// Funktion berechnet die Geschwindigkeit der Flotte. Der Parameter der hier übergeben werden sollte
	// ist der this-Zeiger bzw. die Adresse des Schiffsobjektes, welches die Flotte besitzt
	BYTE GetFleetSpeed(CShip* ship = 0);
	
	// Funktion berechnet die Reichweite der Flotte. Der Parameter der hier übergeben werden sollte
	// ist der this-Zeiger bzw. die Adresse des Schiffsobjektes, welches die Flotte besitzt
	BYTE GetFleetRange(CShip* ship = 0);
	
	// Funktion berechnet den Schiffstyp der Flotte. Wenn hier nur der selbe Schiffstyp in der Flotte vorkommt,
	// dann gibt die Funktion diesen Schiffstyp zurück. Wenn verschiedene Schiffstypen in der Flotte vorkommen,
	// dann liefert und die Funktion ein -1. Der Parameter der hier übergeben werden sollte ist der this-Zeiger
	// des Schiffsobjektes, welches die Flotte besitzt
	short GetFleetShipType(CShip* ship);
	
	// Funktion übernimmt die Befehle des hier als Zeiger übergebenen Schiffsobjektes an alle Mitglieder der Flotte
	void AdoptCurrentOrders(CShip* ship);

	// Diese Funktion liefert TRUE wenn die Flotte den "order" ausführen kann. Als Schiffszeiger muß das Schiff
	// übergeben werden, welches die Flotte beinhaltet. Kann die Flotte den Befehl nicht befolgen liefert die
	// Funktion FALSE zurück
	BOOLEAN CheckOrder(CShip* ship, BYTE order);
	
	// Funktion löscht die gesamte Flotte
	void DeleteFleet();

private:
	ShipArray m_SP;		// Die Schiffe in der Flotte, außer Schiff selbst, welches das Flottenobjekt besitzt
};

#endif // !defined(AFX_FLEET_H__EBC5B38C_84F8_4C71_B3CE_03B4ECFA1D1E__INCLUDED_)
