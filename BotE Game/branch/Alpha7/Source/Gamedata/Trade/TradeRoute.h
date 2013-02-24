/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"

// forward declaration
class CBotEDoc;
class CMajor;

class CTradeRoute :	public CObject
{
public:
	DECLARE_SERIAL (CTradeRoute)
	/// Konstruktor
	CTradeRoute(void);

	/// Destruktor
	~CTradeRoute(void);

	/// Kopierkonstruktor
	CTradeRoute(const CTradeRoute & rhs);

	/// Zuweisungsoperatur
	CTradeRoute & operator=(const CTradeRoute &);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	/// Funktion legt die Handelsroute zum System mit der Koordinate <code>ko</code> an.
	void GenerateTradeRoute(CPoint ko);

	/// Funktion gibt die Zielkoordinate der Handelsroute zurück
	CPoint GetDestKO() const {return m_KO;}

	/// Funktion gibt das Credits inkl. Boni auf Handelroute, welches man durch die Handelsroute bekommt zurück
	USHORT GetCredits(short boni) const;

	/// Funktion gibt die verbleibende Dauer der Handelsroute zurück. Ist die Dauer negativ, so wird sie zum Ende
	/// dieser Zeit aufgehoben.
	short GetDuration() const {return m_iDuration;}

	/// Funktion setzt die Dauer der Handelsroute. Ein positiver Paramter <code>duration</code> bedeutet, dass die
	/// Handelsroute noch solange Bestand hat, ein negativer hingegen bedeutet, dass die Handelsroute bei erreichen
	/// dieser Dauer aufgelöst wird.
	void SetDuration(short duration) {m_iDuration = duration;}

	/// Funktion legt manuell das erzeugte Credits fest.
	/// @param credits erzeugtes Credits der Handelsroute
	void SetCredits(USHORT credits) {m_iCredits = credits;}

	/// Funktion überprüft, ob die Handelsroute noch Bestand haben darf und setzt die Credits, welche
	/// diese Handelsroute fabriziert. Dabei werden noch keinerlei Boni auf die Creditsproduktion angerechnet.
	/// Die Funktion gibt einen Wahrheitswert zurück, der sagt, ob die Handelsroute noch Bestand haben darf.
	bool CheckTradeRoute(const CPoint& pFrom, const CPoint& pDest, CBotEDoc* pDoc);

	/// Diese Funktion verbessert manchmal die Beziehung zu der Minorrace, die in dem betroffenem Sektor lebt.
	/// Als Parameter wird hierfür die Ausgangskoordinate der Handerlsroute, die Zielkoordinate <code>pDest</code>
	/// und ein Zeiger auf das Dokument <code>pDoc</code> übergeben.
	void PerhapsChangeRelationship(const CPoint& pFrom, const CPoint& pDest, CBotEDoc* pDoc);

	/// Funktion zeichnet die Handelsroute auf der Galaxiekarte. Übergeben werden dafür die Koordinate des Systems
	/// von wo die Handelsroute startet <code>start</code> sowie auch der Besitzer dieses Systems <code>pMajor</code>.
	void DrawTradeRoute(CDC* pDC, CPoint start, const CMajor* pMajor);

private:
	/// Die Koordinate des Sektors, zu dem die Handelsroute verläuft
	CPoint m_KO;

	/// Das fabrizierte Credits der Handelsroute ohne irgendwelche Boni
	USHORT m_iCredits;

	/// Wieviele Runden läuft die Handelsroute noch? Beim Anlegen läuft sie 20 Runden lang, wird sie in dieser
	/// Zeit nicht gekündigt, so verlängert sich diese Zeit wieder um 20 Runden. Eine Kündigung dauert 5 Runden.
	short m_iDuration;
};
