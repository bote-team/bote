/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "intelobject.h"

/**
 * Diese Klasse stellt Informationen zu einem Geheimdiensteinsatz mit irgendeinem Wirtschaftsziel bereit.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */
class CEcoIntelObj : public CIntelObject
{
	DECLARE_SERIAL (CEcoIntelObj)
public:
	/// Konstruktor
	CEcoIntelObj(void);

	/// Konstruktor mit Parameterübergabe
	/// @param owner Auslöser/Eigentümer
	/// @param enemy Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param ko Koordinaten des System
	/// @param id ID der betroffenen Gebäude
	/// @param number Anzahl der betroffenen Gebäude
	CEcoIntelObj(BYTE owner, BYTE enemy, USHORT round, BOOLEAN isSpy, const CPoint &ko, USHORT id, BYTE number);

	/// Konstruktor mit Parameterübergabe
	/// @param owner Auslöser/Eigentümer
	/// @param enemy Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param latinum Latinum
	CEcoIntelObj(BYTE owner, BYTE enemy, USHORT round, BOOLEAN isSpy, int latinum);

	/// Destruktor
	~CEcoIntelObj(void);

	/// Kopierkonstruktor
	CEcoIntelObj(const CEcoIntelObj & rhs);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen

	/// Funktion gibt die Koordinaten des Sektors zurück, auf welchen sich die Geheimdienstaktion auswirkt.
	CPoint GetKO() const {return m_KO;}

	/// Funktion gibt die ID des Gebäudes zurück, auf welches sich die Geheimdienstaktion auswirkt.
	USHORT GetID() const {return m_nID;}

	/// Funktion gibt die Anzahl der Gebäude zurück, auf welche sich die Geheimdienstaktion auswirkt.
	BYTE GetNumber() const {return m_byNumber;}

	/// Funktion gibt die Latinummenge zurück
	int GetLatinum() const {return m_iLatinum;}
	
	// sonstige Funktion
	/// Funktion generiert einen Text, welcher eine Geheimdiestaktion beschreibt, für den Auslöser bzw. das Opfer
	/// dieser Aktion.
	/// @param pDoc Zeiger auf das Dokument
	/// @param n Nummer der verschiedenen Textmöglichkeiten, im Normalfall <code>NULL</code>
	/// @param param Hier kann die Rasse übergeben werden, von der das Opfer denkt angegriffen worden zu sein
	void CreateText(CBotf2Doc* pDoc, BYTE n, BYTE param);

private:
	// Attribute
	CPoint m_KO;				///< Systemkoordinaten des Wirtschaftsanschlages

	BYTE m_byNumber;			///< Anzahl der spionierten/sabotierten Gebäude

	USHORT m_nID;				///< ID der zu sabotierenden/sabotierten Gebäude

	int m_iLatinum;				///< Latinummenge
};
