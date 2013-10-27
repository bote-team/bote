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

#include <map>

class CWorker
{
public:
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
	int GetWorker(WORKER::Typ nWorker) const;

	// zum Schreiben der Membervariablen
	void SetWorker(WORKER::Typ nWorker, int Value);
	void InkrementWorker(WORKER::Typ nWorker);
	void DekrementWorker(WORKER::Typ nWorker);

// sonstige Funktionen

private:
	struct EmployedFreeAll
	{
		EmployedFreeAll(int _employed, int _free, int _all) :
			employed(_employed),
			free(_free),
			all(_all)
		{
		}
		int employed;
		int free;
		int all;
	};
public:
	CWorker::EmployedFreeAll CalculateFreeWorkers();
	void CheckWorkers();	// Fkt. überprüft, ob wir nicht zuviele Arbeiter eingestellt haben
	void FreeAll();
	int Cap(WORKER::Typ type, int number);

private:
	std::map<WORKER::Typ, int> m_Workers;

	typedef std::map<WORKER::Typ, int> ::const_iterator const_iterator;
	typedef std::map<WORKER::Typ, int> ::iterator iterator;

	EmployedFreeAll Workers() const;


};
