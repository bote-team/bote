/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "Options.h"
#include "IntelAI.h"
#include <map>

class CBotf2Doc;
class CSectorAI;

using namespace std;
/**
 * Diese Klasse beinhaltet zusätzliche Prioritäten für den Schiffsbau und Truppenbau, sowie den Geheimdienst.
 * Diese Prioritäten werden dann bei der Ausführung der System-KI mitbeachtet.
 */
class CAIPrios
{
public:
	/// Konstruktor
	CAIPrios(CBotf2Doc* pDoc);

	/// Destruktor
	~CAIPrios(void);

	// Zugriffsfunktionen
	/// Funtkion gibt die Prioritäten ein Kolonieschiff zu bauen zurück.
	BYTE GetColoShipPrio(const CString& sRaceID) {return m_mColoShipPrio[sRaceID];}

	/// Funtkion gibt die Prioritäten ein Kolonieschiff zu bauen zurück.
	BYTE GetTransportShipPrio(const CString& sRaceID) {return m_mTransportPrio[sRaceID];}

	/// Funtkion gibt die Prioritäten ein Kriegsschiff zu bauen zurück.
	BYTE GetCombatShipPrio(const CString& sRaceID) {return m_mCombatShipPrio[sRaceID];}

	/// Funktion gibt einen Zeiger auf das Geheimdienstobjekt für eine bestimmte Rasse zurück.
	/// @param race Rasse
	CIntelAI* GetIntelAI(void) {return &m_IntelAI;}

	// sonstige Funktionen
	/// Funktion sollte aufgerufen werden, wenn diese Priorität gewählt wurde. Denn dann verringert sich
	/// diese Priorität.
	void ChoosedColoShipPrio(const CString& sRaceID) {m_mColoShipPrio[sRaceID] /= 2;}

	/// Funktion sollte aufgerufen werden, wenn diese Priorität gewählt wurde. Denn dann verringert sich
	/// diese Priorität.
	void ChoosedTransportShipPrio(const CString& sRaceID) {m_mTransportPrio[sRaceID] /= 2;}

	/// Funktion sollte aufgerufen werden, wenn diese Priorität gewählt wurde. Denn dann verringert sich
	/// diese Priorität.
	void ChoosedCombatShipPrio(const CString& sRaceID) {m_mCombatShipPrio[sRaceID] /= 2;}

	/// Diese Funktion berechnet die Prioritäten der einzelnen Majorrassen, wann sie ein Kolonieschiff in Auftrag
	/// geben sollen.
	void CalcShipPrios(CSectorAI* sectorAI);

	/// Funktion löscht alle vorher berechneten Prioritäten.
	void Clear(void);

private:
	// Attribute
	/// Beinhaltet die Priorität einer Rasse ein Kolonieschiff zu bauen.
	map<CString, BYTE> m_mColoShipPrio;

	/// Beinhaltet die Priorität Truppentransporter zu bauen.
	map<CString, BYTE> m_mTransportPrio;

	/// Beinhaltet die Priorität Kriegschiffe zu bauen.
	map<CString, BYTE> m_mCombatShipPrio;

	/// Beinhaltet Prioritäten für den Geheimdienst und stellt Funktionen zur Steuerung der KI bereit
	CIntelAI m_IntelAI;

	/// Ein Zeiger auf das Document.
	CBotf2Doc* m_pDoc;
};

/*
Schiffsangriff:

Wann wird ein Zielsektor ermittelt?
-----------------------------------
Genau dann, wenn wir angegriffen werden, einen Angriff vorbereiten wollen (noch kein Krieg) oder im Krieg mit einer Rasse
sind und sofort angreifen wollen.


Wie wird ein Zielsektor ermittelt?
----------------------------------
Wir müssen zwischen Offensive und Defensive unterscheiden. Wollen wir angreifen haben wir genug Zeit die Schiffe zu
sammeln. Müssen wir uns verteidigen, so müssen wir auch sammeln, sollten aber auch angreifen, wenn unsere Flotte
insgesamt schwächer ist. Einen Zielsektor in der Offensive sollte eine gegnerische Flotte oder ein gegnerischer
Aussenposten oder auch ein gegnerisches System sein.


Wie wird die Flotte gesammelt?
------------------------------
Zuerst wird der Sektor gesucht, in dem wir die meisten/stärksten Schiffe stationiert haben. All diese Schiffe werden
zu einer Flotte zusammengefasst und fliegen zum Zielsektor. Doch sie fliegen noch nicht direkt zum Zielsektor, sondern
halten rand()%3+1 Felder davor an. Um dies rauszubekommen werden einfach die letzten rand()%3+1 Flugpfadeinträge der
Flotte entfernt. Dann wird gewartet, bis die anderen Schiffsflotten auf diesem Feld eintreffen. Ich schlage mal vor,
dass ca. 80% - 90% der Flotte auf diesem Feld eingetroffen sein muss oder die Schiffsstärke in diesem Feld schon stärker
als die Schiffsstärke im Zielsektor ist. Trifft eine der beiden Bedingungen ein wird der Kurs zum Zielsektor gesetzt.

*/