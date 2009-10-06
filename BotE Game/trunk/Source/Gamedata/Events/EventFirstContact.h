/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
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

	void Create(void);

	// Zeichenfunktionen
	void Draw(Graphics* g, CGraphicPool* graphicPool) const;

private:
	// Attribute
	CString m_sRaceID;		///< kennengelernte Rasse


};
