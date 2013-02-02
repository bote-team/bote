/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "Events\eventscreen.h"

class CEventFirstContact :	public CEventScreen
{
public:
	DECLARE_SERIAL (CEventFirstContact)

	/// Standardkonstruktor
	CEventFirstContact(void);

	/// Standardkonstruktor mit Parameterübergabe
	CEventFirstContact(const CString &sPlayersRaceID, const CString& sRaceID);

	/// Standarddestruktor
	virtual ~CEventFirstContact(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	virtual void Create(void);

	// Zeichenfunktionen
	virtual void Draw(Graphics* g, CGraphicPool* graphicPool) const;

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString GetTooltip(const CPoint &pt) const;

private:
	// Attribute
	CString m_sRaceID;		///< kennengelernte Rasse


};
