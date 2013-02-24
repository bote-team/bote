/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include <map>
#include "Constants.h"

using namespace std;

#define DIPLOMACY_OFFER		0
#define DIPLOMACY_ANSWER	1
#define DIPLOMACY_TEXT		2

class CDiplomacyInfo : public CObject
{
	DECLARE_SERIAL(CDiplomacyInfo)

public:
	/// Standardkonstruktor
	CDiplomacyInfo(void);
	/// Standarddestruktor
	virtual ~CDiplomacyInfo(void);
	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);
	/// Kopierkonstruktor
	CDiplomacyInfo(const CDiplomacyInfo & rhs);
	/// Zuweisungsoperator
	CDiplomacyInfo & operator=(const CDiplomacyInfo &);

	// Funktionen

	/// Funktion zum Zurücksetzen aller Werte
	void Reset(void);

public:
	// Attribute
	CString		m_sToRace;						///< an welche Rasse (ID) geht das Angebot
	CString		m_sFromRace;					///< von welcher Rasse (ID) kommt das Angebot
	CString		m_sText;						///< Angebotstext

	short		m_nFlag;						///< Angebot, Antwort oder irgendwas anderes
	DIPLOMATIC_AGREEMENT::Typ m_nType;			///< Art des Angebots (Krieg, NAP, Geschenk usw.)
	USHORT		m_nSendRound;					///< in welcher Runde wurde das Angebot abgeschickt
	USHORT		m_nCredits;						///< wieviele Credits werden als Geschenk angeboten bzw. bei einer Forderung verlangt
	USHORT		m_nResources[DERITIUM + 1];		///< welche Ressourcen werden mit als Geschenk angeboten bzw. bei einer Forderung verlangt
	CPoint		m_ptKO;							///< aus welchem System stammen die Ressourcen

	short		m_nDuration;					///< Dauer des Angebots
	CString		m_sWarpactEnemy;				///< der Gegner bei einem Kriegspaktangebot
	CString		m_sCorruptedRace;				///< Hauptrasse, welche bei einer Bestechnung betroffen sein soll

	ANSWER_STATUS::Typ	m_nAnswerStatus;		///< wurde das Angebot angenommen, abgelehnt oder nicht darauf reagiert
	CString		m_sHeadline;					///< Überschrift bei Angebotsantwort
	CString		m_sWarPartner;					///< Krieg aufgrund Diplomatiepartner
};

class CBotEDoc;
class CRace;
/// Abstrakte Klasse stellt Methoden bereit, durch welche Rassen auf diplomatische Angebote reagieren sowie
/// selbstständig diplomatische Angebote machen.
class CDiplomacyAI
{
public:
	/// Standardkonstruktor
	/// @param pRace Zeiger auf Rasse, zu welcher das Diplomatieobjekt gehört
	CDiplomacyAI(CRace* pRace);
	/// Standarddestruktor
	virtual ~CDiplomacyAI(void);

	// Funktionen

	/// Diese Funktion berechnet, wie eine computergesteuerte Rasse, auf ein Angebot reagiert
	/// Das ist die komplette KI der Reaktion auf irgendwelche Angebote.
	/// @param info Information des diplomatischen Angebots
	/// @return <code>ACCEPTED</code> für Annahme
	///			<code>DECLINED</code> für Ablehnung
	///			<code>NOT_REACTED</code> für keine Reaktion
	virtual ANSWER_STATUS::Typ ReactOnOffer(const CDiplomacyInfo& info) = 0;

	/// Funktion zur Erstellung eines diplomatischen Angebots.
	/// @param sRaceID RassenID der Rasse, an die das Angebot gemacht werden soll
	/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
	/// @return <code>true</code> wenn ein Angebot erzeugt wurde, ansonsten <code>false</code>
	virtual bool MakeOffer(CString& sRaceID, CDiplomacyInfo& info) = 0;

protected:
	/// Funktion berechnet die Beziehungsverbesserungen durch die Übergabe von Credits und Ressourcen.
	/// Die Credits werden hier jedoch nicht gutgeschrieben, sondern nur die Beziehung zur Majorrace verbessert.
	/// @param info Referenz auf eine Diplomatieinformation
	virtual void ReactOnDowry(const CDiplomacyInfo& info) = 0;

	// Attribute
	CBotEDoc*	m_pDoc;				///< Zeiger auf das Dokument
	CRace*		m_pRace;			///< Zeiger auf die Rasse, zu welcher das Diplomatie-KI Objekt gehört
};
