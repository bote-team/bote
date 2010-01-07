/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "afx.h"
#include "ShipInfo.h"

struct BeamWeaponsObserverStruct
{
	CString WeaponName;
	BYTE maxLevel;
};

struct TupeWeaponsObserverStruct
{
	CString TupeName;
	BYTE number;
	BYTE fireRate;
	BOOLEAN onlyMicro;
	USHORT fireAngle;
};

class CWeaponObserver :	public CObject
{
public:
	DECLARE_SERIAL (CWeaponObserver)
	// Standardkonstruktor
	CWeaponObserver(void);

	// Destruktor
	virtual ~CWeaponObserver(void);

	// Kopierkonstruktor
	CWeaponObserver(const CWeaponObserver & rhs);
	
	// Zuweisungsoperatur
	CWeaponObserver & operator=(const CWeaponObserver &);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Funktion gibt uns den maximal baubaren Typ einer Beamwaffe zurück. Dafür übergeben wir der Funktion
	// den Namen der Beamwaffe. Ist der Rückgabewert NULL, so können wir den Typ nicht bauen
	BYTE GetMaxBeamType(CString NameOfBeamWeapon);

	// Funktion sucht einen weiteren baubaren Torpedo und übernimmt dafür den aktuell angebauten Torpedotyp und
	// den aktuell angebauten Torpedowerfeer
	BYTE GetNextTorpedo(BYTE currentTorpedoType, BOOLEAN onlyMircoTupe);
	
	// Funktion sucht einen weiteren Torpedowerfer, denn wir an das Schiff montieren können und übernimmt dafür
	// den Namen des aktuell angebauten Werfers und den aktuell angebauten Torpedotyp
	TupeWeaponsObserverStruct GetNextTupe(CString currentTupeName, BYTE currentTorpedoType);

	// Funktion gibt den maximal baubaren Schildtyp des Imperiums zurück
	BYTE GetMaxShieldType() {return m_MaxShieldLevel;}

	// Funktion checkt alle baubaren Beamwaffen und setzt alle Variablen auf den richtigen Wert.
	// Der Parameter <code>info</code> muß ein derzeit baubares Schiff des Imperium sein. Die Funktion
	// checkt gleichzeitig auch den max. Schildtyp.
	void CheckBeamWeapons(CShipInfo* info);

	// Funktion checkt alle Torpedowaffen (Torpedowerfer, Torpedoart) die wir bauen können und übernimmt dafür einen
	// Zeiger auf ein Schiffsinfoobjekt. Das Schiff muß zur gleichen Rasse gehören, zu der auch der auch das
	// Observerobjekt gehört. Der Parameter <code>info</code> muß ein derzeit baubares Schiff des Imperium sein.
	void CheckTorpedoWeapons(CShipInfo* info);

	// Resetfunktion für diese Klasse, welche alle Werte wieder auf den Ausgangswert setzt
	void Reset();

private:
	// dynamisches Feld speichert alle baubaren Beamwaffen eines Imperiums
	CArray<BeamWeaponsObserverStruct,BeamWeaponsObserverStruct> m_BeamWeapons;

	// dynamisches Feld speichert alle baubaren Torpedowerfer eines Imperiums
	CArray<TupeWeaponsObserverStruct,TupeWeaponsObserverStruct> m_TupeWeapons;
	
	// Feld speichert welche Torpedos wir an Schiffen anbringen können
	BOOLEAN m_BuildableTorpedos[DIFFERENT_TORPEDOS];

	// Der maximale Schildtyp, den wir derzeit bauen können
	BYTE m_MaxShieldLevel;
};
