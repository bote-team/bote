/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// Statistcs.h: Schnittstelle für die Klasse CStatistcs.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <map>
#include "Options.h"

// forward declaration
class CBotf2Doc;

class CStatistics : public CObject  
{
public:
	DECLARE_SERIAL (CStatistics)
	
	/// Standardkonstruktor
	CStatistics(void);
	/// Standarddestruktor
	virtual ~CStatistics(void);
	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);
	
	// Zugriffsfunktionen	
	// zum Lesen der Membervariablen

	/// Funktion gibt das universumweite Durchschnittstechlevel zurück.
	/// @return durchschnittliches Techlevel aller Hauptrassen
	BYTE GetAverageTechLevel(void) const {return m_byAverageTechLevel;}

	/// Funktion gibt Feld mit durchschnittlichen Ressourcenlagern zurück.
	/// @return Feld der durchschnittliches Menge im Ressourcenlager
	UINT* GetAverageResourceStorages(void) {return m_nAverageResourceStorages;}

	/// Funktion gibt Map mit den Schiffsstärken aller Rassen zurück.
	/// @param sRaceID Rassen-ID für die die Schiffsstärke erfragt werden soll
	/// @return Schiffsstärke der Kriegsschiffe
	UINT GetShipPower(const CString& sRaceID) const;
	
	// zum Schreiben der Membervariablen
	/// Funktion zum Berechnen aller Statistiken.
	/// @param pDoc Zeiger auf das Dokument
	void CalcStats(CBotf2Doc* pDoc);

	/// Funktion zum zurücksetzen aller Werte auf Ausgangswerte.
	void Reset(void);

private:
	// private Funktionen

	/// Funktion zum Berechnen des universumweiten Techdurchschnittlevels.
	/// @param pDoc Zeiger auf das Dokument
	void CalcAverageTechLevel(CBotf2Doc* pDoc);

	/// Funktion zum Berechnen der durchschnittlichen Befüllung der Ressourcenlager.
	/// @param pDoc Zeiger auf das Dokument
	void CalcAverageResourceStorages(CBotf2Doc* pDoc);

	/// Funktion zum Berechnen der gesamten militärischen Schiffsstärken aller Rassen.
	/// @param pDoc Zeiger auf das Dokument
	void CalcShipPowers(CBotf2Doc* pDoc);
	
	// Attribute
	BYTE m_byAverageTechLevel;						///< Durchschnittliches Techlevel aller Rassen
	
	UINT m_nAverageResourceStorages[DILITHIUM + 1];	///< Durschschnittlicher Inhalt der Ressourcenlager	
	
	std::map<CString, UINT> m_mShipPowers;			///< Schiffsstärken aller Rassen
};