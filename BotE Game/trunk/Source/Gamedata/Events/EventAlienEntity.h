/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "Events\EventScreen.h"

class CEventAlienEntity : public CEventScreen
{
public:
	DECLARE_SERIAL (CEventAlienEntity)

	/// Standardkonstruktor
	CEventAlienEntity(void) {};

	/// Standardkonstruktor mit Parameterübergabe
	CEventAlienEntity(const CString& sPlayersRace, const CString& sAlienEntityID, const CString &sHeadline, const CString &sText);

	/// Standarddestruktor
	~CEventAlienEntity(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zeichenfunktionen
	virtual void Draw(Graphics* g, CGraphicPool* graphicPool) const;

private:
	// Attribute
	CString	m_sAlienEntityID;	///< ID der Alien-Entity
};
