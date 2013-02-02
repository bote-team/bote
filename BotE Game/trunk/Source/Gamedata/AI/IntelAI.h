/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "Constants.h"
#include <map>

/**
 * Diese Klasse beinhaltet bestimmte Prioritäten, welche im Zusammenhang des Geheimdienstes stehen. Zusätzlich finden
 * hier die Berechnungen der KI für den Geheimdienst statt.
 */
class CBotEDoc;
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
	BYTE GetIntelPrio(const CString& sRace) {return m_byIntelPrio[sRace];}

	// sonstige Funktionen
	/// Funktion berechnet den Umgang mit dem Geheimdienst für die KI. Sie trifft Zuteilungen für die einzelnen Rassen.
	/// @param pDoc Zeiger auf das Dokument
	void CalcIntelligence(CBotEDoc* pDoc);

	/// Resetfunktion für ein CIntelAI-Objekt.
	void Reset(void);

private:
	// Attribute
	std::map<CString, BYTE> m_byIntelPrio;		///< beinhaltet die Priorität zum Bau von Geheimdienstgebäuden


};
