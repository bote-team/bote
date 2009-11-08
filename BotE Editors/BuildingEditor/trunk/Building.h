/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// Building.h: Schnittstelle für die Klasse CBuilding.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "Options.h"

class CBuilding : public CObject  
{
public:
	DECLARE_SERIAL (CBuilding)		// Klasse serialisierbar machen
	// Standardkonstruktor
	CBuilding(void);
	
	// Destruktor
	virtual ~CBuilding(void);
	
	// Kopierkonstruktor
	CBuilding(const CBuilding & rhs);

	// Zuweisungsoperatur
	CBuilding & operator=(const CBuilding &);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Operatorüberschreibung, der zum Sortieren des Feldes benuzt werden kann
	bool operator< (const CBuilding& elem2) const { return m_iRunningNumber < elem2.m_iRunningNumber;}
	bool operator> (const CBuilding& elem2) const { return m_iRunningNumber > elem2.m_iRunningNumber;}

// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	USHORT GetRunningNumber() const {return m_iRunningNumber;}
	BOOLEAN GetIsBuildingOnline() const {return m_bOnline;}
	
	// zum Schreiben der Membervariablen
	void SetRunningNumber(USHORT RunningNumber) {m_iRunningNumber = RunningNumber;}
	void SetIsBuildingOnline(BOOLEAN TrueOrFalse) {m_bOnline = TrueOrFalse;}

protected:
	// Die laufende Nummer (ID) des Gebäudes
	USHORT m_iRunningNumber;

	// programmiertechnisch benötigte Variablen
	// Ist das Gebäude online?
	BOOLEAN m_bOnline;
};

typedef CArray<CBuilding, CBuilding> BuildingArray;	// Das dynamische Feld wird vereinfacht als BuildingArray angegeben