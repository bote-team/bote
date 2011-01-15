/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include <vector>

// forward declaration
class CBotf2Doc;
class CRace;
class CMajor;
class CMinor;
class CDiplomacyInfo;

/// Klasse zur Verarbeitung aller diplomatischen Ereignisse.
class CDiplomacyController
{
public:

	/// Standardkonstruktor
	CDiplomacyController(void);

	/// Standarddestruktor
	~CDiplomacyController(void);

	// Zugriffsfunktionen

	// sonstige Funktionen
	
	///	Diese Funktion wird bei jeder neuen Rundenberechnung aufgerufen und berechnet wann eine Aktion feuert
	/// und generiert selbst neue diplomatische Nachrichten.
	static void CalcDiplomaticFallouts(void);

private:
	// Attribute

	// private Funktionen

	/// Funktion zum Versenden von diplomatischen Angeboten
	static void Send(void);
	
	/// Funktion zum Versenden von diplomatischen Angeboten an eine Majorrace.
	/// @param pDoc Zeiger auf das Dokument
	/// @param pToMajor Zeiger auf die Empfängerrasse
	/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
	static void SendToMajor(CBotf2Doc* pDoc, CMajor* pToMajor, CDiplomacyInfo* pInfo);

	/// Funktion zum Versenden von diplomatischen Angeboten an eine Minorrace.
	/// @param pDoc Zeiger auf das Dokument
	/// @param pToMinor Zeiger auf die Empfängerrasse
	/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
	static void SendToMinor(CBotf2Doc* pDoc, CMinor* pToMinor, CDiplomacyInfo* pInfo);

	/// Funktion zum Empfangen und Bearbeiten eines diplomatischen Angebots.
	static void Receive(void);

	/// Funktion zum Empfangen und Bearbeiten eines diplomatischen Angebots für eine Majorrace.
	/// @param pDoc Zeiger auf das Dokument
	/// @param pToMajor Zeiger auf die Empfängerrasse
	/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
	static void ReceiveToMajor(CBotf2Doc* pDoc, CMajor* pToMajor, CDiplomacyInfo* pInfo);
	
	/// Funktion zum Empfangen und Bearbeiten eines diplomatischen Angebots für eine Minorrace.
	/// @param pDoc Zeiger auf das Dokument
	/// @param pToMajor Zeiger auf die Empfängerrasse
	/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
	static void ReceiveToMinor(CBotf2Doc* pDoc, CMinor* pToMinor, CDiplomacyInfo* pInfo);

	/// Funktion zum Berechnen aller betroffenen Rassen, welchen zusätzlich der Krieg erklärt wird.
	/// Dies geschieht dadurch, wenn der Kriegsgegner Bündnisse oder Verteidigungspakt mit anderen Rassen besitzt.
	/// @param pDoc Zeiger auf das Dokument
	/// @param pFromMajor Zeiger auf die kriegserklärende Majorrace
	/// @param pToRace Zeiger auf die Rasse, welcher der Krieg erklärt wurde
	/// @return Feld mit allen betroffenen Rassen, denen auch der Krieg erklärt werden muss
	static std::vector<CString> GetEnemiesFromContract(CBotf2Doc* pDoc, CMajor* pFromMajor, CRace* pToRace);
	
	/// Funktion erklärt den Krieg zwischen zwei Rassen und nimmt dabei alle notwendigen Arbeiten vor.
	/// @param pFromRace Zeiger auf die kriegserklärende Rasse
	/// @param pEnemy Zeiger auf die Rasse, welcher Krieg erklärt wird
	/// @param pInfo Diplomatieobjekt
	/// @param bWithMoralEvent <code>true</code> wenn Moralevent mit eingeplant werden soll
	static void DeclareWar(CRace* pFromRace, CRace* pEnemy, CDiplomacyInfo* pInfo, bool bWithMoralEvent);
};
