/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once

// Folgendes Define auskommentieren, wenn es sich um ein fertiges Release handelt!
// (ändert die Pfadangaben für den ganzen Content)
#define DEVELOPMENT_VERSION

#define BOTE_INI_FILE		"BotE.ini"
#define BOTE_LOG_FILE		"BotE.log"
#define BOTE_MANUAL_FILE	"Manual.pdf"
#define BOTE_SAV_FILE1		"auto.sav"
#define BOTE_SAV_FILE2		"auto2.sav"

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
	CString GetIniPath(void) const { return m_sUserDataPath + BOTE_INI_FILE; }

	/// Funktion liefert den kompletten Pfad zur Log-Datei.
	/// @return kompletter Pfad zur Log-Datei
	CString GetLogPath(void) const { return m_sUserDataPath + BOTE_LOG_FILE; }

	/// Funktion liefert den kompletten Pfad zur Autosave-Datei.
	/// @param nRound aktuelle Runde
	/// @return kompletter Pfad zur Autosave-Datei
	CString GetAutoSavePath(int nRound) const { if (nRound%2) return m_sAppPath + BOTE_SAV_FILE2; else return m_sAppPath + BOTE_SAV_FILE1; }

	/// Funktion liefert den kompletten Pfad zum Handbuch.
	/// @return kompletter Pfad zur Handbuch-Datei
	CString GetManualPath(void) const { return m_sAppPath + BOTE_MANUAL_FILE; }

private:
	// Attribute
	CString m_sAppPath;			///< Application Path

	CString m_sUserDataPath;	///< Pfad zum Ordner "Eigene Dateien"

	// private Funktionen	
	/// Funktion ermittelt den Pfad zur BotE.exe Datei.
	/// @return Pfad zur BotE.exe Datei
	CString GetBotEAppPath(void) const;

	/// Funktion ermittelt den Pfad zum Ordner "Eigene Dateien".
	/// @return Pfad zum Ordner "Eigene Dateien"
	CString GetUserDataPath(void) const;
};
