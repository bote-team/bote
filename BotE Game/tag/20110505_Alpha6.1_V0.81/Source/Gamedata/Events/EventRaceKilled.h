/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "Events\EventScreen.h"

class CEventRaceKilled : public CEventScreen
{
public:
	DECLARE_SERIAL (CEventRaceKilled)

	/// Standardkonstruktor
	CEventRaceKilled(void) {};

	/// Standardkonstruktor mit Parameterübergabe
	CEventRaceKilled(const CString& sPlayersRace, const CString& sKilledRace, const CString& sKilledRaceName, const CString& sKilledRaceImage);

	/// Standarddestruktor
	~CEventRaceKilled(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zeichenfunktionen
	virtual void Draw(Graphics* g, CGraphicPool* graphicPool) const;

private:
	// Attribute
	CString	m_sKilledRace;		///< ID der ausgelöschten Rasse
	CString m_sKilledRaceName;	///< Name der ausgelöschten Rasse
	CString m_sKilledRaceImage;	///< Grafikdatei der ausgelöschten Rasse
};