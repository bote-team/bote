/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "intelobject.h"

/**
 * Diese Klasse stellt Informationen zu einem Geheimdiensteinsatz mit irgendeinem Diplomatieziel bereit.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */

class CDiplomacyIntelObj :	public CIntelObject
{
	DECLARE_SERIAL (CDiplomacyIntelObj)
public:
	/// Konstruktor
	CDiplomacyIntelObj(void);

	/// Konstruktor mit Parameterübergabe
	/// @param sOwnerID Auslöser/Eigentümer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param minorRaceKO die Koordinaten des Minorraceheimatsystems
	/// @param agreement Vertrag von <code>enemy</code> mit der Minorrace
	/// @param relationship Beziehung von <code>enemy</code> mit der Minorrace
	CDiplomacyIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &minorRaceKO, short agreement, short relationship);

	/// Konstruktor mit Parameterübergabe
	/// @param sOwnerID Auslöser/Eigentümer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param sMajorRaceID betroffene Majorrace mit der <code>sEnemyID</code> in Beziehung steht
	/// @param agreement Vertrag von <code>enemy</code> mit der Majorrace
	/// @param duration Vertragsdauer
	/// @param relationship Beziehung von <code>enemy</code> mit der Majorrace
	CDiplomacyIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CString& sMajorRaceID, short agreement, short duration, short relationship);

	///  Konstruktor mit Parameterübergabe
	/// @param sOwnerID Auslöser/Eigentümer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param minorRaceKO die Koordinaten des Minorraceheimatsystems
	CDiplomacyIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &minorRaceKO);

	/// Destruktor
	~CDiplomacyIntelObj(void);

	/// Kopierkonstruktor
	CDiplomacyIntelObj(const CDiplomacyIntelObj & rhs);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	/// Funktion gibt die Koordinate des Heimatsystems einer Minorrace zurück.
	const CPoint& GetMinorRaceKO() const {return m_MinorRaceKO;}

	/// Funktion gibt die ID der betroffenen Majorrace zurück.
	const CString& GetMajorRaceID() const {return m_sMajor;}

	/// Funktion gibt einen bestehenden Vertrag zurück
	short GetAgreement() const {return m_nAgreement;}

	/// Funktion gibt die Dauer eines möglichen Vertrages zurück.
	short GetDuration() const {return m_nDuration;}

	/// Funktion gibt die Beziehung zu einer anderen Rasse zurück.
	short GetRelationship() const {return m_nRelationship;}

	// sonstige Funktion
	/// Funktion generiert einen Text, welcher eine Geheimdiestaktion beschreibt, für den Auslöser bzw. das Opfer
	/// dieser Aktion.
	/// @param pDoc Zeiger auf das Dokument
	/// @param n Nummer der verschiedenen Textmöglichkeiten, im Normalfall <code>NULL</code>
	/// @param param Hier kann die Rasse übergeben werden, von der das Opfer denkt angegriffen worden zu sein
	void CreateText(CBotf2Doc* pDoc, BYTE n, const CString& param);

private:
	// Attribute
	CPoint m_MinorRaceKO;		///< Heimatsystemkoordinaten der betroffenen Minorrace

	CString m_sMajor;			///< betroffene Majorrace, mit welcher das Geheimdienstopfer einen Vertrag hat (oder auch nicht)

	short m_nAgreement;			///< Vertrag zwischen den Rassen

	short m_nDuration;			///< Dauer des Vertrages

	short m_nRelationship;		///< Beziehung des Geheimdienstopfers zu einer anderen Major oder Minor
};
