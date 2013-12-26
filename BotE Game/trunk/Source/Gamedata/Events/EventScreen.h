/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "Constants.h"
#include "MyButton.h"

using namespace Gdiplus;

class CEventScreen : public CObject
{
public:
	DECLARE_SERIAL (CEventScreen)

	enum EVENT_SCREEN_TYPE
	{
		EVENT_SCREEN_TYPE_ALIEN_ENTITY,
		EVENT_SCREEN_TYPE_BLOCKADE,
		EVENT_SCREEN_TYPE_BOMBARDMENT,
		EVENT_SCREEN_TYPE_COLONIZATION,
		EVENT_SCREEN_TYPE_FIRST_CONTACT,
		EVENT_SCREEN_TYPE_GAME_OVER,
		EVENT_SCREEN_TYPE_RACE_KILLED,
		EVENT_SCREEN_TYPE_RANDOM,
		EVENT_SCREEN_TYPE_RESEARCH,
		EVENT_SCREEN_TYPE_VICTORY,
		EVENT_SCREEN_TYPE_NONE
	};

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

	EVENT_SCREEN_TYPE GetType() const { return m_Type; }

// Attribute
protected:
	CString m_sRace;

	Gdiplus::Image* m_pBGImage;

	CString m_strImagePath;

	CString m_strHeadline;

	CString m_strText;

	CArray<CMyButton*> m_Buttons;

	EVENT_SCREEN_TYPE m_Type;
};
