/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "Network.h"
#include "Race.h"
#include "Major.h"
#include "Minor.h"

#define	RACESOURCE_DATAFILE		0		///< Rassendaten werden aus *.data Dateien gelesen
#define RACESOURCE_CONST		1		///< Rassendaten kommen hart aus dem Quellcode

/// Klasse zum Verwalten aller Rassen im Spiel.
class CRaceController :	public CObject
{

	DECLARE_SERIAL(CRaceController)

public:
	/// Standardkonstruktor
	CRaceController(void);
	/// Standarddestruktor
	~CRaceController(void);
	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Funktionen
	/// Funktion zum Einlesen und Initialisieren aller am Spiel beteiligten Rassen
	/// @param nSource Datenquelle der Rasseninformationen
	/// @return <code>true</code> wenn Initalisierung erfolgreich war, sonst <code>false</code>
	bool Init(int nSource = RACESOURCE_DATAFILE);

	/// Funktion gibt die Map mit allen Rassen des Spiels zurück.
	/// @return Map mit allen Rassen des Spiels
	map<CString, CRace*>* GetRaces(void) {return &m_mRaces;}

	/// Funktion gibt eine bestimmte Rasse zurück.
	/// @param sID ID der gewünschten Rassen
	/// @return Zeiger auf gewünschte Rasse
	CRace* GetRace(const CString& sID) const;

	/// Funktion gibt alle Hauptrassen des Spiels zurück.
	/// @return Map mit allen Hauptrassen des Spiels
	map<CString, CMajor*>* GetMajors(void) {return &m_mMajors;}

	/// Funktion gibt alle Minorrassen des Spiels zurück.
	/// @return Map mit allen Minorrassen des Spiels
	map<CString, CMinor*>* GetMinors(void) {return &m_mMinors;}

	/// Funktion gibt alle Rassen eines bestimmten Types zurück.
	/// @param type Typ der Rasse (MAJOR, MINOR)
	/// @return Map aller Rassen eines bestimmten Types
	map<CString, CRace*> GetRaces(CRace::RACE_TYPE type) const;

	/// Funktion liefert die Minorrace, welche in einem bestimmten Sektor beheimatet ist.
	/// @param sMinorsHome Heimatsystem
	/// @return Zeiger auf Minorrace (<code>NULL</code> wenn die Rasse nicht gefunden werden konnte)
	CMinor* GetMinorRace(const CString& sMinorsHome) const;

	/// Funktion entfernt eine nicht mehr zu benutzende Rasse.
	/// @param sRaceID Rassen-ID
	void RemoveRace(const CString& sRaceID);

	/// Funktion zum zurücksetzen aller Werte auf Ausgangswerte.
	void Reset(void);

	/// Funktion gibt die zu einer Majorrace zugehörige Netzwerkclient-ID zurück.
	/// Maximal können sechs Clients bestehen. Jede Majorrace benötigt eine eindeutige
	/// Client-ID.
	/// @param sRaceID Rassen-ID einer Majorrace
	/// @return Netzwerk-Client-ID
	/// ALPHA5 -> noch fest!
	network::RACE GetMappedClientID(const CString& sRaceID) const;

	/// Funktion gibt die zu einer Client-ID zugehörige Major-ID zurück.
	/// Maximal können sechs Clients bestehen. Jede Majorrace benötigt eine eindeutige
	/// Client-ID.
	/// @param client-ID Client-ID eines Spielers
	/// @return Rassen-ID
	/// ALPHA5 -> noch fest!
	CString GetMappedRaceID(network::RACE clientID) const;

private:
	// private Funktionen
	/// Funktion zum Einlesen und Initialisieren der beteiligten Majors
	/// @param nSource Datenquelle der Rasseninformationen
	/// @return <code>true</code> wenn Initalisierung erfolgreich war, sonst <code>false</code>
	bool InitMajors(int nSource = RACESOURCE_DATAFILE);

	/// Funktion zum Einlesen und Initialisieren der beteiligten Minors
	/// @param nSource Datenquelle der Rasseninformationen
	/// @return <code>true</code> wenn Initalisierung erfolgreich war, sonst <code>false</code>
	bool InitMinors(int nSource = RACESOURCE_DATAFILE);

	/// Funktion zum Einlesen und Initialisieren der Aliens (Weltraummonster)
	/// @param nSource Datenquelle der Rasseninformationen
	/// @return <code>true</code> wenn Initalisierung erfolgreich war, sonst <code>false</code>
	bool InitAlienEntities(int nSource = RACESOURCE_DATAFILE);

	/// Funktion erstellt die Beziehungen der Rassen untereinander. Dabei werden jedoch nur die
	/// Majorbeziehungen in beide Richtungen vergeben. Bei den Minors wird immer nur die Beziehung
	/// von Minor zu Majors erstellt.
	void InitRelations(void);

	// Attribute
	map<CString, CRace*>		m_mRaces;			///< Map mit allen Rassen des Spiels

	map<CString, CMajor*>		m_mMajors;			///< Map mit allen Majors des Spiels (muss nicht serialisiert werden)

	map<CString, CMinor*>		m_mMinors;			///< Map mit allen Majors des Spiels (muss nicht serialisiert werden)
};
