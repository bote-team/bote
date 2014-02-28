#include "stdafx.h"
#include "Major.h"
#include "AI\MajorAI.h"
#include "General/Loc.h"
#include <algorithm>
#include "BotE.h"
#include "BotEDoc.h"
#include "RaceController.h"
#include "Ships/Ships.h"
#include "ClientWorker.h"



//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CMajor::CMajor(void) :
	CRace(RACE_TYPE_MAJOR),
	m_bPlayer(false),
	m_sEmpireName(),
	m_sEmpireWithArticle(),
	m_sEmpireWithAssignedArticle(),
	m_sPrefix(),
	m_nDiplomacyBonus(0),
	m_pStarmap()
{
}

CMajor::~CMajor(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CMajor::Serialize(CArchive &ar)
{
	CRace::Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_bPlayer;					// wird die Rasse von einem menschlichen Spieler gespielt?

		ar << m_sEmpireName;				// längerer Imperiumsname
		ar << m_sEmpireWithArticle;			// Artikel für Imperiumsnamen
		ar << m_sEmpireWithAssignedArticle;	// bestimmter Artikel für den Imperiumsnamen
		ar << m_sPrefix;					// Rassenprefix
		ar << m_nDiplomacyBonus;			// Bonus bei diplomatischen Verhandlungen, NULL == kein Bonus/kein Malus

		// noch verbleibende Runden des Vertrags (NULL == unbegrenzt)
		ar << m_mAgrDuration.size();
		for (map<CString, short>::const_iterator it = m_mAgrDuration.begin(); it != m_mAgrDuration.end(); ++it)
			ar << it->first << it->second;
		// Dauer des Verteidigungspaktes, einzeln speichern, weil er separat zu anderen Verträgen abgeschlossen werden kann.
		ar << m_mDefDuration.size();
		for (map<CString, short>::const_iterator it = m_mDefDuration.begin(); it != m_mDefDuration.end(); ++it)
			ar << it->first << it->second;
		// besitzt die Majorrace eines Verteidigungspakt mit einer anderen Majorrace (Rassen-ID, Wahrheitswert)
		ar << m_vDefencePact.size();
		for (vector<CString>::const_iterator it = m_vDefencePact.begin(); it != m_vDefencePact.end(); ++it)
			ar << *it;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_bPlayer;					// wird die Rasse von einem menschlichen Spieler gespielt?

		ar >> m_sEmpireName;				// längerer Imperiumsname
		ar >> m_sEmpireWithArticle;			// Artikel für Imperiumsnamen
		ar >> m_sEmpireWithAssignedArticle;	// bestimmter Artikel für den Imperiumsnamen
		ar >> m_sPrefix;					// Rassenprefix
		ar >> m_nDiplomacyBonus;			// Bonus bei diplomatischen Verhandlungen, NULL == kein Bonus/kein Malus

		// noch verbleibende Runden des Vertrags (NULL == unbegrenzt)
		m_mAgrDuration.clear();
		size_t mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			short value;
			ar >> key;
			ar >> value;
			m_mAgrDuration[key] = value;
		}
		// Dauer des Verteidigungspaktes, einzeln speichern, weil er separat zu anderen Verträgen abgeschlossen werden kann.
		m_mDefDuration.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			short value;
			ar >> key;
			ar >> value;
			m_mDefDuration[key] = value;
		}
		// besitzt die Majorrace eines Verteidigungspakt mit einer anderen Majorrace (Rassen-ID, Wahrheitswert)
		m_vDefencePact.clear();
		size_t vectorSize = 0;
		ar >> vectorSize;
		for (size_t i = 0; i < vectorSize; i++)
		{
			CString sID;
			ar >> sID;
			m_vDefencePact.push_back(sID);
		}

		// Diplomatieobjekt neu erstellen
		// Majorrace - KI anlegen
		// Diplomatie neu anlegen
		m_pDiplomacyAI = boost::make_shared<CMajorAI>(shared_from_this());

	}

	// Objekt welches gestalterische Informationen zur Rasse beinhaltet
	m_RaceDesign.Serialize(ar);

	// größere Objekte der Klasse
	m_Empire.Serialize(ar);					// das Imperium (Reich) der Hauptrasse
	m_Trade.Serialize(ar);					// der Börsenhandel für diese Rasse
	m_ShipHistory.Serialize(ar);			// alle statistischen Daten aller Schiffe sind hier zu finden

	// Observer-Objekte
	m_WeaponObserver.Serialize(ar);			// beobachtet die baubaren Waffen für Schiffe. Wird benötigt wenn wir Schiffe designen wollen
	m_MoralObserver.Serialize(ar);			// Moralevents serialisieren
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion setzt die noch verbleibenden Runden des diplomatischen Vertrages.
/// @param sRaceID Rassen-ID der anderen Rasse
/// @param nNewValue neue Rundenanzahl
void CMajor::SetAgreementDuration(const CString& sRaceID, short nNewValue)
{
	if (nNewValue == 0)
		m_mAgrDuration.erase(sRaceID);
	else
		m_mAgrDuration[sRaceID] = nNewValue;
}

/// Funktion legt den diplomatischen Status zu einer anderes Rasse fest.
/// @param sOtherRace andere Rasse
/// @param nNewAgreement neuer Vertrag
void CMajor::SetAgreement(const CString& sOtherRace, DIPLOMATIC_AGREEMENT::Typ nNewAgreement)
{
	if (nNewAgreement == DIPLOMATIC_AGREEMENT::DEFENCEPACT)
	{
		SetDefencePact(sOtherRace, true);

		if (GetAgreement(sOtherRace) == DIPLOMATIC_AGREEMENT::WAR)
			nNewAgreement = DIPLOMATIC_AGREEMENT::NONE;
		else if (GetAgreement(sOtherRace) == DIPLOMATIC_AGREEMENT::ALLIANCE)
			nNewAgreement = DIPLOMATIC_AGREEMENT::ALLIANCE;
	}

	// wenn kein Vertrag besteht, so kann der Eintrag auch aus der Map entfernt werden
	if (nNewAgreement == DIPLOMATIC_AGREEMENT::NONE)
	{
		m_mAgreement.erase(sOtherRace);
		SetAgreementDuration(sOtherRace, 0);
	}
	else if (nNewAgreement != DIPLOMATIC_AGREEMENT::DEFENCEPACT)
		m_mAgreement[sOtherRace] = nNewAgreement;

	// Bei Krieg erlischt der Verteidigungspakt und bei einem Bündnis bekommen wir den automatisch
	if (nNewAgreement == DIPLOMATIC_AGREEMENT::WAR || nNewAgreement == DIPLOMATIC_AGREEMENT::ALLIANCE)
	{
		for (vector<CString>::iterator it = m_vDefencePact.begin(); it != m_vDefencePact.end(); ++it)
			if (*it == sOtherRace)
			{
				m_vDefencePact.erase(it);
				SetDefencePactDuration(sOtherRace, 0);
				break;
			}
	}

	// Bei Krieg wird die Dauer eines alten Vertrages auf NULL gesetzt
	if (nNewAgreement == DIPLOMATIC_AGREEMENT::WAR)
		SetAgreementDuration(sOtherRace, 0);
}

/// Funktion gibt zurück, ob die Hauptrasse einen Verteidigungspakt mit einer anderen Hauptrasse
/// aufrechterhält.
/// @param pRaceID Rassen-ID der anderen Hauptrasse
/// @return <code>true</code> wenn ein Verteidigungspakt besteht, sonst <code>false</code>
bool CMajor::GetDefencePact(const CString& sRaceID) const
{
	if (std::find(m_vDefencePact.begin(), m_vDefencePact.end(), sRaceID) != m_vDefencePact.end())
		return true;

	return false;
}

/// Funktion legt fest, ob ein Verteidigungspakt parallel zu den anderen Verträgen besteht
/// @param sOtherRace andere Rasse
/// @param bIs Verteidigungspakt, ja oder nein
void CMajor::SetDefencePact(const CString& sOtherRace, bool nIs)
{
	if (nIs && !GetDefencePact(sOtherRace))
		m_vDefencePact.push_back(sOtherRace);
	else if (!nIs && GetDefencePact(sOtherRace))
	{
		for (vector<CString>::iterator it = m_vDefencePact.begin(); it != m_vDefencePact.end(); ++it)
			if (*it == sOtherRace)
			{
				m_vDefencePact.erase(it);
				SetDefencePactDuration(sOtherRace, 0);
				break;
			}
	}
}

/// Funktion setzt die noch verbleibenden Runden des Verteidigungspaktes.
/// @param sRaceID Rassen-ID der anderen Rasse
/// @param nNewValue neue Rundenanzahl
void CMajor::SetDefencePactDuration(const CString& sRaceID, short nNewValue)
{
	if (nNewValue == 0)
		m_mDefDuration.erase(sRaceID);
	else
		m_mDefDuration[sRaceID] = nNewValue;
}

/// Funktion verringert die Anzahl der noch verbleibenden Runden der laufenden Verträge um eins.
/// Außer der Vertrag läuft auf unbestimmte Zeit.
/// @param pmMajors Zeiger auf Map mit allen Majors im Spiel
bool CMajor::DecrementAgreementsDuration(map<CString, CMajor*>* pmMajors)
{
	bool bEnd = false;
	// Hier die Anzahl der Runden um eins verringern, außer der Wert steht schon auf
	// NULL, das bedeutet der Vertrag läuft auf unbestimmte Zeit -> unbegrenzt.
	// Wenn wir verringern und auf einen Wert von "1" kommen, dann wird der Vertrag aufgelöst
	// und eine Nachricht an unser Imperium versandt
	vector<CString> vDelAgrs;
	for (map<CString, DIPLOMATIC_AGREEMENT::Typ>::const_iterator it = m_mAgreement.begin(); it != m_mAgreement.end(); ++it)
	{
		if (it->first == m_sID)
			continue;

		// Dauer 1 runterzählen
		short nDuration = this->GetAgreementDuration(it->first);
		if (nDuration > 1)
			this->SetAgreementDuration(it->first, nDuration - 1);
		// bei eins Vertrag auflösen und Nachricht senden
		else if (nDuration == 1)
		{
			CString sAgreement;
			switch (this->GetAgreement(it->first))
			{
			case DIPLOMATIC_AGREEMENT::TRADE:		{sAgreement = CLoc::GetString("TRADE_AGREEMENT"); break;}
			case DIPLOMATIC_AGREEMENT::FRIENDSHIP:	{sAgreement = CLoc::GetString("FRIENDSHIP"); break;}
			case DIPLOMATIC_AGREEMENT::COOPERATION:	{sAgreement = CLoc::GetString("COOPERATION"); break;}
			case DIPLOMATIC_AGREEMENT::ALLIANCE:	{sAgreement = CLoc::GetString("ALLIANCE"); break;}
			case DIPLOMATIC_AGREEMENT::NAP:			{sAgreement = CLoc::GetString("NON_AGGRESSION"); break;}
			}

			CString sRace = "";
			CMajor* pMajor = (*pmMajors)[it->first];
			if (pMajor)
				sRace = pMajor->m_sEmpireWithAssignedArticle;

			CString s = CLoc::GetString("CONTRACT_ENDED", FALSE, sAgreement, sRace);
			CEmpireNews message;
			message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
			this->GetEmpire()->AddMsg(message);
			vDelAgrs.push_back(it->first);
		}
	}
	// nun die abgelaufenen Verträge aus den Maps entfernen
	for (size_t i = 0; i < vDelAgrs.size(); i++)
	{
		m_mAgreement.erase(vDelAgrs[i]);
		m_mAgrDuration.erase(vDelAgrs[i]);
		bEnd = true;
	}
	vDelAgrs.clear();

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////

	// nun die Verteidigungspakte separat betrachten
	for (vector<CString>::iterator it = m_vDefencePact.begin(); it != m_vDefencePact.end(); )
	{
		if (*it == m_sID)
			continue;

		// Dauer 1 runterzählen
		short nDuration = this->GetDefencePactDuration(*it);
		if (nDuration > 1)
			this->SetDefencePactDuration(*it, nDuration - 1);
		// bei eins Vertrag auflösen und Nachricht senden
		else if (nDuration == 1)
		{
			CString sRace = "";
			CMajor* pMajor = (*pmMajors)[*it];
			if (pMajor)
				sRace = pMajor->m_sEmpireWithAssignedArticle;

			CString s = CLoc::GetString("DEFENCE_PACT_ENDED", FALSE, sRace);
			CEmpireNews message;
			message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
			this->GetEmpire()->AddMsg(message);
			vDelAgrs.push_back(*it);
			it = m_vDefencePact.erase(it++);
			continue;
		}

		++it;
	}
	// nun die abgelaufenen Verteidigungspaktdauer aus den Maps entfernen
	for (size_t i = 0; i < vDelAgrs.size(); i++)
	{
		m_mDefDuration.erase(vDelAgrs[i]);
		bEnd = true;
	}
	vDelAgrs.clear();

	return bEnd;
}

/// Funktion zum erstellen einer Rasse.
/// Die Funktion liest einen entsprechenden Eintrag aus einer data Datei.
/// @param saInfo Referenz auf Rasseninformationen
/// @param nPos Referenz auf Position im Array, ab wann die Informationen gelten
void CMajor::Create(const CStringArray& saInfo, int& nPos)
{
	AssertBotE(nPos >= 0);

	// Majorrace nun anlegen
	m_sID			= saInfo[nPos++];				// Rassen-ID
	m_sHomeSystem	= saInfo[nPos++];				// Name des Heimatsystems
	m_sName			= saInfo[nPos++];				// Rassenname
	m_sNameArticle	= saInfo[nPos++];				// Artikel für Rassenname

	m_sEmpireName				= saInfo[nPos++];	// längerer Imperiumsname
	m_sEmpireWithArticle		= saInfo[nPos++] + " " + m_sEmpireName;		// Artikel für Imperiumsnamen
	m_sEmpireWithAssignedArticle= saInfo[nPos++] + " " + m_sEmpireName;		// bestimmter Artikel für den Imperiumsnamen
	m_sPrefix					= saInfo[nPos++];	// Rassenprefix

	m_sDescription				= saInfo[nPos++];				// Rassenbeschreibung
	m_byBuildingNumber	= atoi(saInfo[nPos++]);			// zugewiesene Nummer, welche Gebäude verwendet werden sollen
	m_byShipNumber		= atoi(saInfo[nPos++]);			// zugewiesene Nummer, welche Schiffe verwendet werden sollen
	m_byMoralNumber		= atoi(saInfo[nPos++]);			// zugewiesene Nummer, welche Moralwerte verwendet werden sollen
	// mehrere Rasseneigenschaften sind durch Komma getrennt
	CString sRaceProperties = saInfo[nPos++];
	int nStart = 0;
	while (nStart < sRaceProperties.GetLength())
	{
		RACE_PROPERTY::Typ nProperty = (RACE_PROPERTY::Typ)atoi(sRaceProperties.Tokenize(",", nStart));
		AssertBotE(nProperty >= RACE_PROPERTY::NOTHING_SPECIAL && nProperty <= RACE_PROPERTY::HOSTILE);
		SetRaceProperty(nProperty, true);				// Rasseneigenschaften
	}

	// mehrere Spezialfähigkeiten sind durch Komma getrennt
	CString sRaceAbilities = saInfo[nPos++];
	nStart = 0;
	while (nStart < sRaceAbilities.GetLength())
	{
		int nAbility = atoi(sRaceAbilities.Tokenize(",", nStart));
		SetSpecialAbility(nAbility, true);				// Spezialfähigkeiten der Rasse
	}

	m_nDiplomacyBonus	= atoi(saInfo[nPos++]);			// Bonus bei diplomatischen Verhandlungen, NULL == kein Bonus/kein Malus

	// grafische Attribute
	m_sGraphicFile				= saInfo[nPos++];					// Name der zugehörigen Grafikdatei
	m_RaceDesign.m_clrSector	= CRaceDesign::StringToColor(saInfo[nPos++]);	// Sektormarkierung
	m_RaceDesign.m_clrSmallBtn	= CRaceDesign::StringToColor(saInfo[nPos++]);	// Schriftfarbe für kleinen Button
	m_RaceDesign.m_clrLargeBtn	= CRaceDesign::StringToColor(saInfo[nPos++]);	// Schriftfarbe für großen Button
	m_RaceDesign.m_clrSmallText	= CRaceDesign::StringToColor(saInfo[nPos++]);	// Schriftfarbe für kleinen Text
	m_RaceDesign.m_clrNormalText= CRaceDesign::StringToColor(saInfo[nPos++]);	// Schriftfarbe für normalen Text
	m_RaceDesign.m_clrSecondText= CRaceDesign::StringToColor(saInfo[nPos++]);	// Zweitfarbe für normalen Text

	m_RaceDesign.m_clrGalaxySectorText	= CRaceDesign::StringToColor(saInfo[nPos++]);// Farbe der Systemnamen auf der Galaxiemap

	m_RaceDesign.m_clrListMarkTextColor = CRaceDesign::StringToColor(saInfo[nPos++]);	// Farbe für Text wenn dieser markiert ist
	m_RaceDesign.m_clrListMarkPenColor	= CRaceDesign::StringToColor(saInfo[nPos++]);	// Farbe für Umrandung bei Markierung eines Eintrags in einer Liste
	m_RaceDesign.m_clrRouteColor		= CRaceDesign::StringToColor(saInfo[nPos++]);	// Farbe der Handels- und Ressourcenrouten

	CString sGDI	= saInfo[nPos++];
	CString sGDIPlus= saInfo[nPos++];
	int iStart = 0;
	for (int i = 0; i < 6; i++)
		m_RaceDesign.m_byGDIFontSize[i]		= atoi(sGDI.Tokenize(",", iStart));			// Fontgrößen für GDI Darstellung
	iStart = 0;
	for (int i = 0; i < 6; i++)
		m_RaceDesign.m_byGDIPlusFontSize[i]	=  atoi(sGDIPlus.Tokenize(",", iStart));	// Fontgrößen für GDI+ Darstellung
	m_RaceDesign.m_sFontName	= saInfo[nPos++];					// Name der Schriftart

	m_bPlayer					= false;	// wird die Rasse von einem menschlichen Spieler gespielt?

	// Hier die Variablen für die Handelsklasse rassenspezifisch setzen
	// die Handelssteuerrate festsetzen (alle 15% außer bis jetzt Finanzrassen, der hat 5%)
	float fTax = 1.15f;
	if (IsRaceProperty(RACE_PROPERTY::FINANCIAL))
		fTax = min(fTax, 1.05f);
	m_Trade.SetTax(fTax);

	// Hier die aktuellen Kursdaten initial in die History schreiben
	m_Trade.GetTradeHistory()->SaveCurrentPrices(m_Trade.GetRessourcePrice(), m_Trade.GetTax());

	m_Empire.SetEmpireID(m_sID);

	// Majorrace - KI anlegen
	m_pDiplomacyAI = boost::make_shared<CMajorAI>(shared_from_this());
}

void CMajor::CreateStarmap(void)
{
	m_pStarmap = boost::make_shared<CStarmap>(!m_bPlayer, 3 - SHIP_RANGE::MIDDLE);
}

void CMajor::Contact(const CRace& Race, const CSector& p)
{
	CRace::Contact(Race, p);

	// Nachricht generieren, dass wir eine andere Rasse kennengelernt haben
	const CString& sSectorKO = p.CoordsName(true);

	//message to the involved major
	CString sKey = Race.IsMajor() ? "GET_CONTACT_TO_MAJOR" : "GET_CONTACT_TO_MINOR";
	CString sMsg = CLoc::GetString(sKey,FALSE, Race.GetName(),sSectorKO);

	CEmpireNews message;
	message.CreateNews(sMsg, EMPIRE_NEWS_TYPE::DIPLOMACY, "", p.GetCo());
	m_Empire.AddMsg(message);

	// Eventscreen einfügen
	if (IsHumanPlayer())
	{
		const boost::shared_ptr<CEventFirstContact> event_screen =
			boost::make_shared<CEventFirstContact>(m_sID, Race.GetRaceID());
		m_Empire.PushEvent(event_screen);
	}
}

bool CMajor::AHumanPlays() const {
	if(!IsHumanPlayer())
		return false;
	const CBotEDoc& doc = *resources::pDoc;
	const CCommandLineParameters& clp = *resources::pClp;

	return doc.GetCurrentRound() > clp.GetAutoTurns();
}

void CMajor::AddToLostShipHistory(const CShipHistoryStruct& ship, const CString& sEvent,
	const CString& sStatus, unsigned short round, const CPoint& co)
{
	const CBotEDoc& doc = *resources::pDoc;
	m_ShipHistory.ModifyShip(ship, doc.GetSystem(co.x, co.y).CoordsName(true), round, sEvent, sStatus);
}

void CMajor::LostFlagShip(const CShip& ship)
{
	const CString& eventText = m_MoralObserver.AddEvent(7, GetRaceMoralNumber(), ship.GetName());
	CEmpireNews message;
	message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY, "", ship.GetCo());
	m_Empire.AddMsg(message);
}

void CMajor::LostStation(SHIP_TYPE::Typ type)
{
	CString eventText(m_MoralObserver.AddEvent(9, GetRaceMoralNumber()));
	if (type == SHIP_TYPE::OUTPOST)
		eventText = m_MoralObserver.AddEvent(8, GetRaceMoralNumber());
	CEmpireNews message;
	message.CreateNews(eventText, EMPIRE_NEWS_TYPE::MILITARY);
	m_Empire.AddMsg(message);
}

void CMajor::LostShipToAnomaly(const CShips& ship, const CString& anomaly)
{
	CString sShip;
	sShip.Format("%s (%s, %s)", ship.GetName(), ship.GetShipTypeAsString(), ship.GetShipClass());
	const CString& s = CLoc::GetString("ANOMALY_SHIP_LOST", FALSE, sShip, anomaly);
	CEmpireNews message;
	message.CreateNews(s, EMPIRE_NEWS_TYPE::MILITARY, "", ship.GetCo());
	m_Empire.AddMsg(message);
	resources::pClientWorker->SetToEmpireViewFor(*this);
}

bool CMajor::CanBuildShip(SHIP_TYPE::Typ type,const BYTE researchLevels[6], const CShipInfo& info) const {
	return info.GetRace() == GetRaceShipNumber()
		&& info.GetShipType() == type && info.IsThisShipBuildableNow(researchLevels);
}

short CMajor::BestBuildableVariant(SHIP_TYPE::Typ type, const CArray<CShipInfo, CShipInfo>& shipinfoarray) const
{
	short id = -1;
	USHORT costs = 0;
	const CResearch& rs = *m_Empire.GetResearch();
	const BYTE researchLevels[6] =
	{
		rs.GetBioTech(),
		rs.GetEnergyTech(),
		rs.GetCompTech(),
		rs.GetPropulsionTech(),
		rs.GetConstructionTech(),
		rs.GetWeaponTech()
	};
	for (int l = 0; l < shipinfoarray.GetSize(); l++)
	{
		const CShipInfo& info = shipinfoarray.GetAt(l);
		if (CanBuildShip(type, researchLevels, info) && info.GetBaseIndustry() > costs)
			{
				costs = info.GetBaseIndustry();
				id = info.GetID();
			}
	}
	return id;
}

float CMajor::CreditsMulti() const
{
	float fCreditsMulti = 1.0f;
	if (IsRaceProperty(RACE_PROPERTY::FINANCIAL))
		fCreditsMulti += 0.5f;
	if (IsRaceProperty(RACE_PROPERTY::PRODUCER))
		fCreditsMulti += 0.25f;
	if (IsRaceProperty(RACE_PROPERTY::WARLIKE) || IsRaceProperty(RACE_PROPERTY::HOSTILE))
		fCreditsMulti -= 0.1f;
	if (IsRaceProperty(RACE_PROPERTY::SNEAKY))
		fCreditsMulti -= 0.2f;
	if (IsRaceProperty(RACE_PROPERTY::SECRET))
		fCreditsMulti -= 0.25f;
	if (IsRaceProperty(RACE_PROPERTY::SOLOING))
		fCreditsMulti -= 0.5f;

	fCreditsMulti = max(fCreditsMulti, 0.0f);
	return fCreditsMulti;
}
