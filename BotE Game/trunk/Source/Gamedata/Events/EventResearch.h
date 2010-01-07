/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */

#pragma once
#include "Events\eventscreen.h"

class CEventResearch : public CEventScreen
{
public:
	DECLARE_SERIAL (CEventResearch)

	/// Standardkonstruktor
	CEventResearch(void);

	/// Standardkonstruktor mit Parameterübergabe
	CEventResearch(const CString &sPlayersRaceID, const CString &headline, BYTE byTech);

	/// Standarddestruktor
	virtual ~CEventResearch(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	void Create(void);

	// Zeichenfunktionen
	void Draw(Graphics* g, CGraphicPool* graphicPool) const;

private:
	// Attribute
	BYTE m_byTech;		///< erforschte Tech

	// temporäre Attribute zum Zeichnen -> müssen nicht serialisiert werden
	Gdiplus::Bitmap* _graphic;
	BYTE m_byTechlevel;			///< erforschtes Techlevel
};
