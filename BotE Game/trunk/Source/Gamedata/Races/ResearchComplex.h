/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// ResearchComplex.h: Schnittstelle für die Klasse CResearchComplex.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESEARCHCOMPLEX_H__FA981EBD_61BB_4B24_9362_756715C0CEEC__INCLUDED_)
#define AFX_RESEARCHCOMPLEX_H__FA981EBD_61BB_4B24_9362_756715C0CEEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Options.h"

class CResearchComplex : public CObject  
{
	friend class CResearchInfo;
public:
	DECLARE_SERIAL (CResearchComplex)
	/// Konstruktor
	CResearchComplex();

	/// Destruktor
	virtual ~CResearchComplex();

	/// Kopierkonstruktor
	CResearchComplex(const CResearchComplex & rhs);

	/// Zuweisungsoperator
	CResearchComplex & operator=(const CResearchComplex &);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	/**
	 * Diese Funktion gibt den Status des Komplexes zurück. Der Status des Komplex kann entweder
	 * RESEARCHED, NOTRESEARCHED oder RESEARCHING sein.
	 */
	BYTE GetComplexStatus(void) const {return m_byComplexStatus;}
	
	/**
	 * Diese Funktion gibt den Status eines speziellen Gebietes in dem Komplex zurück. Der Status des Gebietes
	 * kann entweder RESEARCHED, NOTRESEARCHED oder RESEARCHING sein. Als Paramter erwartet die Funktion das
	 * gewünschte Gebiet <code>field</code>. Dieses beginnt bei 1 und nicht bei 0 (maximal 3)!
	 */
	BYTE GetFieldStatus(BYTE field) const {return m_byFieldStatus[field-1];}

	/**
	 * Diese Funktion gibt den Namen (die Bezeichnung) des Komplexes zurück.
	 */
	const CString& GetComplexName(void) const {return m_strComplexName;}
	
	/**
	 * Diese Funktion gibt die Beschreibung des Komplex zurück.
	 */
	const CString& GetComplexDescription(void) const {return m_strComplexDescription;}

	/**
	 * Diese Funktion gibt den Namen eines speziellen Gebietes in dem Komplex zurück. Als Paramter erwartet die
	 * Funktion das gewünschte Gebiet <code>field</code>. Dieses beginnt bei 1 und nicht bei 0 (maximal 3)!
	 */
	const CString& GetFieldName(BYTE field) const {return m_strFieldName[field-1];}

	/**
	 * Diese Funktion gibt die Beschreibung eines speziellen Gebietes in dem Komplex zurück. Als Paramter erwartet die
	 * Funktion das gewünschte Gebiet <code>field</code>. Dieses beginnt bei 1 und nicht bei 0 (maximal 3)!
	 */
	const CString& GetFieldDescription(BYTE field) const {return m_strFieldDescription[field-1];}
	
	/**
	 * Diese Funktion gibt den Bonus eines speziellen Gebietes in dem Komplex zurück. Als Paramter erwartet die
	 * Funktion das gewünschte Gebiet <code>field</code>. Dieses beginnt bei 1 und nicht bei 0 (maximal 3)!
	 */
	short GetBonus(BYTE field) const {return m_iBonus[field-1];}

	// sonstige Funktionen
	/**
	 * Diese Funktion generiert einen Komplex, setzt also erst alle Attribute des Objektes. Als einziger Parameter
	 * muss dafür eine Komplexnumme <code>complex</code> übergeben werden.
	 */
	void GenerateComplex(USHORT complex);
	
	/**
	 * Diese Funktion setzt alle Variablen des CResearchComplex-Objektes wieder auf ihre Ausgangswerte
	 */
	void Reset(void);

private:
	/**
	 * Diese private Funktion liest die Beschreibungen zu den Spezialforschungen aus einer Datei ein und speichert sie
	 * auf die entsprechenden Variablen.
	 */
	void ReadSpecialTech(BYTE complex);

	/// Status des Komplexes (erforscht, nicht erforscht, gerade am erforschen).
	BYTE m_byComplexStatus;
		
	/// Der Status der einzelnen Wahlgebiete des Komplexes (erforscht, nicht erforscht, gerade am erforschen).
	BYTE m_byFieldStatus[3];
	
	/// Der Name (Bezeichnung) des Komplexes (z.B. Schiffstechnik offensiv oder Wirtschaft...).
	CString m_strComplexName;
	
	/// Die Beschreibung des Komplexes.
	CString m_strComplexDescription;

	/// Der Name eines einzelnen Gebietes in dem Komplex.
	CString m_strFieldName[3];

	/// Die Beschreibung eines einzelnen Gebietes in dem Komplex.
	CString m_strFieldDescription[3];

	/// Den einzelnen Bonus, den das jeweilige Gebiet macht.
	short m_iBonus[3];
};

#endif // !defined(AFX_RESEARCHCOMPLEX_H__FA981EBD_61BB_4B24_9362_756715C0CEEC__INCLUDED_)
