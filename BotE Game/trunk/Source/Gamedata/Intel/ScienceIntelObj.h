/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "intelobject.h"

/**
 * Diese Klasse stellt Informationen zu einem Geheimdiensteinsatz mit irgendeinem Forschungsziel bereit.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */

class CScienceIntelObj : public CIntelObject
{
	DECLARE_SERIAL (CScienceIntelObj)
public:
	/// Konstruktor
	CScienceIntelObj(void);

	/// Konstruktor mit Parameterübergabe
	/// @param sOwnerID Auslöser/Eigentümer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param ko Koordinaten des System
	/// @param id ID der betroffenen Gebäude
	/// @param number Anzahl der betroffenen Gebäude
	CScienceIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &ko, USHORT id, BYTE number);

	/// Konstruktor mit Parameterübergabe
	/// @param sOwnerID Auslöser/Eigentümer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param FP globale Forschungspunkte
	CScienceIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, UINT FP);

	/// Konstruktor mit Parameterübergabe
	/// @param sOwnerID Auslöser/Eigentümer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param techLevel aktuelle ausspioniertes Techlevel
	/// @param techType Typ des ausspionierten Techlevel (Biotechnik, Energietechnik usw.)
	/// @param specialTechComplex ausspionierter Spezialforschungskomplex
	/// @param choosenSpecialTech gewählter Bonus aus der Spezialforschungs
	CScienceIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, short techLevel, short techType, RESEARCH_COMPLEX::Typ specialTechComplex, short choosenSpecialTech);

	/// Destruktor
	~CScienceIntelObj(void);

	/// Kopierkonstruktor
	CScienceIntelObj(const CScienceIntelObj & rhs);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	/// Funktion gibt die global produzierten Forschungspunkte des ausspionierten Imperiums zurück.
	UINT GetFP() const {return m_iFP;}

	/// Funktion gibt die Koordinaten des Sektors zurück, auf welchen sich die Geheimdienstaktion auswirkt.
	const CPoint& GetKO() const {return m_KO;}

	/// Funktion gibt die ID des Gebäudes zurück, auf welches sich die Geheimdienstaktion auswirkt.
	USHORT GetID() const {return m_nID;}

	/// Funktion gibt die Anzahl der Gebäude zurück, auf welche sich die Geheimdienstaktion auswirkt.
	BYTE GetNumber() const {return m_byNumber;}

	/// Funktion gibt das ausspionierte Techlevel zurück.
	short GetTechLevel() const {return m_nTechlevel;}

	/// Funktion gibt den Typ des ausspionierten Techlevels zurück (also Biotechnik, Energietechnik usw.)
	short GetTechType() const {return m_nTechType;}

	/// Funktion gibt einen erforschten Spezialforschungskomplex zurück.
	RESEARCH_COMPLEX::Typ GetSpecialTechComplex() const {return m_nSpecialTechComplex;}

	/// Funktion gibt die gewählte Auswahl aus dem Spezialforschungskomplex zurück.
	short GetChoosenSpecialTech() const {return m_nChoosenSpecialTech;}

	// sonstige Funktion
	/// Funktion generiert einen Text, welcher eine Geheimdiestaktion beschreibt, für den Auslöser bzw. das Opfer
	/// dieser Aktion.
	/// @param pDoc Zeiger auf das Dokument
	/// @param n Nummer der verschiedenen Textmöglichkeiten, im Normalfall <code>NULL</code>
	/// @param param Hier kann die Rasse übergeben werden, von der das Opfer denkt angegriffen worden zu sein
	void CreateText(CBotf2Doc* pDoc, BYTE n, const CString& param);

private:
	// Attribute
	UINT m_iFP;						///< gesamte Forschungspunkte

	CPoint m_KO;					///< Systemkoordinaten der Forschungsspionage

	BYTE m_byNumber;				///< Anzahl der spionierten/sabotierten Gebäude

	USHORT m_nID;					///< ID der zu sabotierenden/sabotierten Gebäude

	short m_nTechlevel;				///< aktuelle ausspioniertes Techlevel

	short m_nTechType;				///< Typ des ausspionierten Techlevel (Biotechnik, Energietechnik usw.)

	RESEARCH_COMPLEX::Typ m_nSpecialTechComplex;	///< ausspionierter Spezialforschungskomplex

	short m_nChoosenSpecialTech;	///< gewählter Bonus aus der Spezialforschungs
};
