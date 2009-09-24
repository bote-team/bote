/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// Worker.h: Schnittstelle für die Klasse CWorker.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORKER_H__2E2BC2AE_A921_435B_8FFF_5695C7272F6F__INCLUDED_)
#define AFX_WORKER_H__2E2BC2AE_A921_435B_8FFF_5695C7272F6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef unsigned short USHORT;

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
	USHORT GetWorker(int WhatWorker) const;
	// zum Schreiben der Membervariablen
	void SetWorker(int WhatWorker, int Value);
	void InkrementWorker(int WhatWorker);
	void DekrementWorker(int WhatWorker);

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

#endif // !defined(AFX_WORKER_H__2E2BC2AE_A921_435B_8FFF_5695C7272F6F__INCLUDED_)
