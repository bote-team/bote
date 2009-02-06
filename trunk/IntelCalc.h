/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "afx.h"
#include "Options.h"
#include "EcoIntelObj.h"
#include "ScienceIntelObj.h"
#include "MilitaryIntelObj.h"
#include "DiplomacyIntelObj.h"
#include "IntelReports.h"


/* ZU TUN

	x- alle Geheimdienstobjekte noch serialisieren
	x- CEmpire Class richtig serialisieren
	x- CIntellignce Class richtig serialisieren (responsibleRace + Aggressiveness)
	x- CIntelReport Class richtig serialisieren (m_pAttemptObject)
	x- Dilithium bei Gebäudekosten anzeigen
	x- ResponsibleRace darf nicht die selbe wie das Opfer einer Geheimdienstaktion sein. Dann immer unbekannt.
	x- Schwierigkeitsgrad bei ini-Befehl
	- Wenn Anordnung der Geheimdienstbuttons geändern werden sollte, dann auch checken zu Beginn der OnLeftButtonDown()
	  und in CBotf2Doc -> GetShowIntelReportsInView3()
	- Bei Kriegspaktannahme noch den MoralOberserver wegen Krieggserklärung aufrufen
	- Rücknahme der zu 75% immer erfolgreichen Geheimdienstaktionen, Intelgebäude zu Beginn, bekannte Majors
*/

/**
 * Diese Klasse stellt Methoden zur kompletten Berechnung irgendwelcher Geheimdienstaktionen bereit. Die Klasse
 * ist in der Lage zu ermitteln, ob eine Geheimdienstaktion erfolgreich ist. Außerdem stellt die Klasse
 * Methoden bereit, wodurch die Auswirkungen einer Aktion sofort ausgeführt werden können.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */

class CBotf2Doc;
class CIntelCalc
{
public:
	/// Konstruktor
	CIntelCalc(CBotf2Doc* pDoc);

	/// Destruktor
	~CIntelCalc(void);

	// Zugriffsfunktionen

	// sonstige Funktionen
	/// Funktion berechnet die kompletten Geheimdienstaktionen und nimmt gleichzeitig auch alle Veränderungen vor.
	/// Die Funktion verschickt auch alle relevanten Nachrichten an die betroffenen Imperien.
	/// @param race unsere Rasse von welcher die Aktionen ausgehen
	void StartCalc(BYTE race);

	/// Funktion addiert Innere Sicherheitspunkte sowie die ganzen Depotgeheimdienstpunkte einer Rasse zu den vorhandenen.
	/// @param race unsere Rasse
	void AddPoints(BYTE race);

	/// Funktion zieht einen rassenabhängigen Prozentsatz von den einzelnen Depots ab. Funkion sollte nach Ausführung
	/// aller anderen Geheimdienstfunktionen aufgerufen werden.
	/// @param race Rasse deren Depots betroffen sind
	/// @param perc Prozentsatz um welchen die Depots verringert werden. Standard <code>-1</code>, dann wird der rassenspezifische Prozentsatz verwendet
	void ReduceDepotPoints(BYTE race, int perc = -1);

private:
	// Funktionen
	/// Funktion berechnet ob eine Geheimdienstaktion gegen eine andere Rasse erfolgreich verläuft.
	/// @param enemyRace Zielrasse
	/// @param ourSP unsere effektive Geheimdienstpunkte für diese Aktion
	/// @param isSpy <code>TRUE</code> wenn Spionageaktion, <code>FALSE</code> wenn Sabotageaktion
	/// @param responsibleRace Rasse von welcher <code>enemyRace</code> denkt sie habe die Geheimdiestaktion gestartet. <code>NOBODY</code> wenn Agressor unbekannt
	/// @param type Typ der Aktion -> Wirtschaft == 0, Forschung == 1, Militär == 2, Diplomatie == 3
	/// @return Anzahl der zu startenden Geheimdienstaktionen (<code>NULL</code> bedeutet keine Aktion möglich)
	USHORT IsSuccess(BYTE enemyRace, UINT ourSP, BOOLEAN isSpy, BYTE &responsibleRace, BYTE type);

	/// Funktion entfernt die durch eine Geheimdienstaktion verbrauchten Punkte auf Seiten des Geheimdienstopfers und
	/// auf Seiten des Geheimdienstagressors.
	/// @param ourRace unsere Rasse, der Agressor
	/// @param enemyRace Zielrasse
	/// @param isSpy <code>TRUE</code> wenn Spionageaktion, <code>FALSE</code> wenn Sabotageaktion
	/// @param type Typ der Aktion -> Wirtschaft == 0, Forschung == 1, Militär == 2, Diplomatie == 3
	/// @param isAttempt wenn es sich um einen Anschlag handelt, dann muss der Wert <code>TRUE</code> sein
	void DeleteConsumedPoints(BYTE ourRace, BYTE enemyRace, BOOLEAN isSpy, BYTE type, BOOLEAN isAttempt);

	/// Funktion ruft die jeweilige Unterfunktion auf, welche eine Geheimdienstaktion schlussendlich ausführt.
	/// @param race unsere Rasse
	/// @param enemyRace Zielrasse
	/// @param responsibleRace zu verantworlich machende Rasse
	/// @param type Typ der Aktion -> Wirtschaft == 0, Forschung == 1, Militär == 2, Diplomatie == 3
	/// @param isSpy <code>TRUE</code> wenn Spionageaktion, <code>FALSE</code> wenn Sabotageaktion
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteAction(BYTE race, BYTE enemyRace, BYTE responsibleRace, BYTE type, BOOLEAN isSpy);

	/// Funktion führt eine Wirtschatfsspionageaktion aus.
	/// @param race unsere Rasse
	/// @param enemyRace Zielrasse
	/// @param responsibleRace zu verantworlich machende Rasse
	/// @param createText soll die Beschreibung zu der Aktion schon miterstellt werden
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteEconomySpy(BYTE race, BYTE enemyRace, BYTE responsibleRace, BOOLEAN createText = TRUE);

	/// Funktion führt eine Forschungsspionageaktion aus.
	/// @param race unsere Rasse
	/// @param enemyRace Zielrasse
	/// @param responsibleRace zu verantworlich machende Rasse
	/// @param createText soll die Beschreibung zu der Aktion schon miterstellt werden
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteScienceSpy(BYTE race, BYTE enemyRace, BYTE responsibleRace, BOOLEAN createText = TRUE);

	/// Funktion führt eine Militärspionageaktion aus.
	/// @param race unsere Rasse
	/// @param enemyRace Zielrasse
	/// @param responsibleRace zu verantworlich machende Rasse
	/// @param createText soll die Beschreibung zu der Aktion schon miterstellt werden
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteMilitarySpy(BYTE race, BYTE enemyRace, BYTE responsibleRace, BOOLEAN createText = TRUE);

	/// Funktion führt eine Diplomatiespionageaktion aus.
	/// @param race unsere Rasse
	/// @param enemyRace Zielrasse
	/// @param responsibleRace zu verantworlich machende Rasse
	/// @param createText soll die Beschreibung zu der Aktion schon miterstellt werden
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteDiplomacySpy(BYTE race, BYTE enemyRace, BYTE responsibleRace, BOOLEAN createText = TRUE);
	
	/// Funktion führt eine Wirtschatfssabotageaktion aus.
	/// @param race unsere Rasse
	/// @param enemyRace Zielrasse
	/// @param responsibleRace zu verantworlich machende Rasse
	/// @param reportNumber Nummer eines zugehörigen Spionagereport, auf dessen Daten die Sabotageaktion durchgeführt wird
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteEconomySabotage(BYTE race, BYTE enemyRace, BYTE responsibleRace, int reportNumber = -1);

	/// Funktion führt eine Forschungssabotageaktion aus.
	/// @param race unsere Rasse
	/// @param enemyRace Zielrasse
	/// @param responsibleRace zu verantworlich machende Rasse
	/// @param reportNumber Nummer eines zugehörigen Spionagereport, auf dessen Daten die Sabotageaktion durchgeführt wird
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteScienceSabotage(BYTE race, BYTE enemyRace, BYTE responsibleRace, int reportNumber = -1);

	/// Funktion führt eine Militärsabotageaktion aus.
	/// @param race unsere Rasse
	/// @param enemyRace Zielrasse
	/// @param responsibleRace zu verantworlich machende Rasse
	/// @param reportNumber Nummer eines zugehörigen Spionagereport, auf dessen Daten die Sabotageaktion durchgeführt wird
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteMilitarySabotage(BYTE race, BYTE enemyRace, BYTE responsibleRace, int reportNumber = -1);

	/// Funktion führt eine Diplomatiesabotageaktion aus.
	/// @param race unsere Rasse
	/// @param enemyRace Zielrasse
	/// @param responsibleRace zu verantworlich machende Rasse
	/// @param reportNumber Nummer eines zugehörigen Spionagereport, auf dessen Daten die Sabotageaktion durchgeführt wird
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteDiplomacySabotage(BYTE race, BYTE enemyRace, BYTE responsibleRace, int reportNumber = -1);

	/// Funktion erstellt den Report, welcher aussagt, dass versucht wurde eine Rasse auszuspionieren/zu sabotieren.
	/// @param responsibleRace zu verantworlich machende Rasse
	/// @param enemyRace Zielrasse, welche Wind von unserer Aktion bekommt
	/// @param type Typ der Aktion -> Wirtschaft == 0, Forschung == 1, Militär == 2, Diplomatie == 3
	void CreateMsg(BYTE responsibleRace, BYTE enemyRace, BYTE type);
	
	/// Funktion führt einen Anschlag durch.
	/// @param race unsere Rasse
	/// @param ourSP unsere effektive Geheimdienstpunkte für diese Aktion
	/// @return <code>TRUE</code> wenn der Anschlag erfolgreich war, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteAttempt(BYTE race, UINT ourSP);

	/// Funktion gibt die aktuell komplett generierten inneren Sicherheitspunkte eines Imperiums zurück.
	/// @param enemyRace Rasse von der man die inneren Sicherheitspunkte haben möchte
	UINT GetCompleteInnerSecPoints(BYTE enemyRace);

	// Attribute
	CBotf2Doc* m_pDoc;			///< Zeiger auf das Dokument	
};
