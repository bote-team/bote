/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "afxtempl.h"
#include "Options.h"

class CTradeHistory : public CObject
{
public:
	DECLARE_SERIAL (CTradeHistory)
	// Standardkonstruktor
	CTradeHistory(void);

	// Destruktor
	virtual ~CTradeHistory(void);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	// Funktion gibt alle Preise aller Ressourcen zurück
	CArray<USHORT,USHORT>* GetHistoryPrices() {return m_Prices;}

	// Funktion gibt alle Preise einer bestimmten Ressource zurück
	CArray<USHORT,USHORT>* GetHistoryPriceFromRes(USHORT res) {return &m_Prices[res];}

	// zum Schreiben der Membervariablen

// sonstige Funktionen
	// Funktion fügt die aktuelle Preisliste in das Feld ein
	void SaveCurrentPrices(USHORT* resPrices, float currentTax);

	// Funktion gibt den Minimumpreis einer bestimmten Ressource zurück
	USHORT GetMinPrice(USHORT res);

	// Funktion gibt den Maximumpreis einer bestimmten Ressource zurück. Übergeben wird die Art der Ressource
	// und optional der Bereich, aus dem das Maximum ermittelt werden soll
	USHORT GetMaxPrice(USHORT res, USHORT start = 0, USHORT end = 0);

	// Funktion gibt den Durchschnittspreis einer bestimmten Ressource zurück
	USHORT GetAveragePrice(USHORT res);

	// Resetfunktion für die Klasse CTradeHistory
	void Reset();

private:
	// Feld mit Einträgen (dynamisches Feld) der Preise aller Runden im Spiel
	CArray<USHORT,USHORT> m_Prices[5];
};
