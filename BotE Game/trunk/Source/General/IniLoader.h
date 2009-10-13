/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once

/// Singletonklasse um Daten aus der Ini-Datei zu lesen bzw. zu schreiben.
class CIniLoader
{
private:	
	/// Konstruktor private, damit man sich keine Instanzen holen kann.
    CIniLoader(void);
    
	/// Den Kopierkonstruktor schützen um zu vermeiden, dass das Objekt unbeabsichtigt kopiert wird.
    CIniLoader(const CIniLoader& cc);

public:
	/// Standarddestruktor
	~CIniLoader(void);

	/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
	/// @return Instanz dieser Klasse
	static CIniLoader* GetInstance(void);

	/**
	 * Diese Funktion gibt den zum <code>key</code> gehörenden Eintrag aus der ini-Datei zurück.
	 */
	bool ReadValue(const CString& sSection, const CString& sKey, bool& bValue);

	/**
	 * Diese Funktion gibt den zum <code>key</code> gehörenden Eintrag aus der ini-Datei zurück.
	 */
	bool ReadValue(const CString& sSection, const CString& sKey, int& nValue);

	/**
	 * Diese Funktion gibt den zum <code>key</code> gehörenden Eintrag aus der ini-Datei zurück.
	 */
	bool ReadValue(const CString& sSection, const CString& sKey, float& fValue);

	/**
	 * Diese Funktion gibt den zum <code>key</code> gehörenden Eintrag aus der ini-Datei zurück.
	 */
	bool ReadValue(const CString& sSection, const CString& sKey, CString& sValue);

	/**
	 * Diese Funktion schreibt den zum <code>key</code> gehörenden Eintrag <code>value</code> in die ini-Datei.
	 */
	bool WriteValue(const CString& sSection, const CString& sKey, const CString& sValue);

private:
	/**
	 * Diese Funktion wandelt ein Wort, wie z.B. On oder True in einen Wahrheitswert um.
	 */
	int StringToInt(CString sValue);	

	/**
	 * Diese Funktion gibt den zum <code>key</code> gehörenden Eintrag aus der ini-Datei zurück.
	 */
	bool ReadIniValue(const CString& sSection, const CString& sKey, CString& sReturnValue);

	// Attribute
	CString m_sIniPath;		///< kompletter Pfad zur Ini-Datei
};
