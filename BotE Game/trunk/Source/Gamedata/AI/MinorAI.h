/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "DiplomacyAI.h"

/// Klasse für die Diplomatie KI einer Majorrace
class CMinorAI : public CDiplomacyAI
{
public:
	/// Standardkonstruktor
	/// @param pRace Zeiger auf Rasse, zu welcher das Diplomatieobjekt gehört
	CMinorAI(const boost::shared_ptr<CRace>& pRace);
	/// Standarddestruktor
	virtual ~CMinorAI(void);

	// Funktionen

	/// Diese Funktion berechnet, wie eine computergesteuerte Rasse, auf ein Angebot reagiert
	/// Das ist die komplette KI der Reaktion auf irgendwelche Angebote.
	/// @param info Information des diplomatischen Angebots
	/// @return <code>ACCEPTED</code> für Annahme
	///			<code>DECLINED</code> für Ablehnung
	///			<code>NOT_REACTED</code> für keine Reaktion
	virtual ANSWER_STATUS::Typ ReactOnOffer(const CDiplomacyInfo& info);

	/// Funktion zur Erstellung eines diplomatischen Angebots.
	/// @param sRaceID RassenID der Rasse, an die das Angebot gemacht werden soll
	/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
	/// @return <code>true</code> wenn ein Angebot erzeugt wurde, ansonsten <code>false</code>
	virtual bool MakeOffer(CString& sRaceID, CDiplomacyInfo& info);

protected:
	// private Funktionen
	/// Funktion berechnet die Beziehungsverbesserungen durch die Übergabe von Credits und Ressourcen.
	/// Die Credits werden hier jedoch nicht gutgeschrieben, sondern nur die Beziehung zur Majorrace verbessert.
	/// @param info Referenz auf eine Diplomatieinformation
	virtual void ReactOnDowry(const CDiplomacyInfo& info);

	/// Funktion berechnet den Bestechungsversuch bei der Minorrace
	/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
	/// @return <code>true</code> wenn Bestechung erfolgreich ist, sonst <code>false</code>
	bool TryCorruption(const CDiplomacyInfo& info);

	/// Funktion berechnet die diplomatischen Auswirkungen auf die anderen Rassen, wenn die Beziehung
	/// durch eine Rasse z.B. durch ein Geschenk verbessert wurde.
	/// Diese Funktion wird am Ende von <func>ReactOnDowry</func> aufgerufen und zieht die Prozentpunkte, die die
	/// Geldgeberrasse dazugewonnen hat von den anderen Rassen ab.
	/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
	/// @param nRelationChange gewonnene Prozentpunkte der Geschenkgeberrasse
	void CalcOtherMajorsRelationChange(const CDiplomacyInfo& info, short nRelationChange);

private:
	/// Funktion berechnet, wie viel übergebene Ressourcen als Credits wert sind.
	/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
	/// @return Wert der übergebenen Ressourcen in Credits
	int CalcResInCredits(const CDiplomacyInfo& info);

};
