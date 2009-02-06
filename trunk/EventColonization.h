/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */

#pragma once
#include "eventscreen.h"

class CEventColonization : public CEventScreen
{
public:
	DECLARE_SERIAL (CEventColonization)

	/// Standardkonstruktor
	CEventColonization(void) {};

	/// Standardkonstruktor mit Parameterübergabe
	CEventColonization(BYTE playersRace, const CString &headline = "", const CString &text = "");
	
	/// Standarddestruktor
	~CEventColonization(void);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	void Create(void);

	// Zeichenfunktionen
	void Draw(Graphics* g, CGraphicPool* graphicPool) const;
};
