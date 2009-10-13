/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */

#pragma once

#define BOTE_INI_FILE	"BotE.ini"
#define BOTE_LOG_FILE	"BotE.log"
#define BOTE_SAV_FILE	"auto.sav"

/// Klasse beinhaltet alle Input und Output Informationen für BotE.
class CIOData
{
private:

	/// Konstruktor private, damit man sich keine Instanzen holen kann.
	CIOData(void);

	/// Den Kopierkonstruktor schützen um zu vermeiden, dass das Objekt unbeabsichtigt kopiert wird.
    CIOData(const CIOData& cc);

public:
	/// Standarddestruktor
	~CIOData(void);

	/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
	/// @return Instanz dieser Klasse
	static CIOData* GetInstance(void);

	/// Funktion liefert den kompletten Pfad der Anwendung.
	/// @return kompletter Pfad der Anwendung
	CString GetAppPath(void) const { return m_sAppPath; }

	/// Funktion liefert den kompletten Pfad zur Ini-Datei.
	/// @return kompletter Pfad zur Ini-Datei
	CString GetIniPath(void) const { return m_sAppPath + BOTE_INI_FILE; }

	/// Funktion liefert den kompletten Pfad zur Log-Datei.
	/// @return kompletter Pfad zur Log-Datei
	CString GetLogPath(void) const { return m_sAppPath + BOTE_LOG_FILE; }

	/// Funktion liefert den kompletten Pfad zur Autosave-Datei.
	/// @return kompletter Pfad zur Autosave-Datei
	CString GetAutoSavePath(void) const { return m_sAppPath + BOTE_SAV_FILE; }

private:
	// Attribute
	CString m_sAppPath;			///< Application Path
};
