#include "stdafx.h"
#include "RaceController.h"
#include "Major.h"
#include "Minor.h"
#include "Botf2Doc.h"
#include "AI\MinorAI.h"
#include "HTMLStringBuilder.h"
#include "Ships/Ships.h"
#include "General/ResourceManager.h"

IMPLEMENT_SERIAL (CMinor, CRace, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CMinor::CMinor(void)
{
	this->Reset();
}

CMinor::~CMinor(void)
{
	this->Reset();
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CMinor::Serialize(CArchive &ar)
{
	CRace::Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_ptKO;						// Koordinaten der Rasse
		ar << m_iTechnologicalProgress;		// wie fortschrittlich ist die Minorrace?
		ar << m_iCorruptibility;			// wie stark ändert sich die Beziehung beim Geschenke geben?
		ar << m_bSpaceflight;				// Spaceflightnation (hat Schiffe)

		ar << m_bSubjugated;				// wurde die Rasse unterworfen
		// Akzeptanzpunkte (Rassen-ID, Punkte)
		ar << m_mAcceptance.size();
		for (map<CString, short>::const_iterator it = m_mAcceptance.begin(); it != m_mAcceptance.end(); ++it)
			ar << it->first << it->second;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_ptKO;						// Koordinaten der Rasse
		ar >> m_iTechnologicalProgress;		// wie fortschrittlich ist die Minorrace?
		ar >> m_iCorruptibility;			// wie stark ändert sich die Beziehung beim Geschenke geben?
		ar >> m_bSpaceflight;				// Spaceflightnation (hat Schiffe)

		ar >> m_bSubjugated;			// wurde die Rasse unterworfen
		// Akzeptanzpunkte (Rassen-ID, Punkte)
		m_mAcceptance.clear();
		size_t mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			short value;
			ar >> key;
			ar >> value;
			m_mAcceptance[key] = value;
		}

		// Diplomatieobjekt neu erstellen
		// Minorrace - KI anlegen
		if (m_pDiplomacyAI)
		{
			delete m_pDiplomacyAI;
			m_pDiplomacyAI = NULL;
		}
		m_pDiplomacyAI = new CMinorAI(this);
	}
}

//////////////////////////////////////////////////////////////////////
// Zugriffsfunktionen
//////////////////////////////////////////////////////////////////////

/// Funktion gibt die Akzeptanzpunkte zu einer gewünschten Majorrace zurück.
/// @param sRaceID Rassen-ID der gewünschten Majorrace
/// @return Akzeptanzpunkte
short CMinor::GetAcceptancePoints(const CString& sRaceID) const
{
	map<CString, short>::const_iterator it = m_mAcceptance.find(sRaceID);

	if (it != m_mAcceptance.end())
		return it->second;
	else
		return 0;
}

/// Funktion addiert Akzeptanzpunkte zu den Vorhandenen Punkten dazu.
/// @param sRaceID Rassen-ID der gewünschten Majorrace
/// @param nAdd hinzuzufügende Akzeptanzpunkte
void CMinor::SetAcceptancePoints(const CString& sRaceID, short nAdd)
{
	if (nAdd == 0)
		return;

	m_mAcceptance[sRaceID] += nAdd;

	map<CString, short>::iterator it = m_mAcceptance.find(sRaceID);

	// Konsitenzprüfung durchführen
	// Obergrenze setzen, einfach mal auf 5000
	it->second = min(5000, it->second);
	// Untergrenze ist null
	it->second = max(0, it->second);

	if (it->second == 0)
		m_mAcceptance.erase(it);
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion läßt die Minorrace mit einer gewissen Wahrscheinlichkeit einen weiteren Planeten in ihrem System
/// kolonisieren.
/// @param pDoc Zeiger auf das Dokument
/// @return <code>true</code> wenn ein neuer Planet kolonisiert wurde, ansonsten <code>false</code>
bool CMinor::PerhapsExtend(CBotf2Doc* pDoc)
{
	ASSERT(pDoc);

	// Wenn der dazugehörige Sektor nicht der kleinen Rasse gehört, also eine andere Rasse diese vereinnahmt hat,
	// dann wächst das System auch nicht mehr automatisch
	if (pDoc->GetSector(m_ptKO.x, m_ptKO.y).GetOwnerOfSector() != m_sID)
		return false;

	std::vector<CPlanet>& planets = pDoc->GetSector(m_ptKO.x, m_ptKO.y).GetPlanets();
	bool bColonized = false;
	for (int i = 0; i < static_cast<int>(planets.size()); i++)
	{
		// ist der Planet noch nicht geterraformt
		if (planets.at(i).GetColonized() == FALSE && planets.at(i).GetHabitable() == TRUE && planets.at(i).GetTerraformed() == FALSE)
		{
			// mit einer gewissen Wahrscheinlichkeit wird der Planet geterraformt und kolonisiert
			if (rand()%200 >= (200 - (m_iTechnologicalProgress+1)))
			{
				bColonized = true;
				planets.at(i).SetNeededTerraformPoints(planets.at(i).GetNeededTerraformPoints());
				planets.at(i).SetTerraformed(TRUE);
				planets.at(i).SetColonisized(TRUE);
				planets.at(i).SetIsTerraforming(FALSE);
				if (planets.at(i).GetMaxHabitant() < 1.0f)
					planets.at(i).SetCurrentHabitant(planets.at(i).GetMaxHabitant());
				else
					planets.at(i).SetCurrentHabitant(1.0f);
			}
		}
		// ist der Planet schon geterraformt
		else if (planets.at(i).GetColonized() == FALSE && planets.at(i).GetTerraformed() == TRUE)
		{
			// dann wird mit einer größeren Wahrscheinlichkeit kolonisiert
			if (rand()%200 >= (200 - 3*(m_iTechnologicalProgress+1)))
			{
				bColonized = true;
				planets.at(i).SetColonisized(TRUE);
				if (planets.at(i).GetMaxHabitant() < 1.0f)
					planets.at(i).SetCurrentHabitant(planets.at(i).GetMaxHabitant());
				else
					planets.at(i).SetCurrentHabitant(1.0f);
			}
		}
	}

	if (bColonized)
		// dann sind im System auch weitere Einwohner hinzugekommen
		pDoc->GetSystem(m_ptKO.x, m_ptKO.y).SetHabitants(pDoc->GetSector(m_ptKO.x, m_ptKO.y).GetCurrentHabitants());

	return bColonized;
}

/// Diese Funktion lässt die Minorrace vielleicht ein Schiff bauen
/// @param pDoc Zeiger auf das Dokument
void CMinor::PerhapsBuildShip(CBotf2Doc* pDoc)
{
	ASSERT(pDoc);

	// wenn keine Spaceflightrasse, dann Abbruch
	if (!m_bSpaceflight)
		return;

	if (m_ptKO == CPoint(-1,-1))
		return;

	// Wenn der dazugehörige Sektor nicht der kleinen Rasse gehört, also eine andere Rasse diese vereinnahmt hat,
	// dann wächst das System auch nicht mehr automatisch
	if (pDoc->GetSector(m_ptKO.x, m_ptKO.y).GetOwnerOfSector() != m_sID)
		return;

	// Vielleicht baut die Minorrace ein Schiff in dieser Runde
	for (int i = 0; i < pDoc->m_ShipInfoArray.GetSize(); i++)
	{
		CShipInfo* pShipInfo = &(pDoc->m_ShipInfoArray[i]);
		if (pShipInfo->GetRace() == MINORNUMBER)
		{
			if (pShipInfo->GetOnlyInSystem() == this->GetHomesystemName())
			{
				// schauen ob es technologisch baubar ist
				BYTE byAvgTechLevel = pDoc->GetStatistics()->GetAverageTechLevel();
				BYTE byTechLevel = byAvgTechLevel + m_iTechnologicalProgress / 2;

				BYTE researchLevels[6] = {byTechLevel, byTechLevel, byTechLevel, byTechLevel, byTechLevel, byTechLevel};
				if (pShipInfo->IsThisShipBuildableNow(researchLevels))
				{
					// Wahrscheinlichkeit berechnen, ob das Schiff gebaut wird
					int nNumber = 1;
					for(CShipMap::const_iterator j = pDoc->m_ShipMap.begin(); j != pDoc->m_ShipMap.end(); ++j)
					{
						const CShips* pShip = &j->second;
						if (pShip->GetOwnerOfShip() == m_sID && pShip->GetShipClass() == pShipInfo->GetShipClass())
							nNumber++;
					}
					nNumber *= 5;
					if (rand()%nNumber == 0)
					{
						CShipMap::iterator ship = pDoc->BuildShip(pShipInfo->GetID(), GetRaceKO(), m_sID);
						// Befehl für nicht "böse" Rassen auf Meiden stellen
						if (!IsRaceProperty(RACE_PROPERTY::HOSTILE)
							&& !IsRaceProperty(RACE_PROPERTY::WARLIKE)
							&& !IsRaceProperty(RACE_PROPERTY::SNEAKY)
							&& !IsRaceProperty(RACE_PROPERTY::SECRET))
							ship->second.SetCurrentOrder(SHIP_ORDER::AVOID);
						return;
					}
				}
			}
		}
	}
}

/// Funktion berechnet wieviele Beziehungspunkte eine Majorrace für längere Beziehungsdauer bekommt.
/// Umso mehr Punkte sie hat, umso schwerer wird es für andere Majorraces diese Rasse durch Geschenke
/// zu beeinflussen
/// @param pDoc Zeiger auf das Dokument
void CMinor::CalcAcceptancePoints(CBotf2Doc* pDoc)
{
	ASSERT(pDoc);

	// alle Majors holen
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	// und durchiterieren
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		// nur wenn die Rasse bekannt ist weitermachen
		if (!IsRaceContacted(it->first))
			continue;

		short nAgreement = GetAgreement(it->first);
		short nAccPoints = 0;
		// bei keinem aktuell laufendem Vertrag verringern sich die gesammelten Punkte langsam
		if (nAgreement == DIPLOMATIC_AGREEMENT::NONE)
			nAccPoints -= rand()%80 + 1;
		else if (nAgreement == DIPLOMATIC_AGREEMENT::FRIENDSHIP)
			nAccPoints += 10;
		else if (nAgreement == DIPLOMATIC_AGREEMENT::COOPERATION)
			nAccPoints += 20;
		else if (nAgreement == DIPLOMATIC_AGREEMENT::AFFILIATION)
			nAccPoints += 30;
		else if (nAgreement == DIPLOMATIC_AGREEMENT::MEMBERSHIP)
		{
			nAccPoints += 40;
			// bei einer Mitgliedschaft erhöht sich womoglich auch die Beziehung ein wenig
			SetRelation(it->first, rand()%2);
		}
		// bei Krieg werden alle Punkte gelöscht
		else if (nAgreement == DIPLOMATIC_AGREEMENT::WAR)
			nAccPoints -= GetAcceptancePoints(it->first);

		// Akzeptanzpunkte nun hinzufügen bzw. abziehen
		SetAcceptancePoints(it->first, nAccPoints);
	}
}

/// Funktion berechnet, wieviel Rohstoffe pro Runde (also bei jedem NextRound() aufrufen) von der
/// Minorrace verbraucht werden. Wenn die Minorrace die Möglichkeit hat, den jeweiligen Rohstoff
/// selbst abzubauen, damm ist der Verbrauch natürlich geringer.
/// @param pDoc Zeiger auf das Dokument
void CMinor::ConsumeResources(CBotf2Doc* pDoc)
{
	ASSERT(pDoc);

	// bewohnbar sind:    C,F,G,H,K,L,M,N,O,P,Q,R
	BOOLEAN exist[DERITIUM + 1] = {0};
	pDoc->GetSector(m_ptKO.x, m_ptKO.y).GetAvailableResources(exist, true);

	short div;
	int value;

	// Titan
	exist[TITAN] == TRUE ? div = 1000 : div = 4000;
	value = rand()%div;
	value = min(3000, value);
	pDoc->GetSystem(m_ptKO.x, m_ptKO.y).SetResourceStore(TITAN, -value);

	//CString s;
	//s.Format("Name der Rasse: %s\nTitanabbau: %d\nVerbrauch: %d\nLager: %d",m_strRaceName,exist[0],value,m_iRessourceStorage[0]);
	//AfxMessageBox(s);

	// Deuterium
	exist[DEUTERIUM] == TRUE ? div = 1500 : div = 4000;
	value = rand()%div;
	value = min(3000, value);
	pDoc->GetSystem(m_ptKO.x, m_ptKO.y).SetResourceStore(DEUTERIUM, -value);

	// Duranium
	exist[DURANIUM] == TRUE ? div = 1500 : div = 4000;
	value = rand()%div;
	value = min(3000, value);
	pDoc->GetSystem(m_ptKO.x, m_ptKO.y).SetResourceStore(DURANIUM, -value);

	// Kristalle
	exist[CRYSTAL] == TRUE ? div = 1500 : div = 4000;
	value = rand()%div;
	value = min(3000, value);
	pDoc->GetSystem(m_ptKO.x, m_ptKO.y).SetResourceStore(CRYSTAL, -value);

	// Iridium
	exist[IRIDIUM] == TRUE ? div = 1500 : div = 4000;
	value = rand()%div;
	value = min(3000, value);
	pDoc->GetSystem(m_ptKO.x, m_ptKO.y).SetResourceStore(IRIDIUM, -value);

	// Deritium
	value = rand()%2;
	pDoc->GetSystem(m_ptKO.x, m_ptKO.y).SetResourceStore(DERITIUM, -value);
}

/// Funktion überprüft, ob die Minorrace das Angebot aufgrund anderer Verträge überhaupt annehmen kann.
/// @param pDoc Zeiger auf das Dokument
/// @param sMajorID MajorraceID, von welcher das Angebot stammt
/// @param nType angebotener Vertrag
/// @return <code>true</code> wenn das Angebot theoretisch angenommen werden könnte, ansonsten <code>false</code>
bool CMinor::CanAcceptOffer(CBotf2Doc* pDoc, const CString& sMajorID, short nType) const
{
	ASSERT(pDoc);

	// Nur wenn der aktuelle Vertrag nicht höherwertiger ist als der angebotene, dann wird er akzeptiert
	if (this->GetAgreement(sMajorID) >= nType)
		return false;

	// Checken ob wir ein Angebot überhaupt annehmen können, wenn z.B. eine andere Hauptrasse
	// eine Mitgliedschaft mit einer Minorrace hat, dann können wir ihr kein Angebot machen, außer
	// Krieg erklären, Geschenke geben und Bestechen
	DIPLOMATIC_AGREEMENT::Typ nOthersAgreement = DIPLOMATIC_AGREEMENT::NONE;
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	// nicht wir selbst
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		if (it->first != sMajorID)
		{
			DIPLOMATIC_AGREEMENT::Typ nTemp = this->GetAgreement(it->first);
			if (nTemp > nOthersAgreement)
				nOthersAgreement = nTemp;
		}
	}

	if ((nType == DIPLOMATIC_AGREEMENT::COOPERATION || nType == DIPLOMATIC_AGREEMENT::AFFILIATION || nType == DIPLOMATIC_AGREEMENT::MEMBERSHIP) && nOthersAgreement > DIPLOMATIC_AGREEMENT::FRIENDSHIP)
		return false;
	if (nType == DIPLOMATIC_AGREEMENT::TRADE && nOthersAgreement > DIPLOMATIC_AGREEMENT::AFFILIATION)
		return false;
	if (nType == DIPLOMATIC_AGREEMENT::FRIENDSHIP && nOthersAgreement > DIPLOMATIC_AGREEMENT::COOPERATION)
		return false;

	return true;
}

/// Funktion checkt die diplomatische Konsistenz und generiert bei Kündigungen auch die entsprechende Nachricht
/// für das entsprechende Imperium der Majorrace.
/// @param pDoc Zeiger auf das Dokument
void CMinor::CheckDiplomaticConsistence(CBotf2Doc* pDoc)
{
	ASSERT(pDoc);

	// alle Majors holen
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	// und durchiterieren
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		// nur wenn die Rasse bekannt ist weitermachen
		if (!IsRaceContacted(it->first))
			continue;

		// Wenn die Minorrace unterworfen wurde, so wird jeglicher Vertrag gekündigt aller bakannten Majors gekündigt
		if (GetSubjugated())
		{
			DIPLOMATIC_AGREEMENT::Typ nAgreement = GetAgreement(it->first);
			CString s = "";
			switch (nAgreement)
			{
			case DIPLOMATIC_AGREEMENT::TRADE: {s = CResourceManager::GetString("CANCEL_TRADE_AGREEMENT", FALSE, m_sName);	break;}
			case DIPLOMATIC_AGREEMENT::FRIENDSHIP: {s = CResourceManager::GetString("CANCEL_FRIENDSHIP", FALSE, m_sName);	break;}
			case DIPLOMATIC_AGREEMENT::COOPERATION: {s = CResourceManager::GetString("CANCEL_COOPERATION", FALSE, m_sName);			break;}
			case DIPLOMATIC_AGREEMENT::AFFILIATION: {s = CResourceManager::GetString("CANCEL_AFFILIATION", FALSE, m_sName);			break;}
			case DIPLOMATIC_AGREEMENT::MEMBERSHIP: {s = CResourceManager::GetString("CANCEL_MEMBERSHIP", FALSE, m_sName);				break;}
			}
			// Krieg bleibt weiterhin bestehen
			if (nAgreement != DIPLOMATIC_AGREEMENT::WAR)
			{
				// Vertrag bei beiden Rassen auflösen
				SetAgreement(it->first, DIPLOMATIC_AGREEMENT::NONE);
				CMajor* pMajor = it->second;
				pMajor->SetAgreement(m_sID, DIPLOMATIC_AGREEMENT::NONE);
				if (!s.IsEmpty())
				{
					CMessage message;
					message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY,"",0,0);
					pMajor->GetEmpire()->AddMessage(message);
				}
			}
		}

		DIPLOMATIC_AGREEMENT::Typ nMajorsAgreement = GetAgreement(it->first);
		// hat die andere Rasse eine Mitgliedschaft, dann müssen Handelsvertrag und Freundschaft von allen anderen Rassen gekündigt werden
		if (nMajorsAgreement == DIPLOMATIC_AGREEMENT::MEMBERSHIP)
		{
			for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); ++itt)
			{
				if (it->first == itt->first)
					continue;

				DIPLOMATIC_AGREEMENT::Typ nAgreement = GetAgreement(itt->first);
				CString s = "";

				if (nAgreement == DIPLOMATIC_AGREEMENT::TRADE)
					s = CResourceManager::GetString("CANCEL_TRADE_AGREEMENT", FALSE, m_sName);
				else if (nAgreement == DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					s = CResourceManager::GetString("CANCEL_FRIENDSHIP", FALSE, m_sName);
				else if (nAgreement == DIPLOMATIC_AGREEMENT::COOPERATION)
					s = CResourceManager::GetString("CANCEL_COOPERATION", FALSE, m_sName);
				else if (nAgreement == DIPLOMATIC_AGREEMENT::AFFILIATION)
					s = CResourceManager::GetString("CANCEL_AFFILIATION", FALSE, m_sName);
				else if (nAgreement == DIPLOMATIC_AGREEMENT::MEMBERSHIP)
					s = CResourceManager::GetString("CANCEL_MEMBERSHIP", FALSE, m_sName);

				if (!s.IsEmpty())
				{
					// Vertrag bei beiden Rassen auflösen
					SetAgreement(itt->first, DIPLOMATIC_AGREEMENT::NONE);
					CMajor* pMajor = itt->second;
					pMajor->SetAgreement(m_sID, DIPLOMATIC_AGREEMENT::NONE);
					CMessage message;
					message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
					pMajor->GetEmpire()->AddMessage(message);
				}
			}
		}
		else if (nMajorsAgreement == DIPLOMATIC_AGREEMENT::AFFILIATION)
		{
			for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); ++itt)
			{
				if (it->first == itt->first)
					continue;

				DIPLOMATIC_AGREEMENT::Typ nAgreement = GetAgreement(itt->first);
				CString s = "";

				if (nAgreement == DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					s = CResourceManager::GetString("CANCEL_FRIENDSHIP", FALSE, m_sName);
				else if (nAgreement == DIPLOMATIC_AGREEMENT::COOPERATION)
					s = CResourceManager::GetString("CANCEL_COOPERATION", FALSE, m_sName);
				else if (nAgreement == DIPLOMATIC_AGREEMENT::AFFILIATION)
					s = CResourceManager::GetString("CANCEL_AFFILIATION", FALSE, m_sName);

				if (!s.IsEmpty())
				{
					// Vertrag bei beiden Rassen auflösen
					SetAgreement(itt->first, DIPLOMATIC_AGREEMENT::NONE);
					CMajor* pMajor = itt->second;
					pMajor->SetAgreement(m_sID, DIPLOMATIC_AGREEMENT::NONE);
					CMessage message;
					message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
					pMajor->GetEmpire()->AddMessage(message);
				}
			}
		}
		else if (nMajorsAgreement == DIPLOMATIC_AGREEMENT::COOPERATION)
		{
			for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); ++itt)
			{
				if (it->first == itt->first)
					continue;

				DIPLOMATIC_AGREEMENT::Typ nAgreement = GetAgreement(itt->first);
				CString s = "";

				if (nAgreement == DIPLOMATIC_AGREEMENT::COOPERATION)
					s = CResourceManager::GetString("CANCEL_COOPERATION", FALSE, m_sName);

				if (!s.IsEmpty())
				{
					// Vertrag bei beiden Rassen auflösen
					SetAgreement(itt->first, DIPLOMATIC_AGREEMENT::NONE);
					CMajor* pMajor = itt->second;
					pMajor->SetAgreement(m_sID, DIPLOMATIC_AGREEMENT::NONE);
					CMessage message;
					message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
					pMajor->GetEmpire()->AddMessage(message);
				}
			}
		}
	}
}

/// Funktion überprüft die Beziehungen zu den Hauptrassen. Wenn diese zuweit vom Vertrag abweicht könnte gekündigt werden.
/// @param pDoc Zeiger auf das Dokument
void CMinor::PerhapsCancelAgreement(CBotf2Doc* pDoc)
{
	ASSERT(pDoc);

	// alle Majors holen
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	// und durchiterieren
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		// nur zu 25% wird in dieser Runde dann auch gekündigt
		if (rand()%3 != 0)
			continue;
		// nur wenn die Rasse bekannt ist weitermachen
		if (!IsRaceContacted(it->first))
			continue;

		short nRelation	= GetRelation(it->first);
		DIPLOMATIC_AGREEMENT::Typ nAgreement = GetAgreement(it->first);
		if (nRelation < nAgreement * 12 && nAgreement >= DIPLOMATIC_AGREEMENT::TRADE && nAgreement <= DIPLOMATIC_AGREEMENT::MEMBERSHIP)
		{
			// Jetzt wird gekündigt
			CString sText;
			switch (nAgreement)
			{
			case DIPLOMATIC_AGREEMENT::TRADE:
			{
				sText = CResourceManager::GetString("CANCEL_TRADE_AGREEMENT", FALSE, m_sName);
				break;
			}
			case DIPLOMATIC_AGREEMENT::FRIENDSHIP:
			{
				sText = CResourceManager::GetString("CANCEL_FRIENDSHIP", FALSE, m_sName);
				break;
			}
			case DIPLOMATIC_AGREEMENT::COOPERATION:
			{
				sText = CResourceManager::GetString("CANCEL_COOPERATION", FALSE, m_sName);
				break;
			}
			case DIPLOMATIC_AGREEMENT::AFFILIATION:
			{
				sText = CResourceManager::GetString("CANCEL_AFFILIATION", FALSE, m_sName);
				break;
			}
			case DIPLOMATIC_AGREEMENT::MEMBERSHIP:
			{
				sText = CResourceManager::GetString("CANCEL_MEMBERSHIP", FALSE, m_sName);
				break;
			}
			}

			if (!sText.IsEmpty())
			{
				CMajor* pMajor = it->second;
				pMajor->SetAgreement(m_sID, DIPLOMATIC_AGREEMENT::NONE);
				SetAgreement(it->first, DIPLOMATIC_AGREEMENT::NONE);
				CMessage message;
				message.GenerateMessage(sText, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
				pMajor->GetEmpire()->AddMessage(message);
			}
		}
	}
}

/// Funktion erstellt eine Tooltipinfo der Rasse.
/// @return	der erstellte Tooltip-Text
CString CMinor::GetTooltip(void) const
{
	CString sTip = __super::GetTooltip();
	sTip += CHTMLStringBuilder::GetHTMLStringNewLine();

	// Technischen Fortschritt anzeigen
	CString sProgress(CResourceManager::GetString("TECHNICAL_PROGRESS"));
	sProgress = CHTMLStringBuilder::GetHTMLColor(sProgress, _T("silver"));
	sProgress = CHTMLStringBuilder::GetHTMLHeader(sProgress, _T("h4"));
	sProgress += CHTMLStringBuilder::GetHTMLStringNewLine();
	sProgress += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sProgress += CHTMLStringBuilder::GetHTMLStringNewLine();

	CString s = "";
	switch (GetTechnologicalProgress())
	{
	case 0: s = CResourceManager::GetString("VERY_UNDERDEVELOPED");	break;
	case 1: s = CResourceManager::GetString("UNDERDEVELOPED");		break;
	case 2: s = CResourceManager::GetString("NORMAL_DEVELOPED");	break;
	case 3: s = CResourceManager::GetString("DEVELOPED");			break;
	case 4: s = CResourceManager::GetString("VERY_DEVELOPED");		break;
	}

	s = CHTMLStringBuilder::GetHTMLColor(s);
	s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h5"));
	sProgress += s;
	sProgress += CHTMLStringBuilder::GetHTMLStringNewLine();
	sProgress += CHTMLStringBuilder::GetHTMLStringNewLine();

	// Bestechlichkeit anzeigen
	CString sCor(CResourceManager::GetString("CORRUPTIBILITY"));
	sCor = CHTMLStringBuilder::GetHTMLColor(sCor, _T("silver"));
	sCor = CHTMLStringBuilder::GetHTMLHeader(sCor, _T("h4"));
	sCor += CHTMLStringBuilder::GetHTMLStringNewLine();
	sCor += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sCor += CHTMLStringBuilder::GetHTMLStringNewLine();

	s = "";
	switch (GetCorruptibility())
	{
	case 0: s = CResourceManager::GetString("VERY_LOW_CORRUPTIBILITY");	break;
	case 1: s = CResourceManager::GetString("LOW_CORRUPTIBILITY");		break;
	case 2: s = CResourceManager::GetString("NORMAL_CORRUPTIBILITY");	break;
	case 3: s = CResourceManager::GetString("HIGH_CORRUPTIBILITY");		break;
	case 4: s = CResourceManager::GetString("VERY_HIGH_CORRUPTIBILITY");break;
	}
	s = CHTMLStringBuilder::GetHTMLColor(s);
	s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h5"));
	sCor += s;
	sCor += CHTMLStringBuilder::GetHTMLStringNewLine();

	return CHTMLStringBuilder::GetHTMLCenter(sTip + sProgress + sCor);
}

/// Funktion zum erstellen einer Rasse.
/// Die Funktion liest einen entsprechenden Eintrag aus einer data Datei.
/// @param saInfo Referenz auf Rasseninformationen
/// @param nPos Referenz auf Position im Array, ab wann die Informationen gelten
void CMinor::Create(const CStringArray& saInfo, int& nPos)
{
	ASSERT(nPos >= 0);

	Reset();

	// Minorrace nun anlegen
	m_sID				= saInfo[nPos++];				// Rassen-ID
	m_sID.Remove(':');
	m_sHomeSystem		= m_sID;						// Name des Heimatsystems
	m_sHomeSystem.MakeLower();
	CString sChar = (CString)m_sHomeSystem.GetAt(0);
	m_sHomeSystem.SetAt(0, sChar.MakeUpper().GetAt(0));

	m_sName				= saInfo[nPos++];				// Rassenname
	m_sDesc				= saInfo[nPos++];				// Rassenbeschreibung

	// grafische Attribute
	m_sGraphicFile				= saInfo[nPos++];		// Name der zugehörigen Grafikdatei

	// Beziehungen
	// ALPHA5 muss natürlich noch auf das neue System umgestellt werden -> werden nicht mehr in der data Datei gespeichert
	//for (int i = 0; i < 6; i++)
	//{
	//	int ALPHA5 = atoi(saInfo[nPos++]);
	//}
	//TODO
	//relations from the minor data file aren't used
	nPos = nPos + 6;

	m_iTechnologicalProgress = atoi(saInfo[nPos++]);
	m_RaceType			= RACE_TYPE_MINOR;						// Rassentyp (Major, Medior, Minor)

	// mehrere Rasseneigenschaften sind durch Komma getrennt
	CString sRaceProperties = saInfo[nPos++];
	int nStart = 0;
	while (nStart < sRaceProperties.GetLength())
	{
		RACE_PROPERTY::Typ nProperty = (RACE_PROPERTY::Typ)atoi(sRaceProperties.Tokenize(",", nStart));
		ASSERT(nProperty >= RACE_PROPERTY::NOTHING_SPECIAL && nProperty <= RACE_PROPERTY::HOSTILE);
		SetRaceProperty(nProperty, true);				// Rasseneigenschaften
	}

	m_bSpaceflight		= atoi(saInfo[nPos++]) == 0 ? false : true;
	m_iCorruptibility	= atoi(saInfo[nPos++]);


	// Schiffsnummer vergeben
	m_byShipNumber = MINORNUMBER;

	// Minorrace - KI anlegen
	m_pDiplomacyAI = new CMinorAI(this);
}

/// Funktion zum erstellen von Weltraummonstern
/// Die Funktion liest einen entsprechenden Eintrag aus einer data Datei.
/// @param saInfo Referenz auf Rasseninformationen
/// @param nPos Referenz auf Position im Array, ab wann die Informationen gelten
void CMinor::CreateAlienEntities(const CStringArray& saInfo, int& nPos)
{
	ASSERT(nPos >= 0);

	Reset();

	// Minorrace nun anlegen
	m_sID				= saInfo[nPos++];				// Rassen-ID
	m_sID.Remove(':');
	m_sName				= saInfo[nPos++];				// Rassenname
	m_sDesc				= saInfo[nPos++];				// Rassenbeschreibung

	// grafische Attribute
	m_sGraphicFile		= saInfo[nPos++];				// Name der zugehörigen Grafikdatei
	m_iTechnologicalProgress = atoi(saInfo[nPos++]);
	m_RaceType			= RACE_TYPE_MINOR;						// Rassentyp (Major, Medior, Minor)

	// mehrere Rasseneigenschaften sind durch Komma getrennt
	CString sRaceProperties = saInfo[nPos++];
	int nStart = 0;
	while (nStart < sRaceProperties.GetLength())
	{
		RACE_PROPERTY::Typ nProperty = (RACE_PROPERTY::Typ)atoi(sRaceProperties.Tokenize(",", nStart));
		ASSERT(nProperty >= RACE_PROPERTY::NOTHING_SPECIAL && nProperty <= RACE_PROPERTY::HOSTILE);
		SetRaceProperty(nProperty, true);				// Rasseneigenschaften
	}
	m_nSpecialAbility	= atoi(saInfo[nPos++]);

	m_bSpaceflight		= atoi(saInfo[nPos++]) == 0 ? false : true;
	m_iCorruptibility	= atoi(saInfo[nPos++]);

	// Schiffsnummer vergeben
	m_byShipNumber = MINORNUMBER;

	// Minorrace - KI anlegen
	m_pDiplomacyAI = new CMinorAI(this);
}

/// Funktion zum zurücksetzen aller Werte auf Ausgangswerte.
void CMinor::Reset(void)
{
	CRace::Reset();

	m_ptKO = CPoint(-1,-1);					// Koordinaten der Rasse
	m_iTechnologicalProgress = 0;			// wie fortschrittlich ist die Minorrace?
	m_iCorruptibility = 0;					// wie stark ändert sich die Beziehung beim Geschenke geben?
	m_bSpaceflight = false;					// Spaceflightnation (hat Schiffe)
	m_bSubjugated = false;					// wurde die Rasse unterworfen
	m_mAcceptance.clear();					// Punkte die eine MajorRace durch längere Beziehung mit der Rasse ansammelt, wird schwerer diese Rasse wegzukaufen
}

void CMinor::Contact(const CRace& Race, const CPoint& p) {
	CRace::Contact(Race, p);
}

bool CMinor::IsMemberTo(const CString& MajorID) const {
	if(!MajorID.IsEmpty())
		return GetAgreement(MajorID) == DIPLOMATIC_AGREEMENT::MEMBERSHIP;

	for(std::map<CString, DIPLOMATIC_AGREEMENT::Typ>::const_iterator it = m_mAgreement.begin();
		it != m_mAgreement.end(); ++it) {
		if(it->second == DIPLOMATIC_AGREEMENT::MEMBERSHIP)
			return true;
	}
	return false;
}
