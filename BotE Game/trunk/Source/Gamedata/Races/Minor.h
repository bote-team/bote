/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "Race.h"

// forward declaration
class CBotf2Doc;

/// Klasse zur Abstraktion einer Minorrace
class CMinor : public CRace
{	
	friend class CMinorAI;
	friend class CRaceController;

	DECLARE_SERIAL(CMinor)
public:
	/// Standardkonstruktur
	CMinor(void);
	/// Standarddestruktor
	virtual ~CMinor(void);
	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	// zum Lesen der Membervariablen

	/// Funktion gibt die Koordinate des Heimatsystems der Minorrace zurück.
	/// @return Koordinate des Heimatsystems
	const CPoint& GetRaceKO(void) const {return m_ptKO;}

	/// Funktion gibt technologische Fortschrittlichkeit der Rasse zurück.
	/// @return technologische Fortschrittlichkeit
	BYTE GetTechnologicalProgress(void) const {return m_iTechnologicalProgress;}

	/// Funktion gibt diplomatische Bestechlichkeit der Rasse zurück.
	/// @return Bestechlichkeit
	BYTE GetCorruptibility(void) const {return m_iCorruptibility;}
	
	/// Funktion gibt zurück, ob die Rasse eine Raumfahrtnation ist.
	/// @return <code>true</code> wenn Raumfahrtnation, sonst <code>false</code>
	bool GetSpaceflightNation(void) const {return m_bSpaceflight;}	

	/// Wurde die Minorrace von einer Hauptrasse unterworfen.
	/// @return <code>true</code> wenn unterworfen, sonst <code>false</code>
	bool GetSubjugated(void) const {return m_bSubjugated;}

	/// Funktion gibt die Akzeptanzpunkte zu einer gewünschten Majorrace zurück.
	/// @param sRaceID Rassen-ID der gewünschten Majorrace
	/// @return Akzeptanzpunkte
	short GetAcceptancePoints(const CString& sRaceID) const;

	/// Funktion erstellt eine Tooltipinfo der Rasse.
	/// @return	der erstellte Tooltip-Text
	virtual CString GetTooltip(void) const;

	// zum Schreiben der Membervariablen
	/// Funktion schreibt die Koordinate des Heimatsystems der Minorrace.
	/// @param Koordinate des Heimatsystems
	void SetRaceKO(const CPoint& ko) {m_ptKO = ko;}

	/// Funktion zum Setzen der technischen Fortschrittlichkeit der Rasse.
	/// @param byProgress Fortschrittlichkeit
	void SetTechnologicalProgress(BYTE byProgress) {m_iTechnologicalProgress = byProgress;}
	
	/// Funktion zum Setzen der Bestechlichkeit der Rasse.
	/// @param byCorr Bestechlichkeit
	void SetCorruptibility(BYTE byCorr) {m_iCorruptibility = byCorr;}

	/// Funktion zum Setzen, ob die Rasse eine Raumfahrtnation ist, also Werften besitzt.
	/// @param bSpaceflight <code>true</code> für Raumfahrtnation, sonst <code>false</code>
	void SetSpaceflightNation(bool bSpaceflight = true) {m_bSpaceflight = bSpaceflight;}

	/// Funktion addiert Akzeptanzpunkte zu den Vorhandenen Punkten dazu.
	/// @param sRaceID Rassen-ID der gewünschten Majorrace
	/// @param nAdd hinzuzufügende Akzeptanzpunkte
	void SetAcceptancePoints(const CString& sRaceID, short nAdd);

	/// Funktion legt fest, ob eine Minorrace als erobert gilt oder nicht.
	/// @param bIs Wahrheitswert
	void SetSubjugated(bool bIs) {m_bSubjugated = bIs;}
	
	// Funktionen
	/// Diese Funktion lässt die Minorrace mit einer gewissen Wahrscheinlichkeit einen weiteren Planeten
	/// in ihrem System kolonisieren.
	/// @param pDoc Zeiger auf das Dokument
	/// @return <code>true</code> wenn ein neuer Planet kolonisiert wurde, ansonsten <code>false</code>
	bool PerhapsExtend(CBotf2Doc* pDoc);

	/// Diese Funktion lässt die Minorrace vielleicht ein Schiff bauen
	/// @param pDoc Zeiger auf das Dokument
	void PerhapsBuildShip(CBotf2Doc* pDoc);	

	/// Funktion berechnet wieviele Beziehungspunkte eine Majorrace für längere Beziehungsdauer bekommt.
	/// Umso mehr Punkte sie hat, umso schwerer wird es für andere Majorraces diese Rasse durch Geschenke
	/// zu beeinflussen
	/// @param pDoc Zeiger auf das Dokument
	void CalcAcceptancePoints(CBotf2Doc* pDoc);

	/// Funktion berechnet, wieviel Rohstoffe pro Runde (also bei jedem NextRound() aufrufen) von der
	/// Minorrace verbraucht werden. Wenn die Minorrace die Möglichkeit hat, den jeweiligen Rohstoff
	/// selbst abzubauen, damm ist der Verbrauch natürlich geringer.
	/// @param pDoc Zeiger auf das Dokument
	void ConsumeResources(CBotf2Doc* pDoc);

	/// Funktion überprüft, ob die Minorrace das Angebot aufgrund anderer Verträge überhaupt annehmen kann.
	/// @param pDoc Zeiger auf das Dokument
	/// @param sMajorID MajorraceID, von welcher das Angebot stammt
	/// @param nType angebotener Vertrag
	/// @return <code>true</code> wenn das Angebot theoretisch angenommen werden könnte, ansonsten <code>false</code>
	bool CanAcceptOffer(CBotf2Doc* pDoc, const CString& sMajorID, short nType) const;

	/// Funktion checkt die diplomatische Konsistenz und generiert bei Kündigungen auch die entsprechende Nachricht
	/// für das entsprechende Imperium der Majorrace.
	/// @param pDoc Zeiger auf das Dokument
	void CheckDiplomaticConsistence(CBotf2Doc* pDoc);

	/// Funktion überprüft die Beziehungen zu den Hauptrassen. Wenn diese zuweit vom Vertrag abweicht könnte gekündigt werden.
	/// @param pDoc Zeiger auf das Dokument
	void PerhapsCancelAgreement(CBotf2Doc* pDoc);

	/// Funktion zum erstellen einer Rasse.
	/// Die Funktion liest einen entsprechenden Eintrag aus einer data Datei.
	/// @param saInfo Referenz auf Rasseninformationen
	/// @param nPos Referenz auf Position im Array, ab wann die Informationen gelten
	virtual void Create(const CStringArray& saInfo, int& nPos);

	/// Funktion zum erstellen von Weltraummonstern
	/// Die Funktion liest einen entsprechenden Eintrag aus einer data Datei.
	/// @param saInfo Referenz auf Rasseninformationen
	/// @param nPos Referenz auf Position im Array, ab wann die Informationen gelten
	void CreateAlienEntities(const CStringArray& saInfo, int& nPos);

	/// Funktion gibt zurück, ob es sich um eine Alienrasse (kein Heimatsystem) handelt
	/// @return <code>true</code> wenn es eine Alienrasse ist, sonst <code>false</code>
	bool IsAlienRace() const { return m_sHomeSystem == ""; }

	/// Funktion zum zurücksetzen aller Werte auf Ausgangswerte.
	virtual void Reset(void);

protected:
	// Attribute
	// feste Attribute aus der Data-Datei
	CPoint m_ptKO;							///< Koordinaten der Rasse
	BYTE m_iTechnologicalProgress;			///< wie fortschrittlich ist die Minorrace?
	BYTE m_iCorruptibility;					///< wie stark ändert sich die Beziehung beim Geschenke geben?
	bool m_bSpaceflight;					///< Spaceflightnation (hat Schiffe)

	// Ingame Attribute
	bool				m_bSubjugated;		///< wurde die Rasse unterworfen
	map<CString, short> m_mAcceptance;		///< Punkte die eine MajorRace durch längere Beziehung mit der Rasse ansammelt, wird schwerer diese Rasse wegzukaufen
};
