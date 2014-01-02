/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "eventscreen.h"

class CEventBlockade :	public CEventScreen
{
public:
	DECLARE_SERIAL (CEventBlockade)

	/// Standardkonstruktor
	CEventBlockade(void);

	/// Standardkonstruktor mit Parameterübergabe
	CEventBlockade(const CString &sPlayersRaceID, const CString &headline = "", const CString &text = "");

	/// Standarddestruktor
	~CEventBlockade(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	virtual void Create(void);

	// Zeichenfunktionen
	virtual void Draw(Graphics* g, CGraphicPool* graphicPool) const;
};
