/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// Diplomacy.h: Schnittstelle für die Klasse CDiplomacy.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIPLOMACY_H__6A6729CC_0D3B_4E2E_B308_B4DD48CFDC67__INCLUDED_)
#define AFX_DIPLOMACY_H__6A6729CC_0D3B_4E2E_B308_B4DD48CFDC67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MajorRace.h"
#include "GenDiploMessage.h"
#include "GlobalStorage.h"

/* Alle Angebote werden in dieser Struktur gespeichert! Wenn das Angebot an/von eine/r MinorRace geht/ist, dann stimmt
   auch alles mit den Variablennamen überein. Wenn das Angebot an/von eine/r MajorRace geht/ist, dann ist 'majorRace'
   nicht unsere Rasse, sondern die Rasse an die das Angebot geht. 'minorRace' ist dann einfach NULL. Außerdem wird
   'corruptedMajor' bei Angeboten an andere Majorraces als Variable für die Rundendauer des Vertrages verwendet.
   Ist die Dauer "unbegrenzt" so ist der Wert NULL. Bei Bestechungsversuch einer Minorrace ist 'corruptedMajor' die
   Majorrace, die durch das Bestechen benutzt wird
*/
typedef struct {
	BYTE majorRace;
	USHORT minorRace;
	USHORT latinum;
	USHORT ressource[5];
	USHORT round;
	CString shownText;
	short type;
	USHORT offerFromRace;
	CPoint KO;
	BYTE corruptedMajor;
} DiplomaticActionStruct;

typedef struct {
	BYTE majorRace;
	USHORT minorRace;
	CString headline;
	CString message;
} DiplomaticAnswersStruct;

class CDiplomacy : public CObject  
{
public:
	DECLARE_SERIAL (CDiplomacy)

	/// Konstruktor
	CDiplomacy(void);

	/// Destruktor
	virtual ~CDiplomacy(void);

	/// Kopierkonstruktor
	CDiplomacy(const CDiplomacy & rhs);

	/// Zuweisungsoperator
	CDiplomacy & operator=(const CDiplomacy &);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	/// Diese Funktion gibt einen Zeiger auf die Variable zurück, welche alle gemachten Angebote beinhaltet.
	CArray<DiplomaticActionStruct,DiplomaticActionStruct>* GetMadeOffers() {return &m_MadeOfferArray;}
	
	/// Diese Funktion gibt einen Zeiger auf die Variable zurück, welche alle Antworten auf Angebote beinhaltet.
	CArray<DiplomaticAnswersStruct,DiplomaticAnswersStruct>* GetAnswers() {return &m_AnswerArray;}
	
	// zum Schreiben der Membervariablen
	/// Diese Funktion setzt die zugehörige Nummer des Imperiums. Übergeben wird dafür die Rassennummer <code>
	/// raceNumber</code> des jeweiligen Imperiums.
	void SetDiplomacyNumber(BYTE raceNumber) {m_iDiplomacy = raceNumber;}

// sonstige Funktionen
	/// Funktion legt ein neues Strukturobjekt in dem Feld <code>m_MadeOfferArray</code> an. Dieses Feld wird bei
	/// jeder neuen Runde durchlaufen. Wenn eine neue Runde erreicht wurde, welche in der jeweiligen Struktur steht,
	/// dann wird die Aktion ausgeführt.
	void MakeOfferToMinorRace(BYTE majorRace, USHORT minorRace, USHORT latinum, USHORT* ressource, USHORT currentRound, const CString &ShownText, short type, CPoint KO, BYTE corruptedMajor);
	
	/// Funktion legt ein neues Strukturobjekt im Feld <code>m_MadeOfferArray</code> an. Dieses Feld wird bei jeder
	/// neuen Runde durchlaufen. Wenn eine Runde erreicht wurde, welche in der jeweiligen Struktur steht, dann würde
	/// die Aktion ausgeführt werden. <code>duration</code> hat hier mehrere Funktionen. Einerseits steht in der Variable
	/// die Dauer eines Vertrags, natürlich	nur dort, wo man auch eine Dauer angeben kann. Bei einem Kriegspakt steht
	/// in <code>duration</code> die Rasse, welcher der Krieg erklärt werden soll. Ansonsten wird diese Variable nicht benötigt.
	void GetOfferFromMajorRace(BYTE myRace, USHORT latinum, USHORT* ressource, USHORT currentRound, const CString &ShownText, short type, CPoint KO, BYTE duration);
	
	/// Wenn man ein Angebot an eine andere Majorrace machen will, dann muss diese Funktion aufgerufen werden, welche
	/// einen Eintrag in <code>m_MadeOfferArray</code> macht. Dadurch hat man einer anderen Majorrace <code>toMajorRace
	/// </code> ein Angebot gemacht. Diese Funktion wird für das Diplomatieobjekt aufgerufen, für die Rasse
	/// welche das Angebot macht, während <code>GetOfferFromMajorRace()</code> für das Diplomatieobjekt aufgerufen wird,
	/// an die das Angebot geht. <code>duration</code> hat hier mehrere Funktionen. Einerseits steht in der Variable
	/// die Dauer eines Vertrags, natürlich	nur dort, wo man auch eine Dauer angeben kann. Bei einem Kriegspakt steht
	/// in <code>duration</code> die Rasse, welcher der Krieg erklärt werden soll. Ansonsten wird diese Variable nicht benötigt.
	void MakeOfferToMajorRace(BYTE toMajorRace, USHORT latinum, USHORT* ressource, USHORT currentRound, const CString &ShownText, short type, CPoint KO, BYTE duration);
	
	/// Funktion legt ein neues Strukturobjekt im Feld <code>m_MadeOfferArray</code> an. Nur das diesmal der Parameter
	/// <code>offerFromRace</code> auf <code>TRUE</code> gesetzt wird.	Wenn dieses Feld durchlaufen wird und der Nutzer
	/// dieses Objekt nicht durch eine Ablehnung des Angebots gelöscht hat, dann wird der Status der kleinen Rasse auf
	/// ihr Angebot automatisch verändert.
	void MinorMakesOfferToMajor(USHORT minorRace, BYTE majorRace, USHORT currentRound, short type, MinorRaceArray* minors, CMajorRace* majors, CEmpire* empire);
	
	///	Diese Funktion wird bei jeder neuen Rundenberechnung aufgerufen und berechnet wann eine Aktion feuert
	/// und generiert selbst neue diplomatische Nachrichten.
	void CalculateDiplomaticFallouts(MinorRaceArray* minors, CMajorRace* majors, CEmpire* empire,
		USHORT currentRound, CSystem systems[30][20], UINT* averageRessourceStorages, CGlobalStorage* globalStorages, const UINT* shipPowers);
	
	/// Diese Funktion löscht alle Einträge aus den Variablen in denen auf die Minorrace <code>minor</code> verwiesen wurde.
	/// @param minor Die Minorrasse, um die es sich handelt
	/// @param minorDeleted Wurde die Rasse komplett ausgelöscht (nicht nur erobert)
	void ClearArraysFromMinor(USHORT minor, BOOLEAN minorDeleted = FALSE);
	
	/// Resetfunktion für die Klasse CDiplomacy.
	void Reset();

private:
	/// Diese Funktion generiert eine Antwort einer Minorrace auf ein diplomatisches Angebot.
	DiplomaticAnswersStruct GenerateMinorAnswer(BYTE majorRace, USHORT minorRace, short type, CString minorRaceName, USHORT minorType, BOOLEAN accepted);
	
	/// Diese Funktion generiert eine Antwort einer Manorrace auf ein diplomatisches Angebot.
	DiplomacyMajorAnswersStruct GenerateMajorAnswer(short type, BOOLEAN accepted, BYTE answerFrom);
	
	/// Variable beinhaltet alle gemachtet diplomatischen Angebote von uns und an uns.
	CArray<DiplomaticActionStruct, DiplomaticActionStruct> m_MadeOfferArray;

	/// Variable beinhaltet alle Antworten auf ein diplomatisches Angebot.
	CArray<DiplomaticAnswersStruct, DiplomaticAnswersStruct> m_AnswerArray;
	
	/// Zu welchen Imperium gehört dieses Objekt der Diplomatieklasse.
	BYTE m_iDiplomacy;
};

#endif // !defined(AFX_DIPLOMACY_H__6A6729CC_0D3B_4E2E_B308_B4DD48CFDC67__INCLUDED_)
