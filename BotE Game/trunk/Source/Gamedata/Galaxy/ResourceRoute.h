/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"

// forward eclaration
class CSector;
class CMajor;

class CResourceRoute : public CObject
{
public:
	DECLARE_SERIAL (CResourceRoute)

	/// Konstruktor
	CResourceRoute(void);

	/// Destruktor
	~CResourceRoute(void);

	/// Kopierkonstruktor
	CResourceRoute(const CResourceRoute & rhs);

	/// Zuweisungsoperatur
	CResourceRoute & operator=(const CResourceRoute &);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	/// Funktion gibt die Koordinate des Zielsystems der Ressourcenroute zurück.
	CPoint GetCo() const {return m_KO;}

	/// Funktion gibt den prozentualen Anteil der Ressource für den Bauauftrag zurück, der aus dem
	/// Startsystem genommen werden musste.
	BYTE GetPercent() const {return m_byPercent;}

	/// Funktion gibt die Art der Ressource zurück, die mittels dieser Handelsroute gehandelt wird.
	BYTE GetResource() const {return m_byResource;}

	/// Funktion legt den prozentualen Anteil fest.
	/// @param perc prozentualer Anteil
	void SetPercent(BYTE perc) {m_byPercent = perc;}

	// sonstige Funktionen
	/// Funktion legt eine Ressourcenroute im Zielsystem an.
	/// @param dest Koordinate des Zielsystems
	/// @param res Art der Ressource
	void GenerateResourceRoute(const CPoint &dest, BYTE res) {m_KO = dest; m_byResource = res; m_byPercent = NULL;}

	/// Funktion überprüft, ob die Ressourcenroute noch Bestand haben darf.
	/// @param sOwner Besitzer dieser Ressourcenroute
	/// @param dest Zeiger auf den Startsektor dieser Ressourcenroute
	/// @return Wahrheitswert, der sagt, ob die Ressourcenroute noch Bestand haben darf
	BOOLEAN CheckResourceRoute(const CString& sOwner, CSector* dest);

	/// Funktion zeichnet die Ressourcensroute auf der Galaxiekarte. Übergeben werden dafür die Koordinate des
	/// Zielsystems <code>dest</code> sowie der Besitzer des Systems <code>pMajor</code>.
	void DrawResourceRoute(CDC* pDC, CPoint dest, const CMajor* pMajor);


// Attribute
private:
	CPoint m_KO;		///< Koordinate des Zielsystems

	BYTE m_byPercent;	///< Prozentualer Anteil, welchen die Ressourcenroute dem Bauauftrag zugesteuert hat

	BYTE m_byResource;	///< Welche Ressource handelt diese Route?
};
