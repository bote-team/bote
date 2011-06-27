/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include <map>
#include <list>

class CGlobalBuildings : public CObject
{
public:
	DECLARE_SERIAL (CGlobalBuildings)
	// Standardkonstruktor
	CGlobalBuildings(void);

	// Destruktor
	~CGlobalBuildings(void);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	/// Funktion gibt die Anzahl eines bestimmtes Gebäudes für eine bestimmte Major-Rasse zurück.
	/// @param sRaceID Rassen-ID
	/// @param nID Gebäude-ID
	/// @return Anzahl der Gebäude
	int GetCountGlobalBuilding(const CString& sRaceID, USHORT nID) const;
	
	// zum Schreiben der Membervariablen
	/// Funktion fügt ein Gebäude für eine bestimmte Rasse den globalen Gebäuden hinzu.
	/// @param sRaceID Rassen-ID
	/// @param nID Gebäude-ID
	void AddGlobalBuilding(const CString& sRaceID, USHORT nID) {m_mGlobalBuildings[sRaceID].push_back(nID);}

	// sonstige Funktionen
	/// Funktion löscht ein globales Gebäude aus dem Feld der globalen Gebäude. Diese Funktion sollte aufgerufen
	/// werden, wenn ein solches Gebäude abgerissen oder verloren wird.
	/// @param sRaceID Rassen-ID
	/// @param nID Gebäude-ID
	void DeleteGlobalBuilding(const CString& sRaceID, USHORT nID);

	// Resetfunktion für die Klasse CGlobalBuildings
	void Reset();

private:
	// Dieses Feld beinhaltet die Gebäude, welche ein imperienweites Attribut haben und in einer Bauliste stehen, sowei
	// alle Gebäude die in allen Systemen stehen. Kommen mehrere gleiche Gebäude vor, so ist deren ID auch mehrmals
	// hier in dem Feld vorhanden.
	std::map<CString, std::list<USHORT> > m_mGlobalBuildings;	
};
