/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once

#include "Empire.h"
#include "Sector.h"

// Die verschiedenen Arten, welchen Status eine Minorrace gegenüber ein Majorrace haben kann
/*	WAR						-1
	NO_AGREEMENT			0
	NON_AGGRESSION_PACT		1
	TRADE_AGREEMENT			2
	FRIENDSHIP_AGREEMENT	3
	ALLIANCE				4
	AFFILIATION				5	// Wie Kooperation, nur wenn jemand uns Krieg erklärt, erklärt die Minorrace ihm auch Krieg und umgekehrt
	MEMBERSHIP				6
	PRESENT					10
	CORRUPTION				11

	NOT_REACTED				2
	DECLINED				3*/

/// Liefert verschiedene Attributswerte der Rassenklassen.
enum RaceAttributes
{
	/// ist die Minorrace eine Raumfahrtnation
	RACE_SPACEFLIGHT			= 1,
	/// Wurde die Minorrace von einem Imperium unterworfen
	RACE_SUBJUGATED				= 2,	
	/// kennt die jeweilige Rasse die andere Hauptrasse
	RACE_KNOWNHUMAN				= 4,	
	RACE_KNOWNFERENGI			= 8,	
	RACE_KNOWNKLINGON			= 16,
	RACE_KNOWNROMULAN			= 32,
	RACE_KNOWNCARDASSIAN		= 64,
	RACE_KNOWNDOMINION			= 128,
	/// Wurde der jeweiligen Majorrace ein Verteidigungspakt abgeschlossen
	RACE_DEFENCEPACTHUMAN		= 256,	
	RACE_DEFENCEPACTFERENGI		= 512,	
	RACE_DEFENCEPACTKLINGON		= 1024,
	RACE_DEFENCEPACTROMULAN		= 2048,
	RACE_DEFENCEPACTCARDASSIAN	= 4096,
	RACE_DEFENCEPACTDOMINION	= 8192
};

class CMinorRace : public CObject  
{
public:
	DECLARE_SERIAL (CMinorRace)
	// Konstruktion & Destruktion
	CMinorRace(void);
	virtual ~CMinorRace(void);
	
	/// Kopierkonstruktor
	CMinorRace(const CMinorRace & rhs);
	
	/// Zuweisungsoperator
	CMinorRace & operator=(const CMinorRace &);
	
	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Testweise mal den "<" und ">" Operator überschrieben -> es wird jetzt sortiert.
	// hier wird aktuell nach dem Rassennamen sortiert
	bool operator< (const CMinorRace& elem2) const { return m_strRaceName < elem2.m_strRaceName;}
	bool operator> (const CMinorRace& elem2) const { return m_strRaceName > elem2.m_strRaceName;}

// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	CPoint GetRaceKO() const {return m_pKO;}
	const CString& GetRaceName() const {return m_strRaceName;}
	const CString& GetHomeplanetName() const {return m_strHomePlanet;}
	const CString& GetRaceDescription() const {return m_strRaceDescription;}
	const CString& GetGraphicName() const {return m_strGraphicName;}
	BYTE GetRelationshipToMajorRace(USHORT race) const;
	BYTE GetTechnologicalProgress() const {return m_iTechnologicalProgress;}
	BYTE GetKind() const {return m_iKind;}
	BOOLEAN GetSpaceflightNation() const {return (m_Attributes & RACE_SPACEFLIGHT) == RACE_SPACEFLIGHT;}

	/**
	 * Diese Funktion gibt die Bestechlichkeit der Minorrace zurück. Wie gut verändert sich also die Beziehung durch
	 * Geschenke geben. D.h. wie gut verbessert sich die Beziehung dieser Rasse
	 * dir gegenüber, nachdem du ihr ein "Geschenk" gemacht hast. Werte gelten wieder von 0 bis 4, wobei 0 bedeutet
	 * "kaum", 1 bedeutet "wenig", 2 bedeutet "mittel", 3 bedeutet "viel" und 4 bedeutet "sehr viel"
	 */
	BYTE GetCorruptibility() const {return m_iCorruptibility;}
	BOOLEAN GetKnownByMajorRace(BYTE race) const {
		switch (race)
		{
		case HUMAN:		return (m_Attributes & RACE_KNOWNHUMAN) == RACE_KNOWNHUMAN;
		case FERENGI:	return (m_Attributes & RACE_KNOWNFERENGI) == RACE_KNOWNFERENGI;
		case KLINGON:	return (m_Attributes & RACE_KNOWNKLINGON) == RACE_KNOWNKLINGON;
		case ROMULAN:	return (m_Attributes & RACE_KNOWNROMULAN) == RACE_KNOWNROMULAN;
		case CARDASSIAN:return (m_Attributes & RACE_KNOWNCARDASSIAN) == RACE_KNOWNCARDASSIAN;
		case DOMINION:	return (m_Attributes & RACE_KNOWNDOMINION) == RACE_KNOWNDOMINION;
		default:		return FALSE;
		}
	}
	BOOLEAN GetSubjugated() const {return (m_Attributes & RACE_SUBJUGATED) == RACE_SUBJUGATED;}
	short GetDiplomacyStatus(USHORT race) const {return m_iDiplomacyStatus[race-1];}
	int GetResourceStorage(USHORT resource) const {return m_iResourceStorage[resource];}
	short GetRelationPoints(USHORT race) const {return m_iRelationPoints[race-1];}
	
	// zum Schreiben der Membervariablen
	void SetRaceKO(CPoint KO) {m_pKO = KO;}
	void SetKnownByMajorRace(USHORT race, BOOLEAN is)
	{
		ASSERT(race >= HUMAN && race <= DOMINION);
		switch (race)
		{
		case HUMAN:		SetAttributes(is, RACE_KNOWNHUMAN, m_Attributes);		break;
		case FERENGI:	SetAttributes(is, RACE_KNOWNFERENGI, m_Attributes);		break;
		case KLINGON:	SetAttributes(is, RACE_KNOWNKLINGON, m_Attributes);		break;
		case ROMULAN:	SetAttributes(is, RACE_KNOWNROMULAN, m_Attributes);		break;
		case CARDASSIAN:SetAttributes(is, RACE_KNOWNCARDASSIAN, m_Attributes);	break;
		case DOMINION:	SetAttributes(is, RACE_KNOWNDOMINION, m_Attributes);	break;
		}
	}

	void SetDiplomacyStatus(USHORT race, short status) {m_iDiplomacyStatus[race-1] = status;}
	void SetRelationshipToMajorRace(USHORT race, short newRelationAdd);
	void SetSubjugated(BOOLEAN is) {SetAttributes(is, RACE_SUBJUGATED, m_Attributes);}
	void SetSpaceflightNation(BOOLEAN is) {SetAttributes(is, RACE_SPACEFLIGHT, m_Attributes);}
	
// sonstige Funktionen
	void GenerateMinorRace(const CString &HomeplanetName);			// Funktion belegt alle Variablen der Klasse mit den richtigen Werten
	void GiveLatinumPresent(USHORT race, short diplomacyBonus, USHORT latinum);	// Funktion brechnet die neuen Bezihungen zu den einzelnen Rassen, wenn die Minorrace ein Geldgeschenk bekommt
	USHORT GiveResourcePresent(USHORT race, USHORT* resource, BOOLEAN corruption); // Funktion berechnet wieviel Latinum die gegebenen Ressourcen wert sind und ruft am ende die Fkt. GiveLatinumPresent auf
	void CalculateDiplomaticFalloutFromPresent(USHORT race, USHORT bonus);	// Fkt. berechnet die Auswirkungen auf die anderen Rassen, wenn Geld gegeben wurde
	short PerhapsMakeOfferToMajorRace(BYTE race);	// Funktion berechnet, ob die Minorrace vlt. ein Angebot an die "race" macht
	BOOLEAN CalculateDiplomaticOffer(USHORT race, short offer, USHORT fromMinor);	// Fkt. berechnet, ob das Angebot angenommen wurde oder nicht
	void CheckDiplomaticConsistence(CEmpire* empires);	// Fkt. checkt die diplomatische Konsistenz und generiert bei Kündigungen auch die entsprechende Nachricht
	void PerhapsCancelAgreement(CEmpire* empires);	// Fkt. checkt die Beziehungen zu den Hauptrasse, wenn diese zuweit von Vertrag abweicht könnte gekündigt werden
	void ConsumeRessources(CSector* sector);	// Fkt. berechnet wieviel die Rasse an Ressourcen pro Runde verbraucht
	void CalculateRelationPoints();				// Fkt. berechnet bei jeder neuen Runde ob eine Majorrace weitere Beziehungspunkte für längere Beziehung bekommt
	BOOLEAN TryCorruption(USHORT race, USHORT latinum, CEmpire* empire, USHORT corruptedMajor);	// Fkt. die unseren Bestechungsversuch berechnet
	
	
	/// Diese Funktion läßt die Minorrace mit einer gewissen Wahrscheinlichkeit einen weiteren Planeten in ihrem System
	/// kolonisieren.
	/// @param planets ist das Feld der Planeten in dem Sektor der Minorrace.
	/// @return <code>true</code> wenn ein neuer Planet kolonisiert wurde, ansonsten <code>false</code>
	BOOLEAN PerhapsExtend(CArray<CPlanet>* planets);	
	
	/// Resetfunktion für die CMinorRace Klasse.
	void Reset(void);

private:
	CPoint m_pKO;						///< Koordinaten der Rasse
	CString m_strRaceName;				///< Der Name der Minorrace
	CString m_strHomePlanet;			///< Der Name des Heimatplaneten der Minorrace
	CString m_strRaceDescription;		///< Die Beschreibung der Minorrace
	CString m_strGraphicName;			///< Name der zugehörigen Grafikdatei
	BYTE m_byMajorRelations[6];			///< Die Beziehung zu den einzelnen Hauptrassen
	BYTE m_iTechnologicalProgress;		///< wie fortschrittlich ist die Minorrace?
	BYTE m_iKind;						///< Art der Minorrace (landwirtschaftlich, kriegerish, industriell usw.)
	BYTE m_iCorruptibility;				///< wie stark ändert sich die Beziehung beim Geschenke geben?
	int m_Attributes;					///< diese Variable beinhaltet alle booleschen Rassenattribute
	short m_iDiplomacyStatus[6];		///< Diplomatischer Status gegenüber den Hauptrassen (z.B. Nichtangriffspakt, Krieg, Bündnis usw.)
	short m_iRelationPoints[6];			///< Punkte die eine MajorRace durch längere Beziehung mit der Rasse ansammelt, wird schwerer diese Rasse wegzukaufen
	int m_iResourceStorage[5];			///< die Menge der jeweiligen Ressource, die in dem System der kleinen Rasse gelagert sind
};

// Das dynamische Feld wird vereinfacht als MinorRaceArray angegeben
typedef CArray<CMinorRace,CMinorRace> MinorRaceArray;