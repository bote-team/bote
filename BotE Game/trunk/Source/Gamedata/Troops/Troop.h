/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "Options.h"

class CTroop : public CObject
{
public:
	DECLARE_SERIAL (CTroop)
	/// Konstruktor
	CTroop(void);

	/// Destruktor
	~CTroop(void);

	/// Kopierkonstruktor
	CTroop(const CTroop & rhs);

	/// Zuweisungsoperator
	CTroop & operator=(const CTroop &);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen zum Lesen der Membervariablen
	/**
	 * Diese Funktion gibt die ID der Truppe zurück. Mir dieser kann man aus der CTroopInfo Klasse weitere Informationen
	 * zu dieser Einheit abrufen.
	 */
	BYTE GetID() const {return m_byID;}

	/**
	 * Diese Funktion gibt den Besitzer dieser Truppe zurück.
	 */
	const CString& GetOwner(void) const {return m_sOwner;}

	/**
	 * Diese Funktion gibt die Angriffsstärke der Truppe zurück.
	 */
	BYTE GetOffense() const {return m_byOffense;}

	/**
	 * Diese Funktion gibt die Verteidigungsstärke der Truppe zurück.
	 */
	BYTE GetDefense() const {return m_byDefense;}



	/**
	 * Diese Funktion gibt die derzeitige Erfahrung der Einheit zurück.
	 */
	USHORT GetExperiance() const {return m_iExperiance;}

	// Zugriffsfunktionen zum Schreiben der Membervariablen
	/**
	 * Diese Funktion setzt die Angriffsstärke der Truppe.
	 */
	void SetOffense(BYTE power) {m_byOffense = power;}


	/**
	 * Diese Funktion setzt die Verteidigungsstärke der Truppe.
	 */
	void SetDefense(BYTE power) {m_byDefense = power;}

	/**
	 * Diese Funktion addiert <code>add</code> zur derzeitigen Erfahrung der Truppe.
	 */
	void AddExperiancePoints(USHORT add) {m_iExperiance += add;}

	// sonstige Funktionen
	/**
	 * Diese Funktion generiert eine Truppe. Sie erwartet als Paramter einen Zeiger auf eine andere Truppe
	 * <code>troop</code>, aus deren Eigenschaften die identische neue Einheit generiert wird.
	 */
	void GenerateTroop(const CTroop* troop);

	/**
	 * Mittels dieser Funktion wird eine andere Truppe, welche hier als Zeiger als Parameter <code>enemy</code>
	 * übergeben werden muss, angegriffen. Es kann ein zusätzlicher Angriffsbonu <code>offenceBoni</code> übergeben
	 * werden. Außerdem wird der Verteidigungsbonus in dem System, in welchem die Truppe stationiert ist im
	 * Parameter <code>defenceBoni</code> übergeben. Wenn die Funktion eine 0 zurückgibt, dann hat diese Einheit gewonnen,
	 * wenn sie eine 1 zurückgibt, dann hat die übergebene Einheit gewonnen, wenn sie eine 2 zurückgibt, dann haben sich
	 * beide Einheiten gegenseitig zerstört.
	 */
	BYTE Attack(CTroop* enemy, BYTE offenceBoni, short defenceBoni);

protected:
	/// Mittels der ID kann man die entsprechenden Informationen aus der zugehörigen Infoklasse abfragen
	BYTE m_byID;

	/// Welcher Rasse gehört die Truppe
	CString m_sOwner;

	/// Diese Variable gibt die Offensiv-Stärke der Truppe an
	BYTE m_byOffense;

	/// Diese Variable gibt die Defensiv-Stärke der Truppe an
	BYTE m_byDefense;

	/// Die Erfahrung der Truppe
	USHORT m_iExperiance;
};
