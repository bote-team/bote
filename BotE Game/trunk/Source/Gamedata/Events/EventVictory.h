/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "Events/EventScreen.h"

class CEventVictory : public CEventScreen
{
public:
	DECLARE_SERIAL (CEventVictory)

	/// Standardkonstruktor
	CEventVictory(void) {};

	/// Standardkonstruktor mit Parameterübergabe
	CEventVictory(const CString& sPlayersRace, const CString& sWinnerRace, int nType, const CString &imageName);

	/// Standarddestruktor
	~CEventVictory(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	virtual void Create(void);

	virtual void Close(void);

	// Zeichenfunktionen
	virtual void Draw(Graphics* g, CGraphicPool* graphicPool) const;

private:
	// Attribute
	CString	m_sWinnerRace;

	int	m_nType;
};
