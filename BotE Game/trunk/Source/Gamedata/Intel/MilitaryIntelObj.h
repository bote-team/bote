/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "intelobject.h"

/**
 * Diese Klasse stellt Informationen zu einem Geheimdiensteinsatz mit irgendeinem militärischen Ziel bereit.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */

class CMilitaryIntelObj : public CIntelObject
{
	DECLARE_SERIAL (CMilitaryIntelObj)
public:
	/// Konstruktor
	CMilitaryIntelObj(void);

	/// Konstruktor mit Parameterübergabe
	/// @param sOwnerID Auslöser/Eigentümer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param ko Koordinaten des System
	/// @param id ID der betroffenen Gebäude
	CMilitaryIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &ko, USHORT id, USHORT number,
		BOOLEAN building, BOOLEAN ship, BOOLEAN troop);

	/// Destruktor
	~CMilitaryIntelObj(void);

	/// Kopierkonstruktor
	CMilitaryIntelObj(const CMilitaryIntelObj & rhs);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	/// Funktion gibt die Koordinaten des Sektors zurück, auf welchen sich die Geheimdienstaktion auswirkt.
	CPoint GetCo() const {return m_KO;}

	/// Funktion gibt die ID des Gebäudes, des Schiffes oder der Truppe zurück, auf welches sich die Geheimdienstaktion
	/// auswirkt.
	USHORT GetID() const {return m_nID;}

	/// Funktion gibt die Anzahl der Gebäude, Schiffe oder Truppen zurück, auf welche sich die Geheimdienstaktion auswirkt.
	USHORT GetNumber() const {return m_nNumber;}

	/// Funktion gibt zurück, ob das Ziel dieser Militäraktion ein Gebäude ist.
	BOOLEAN GetIsBuilding() const {return m_bBuilding;}

	/// Funktion gibt zurück, ob das Ziel dieser Militäraktion ein Schiff/Aussenposten ist.
	BOOLEAN GetIsShip() const {return m_bShip;}

	/// Funktion gibt zurück, ob das Ziel dieser Militäraktion eine Truppe ist.
	BOOLEAN GetIsTroop() const {return m_bTroop;}

	// sonstige Funktionen
	/// Funktion generiert einen Text, welcher eine Geheimdiestaktion beschreibt, für den Auslöser bzw. das Opfer
	/// dieser Aktion.
	/// @param pDoc Zeiger auf das Dokument
	/// @param n Nummer der verschiedenen Textmöglichkeiten, im Normalfall <code>NULL</code>
	/// @param param Hier kann die Rasse übergeben werden, von der das Opfer denkt angegriffen worden zu sein
	void CreateText(CBotEDoc* pDoc, BYTE n, const CString& param);

private:
	// Atribute
	CPoint m_KO;					///< Systemkoordinaten der Militärsspionage

	USHORT m_nID;					///< ID der zu sabotierenden/sabotierten Gebäude, Truppen, Schiffe

	USHORT m_nNumber;				///< Anzahl der Gebäude/Schiffe/Truppen

	BOOLEAN m_bTroop;				///< handelt es sich beim Ziel der Aktion um eine Truppe

	BOOLEAN m_bShip;				///< handelt es sich beim Ziel der Aktion um ein Schiff/Schiffe

	BOOLEAN m_bBuilding;			///< handelt es sich beim Ziel der Aktion um ein/mehrere Gebäude
};
