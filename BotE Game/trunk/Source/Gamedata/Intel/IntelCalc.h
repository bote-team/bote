/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
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
	x- CIntellignce Class richtig serialisieren (pResponsibleRace + Aggressiveness)
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

// forward declaration
class CBotf2Doc;
class CMajor;

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
	/// @param pRace unsere Rasse von welcher die Aktionen ausgehen
	void StartCalc(CMajor* pRace);

	/// Funktion addiert Innere Sicherheitspunkte sowie die ganzen Depotgeheimdienstpunkte einer Rasse zu den vorhandenen.
	/// @param pRace unsere Rasse
	void AddPoints(CMajor* pRace);

	/// Funktion zieht einen rassenabhängigen Prozentsatz von den einzelnen Depots ab. Funkion sollte nach Ausführung
	/// aller anderen Geheimdienstfunktionen aufgerufen werden.
	/// @param pRace Rasse deren Depots betroffen sind
	/// @param perc Prozentsatz um welchen die Depots verringert werden. Standard <code>-1</code>, dann wird der rassenspezifische Prozentsatz verwendet
	void ReduceDepotPoints(CMajor* pRace, int perc = -1);

private:
	// Funktionen
	/// Funktion berechnet ob eine Geheimdienstaktion gegen eine andere Rasse erfolgreich verläuft.
	/// @param pEnemyRace Zielrasse
	/// @param ourSP unsere effektive Geheimdienstpunkte für diese Aktion
	/// @param isSpy <code>TRUE</code> wenn Spionageaktion, <code>FALSE</code> wenn Sabotageaktion
	/// @param pResponsibleRace Rasse von welcher <code>pEnemyRace</code> denkt sie habe die Geheimdiestaktion gestartet. <code>NOBODY</code> wenn Agressor unbekannt
	/// @param type Typ der Aktion -> Wirtschaft == 0, Forschung == 1, Militär == 2, Diplomatie == 3
	/// @return Anzahl der zu startenden Geheimdienstaktionen (<code>NULL</code> bedeutet keine Aktion möglich)
	USHORT IsSuccess(CMajor* pEnemyRace, int ourSP, BOOLEAN isSpy, CMajor* pResponsibleRace, BYTE type);

	/// Funktion entfernt die durch eine Geheimdienstaktion verbrauchten Punkte auf Seiten des Geheimdienstopfers und
	/// auf Seiten des Geheimdienstagressors.
	/// @param pOurRace unsere Rasse, der Agressor
	/// @param pEnemyRace Zielrasse
	/// @param isSpy <code>TRUE</code> wenn Spionageaktion, <code>FALSE</code> wenn Sabotageaktion
	/// @param type Typ der Aktion -> Wirtschaft == 0, Forschung == 1, Militär == 2, Diplomatie == 3
	/// @param isAttempt wenn es sich um einen Anschlag handelt, dann muss der Wert <code>TRUE</code> sein
	void DeleteConsumedPoints(CMajor* pOurRace, CMajor* pEnemyRace, BOOLEAN isSpy, BYTE type, BOOLEAN isAttempt);

	/// Funktion ruft die jeweilige Unterfunktion auf, welche eine Geheimdienstaktion schlussendlich ausführt.
	/// @param pOurRace unsere Rasse
	/// @param pEnemyRace Zielrasse
	/// @param pResponsibleRace zu verantworlich machende Rasse
	/// @param type Typ der Aktion -> Wirtschaft == 0, Forschung == 1, Militär == 2, Diplomatie == 3
	/// @param isSpy <code>TRUE</code> wenn Spionageaktion, <code>FALSE</code> wenn Sabotageaktion
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteAction(CMajor* pOurRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, BYTE type, BOOLEAN isSpy);

	/// Funktion führt eine Wirtschatfsspionageaktion aus.
	/// @param pOurRace unsere Rasse
	/// @param pEnemyRace Zielrasse
	/// @param pResponsibleRace zu verantworlich machende Rasse
	/// @param createText soll die Beschreibung zu der Aktion schon miterstellt werden
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteEconomySpy(CMajor* pOurRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, BOOLEAN createText = TRUE);

	/// Funktion führt eine Forschungsspionageaktion aus.
	/// @param race unsere Rasse
	/// @param pEnemyRace Zielrasse
	/// @param pResponsibleRace zu verantworlich machende Rasse
	/// @param createText soll die Beschreibung zu der Aktion schon miterstellt werden
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteScienceSpy(CMajor* pOurRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, BOOLEAN createText = TRUE);

	/// Funktion führt eine Militärspionageaktion aus.
	/// @param pOurRace unsere Rasse
	/// @param pEnemyRace Zielrasse
	/// @param pResponsibleRace zu verantworlich machende Rasse
	/// @param createText soll die Beschreibung zu der Aktion schon miterstellt werden
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteMilitarySpy(CMajor* pOurRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, BOOLEAN createText = TRUE);

	/// Funktion führt eine Diplomatiespionageaktion aus.
	/// @param race unsere Rasse
	/// @param pEnemyRace Zielrasse
	/// @param pResponsibleRace zu verantworlich machende Rasse
	/// @param createText soll die Beschreibung zu der Aktion schon miterstellt werden
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteDiplomacySpy(CMajor* pOurRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, BOOLEAN createText = TRUE);
	
	/// Funktion führt eine Wirtschatfssabotageaktion aus.
	/// @param pOurRace unsere Rasse
	/// @param pEnemyRace Zielrasse
	/// @param pResponsibleRace zu verantworlich machende Rasse
	/// @param reportNumber Nummer eines zugehörigen Spionagereport, auf dessen Daten die Sabotageaktion durchgeführt wird
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteEconomySabotage(CMajor* pOurRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, int reportNumber = -1);

	/// Funktion führt eine Forschungssabotageaktion aus.
	/// @param pOurRace unsere Rasse
	/// @param pEnemyRace Zielrasse
	/// @param pResponsibleRace zu verantworlich machende Rasse
	/// @param reportNumber Nummer eines zugehörigen Spionagereport, auf dessen Daten die Sabotageaktion durchgeführt wird
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteScienceSabotage(CMajor* pOurRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, int reportNumber = -1);

	/// Funktion führt eine Militärsabotageaktion aus.
	/// @param pOurRace unsere Rasse
	/// @param pEnemyRace Zielrasse
	/// @param pResponsibleRace zu verantworlich machende Rasse
	/// @param reportNumber Nummer eines zugehörigen Spionagereport, auf dessen Daten die Sabotageaktion durchgeführt wird
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteMilitarySabotage(CMajor* pOurRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, int reportNumber = -1);

	/// Funktion führt eine Diplomatiesabotageaktion aus.
	/// @param pOurRace unsere Rasse
	/// @param pEnemyRace Zielrasse
	/// @param pResponsibleRace zu verantworlich machende Rasse
	/// @param reportNumber Nummer eines zugehörigen Spionagereport, auf dessen Daten die Sabotageaktion durchgeführt wird
	/// @return <code>TRUE</code> wenn eine Aktion durchgeführt wurde, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteDiplomacySabotage(CMajor* pOurRace, CMajor* pEnemyRace, CMajor* pResponsibleRace, int reportNumber = -1);

	/// Funktion erstellt den Report, welcher aussagt, dass versucht wurde eine Rasse auszuspionieren/zu sabotieren.
	/// @param pResponsibleRace zu verantworlich machende Rasse
	/// @param pEnemyRace Zielrasse, welche Wind von unserer Aktion bekommt
	/// @param type Typ der Aktion -> Wirtschaft == 0, Forschung == 1, Militär == 2, Diplomatie == 3
	void CreateMsg(CMajor* pResponsibleRace, CMajor* pEnemyRace, BYTE type);
	
	/// Funktion führt einen Anschlag durch.
	/// @param pRace unsere Rasse
	/// @param ourSP unsere effektive Geheimdienstpunkte für diese Aktion
	/// @return <code>TRUE</code> wenn der Anschlag erfolgreich war, ansonsten <code>FALSE</code>
	BOOLEAN ExecuteAttempt(CMajor* pRace, UINT ourSP);

	/// Funktion gibt die aktuell komplett generierten inneren Sicherheitspunkte eines Imperiums zurück.
	/// @param pEnemyRace Rasse von der man die inneren Sicherheitspunkte haben möchte
	UINT GetCompleteInnerSecPoints(CMajor* pEnemyRace);

	// Attribute
	CBotf2Doc* m_pDoc;			///< Zeiger auf das Dokument	
};
