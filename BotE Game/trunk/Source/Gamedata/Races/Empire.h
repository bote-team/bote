/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "General\Message.h"
#include "Research.h"
#include "Intel\Intelligence.h"
#include "System\GlobalStorage.h"
#include "Events\EventColonization.h"
#include "Events\EventBlockade.h"
#include "Events\EventBombardment.h"
#include "Events\EventResearch.h"
#include "Events\EventFirstContact.h"
#include "Events\EventGameOver.h"
#include "Events\EventRaceKilled.h"
#include "Events\EventVictory.h"
#include "Events\EventAlienEntity.h"

/**
 * Struktur, die die wichtigsten Informationen eines Systems aufnehmen kann. Dies wird benötigt, wenn wir in einer
 * View eine Liste der Systeme aufnehmen möchten. Ich möchte da nur die wichtigsten Infos haben
 */
struct SystemViewStruct {
		bool operator< (const SystemViewStruct& elem2) const { return name < elem2.name;}
		bool operator> (const SystemViewStruct& elem2) const { return name > elem2.name;}

		SystemViewStruct& operator=(const SystemViewStruct &other) {name = other.name; ko = other.ko; return *this;}
		SystemViewStruct(const CString& _name, CPoint _ko) : name(_name), ko(_ko) {};
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

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	/// Funktion gibt die Anzahl der Systeme des Imperiums zurück.
	BYTE GetNumberOfSystems() const {return m_byNumberOfSystems;}

	/// Funktion gibt einen Zeiger auf die Liste der zum Imperium gehörenden Systeme zurück.
	CArray<SystemViewStruct>* GetSystemList() {return &m_SystemList;}

	/// Funktion gibt die Nummer des Imperiums zurück.
	const CString& GetEmpireID() const {return m_sEmpireID;}

	/// Funktion gibt den aktuellen Bestand an Credits zurück.
	long GetCredits() const {return m_iCredits;}

	/// Funktion gibt die gesamte Creditsänderung seit der letzten Runde zurück.
	long GetCreditsChange() const {return m_iCreditsChange;}

	/// Funktion gibt die Schiffsunterstützungskosten zurück.
	USHORT GetShipCosts() const {return m_iShipCosts;}

	/// Funktion gibt die Schiffunterstützungkosten durch die Bevölkerung zurück.
	USHORT GetPopSupportCosts() const {return m_iPopSupportCosts;}

	/// Funktion gibt die aktuell produzierten Forschungspunkte zurück.
	UINT GetFP() const {return m_lFP;}

	/// Funktion gibt die aktuell produtierten Geheimdienstpunkte zurück.
	UINT GetSP() const {return m_Intelligence.GetSecurityPoints();}

	/// Funktion gibt ein Feld mit der Menge aller Ressourcen auf allen Systemen des Imperiums zurück.
	const UINT* GetStorage() const {return m_lResourceStorages;}

	/// Funktion gibt einen Zeiger auf alle Nachrichten an das Imperium zurück.
	MessageArray* GetMessages() {return &m_Messages;}

	/// Funktion gibt einen Zeiger auf das Feld mit den Eventnachrichten für das Imperium zurück.
	/// @return Pointer auf <code>CObArray</code>
	CObArray* GetEventMessages() {return &m_EventMessages;}

	/// Funktion gibt einen Zeiger auf das Forschungsobjekt des Imperiums zurück.
	CResearch* GetResearch(void) {return &m_Research;}

	/// Funktion gibt einen Zeiger auf das Geheimdienstobjekt des Imperiums zurück.
	CIntelligence* GetIntelligence(void) {return &m_Intelligence;}

	/// Funktion liefert einen Zeiger auf das globale Lager eines Imperiums.
	CGlobalStorage* GetGlobalStorage(void) {return &m_GlobalStorage;}

	// zum Schreiben der Membervariablen
	/// Funktion addiert die im Parameter <code>add</code> übergebene Menge zu der Anzahl der Systeme
	/// des Imperiums.
	/// @param n Anzahl der Systene
	void SetNumberOfSystems(BYTE n) {m_byNumberOfSystems = n;}

	/// Funktion legt die zugehörige Rassennummer des Imperiums fest.
	/// @param empireNumber Rassennummer
	void SetEmpireID(const CString& sRaceID) {m_sEmpireID = sRaceID; m_Intelligence.SetRaceID(sRaceID);}

	/// Funktion addiert die übergebene Menge zum Creditsbestand.
	/// @param add Anzahl des zu addierenden Creditss
	void SetCredits(int add) {m_iCredits += add;}

	/// Funktion legt die Creditsänderung zur vorherigen Runde fest.
	/// @param change Änderung des Creditss gegnüber der vorherigen Runde.
	void SetCreditsChange(int change) {m_iCreditsChange = change;}

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
	void SetStorageAdd(USHORT res, UINT add) {m_lResourceStorages[res] += add;}

	/// Funktion fügt eine übergebene Nachricht dem Nachrichtenfeld des Imperiums hinzu.
	/// @param message Nachricht
	void AddMessage(const CMessage &message) {m_Messages.Add(message);}

	// Sonstige Funktionen
	/// Resetfunktion für das CEmpire-Objekt.
	void Reset(void);

	/// Funktion generiert die Liste der Systeme für das Imperium anhand aller Systeme.
	/// @param systems Feld aller Systeme
	void GenerateSystemList(const std::vector<CSystem>& systems, const std::vector<CSector>& sectors);

	/// Funktion setzt die Lager und aktuell prodzuierten Forschungs- und Geheimdienstpunkte wieder auf NULL
	/// damit man diese später wieder füllen kann. Die wirklichen Lager und Punkte in den einzelnen Systemen
	/// werden dabei nicht angerührt
	void ClearAllPoints(void);

	/// Funktion löscht alle Nachrichten und Antworten an das Imperiums.
	void ClearMessagesAndEvents(void);

private:
	long m_iCredits;				///< Credits des Imperiums

	long m_iCreditsChange;			///< Gewinn bzw Creditsverlust zur letzten Runde

	USHORT m_iShipCosts;			///< die Schiffsunterstützungskosten des Imperiums

	USHORT m_iPopSupportCosts;		///< Unterstützungskosten aufgrund der Bevölkerung

	UINT m_lFP;						///< aktuelle FP des Imperiums

	UINT m_lResourceStorages[DERITIUM+1];	///< die gesamte Menge auf allen Systemen der jeweiligen Ressource

	CString m_sEmpireID;			///< gibt die ID der Rasse des Imperiums zurück

	BYTE m_byNumberOfSystems;		///< Anzahl Systeme des Imperiums

	MessageArray m_Messages;		///< alle Nachrichten an das Imperium

	CObArray m_EventMessages;		///< alle Events für das Imperium

	CResearch m_Research;			///< die Forschung des Imperiums

	CIntelligence m_Intelligence;	///< der Geheimdienst des Imperiums

	CGlobalStorage m_GlobalStorage;	///< das globale Lager des Imperiums

	CArray<SystemViewStruct> m_SystemList;	///< Zeiger auf die zum Imperium gehörenden Systeme
};
