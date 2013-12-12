/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "afx.h"
#include "BotEStandardTypes.h"
#include "Constants.h"
#include "AI\DiplomacyAI.h"
#include "General/InGameEntity.h"

class CShips;
class CShip;
class CEmpire;

using namespace std;

enum RaceProberties
{
	RACE_AGRARIAN				= 1,
	RACE_FINANCIAL				= 2,
	RACE_HOSTILE				= 4,
	RACE_INDUSTRIAL				= 8,
	RACE_PACIFIST				= 16,
	RACE_PRODUCER				= 32,
	RACE_SCIENTIFIC				= 64,
	RACE_SECRET					= 128,
	RACE_SNEAKY					= 256,
	RACE_SOLOING				= 512,
	RACE_WARLIKE				= 1024
};

enum RaceSpecialAbilities
{
	SPECIAL_NEED_NO_FOOD		= 1,	///< Rasse braucht keine Nahrung
	SPECIAL_NO_DIPLOMACY		= 2,	///< Rasse kann keine Diplomatie (kein First Contact)
	SPECIAL_ALIEN_DIPLOMACY		= 4		///< nur Alien-Diplomatie möglich (NAP, Freundschaft und Krieg)
};

/// Basisklasse für alle Rassen.
class CRace : public CInGameEntity, public boost::enable_shared_from_this<CRace>
{
public:

	enum RACE_TYPE {
		RACE_TYPE_MAJOR				=	0,	// Hauptrasse
		RACE_TYPE_MINOR				=	1,	// kleine Rasse (keine Ausbreitung)
		RACE_TYPE_ALIEN				=	2
	};

protected:
	/// Standardkonstruktor
	CRace(RACE_TYPE type);

public:
	/// Standarddestruktor
	virtual ~CRace(void);
	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);
	// Vergleichsopertoren
	bool operator< (const CRace& race) const { return m_sName < race.m_sName;}
	bool operator> (const CRace& race) const { return m_sName > race.m_sName;}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Funktionen
	/// Funktion zum Erfragen der Rasseneigenschaften
	/// @param nProp Rasseneigenschaft
	/// @return <code>true</code>, wenn die Rasse die Eigenschaft besitzt, sonst <code>false</code>
	bool IsRaceProperty(RACE_PROPERTY::Typ nProp) const;

	/// Funktion gibt zurück, ob eine andere Rasse bekannt ist.
	/// @param sRaceID ID der anderen Rasse
	/// @return <code>true</code> wenn Kontakt zu der anderen Rasse besteht, ansonsten <code>false</code>
	bool IsRaceContacted(const CString& sRaceID) const;

	/// Funktion gibt die ID der Rasse zurück
	/// @return Rassen-ID
	const CString& GetRaceID(void) const {return m_sID;}

	virtual const CEmpire* GetEmpire(void) const { return NULL; }

	/// Funktion gibt den Namen des Heimatsystems zurück
	/// @return Name des Heimatsystems
	const CString& GetHomesystemName(void) const {return m_sHomeSystem;}

	/// Funktion gibt den Rassennamen inkl. vorangestelltem Artikel zurück
	/// @return Rassenname inkl Artikel
	CString GetRaceNameWithArticle(void) const {return m_sNameArticle + " " + m_sName;}

	/// Funktion gibt den Rassentyp zurück (MAJOR, MINOR).
	/// @return Rassentyp
	RACE_TYPE GetType(void) const {return m_RaceType;}

	bool IsMajor() const {
		return m_RaceType == RACE_TYPE_MAJOR;
	}
	bool IsMinor() const {
		return m_RaceType == RACE_TYPE_MINOR || m_RaceType == RACE_TYPE_ALIEN;
	}

	/// Funktion gibt zurück, ob es sich um eine Alienrasse (kein Heimatsystem) handelt
	/// @return <code>true</code> wenn es eine Alienrasse ist, sonst <code>false</code>
	bool IsAlien() const
	{
		const bool is = m_RaceType == RACE_TYPE_ALIEN;
		AssertBotE(m_sHomeSystem.IsEmpty() == is);
		return is;
	}

	/// Funktion gibt die Nummer zurück, welche auf bestimmte Schiffe gemappt werden kann.
	/// @return Rassennummer
	BYTE GetRaceShipNumber(void) const {return m_byShipNumber;}

	/// Funktion gibt die Nummer zurück, welche auf bestimmte Gebäude gemappt werden kann.
	/// @return Rassennummer
	BYTE GetRaceBuildingNumber(void) const {return m_byBuildingNumber;}

	/// Funktion gibt die Nummer zurück, welche auf bestimmte Moralwerte gemappt werden kann.
	/// @return Rassennummer
	BYTE GetRaceMoralNumber(void) const {return m_byMoralNumber;}

	/// Funktion gibt Beziehungswert zu einer anderes Rasse zurück.
	/// @param sOtherRace Beziehung zu der anderen Rasse
	/// @return Beziehungswert (0-100)
	BYTE GetRelation(const CString& sOtherRace) const
	{
		map<CString, BYTE>::const_iterator it = m_mRelations.find(sOtherRace);
		if (it != m_mRelations.end())
			return it->second;
		else
			return 0;
	}

	/// Funktion setzt die neue Beziehung zur Rasse.
	/// @param sRaceID Rassen-ID zu der die Beziehung geändert werden soll
	/// @param nAdd Wert der zur alten Beziehung addiert werden soll
	void SetRelation(const CString& sRaceID, short nAdd);

	/// Funktion gibt den diplomatischen Status zu einer anderes Rasse zurück.
	/// @param sOtherRace andere Rasse
	/// @return VertragsID (anscheinend -5 bis 6, Grenzen eingeschlossen)
	DIPLOMATIC_AGREEMENT::Typ GetAgreement(const CString& sOtherRace) const
	{
		map<CString, DIPLOMATIC_AGREEMENT::Typ>::const_iterator it = m_mAgreement.find(sOtherRace);
		if (it != m_mAgreement.end())
			return it->second;
		else
			return DIPLOMATIC_AGREEMENT::NONE;
	}

	/// Funktion gibt das Feld der ausgehenden Nachrichten zurück.
	/// @return Feld der ausgehenden diplomatischen Nachrichten
	vector<CDiplomacyInfo>* GetOutgoingDiplomacyNews(void) {return &m_vDiplomacyNewsOut;}

	/// Funktion gibt das Feld der eingehenden Nachrichten zurück.
	/// @return Feld der eingehenden diplomatischen Nachrichten
	vector<CDiplomacyInfo>* GetIncomingDiplomacyNews(void) {return &m_vDiplomacyNewsIn;}

	/// Funktion gibt das zuletzt gemachte Angebot an eine bestimmte Rasse aus den letzten beiden Runden zurück.
	/// @param sToRace Rasse an welche das Angebot ging
	/// @return Angebot oder <code>NULL</code>, wenn kein Angebot aus den letzten beiden Runden vorliegt.
	const CDiplomacyInfo* GetLastOffer(const CString& sToRace) const
	{
		map<CString, CDiplomacyInfo>::const_iterator it = m_mLastOffers.find(sToRace);
		if (it != m_mLastOffers.end())
			return &it->second;
		else
			return NULL;
	}

	/// Funktion gibt den zu nutzenden Grafikdateinamen inkl. Dateiendung zurück.
	/// @return Name der Grafikdatei
	const CString& GetGraphicFileName(void) const {return m_sGraphicFile;}

	/// Funktion erstellt eine Tooltipinfo der Rasse.
	/// @return	der erstellte Tooltip-Text
	virtual CString GetTooltip(void) const;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Funktionen zum Schreiben der Membervariablen
	/// Funktion legt fest, ob die Rasse eine andere Rasse kennt.
	/// @param sRace andere Rasse
	/// @param bKnown <code>TRUE</code> wenn sie sie kennenlernt, ansonsten <code>FALSE</code>
	void SetIsRaceContacted(const CString& sRace, bool bKnown);

	/// Funktion legt den diplomatischen Status zu einer anderes Rasse fest.
	/// @param sOtherRace andere Rasse
	/// @param nNewAgreement neuer Vertrag
	virtual void SetAgreement(const CString& sOtherRace, DIPLOMATIC_AGREEMENT::Typ nNewAgreement);

	/// Funktion zum Setzen von Rasseneigenschaften.
	/// @param nProp Rasseneigenschaft
	/// @param is <code>true</code> oder <code>false</code>
	void SetRaceProperty(RACE_PROPERTY::Typ nProp, bool is);

	/// Funktion setzt die Schiffsnummer für die Rasse.
	/// @param nNumber Nummer des Schiffssets
	void SetRaceShipNumber(BYTE nNumber) {m_byShipNumber = nNumber;}

	/// Funktion zum Erfragen der Rassenspezialeigenschaften
	/// @param ability Rassenspezialeigenschaft
	/// @return <code>true</code>, wenn die Rasse die Spezialeigenschaft besitzt, sonst <code>false</code>
	bool HasSpecialAbility(int ability) const;

	bool CanBeContactedBy(const CString& sRaceID) const;
	virtual void Contact(const CRace& Race, const CPoint& p);

	virtual void AddToLostShipHistory(const CShips&, const CString&,
		const CString&, unsigned short) {};

	virtual void LostFlagShip(const CShip&) {};
	virtual void LostStation(SHIP_TYPE::Typ) {};
	virtual void LostShipToAnomaly(const CShips& ship, const CString& anomaly);

	/// Funktion zum Setzen von Spezialeigenschaften der Rasse.
	/// @param ability Spezialeigenschaft
	/// @param is <code>true</code> oder <code>false</code>
	void SetSpecialAbility(int ability, bool is);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Funktion zum erstellen einer Rasse.
	/// Die Funktion liest einen entsprechenden Eintrag aus einer data Datei.
	/// @param saInfo Referenz auf Rasseninformationen
	/// @param nPos Referenz auf Position im Array, ab wann die Informationen gelten
	virtual void Create(const CStringArray&, int&) = 0;

	/// Funktion lässt die Diplomatie-KI der Rasse Angebote an andere Rassen erstellen.
	void MakeOffersAI(void);

	/// Funktion lässt die KI auf diplomatische Angebote reagieren.
	/// @param pOffer diplomatisches Angebot
	void ReactOnOfferAI(CDiplomacyInfo* pOffer);

	/// Funktion zum zurücksetzen aller Werte auf Ausgangswerte.
	virtual void Reset(bool call_up);
public:

	void Delete() { m_bDeleted = true; }
	bool Deleted() const { return m_bDeleted; }

	// zum Schreiben der Membervariablen
	/// Funktion schreibt die Koordinate des Heimatsystems der Minorrace.
	/// @param Koordinate des Heimatsystems
	void SetRaceKO(const CPoint& ko)
	{
		AssertBotE(PT_IN_RECT(ko, 0, 0, STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT));
		m_Co = ko;
	}

protected:
	// Attribute
	// Beschreibende Attribute
	CString				m_sID;			///<!!! Rassen-ID
	CString				m_sHomeSystem;	///<!!! Name des Heimatsystems
	CString				m_sNameArticle;	///<!!! Artikel für Rassenname
	const RACE_TYPE		m_RaceType;		///<!!! Rassentyp (Major, Minor, Alien)
	int					m_nProperty;	///<!!! Rasseneigenschaften
	BYTE				m_byShipNumber;	///<!!! zugewiesene Nummer, welche Schiffe verwendet werden sollen
	BYTE				m_byBuildingNumber;	///<!!! zugewiesene Nummer, welche Gebäude verwendet werden sollen
	BYTE				m_byMoralNumber;	///<!!! zugewiesene Nummer, welche Moralwerte verwendet werden sollen
	int					m_nSpecialAbility;	///< Spezialfähigkeiten der Rasse

	// Ingame-Attribute (Rassenwechselwirkung)
	CBotEMap<CString, BYTE>	m_mRelations;	///< Beziehungsmap (Rassen-ID, Beziehungswert)
	CBotEMap<CString, DIPLOMATIC_AGREEMENT::Typ, int>	m_mAgreement;	///< Diplomatischer Status gegenüber anderen Rassen (Rassen-ID, Status)
	CBotESet<CString>		m_vInContact;	///< kennt die Rasse eine andere Rasse (Rassen-ID)
	// diplomatische Nachrichten
	CBotEVector<CDiplomacyInfo>	m_vDiplomacyNewsIn;	///< Vektor mit allen eingehenden diplomatischen Nachrichten
	CBotEVector<CDiplomacyInfo>	m_vDiplomacyNewsOut;///< Vektor mit allen ausgehenden diplomatischen Nachrichten
	map<CString, CDiplomacyInfo> m_mLastOffers;	///< Mep mit den von dieser Rasse gemachten Angeboten der letzten 2 Runden.

	// grafische Attribute
	CString				m_sGraphicFile;			///<!!! Name der zugehörigen Grafikdatei

	// Rassen Diplomatie-KI
	boost::shared_ptr<CDiplomacyAI> m_pDiplomacyAI; ///< Diplomatie-KI (muss nicht serialisiert werden)

private:
	bool m_bDeleted;

};
