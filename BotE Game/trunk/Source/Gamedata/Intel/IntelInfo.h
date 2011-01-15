/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "Options.h"
#include <map>

using namespace std;

// forward declaration
class CBotf2Doc;
class CMajor;

/**
 * Diese Klasse beinhaltet und ermittelt alle Möglichen Informationen, welche in der Informationsansicht des
 * Geheimdienstmenüs angezeigt werden können. Sie stellt geheimdienstliche Informationen zu allen anderen dem
 * Imperium bekannten Rassen bereit, sofern diese ermittelt wurden. Dieses Objekt braucht nicht serialisiert zu
 * werden, da die Geheimdienstinformationen auf Clientseite nur bei Bedarf berechnet werden.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */
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
	short GetControlledSectors(const CString& sRace) {return m_nControlledSectors[sRace];}

	/// Funktion gibt die Anzahl der besitzten Systeme zurück.
	/// @param race Rasse
	short GetOwnedSystems(const CString& sRace) {return m_nOwnedSystems[sRace];}

	/// Funktion gibt die Anzahl der bewohnten Systeme zurück.
	/// @param race Rasse
	short GetInhabitedSystems(const CString& sRace) {return m_nInhabitedSystems[sRace];}

	/// Funktion gibt die Anzahl der bekannten Minorraces zurück.
	/// @param race Rasse
	short GetKnownMinors(const CString& sRace) {return m_nKnownMinors[sRace];}
	
	/// Funktion gibt die Anzahl der Minorracemitglieder zurück.
	/// @param race Rasse
	short GetMinorMembers(const CString& sRace) {return m_nMinorMembers[sRace];}

	// sonstige Funktionen
	/// Funktion ermittelt alle Geheimdienstinformationen für den Besitzer dieses Objektes.
	/// @param pDoc Zeiger auf das Dokument
	/// @param pOurRace Besitzer dieses Objektes
	void CalcIntelInfo(CBotf2Doc* pDoc, CMajor* pOurRace);

	/// Resetfunktion für das CIntelInfo Objekt.
	void Reset();

private:
	// Attribute
	map<CString, short> m_nControlledSectors;		///< Anzahl der kontrollierten Sektoren

	map<CString, short> m_nOwnedSystems;			///< Anzahl besitzte Systeme, auch unbewohnte

	map<CString, short> m_nInhabitedSystems;		///< Anzahl bewohnter Systeme

	map<CString, short> m_nKnownMinors;				///< Anzahl bekannter Minorraces

	map<CString, short> m_nMinorMembers;			///< Anzahl der Minorracemitglieder

	BOOLEAN m_bCalculated;							///< wurden die Information in der aktuellen Runde schon berechnet
};
