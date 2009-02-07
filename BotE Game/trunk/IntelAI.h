/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "Options.h"

/**
 * Diese Klasse beinhaltet bestimmte Prioritäten, welche im Zusammenhang des Geheimdienstes stehen. Zusätzlich finden
 * hier die Berechnungen der KI für den Geheimdienst statt.
 */
class CBotf2Doc;
class CIntelAI
{
public:
	/// Konstruktor
	CIntelAI(void);
	
	/// Destruktor
	~CIntelAI(void);

	// Zugriffsfunktionen
	/// Funktion gibt die Priorität für den Geheimdienst für eine Rasse zurück.
	/// @param race gewünschte Rasse
	BYTE GetIntelPrio(BYTE race) {ASSERT(race); return m_byIntelPrio[race-1];}

	// sonstige Funktionen
	/// Funktion berechnet den Umgang mit dem Geheimdienst für die KI. Sie trifft Zuteilungen für die einzelnen Rassen.
	/// @param pDoc Zeiger auf das Dokument
	void CalcIntelligence(CBotf2Doc* pDoc);

	/// Resetfunktion für ein CIntelAI-Objekt.
	void Reset(void);

private:
	// Attribute
	BYTE m_byIntelPrio[DOMINION];		///< beinhaltet die Priorität zum Bau von Geheimdienstgebäuden


};
