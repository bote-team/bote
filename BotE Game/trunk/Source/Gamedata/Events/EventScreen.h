/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "Options.h"
#include "MyButton.h"

using namespace Gdiplus;

class CEventScreen : public CObject
{
public:
	DECLARE_SERIAL (CEventScreen)

	/// Standardkonstruktor
	CEventScreen(void);

	/// Standardkonstruktor mit Parameterübergabe
	CEventScreen(const CString &sPlayersRaceID, const CString &imageName, const CString &headline = "", const CString &text = "");

	/// Standarddestruktor
	virtual ~CEventScreen(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	CArray<CMyButton*>* GetButtons() {return &m_Buttons;}

	virtual void Create(void);

	virtual void Close(void) {};

	// Zeichenfunktionen
	virtual void Draw(Graphics* g, CGraphicPool* graphicPool) const;

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString GetTooltip(const CPoint &) const {return "";}

// Attribute
protected:
	CString m_sRace;

	Gdiplus::Image* m_pBGImage;

	CString m_strImagePath;

	CString m_strHeadline;

	CString m_strText;

	CArray<CMyButton*> m_Buttons;
};
