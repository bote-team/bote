/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
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
#include "boost/shared_ptr.hpp"

#define	RACESOURCE_DATAFILE		0		///< Rassendaten werden aus *.data Dateien gelesen
#define RACESOURCE_CONST		1		///< Rassendaten kommen hart aus dem Quellcode

typedef const boost::shared_ptr<const CRace> ConstRacePtr;
typedef boost::shared_ptr<CRace> RacePtr;

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


	typedef std::map<CString, boost::shared_ptr<CRace>>::const_iterator const_iterator;
	typedef std::map<CString, boost::shared_ptr<CRace>>::iterator iterator;

	const_iterator begin() const { return m_mRaces.begin(); }
	iterator begin() { return m_mRaces.begin(); }

	const_iterator end() const { return m_mRaces.end(); }
	iterator end() { return m_mRaces.end(); }

	unsigned size() const { return m_mRaces.size(); }

	const_iterator find(const CString& id) const { return m_mRaces.find(id); }
	iterator find(const CString& id) { return m_mRaces.find(id); }

	// Funktionen
	/// Funktion zum Einlesen und Initialisieren aller am Spiel beteiligten Rassen
	/// @param nSource Datenquelle der Rasseninformationen
	/// @return <code>true</code> wenn Initalisierung erfolgreich war, sonst <code>false</code>
	bool Init(int nSource = RACESOURCE_DATAFILE);

	/// Funktion gibt eine bestimmte Rasse zurück.
	/// @param sID ID der gewünschten Rassen
	/// @return Zeiger auf gewünschte Rasse
	const CRace* GetRace(const CString& sID) const;
	CRace* GetRace(const CString& sID);

	const boost::shared_ptr<const CRace> GetRaceSafe(const CString& sID) const;
	boost::shared_ptr<CRace> GetRaceSafe(const CString& sID);

	const boost::shared_ptr<const CMajor> GetMajorSafe(const CString& sID) const;
	boost::shared_ptr<CMajor> GetMajorSafe(const CString& sID);

	/// Funktion gibt alle Hauptrassen des Spiels zurück.
	/// @return Map mit allen Hauptrassen des Spiels
	map<CString, CMajor*>* GetMajors(void) {return &m_mMajors;}

	/// Funktion gibt alle Minorrassen des Spiels zurück.
	/// @return Map mit allen Minorrassen des Spiels
	map<CString, CMinor*>* GetMinors(void) {return &m_mMinors;}

	/// Funktion liefert die Minorrace, welche in einem bestimmten Sektor beheimatet ist.
	/// @param sMinorsHome Heimatsystem
	/// @return Zeiger auf Minorrace (<code>NULL</code> wenn die Rasse nicht gefunden werden konnte)
	CMinor* GetMinorRace(const CString& sMinorsHome) const;

	/// Funktion entfernt eine nicht mehr zu benutzende Rasse.
	/// @param sRaceID Rassen-ID
	void RemoveRace(const CString& sRaceID);

	/// Funktion zum zurücksetzen aller Werte auf Ausgangswerte.
private:

	const_iterator RemoveRaceInternal(const const_iterator& it);

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
	std::map<CString, boost::shared_ptr<CRace>> m_mRaces; ///< Map mit allen Rassen des Spiels

	map<CString, CMajor*>		m_mMajors;			///< Map mit allen Majors des Spiels (muss nicht serialisiert werden)

	map<CString, CMinor*>		m_mMinors;			///< Map mit allen Majors des Spiels (muss nicht serialisiert werden)
};
