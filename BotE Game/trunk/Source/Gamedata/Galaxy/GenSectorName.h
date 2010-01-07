/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */

#pragma once

// Klasse zum Einlesen aller möglichen Sektornamen.
class CGenSectorName
{
private:
	/// Konstruktor private, damit man sich keine Instanzen holen kann.
	CGenSectorName(void);

	/// Den Kopierkonstruktor schützen um zu vermeiden, dass das Objekt unbeabsichtigt kopiert wird.
    CGenSectorName(const CGenSectorName& cc);

public:
	/// Destruktor
	virtual ~CGenSectorName(void);

	/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
	/// @return Instanz dieser Klasse
	static CGenSectorName* GetInstance(void);
	
	/// Diese Funktion gibt uns einen einzigartigen Systemnamen zurück. Dieser Name wurde vorher noch nicht vergeben.
	/// Als Parameter wird nur ein Wert übergeben, der der Funktion sagt ob es sich um ein Minorracesystem handelt
	/// oder nicht.	
	CString GetNextRandomSectorName(bool bMinor = false);

private:
	// Funtkionen
	/// Funktion liest die Systemnames aus den externen Datafiles ein
	void ReadSystemNames(void);

	// Attribute
	CArray<CString> m_strName;		///< Feld aller Namen der Planetensysteme ohne Minor- und Majorracesysteme
	CArray<CString> m_strRaceName;	///< Feld aller Namen der Minorracesysteme
};