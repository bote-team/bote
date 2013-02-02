#include "stdafx.h"
#include "DiplomacyIntelObj.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"
#include "IOData.h"
#include "General/Loc.h"

IMPLEMENT_SERIAL (CDiplomacyIntelObj, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CDiplomacyIntelObj::CDiplomacyIntelObj(void) :
	m_nDuration(0),
	m_nRelationship(0)
{
	m_byType = 3;
	m_MinorRaceKO = CPoint(-1,-1);
}

CDiplomacyIntelObj::CDiplomacyIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &minorRaceKO, DIPLOMATIC_AGREEMENT::Typ agreement, short relationship)
	: CIntelObject(sOwnerID, sEnemyID, round, isSpy, 3), m_MinorRaceKO(minorRaceKO), m_nAgreement(agreement), m_nRelationship(relationship)
{
	m_sMajor = "";
	m_nDuration = NULL;
}

CDiplomacyIntelObj::CDiplomacyIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CString& sMajorRaceID, DIPLOMATIC_AGREEMENT::Typ agreement, short duration, short relationship)
	: CIntelObject(sOwnerID, sEnemyID, round, isSpy, 3), m_sMajor(sMajorRaceID), m_nAgreement(agreement), m_nDuration(duration), m_nRelationship(relationship)
{
	m_MinorRaceKO = CPoint(-1,-1);
}

CDiplomacyIntelObj::CDiplomacyIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &minorRaceKO)
	: CIntelObject(sOwnerID, sEnemyID, round, isSpy, 3), m_MinorRaceKO(minorRaceKO)
{
	m_sMajor = "";
	m_nAgreement = DIPLOMATIC_AGREEMENT::NONE;
	m_nDuration = NULL;
	m_nRelationship = NULL;
}

CDiplomacyIntelObj::~CDiplomacyIntelObj(void)
{
}

/// Kopierkonstruktor
CDiplomacyIntelObj::CDiplomacyIntelObj(const CDiplomacyIntelObj & rhs)
{
	m_sOwner = rhs.m_sOwner;
	m_sEnemy = rhs.m_sEnemy;
	m_nRound = rhs.m_nRound;
	m_bIsSpy = rhs.m_bIsSpy;
	m_byType = rhs.m_byType;
	m_strEnemyDesc = rhs.m_strEnemyDesc;
	m_strOwnerDesc = rhs.m_strOwnerDesc;

	m_sMajor = rhs.m_sMajor;
	m_MinorRaceKO = rhs.m_MinorRaceKO;
	m_nAgreement = rhs.m_nAgreement;
	m_nDuration = rhs.m_nDuration;
	m_nRelationship = rhs.m_nRelationship;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CDiplomacyIntelObj::Serialize(CArchive &ar)
{
	__super::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_MinorRaceKO;
		ar << m_sMajor;
		ar << m_nAgreement;
		ar << m_nDuration;
		ar << m_nRelationship;
	}
	else if (ar.IsLoading())
	{
		ar >> m_MinorRaceKO;
		ar >> m_sMajor;
		int nAgreement;
		ar >> nAgreement;
		m_nAgreement = (DIPLOMATIC_AGREEMENT::Typ)nAgreement;
		ar >> m_nDuration;
		ar >> m_nRelationship;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion generiert einen Text, welcher eine Geheimdiestaktion beschreibt, für den Auslöser bzw. das Opfer
/// dieser Aktion.
void CDiplomacyIntelObj::CreateText(CBotEDoc* pDoc, BYTE n, const CString& param)
{
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName;
	if (this->GetIsSpy())
		fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MajorIntelSpyOff.data";			// Name des zu Öffnenden Files
	else
		fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MajorIntelSabOff.data";		// Name des zu Öffnenden Files
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))			// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			int pos = 0;
			CString s = csInput.Tokenize(":", pos);
			// Rasse bestimmen
			if (s == m_sOwner)
			{
				s = csInput.Tokenize(":", pos);
				// Typ (Wirtschaft, Militär...) bestimmen
				if (atoi(s) == m_byType)
				{
					s = csInput.Tokenize(":", pos);
					// Nummer der Nachricht bestimmen
					if (atoi(s) == n)
					{
						csInput.Delete(0, pos);
						// bei allen Texten außer Typ 3, den Beziehungstexten
						if (n != 3)
						{
							// in csInput steht nun die Beschreibung für den Aggressor
							// Jetzt müssen noch die Variablen mit dem richtigen Text gefüllt werden
							CMajor* pEnemey = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sEnemy));
							if (pEnemey)
							{
								if (this->GetIsSpy())
									s = pEnemey->GetEmpireNameWithArticle();
								else
									s = pEnemey->GetEmpireNameWithAssignedArticle();
								csInput.Replace("$race$", s);
							}

							if (m_MinorRaceKO != CPoint(-1,-1))
							{
								s = pDoc->GetRaceCtrl()->GetMinorRace(pDoc->GetSector(m_MinorRaceKO.x, m_MinorRaceKO.y).GetName())->GetRaceName();
								csInput.Replace("$minor$", s);
							}
							switch (m_nAgreement)
							{
							case DIPLOMATIC_AGREEMENT::DEFENCEPACT:		{s = CLoc::GetString("DEFENCE_PACT_WITH_ARTICLE"); break;}
							case DIPLOMATIC_AGREEMENT::WAR:				{s = CLoc::GetString("WAR_WITH_ARTICLE"); break;}
							case DIPLOMATIC_AGREEMENT::NONE:			{s = CLoc::GetString("NO_AGREEMENT"); break;}
							case DIPLOMATIC_AGREEMENT::NAP:				{s = CLoc::GetString("NON_AGGRESSION_WITH_ARTICLE"); break;}
							case DIPLOMATIC_AGREEMENT::TRADE:			{s = CLoc::GetString("TRADE_AGREEMENT_WITH_ARTICLE"); break;}
							case DIPLOMATIC_AGREEMENT::FRIENDSHIP:		{s = CLoc::GetString("FRIENDSHIP_WITH_ARTICLE"); break;}
							case DIPLOMATIC_AGREEMENT::COOPERATION:		{s = CLoc::GetString("COOPERATION_WITH_ARTICLE"); break;}
							case DIPLOMATIC_AGREEMENT::AFFILIATION:		{s = CLoc::GetString("AFFILIATION_WITH_ARTICLE"); break;}
							case DIPLOMATIC_AGREEMENT::MEMBERSHIP:		{s = CLoc::GetString("MEMBERSHIP_WITH_ARTICLE"); break;}
							default: s = "";
							}
							csInput.Replace("$agreement$", s);

							if (m_sMajor != "")
							{
								CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sMajor));
								if (pMajor)
								{
									s = pMajor->GetEmpireNameWithAssignedArticle();
									csInput.Replace("$major$", s);
								}

								if (m_nAgreement == DIPLOMATIC_AGREEMENT::NONE || m_nAgreement == DIPLOMATIC_AGREEMENT::WAR)
									csInput.Replace("($duration$) ", "");
								else
								{
									if (m_nDuration == NULL)
										csInput.Replace("$duration$", CLoc::GetString("UNLIMITED"));
									else
									{
										s.Format("%d %s", m_nDuration, CLoc::GetString("ROUNDS"));
										csInput.Replace("$duration$", s);
									}
								}
							}
						}
						// handelt es sich um die Beziehungstexte
						else
						{
							CMajor* pEnemey = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sEnemy));
							if (pEnemey)
							{
								s = pEnemey->GetEmpireNameWithAssignedArticle();
								csInput.Replace("$race$", s);
							}

							if (m_sMajor != "")
							{
								CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sMajor));
								if (pMajor)
									s = pMajor->GetEmpireNameWithArticle();
							}
							else if (m_MinorRaceKO != CPoint(-1,-1))
								s = CLoc::GetString("FEMALE_ARTICLE")+" " + pDoc->GetRaceCtrl()->GetMinorRace(pDoc->GetSector(m_MinorRaceKO.x, m_MinorRaceKO.y).GetName())->GetRaceName();
							csInput.Replace("$major$", s);

							if (m_nRelationship < 5) s = CLoc::GetString("HATEFUL");
							else if (m_nRelationship < 15) s = CLoc::GetString("FURIOUS");
							else if (m_nRelationship < 25) s = CLoc::GetString("HOSTILE");
							else if (m_nRelationship < 35) s = CLoc::GetString("ANGRY");
							else if (m_nRelationship < 45) s = CLoc::GetString("NOT_COOPERATIVE");
							else if (m_nRelationship < 55) s = CLoc::GetString("NEUTRAL");
							else if (m_nRelationship < 65) s = CLoc::GetString("COOPERATIVE");
							else if (m_nRelationship < 75) s = CLoc::GetString("FRIENDLY");
							else if (m_nRelationship < 85) s = CLoc::GetString("OPTIMISTIC");
							else if (m_nRelationship < 95) s = CLoc::GetString("ENTHUSED");
							else s = CLoc::GetString("DEVOTED");
							csInput.Replace("$relation$", s);

							if (m_MinorRaceKO != CPoint(-1,-1))
							{
								s = pDoc->GetRaceCtrl()->GetMinorRace(pDoc->GetSector(m_MinorRaceKO.x, m_MinorRaceKO.y).GetName())->GetRaceName();
								csInput.Replace("$minor$", s);
							}
						}

						m_strOwnerDesc = csInput;
						break;
					}
				}
			}
		}
	}
	else
	{
		if (this->GetIsSpy())
			AfxMessageBox("Error! Could not open file \"MajorIntelSpyOff.data\"...");
		else
			AfxMessageBox("Error! Could not open file \"MajorIntelSabOff.data\"...");
		exit(1);
	}
	file.Close();

	// Nachricht für das Opfer erstellen
	if (this->GetIsSabotage())
	{
		fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MajorIntelSabDef.data";			// Name des zu Öffnenden Files
		if (file.Open(fileName, CFile::modeRead | CFile::typeText))			// Datei wird geöffnet
		{
			while (file.ReadString(csInput))
			{
				int pos = 0;
				CString s = csInput.Tokenize(":", pos);
				// Rasse bestimmen
				if (s == m_sEnemy)
				{
					s = csInput.Tokenize(":", pos);
					// Typ (Wirtschaft, Militär...) bestimmen
					if (atoi(s) == m_byType)
					{
						s = csInput.Tokenize(":", pos);
						// Nummer der Nachricht bestimmen
						if (atoi(s) == n)
						{
							csInput.Delete(0, pos);
							// in csInput steht nun die Beschreibung für das Opfer
							// Jetzt müssen noch die Variablen mit dem richtigen Text gefüllt werden
							CMajor* pOwner = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sOwner));
							if (pOwner)
							{
								s = pOwner->GetEmpireNameWithAssignedArticle();
								csInput.Replace("$race$", s);
							}
							if (m_MinorRaceKO != CPoint(-1,-1))
							{
								s = pDoc->GetRaceCtrl()->GetMinorRace(pDoc->GetSector(m_MinorRaceKO.x, m_MinorRaceKO.y).GetName())->GetRaceName();
								csInput.Replace("$minor$", s);
							}
							if (m_sMajor != "")
							{
								CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sMajor));
								if (pMajor)
								{
									s = pMajor->GetEmpireNameWithArticle();
									csInput.Replace("$major$", s);
								}
							}
							m_strEnemyDesc = csInput;
							if (param != "")
							{
								CMajor* pParam = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(param));
								if (pParam)
								{
									s = pParam->GetEmpireNameWithArticle();
									csInput = CLoc::GetString("KNOW_RESPONSIBLE_SABOTAGERACE", FALSE, s);
								}
							}
							else
								csInput = CLoc::GetString("DO_NOT_KNOW_RESPONSIBLE_RACE");
							m_strEnemyDesc += " "+csInput;
							break;
						}
					}
				}
			}
		}
		else
		{
			AfxMessageBox("Error! Could not open file \"MajorIntelSabDef.data\"...");
			exit(1);
		}
		file.Close();
	}
}
