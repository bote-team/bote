/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once

class CIniLoader
{
public:
	/// Konstruktor
	CIniLoader(void);

	/// Destruktor
	virtual ~CIniLoader(void);

	/**
	 * Diese Funktion gibt den zum <code>key</code> gehörenden Eintrag aus der ini-Datei zurück.
	 */
	int GetValue(CString key);

	/**
	 * Diese Funktion gibt den zum <code>key</code> gehörenden Eintrag aus der ini-Datei zurück.
	 */
	float GetFloatValue(CString key);

	/**
	 * Diese Funktion gibt den zum <code>key</code> gehörenden Eintrag aus der ini-Datei zurück.
	 */
	CString GetStringValue(CString key);

	/**
	 * Diese Funktion schreibt den zum <code>key</code> gehörenden Eintrag <code>value</code> in die ini-Datei.
	 */
	BOOL WriteIniString(CString key, CString value, CString filename = "BotE.ini");
	
private:
	/**
	 * Diese Funktion liest das ini-File ein und speichert die Daten in der CStringToStringMap. Konnte die ini-
	 * Datei aus irgendeinem Grund nicht geöffnet werden liefert die Funktion <code>FALSE</code> zurück, ansonsten
	 * wird <code>TRUE</code> zurückgegeben.
	 */
	BOOL ReadIniFile(CString filename);
	
	/**
	 * Funktion erstellt die ini-Datei und schreibt einige Standartdaten in die Ini-Datei.
	 */
	void CreateIniFile(CString filename = "BotE.ini");

	/**
	 * Diese Funktion wandelt ein Wort, wie z.B. On oder True in einen Wahrheitswert um.
	 */
	int StringToInt(CString string);

	CMapStringToString m_Strings;		///< Speichert die eingelesenen Zeilen aus der ini Datei
};
