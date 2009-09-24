/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */

#pragma once
#include "eventscreen.h"

class CEventBlockade :	public CEventScreen
{
public:
	DECLARE_SERIAL (CEventBlockade)

	/// Standardkonstruktor
	CEventBlockade(void) {};

	/// Standardkonstruktor mit Parameterübergabe
	CEventBlockade(const CString &sPlayersRaceID, const CString &headline = "", const CString &text = "");

	/// Standarddestruktor
	~CEventBlockade(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	void Create(void);

	// Zeichenfunktionen
	void Draw(Graphics* g, CGraphicPool* graphicPool) const;
};
