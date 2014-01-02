/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "eventscreen.h"

class CEventColonization : public CEventScreen
{
public:
	DECLARE_SERIAL (CEventColonization)

	/// Standardkonstruktor
	CEventColonization(void);

	/// Standardkonstruktor mit Parameterübergabe
	CEventColonization(const CString& sPlayersRaceID, const CString &headline = "", const CString &text = "");

	/// Standarddestruktor
	~CEventColonization(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	virtual void Create(void);

	// Zeichenfunktionen
	virtual void Draw(Graphics* g, CGraphicPool* graphicPool) const;
};
