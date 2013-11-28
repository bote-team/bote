/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
/*
 *@file
 * common properties of races, map tiles, ships
 */

#include "Constants.h"
#include "AssertBote.h"

#pragma once


// forward declaration

class CInGameEntity
{

//////////////////////////////////////////////////////////////////////
// construction/destruction
//////////////////////////////////////////////////////////////////////

protected:
	/// Konstruktor
	CInGameEntity(void);
	CInGameEntity(int x, int y);

	///Kopierkontruktor
	CInGameEntity(const CInGameEntity& other);

	CInGameEntity& operator=(const CInGameEntity&);
public:

	/// Destruktor
	virtual ~CInGameEntity(void);

protected:
	void Reset();

//////////////////////////////////////////////////////////////////////
// serialization
//////////////////////////////////////////////////////////////////////

protected:
	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);
public:

//////////////////////////////////////////////////////////////////////
// getting
//////////////////////////////////////////////////////////////////////

	CPoint GetCo() const
	{
		AssertBotE(PT_IN_RECT(m_Co, 0, 0, STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT));
		return m_Co;
	}

	CString GetName() const { return m_sName; }

//////////////////////////////////////////////////////////////////////
// setting
//////////////////////////////////////////////////////////////////////

	/// Funktion legt den Namen des Sektors fest.
	void SetName(const CString& name) { m_sName = name; }

//////////////////////////////////////////////////////////////////////
// members
//////////////////////////////////////////////////////////////////////

protected:

	CPoint m_Co;
	CString m_sName;

};
