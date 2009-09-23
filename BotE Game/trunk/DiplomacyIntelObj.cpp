#include "stdafx.h"
#include "DiplomacyIntelObj.h"
#include "Botf2Doc.h"
#include "RaceController.h"

IMPLEMENT_SERIAL (CDiplomacyIntelObj, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CDiplomacyIntelObj::CDiplomacyIntelObj(void)
{
	m_byType = 3;
	m_MinorRaceKO = CPoint(-1,-1);
}

CDiplomacyIntelObj::CDiplomacyIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &minorRaceKO, short agreement, short relationship)
	: CIntelObject(sOwnerID, sEnemyID, round, isSpy, 3), m_MinorRaceKO(minorRaceKO), m_nAgreement(agreement), m_nRelationship(relationship)
{
	m_sMajor = "";
	m_nDuration = NULL;
}

CDiplomacyIntelObj::CDiplomacyIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CString& sMajorRaceID, short agreement, short duration, short relationship)
	: CIntelObject(sOwnerID, sEnemyID, round, isSpy, 3), m_sMajor(sMajorRaceID), m_nAgreement(agreement), m_nDuration(duration), m_nRelationship(relationship)
{
	m_MinorRaceKO = CPoint(-1,-1);	
}

CDiplomacyIntelObj::CDiplomacyIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &minorRaceKO)
	: CIntelObject(sOwnerID, sEnemyID, round, isSpy, 3), m_MinorRaceKO(minorRaceKO)
{
	m_sMajor = "";
	m_nAgreement = NULL;
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
		ar >> m_nAgreement;
		ar >> m_nDuration;
		ar >> m_nRelationship;		
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion generiert einen Text, welcher eine Geheimdiestaktion beschreibt, für den Auslöser bzw. das Opfer
/// dieser Aktion.
void CDiplomacyIntelObj::CreateText(CBotf2Doc* pDoc, BYTE n, const CString& param)
{
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName;
	if (this->GetIsSpy())
		fileName = *((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Races\\MajorIntelSpyOff.data";			// Name des zu Öffnenden Files 
	else
		fileName = *((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Races\\MajorIntelSabOff.data";		// Name des zu Öffnenden Files 
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
								s = pDoc->GetRaceCtrl()->GetMinorRace(pDoc->m_Sector[m_MinorRaceKO.x][m_MinorRaceKO.y].GetName())->GetRaceName();
								csInput.Replace("$minor$", s);
							}
							switch (m_nAgreement)
							{
							case DEFENCE_PACT:			{s = CResourceManager::GetString("DEFENCE_PACT_WITH_ARTICLE"); break;}
							case WAR:					{s = CResourceManager::GetString("WAR_WITH_ARTICLE"); break;}
							case NO_AGREEMENT:			{s = CResourceManager::GetString("NO_AGREEMENT"); break;}
							case NON_AGGRESSION_PACT:	{s = CResourceManager::GetString("NON_AGGRESSION_WITH_ARTICLE"); break;}
							case TRADE_AGREEMENT:		{s = CResourceManager::GetString("TRADE_AGREEMENT_WITH_ARTICLE"); break;}
							case FRIENDSHIP_AGREEMENT:	{s = CResourceManager::GetString("FRIENDSHIP_WITH_ARTICLE"); break;}
							case COOPERATION:			{s = CResourceManager::GetString("COOPERATION_WITH_ARTICLE"); break;}
							case AFFILIATION:			{s = CResourceManager::GetString("AFFILIATION_WITH_ARTICLE"); break;}
							case MEMBERSHIP:			{s = CResourceManager::GetString("MEMBERSHIP_WITH_ARTICLE"); break;}
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
								
								if (m_nAgreement == NO_AGREEMENT || m_nAgreement == WAR)
									csInput.Replace("($duration$) ", "");
								else
								{
									if (m_nDuration == NULL)
										csInput.Replace("$duration$", CResourceManager::GetString("UNLIMITED"));
									else
									{
										s.Format("%d %s", m_nDuration, CResourceManager::GetString("ROUNDS"));
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
								s = CResourceManager::GetString("FEMALE_ARTICLE")+" " + pDoc->GetRaceCtrl()->GetMinorRace(pDoc->m_Sector[m_MinorRaceKO.x][m_MinorRaceKO.y].GetName())->GetRaceName();
							csInput.Replace("$major$", s);
							
							if (m_nRelationship < 5) s = CResourceManager::GetString("HATEFUL");
							else if (m_nRelationship < 15) s = CResourceManager::GetString("FURIOUS");
							else if (m_nRelationship < 25) s = CResourceManager::GetString("HOSTILE");
							else if (m_nRelationship < 35) s = CResourceManager::GetString("ANGRY");
							else if (m_nRelationship < 45) s = CResourceManager::GetString("NOT_COOPERATIVE");
							else if (m_nRelationship < 55) s = CResourceManager::GetString("NEUTRAL");
							else if (m_nRelationship < 65) s = CResourceManager::GetString("COOPERATIVE");
							else if (m_nRelationship < 75) s = CResourceManager::GetString("FRIENDLY");
							else if (m_nRelationship < 85) s = CResourceManager::GetString("OPTIMISTIC");
							else if (m_nRelationship < 95) s = CResourceManager::GetString("ENTHUSED");
							else s = CResourceManager::GetString("DEVOTED");
							csInput.Replace("$relation$", s);

							if (m_MinorRaceKO != CPoint(-1,-1))
							{
								s = pDoc->GetRaceCtrl()->GetMinorRace(pDoc->m_Sector[m_MinorRaceKO.x][m_MinorRaceKO.y].GetName())->GetRaceName();
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
		fileName = *((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Races\\MajorIntelSabDef.data";			// Name des zu Öffnenden Files 
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
								s = pDoc->GetRaceCtrl()->GetMinorRace(pDoc->m_Sector[m_MinorRaceKO.x][m_MinorRaceKO.y].GetName())->GetRaceName();
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
									csInput = CResourceManager::GetString("KNOW_RESPONSIBLE_SABOTAGERACE", FALSE, s);
								}
							}
							else
								csInput = CResourceManager::GetString("DO_NOT_KNOW_RESPONSIBLE_RACE");
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