/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once

#include "fmod.hpp"
#include "network.h"

#include <list>
#include <string>

/**
 * Konstanten für Hintergrundmusik und Geräusche, die die Oberfläche erzeugen kann.
 */
typedef enum SNDMGR_VALUE
{
	SNDMGR_INVALID,								///< ungültiger Wert

	SNDMGR_MUSIC_FED,							///< Hintergrundmusik Föderation
	SNDMGR_MUSIC_FER,							///< Hintergrundmusik Ferengi
	SNDMGR_MUSIC_KLI,							///< Hintergrundmusik Klingonen
	SNDMGR_MUSIC_ROM,							///< Hintergrundmusik Romulaner
	SNDMGR_MUSIC_CAR,							///< Hintergrundmusik Cardassianer
	SNDMGR_MUSIC_DOM,							///< Hintergrundmusik Dominion

	SNDMGR_VOICE_FED_RACESELECT,				///< Stimme zur Rassenauswahl für Föderation
	SNDMGR_VOICE_FER_RACESELECT,				///< Stimme zur Rassenauswahl für Ferengi
	SNDMGR_VOICE_KLI_RACESELECT,				///< Stimme zur Rassenauswahl für Klingonen
	SNDMGR_VOICE_ROM_RACESELECT,				///< Stimme zur Rassenauswahl für Romulaner
	SNDMGR_VOICE_CAR_RACESELECT,				///< Stimme zur Rassenauswahl für Cardassianer
	SNDMGR_VOICE_DOM_RACESELECT,				///< Stimme zur Rassenauswahl für Dominion

	SNDMGR_SOUND_MAINMENU,						///< Button im Hauptmenü
	SNDMGR_SOUND_ENDOFROUND,					///< Rundenende-Button
	SNDMGR_SOUND_SHIPTARGET,					///< Ziel für Schiff auswählen
	SNDMGR_SOUND_ERROR,							///< Fehlerpiepsen

	SNDMGR_MSG_BATTLELOST,
	
	SNDMGR_MSG_ALIENCONTACT,					///< Alienkontakt wurde hergestellt (Minorrace getroffen)
	SNDMGR_MSG_CLAIMSYSTEM,						///< neue Kolonie / neues System
	SNDMGR_MSG_COLONIZING,						///< Kolonisierung gestartet
	SNDMGR_MSG_DIPLOMATICNEWS,					///< neue diplomatische Nachrichten eingetroffen
	SNDMGR_MSG_FIRSTCONTACT,					///< Erstkontakt mit einer Majorrace
	SNDMGR_MSG_INTELNEWS,						///< neue Geheimdienstnachrichten sind eingetroffen
	SNDMGR_MSG_NEWTECHNOLOGY,					///< neue Technologie erforscht
	SNDMGR_MSG_OUTPOST_CONSTRUCT,				///< ein Aussenposten wird gebaut
	SNDMGR_MSG_OUTPOST_READY,					///< Bau eines Aussenpostens abgeschlossen
	SNDMGR_MSG_SCIENTISTNEWS,					///< technologischer Durchbruch
	SNDMGR_MSG_STARBASE_CONSTRUCT,				///< eine Sternbasis wird gebaut
	SNDMGR_MSG_STARBASE_READY,					///< Bau einer Sternbasis abgeschlossen
	SNDMGR_MSG_TERRAFORM_COMPLETE,				///< Terraforming ist fertig
	SNDMGR_MSG_TERRAFORM_SELECT,				///< Planet zum Terraformen auswählen
	
	// TODO ergänzen

}
SNDMGR_VALUE;

/**
 * Prioritäten für Geräusche.
 */
typedef enum
{
	SNDMGR_PRIO_NORMAL,							///< normale Priorität
	SNDMGR_PRIO_HIGH,							///< hohe Priorität, aber niedriger als die Hintergrundmusik
}
SNDMGR_PRIO;

/**
 * Typ eines Eintrags in <code>SNDMGR_SOUNDLIST</code>.
 */
typedef struct SNDMGR_SOUNDENTRY
{
	SNDMGR_VALUE nType;							///< <code>SNGMGR_SOUND</code>-Konstante
	FMOD::Sound *pSound;						///< geladenes <code>FMOD::Sound</code>-Objekt
}
SNDMGR_SOUNDENTRY;

/**
 * Typ für <code>CSoundManager::m_lSounds</code>.
 */
typedef std::list<SNDMGR_SOUNDENTRY> SNDMGR_SOUNDLIST;

/**
 * Eintrag in <code>SNDMGR_MESSAGELIST</code>.
 */
typedef struct SNDMGR_MESSAGEENTRY
{
	SNDMGR_VALUE nMessage;						///< Typ der Nachricht
	network::RACE nRace;						///< "Stimme", mit der die Nachricht abgespielt wird
	int nPriority;								///< Priorität (0 Standard/geringste)
	float fVolume;								///< Lautstärke, mit der abgespielt werden soll
}
SNDMGR_MESSAGEENTRY;

/**
 * Typ für <code>CSoundManager::m_lMessages</code>.
 */
typedef std::list<SNDMGR_MESSAGEENTRY> SNDMGR_MESSAGELIST;

/**
 * Verwaltet das Abspielen von Hintergrundmusik und Vordergrundgeräuschen.
 *
 * @author CBot
 * @version 0.0.2
 */
class CSoundManager
{
private:
	static const int PRIORITY_SOUND_HIGH = 127;		///< hohe Priorität der Geräusche, die die Oberfläche erzeugt
	static const int PRIORITY_SOUND_NORMAL = 128;	///< normale Priorität der Geräusche, die die Oberfläche erzeugt

	FMOD_RESULT m_nLastResult;						///< letzter Rückgabewert einer FMOD-Funktion
	FMOD::System *m_pSystem;

	FMOD::Channel *m_pMusicChannel;					///< Kanal, in dem die Hintergrundmusik abgespielt wird
	FMOD::Sound *m_pMusic;							///< Hintergrundmusik

	/// Liste der geladenen Sounds; das zuletzt verwendete Objekt wird ans Ende der Liste gesetzt; der erste
	/// Eintrag entspricht demjenigen Sound, dessen letzte Verwendung am längsten her ist
	SNDMGR_SOUNDLIST m_lSounds;
	UINT m_nMaxLoadedSounds;						///< maximale Anzahl gleichzeitig geladener Sounds
	FMOD::Channel *m_pSoundChannel;					///< Kanal, in dem die Sounds abgespielt werden
	float m_fSoundMasterVolume;						///< Master-Lautstärke für Sounds

	SNDMGR_MESSAGELIST m_lMessages;					///< Liste der noch abzuspielenden Nachrichten
	FMOD::Channel *m_pMessageChannel;				///< Kanal, in dem Nachrichten abgespielt werden
	FMOD::Sound *m_pLastMessage;					///< aktuell abzuspielende Nachricht
	bool m_bMessagesPlaying;						///< <code>true</code> gdw. Messages gerade abgespielt werden
	bool m_bInterruptMessages;						///< soll Abspielen der Nachrichten unterbrochen werden?
	DWORD m_dwFirstMillis;							///< Pause vor dem Abspielen der ersten Nachricht (Millisekunden)
	DWORD m_dwMillis;								///< Zeit zwischen zwei Nachrichten (Millisekunden)
	HANDLE m_hThreadStopped;						///< Event, das ausgelöst wird, wenn der Abspielthread beendet ist
	float m_fMessageMasterVolume;					///< Master-Lautstärke für Nachrichten

	BOOL m_bUseSoftwareSound;						///< Soundausgabe hardwarebeschleunigt oder per Software

public:
	/**
	 * @param nMaxLoadedSound maximale Anzahl gleichzeitig geladener Sounds
	 */
	CSoundManager(UINT nMaxLoadedSounds = 10);

	/**
	 * Hält den Thread an, gibt sämtliche Ressourcen frei.
	 */
	virtual ~CSoundManager();

	/**
	 * Setzt die maximale Anzahl gleichzeitig zu ladender Oberflächen-Sounds. Wird dieser Wert auf einen
	 * kleineren als bisher gesetzt, wird die entsprechende Anzahl geladener Sounds beim nächsten Aufruf
	 * von <code>PlaySound()</code> freigegeben.
	 */
	void SetMaxLoadedSounds(UINT nMaxLoadedSounds) {m_nMaxLoadedSounds = nMaxLoadedSounds;}
	UINT GetMaxLoadedSounds() const {return m_nMaxLoadedSounds;}

	/// @return Anzahl der gleichzeitig geladenen Sounds
	UINT GetLoadedSounds() const {return (UINT)m_lSounds.size();}

	/**
	 * Gibt alle geladenen Oberflächen-Sounds frei. Wird einer der Sounds abgespielt, so wird das
	 * Abspielen durch das Freigeben unterbrochen.
	 */
	void UnloadAllSounds();

	/**
	 * Liefert den zuletzt aufgetretenen Fehler, wenn eine Methode von <code>CSoundManager</code>
	 * den Wert <code>FALSE</code> bwz. <code>NULL</code> geliefert hat. Sonst ist der gelieferte Wert
	 * undefiniert.
	 */
	FMOD_RESULT GetLastError() const {return m_nLastResult;}

	/**
	 * Initialisiert das Sound-System.
	 * @return <code>TRUE</code> genau bei Erfolg
	 */
	BOOL Init(BOOL bUseSoftwareSound = FALSE);

	/**
	 * Gibt das Sound-System und alle Ressourcen wieder frei.
	 *
	 * Das Sound-System kann mit <code>Init()</code> anschließend wieder initialisiert werden.
	 */
	void Release();

	/**
	 * Startet das Abspielen der angegebenen Datei als Hintergrundmusik. Bisher abgespielte Hintergrundmusik
	 * wird unterbrochen und freigegeben.
	 *
	 * @return <code>TRUE</code> genau bei Erfolg
	 */
	BOOL StartMusic(network::RACE race, float fVolume = 1.0f);

	/**
	 * Hält die Hintergrundmusik an und gibt sie frei.
	 */
	void StopMusic();

	/**
	 * Blendet die abgespielte Hintergrundmusik aus. Dafür wird die Lautstärke in <code>nSteps</code> Schritten
	 * jeweils nach <code>dwMilliseconds</code> Millisekunden um <code>fVolume / nSteps</code> gesenkt,
	 * wobei <code>fVolume</code> die gesetzte Lautstärke der Hintergrundmusik beim Aufruf dieser Methode ist. Bei
	 * <code>nSteps == 0</code> wird die Lautstärke ohne zu Warten sofort auf <code>0</code> gesetzt.
	 *
	 * Diese Methode kehrt erst zurück, wenn die Hintergrundmusik die Lautstärke <code>0</code> erreicht hat.
	 */
	void FadeOutMusic(UINT nSteps = 20, DWORD dwMilliseconds = 100);

	/**
	 * Setzt die Lautstärke der Hintergrundmusik, wenn diese läuft.
	 */
	void SetMusicVolume(float fVolume);

	/**
	 * Hält die Hintergrundmusik an bzw. setzt sie fort.
	 */
	void PauseMusic(BOOL bPaused = TRUE);

	/**
	 * Spielt den angegebenen Oberflächen-Sound ab. Wird bereits ein Sound abgespielt, wird dieses unterbrochen.
	 * @return <code>TRUE</code> genau bei Erfolg
	 */
	BOOL PlaySound(SNDMGR_VALUE nSound, SNDMGR_PRIO nPriority = SNDMGR_PRIO_NORMAL, float fVolume = 1.0f, network::RACE nRace = network::RACE_NONE);

	/**
	 * Setzt die Master-Lautstärke für Oberflächen-Sounds. Ein Wert von <code>0</code> schaltet die Sound-
	 * Ausgabe ab.
	 */
	void SetSoundMasterVolume(float fSoundMasterVolume);

	/**
	 * Hängt an die Liste der abzuspielenden Nachrichten die gegebene Nachricht an. Diese wird später mit der
	 * "Stimme" des angegebenen Volkes abgespielt. Es kann eine Priotität angegeben werden. Nachrichten mit
	 * größeren Werten von <code>nPriority</code> werden eher abgespielt.
	 */
	void AddMessage(SNDMGR_VALUE nMsg, network::RACE nRace = network::RACE_NONE, int nPriority = 0,
		float fVolume = 1.0f);

	/**
	 * Leert die Liste der Nachrichten.
	 */
	void ClearMessages();

	/**
	 * Setzt die Master-Lautstärke für Nachrichten. Ein Wert von <code>0</code> schaltet die Wiedergabe ab.
	 */
	void SetMessageMasterVolume(float fMessageMasterVolume);

	/**
	 * Sortiert die Liste der Nachrichten nach Priorität und spielt diese der Reihe nach ab. Vor der ersten
	 * Nachricht wird <code>dwFirstMillis</code> Millisekunden gewartet, zwischen zwei Nachrichten wird
	 * <code>dwMillis</code> Millisekunden pausiert.
	 */
	BOOL PlayMessages(DWORD dwFirstMillis = 0, DWORD dwMillis = 0);

	/**
	 * Unterbricht das Abspielen der Nachrichten. Für <code>bHardInterrupt = FALSE</code> wird eine begonnene
	 * Nachricht bis zum Ende abgespielt, sonst wird diese sofort unterbrochen. Ein Aufruf von
	 * <code>PlayMessages()</code> startet das Abspielen wieder und setzt mit der nächsten Nachricht fort.
	 *
	 * Im Fall des sofortigen Unterbrechens kehrt die Methode erst zurück, wenn der Abspielthread beendet wurde
	 * oder der angegebene Timeout abgelaufen ist.
	 *
	 * @return beim normalen Unterbrechen immer <code>TRUE</code>; beim sofortigen Unterbrechen <code>TRUE</code>,
	 * wenn der Thread beendet wurde, bevor der Timeout abgelaufen ist
	 */
	BOOL StopMessages(BOOL bHardInterrupt = FALSE, DWORD dwTimeout = INFINITE);

private:
	/**
	 * Liefert den Zeiger auf das zum angegebenen Sound gehörige <code>FMOD::Sound</code>-Objekt. Es werden maximal
	 * <code>CSoundManager::m_nMaxLoadedSounds</code> verschiedene Sounds gleichzeitig geladen. Muss bei maximaler
	 * Auslastung ein weiterer Sound geladen werden, wird der Sound, dessen letztes Abspielen am längsten zurück liegt,
	 * entfernt.
	 * @return geladenes Sound-Objekt bei Erfolg; sonst <code>NULL</code>, <code>CSoundManager::GetLastError()</code>
	 * liefert dann den Fehler
	 */
	FMOD::Sound *LoadSound(SNDMGR_VALUE nSound, network::RACE nRace = network::RACE_NONE);

	/**
	 * @return den zum angegebenen Sound gehörigen Dateinamen
	 */
	static std::string GetPathName(SNDMGR_VALUE nSound, network::RACE nRace = network::RACE_NONE);

	/**
	 * @return die dem angegebenen Volk zugeordnete Hintergrundmusik als
	 * <code>SNDMGR_VALUE</code>-Konstante; <code>SNDMGR_INVALID</code> wenn Volk ungültig oder keine
	 * Zuordnung angegeben ist
	 */
	static SNDMGR_VALUE GetRaceMusic(network::RACE race);

	static UINT ThreadProc(LPVOID pParam);

	/**
	 * Entfernt die nächste Nachricht aus der Liste und spielt diese ab.
	 */
	BOOL PlayNextMessage();

	static bool CompareMessageEntries(const SNDMGR_MESSAGEENTRY &entry1, const SNDMGR_MESSAGEENTRY &entry2);

};
