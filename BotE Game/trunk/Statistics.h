/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// Statistcs.h: Schnittstelle für die Klasse CStatistcs.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATISTICS_H__684E34B7_6287_4078_8C62_82CF8B35E9E4__INCLUDED_)
#define AFX_STATISTCS_H__684E34B7_6287_4078_8C62_82CF8B35E9E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Empire.h"

class CStatistics : public CObject  
{
public:
	DECLARE_SERIAL (CStatistics)
	// Konstruktor
	CStatistics();
	// Destruktor
	virtual ~CStatistics();
	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);
	// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	const USHORT GetAverageTechLevel() {return m_iAverageTechLevel;}
	ULONG* GetAverageResourceStorages() {return m_lAverageResourceStorages;}
	// zum Schreiben der Membervariablen


	// sonstige Funktionen
	void CalculateAverageTechLevel(CEmpire* empire);
	void CalculateAverageResourceStorages(CEmpire* empire);

private:
	USHORT m_iAverageTechLevel;					// Durchschnittliches Techlevel aller Rassen
	ULONG m_lAverageResourceStorages[IRIDIUM+1];	// Durschschnittlicher Inhalt der Ressourcenlager
};

#endif // !defined(AFX_STATISTCS_H__684E34B7_6287_4078_8C62_82CF8B35E9E4__INCLUDED_)
