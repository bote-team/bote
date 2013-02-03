/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once

#include "Constants.h"

class CEmpireNews : public CObject
{
public:
	// Klasse serialisierbar machen
	DECLARE_SERIAL (CEmpireNews)

	/// Konstruktor
	CEmpireNews(void);

	/// Destruktor
	virtual ~CEmpireNews(void);

	/// Kopierkonstruktor
	CEmpireNews(const CEmpireNews & rhs);

	/// Zuweisungsoperator
	CEmpireNews & operator=(const CEmpireNews &);

	/// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	EMPIRE_NEWS_TYPE::Typ GetType(void) const {return m_nType;}
	const CPoint& GetKO(void) const {return m_ptKO;}
	BYTE GetFlag(void) const {return m_byFlag;}
	const CString& GetText(void) const {return m_sText;}

	// sonstige Funktionen
	void CreateNews(const CString& sMessage, EMPIRE_NEWS_TYPE::Typ nMessageType, const CString& sSystemName = "", const CPoint& ptKO = CPoint(-1 ,-1), bool bUpdate = false, BYTE byFlag = 0);
	void CreateNews(const CString& sMessage, EMPIRE_NEWS_TYPE::Typ nMessageType, BYTE byFlag);

private:
	CString	m_sText;
	EMPIRE_NEWS_TYPE::Typ m_nType;
	CPoint	m_ptKO;			// Koordinaten des Systems, worauf sich die Nachricht bezieht
	BYTE	m_byFlag;		// irgendeine Zusatzinformation, die ich mit übergeben kann
};

typedef CArray<CEmpireNews,CEmpireNews> EmpiresNews; // Das dynamische Feld wird vereinfacht als EmpiresNews angegeben
