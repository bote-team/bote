/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "eventscreen.h"

class CEventBombardment : public CEventScreen
{
public:
	DECLARE_SERIAL (CEventBombardment)

	/// Standardkonstruktor
	CEventBombardment(void);

	/// Standardkonstruktor mit Parameterübergabe
	CEventBombardment(const CString& sPlayersRace, const CString &imageName, const CString &headline = "", const CString &text = "");

	/// Standarddestruktor
	~CEventBombardment(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	virtual void Create(void);

	// Zeichenfunktionen
	virtual void Draw(Graphics* g, CGraphicPool* graphicPool) const;
};
