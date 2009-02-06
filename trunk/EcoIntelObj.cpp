#include "stdafx.h"
#include "EcoIntelObj.h"
#include "Botf2Doc.h"

IMPLEMENT_SERIAL (CEcoIntelObj, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CEcoIntelObj::CEcoIntelObj(void)
{
	m_byType = 0;
	m_KO = CPoint(-1,-1);
}

CEcoIntelObj::CEcoIntelObj(BYTE owner, BYTE enemy, USHORT round, BOOLEAN isSpy, const CPoint &ko, USHORT id, BYTE number)
	: CIntelObject(owner, enemy, round, isSpy, 0), m_KO(ko), m_nID(id), m_byNumber(number)
{
	m_iLatinum = NULL;
}

CEcoIntelObj::CEcoIntelObj(BYTE owner, BYTE enemy, USHORT round, BOOLEAN isSpy, int latinum)
	: CIntelObject(owner, enemy, round, isSpy, 0), m_iLatinum(latinum)
{
	m_KO = CPoint(-1,-1);
	m_nID = 0;
	m_byNumber = 0;
}

CEcoIntelObj::~CEcoIntelObj(void)
{
}

/// Kopierkonstruktor
CEcoIntelObj::CEcoIntelObj(const CEcoIntelObj & rhs)
{
	m_byOwner = rhs.m_byOwner;
	m_byEnemy = rhs.m_byEnemy;
	m_nRound = rhs.m_nRound;
	m_bIsSpy = rhs.m_bIsSpy;
	m_byType = rhs.m_byType;
	m_strEnemyDesc = rhs.m_strEnemyDesc;
	m_strOwnerDesc = rhs.m_strOwnerDesc;

	m_KO = rhs.m_KO;
	m_nID = rhs.m_nID;
	m_byNumber = rhs.m_byNumber;
	m_iLatinum = rhs.m_iLatinum;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CEcoIntelObj::Serialize(CArchive &ar)
{
	__super::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_KO;
		ar << m_nID;
		ar << m_byNumber;
		ar << m_iLatinum;
	}
	else if (ar.IsLoading())
	{
		ar >> m_KO;
		ar >> m_nID;
		ar >> m_byNumber;
		ar >> m_iLatinum;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion generiert einen Text, welcher eine Geheimdiestaktion beschreibt, für den Auslöser bzw. das Opfer
/// dieser Aktion.
void CEcoIntelObj::CreateText(CBotf2Doc* pDoc, BYTE n, BYTE param)
{
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName;
	if (this->GetIsSpy())
		fileName = *((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Races\\MajorIntelSpyOff.data";		// Name des zu Öffnenden Files 
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
			if (atoi(s) == m_byOwner)
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
						// in csInput steht nun die Beschreibung für den Aggressor
						// Jetzt müssen noch die Variablen mit dem richtigen Text gefüllt werden
						switch (m_byEnemy)
						{
						case HUMAN:		{s = CResourceManager::GetString("ARTICLE_RACE1_EMPIRE"); break;}
						case FERENGI:	{s = CResourceManager::GetString("ARTICLE_RACE2_EMPIRE"); break;}
						case KLINGON:	{s = CResourceManager::GetString("ARTICLE_RACE3_EMPIRE"); break;}
						case ROMULAN:	{s = CResourceManager::GetString("ARTICLE_RACE4_EMPIRE"); break;}
						case CARDASSIAN:{s = CResourceManager::GetString("ARTICLE_RACE5_EMPIRE"); break;}
						case DOMINION:	{s = CResourceManager::GetString("ARTICLE_RACE6_EMPIRE"); break;}
						}
						csInput.Replace("$race$", s);
						if (m_KO != CPoint(-1,-1))
						{
							s = pDoc->m_Sector[m_KO.x][m_KO.y].GetName();
							csInput.Replace("$system$", s);
						}
						if (m_nID != NULL)
						{
							s = pDoc->GetBuildingName(m_nID);
							csInput.Replace("$building$", s);
						}
						if (m_byNumber != NULL)
						{
							s.Format("%d", m_byNumber);
							csInput.Replace("$number$", s);
						}
						s.Format("%d", m_iLatinum);
						csInput.Replace("$latinum$", s);
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
				if (atoi(s) == m_byEnemy)
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
							if (m_KO != CPoint(-1,-1))
							{
								s = pDoc->m_Sector[m_KO.x][m_KO.y].GetName();
								csInput.Replace("$system$", s);
							}
							if (m_nID != NULL)
							{
								s = pDoc->GetBuildingName(m_nID);
								csInput.Replace("$building$", s);
							}
							if (m_byNumber != NULL)
							{
								s.Format("%d", m_byNumber);
								csInput.Replace("$number$", s);
							}
							s.Format("%d", m_iLatinum);
							csInput.Replace("$latinum$", s);
							m_strEnemyDesc = csInput;
							if (param != NOBODY)
							{
								switch (param)
								{
								case HUMAN:		{s = CResourceManager::GetString("ARTICLE_RACE1_EMPIRE"); break;}
								case FERENGI:	{s = CResourceManager::GetString("ARTICLE_RACE2_EMPIRE"); break;}
								case KLINGON:	{s = CResourceManager::GetString("ARTICLE_RACE3_EMPIRE"); break;}
								case ROMULAN:	{s = CResourceManager::GetString("ARTICLE_RACE4_EMPIRE"); break;}
								case CARDASSIAN:{s = CResourceManager::GetString("ARTICLE_RACE5_EMPIRE"); break;}
								case DOMINION:	{s = CResourceManager::GetString("ARTICLE_RACE6_EMPIRE"); break;}
								}
								csInput = CResourceManager::GetString("KNOW_RESPONSIBLE_SABOTAGERACE", FALSE, s);
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