/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
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

	/// Funktion initialisiert alle möglichen Systemnamen.
	/// @pram vMinorRaceSystemNames Namen aller Minorracesysteme
	void Init(const CStringArray& vMinorRaceSystemNames);

	/// Diese Funktion gibt uns einen einzigartigen Systemnamen zurück. Dieser Name wurde vorher noch nicht vergeben.
	/// @param ptKO Sektorkoordinate
	/// @param [in|out] bMinor handelt sich um ein Minorracesystem, wenn keine Minorracesysteme mehr vorhanden sind, dann wird der Parameter auf <code>false</code> gesetzt
	/// @return generierter Sektorname
	CString GetNextRandomSectorName(const CPoint& ptKO, bool& bMinor);

private:
	// Funtkionen
	/// Funktion liest die Systemnames aus den externen Datafiles ein
	void ReadSectorNames(void);

	// Attribute
	CStringArray m_strNames;		///< Feld aller Namen der Planetensysteme ohne Minor- und Majorracesysteme
	CStringArray m_strRaceNames;	///< Feld aller Namen der Minorracesysteme
};
