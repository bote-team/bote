/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
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
	bool ReadValue(const CString& sSection, const CString& sKey, bool& bValue) const;


	// Returns bDefault in case the value doesn't exist or other error.
	bool ReadValueDefault(const CString& sSection, const CString& sKey, bool bDefault) const;

	/**
	 * Diese Funktion gibt den zum <code>key</code> gehörenden Eintrag aus der ini-Datei zurück.
	 */
	bool ReadValue(const CString& sSection, const CString& sKey, int& nValue) const;

	// Returns iDefault in case the value doesn't exist or other error.
	int ReadValueDefault(const CString& sSection, const CString& sKey, int iDefault) const;

	/**
	 * Diese Funktion gibt den zum <code>key</code> gehörenden Eintrag aus der ini-Datei zurück.
	 */
	bool ReadValue(const CString& sSection, const CString& sKey, float& fValue) const;

	/**
	 * Diese Funktion gibt den zum <code>key</code> gehörenden Eintrag aus der ini-Datei zurück.
	 */
	bool ReadValue(const CString& sSection, const CString& sKey, CString& sValue) const;

	/**
	 * Diese Funktion schreibt den zum <code>key</code> gehörenden Eintrag <code>value</code> in die ini-Datei.
	 */
	bool WriteValue(const CString& sSection, const CString& sKey, const CString& sValue);

private:
	/**
	 * Diese Funktion wandelt ein Wort, wie z.B. On oder True in einen Wahrheitswert um.
	 */
	static int StringToInt(const CString& sValue);

	/**
	 * Diese Funktion gibt den zum <code>key</code> gehörenden Eintrag aus der ini-Datei zurück.
	 */
	bool ReadIniValue(const CString& sSection, const CString& sKey, CString& sReturnValue) const;

	// Attribute
	CString m_sIniPath;		///< kompletter Pfad zur Ini-Datei
};
