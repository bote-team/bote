/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */

#pragma once
#include "eventscreen.h"

class CEventBombardment : public CEventScreen
{
public:
	DECLARE_SERIAL (CEventBombardment)

	/// Standardkonstruktor
	CEventBombardment(void) {};

	/// Standardkonstruktor mit Parameterübergabe
	CEventBombardment(BYTE playersRace, const CString &imageName, const CString &headline = "", const CString &text = "");

	/// Standarddestruktor
	~CEventBombardment(void);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	void Create(void);

	// Zeichenfunktionen
	void Draw(Graphics* g, CGraphicPool* graphicPool) const;
};
