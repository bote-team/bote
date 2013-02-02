/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "IntelAssignment.h"
#include "IntelReports.h"
#include "IntelInfo.h"
#include <map>

using namespace std;
/**
 * Diese Klasse abstrahiert den Geheimdienst einer Hauptrasse. Sie beinhaltet alle Funktionen zur Manipulation
 * von Geheimdienstwerten.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */
class CIntelligence : public CObject
{
public:
	DECLARE_SERIAL (CIntelligence)

	/// Standardkonstruktor
	CIntelligence(void);

	/// Destruktor
	~CIntelligence(void);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	/// Funktion gibt die Nummer der Rasse zurück, welche wir für unsere Geheimdienstaten verantworlich machen wollen.
	const CString& GetResponsibleRace() const {return m_sResponsibleRace;}

	/// Funktion gibt die Anzahl der produzierten Geheimdienstpunkte zurück.
	UINT GetSecurityPoints() const {return m_iSecurityPoints;}

	/// Funktion gibt die gesammelten Punkte für die innere Sicherheit zurück.
	UINT GetInnerSecurityStorage() const {return m_iInnerStorage;}

	/// Funktion gibt die angesammelten Punkte bei der Spionage oder der Sabotage bei einer bestimmten Rasse
	/// zurück.
	/// @param type Spionage == 0, Sabotage == 1
	/// @param sRace Rasse
	UINT GetSPStorage(BOOLEAN type, const CString& sRace) {return m_iSPStorage[type][sRace];}

	/// Funktion gibt einen Bonus auf ein Geheimdienstressort zurück.
	/// @param bonus Wirtschaft == 0, Forschung == 1, Militär == 2, Diplomatie == 3, innere Sicherheit == 4
	/// @param type Spionage == 0, Sabotage == 1
	short GetBonus(BYTE bonus, BOOLEAN type) const;

	/// Funktion gibt den Bonus der inneren Sicherheit zurück.
	short GetInnerSecurityBoni() const {return (m_nInnerSecurityBoni < -100) ? -100 : m_nInnerSecurityBoni;}

	/// Funktion gibt den Bonus für Wirtschaft zurück.
	/// @param type Spionage == 0, Sabotage == 1
	short GetEconomyBonus(BOOLEAN type) const {return (m_nEconomyBoni[type] < -100) ? -100 : m_nEconomyBoni[type];}

	/// Funktion gibt den Bonus für Wissenschaft zurück.
	/// @param type Spionage == 0, Sabotage == 1
	short GetScienceBonus(BOOLEAN type) const {return (m_nScienceBoni[type] < -100) ? -100 : m_nScienceBoni[type];}

	/// Funktion gibt den Bonus für Militär zurück.
	/// @param type Spionage == 0, Sabotage == 1
	short GetMilitaryBonus(BOOLEAN type) const {return (m_nMilitaryBoni[type] < -100) ? -100 : m_nMilitaryBoni[type];}

	/// Funktion gibt die Aggressivität bei Geheimdienstaktionen zurück.
	/// @param type Spionage == 0, Sabotage == 1
	/// @param sRace Rasse
	BYTE GetAggressiveness(BOOLEAN type, const CString& sRace) {return m_byAggressiveness[type][sRace];}

	/// Funktion gibt einen Zeiger auf die Geheimdienstreporte zurück.
	CIntelReports* GetIntelReports() {return &m_Reports;}

	/// Funktion gibt einen Zeiger auf die Geheimdienstinformationen eines Imperiums zurück.
	CIntelInfo* GetIntelInfo() {return &m_IntelInfo;}

	/// Funktion gibt einen Zeiger auf die Zuteilung der einzelnen Geheimdienstressorts zurück.
	CIntelAssignment* GetAssignment() {return &m_Assignment;}

	/// Funktion gibt einen Zeiger auf die Zuteilung der einzelnen Geheimdienstressorts zurück.
	CIntelAssignment* SetAssignment() {return &m_Assignment;}

	/// Funktion legt die Rassennummer fest.
	/// @param sRace Nummer der zugehörigen Rasse/Imperium
	void SetRaceID(const CString& sRace) {m_sRace = sRace; SetResponsibleRace(sRace);}

	/// Funktion legt die für unsere Geheimdiensttaten verantworlich machbare Rasse fest. Somit läßt sich die Sache
	/// auf eigentlich unbeteiligte Rassen abwälzen.
	/// @param responibleRace neue verantworlichmachbare Rasse
	void SetResponsibleRace(const CString& responsibleRace) {m_sResponsibleRace = responsibleRace;}

	/// Funktion setzt die Aggressivität bei Geheimdienstaktionen fest.
	/// @param type Spionage == 0, Sabotage == 1
	/// @param sRace Rasse
	/// @param value vorsichtig == 0, normal == 1, aggressiv = 2
	void SetAggressiveness(BOOLEAN type, const CString& sRace, BYTE value) {m_byAggressiveness[type][sRace] = value;}

	/// Funktion addiert einen Wert zum vorhandenen Wert der inneren Sicherheit. Funktion überprüft auch, ob der neue
	/// Wert im richtigen Bereich liegt.
	/// @param add der zu addierende Wert
	void AddInnerSecurityPoints(int add);

	/// Funktion addiert die übergebenen Punkte zum jeweiligen Lager einer Rasse. Gleichzeitig wird überprüft, dass
	/// ihr neuer Wert im richtigen Bereich liegt.
	/// @param type Spionage == 0, Sabotage == 1
	/// @param sRace Rasse
	/// @param add der zu addierende Wert
	void AddSPStoragePoints(BOOLEAN type, const CString& sRace, int add);

	/// Funktion fügt den im Parameter übergebenen Wert dem Bonus für die innere Sicherheit hinzu.
	/// @param add hinzuzufügender Bonus
	void AddInnerSecurityBonus(short add) {m_nInnerSecurityBoni += add;}

	/// Funktion fügt den im Parameter übergebenen Wert dem Bonus für die Wirtschaft hinzu.
	/// @param add hinzuzufügender Bonus
	/// @param type Spionage == 0, Sabotage == 1
	void AddEconomyBonus(short add, BOOLEAN type) {m_nEconomyBoni[type] += add;}

	/// Funktion fügt den im Parameter übergebenen Wert dem Bonus für die Wissenschaft hinzu.
	/// @param add hinzuzufügender Bonus
	/// @param type Spionage == 0, Sabotage == 1
	void AddScienceBonus(short add, BOOLEAN type) {m_nScienceBoni[type] += add;}

	/// Funktion fügt den im Parameter übergebenen Wert dem Bonus für die Militär hinzu.
	/// @param add hinzuzufügender Bonus
	/// @param type Spionage == 0, Sabotage == 1
	void AddMilitaryBonus(short add, BOOLEAN type) {m_nMilitaryBoni[type] += add;}

	// sonstige Funktionen
	/// Funktion löscht die produzierten Geheimdienstpunkte.
	void ClearSecurityPoints() {m_iSecurityPoints = 0;}

	/// Funktion löscht alle Geheimdienstboni.
	void ClearBoni();

	/// Funktion addiert den im Paramter übergebenen Wert zu den aktuell produzierten Geheimdienstpunkten.
	/// @param add Anzahl der addierten Geheimdienstpunkte
	void AddSecurityPoints(int add);

	/// Resetfunktion für das CIntelligence-Objekt.
	void Reset();

private:
	// Attribute
	CString m_sRace;					///< Rassen-ID des Imperiums zu welchem dieses Geheimdienstobjekt gehört

	CString m_sResponsibleRace;			///< Rasse welche für unsere Geheimdiensttaten verantwortlich gemacht werden kann

	UINT m_iSecurityPoints;				///< globale Anzahl der produzierten Geheimdienstpunkte

	UINT m_iInnerStorage;				///< Lager der angesammelten Geheimdienstpunkte für die innere Sicherheit

	map<CString, UINT> m_iSPStorage[2];	///< Lager der angesammelten Spionage- und Sabotagepunkte bei einer bestimmten Rasse

	short m_nInnerSecurityBoni;			///< Bonus auf die innere Sicherheit

	short m_nEconomyBoni[2];			///< Wirtschaftsbonus für Spionage und Sabotage;

	short m_nScienceBoni[2];			///< Wissenschaftsbonus für Spionage und Sabotage;

	short m_nMilitaryBoni[2];			///< Militärbonus für Spionage und Sabotage;

	map<CString, BYTE> m_byAggressiveness[2];	///< Aggressivität, mit der bei Spionage und Sabotage vorgegangen wird

	CIntelAssignment m_Assignment;		///< Zuteilungen auf die einzelnen Geheimdienstressorts

	CIntelReports m_Reports;			///< alle Geheimdienstreports (Aktionen)

	CIntelInfo m_IntelInfo;				///< Geheimdienstinformationen (Datensammlung über andere Rassen)
};
