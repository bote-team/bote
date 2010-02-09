/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "afxtempl.h"

class CGlobalBuildings : public CObject
{
public:
	DECLARE_SERIAL (CGlobalBuildings)
	// Standardkonstruktor
	CGlobalBuildings(void);

	// Destruktor
	~CGlobalBuildings(void);

	// Kopierkonstruktor
	CGlobalBuildings(const CGlobalBuildings & rhs);
	
	// Zuweisungsoperatur
	CGlobalBuildings & operator=(const CGlobalBuildings &);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen
// zum Lesen der Membervariablen
	// Funktion gibt einen Zeiger auf das Feld aller globalen Gebäude zurück
	CArray<USHORT,USHORT>* GetGlobalBuildings() {return &m_GlobalBuildings;}

// zum Schreiben der Membervariablen
	void AddGlobalBuilding(USHORT id) {m_GlobalBuildings.Add(id);}

// sonstige Funktionen
	// Funktion löscht ein globales Gebäude aus dem Feld der globalen Gebäude. Diese Funktion sollte aufgerufen
	// werden, wenn wir ein solches Gebäude abreißen oder verlieren.
	void DeleteGlobalBuilding(USHORT id);

	// Resetfunktion für die Klasse CGlobalBuildings
	void Reset();

private:
	// Dieses Feld beinhaltet die Gebäude, welche ein imperienweites Attribut haben und in einer Bauliste stehen, sowei
	// alle Gebäude die in allen Systemen stehen. Kommen mehrere gleiche Gebäude vor, so ist deren ID auch mehrmals
	// hier in dem Feld vorhanden.
	CArray<USHORT,USHORT> m_GlobalBuildings;
};
