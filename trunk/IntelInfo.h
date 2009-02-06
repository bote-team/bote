/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "afx.h"
#include "Options.h"

/**
 * Diese Klasse beinhaltet und ermittelt alle Möglichen Informationen, welche in der Informationsansicht des
 * Geheimdienstmenüs angezeigt werden können. Sie stellt geheimdienstliche Informationen zu allen anderen dem
 * Imperium bekannten Rassen bereit, sofern diese ermittelt wurden. Dieses Objekt braucht nicht serialisiert zu
 * werden, da die Geheimdienstinformationen auf Clientseite nur bei Bedarf berechnet werden.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */

class CBotf2Doc;
class CIntelInfo
{
public:
	/// Standardkonstruktor
	CIntelInfo(void);

	/// Destruktor
	~CIntelInfo(void);

	// Zugriffsfunktionen
	/// Funktion gibt die Anzahl der kontrollierten Sektoren zurück.
	/// @param race Rasse
	short GetControlledSectors(BYTE race) const {ASSERT(race); return m_nControlledSectors[race-1];}

	/// Funktion gibt die Anzahl der besitzten Systeme zurück.
	/// @param race Rasse
	short GetOwnedSystems(BYTE race) const {ASSERT(race); return m_nOwnedSystems[race-1];}

	/// Funktion gibt die Anzahl der bewohnten Systeme zurück.
	/// @param race Rasse
	short GetInhabitedSystems(BYTE race) const {ASSERT(race); return m_nInhabitedSystems[race-1];}

	/// Funktion gibt die Anzahl der bekannten Minorraces zurück.
	/// @param race Rasse
	short GetKnownMinors(BYTE race) const {ASSERT(race); return m_nKnownMinors[race-1];}
	
	/// Funktion gibt die Anzahl der Minorracemitglieder zurück.
	/// @param race Rasse
	short GetMinorMembers(BYTE race) const {ASSERT(race); return m_nMinorMembers[race-1];}

	// sonstige Funktionen
	/// Funktion ermittelt alle Geheimdienstinformationen für den Besitzer dieses Objektes.
	/// @param pDoc Zeiger auf das Dokument
	/// @param ourRace Besitzer dieses Objektes
	void CalcIntelInfo(CBotf2Doc* pDoc, BYTE ourRace);

	/// Resetfunktion für das CIntelInfo Objekt.
	void Reset();

private:
	// Attribute
	short m_nControlledSectors[DOMINION];			///< Anzahl der kontrollierten Sektoren

	short m_nOwnedSystems[DOMINION];				///< Anzahl besitzte Systeme, auch unbewohnte

	short m_nInhabitedSystems[DOMINION];			///< Anzahl bewohnter Systeme

	short m_nKnownMinors[DOMINION];					///< Anzahl bekannter Minorraces

	short m_nMinorMembers[DOMINION];				///< Anzahl der Minorracemitglieder

	BOOLEAN m_bCalculated;							///< wurden die Information in der aktuellen Runde schon berechnet
};
