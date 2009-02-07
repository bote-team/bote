/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "Message.h"
#include "Research.h"
#include "Intelligence.h"
#include "EventColonization.h"
#include "EventBlockade.h"
#include "EventBombardment.h"

struct DiplomacyMajorAnswersStruct
{
	USHORT AnswerFromMajorRace;
	CString headline;
	CString message;
};

/**
 * Struktur, die die wichtigsten Informationen eines Systems aufnehmen kann. Dies wird benötigt, wenn wir in einer
 * View eine Liste der Systeme aufnehmen möchten. Ich möchte da nur die wichtigsten Infos haben
 */ 
struct SystemViewStruct {
		bool operator< (const SystemViewStruct& elem2) const { return name < elem2.name;}
		bool operator> (const SystemViewStruct& elem2) const { return name > elem2.name;}

		SystemViewStruct operator=(const SystemViewStruct &other) {name = other.name; ko = other.ko; return *this;}
		SystemViewStruct(CString _name, CPoint _ko) : name(_name), ko(_ko) {};
		SystemViewStruct() : name(""), ko(CPoint(-1,-1)) {};

		CString name;
		CPoint ko;
	};

class CSystem;
class CSector;
class CEmpire : public CObject  
{
public:
	DECLARE_SERIAL (CEmpire)
	
	/// Konstruktor
	CEmpire();
	
	/// Destruktor
	~CEmpire();
	
/*	/// Kopierkonstruktor
	CEmpire(const CEmpire & rhs);

	/// Zuweisungsoperator
	CEmpire & operator=(const CEmpire &);
*/

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);
	
	// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	/// Funktion gibt die Anzahl der Systeme des Imperiums zurück.
	BYTE GetNumberOfSystems() const {return m_byNumberOfSystems;}

	/// Funktion gibt einen Zeiger auf die Liste der zum Imperium gehörenden Systeme zurück.
	CArray<SystemViewStruct>* GetSystemList() {return &m_SystemList;}
	
	/// Funktion gibt die Nummer des Imperiums zurück.
	BYTE GetEmpireName() const {return m_iEmpire;} 
	
	/// Funktion gibt zurück, ob das Imperium von einem Menschen oder vom Computer gesteuert wird.
	BYTE GetPlayerOfEmpire() const {return m_iPlayerOfEmpire;}
	
	/// Funktion gibt den aktuellen Bestand an Latinum zurück.
	long GetLatinum() const {return m_iLatinum;}
	
	/// Funktion gibt die gesamte Latinumänderung seit der letzten Runde zurück.
	long GetLatinumChange() const {return m_iLatinumChange;}
	
	/// Funktion gibt die Schiffsunterstützungskosten zurück.
	USHORT GetShipCosts() const {return m_iShipCosts;}
	
	/// Funktion gibt die Schiffunterstützungkosten durch die Bevölkerung zurück.
	USHORT GetPopSupportCosts() const {return m_iPopSupportCosts;}
	
	/// Funktion gibt die aktuell produzierten Forschungspunkte zurück.
	UINT GetFP() const {return m_lFP;}

	/// Funktion gibt die aktuell produtierten Geheimdienstpunkte zurück.
	UINT GetSP() const {return m_Intelligence.GetSecurityPoints();}

	/// Funktion gibt ein Feld mit der Menge aller Ressourcen auf allen Systemen des Imperiums zurück.
	const UINT* GetStorage() const {return m_lRessourceStorages;}
	
	/// Funktion gibt einen Zeiger auf alle Nachrichten an das Imperium zurück.
	MessageArray* GetMessages() {return &m_Messages;}

	/// Funktion gibt einen Zeiger auf das Feld mit den Eventnachrichten für das Imperium zurück.
	/// @return Pointer auf <code>CObArray</code>
	CObArray* GetEventMessages() {return &m_EventMessages;}

	/// Funktion gibt einen Zeiger auf Nachrichten von anderen Majorraces an unser Imperium zurück.
	CArray<DiplomacyMajorAnswersStruct,DiplomacyMajorAnswersStruct>* GetIncomingMajorAnswers() {return &m_IncomingMajorAnswers;}
	
	/// Funktion gibt einen Zeiger auf das Forschungsobjekt des Imperiums zurück.
	CResearch* GetResearch() {return &m_Research;}
	
	/// Funktion gibt einen Zeiger auf das Geheimdienstobjekt des Imperiums zurück.
	CIntelligence* GetIntelligence() {return &m_Intelligence;}	
	
	// zum Schreiben der Membervariablen
	/// Funktion addiert die im Parameter <code>add</code> übergebene Menge zu der Anzahl der Systeme
	/// des Imperiums.
	/// @param n Anzahl der Systene
	void SetNumberOfSystems(BYTE n) {m_byNumberOfSystems = n;}

	/// Funktion legt die zugehörige Rassennummer des Imperiums fest.
	/// @param empireNumber Rassennummer
	void SetEmpireName(BYTE empireNumber) {m_iEmpire = empireNumber; m_Intelligence.SetRaceNumber(empireNumber);}

	/// Funktion legt fest, ob das Imperium von einem menschlichen Spieler oder vom Computer gesteuert wird.
	/// @param whoIsPlayer Mensch oder Computer
	void SetPlayerOfEmpire(BYTE whoIsPlayer) {m_iPlayerOfEmpire = whoIsPlayer;}

	/// Funktion addiert die übergebene Menge zum Latinumbestand.
	/// @param add Anzahl des zu addierenden Latinums
	void SetLatinum(int add) {m_iLatinum += add;}

	/// Funktion legt die Latinumänderung zur vorherigen Runde fest.
	/// @param change Änderung des Latinums gegnüber der vorherigen Runde.
	void SetLatinumChange(int change) {m_iLatinumChange = change;}
	
	/// Funktion setzt die Schiffsunterstützungskosten auf den Wert von <code>costs</code>.
	void SetShipCosts(USHORT costs) {m_iShipCosts = costs;}

	/// Funktion addiert die Schiffsunterstützungskosten, übergeben durch den Parameter <code>add</code>, zu den
	/// aktuellen Schiffsunterstützungskosten des Imperiums.
	void AddShipCosts(USHORT add) {m_iShipCosts += add;}

	/// Funktion setzt die Bevölkerungsunterstützungskosten auf den Wert von <code>supportCosts</code>.
	void SetPopSupportCosts(USHORT supportCosts) {m_iPopSupportCosts = supportCosts;}

	/// Funktion addiert die Bevölkerungsunterstützungskosten, übergeben durch den Parameter <code>add</code>, zu den
	/// aktuellen Bevölkerungsunterstützungskosten des Imperiums.
	void AddPopSupportCosts(USHORT add) {m_iPopSupportCosts += add;}

	/// Funktion addiert die übergebene Anzahl an Forschungspunkten zu den Forschungspunkten des Imperiums.
	/// @param add Anzahl der zu addierenden Forschungspunkte
	void AddFP(int add);

	/// Funktion addiert die übergebene Anzahl an Geheimdienstpunkten zu den Geheimdienstpunkten des Imperiums.
	/// @param add Anzahl der zu addierenden Geheimdienstpunkte
	void AddSP(UINT add) {m_Intelligence.AddSecurityPoints(add);}

	/// Funktion addiert die übergebene Menge an Ressourcen zum zugehörigen Ressourcenlager.
	/// @param res Ressource
	/// @param add zu addierende Menge
	void SetStorageAdd(USHORT res, UINT add) {m_lRessourceStorages[res] += add;}

	/// Funktion fügt eine übergebene Nachricht dem Nachrichtenfeld des Imperiums hinzu.
	/// @param message Nachricht
	void AddMessage(const CMessage &message) {m_Messages.Add(message);}

	/// Funktion fügt eine Antwort einer anderen Hauptrasse dem Feld der Antworten von anderen Hauptrassen hinzu.
	/// @param theAnswerToAdd Referenz auf hinzuzufügende Antwort.
	void AddIncomingMajorAnswer(const DiplomacyMajorAnswersStruct &theAnswerToAdd) {m_IncomingMajorAnswers.Add(theAnswerToAdd);}

	/// Funktion generiert die Liste der Systeme für das Imperium anhand aller Systeme.
	/// @param systems Feld aller Systeme
	void GenerateSystemList(const CSystem systems[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT], const CSector sectors[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT]);

// Sonstige Funktionen
	/// Resetfunktion für das CEmpire-Objekt.
	void Reset(void);
	
	/// Funktion setzt die Lager und aktuell prodzuierten Forschungs- und Geheimdienstpunkte wieder auf NULL
	/// damit man diese später wieder füllen kann. Die wirklichen Lager und Punkte in den einzelnen Systemen
	/// werden dabei nicht angerührt
	void ClearAllPoints(void);

	/// Funktion löscht alle Nachrichten und Antworten an das Imperiums.
	void ClearMessagesAndAnswers(void);

private:
	long m_iLatinum;				///< Latinum des Imperiums
	
	long m_iLatinumChange;			///< Gewinn bzw Latinumverlust zur letzten Runde
	
	USHORT m_iShipCosts;			///< die Schiffsunterstützungskosten des Imperiums
	
	USHORT m_iPopSupportCosts;		///< Unterstützungskosten aufgrund der Bevölkerung
	
	UINT m_lFP;						///< aktuelle FP des Imperiums

	UINT m_lRessourceStorages[IRIDIUM+1];	///< die gesamte Menge auf allen Systemen der jeweiligen Ressource
	
	BYTE m_iEmpire;					///< gibt das Imperium zurück, 1 für Föd, 2 für Fer, also wie immer, siehe enum
	
	BYTE m_iPlayerOfEmpire;			///< wird das Imperium vom Computer oder vom Spieler gespielt
	
	BYTE m_byNumberOfSystems;		///< Anzahl Systeme des Imperiums
	
	MessageArray m_Messages;		///< alle Nachrichten an das Imperium

	CObArray m_EventMessages;		///< alle Events für das Imperium
	
	CResearch m_Research;			///< die Forschung des Imperiums

	CIntelligence m_Intelligence;	///< der Geheimdienst des Imperiums

	CArray<SystemViewStruct> m_SystemList;	///< Zeiger auf die zum Imperium gehörenden Systeme

	/// Nachrichten von anderen MajorRaces an uns, normalerweise in CDiplomacy, aber Nachrichten von anderen
	/// Majorraces müssen hier gespeichert werden!
	CArray<DiplomacyMajorAnswersStruct, DiplomacyMajorAnswersStruct> m_IncomingMajorAnswers;
};
