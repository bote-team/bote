#include "stdafx.h"
#include "DiplomacyController.h"
#include "RaceController.h"
#include "GenDiploMessage.h"
#include "botf2.h"
#include "Botf2Doc.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CDiplomacyController::CDiplomacyController(void)
{	
}

CDiplomacyController::~CDiplomacyController(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion zum Versenden von diplomatischen Angeboten
void CDiplomacyController::Send(void)
{
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);

	std::map<CString, CRace*>* races = pDoc->GetRaceCtrl()->GetRaces();
	ASSERT(races);

	// KI Angebote erstellen lassen
	for (map<CString, CRace*>::const_iterator it = races->begin(); it != races->end(); ++it)
	{
		CRace* pRace = it->second;
		ASSERT(pRace);

		pRace->MakeOffersAI();
	}

	// durch alle Rassen iterieren und Angebote versenden
	for (map<CString, CRace*>::const_iterator it = races->begin(); it != races->end(); ++it)
	{
		CRace* pRace = it->second;
		ASSERT(pRace);

		// alle eingegangenen Antworten aus der letzten Runde löschen
		for (UINT i = 0; i < pRace->GetIncomingDiplomacyNews()->size(); i++)
		{
			CDiplomacyInfo* pInfo = &(pRace->GetIncomingDiplomacyNews()->at(i));
			if (pInfo->m_nFlag == DIPLOMACY_ANSWER)
				pRace->GetIncomingDiplomacyNews()->erase(pRace->GetIncomingDiplomacyNews()->begin() + i--);	
		}

		// nun durch alle ausgehenden Nachrichten iterieren
		for (UINT i = 0; i < pRace->GetOutgoingDiplomacyNews()->size(); i++)
		{
			CDiplomacyInfo* pInfo = &(pRace->GetOutgoingDiplomacyNews()->at(i));
			// exisitiert die Zielrasse?
			if (races->find(pInfo->m_sToRace) != races->end())
			{
				CRace* pToRace = (*races)[pInfo->m_sToRace];
				// Angebote senden
				if (pToRace->GetType() == MAJOR)
				{
					SendToMajor(pDoc, (CMajor*)pToRace, pInfo);
				}
				else if (pToRace->GetType() == MINOR)
				{
					SendToMinor(pDoc, (CMinor*)pToRace, pInfo);
				}				
			}			
		}

		// alle ausgehenden Nachrichten werden gelöscht
		pRace->GetOutgoingDiplomacyNews()->clear();		
	}	
}

/// Funktion zum Empfangen und Bearbeiten eines diplomatischen Angebots.
void CDiplomacyController::Receive(void)
{
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);

	std::map<CString, CRace*>* races = pDoc->GetRaceCtrl()->GetRaces();
	ASSERT(races);

	// durch alle Rassen iterieren und Nachrichten empfangen sowie darauf reagieren
	for (map<CString, CRace*>::const_iterator it = races->begin(); it != races->end(); ++it)
	{
		CRace* pRace = it->second;
		ASSERT(pRace);

		// nun durch alle eingegangen Nachrichten iterieren und darauf reagieren
		for (UINT i = 0; i < pRace->GetIncomingDiplomacyNews()->size(); i++)
		{
			CDiplomacyInfo* pInfo = &(pRace->GetIncomingDiplomacyNews()->at(i));
			// exisitiert die Rasse welche die Nachricht gesendet hat?
			if (races->find(pInfo->m_sFromRace) != races->end())
			{
				CRace* pToRace = (*races)[pInfo->m_sToRace];
				if (pToRace->GetType() == MAJOR)
				{
					ReceiveToMajor(pDoc, (CMajor*)pToRace, pInfo);
				}
				else if (pToRace->GetType() == MINOR)
				{
					ReceiveToMinor(pDoc, (CMinor*)pToRace, pInfo);
				}				
			}			
		}

		// alle eingegangenen Angebote aus der letzten Runde löschen
		for (UINT i = 0; i < pRace->GetIncomingDiplomacyNews()->size(); i++)
		{
			CDiplomacyInfo* pInfo = &(pRace->GetIncomingDiplomacyNews()->at(i));
			if (pInfo->m_nSendRound < pDoc->GetCurrentRound() - 1)
				pRace->GetIncomingDiplomacyNews()->erase(pRace->GetIncomingDiplomacyNews()->begin() + i--);
		}
	}

	// die diplomatische Konsistenz überprüfen und Auswirkungen beachten
	CalcDiplomacyFallouts(pDoc);
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion überprüft die diplomatische Konsistenz und berechnet die direkten diplomatischen Auswirkungen.
/// Sie sollte nach <func>Receive</func> aufgerufen werden.
/// Stellt die Funktion Probleme fest, so werden diese automatisch behoben.
/// @param pDoc Zeiger auf das Dokument
void CDiplomacyController::CalcDiplomacyFallouts(CBotf2Doc* pDoc)
{
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	map<CString, CMinor*>* pmMinors = pDoc->GetRaceCtrl()->GetMinors();
	
	for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); ++it)
	{
		CMinor* pMinor = it->second;
		
		// Die diplomatische Konsistenz überprüfen! z.B kann niemand mehr eine Freundschaft haben, wenn jemand eine
		// Mitgliedschaft mit dieser Rasse hat. Verträge werden gekündigt, wenn die Minorrace unterworfen wurde
		pMinor->CheckDiplomaticConsistence(pDoc);
		pMinor->PerhapsCancelAgreement(pDoc);
		if (pMinor->GetRaceKO() == CPoint(-1,-1))
			continue;

		for (map<CString, CMajor*>::const_iterator jt = pmMajors->begin(); jt != pmMajors->end(); ++jt)
		{
			CMajor* pMajor = jt->second;

			// Wenn wir mit der Minorrace mindst. einen Handelsvertrag abgeschlossen haben, dann wird deren Sector gescannt/gesehen
			if (pMinor->GetAgreement(pMajor->GetRaceID()) >= DIPLOMATIC_AGREEMENT::TRADE)
				pDoc->GetSector(pMinor->GetRaceKO()).SetScanned(pMajor->GetRaceID());
			// Wenn wir mit der Minorrace mindst. einen Freundschaftsvertrag abgeschlossen haben, dann wird deren Sector bekannt
			if (pMinor->GetAgreement(pMajor->GetRaceID()) >= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
				pDoc->GetSector(pMinor->GetRaceKO()).SetKnown(pMajor->GetRaceID());
			// Wenn wir eine Mitgliedschaft mit der kleinen Rasse haben und das System noch der kleinen Rasse gehört, dann bekommen wir das
			if (pMinor->GetAgreement(pMajor->GetRaceID()) == DIPLOMATIC_AGREEMENT::MEMBERSHIP && pDoc->GetSystem(pMinor->GetRaceKO()).GetOwnerOfSystem() == "")
			{
				pDoc->GetSector(pMinor->GetRaceKO()).SetFullKnown(pMajor->GetRaceID());
				pDoc->GetSystem(pMinor->GetRaceKO()).SetOwnerOfSystem(pMajor->GetRaceID());
				pDoc->GetSector(pMinor->GetRaceKO()).SetOwnerOfSector(pMajor->GetRaceID());
				pDoc->GetSector(pMinor->GetRaceKO()).SetOwned(TRUE);
				// Der Sector gilt jetzt als nicht eingenommen
				pDoc->GetSector(pMinor->GetRaceKO()).SetTakenSector(FALSE);
				// Nun Gebäude in neuen System bauen
				pDoc->GetSystem(pMinor->GetRaceKO()).BuildBuildingsForMinorRace(&pDoc->GetSector(pMinor->GetRaceKO()), &pDoc->BuildingInfo, pDoc->GetStatistics()->GetAverageTechLevel(), pMinor);
				// Gebäude so weit wie möglich mit Arbeitern besetzen
				pDoc->GetSystem(pMinor->GetRaceKO()).SetWorkersIntoBuildings();
				// alle Schiffe der Minor gehen nun an den Major
				for (int i = 0; i < pDoc->m_ShipArray.GetSize(); i++)
				{
					CShip* pShip = &(pDoc->m_ShipArray[i]);
					if (pShip->GetOwnerOfShip() == pMinor->GetRaceID())
					{
						pShip->SetOwnerOfShip(pMajor->GetRaceID());
						// Schiff in die Shiphistory stecken
						pMajor->GetShipHistory()->AddShip(pShip, pDoc->GetSector(pMinor->GetRaceKO()).GetName(true), pDoc->GetCurrentRound());
					}
				}
			}
		}
	}

	// Hat die Rasse die Mitgliedschaft gekündigt, wurde Bestochen oder irgendein anderer Grund ist dafür
	// verantwortlich, warum eine Major plötzlich nicht mehr Zugriff auf das System der Minor hat.
	for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); ++it)
	{
		CMinor* pMinor = it->second;
		if (pMinor->GetRaceKO() == CPoint(-1,-1))
			continue;
	
		CString sOwner = pDoc->GetSystem(pMinor->GetRaceKO()).GetOwnerOfSystem();
		if (sOwner != "")
		{
			// Wenn wir eine Mitgliedschaft bei der kleinen Rasse hatten, sprich uns das System noch gehört, wir aber
			// der kleinen Rasse den Krieg erklären bzw. den Vertrag aufheben (warum auch immer?!?) und das System nicht
			// gewaltätig erobert wurde, dann gehört uns das System auch nicht mehr
			if (pMinor->GetAgreement(sOwner) != DIPLOMATIC_AGREEMENT::MEMBERSHIP && pDoc->GetSector(pMinor->GetRaceKO()).GetMinorRace() == TRUE &&
				pDoc->GetSector(pMinor->GetRaceKO()).GetTakenSector() == FALSE)
			{
				pDoc->GetSector(pMinor->GetRaceKO()).SetOwned(false);
				pDoc->GetSector(pMinor->GetRaceKO()).SetOwnerOfSector(pMinor->GetRaceID());
				pDoc->GetSystem(pMinor->GetRaceKO()).SetOwnerOfSystem("");
				CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(sOwner));
				if (pMajor)
				{
					int nCount = pMajor->GetEmpire()->GetNumberOfSystems();
					pMajor->GetEmpire()->SetNumberOfSystems(nCount - 1);
				}
			}
		}
	}
}

/// Funktion zum Versenden von diplomatischen Angeboten an eine Majorrace.
/// @param pDoc Zeiger auf das Dokument
/// @param pToMajor Zeiger auf die Empfängerrasse
/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
void CDiplomacyController::SendToMajor(CBotf2Doc* pDoc, CMajor* pToMajor, CDiplomacyInfo* pInfo)
{
	// nur Reaktion beim diplomatischen Angebot
	if (pInfo->m_nFlag != DIPLOMACY_OFFER)
		return;

	// Die Rasse welche die Nachricht abgeschickt hat
	CRace* pFromRace = pDoc->GetRaceCtrl()->GetRace(pInfo->m_sFromRace);
	if (!pFromRace)
		return;

	////////////////////////////////////////////////////////////////////////////////////////
	// das Angebot geht an einen Major
	////////////////////////////////////////////////////////////////////////////////////////	
	CMajor* pWarpactEnemy = NULL;
	if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::WARPACT)
	{
		pWarpactEnemy = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(pInfo->m_sWarpactEnemy));
		if (!pWarpactEnemy)
			return;
	}
	
	// Imperiumsnamen mit bestimmten Artikel holen
	CString sEmpireAssignedArticleName = pToMajor->GetEmpireNameWithAssignedArticle();
	CString sEmpireArticleName = "";
	if (pFromRace->GetType() == MAJOR)
	{
		// Imperiumsnamen inkl. Artikel holen
		sEmpireArticleName = ((CMajor*)pFromRace)->GetEmpireNameWithArticle();
		// groß beginnen
		CString sUpper = (CString)sEmpireArticleName.GetAt(0);
		sEmpireArticleName.SetAt(0, sUpper.MakeUpper().GetAt(0));
	}

	CString sAgreement;
	switch (pInfo->m_nType)
	{
	case DIPLOMATIC_AGREEMENT::TRADE:		{sAgreement = CResourceManager::GetString("TRADE_AGREEMENT_WITH_ARTICLE");	break;}
	case DIPLOMATIC_AGREEMENT::FRIENDSHIP:	{sAgreement = CResourceManager::GetString("FRIENDSHIP_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::COOPERATION:	{sAgreement = CResourceManager::GetString("COOPERATION_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::AFFILIATION:	{sAgreement = CResourceManager::GetString("AFFILIATION_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::MEMBERSHIP:	{sAgreement = CResourceManager::GetString("MEMBERSHIP_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::NAP:			{sAgreement = CResourceManager::GetString("NON_AGGRESSION_WITH_ARTICLE");	break;}
	case DIPLOMATIC_AGREEMENT::DEFENCEPACT:	{sAgreement = CResourceManager::GetString("DEFENCE_PACT_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::WARPACT:		{sAgreement = CResourceManager::GetString("WAR_PACT_WITH_ARTICLE", FALSE, pWarpactEnemy->GetRaceName()); break;}
	case DIPLOMATIC_AGREEMENT::REQUEST:		{sAgreement = CResourceManager::GetString("REQUEST_WITH_ARTICLE");			break;}
	}
	
	CMessage message;

	// Wenn wir sozusagen eine Runde gewartet haben, dann Nachricht machen, das wir ein Geschenk gemacht haben
	if (pInfo->m_nSendRound == pDoc->GetCurrentRound() - 1)
	{
		if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::PRESENT)
		{
			if (pFromRace->GetType() == MAJOR)
			{
				CString s = CResourceManager::GetString("WE_GIVE_PRESENT", FALSE, sEmpireAssignedArticleName);
				message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
				((CMajor*)pFromRace)->GetEmpire()->AddMessage(message);

				s = CResourceManager::GetString("WE_GET_PRESENT", FALSE, sEmpireArticleName);
				message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
				pToMajor->GetEmpire()->AddMessage(message);
				// Die Credits des Geschenkes gutschreiben
				pToMajor->GetEmpire()->SetCredits(pInfo->m_nCredits);
				// Die Rohstoffe des Geschenkes gutschreiben
				for (int k = TITAN; k <= IRIDIUM; k++)
					if (pInfo->m_nResources[k] > 0)
						pToMajor->GetEmpire()->GetGlobalStorage()->AddRessource(pInfo->m_nResources[k], k, CPoint(-1,-1));
				// Deritium kommt nicht ins globale Lager sondern ins Heimatsystem
				if (pInfo->m_nResources[DERITIUM] > 0)
				{
					CPoint p = pDoc->GetRaceKO(pToMajor->GetRaceID());
					// gehört das System auch noch dem Major
					if (p != CPoint(-1,-1) && pDoc->GetSystem(p).GetOwnerOfSystem() == pToMajor->GetRaceID())
						pDoc->GetSystem(p).SetDeritiumStore(pInfo->m_nResources[DERITIUM]);
				}

				// Angebot in den Nachrichteneingang legen
				pToMajor->GetIncomingDiplomacyNews()->push_back(*pInfo);
			}
		}
		// handelt es sich um eine Forderung
		else if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::REQUEST)
		{
			if (pFromRace->GetType() == MAJOR)
			{
				CString s = CResourceManager::GetString("WE_HAVE_REQUEST", FALSE, sEmpireAssignedArticleName, sAgreement);
				message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
				((CMajor*)pFromRace)->GetEmpire()->AddMessage(message);

				s = CResourceManager::GetString("WE_GET_REQUEST", FALSE, sEmpireArticleName, sAgreement);
				message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
				pToMajor->GetEmpire()->AddMessage(message);

				// Angebot in den Nachrichteneingang legen
				pToMajor->GetIncomingDiplomacyNews()->push_back(*pInfo);
			}
		}
		// handelt es sich um eine Kriegserklärung
		else if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::WAR)
		{
			// Krieg erklären
			DeclareWar(pFromRace, pToMajor, pInfo, true);
			
			// aufgrund diplomatischer Beziehungen könnte so weiter Krieg erklärt werden
			std::vector<CString> vEnemies;
			if (pFromRace->GetType() == MAJOR)
				vEnemies = GetEnemiesFromContract(pDoc, (CMajor*)pFromRace, pToMajor);
			// allen weiteren Gegnern den Krieg erklären
			for (UINT i = 0; i < vEnemies.size(); i++)
			{
				CRace* pEnemy = pDoc->GetRaceCtrl()->GetRace(vEnemies[i]);
				if (pEnemy)
				{
					CDiplomacyInfo war = *pInfo;
					// lediglich die Zielrasse und den Kriegsgrund anpassen
					war.m_sToRace		= pEnemy->GetRaceID();
					war.m_sWarPartner	= pToMajor->GetRaceID();
					DeclareWar(pFromRace, pEnemy, &war, false);
				}
			}
		}		
		// sonstige Vertragangebote
		else
		{
			// das Angebot stammt von einem Major
			if (pFromRace->GetType() == MAJOR)
			{
				CString s = CResourceManager::GetString("WE_MAKE_MAJ_OFFER", FALSE, sEmpireAssignedArticleName, sAgreement);
				message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
				((CMajor*)pFromRace)->GetEmpire()->AddMessage(message);

				s = CResourceManager::GetString("WE_GET_MAJ_OFFER", FALSE, sEmpireArticleName, sAgreement);
				message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
				pToMajor->GetEmpire()->AddMessage(message);
			}
			// das Angebot stammt von einem Minor
			else if (pFromRace->GetType() == MINOR)
			{
				CString s = "";
				switch (pInfo->m_nType)
				{
				case DIPLOMATIC_AGREEMENT::TRADE:		s = CResourceManager::GetString("MIN_OFFER_TRADE", FALSE, pFromRace->GetRaceName());	break;
				case DIPLOMATIC_AGREEMENT::FRIENDSHIP:s = CResourceManager::GetString("MIN_OFFER_FRIEND", FALSE, pFromRace->GetRaceName());	break;
				case DIPLOMATIC_AGREEMENT::COOPERATION:			s = CResourceManager::GetString("MIN_OFFER_COOP", FALSE, pFromRace->GetRaceName());		break;
				case DIPLOMATIC_AGREEMENT::AFFILIATION:			s = CResourceManager::GetString("MIN_OFFER_AFFI", FALSE, pFromRace->GetRaceName());		break;
				case DIPLOMATIC_AGREEMENT::MEMBERSHIP:			s = CResourceManager::GetString("MIN_OFFER_MEMBER", FALSE, pFromRace->GetRaceName());	break;
				}				
				message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
				pToMajor->GetEmpire()->AddMessage(message);
			}
			// Angebot in den Nachrichteneingang legen
			pToMajor->GetIncomingDiplomacyNews()->push_back(*pInfo);
		}
	}	
}

/// Funktion zum Empfangen und Bearbeiten eines diplomatischen Angebots für eine Majorrace.
/// @param pDoc Zeiger auf das Dokument
/// @param pToMajor Zeiger auf die Empfängerrasse
/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
void CDiplomacyController::ReceiveToMajor(CBotf2Doc* pDoc, CMajor* pToMajor, CDiplomacyInfo* pInfo)
{
	// nur Reaktion beim diplomatischen Angebot
	if (pInfo->m_nFlag != DIPLOMACY_OFFER)
		return;

	// Die Rasse welche die Nachricht abgeschickt hat
	CRace* pFromRace = pDoc->GetRaceCtrl()->GetRace(pInfo->m_sFromRace);
	if (!pFromRace)
		return;

	////////////////////////////////////////////////////////////////////////////////////////
	// das Angebot geht an einen Major
	////////////////////////////////////////////////////////////////////////////////////////	
	CMajor* pWarpactEnemy = NULL;
	if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::WARPACT)
	{
		pWarpactEnemy = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(pInfo->m_sWarpactEnemy));
		if (!pWarpactEnemy)
			return;
	}

	CString sAgreement;
	switch (pInfo->m_nType)
	{
	case DIPLOMATIC_AGREEMENT::TRADE:		{sAgreement = CResourceManager::GetString("TRADE_AGREEMENT_WITH_ARTICLE");	break;}
	case DIPLOMATIC_AGREEMENT::FRIENDSHIP:	{sAgreement = CResourceManager::GetString("FRIENDSHIP_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::COOPERATION:	{sAgreement = CResourceManager::GetString("COOPERATION_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::AFFILIATION:	{sAgreement = CResourceManager::GetString("AFFILIATION_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::MEMBERSHIP:	{sAgreement = CResourceManager::GetString("MEMBERSHIP_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::NAP:			{sAgreement = CResourceManager::GetString("NON_AGGRESSION_WITH_ARTICLE");	break;}
	case DIPLOMATIC_AGREEMENT::DEFENCEPACT:	{sAgreement = CResourceManager::GetString("DEFENCE_PACT_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::WARPACT:		{sAgreement = CResourceManager::GetString("WAR_PACT_WITH_ARTICLE", FALSE, pWarpactEnemy->GetRaceName()); break;}
	case DIPLOMATIC_AGREEMENT::REQUEST:		{sAgreement = CResourceManager::GetString("REQUEST_WITH_ARTICLE");			break;}
	}
	
	CMessage message;

	if (pInfo->m_nSendRound == pDoc->GetCurrentRound() - 2)
	{
		// Nur wenn der Computer die Rasse spielt
		if (!pToMajor->IsHumanPlayer())
			pToMajor->ReactOnOfferAI(pInfo);
			
		// Antwort der Majorrace erzeugen
		if (pInfo->m_nType != DIPLOMATIC_AGREEMENT::PRESENT && pInfo->m_nType != DIPLOMATIC_AGREEMENT::WAR)
		{
			CDiplomacyInfo answer = *pInfo;				
			answer.m_nSendRound = pDoc->GetCurrentRound();
			CGenDiploMessage::GenerateMajorAnswer(answer);
			
			// Die Antwort geht an einen Major
			if (pFromRace->GetType() == MAJOR)
			{
				pFromRace->GetIncomingDiplomacyNews()->push_back(answer);

				// Nachricht über Vertragsannahme oder Ablehnung
				if (answer.m_nType != DIPLOMATIC_AGREEMENT::REQUEST && answer.m_nType != DIPLOMATIC_AGREEMENT::WARPACT)
				{
					CString sAgreement2;
					switch (answer.m_nType)
					{
						case DIPLOMATIC_AGREEMENT::TRADE:		{sAgreement2 = CResourceManager::GetString("TRADE_AGREEMENT");	break;}
						case DIPLOMATIC_AGREEMENT::FRIENDSHIP:	{sAgreement2 = CResourceManager::GetString("FRIENDSHIP");		break;}
						case DIPLOMATIC_AGREEMENT::COOPERATION:	{sAgreement2 = CResourceManager::GetString("COOPERATION");		break;}
						case DIPLOMATIC_AGREEMENT::AFFILIATION:	{sAgreement2 = CResourceManager::GetString("AFFILIATION");		break;}
						case DIPLOMATIC_AGREEMENT::NAP:			{sAgreement2 = CResourceManager::GetString("NON_AGGRESSION");	break;}
						case DIPLOMATIC_AGREEMENT::DEFENCEPACT:	{sAgreement2 = CResourceManager::GetString("DEFENCE_PACT");		break;}
					}

					// Das Angebot wurde angenommen
					if (answer.m_nAnswerStatus == ANSWER_STATUS::ACCEPTED)
					{
						CString s  = CResourceManager::GetString("WE_ACCEPT_MAJ_OFFER", FALSE, sAgreement, ((CMajor*)pFromRace)->GetEmpireNameWithAssignedArticle());
						CString s2 = CResourceManager::GetString("MAJ_ACCEPT_OFFER", TRUE, pToMajor->GetEmpireNameWithArticle(), sAgreement2);
					
						// Diplomatischen Status festlegen
						pFromRace->SetAgreement(pToMajor->GetRaceID(), answer.m_nType);
						pToMajor->SetAgreement(pFromRace->GetRaceID(), answer.m_nType);
						if (answer.m_nType != DIPLOMATIC_AGREEMENT::DEFENCEPACT)
						{
							((CMajor*)pFromRace)->SetAgreementDuration(pToMajor->GetRaceID(), answer.m_nDuration);
							pToMajor->SetAgreementDuration(pFromRace->GetRaceID(), answer.m_nDuration);
						}
						else
						{
							((CMajor*)pFromRace)->SetDefencePactDuration(pToMajor->GetRaceID(), answer.m_nDuration);
							pToMajor->SetDefencePactDuration(pFromRace->GetRaceID(), answer.m_nDuration);
						}

						message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
						pToMajor->GetEmpire()->AddMessage(message);
						message.GenerateMessage(s2, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
						((CMajor*)pFromRace)->GetEmpire()->AddMessage(message);

						// zusätzliche Eventnachricht wegen der Moral an die Imperien
						CString sEventText = "";
						CString sEventText2 = "";
						
						switch (answer.m_nType)
						{
						// Sign Trade Treaty #34
						case DIPLOMATIC_AGREEMENT::TRADE:
							{	
								sEventText = pToMajor->GetMoralObserver()->AddEvent(34, pToMajor->GetRaceMoralNumber(), ((CMajor*)pFromRace)->GetEmpireNameWithAssignedArticle());
								sEventText2 = ((CMajor*)pFromRace)->GetMoralObserver()->AddEvent(34, ((CMajor*)pFromRace)->GetRaceMoralNumber(), pToMajor->GetEmpireNameWithAssignedArticle());
								break;
							}
						// Sign Friendship/Cooperation Treaty #35
						case DIPLOMATIC_AGREEMENT::FRIENDSHIP:
							{
								sEventText = pToMajor->GetMoralObserver()->AddEvent(35, pToMajor->GetRaceMoralNumber(), ((CMajor*)pFromRace)->GetEmpireNameWithAssignedArticle());
								sEventText2 = ((CMajor*)pFromRace)->GetMoralObserver()->AddEvent(35, ((CMajor*)pFromRace)->GetRaceMoralNumber(), pToMajor->GetEmpireNameWithAssignedArticle());
								break;
							}
						// Sign Friendship/Cooperation Treaty #35
						case DIPLOMATIC_AGREEMENT::COOPERATION:
							{
								sEventText = pToMajor->GetMoralObserver()->AddEvent(35, pToMajor->GetRaceMoralNumber(), ((CMajor*)pFromRace)->GetEmpireNameWithAssignedArticle());
								sEventText2 = ((CMajor*)pFromRace)->GetMoralObserver()->AddEvent(35, ((CMajor*)pFromRace)->GetRaceMoralNumber(), pToMajor->GetEmpireNameWithAssignedArticle());
								break;
							}
						// Sign an Affiliation Treaty #36
						case DIPLOMATIC_AGREEMENT::AFFILIATION:
							{
								sEventText = pToMajor->GetMoralObserver()->AddEvent(36, pToMajor->GetRaceMoralNumber(), ((CMajor*)pFromRace)->GetEmpireNameWithAssignedArticle());
								sEventText2 = ((CMajor*)pFromRace)->GetMoralObserver()->AddEvent(36, ((CMajor*)pFromRace)->GetRaceMoralNumber(), pToMajor->GetEmpireNameWithAssignedArticle());
								break;
							}
						}
						if (!sEventText.IsEmpty())
						{
							message.GenerateMessage(sEventText, MESSAGE_TYPE::SOMETHING, "", 0, 0);
							pToMajor->GetEmpire()->AddMessage(message);						
						}
						if (!sEventText2.IsEmpty())
						{
							message.GenerateMessage(sEventText2, MESSAGE_TYPE::SOMETHING, "", 0, 0);
							((CMajor*)pFromRace)->GetEmpire()->AddMessage(message);						
						}

						// Die möglicherweise dazugegebenen Credits und die Ressourcen gutschreiben.
						pToMajor->GetEmpire()->SetCredits(answer.m_nCredits);
						for (int res = TITAN; res <= IRIDIUM; res++)
							pToMajor->GetEmpire()->GetGlobalStorage()->AddRessource(answer.m_nResources[res], res, CPoint(-1,-1));
						// Deritium kommt nicht ins globale Lager sondern ins Heimatsystem
						if (pInfo->m_nResources[DERITIUM] > 0)
						{
							CPoint p = pDoc->GetRaceKO(pToMajor->GetRaceID());
							// gehört das System auch noch dem Major
							if (p != CPoint(-1,-1) && pDoc->GetSystem(p).GetOwnerOfSystem() == pToMajor->GetRaceID())
								pDoc->GetSystem(p).SetDeritiumStore(pInfo->m_nResources[DERITIUM]);
						}

						// Beziehungsverbesserung
						pToMajor->SetRelation(pFromRace->GetRaceID(), abs(answer.m_nType * 2));
						pFromRace->SetRelation(pToMajor->GetRaceID(), abs(answer.m_nType * 2));
						
						// Wenn die Beziehung zu schlecht ist, z.B. nachdem ein Krieg war und ein Nichtangriffspakt nachher
						// angenommen wurde, so die Beziehung auf ein Minimum setzen.
						if (pToMajor->GetRelation(pFromRace->GetRaceID()) < (answer.m_nType + 3) * 10)
						{
							int add = ((answer.m_nType + 3) * 10 - pToMajor->GetRelation(pFromRace->GetRaceID()));
							pToMajor->SetRelation(pFromRace->GetRaceID(), add);
						}						
						// Wenn die Beziehung zu schlecht ist, z.B. nachdem ein Krieg war und ein Nichtangriffspakt nachher
						// angenommen wurde, so die Beziehung auf ein Minimum setzen.
						if (pFromRace->GetRelation(pToMajor->GetRaceID()) < (answer.m_nType + 3) * 10)
						{
							int add = ((answer.m_nType + 3) * 10 - pFromRace->GetRelation(pToMajor->GetRaceID()));
							pFromRace->SetRelation(pToMajor->GetRaceID(), add);
						}
					}
					// Wir haben das Angebot abgelehnt oder nicht darauf reagiert
					else
					{
						// Nachricht über Ablehnung
						if (answer.m_nAnswerStatus == ANSWER_STATUS::DECLINED)
						{
							CString s = CResourceManager::GetString("WE_DECLINE_MAJ_OFFER", FALSE, sAgreement, pFromRace->GetRaceName());
							message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
							pToMajor->GetEmpire()->AddMessage(message);

							CString s2 = CResourceManager::GetString("MAJ_DECLINE_OFFER", TRUE, pToMajor->GetEmpireNameWithArticle(), sAgreement2);
							message.GenerateMessage(s2, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
							((CMajor*)pFromRace)->GetEmpire()->AddMessage(message);

							// Beziehungsverschlechterung, wenn unser Angebot abgelehnt wird, so geht die
							// Beziehung von uns zum Ablehner stärker runter, als die vom Ablehner zu uns
							pToMajor->SetRelation(pFromRace->GetRaceID(), -(USHORT)(rand()%(abs(answer.m_nType))) / 2);
							pFromRace->SetRelation(pToMajor->GetRaceID(), -(USHORT)(rand()%(abs(answer.m_nType))));
						}
						// es wurde nicht auf das Angebot reagiert
						else
						{
							CString s = CResourceManager::GetString("NOT_REACTED", TRUE, pToMajor->GetEmpireNameWithArticle());
							message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
							((CMajor*)pFromRace)->GetEmpire()->AddMessage(message);
						}
						
						// Wenn wir das Angebot abgelehnt haben, dann bekommt die Majorrace, die es mir gemacht hat
						// ihre Ressourcen und ihre Credits wieder zurück, sofern sie es mir als Anreiz mit zum Vertrags-
						// angebot gemacht haben
						((CMajor*)pFromRace)->GetEmpire()->SetCredits(answer.m_nCredits);
						for (int res = TITAN; res <= DERITIUM; res++)
						{
							CPoint pt = answer.m_ptKO;
							if (pt != CPoint(-1,-1))
								if (pDoc->GetSystem(pt).GetOwnerOfSystem() == pFromRace->GetRaceID())
									pDoc->GetSystem(pt).SetResourceStore(res, answer.m_nResources[res]);
						}
					}				
				}
				// Forderungsannahme oder Ablehnung
				else if (answer.m_nType == DIPLOMATIC_AGREEMENT::REQUEST)
				{
					CString s;
					// Wir haben die Forderung angenommen
					if (answer.m_nAnswerStatus == ANSWER_STATUS::ACCEPTED)
					{
						s = CResourceManager::GetString("WE_ACCEPT_REQUEST", TRUE, ((CMajor*)pFromRace)->GetEmpireNameWithAssignedArticle());
						message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
						pToMajor->GetEmpire()->AddMessage(message);

						s = CResourceManager::GetString("OUR_REQUEST_ACCEPT", TRUE, pToMajor->GetEmpireNameWithArticle());
						message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
						((CMajor*)pFromRace)->GetEmpire()->AddMessage(message);

						// Die geforderten Credits und die Ressourcen gutschreiben.
						((CMajor*)pFromRace)->GetEmpire()->SetCredits(answer.m_nCredits);
						for (int res = TITAN; res <= IRIDIUM; res++)
							((CMajor*)pFromRace)->GetEmpire()->GetGlobalStorage()->AddRessource(answer.m_nResources[res], res, CPoint(-1,-1));
						// Deritium kommt nicht ins globale Lager sondern ins Heimatsystem
						if (pInfo->m_nResources[DERITIUM] > 0)
						{
							CPoint p = pDoc->GetRaceKO(pFromRace->GetRaceID());
							// gehört das System auch noch dem Major
							if (p != CPoint(-1,-1) && pDoc->GetSystem(p).GetOwnerOfSystem() == pFromRace->GetRaceID())
								pDoc->GetSystem(p).SetDeritiumStore(pInfo->m_nResources[DERITIUM]);
						}

						// Beziehungsverbesserung bei Annahme auf der fordernden Seite (0 bis 10 Punkte)
						pFromRace->SetRelation(pToMajor->GetRaceID(), rand()%DIPLOMATIC_AGREEMENT::REQUEST);					
					}
					// Wir haben die Forderung abgelehnt
					else if (answer.m_nAnswerStatus == ANSWER_STATUS::DECLINED)
					{
						s = CResourceManager::GetString("WE_DECLINE_REQUEST", TRUE, ((CMajor*)pFromRace)->GetEmpireNameWithAssignedArticle());
						message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
						pToMajor->GetEmpire()->AddMessage(message);

						s = CResourceManager::GetString("OUR_REQUEST_DECLINE", TRUE, pToMajor->GetEmpireNameWithArticle());
						message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
						((CMajor*)pFromRace)->GetEmpire()->AddMessage(message);
						// Beziehungsverschlechterung bei Ablehnung, hier wird ein bissl vom Computer gecheated.
						// Wenn ein Computer von einem anderen Computer die Forderung ablehnt, dann wird die Beziehung
						// nicht ganz so stark verschlechtert als wenn ein Mensch die Forderung des Computers ablehnt
						if (((CMajor*)pFromRace)->IsHumanPlayer() == false)
							((CMajor*)pFromRace)->SetRelation(pToMajor->GetRaceID(), (short)((-rand()%DIPLOMATIC_AGREEMENT::REQUEST)/5));
						else
							((CMajor*)pFromRace)->SetRelation(pToMajor->GetRaceID(), -rand()%DIPLOMATIC_AGREEMENT::REQUEST);
					}
					// Wir haben nicht auf die Forderung reagiert
					else
					{
						s = CResourceManager::GetString("NOT_REACTED_REQUEST", FALSE, pToMajor->GetEmpireNameWithArticle());
						message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
						((CMajor*)pFromRace)->GetEmpire()->AddMessage(message);

						// Beziehungsverschlechterung bei Ablehnung, hier wird ein bissl vom Computer gecheated.
						// Wenn ein Computer von einem anderen Computer die Forderung ablehnt, dann wird die Beziehung
						// nicht ganz so stark verschlechtert als wenn ein Mensch die Forderung des Computers ablehnt
						if (((CMajor*)pFromRace)->IsHumanPlayer() == false)
							((CMajor*)pFromRace)->SetRelation(pToMajor->GetRaceID(), (short)((-rand()%DIPLOMATIC_AGREEMENT::REQUEST)/10));
						else
							((CMajor*)pFromRace)->SetRelation(pToMajor->GetRaceID(), (short)((-rand()%DIPLOMATIC_AGREEMENT::REQUEST)/2));
					}
				}
				// Kriegspaktannahme oder Ablehnung (das ist hier ein bissl kompliziert, weil wir alle mgl. Bündnisse beachten müssen
				else if (answer.m_nType == DIPLOMATIC_AGREEMENT::WARPACT)
				{
					CString s;
					
					// Wir haben den Kriegspakt angenommen
					if (answer.m_nAnswerStatus == ANSWER_STATUS::ACCEPTED)
					{
						MYTRACE(MT::LEVEL_INFO, "Race: %s accepted WARPACT from %s versus %s", pToMajor->GetRaceID(), pFromRace->GetRaceID(), pWarpactEnemy->GetRaceID());
						
						s = CResourceManager::GetString("WE_ACCEPT_WARPACT", FALSE, pWarpactEnemy->GetRaceName(), ((CMajor*)pFromRace)->GetEmpireNameWithAssignedArticle());
						message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
						pToMajor->GetEmpire()->AddMessage(message);
						s = CResourceManager::GetString("OUR_WARPACT_ACCEPT", TRUE, pToMajor->GetEmpireNameWithArticle(), pWarpactEnemy->GetRaceName());
						message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
						((CMajor*)pFromRace)->GetEmpire()->AddMessage(message);
							
						// Beziehungsverbesserung
						pToMajor->SetRelation(pFromRace->GetRaceID(), abs(answer.m_nType));
						pFromRace->SetRelation(pToMajor->GetRaceID(), abs(answer.m_nType));

						// Die möglicherweise dazugegebenen Credits und die Ressourcen gutschreiben.
						pToMajor->GetEmpire()->SetCredits(answer.m_nCredits);
						for (int res = TITAN; res <= IRIDIUM; res++)
							pToMajor->GetEmpire()->GetGlobalStorage()->AddRessource(answer.m_nResources[res], res, CPoint(-1,-1));
						// Deritium kommt nicht ins globale Lager sondern ins Heimatsystem
						if (pInfo->m_nResources[DERITIUM] > 0)
						{
							CPoint p = pDoc->GetRaceKO(pToMajor->GetRaceID());
							// gehört das System auch noch dem Major
							if (p != CPoint(-1,-1) && pDoc->GetSystem(p).GetOwnerOfSystem() == pToMajor->GetRaceID())
								pDoc->GetSystem(p).SetDeritiumStore(pInfo->m_nResources[DERITIUM]);
						}

						// Kriegserklärung erstellen (für den der das Angebot angenommen hat)
						// nun muss in der Antwort die ToRace mit dem Kriegspaktgegner ersetzt werden
						// zusätzlich muss die Antwort sich um einen Krieg handeln, nicht um den Kriegspakt
						CDiplomacyInfo warOffer;
						warOffer.m_sToRace		= pWarpactEnemy->GetRaceID();
						warOffer.m_sFromRace	= answer.m_sFromRace;
						warOffer.m_nFlag		= DIPLOMACY_OFFER;
						warOffer.m_nSendRound	= answer.m_nSendRound;
						warOffer.m_nType		= DIPLOMATIC_AGREEMENT::WAR;
						CGenDiploMessage::GenerateMajorOffer(warOffer);
						
						DeclareWar(pToMajor, pWarpactEnemy, &warOffer, true);
						// aufgrund diplomatischer Beziehungen könnte so weiter Krieg erklärt werden
						std::vector<CString> vEnemies;
						vEnemies = GetEnemiesFromContract(pDoc, pToMajor, pWarpactEnemy);
						// allen weiteren Gegnern den Krieg erklären
						for (UINT i = 0; i < vEnemies.size(); i++)
						{
							if (vEnemies[i] != pFromRace->GetRaceID())
							{
								CRace* pEnemy = pDoc->GetRaceCtrl()->GetRace(vEnemies[i]);
								if (pEnemy)
								{
									CDiplomacyInfo war = warOffer;
									// lediglich die Zielrasse und den Krieggrund anpassen
									war.m_sToRace		= pEnemy->GetRaceID();
									war.m_sWarPartner	= pWarpactEnemy->GetRaceID();
									DeclareWar(pToMajor, pEnemy, &war, false);
								}
							}
						}
						// Kriegserklärung erstellen (für den der das Angebot abgegeben hat)
						// dabei muss der, der das Angebot abgegeben hat umgesetzt werden
						warOffer.m_sFromRace = answer.m_sToRace;
						CGenDiploMessage::GenerateMajorOffer(warOffer);

						DeclareWar(pFromRace, pWarpactEnemy, &warOffer, true);
						// aufgrund diplomatischer Beziehungen könnte so weiter Krieg erklärt werden
						vEnemies.clear();
						vEnemies = GetEnemiesFromContract(pDoc, (CMajor*)pFromRace, pWarpactEnemy);
						// allen weiteren Gegnern den Krieg erklären
						for (UINT i = 0; i < vEnemies.size(); i++)
						{
							if (vEnemies[i] != pToMajor->GetRaceID())
							{
								CRace* pEnemy = pDoc->GetRaceCtrl()->GetRace(vEnemies[i]);
								if (pEnemy)
								{
									CDiplomacyInfo war = warOffer;
									// lediglich die Zielrasse und den Krieggrund anpassen
									war.m_sToRace		= pEnemy->GetRaceID();
									war.m_sWarPartner	= pWarpactEnemy->GetRaceID();
									DeclareWar(pFromRace, pEnemy, &war, false);
								}
							}
						}
					}
					// das Angebot wurde abgelehnt bzw. ignoriert
					else
					{
						// Wir haben den Kriegspakt abgelehnt
						if (answer.m_nAnswerStatus == ANSWER_STATUS::DECLINED)
						{
							s = CResourceManager::GetString("WE_DECLINE_WARPACT", FALSE, ((CMajor*)pFromRace)->GetEmpireNameWithAssignedArticle(), pWarpactEnemy->GetRaceName());
							message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
							pToMajor->GetEmpire()->AddMessage(message);
							s = CResourceManager::GetString("OUR_WARPACT_DECLINE", TRUE, pToMajor->GetEmpireNameWithArticle(), pWarpactEnemy->GetRaceName());
							message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
							((CMajor*)pFromRace)->GetEmpire()->AddMessage(message);
							
							// Beziehungsverschlechterung
							pToMajor->SetRelation(pFromRace->GetRaceID(), -(USHORT)(rand()%(abs(answer.m_nType)))/2);
							pFromRace->SetRelation(pToMajor->GetRaceID(), -(USHORT)(rand()%(abs(answer.m_nType))));
						}
						// auf unseren Kriegspakt wurde nicht reagiert
						else
						{
							s = CResourceManager::GetString("NOT_REACTED_WARPACT", TRUE, pToMajor->GetEmpireNameWithArticle(), pWarpactEnemy->GetRaceName());
							message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
							((CMajor*)pFromRace)->GetEmpire()->AddMessage(message);							
						}

						// Wenn wir das Angebot abgelehnt haben, dann bekommt die Majorrace, die es mir gemacht hat
						// ihre Ressourcen und ihre Credits wieder zurück, sofern sie es mir als Anreiz mit zum Vertrags-
						// angebot gemacht haben
						((CMajor*)pFromRace)->GetEmpire()->SetCredits(answer.m_nCredits);
						for (int res = TITAN; res <= DERITIUM; res++)
						{
							CPoint pt = answer.m_ptKO;
							if (pt != CPoint(-1,-1))
								if (pDoc->GetSystem(pt).GetOwnerOfSystem() == pFromRace->GetRaceID())
									pDoc->GetSystem(pt).SetResourceStore(res, answer.m_nResources[res]);
						}
					}
				}
			}
			// die Antwort geht an einen Minor
			else if (pFromRace->GetType() == MINOR)
			{
				// wir haben das Angebot der Minor angenommen
				if (pInfo->m_nAnswerStatus == ANSWER_STATUS::ACCEPTED)
				{
					CString sEventText = "";					
					if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::MEMBERSHIP)
						sEventText = pToMajor->GetMoralObserver()->AddEvent(10, pToMajor->GetRaceMoralNumber() , pFromRace->GetRaceName());
					
					// wenn es keine Bestechung ist
					if (pInfo->m_nType != DIPLOMATIC_AGREEMENT::CORRUPTION)
					{
						// Prüfen ob der Vertrag aufgrund aktuell bestehender Verträge überhaupt angenommen werden darf
						// Wenn die Minorrace unser Angebot in der gleichen Runde angenommen hat, dann
						// wird der Vertrag hier nicht gesetzt
						CMinor* pMinor = dynamic_cast<CMinor*>(pFromRace);
						ASSERT(pMinor);
						if (!pMinor->CanAcceptOffer(pDoc, pToMajor->GetRaceID(), pInfo->m_nType))
							return;
						
						// nur Text bei Vertragsformen erstellen
						if (!sAgreement.IsEmpty())
						{
							CString	s = CResourceManager::GetString("FEMALE_ARTICLE", TRUE) + " " + pFromRace->GetRaceName() + " " + CResourceManager::GetString("MIN_ACCEPT_OFFER", FALSE, sAgreement, pToMajor->GetEmpireNameWithAssignedArticle());
							message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
							pToMajor->GetEmpire()->AddMessage(message);					
						}

						// Moralevent einplanen
						CString sEventText = "";
						switch (pInfo->m_nType)
						{
						case DIPLOMATIC_AGREEMENT::TRADE: sEventText = pToMajor->GetMoralObserver()->AddEvent(34, pToMajor->GetRaceMoralNumber(), pFromRace->GetRaceName());	break;
						// Sign Friendship/Cooperation Treaty #35
						case DIPLOMATIC_AGREEMENT::FRIENDSHIP: sEventText = pToMajor->GetMoralObserver()->AddEvent(35, pToMajor->GetRaceMoralNumber(), pFromRace->GetRaceName()); break;
						// Sign Friendship/Cooperation Treaty #35
						case DIPLOMATIC_AGREEMENT::COOPERATION: sEventText = pToMajor->GetMoralObserver()->AddEvent(35, pToMajor->GetRaceMoralNumber(), pFromRace->GetRaceName());	break;
						// Sign an Affiliation Treaty #36
						case DIPLOMATIC_AGREEMENT::AFFILIATION: sEventText = pToMajor->GetMoralObserver()->AddEvent(36, pToMajor->GetRaceMoralNumber(), pFromRace->GetRaceName());	break;
						// Sign a Membership #10
						case DIPLOMATIC_AGREEMENT::MEMBERSHIP: sEventText = pToMajor->GetMoralObserver()->AddEvent(10, pToMajor->GetRaceMoralNumber() , pFromRace->GetRaceName());	break;
						}	
						
						if (!sEventText.IsEmpty())
						{
							CMessage message;
							message.GenerateMessage(sEventText, MESSAGE_TYPE::SOMETHING, "", 0, 0);
							pToMajor->GetEmpire()->AddMessage(message);
						}

						// Vertrag setzen
						pFromRace->SetAgreement(pToMajor->GetRaceID(), pInfo->m_nType);
						pToMajor->SetAgreement(pFromRace->GetRaceID(), pInfo->m_nType);
					}
				}
				// wir haben das Angebot der Minor abgelehnt
				else if (pInfo->m_nAnswerStatus == ANSWER_STATUS::DECLINED)
				{
					switch (pInfo->m_nType)
					{
						case DIPLOMATIC_AGREEMENT::TRADE:		sAgreement = CResourceManager::GetString("TRADE_AGREEMENT"); break;
						case DIPLOMATIC_AGREEMENT::FRIENDSHIP:sAgreement = CResourceManager::GetString("FRIENDSHIP"); break;
						case DIPLOMATIC_AGREEMENT::COOPERATION:			sAgreement = CResourceManager::GetString("COOPERATION"); break;
						case DIPLOMATIC_AGREEMENT::AFFILIATION:			sAgreement = CResourceManager::GetString("AFFILIATION"); break;
						case DIPLOMATIC_AGREEMENT::MEMBERSHIP:			sAgreement = CResourceManager::GetString("MEMBERSHIP"); break;
					}
					CString s = CResourceManager::GetString("WE_DECLINE_MIN_OFFER", FALSE, sAgreement, pFromRace->GetRaceName());					
					message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
					pToMajor->GetEmpire()->AddMessage(message);
				}
			}
		}
	}
}

/// Funktion zum Versenden von diplomatischen Angeboten an eine Minorrace.
/// @param pDoc Zeiger auf das Dokument
/// @param pToMinor Zeiger auf die Empfängerrasse
/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
void CDiplomacyController::SendToMinor(CBotf2Doc* pDoc, CMinor* pToMinor, CDiplomacyInfo* pInfo)
{
	// nur Reaktion beim diplomatischen Angebot
	if (pInfo->m_nFlag != DIPLOMACY_OFFER)
		return;

	// Nur wenn es ein Major abgeschickt hat macht dies auch Sinn hier weiter zu machen. Die
	// anderen Rassenarten bekommen keine Imperiumsnachrichten
	CMajor* pFromMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(pInfo->m_sFromRace));
	if (!pFromMajor)
		return;

	// Wenn wir sozusagen eine Runde gewartet haben, dann Nachricht machen erzeugen
	if (pInfo->m_nSendRound == pDoc->GetCurrentRound() - 1)
	{
		// Angebot der Minor in den Nachrichteneingang legen
		if (pInfo->m_nType !=DIPLOMATIC_AGREEMENT:: WAR && pInfo->m_nType != DIPLOMATIC_AGREEMENT::NONE)
			pToMinor->GetIncomingDiplomacyNews()->push_back(*pInfo);
	
		////////////////////////////////////////////////////////////////////////////////////////
		// das Angebot geht an einen Minor
		////////////////////////////////////////////////////////////////////////////////////////
		// Imperiumsnamen inkl. Artikel holen
		CString sEmpireName = pFromMajor->GetEmpireNameWithArticle();
		// groß beginnen
		CString sUpper = (CString)sEmpireName.GetAt(0);
		sEmpireName.SetAt(0, sUpper.MakeUpper().GetAt(0));

		CString s;
		CMessage message;

		// Wenn eine Runde vergangen ist, dann Nachricht erstellen, dass ein Geschenk gemacht wurde.
		if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::PRESENT)
		{	
			// Credits geschenkt
			if (pInfo->m_nCredits > 0)
			{
				CString sCredits;
				sCredits.Format("%d", pInfo->m_nCredits);
				s = sEmpireName + " " + CResourceManager::GetString("CREDITS_PRESENT", FALSE, sCredits, pToMinor->GetRaceName());
				message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, FALSE);
				pFromMajor->GetEmpire()->AddMessage(message);
				s = "";
			}
			// Ressourcen geschenkt (allein oder zusätzlich zu Credits)
			if (pInfo->m_nResources[TITAN] > 0)
			{
				CString number;
				number.Format("%d", pInfo->m_nResources[TITAN]);
				s = sEmpireName + " " + CResourceManager::GetString("TITAN_PRESENT", FALSE, number, pToMinor->GetRaceName());			
			}
			else if (pInfo->m_nResources[DEUTERIUM] > 0)
			{
				CString number;
				number.Format("%d", pInfo->m_nResources[DEUTERIUM]);
				s = sEmpireName + " " + CResourceManager::GetString("DEUTERIUM_PRESENT", FALSE, number, pToMinor->GetRaceName());			
			}
			else if (pInfo->m_nResources[DURANIUM] > 0)
			{
				CString number;
				number.Format("%d", pInfo->m_nResources[DURANIUM]);
				s = sEmpireName + " " + CResourceManager::GetString("DURANIUM_PRESENT", FALSE, number, pToMinor->GetRaceName());
			}
			else if (pInfo->m_nResources[CRYSTAL] > 0)
			{
				CString number;
				number.Format("%d", pInfo->m_nResources[CRYSTAL]);
				s = sEmpireName + " " + CResourceManager::GetString("CRYSTAL_PRESENT", FALSE, number, pToMinor->GetRaceName());			
			}
			else if (pInfo->m_nResources[IRIDIUM] > 0)
			{
				CString number;
				number.Format("%d", pInfo->m_nResources[IRIDIUM]);
				s = sEmpireName + " " + CResourceManager::GetString("IRIDIUM_PRESENT", FALSE, number, pToMinor->GetRaceName());			
			}
			else if (pInfo->m_nResources[DERITIUM] > 0)
			{
				CString number;
				number.Format("%d", pInfo->m_nResources[DERITIUM]);
				s = sEmpireName + " " + CResourceManager::GetString("DERITIUM_PRESENT", FALSE, number, pToMinor->GetRaceName());			
			}		
		}
		// Nachricht machen, das wir eine Bestechung versuchen
		else if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::CORRUPTION)
		{
			s = sEmpireName + " " + CResourceManager::GetString("TRY_CORRUPTION", FALSE, pToMinor->GetRaceName());		
		}
		// Nachricht machen, das wir Vertrag aufgehoben haben
		else if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::NONE)
		{
			s = sEmpireName + " " + CResourceManager::GetString("CANCEL_CONTRACT", FALSE, pToMinor->GetRaceName());
			pFromMajor->SetAgreement(pToMinor->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);
			pToMinor->SetAgreement(pFromMajor->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);
		}
		// Nachricht erstellen, dass Krieg erklärt wurde
		else if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::WAR)
		{
			// Krieg erklären
			DeclareWar(pFromMajor, pToMinor, pInfo, true);
			
			// aufgrund diplomatischer Beziehungen könnte so weiter Krieg erklärt werden
			std::vector<CString> vEnemies;
			vEnemies = GetEnemiesFromContract(pDoc, pFromMajor, pToMinor);
			// allen weiteren Gegnern den Krieg erklären
			for (UINT i = 0; i < vEnemies.size(); i++)
			{
				CRace* pEnemy = pDoc->GetRaceCtrl()->GetRace(vEnemies[i]);
				if (pEnemy)
				{
					CDiplomacyInfo war = *pInfo;
					// lediglich die Zielrasse und den Krieggrund anpassen
					war.m_sToRace		= pEnemy->GetRaceID();
					war.m_sWarPartner	= pToMinor->GetRaceID();
					DeclareWar(pFromMajor, pEnemy, &war, false);
				}
			}
		}
		// Nachricht machen, das wir einen Vertrag angeboten haben
		else
		{
			CString sAgreement;
			switch(pInfo->m_nType)
			{
			case DIPLOMATIC_AGREEMENT::TRADE:		{sAgreement = CResourceManager::GetString("TRADE_AGREEMENT_WITH_ARTICLE");	break;}
			case DIPLOMATIC_AGREEMENT::FRIENDSHIP:{sAgreement = CResourceManager::GetString("FRIENDSHIP_WITH_ARTICLE");		break;}
			case DIPLOMATIC_AGREEMENT::COOPERATION:			{sAgreement = CResourceManager::GetString("COOPERATION_WITH_ARTICLE");		break;}
			case DIPLOMATIC_AGREEMENT::AFFILIATION:			{sAgreement = CResourceManager::GetString("AFFILIATION_WITH_ARTICLE");		break;}
			case DIPLOMATIC_AGREEMENT::MEMBERSHIP:			{sAgreement = CResourceManager::GetString("MEMBERSHIP_WITH_ARTICLE");		break;}
			}
			s = sEmpireName + " " + CResourceManager::GetString("OUR_MIN_OFFER", FALSE, pToMinor->GetRaceName(), sAgreement);		
		}
		
		if (!s.IsEmpty())
		{
			message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
			pFromMajor->GetEmpire()->AddMessage(message);
		}
	}
}

/// Funktion zum Empfangen und Bearbeiten eines diplomatischen Angebots für eine Minorrace.
/// @param pDoc Zeiger auf das Dokument
/// @param pToMajor Zeiger auf die Empfängerrasse
/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
void CDiplomacyController::ReceiveToMinor(CBotf2Doc* pDoc, CMinor* pToMinor, CDiplomacyInfo* pInfo)
{
	// nur Reaktion beim diplomatischen Angebot
	if (pInfo->m_nFlag != DIPLOMACY_OFFER)
		return;

	// Die Rasse welche die Nachricht abgeschickt hat
	CMajor* pFromMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(pInfo->m_sFromRace));
	if (!pFromMajor)
		return;

	////////////////////////////////////////////////////////////////////////////////////////
	// das Angebot geht an einen Minor
	////////////////////////////////////////////////////////////////////////////////////////
	if (pInfo->m_nSendRound == pDoc->GetCurrentRound() - 2)
	{
		pToMinor->ReactOnOfferAI(pInfo);
		
		if (pInfo->m_nType != DIPLOMATIC_AGREEMENT::PRESENT && pInfo->m_nType != DIPLOMATIC_AGREEMENT::WAR)
		{
			if (pInfo->m_nType != DIPLOMATIC_AGREEMENT::NONE)
			{
				CDiplomacyInfo answer = *pInfo;
				answer.m_nSendRound = pDoc->GetCurrentRound();
				CGenDiploMessage::GenerateMinorAnswer(answer);
				pFromMajor->GetIncomingDiplomacyNews()->push_back(answer);
			}

			// Minor hat angenommen
			if (pInfo->m_nAnswerStatus == ANSWER_STATUS::ACCEPTED)
			{
				CString sEventText = "";

				// wenn es keine Bestechung ist
				if (pInfo->m_nType != DIPLOMATIC_AGREEMENT::CORRUPTION)
				{
					CString sAgreement;
					switch (pInfo->m_nType)
					{
					case DIPLOMATIC_AGREEMENT::TRADE:		{sAgreement = CResourceManager::GetString("TRADE_AGREEMENT_WITH_ARTICLE");	break;}
					case DIPLOMATIC_AGREEMENT::FRIENDSHIP:{sAgreement = CResourceManager::GetString("FRIENDSHIP_WITH_ARTICLE");		break;}
					case DIPLOMATIC_AGREEMENT::COOPERATION:			{sAgreement = CResourceManager::GetString("COOPERATION_WITH_ARTICLE");		break;}
					case DIPLOMATIC_AGREEMENT::AFFILIATION:			{sAgreement = CResourceManager::GetString("AFFILIATION_WITH_ARTICLE");		break;}
					case DIPLOMATIC_AGREEMENT::MEMBERSHIP:			{sAgreement = CResourceManager::GetString("MEMBERSHIP_WITH_ARTICLE");		break;}					
					}
					// nur Text bei Vertragsformen erstellen
					if (!sAgreement.IsEmpty())
					{
						CString	s = CResourceManager::GetString("FEMALE_ARTICLE", TRUE) + " " + pToMinor->GetRaceName() + " " + CResourceManager::GetString("MIN_ACCEPT_OFFER", FALSE, sAgreement, pFromMajor->GetEmpireNameWithAssignedArticle());
						CMessage message;
						message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
						pFromMajor->GetEmpire()->AddMessage(message);						
					}
					pToMinor->SetAgreement(pFromMajor->GetRaceID(), pInfo->m_nType);
					pFromMajor->SetAgreement(pToMinor->GetRaceID(), pInfo->m_nType);

					// Moralevent einplanen
					switch (pInfo->m_nType)
					{
					case DIPLOMATIC_AGREEMENT::TRADE: sEventText = pFromMajor->GetMoralObserver()->AddEvent(34, pFromMajor->GetRaceMoralNumber(), pToMinor->GetRaceName());	break;
					// Sign Friendship/Cooperation Treaty #35
					case DIPLOMATIC_AGREEMENT::FRIENDSHIP: sEventText = pFromMajor->GetMoralObserver()->AddEvent(35, pFromMajor->GetRaceMoralNumber(), pToMinor->GetRaceName()); break;
					// Sign Friendship/Cooperation Treaty #35
					case DIPLOMATIC_AGREEMENT::COOPERATION: sEventText = pFromMajor->GetMoralObserver()->AddEvent(35, pFromMajor->GetRaceMoralNumber(), pToMinor->GetRaceName());	break;
					// Sign an Affiliation Treaty #36
					case DIPLOMATIC_AGREEMENT::AFFILIATION: sEventText = pFromMajor->GetMoralObserver()->AddEvent(36, pFromMajor->GetRaceMoralNumber(), pToMinor->GetRaceName());	break;
					// Sign a Membership #10
					case DIPLOMATIC_AGREEMENT::MEMBERSHIP: sEventText = pFromMajor->GetMoralObserver()->AddEvent(10, pFromMajor->GetRaceMoralNumber() , pToMinor->GetRaceName());	break;
					}	
					
					if (!sEventText.IsEmpty())
					{
						CMessage message;
						message.GenerateMessage(sEventText, MESSAGE_TYPE::SOMETHING, "", 0, 0);
						pFromMajor->GetEmpire()->AddMessage(message);
					}
				}

				// übergebene Ressourcen ins System der Minor einlagern
				CPoint pt = pToMinor->GetRaceKO();
				if (pt != CPoint(-1,-1))
				{
					for (int res = TITAN; res <= DERITIUM; res++)
						pDoc->GetSystem(pt).SetResourceStore(res, pInfo->m_nResources[res]);
				}
			}			
			else if (pInfo->m_nAnswerStatus == ANSWER_STATUS::DECLINED)
			{
				if (pInfo->m_nType != DIPLOMATIC_AGREEMENT::CORRUPTION)
				{
					CString s = "";
					CString sAgreement = "";
					switch (pInfo->m_nType)
					{
						case DIPLOMATIC_AGREEMENT::TRADE:		sAgreement = CResourceManager::GetString("TRADE_AGREEMENT"); break;
						case DIPLOMATIC_AGREEMENT::FRIENDSHIP:sAgreement = CResourceManager::GetString("FRIENDSHIP"); break;
						case DIPLOMATIC_AGREEMENT::COOPERATION:			sAgreement = CResourceManager::GetString("COOPERATION"); break;
						case DIPLOMATIC_AGREEMENT::AFFILIATION:			sAgreement = CResourceManager::GetString("AFFILIATION"); break;
						case DIPLOMATIC_AGREEMENT::MEMBERSHIP:			sAgreement = CResourceManager::GetString("MEMBERSHIP"); break;
					}
					
					s = CResourceManager::GetString("MIN_DECLINE_OFFER", FALSE, pToMinor->GetRaceName(), sAgreement);
					CMessage message;
					message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
					pFromMajor->GetEmpire()->AddMessage(message);
				
					// Wenn das Angebot abgelehnt wurde, dann bekommt die Majorrace, die es gemacht hat
					// ihre Ressourcen und ihre Credits wieder zurück, sofern sie es als Anreiz mit zum Vertrags-
					// angebot gemacht haben
					pFromMajor->GetEmpire()->SetCredits(pInfo->m_nCredits);
					for (int res = TITAN; res <= DERITIUM; res++)
					{
						CPoint pt = pInfo->m_ptKO;
						if (pt != CPoint(-1,-1))
							if (pDoc->GetSystem(pt).GetOwnerOfSystem() == pFromMajor->GetRaceID())
								pDoc->GetSystem(pt).SetResourceStore(res, pInfo->m_nResources[res]);
					}
				}
			}
		}
	}
}

/// Funktion zum Berechnen aller betroffenen Rassen, welchen zusätzlich der Krieg erklärt wird.
/// Dies geschieht dadurch, wenn der Kriegsgegner Bündnisse oder Verteidigungspakt mit anderen Rassen besitzt.
/// @param pDoc Zeiger auf das Dokument
/// @param pFromMajor Zeiger auf die kriegserklärende Majorrace
/// @param pToRace Zeiger auf die Rasse, welcher der Krieg erklärt wurde
/// @return Feld mit allen betroffenen Rassen, denen auch der Krieg erklärt werden muss
std::vector<CString> CDiplomacyController::GetEnemiesFromContract(CBotf2Doc* pDoc, CMajor* pFromMajor, CRace* pToRace)
{
	std::vector<CString> vEnemies;
	// Wenn wir jemanden den Krieg erklären und dieser Jemand ein Bündnis oder einen
	// Verteidigungspakt mit einer anderen Majorrace hat, so erklären wir der anderen Majorrace
	// auch den Krieg. Außerdem erklären wir auch jeder Minorrace den Krieg, mit der der Jemand 
	// mindst. ein Bündnis hatte.	
	map<CString, CRace*>* mRaces = pDoc->GetRaceCtrl()->GetRaces();
	for (map<CString, CRace*>::const_iterator it = mRaces->begin(); it != mRaces->end(); ++it)
	{
		// nicht wir selbst
		if (it->first != pToRace->GetRaceID())
		{
			// hat die Rasse mit der anderen Rasse ein Bündnis oder einen Verteidigungspakt
			if (pToRace->GetAgreement(it->first) >= DIPLOMATIC_AGREEMENT::AFFILIATION || (pToRace->GetType() == MAJOR && ((CMajor*)pToRace)->GetDefencePact(it->first)))
			{
				// haben wir nicht schon Krieg mit dem anderen Major
				if (pFromMajor->GetAgreement(it->first) != DIPLOMATIC_AGREEMENT::WAR)
				{
					vEnemies.push_back(it->first);					
					MYTRACE(MT::LEVEL_INFO, "Race: %s declares Race: %s WAR because affiliation contract with Race %s", pFromMajor->GetRaceID(), it->first, pToRace->GetRaceID());
				}
			}
		}
	}
	
	return vEnemies;
}

/// Funktion erklärt den Krieg zwischen zwei Rassen und nimmt dabei alle notwendigen Arbeiten vor.
/// @param pFromRace Zeiger auf die kriegserklärende Rasse
/// @param pEnemy Zeiger auf die Rasse, welcher Krieg erklärt wird
/// @param pInfo Diplomatieobjekt
/// @param bWithMoralEvent <code>true</code> wenn Moralevent mit eingeplant werden soll
void CDiplomacyController::DeclareWar(CRace* pFromRace, CRace* pEnemy, CDiplomacyInfo* pInfo, bool bWithMoralEvent)
{
	// haben wir schon Krieg, so kann keiner nochmal erklärt werden
	if (pFromRace->GetAgreement(pEnemy->GetRaceID()) == DIPLOMATIC_AGREEMENT::WAR)
		return;

	CString s;
	CMessage message;

	if (pFromRace->GetType() == MAJOR)
	{
		if (pEnemy->GetType() == MAJOR)
			s = CResourceManager::GetString("WE_DECLARE_WAR", FALSE, ((CMajor*)pEnemy)->GetEmpireNameWithAssignedArticle());
		else if (pEnemy->GetType() == MINOR)
			s = CResourceManager::GetString("WE_DECLARE_WAR_TO_MIN", FALSE, pEnemy->GetRaceName());
		
		message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0);
		((CMajor*)pFromRace)->GetEmpire()->AddMessage(message);

		if (bWithMoralEvent)
		{
			// zusätzliche Eventnachricht wegen der Moral an das Imperium
			CString sEventText = "";
			short nAgreement = pFromRace->GetAgreement(pEnemy->GetRaceID());
			CString sParam = CResourceManager::GetString("FEMALE_ARTICLE") + " " + pEnemy->GetRaceName();
			if (pEnemy->GetType() == MAJOR)
				sParam = ((CMajor*)pEnemy)->GetEmpireNameWithArticle();
						
			// Declare War on an Empire with Defense Pact #28 (nur, wenn wir einen Vertrag kleiner als den der
			// Kooperation haben und dazu auch noch einen Verteidigungspakt)
			if (pEnemy->GetType() == MAJOR && nAgreement < DIPLOMATIC_AGREEMENT::COOPERATION && ((CMajor*)pFromRace)->GetDefencePact(pEnemy->GetRaceID()) == true)
				sEventText = ((CMajor*)pFromRace)->GetMoralObserver()->AddEvent(28, ((CMajor*)pFromRace)->GetRaceMoralNumber(), sParam);
			// Declare War on an Empire when Neutral #24
			else if (nAgreement == DIPLOMATIC_AGREEMENT::NONE)
				sEventText = ((CMajor*)pFromRace)->GetMoralObserver()->AddEvent(24, ((CMajor*)pFromRace)->GetRaceMoralNumber(), sParam);
			// Declare War on an Empire when Non-Aggression #25
			else if (nAgreement == DIPLOMATIC_AGREEMENT::NAP)
				sEventText = ((CMajor*)pFromRace)->GetMoralObserver()->AddEvent(25, ((CMajor*)pFromRace)->GetRaceMoralNumber(), sParam);
			// Declare War on an Empire with Trade Treaty #26
			else if (nAgreement == DIPLOMATIC_AGREEMENT::TRADE)
				sEventText = ((CMajor*)pFromRace)->GetMoralObserver()->AddEvent(26, ((CMajor*)pFromRace)->GetRaceMoralNumber(), sParam);
			// Declare War on an Empire with Friendship Treaty #27
			else if (nAgreement == DIPLOMATIC_AGREEMENT::FRIENDSHIP)
				sEventText = ((CMajor*)pFromRace)->GetMoralObserver()->AddEvent(27, ((CMajor*)pFromRace)->GetRaceMoralNumber(), sParam);
			// Declare War on an Empire with CooperationTreaty #29
			else if (nAgreement == DIPLOMATIC_AGREEMENT::COOPERATION)
				sEventText = ((CMajor*)pFromRace)->GetMoralObserver()->AddEvent(29, ((CMajor*)pFromRace)->GetRaceMoralNumber(), sParam);
			// Declare War on an Empire with Affiliation #30
			else if (nAgreement == DIPLOMATIC_AGREEMENT::AFFILIATION)
				sEventText = ((CMajor*)pFromRace)->GetMoralObserver()->AddEvent(30, ((CMajor*)pFromRace)->GetRaceMoralNumber(), sParam);
			
			if (!sEventText.IsEmpty())
			{
				message.GenerateMessage(sEventText, MESSAGE_TYPE::SOMETHING, "", 0, 0);
				((CMajor*)pFromRace)->GetEmpire()->AddMessage(message);				
			}
		}

		if (pEnemy->GetType() == MAJOR)
		{
			// Antwort auf Kriegserklärung erstellen
			CDiplomacyInfo answer = *pInfo;			
			CGenDiploMessage::GenerateMajorAnswer(answer);
			((CMajor*)pFromRace)->GetIncomingDiplomacyNews()->push_back(answer);

			// Imperiumsnamen inkl. Artikel holen
			CString sEmpireArticleName = ((CMajor*)pFromRace)->GetEmpireNameWithArticle();
			// groß beginnen
			CString sUpper = (CString)sEmpireArticleName.GetAt(0);
			sEmpireArticleName.SetAt(0, sUpper.MakeUpper().GetAt(0));

			// ging die Kriegserklärung direkt an uns (also nicht indirekt durch diplomatische Pakte)
			if (pInfo->m_sWarPartner.IsEmpty())
			{
				s = CResourceManager::GetString("WE_GET_WAR", FALSE, sEmpireArticleName);			
				message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
				((CMajor*)pEnemy)->GetEmpire()->AddMessage(message);						
			}
			// wurde uns der Krieg aufgrund einer diplomatischen Beziehung indirekt erklärt
			else
			{
				// Nachricht und Kriegserklärung wegen unserem Bündnispartner
				CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
				ASSERT(pDoc);
				CRace* pPartner = pDoc->GetRaceCtrl()->GetRace(pInfo->m_sWarPartner);
				if (!pPartner)
					return;

				s = sEmpireArticleName + " " + CResourceManager::GetString("WAR_TO_PARTNER", FALSE, pPartner->GetRaceName());
				message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
				((CMajor*)pEnemy)->GetEmpire()->AddMessage(message);
				
				s = sEmpireArticleName + " " + CResourceManager::GetString("WAR_TO_US_AS_PARTNER");
				message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
				((CMajor*)pEnemy)->GetEmpire()->AddMessage(message);						
			}
		}		
		else if (pEnemy->GetType() == MINOR)
		{
			// Antwort auf Kriegserklärung erstellen
			CDiplomacyInfo answer = *pInfo;
			CGenDiploMessage::GenerateMinorAnswer(answer);
			((CMajor*)pFromRace)->GetIncomingDiplomacyNews()->push_back(answer);
		}
	}
	else if (pFromRace->GetType() == MINOR)
	{
		s = CResourceManager::GetString("MIN_OFFER_WAR", FALSE, pFromRace->GetRaceName());
		message.GenerateMessage(s, MESSAGE_TYPE::DIPLOMACY, "", 0, 0, 2);
		// Kriegserklärung von Minor kann nur an Majors gehen
		((CMajor*)pEnemy)->GetEmpire()->AddMessage(message);
	}

	// Moral für den, dem Krieg erklärt wurde (braucht nur bei Majors gemacht werden)
	if (pEnemy->GetType() == MAJOR)
	{
		// zusätzliche Eventnachricht wegen der Moral an das Imperium
		CString sParam = CResourceManager::GetString("FEMALE_ARTICLE") + " " + pFromRace->GetRaceName();
		if (pFromRace->GetType() == MAJOR)
			sParam = ((CMajor*)pFromRace)->GetEmpireNameWithArticle();
		CString sEventText = "";
		short nAgreement = pFromRace->GetAgreement(pEnemy->GetRaceID());
		// Other Empire Declares War when Neutral #31
		if (nAgreement == DIPLOMATIC_AGREEMENT::NONE)
			sEventText = ((CMajor*)pEnemy)->GetMoralObserver()->AddEvent(31, ((CMajor*)pEnemy)->GetRaceMoralNumber(), sParam);
		// Other Empire Declares War with an Affiliation (or Membership) #33
		else if (nAgreement >= DIPLOMATIC_AGREEMENT::AFFILIATION)
			sEventText = ((CMajor*)pEnemy)->GetMoralObserver()->AddEvent(33, ((CMajor*)pEnemy)->GetRaceMoralNumber(), sParam);
		// Other Empire Declares War with Treaty #32
		else
			sEventText = ((CMajor*)pEnemy)->GetMoralObserver()->AddEvent(32, ((CMajor*)pEnemy)->GetRaceMoralNumber(), sParam);
		
		if (!sEventText.IsEmpty())
		{
			message.GenerateMessage(sEventText, MESSAGE_TYPE::SOMETHING, "", 0, 0);
			((CMajor*)pEnemy)->GetEmpire()->AddMessage(message);
		}

		// Angebot in den Nachrichteneingang legen
		((CMajor*)pEnemy)->GetIncomingDiplomacyNews()->push_back(*pInfo);
	}

	// im Kriegsfall sofort Krieg erklären und Beziehung verschlechtern
	// Beziehungen runtersetzen
	pFromRace->SetRelation(pEnemy->GetRaceID(), -100);
	pEnemy->SetRelation(pFromRace->GetRaceID(), -100);
	// Vertragsform auf Krieg setzen
	pFromRace->SetAgreement(pEnemy->GetRaceID(), DIPLOMATIC_AGREEMENT::WAR);
	pEnemy->SetAgreement(pFromRace->GetRaceID(), DIPLOMATIC_AGREEMENT::WAR);
}