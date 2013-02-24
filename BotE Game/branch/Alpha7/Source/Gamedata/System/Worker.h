/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// Worker.h: Schnittstelle für die Klasse CWorker.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "Constants.h"

class CWorker : public CObject
{
public:
	DECLARE_SERIAL (CWorker)
// Konstruktion & Destruktion
	CWorker();
	virtual ~CWorker();
// Kopierkonstruktor
	CWorker(const CWorker & rhs);
// Zuweisungsoperatur
	CWorker & operator=(const CWorker &);
// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);
// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	USHORT GetWorker(WORKER::Typ nWorker) const;
	// zum Schreiben der Membervariablen
	void SetWorker(WORKER::Typ nWorker, int Value);
	void InkrementWorker(WORKER::Typ nWorker);
	void DekrementWorker(WORKER::Typ nWorker);

// sonstige Funktionen
	void CalculateFreeWorkers();
	void CheckWorkers();	// Fkt. überprüft, ob wir nicht zuviele Arbeiter eingestellt haben

private:
	BYTE m_iFoodWorker;
	BYTE m_iIndustryWorker;
	BYTE m_iEnergyWorker;
	BYTE m_iSecurityWorker;
	BYTE m_iResearchWorker;
	BYTE m_iTitanWorker;
	BYTE m_iDeuteriumWorker;
	BYTE m_iDuraniumWorker;
	BYTE m_iCrystalWorker;
	BYTE m_iIridiumWorker;
	USHORT m_iAllWorkers;
	USHORT m_iFreeWorkers;
};
