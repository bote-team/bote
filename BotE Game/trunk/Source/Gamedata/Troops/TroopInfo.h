/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "troop.h"

class CTroopInfo :	public CTroop
{
public:
	/// Konstruktor
	CTroopInfo(void);

	/// Konstruktor mit kompletter Parameterübergabe
	CTroopInfo(CString name, CString desc, BYTE power, BYTE costs, BYTE techs[6], USHORT res[5], USHORT ip,
		BYTE ID, const CString& sOwner, USHORT size, BYTE moralValue);
	
	/// Destruktor
	~CTroopInfo(void);

	// Zugriffsfunktionen zum Lesen der Membervariablen
	/**
	 * Diese Funktion gibt den Namen der Truppe zurück.
	 */
	const CString& GetName() const {return m_strName;}

	/**
	 * Dieser Funktion gibt die Beschreibung der Truppe zurück.
	 */
	const CString& GetDescription() const {return m_strDescription;}

	/**
	 * Diese Funktion gibt die Unterhaltskosten der Truppe zurück.
	 */
	BYTE GetMaintenanceCosts() const {return m_byMaintenanceCosts;}

	/**
	 * Diese Funktion gibt einen Zeiger auf die benötigten Techlevel dieser Einheit zurück.
	 */
	BYTE* GetNeededTechlevels() {return m_byNeededTechs;}

	/**
	 * Diese Funktion gibt ein benötigtes Techlevel zurück, welches man mit dem Paramter <code>tech</code>
	 * erfragt hat.
	 */
	BYTE GetNeededTechlevel(BYTE tech) const {return m_byNeededTechs[tech];}

	/**
	 * Diese Funktion gibt einen Zeiger auf die benötigten Ressourcen dieser Truppe zurück
	 */
	USHORT* GetNeededResources() {return m_iNeededResources;}

	/**
	 * Diese Funktion gibt die benötigte Industrie zum Bau dieser Einheit zurück.
	 */
	USHORT GetNeededIndustry() const {return m_iNeededIndustry;}

	/**
	 * Diese Funktion gibt die Truppengröße, somit den benötigten Platz bei einem Transport zurück.
	 */
	USHORT GetSize() const {return m_iSize;}

	/**
	 * Diese Funktion gibt den Moralwert dieser Truppe zurück.
	 */
	BYTE GetMoralValue() const {return m_byMoralValue;}
	
	// Zugriffsfunktionen zum Schreiben der Membervariablen
	/**
	 * Diese Funktion setzt den Namen der Einheit.
	 */
	void SetName(const CString& name) {m_strName = name;}

	/**
	 * Diese Funktion setzt die Beschreibung der Truppe.
	 */
	void SetDescription(const CString& desc) {m_strDescription = desc;}

	/**
	 * Diese Funktion setzt die zum Bau benötigte Industrie der Truppe.
	 */
	void SetNeededIndustry(USHORT ip) {m_iNeededIndustry = ip;}

	/// Funktion gibt zurück, ob die Truppe mit der aktuellen Forschung einer Rasse baubar ist.
	/// @param researchLevels Forschungsstufen der Rasse
	/// @return Wahrheitswert
	bool IsThisTroopBuildableNow(const BYTE reserachLevels[6]) const;

private:
	/// Der Name der Einheit
	CString m_strName;

	/// Die Beschreibung der Einheit
	CString m_strDescription;

	/// Diese Variable beinhaltet die Unterhaltskosten der Einheit
	BYTE m_byMaintenanceCosts;

	/// Dieses Feld beinhaltet die benötigten Forschungslevel zum Bau dieser Einheit
	BYTE m_byNeededTechs[6];

	/// Dieses Feld beinhaltet die benötigten Ressourcen zum Bau dieser Einheit
	USHORT m_iNeededResources[5];

	/// Die benötigte Industrie um eine Einheit fertigzustellen
	USHORT m_iNeededIndustry;

	/// Die Anzahl der Soldaten bzw. der benötigte Platz bei einem Transport für diese Einheit
	USHORT m_iSize;

	/// Der Moralwert der Truppe. Um dieses Wert wird die Moral bei unter 100% im System erhöht bzw.
	/// bei über 100% in dem System verringert
	BYTE m_byMoralValue;
};
