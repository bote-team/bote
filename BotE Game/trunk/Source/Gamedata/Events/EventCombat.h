/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "Events/EventScreen.h"

class CEventCombat : public CEventScreen
{
public:
	DECLARE_SERIAL (CEventCombat)

	/// Standardkonstruktor
	CEventCombat(void) {};

	/// Standardkonstruktor mit Parameterübergabe
	CEventCombat(const CString &sPlayersRaceID, const CString &headline = "", const CString &text = "");

	/// Standarddestruktor
	virtual ~CEventCombat(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	void Create(void);

	// Zeichenfunktionen
	void Draw(Graphics* g, CGraphicPool* graphicPool) const;
};
