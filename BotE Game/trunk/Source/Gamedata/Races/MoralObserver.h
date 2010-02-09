/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "Options.h"
#include "System\System.h"

#define EVENTNUMBER 61

class CMoralObserver : public CObject
{
public:
	DECLARE_SERIAL (CMoralObserver)

	/// Konstruktor
	CMoralObserver(void);

	/// Destruktor
	~CMoralObserver(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	/// Serialisierungsfunktion für statische Variable
	static void SerializeStatics(CArchive &ar);

	/**
	 * Funktion fügt das vorgekommene Ereignis dem MoralObserver-Objekt hinzu, so dass es später
	 * bei der Moralberechnung mit beachtet werden kann. Als Parameter wird dabei die Nummer des
	 * <code>event</code> und der jeweiligen Majorrace <code>major</code> übergeben. Welche Nummer
	 * zu welchem Event gehört, kann in der Funktion <code>CalculateEvents</code> nachgelesen werde.
	 * Als Rückgabewert gibt die Funktion den jeweiligen Text zurück, der in der Ereignisübersicht
	 * angezeigt werden kann. Optional muss auch ein Textstring <code>param</code> übergeben werden,
	 * der in bestimmte Nachrichten eingebaut wird.
	 */
	CString AddEvent(unsigned short Event, BYTE major, CString param = "") {
		m_iEvents.Add(Event); return GenerateText(Event, major, param);
	}
	/**
	 * Funktion berechnet die Moralauswirkung auf alle Systeme, abhängig von den vorgekommenen Events und
	 * der jeweiligen Majorrace. Übergeben werden dafür alle Systeme <code>systems</code>, die RaceID und die
	 * gemappte Nummer der Majorrace <code>byMappedRaceNumber</code>, auf welche Moralwerte sich bezogen werden soll.
	 */
	void CalculateEvents(CSystem systems[][STARMAP_SECTORS_VCOUNT], const CString& sRaceID, BYTE byMappedRaceNumber);

	/**
	 * Funktion erstellt einen Text, der in der Ereignisansicht angezeigt werden kann, aufgrund eines speziellen
	 * Events. Dieser Text ist rassenabhängig und wird von der Funktion zurückgegeben. Als Parameter werden an diese
	 * Funktion das Event <code>event</code> und die Nummer der Majorrace <code>major<code> übergeben. Optional muss
	 * auch ein Textstring <code>param</code> übergeben werden, der in bestimmte Nachrichten eingebaut wird.
	 */
	CString GenerateText(unsigned short Event, BYTE major, CString param = "");

	/**
	 * Funktion lädt die zu Beginn die ganzen Moralwerte für alle verschiedenen Ereignisse in eine Moralmatrix.
	 */
	static void InitMoralMatrix();

	/// Funktion gibt einen bestimmten Moralwert zurück.
	/// @param byMappedRaceNumber gemappte Rassennummer
	/// @param Event Eventnummer
	/// @return Moralwert
	static short GetMoralValue(BYTE byMappedRaceNumber, unsigned short Event);

private:
	/// Das Feld, in dem alle Vertragsannahmen stehen.
	CArray<short,short> m_iEvents;

	/// Matrix in der die Moralwerte für die unterschiedlichen Ereignisse abgelegt werden.
	static short m_iMoralMatrix[EVENTNUMBER][DOMINION];

	/// Matrix in der die Textnachrichten für die unterschiedlichen Ereignisse abgelegt werden.
	static CString m_strTextMatrix[EVENTNUMBER][DOMINION];
};
